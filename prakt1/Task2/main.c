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
    int tasks;
    int length;

    if(argc < 2)
    {
        printf("Необходимо указать сторону квадрата\n");
        exit(EXIT_FAILURE);
    }
    
    else if(argc == 2)
    {   
        length = atoi(argv[1]);
        printf("Площадь квадрата: %d\n", length*length);
        exit(EXIT_SUCCESS);
    }

    else
        tasks = (argc-1)/2;


    switch(pid = fork()) 
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            printf("This is child process\n");
            for(int i = 1; i < tasks; i++)
            {
                length = atoi(argv[i]);
                printf("Площадь квадрата %d: %d\n", i, length*length);
            }
            exit(EXIT_SUCCESS);

        default:
        printf("This is parent process\n");
            for(int i = tasks; i < argc; i++)
            {
                length = atoi(argv[i]);
                printf("Площадь квадрата %d: %d\n", i, length*length);
            }
            wait(&rv);
            printf("PARENT: RETURN STATUS FOR CHILD- %d\n", WEXITSTATUS(rv));
    }
    
    exit(EXIT_SUCCESS);
}