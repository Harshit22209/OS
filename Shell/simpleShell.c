#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "commands.h"
#include <time.h>
// #include "Colors.h"
// stores the userName
char user[50];

// stores the current path
char curr_path[200] = "./";
int create_process_and_run(char **command, char *inpt);
int launch(char **command, char *inpt);
void pipeHandler(char **cmds, char *inpt);
// stores the history
char *history[200];

// count the total cmds;
int totalCmds = 0;
// count total processes
int totalProcesses = 0;

// stores the path till user directory
char *userRoot;

// stores the process Details
struct myPCB
{
    pid_t pid;
    // long duration;
    struct tm start;
    
    char *cmd;
    double duration;
};

struct myPCB *processes[200];
// store all the processes
// struct myPCB *processes[200];


int cntHandler = 0;
int iniPid = 0;
// to get current local time
struct tm *getTime()
{
    time_t *t = malloc(sizeof(time_t));
    time(t);
    return localtime(t);
}

// to handle cntrl + C
static void my_handler(int signum)
{

    printf("\n%d %d %d\n", totalProcesses, getpid(), iniPid);
    if (getpid() == iniPid)
    {
        for (int i = 0; i < totalProcesses; i++)
        {
            
            struct myPCB *process = processes[i];

            printf("pid:%d  command:%s  startTime:%02d:%02d:%02d  duration:%.2lf\n",
                   process->pid, process->cmd, process->start.tm_hour, process->start.tm_min, process->start.tm_sec, process->duration);
        }
    }
    exit(0);
}









// To execute C and sh file
void run(char **command)
{
    int status = fork();
    if (status < 0)
    {
        printf("Error in fork\n");
    }
    else if (status == 0)
    {
         //printf("%d", getLength(command));
         if (strstr(command[0], ".sh"))
            {
                if(execlp("/bin/sh", "sh", command[0], (char *)NULL)==-1){
                    perror("Error");
                }
            }
            else
            {
                if(execv(command[0], command)==-1){
                    perror("Error");
                }
            }
            exit(0);
    }
    else
    {
        int l = getLength(command);
        // To check for background processes
        if (strcmp(command[l - 1], "&"))
        {
            int ret;
            int pid = wait(&ret);
            if (WIFEXITED(ret))
            {
                printf("1. %d Exit =%d\n", pid, WEXITSTATUS(ret));
            }
            else
            {
                printf("Abnormal termination of %d\n", pid);
            }
        }
    }
}
/// @brief
/// @param command
/// @param inpt
/// @return
int create_process_and_run(char **command, char *inpt)
{
    // cmd stores the main command like for " mkdir xyz " it stores mkdir
    char *cmd = command[0];
    // printf("%d",getLength(command));
    // removeNext(cmd);
    strcpy(curr_path, pwd_cmd());
    struct myPCB *process = (struct myPCB *)malloc(sizeof(struct myPCB));

    process->start = *getTime();

    process->cmd = inpt;
    processes[totalProcesses++] = process;
    int status = fork();
    if (status < 0)
    {
        printf("Error in fork\n");
    }
    else if (status == 0)
    {

        // checking which command is to be executed
        if (!strcmp("grep", cmd))
        {
            PrintLines(grep_cmd(command + 1, curr_path));
        }
        else if (!strcmp("wc", cmd))
        {
            wc_cmd(command + 1, curr_path);
        }
        else if (!strcmp("sort", cmd))
        {
            PrintLines(sort_cmd(command + 1, curr_path));
        }

        else if (!strcmp("cat", cmd))
        {
            // Abhay
            PrintLines(cat_cmd(command + 1, curr_path));
        }

        else if (!strcmp("uniq", cmd))
        {
            PrintLines(unique_cmd(command + 1, curr_path));
        }
        else if (!strcmp("ls", cmd))
        {
            ls_cmd(command + 1, cmd);
        }
        else
        {

           run(command);
        }
        // process->duration = difftime(mktime(getTime()), mktime(process->start));
        exit(0);
    }
    else
    {

        int ret;
        int pid = wait(&ret);

        if (WIFEXITED(ret))
        {
            process->pid = pid;
            process->duration = difftime(mktime(getTime()), mktime(&process->start));
            printf("%d Exit =%d\n", pid, WEXITSTATUS(ret));
        }
        else
        {
            printf("Abnormal termination of %d\n", pid);
        }
    }

    return 1;
}

