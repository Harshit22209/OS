                // Radhe Radhe
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>
#include "myQueue.h"
#include "helper.h"
#include <semaphore.h>
#include <sys/shm.h>


// No of Cpu's
int NCPU;   

//Time Quantum
int TSLICE; 

//Ready Queue for scheduler
struct Queue *readyQueue;

//Contains the nodes for queue 
//As the memory need to be shared ,we created few nodes in main using mmap
struct Node *newNodes;

//Contains Process Address
//reason is same as for new Nodes
struct Process *newProcesses;

//mutex for semaphore
sem_t mutex;
sem_t inp_mutex;
pid_t *s_pid;
pid_t mpid;
//for getting the process idx from the process created as buffer
int *cnt_p;

//For nodes similar to processes
int *cnt_n;

struct tm initial;

void my_handler(int sig){
    //printf("I am in my_handler: %d %d\n",getpid(),*s_pid);
    if(mpid==getpid() || *s_pid==getpid()){
         //printf("I am in my_handler2: %d\n",getpid());
         double avg_wait=0;
         double avg_exec=0;
        for(int i=0;i<*cnt_p;i++){
            if(i==0){
                green("\n---------------------------\n");
            }
            struct Process *process=&newProcesses[i];
             printf("pid: %d created:%.2lf wait:%.2lf execTime:%.2lf\n",process->pid,process->created_time,process->wait_time,process->exec_time);
             avg_wait+=process->wait_time;
            
             avg_exec+=process->exec_time;
             if(i==*cnt_p-1){
                green("---------------------------\n");
                printf("Average Execution time: %.2lf\n",(avg_exec/(*cnt_p)));
                printf("Average Wait time: %.2lf\n",(avg_wait/(*cnt_p)));
            }
        }
        if(*s_pid==getpid()){
        munmap(newProcesses,100);
        munmap(newNodes,100);
        munmap(readyQueue,sizeof(struct Queue)*4);
        
        //kill(*s_pid,SIGINT);
        }
        sem_destroy(&mutex);
    }
    exit(0);
}

// for scheduling the execution of process
void scheduler()
{
    //help in calculating factor to minimize starvation
    int sumProcess=0;
    int count=0;

    //Contains all the processes runnung
    struct Process *running_processes[100];
    while (1)
    {
        

        int no_runningP = 0;

       printQueue(readyQueue);
        
        for(int i=0;i<NCPU;i++)
        {
            
            sem_wait(&mutex);
            struct Process *process = getProcess(readyQueue);
           
            sem_post(&mutex);

            //if there is no process in queue
            if (process == NULL)
            {
                printf("No process available\n");
                if(readyQueue[0].head!=NULL || readyQueue[1].head!=NULL||readyQueue[2].head!=NULL||readyQueue[3].head!=NULL)
                    printf("Process left\n");
                break;
            }
            
            
            running_processes[no_runningP++] = process;

            //if process had first time arrived for execution
            if (process->wait_time == -1)
            {
                process->wait_time = getDiff(&process->start);
            }

            // sending signal for Continue running the process
            int status = kill(process->pid, SIGCONT);
            
           
        }

       //waiting for tslice milliseconds
        usleep(1000 * TSLICE);

        //pushing back the processes to ready queue for context switching
        for (int i = 0; i < no_runningP; i++)
        {

           
            struct Process *process = running_processes[i];
           
            int status = kill(process->pid, SIGSTOP);

            // checking if the process has done it job or not
            //if status ==-1 then it means that the process is terminated
            
            if (status == -1){
                process->exec_time=getDiff(&process->start);
                printf("pid: %d wait:%.2lf execTime:%.2lf\n",process->pid,process->wait_time,getDiff(&process->start));
                readyQueue[process->priority-1].cnt--;
                continue;
            }
           
            //adding process back to the queue           
            sem_wait(&mutex);
            addProcess(&readyQueue[process->priority - 1], process, &newNodes[process->idx],*cnt_n,*cnt_p);
            sem_post(&mutex);

            
        }

        //To push the process to higher priority queue 
       if(no_runningP==NCPU){

           //calculate average no of process in queue
           sumProcess+=readyQueue[0].cnt+readyQueue[1].cnt+readyQueue[2].cnt+readyQueue[3].cnt;
           //printf("cnt: %d\n",sumProcess);
           count++;
           double avg=(double)sumProcess/count;

           //calculating factor 
           double factor=(avg*TSLICE)/(1000*NCPU);
           printf("factor: %.2lf",factor);
           //calling the function to handle movement of process from one queue to another
           //sem_wait(&mutex);
           pushUpwards(readyQueue,factor);
           //sem_post(&mutex);
          // printf("exited..");
       }
        
        no_runningP = 0;
        
    }
    
}

