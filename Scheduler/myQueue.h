#include <stdio.h>
#include <stdlib.h>
#include "struct_helper.h"
#include <sys/mman.h>
#include <unistd.h>
#include <sys/time.h>
// To get current Time
struct tm *getTime()
{
    time_t *t = malloc(sizeof(time_t));
    time(t);
    return localtime(t);
}

// To get Time difference
double getDiff(struct tm *start)
{
    // printf("I am in getDiff\n");
    return difftime(mktime(getTime()), mktime(start));
}
void printQueue(struct Queue *readyQueue){
    printf("\n########################\n");
    for(int i=0;i<4;i++){
        struct Node *curr=readyQueue[i].head;
        while(curr!=NULL){
            printf("pid: %d ---> ",curr->process->pid);
            curr=curr->next;
        }
        printf("\n");
    }
    printf("########################\n");
}
// adding process to queue
void addProcess(struct Queue *queue, struct Process *process, struct Node *node,int cnt_n,int cnt_p)
{

    // Allocating node
 //  printf("getPid: %d\n",getpid());
   //printf("n: %d,p: %d \n",cnt_n,cnt_p);
   //printf("Node addr: %p,process addr: %p,pid: %d,priority: %d\n",node,process,process->pid,process->priority);
    node->process = process;
    node->add_time = *getTime();
    node->next = NULL;

    // Case 1: if Queue is Empty...Adding first Node
    if (queue->head == NULL)
    {

        queue->head = node;
        queue->tail = node;

        return;
    }

    // Case 2: if Queue is not empty
    queue->tail->next = node;
    queue->tail = node;
}

// To remove a process from Queue
void pop(struct Queue *queue)
{
    if (queue->head == queue->tail)
    {
        queue->head = NULL;
        queue->tail = NULL;
        return;
    }
    
    queue->head = queue->head->next;
}

// To get process from queue and remove from it.
struct Process *getProcess(struct Queue *queues)
{
    // Checking for  a process with higher priority first
    for (int i = 3; i >= 0; i--)
    {
        if (queues[i].head != NULL)
        {

            struct Process *process = queues[i].head->process;
            pop(&queues[i]);
            return process;
        }
    }
    // If there is no process in queue
    return NULL;
}

// To prevent Starvation if a process wait time >factor ,move it to higher level priority queue
void pushUpwards(struct Queue *queues, double factor)
{
    // checking for the nodes of priority 1 to 3 if their waiting time is greater than factor
    //  printf("Inside pushUpwards\n");
    for (int i = 2; i >= 0; i--)
    {
        // printf("%d\n",i);
        if (queues[i].head != NULL)
        {
            // printf("%d is not NULL\n",i);
            struct Node *curr = queues[i].head;
            while (curr != NULL && getDiff(&curr->add_time) > factor * (3 - i))
            {
                // printf("%d\n",i);
                struct Node *tmp = curr->next;
                pop(&queues[i]);
                addProcess(&queues[i + 1], curr->process, curr,-1,-1);
                // printf("%d",curr->process->pid);
                curr = tmp;
            }
        }
    }
}

// To create a Process
struct Process *createProcess(pid_t pid, int priority, struct Process *process, int idx, struct tm *start, struct tm *initial)
{

    // struct Process *process = (struct Process *)mmap(NULL, sizeof(struct Process), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    process->pid = pid;
    process->priority = priority;
    process->exec_time = -1;
    process->wait_time = -1;
    process->created_time = getDiff(initial);
    process->start = *start;
    process->idx = idx;
    printf("%d, process addr: %p,pid: %d,priority: %d\n",getpid(),process,pid,priority);
    return process;
}