// To handle pipes
void pipeHandler(char **cmds, char *inpt)
{
    int status = fork();
    if (status < 0)
    {
        printf("Error in fork\n");
    }
    else if (status == 0)
    {

        int n = getLength(cmds);
        char **output;
        // get process details
        struct myPCB *process = (struct myPCB *)malloc(sizeof(struct myPCB));
        process->start = *getTime();
        process->pid = getpid();
        process->cmd = inpt;
        processes[totalProcesses++] = process;
        int flag=0;
        // loops through all the commands under pipe
        for (int i = 0; i < n; i++)
        {

            char **command = split(cmds[i], " ");
            char *cmd = command[0];
            if (!strcmp(cmd, "cat"))
            {
                output = cat_cmd(command + 1, curr_path);
            }
            else if (!strcmp(cmd, "grep"))
            {
                if (i == 0)
                {
                    output = grep_cmd(command + 1, curr_path);
                }
                else
                {
                    output = grep_pipe(command[1], output);
                }
            }
            else if (!strcmp(cmd, "sort"))
            {
                if (i == 0)
                {
                    output = sort_cmd(command + 1, curr_path);
                }
                else
                {
                    output = sort_pipe(output);
                }
            }
            else if (!strcmp(cmd, "uniq"))
            {
                if (i == 0)
                {
                    output = unique_cmd(command + 1, curr_path);
                }
                else
                {
                    output = sort_pipe(output);
                }
            }
            else if (!strcmp(cmd, "wc"))
            {
                printf("%d\n", wc_pipe(command[1], output));
                flag=1;
            }

        }
        if(!flag)
            PrintLines(output);
        process->duration = difftime(mktime(getTime()), mktime(&process->start));
        exit(0);
    }
    else
    {

        char *cmd = cmds[getLength(cmds) - 1];
        char **tmp = split(cmd, " ");
        if (strcmp(tmp[getLength(tmp) - 1], "&"))
        {
            int ret;
            int pid = wait(&ret);
            if (WIFEXITED(ret))
            {
                printf("1. %d Exit =%d\n", pid, WEXITSTATUS(ret));
            }
            else
            {
                printf("Abnormal termination of %d\n", pid);
            }
        }
    }
}

int launch(char **command, char *cmd)
{
    int status;
    char *base_cmd = command[0];

    // checking which command is to be executed
    if (!strcmp("cd", base_cmd))
    {
        // printf("I am in cd");
        cd_cmd(command + 1, curr_path);
    }
    else if (!strcmp("mkdir", cmd))
    {
        mkdir_cmd(command + 1, curr_path);
    }
    else if (!strcmp("rmdir", cmd))
    {
        rmdir_cmd(command + 1, curr_path);
    }
    else if (!strcmp("echo", cmd))
    {

        echo_cmd(command + 1, curr_path);
    }
    else if (!strcmp("pwd", cmd))
    {

        printf("%s \n", pwd_cmd());
    }
    else if (!strcmp("history", cmd))
    {
        for (int i = 0; i < totalCmds; i++)
        {
            printf("%s\n", history[i]);
        }
    }
    else
    {

        status = create_process_and_run(command, cmd);
        return status;
    }
    return 1;
}

// To get the relative path with respect to my custom user
char *getPath()
{
    int i = 0;
    char *path = pwd_cmd();
    while (userRoot[i] == path[i])
    {
        i++;
    }
    // printf("%s %s %d \n",path,userRoot,i);
    return (path + i);
}

// Intiating a shell
void shell(char *rootDir)
{
    int status;
    do
    {
        char root[50] = "root@";
        strcat(root, user);
        green(root);
        // harshit->abc/xyz /abc
        printf(":");
        blue("~");
        if (strcmp(pwd_cmd(), userRoot))
            blue(getPath());
        printf("$ ");
        char *cmd = readInput();
        history[totalCmds] =(char*)malloc(sizeof(char)*100);
        strcpy(history[totalCmds++],cmd);
        //printf("%s\n",history[totalCmds-1]);
        char **pipes = split(cmd, "|");
        if (getLength(pipes) > 1)
        {
            pipeHandler(pipes, cmd);
            continue;
        }

        char **cmds = split(cmd, " ");
        status = launch(cmds, cmd);

    } while (status);
}

int main()
{
    // Asking for user Details
    blue("Enter User name: ");
    fgets(user, 50, stdin);

    // Removing \n
    removeNext(user);
    int shared_id;

    char rootDir[100] = "./";
    strcat(rootDir, user);
    struct stat st;
    // checking if already a directory exist for the entered username.
    int isExist = stat(rootDir, &st);
    iniPid = getpid();
    signal(SIGINT, my_handler);
    if (isExist != -1 || mkdir(rootDir, 0777) == 0)
    {

        printf("Successfuly Created the dir\n");
        printf("Welcome %s to my shell environment!!!\n", user);

        strcat(curr_path, user);
        if (chdir(curr_path) == -1)
        {
            perror("Error in login");
        }
        userRoot = pwd_cmd();
        shell(rootDir);
    }
    else
    {
        printf("Error");
    }
}