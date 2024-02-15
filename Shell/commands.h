#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "helper.h"

char *pwd_cmd()
{
    char *path = malloc(sizeof(char) * 200);
    if (getcwd(path, 200) != NULL)
    {
        return path;
    }
    else
        return "NULL";
}

void cd_cmd(char **command, char *curr_path)
{
    
    strcat(curr_path, command[0]);
    removeNext(command[0]);
   if(chdir(command[0])==-1){
    perror("No such directory exist");
   };
}

int ls_cmd(char **command, char *cmd)
{
    if(execlp("ls", "ls", command[0], NULL)==-1){
        perror("error\n");
    }
    return 1;
    
}
void mkdir_cmd(char **command, char *curr_path)
{
    char dir[200];
    
    // strcpy(dir, curr_path);
    // strcat(dir, "/");
    // removeNext(command[0]);
    // // printf("%s", command[0]);
    // strcat(dir, command[0]);
    if (mkdir(command[0], 0777) == 0)
    {
        printf("Successfully created %s\n", command[0]);
    }
    else
    {
        perror("Error in creating directory\n");
    }
}
void rmdir_cmd(char **command, char *curr_path)
{
    // char dir[200];
    // strcpy(dir, curr_path);
    // strcat(dir, "/");
    // removeNext(command[0]);
    // // printf("%s", command[0]);
    // strcat(dir, command[0]);
    if (rmdir(command[0]) == 0)
    {
        printf("Successfully Deleted %s\n", command[0]);
    }
    else
    {
        perror("Error\n");
    }
}
void echo_cmd(char **command, char *curr_path)
{
    //Looping through entire string and printing
    while (*command != NULL)
    {
        printf("%s ", command[0]);
        command++;
    }
    printf("\n");
}

char ** cat_cmd(char **command, char *curr_path)
{
    char **lines=(char**)malloc(200*sizeof(char*));
    int cnt=0;
    //looping through all the files
    for (int i = 0; command[i] != NULL; i++)
    {
        FILE *file = fopen(command[i], "r");
        if(file==NULL){
         perror("Error in opening file");
         exit(1);
    }
        char line[100];
        //reading each line of the file
        while (fgets(line, 100, file) != NULL)
        {    
            lines[cnt]=(char*)malloc(sizeof(char)*100);
            strcpy(lines[cnt++], line);
        }
        fclose(file);
    }
    lines[cnt]==NULL;
    return lines;
}

char** grep_cmd(char **command, char *curr_path)
{   
    char **lines=(char**)malloc(200*sizeof(char*));
    int cnt=0;
    //looping through all the files
    for (int i = 1; command[i] != NULL; i++)
    {
        FILE *file = fopen(command[1], "r");
        if(file==NULL){
        perror("Error in opening file");
        exit(1);
    }
    //reading Each line of a file
        char line[100];
        while (fgets(line, 100, file) != NULL)
        {
            //Chacking if the line contains the pattern
            if (strstr(line, command[0]))
            {
               lines[cnt]=(char*)malloc(sizeof(char)*100);
                strcpy(lines[cnt++], line);
            }
        }
        fclose(file);
    }
    lines[cnt]=NULL;
    return lines;
}

void wc_cmd(char **command, char *curr_path)
{
    FILE *file = fopen(command[1], "r");
    if(file==NULL){
        perror("Error in opening file");
        exit(1);
    }
    char line[100];
    int cnt_l = 0;
    int cnt_c = 0;

    while (fgets(line, 100, file) != NULL)
    {
        for (int i = 0; line[i] != '\n'; i++)
        {

            cnt_c++;
        }
        cnt_l++;
    }
    if (strcmp(command[0], "-l") == 0)
    {
        printf("%d\n", cnt_l);
    }
    else if (strcmp(command[0], "-c") == 0)
    {
        printf("%d\n", cnt_c);
    }
    fclose(file);
}

