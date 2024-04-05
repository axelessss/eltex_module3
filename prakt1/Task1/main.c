#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void print_sucsess1();
void print_sucsess2();

int main(int argc, char *argv[])
{
    pid_t pid;
    int rv;

    switch(pid = fork()) 
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            printf("CHILD: PID - %d\n", getpid());
            printf("CHILD: PPID - %d\n", getppid());
            for(int i = 1; i < argc; i++)
                printf("Argument %d: %s\n", i, argv[i]);
            exit(EXIT_SUCCESS);

        default:
            printf("PARENT: PID - %d\n", getpid());
            printf("PARENT: CHILD PID - %d\n", pid);
            for(int i = 1; i < argc; i++)
                printf("Argument %d: %s\n", i, argv[i]);
            wait(&rv);
            printf("PARENT: RETURN STATUS FOR CHILD- %d\n", WEXITSTATUS(rv));
    }
    
    atexit(print_sucsess1);
    atexit(print_sucsess2);
    exit(EXIT_SUCCESS);
}

void print_sucsess1()
{
    printf("sucsessfully\n");
}

void print_sucsess2()
{
    printf("Program finished ");
}