#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
struct Process{
    pid_t pid;
    int priority;
    double exec_time;
    double wait_time;
    struct tm start;
    double created_time;
    int idx;

};

struct Node{
    struct Process *process;
    struct Node *next;
    struct tm add_time; 
};

struct Queue{
    struct Node *head;
    struct Node *tail;
    int cnt;
    
};