int mySortFunc(const void *str1, const void *str2)
{
    return strcmp(*(char **)str1, *(char **)str2);
}

char** sort_cmd(char **command, char *curr_path)
{
    FILE *file = fopen(command[0], "r");
    if(file==NULL){
        perror("Error in opening file");
        exit(1);
    }
    char line[100];
    char **lines=(char**)malloc(200*sizeof(char*));
    int cnt = 0;
    
    while (fgets(line, 100, file) != NULL)
    {
        lines[cnt]=(char*)malloc(sizeof(char)*100);
        strcpy(lines[cnt++], line);
        // printf("%s",lines[cnt-1]);
    }
    fclose(file);
    qsort(lines, cnt, sizeof(char *), mySortFunc);
    lines[cnt] = NULL;
    return lines;
}
char** unique_cmd(char **command, char *cmd)
{
    FILE *file = fopen(command[0], "r");
    if(file==NULL){
        perror("Error in opening file");
        exit(1);
    }
    char line[100];
    char prev[100];
    char **lines=(char**)malloc(200*sizeof(char*));
    fgets(prev, 100, file);
    lines[0]=(char*)malloc(sizeof(char)*100);
    strcpy(lines[0], prev);
    int cnt=1;
    // printf("%s", prev);


    while (fgets(line, 100, file) != NULL)
    {

        if (strcmp(prev, line) == 0)
        {
            continue;
        }
        else
        {
            strcpy(prev, line);
            lines[cnt]=(char*)malloc(sizeof(char)*100);
            strcpy(lines[cnt++], prev);
        }
    }
    fclose(file);

    lines[cnt]=NULL;
    return lines;
}


// FUNCTIONS FOR PIPE-Made this functions to handle pipe
char **grep_pipe(char *cmd, char **input)
{
    char **lines=(char**)malloc(200*sizeof(char*));
    int cnt = 0;
    for (int i = 0; input[i] != NULL; i++)
    {
        if (strstr(input[i], cmd))
        {
            lines[cnt++] = input[i];
        }
    }
    lines[cnt] = NULL;
    return lines;
}
char** sort_pipe(char **input)
{
    //FILE *file = fopen(command[0], "r");
    
    char **lines=(char**)malloc(200*sizeof(char*));
    int cnt = 0;
    
     for (int i = 0; input[i] != NULL; i++)
    {
        
        lines[cnt]=(char*)malloc(sizeof(char)*100);
        strcpy(lines[cnt++], input[i]);
        // printf("%s",lines[cnt-1]);
    }
    
    qsort(lines, cnt, sizeof(char *), mySortFunc);
    lines[cnt] = NULL;
    return lines;
}
char** uniq_pipe(char **input)
{
    //FILE *file = fopen(command[0], "r");
    char line[100];
    char prev[100];
    //char **lines=(char**)malloc(200*sizeof(char*));
    //fgets(prev, 100, file);
    char **lines=(char**)malloc(200*sizeof(char*));
    strcpy(prev,input[0]);
    strcpy(lines[0],prev);
    int cnt = 1;
    
     for (int i = 1; input[i] != NULL; i++)
    {
        if(strcmp(prev,line)){
        
            strcpy(prev, line);
            lines[cnt]=(char*)malloc(sizeof(char)*100);
            strcpy(lines[cnt++], line);
        }
        
    }
    lines[cnt]=NULL;
    return lines;
}

int wc_pipe(char *cmd, char **input)
{

    int cnt_l = 0;
    int cnt_c = 0;

    for (int i = 0; input[i] != NULL; i++)
    {
        char *line = input[i];
        for (int i = 0; line[i] != '\n'; i++)
            cnt_c++;
        cnt_l++;
    }
    if (strcmp(cmd, "-l") == 0)
    {
        return cnt_l;
    }
    else if (strcmp(cmd, "-c") == 0)
    {
        return cnt_c;
    }
}