//To execute file in background
void run(char *cmd,int priority)
{
    pid_t child_pid = fork();

    if (child_pid == 0)
    {
        
        execl(cmd, cmd, (char *)NULL);
     
        perror("Error in Executing file");
        exit(1);
    }
    else if (child_pid > 0)
    {
        //Stopping the process from execution    
        kill(child_pid, SIGSTOP);

        //Adding process to Queue
        sem_wait(&mutex);
        addProcess(&readyQueue[priority-1], createProcess(child_pid, priority, &newProcesses[*cnt_p],*cnt_n,getTime(),&initial), &newNodes[*cnt_n],*cnt_n,*cnt_p);
        readyQueue[priority-1].cnt+=1;
       
        *cnt_p += 1;
        *cnt_n += 1;
         printf("cnt_p: %d, cnt_n: %d\n",*cnt_p,*cnt_n);
        //printf("added %d with priority: %d\n",child_pid,priority);
        sem_post(&mutex);


        int status;
        pid_t terminated_pid = wait(&status);

        if (WIFEXITED(status))
        {

            printf("Process:%d terminated with exit status: %d\n",terminated_pid, WEXITSTATUS(status));

        }
   

        
        
    }
    else
    {
        // Handle fork error
        perror("Error in creating childe process\n");
    }
}

//To create scheduler as  a deamon process
void call_scheduler()
{
    pid_t sched_pid = fork();
    if (sched_pid == 0)
    {

        //printf("scheduler %d\n", getpid());
        scheduler();
    }
    else if (sched_pid > 0)
    {
        *s_pid = sched_pid;
        exit(0);
    }
    else
    {
        perror("Error in call_Scheduler");
    }
}

//Helps to run a process 
void helper_fork(char *cmd,int priority){

    pid_t child=fork();
    if(child==0){
        //printf("%d",getpid());
        run(cmd,priority);
        exit(0);
    }
    else if(child>0){
        return;
    }
    else{
        perror("error in helper_fork");
    }
}

//To run shell and scheduler
void shell()
{
    //To create a process to call scheduler and exit.hence creating a deamon process
    pid_t sched_pid = fork();
    int cnt = 0;

    if (sched_pid == 0)
    {
        //printf("call_scheduler %d\n", getpid());
        //calling scheduler
        call_scheduler();

        exit(0);
    }
    else
    {

        while (1)
        {
            sleep(1);
            //usleep(100);
            printf("SimpleShell$ ");
           
            char *cmd;
            //sem_wait(&mutex);
            cmd = readInput();
            //sem_post(&mutex);
            if (!strcmp(cmd, "\n"))
                continue;
            if(cmd==NULL){
                continue;
            }
            char **cmds = split(cmd, " ");
            int length = getLength(cmds);
            if(length==0) continue;
            printf("\n\n Input is: %s\n\n", cmd);
           
            if (!strcmp(cmds[0], "submit"))
            {
                int priority=1;
                if(length>2){
                    priority=atoi(cmds[2]);
                }
               // sem_wait(&mutex);
                helper_fork(cmd+7,priority);
               // sem_post(&mutex);
            }
            else
            {
                printf("Command Not supported");
            }
            // kill(*s_pid,SIGSTOP);
        }
    }
}
int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf(" NCPU and TSLICE missing");
        return 1;
    }

    NCPU = atoi(argv[1]);
    TSLICE = atoi(argv[2]);

   signal(SIGINT,my_handler);
    void *ptr = mmap(NULL, sizeof(struct Queue) * 4 , PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    cnt_p = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    cnt_n = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   
    *cnt_p = 0;
    *cnt_n = 0;
    if (ptr == MAP_FAILED)
    {
        printf("Error||||\n");
    }
    initial=*getTime();
    //allocating shared memory to ready queue ,Processes, Nodes
    newProcesses = (struct Process *)mmap(NULL, sizeof(struct Process) * 100, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    newNodes = (struct Node *)mmap(NULL, sizeof(struct Node) * 100, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    s_pid = (pid_t *)mmap(NULL, sizeof(pid_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    readyQueue = (struct Queue *)ptr;
    sem_init(&mutex, 1, 1);
   
    
     mpid=getpid();

    for (int i = 0; i < 5; i++)
    {
        
        readyQueue[i].head = NULL;
        readyQueue[i].tail = NULL;
        
    }
   
    shell();
    
    return 0;
}
