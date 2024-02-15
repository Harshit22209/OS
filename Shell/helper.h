#include<stdio.h>
//To print in green Color
void green(char *s){
    printf("\033[1;32m%s\033[1;0m",s);
}

//To print in Blue Color
void blue(char *s){
    printf("\033[1;34m%s\033[1;0m",s);
}

//To remove \n
void removeNext(char *data){
    int i=0;
    while(data[i]!='\n' && data[i]!='\0'){
        i++;
    }
    data[i]='\0';

} 

// To get the length of arrays of string
int getLength(char **strs){
    int cnt=0;
    while(*strs!=NULL){
        strs++;
        
        //printf("%d",cnt);
        cnt++;
    }
    return cnt;
}
// to print the array of strings
void PrintLines(char **strs){
    int cnt=0;
    while(*strs!=NULL){
        printf("%s",*strs);
        strs++;
    }
}
// To get the string splitted based on param
char **split(char *s, char *param)
{

    char **result = (char **)malloc(10 * sizeof(char *));
    int count1 = 0;

    char *data = strtok(s, param);
    while (data != NULL && count1 < 10)
    {
        
        result[count1++] = data;
        data = strtok(NULL, param);
    }
    result[count1] = NULL;
    return result;
}
char *readInput()
{
    char *cmd = (char *)malloc(100 * sizeof(char));
    fgets(cmd, 100, stdin);
    removeNext(cmd);
    return cmd;
}