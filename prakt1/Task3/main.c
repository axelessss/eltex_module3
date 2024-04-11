#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 100

int main(int argc, char *argv[])
{
    pid_t pid;
    int rv;
    int count_args;
    char args_in[N];
    char *arg;
    char args[N][N];
    char **args_out = (char **)malloc(N*sizeof(char*));

    while(true)
    {
        count_args = 0;
        printf(">> ");

        gets(args_in);

        if(strcmp(args_in, "exit") == 0)
            break;

        arg = strtok(args_in, " ");

        while(arg != NULL)
        {
            strcpy(args[count_args], arg);
            arg = strtok(NULL, " ");
            count_args++;
        }
    
        for(int i = 0; i < count_args; i++)
        {
            args_out[i] = args[i];
        }
        args_out[count_args] = NULL;

        switch(pid = fork()) 
        {
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0:
                printf("This is child process\n");
                execv(args_out[0], args_out);
                exit(EXIT_SUCCESS);

            default:
                printf("This is parent process\n");
                wait(&rv);
                printf("\nPARENT: RETURN STATUS FOR CHILD- %d\n", WEXITSTATUS(rv));
        }  
    }
    free(args_out);
    exit(EXIT_SUCCESS);
}