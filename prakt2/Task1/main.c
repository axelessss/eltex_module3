#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int p[2];
    char buf[80];
    int rv;
    int count_args;
    int readen;

    if(argc < 2||!atoi(argv[1])||atoi(argv[1]) <= 0)
    {
        printf("Необходимо указать количество чисел(целое число, большее 0)\n");
        exit(EXIT_FAILURE);
    }
    
    if (pipe(p))
    {
        perror("ошибка канала");
        exit(EXIT_FAILURE);
    }

    count_args = atoi(argv[1]);

    switch(pid = fork()) 
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            printf("This is child process\n");
            close(p[0]);

            for(int i = 0; i < count_args; i++)
            {
                sprintf(buf, "%d", rand()%100);
                printf("Send: %s", buf);
                write(p[1], buf, sizeof(int));
            }            
            exit(EXIT_SUCCESS);

        default:
        printf("This is parent process\n");
            close(p[1]);
            for(int j = 0; j < count_args; j++)
            {
                wait(&rv);
                read(p[0], buf, sizeof(buf));
                printf("\nReaden %d: %d", j+1, atoi(buf));
            }
            
            printf("PARENT: RETURN STATUS FOR CHILD- %d\n", WEXITSTATUS(rv));
    }
    exit(EXIT_SUCCESS);
}