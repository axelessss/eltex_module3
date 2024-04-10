#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int p1[2];
    int p2[2];
    char buf[80];
    char buf_change[80];
    char buf_mul[80];
    char buf_recv[80];
    int count_args;
    
    srand(time(NULL));

    if(argc < 2||!atoi(argv[1])||atoi(argv[1]) <= 0)
    {
        printf("Необходимо указать количество чисел(целое число, большее 0)\n");
        exit(EXIT_FAILURE);
    }
    
    if(pipe(p1)==-1 || pipe(p2) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    

    count_args = atoi(argv[1]);

    switch(pid = fork()) 
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            close(p1[0]);
            close(p2[1]);
            printf("This is child process\n");

            for(int i = 0; i < count_args; i++)
            {
                sprintf(buf, "%d", rand()%100);
                write(p1[1], buf, sizeof(int));
                printf("Sent %d: %s\n", i+1, buf);
                read(p2[0], buf_recv, sizeof(buf_recv));
                printf("Readen %d: %d\n", i+1, atoi(buf_recv));
            }

            exit(EXIT_SUCCESS);

        default:
            close(p1[1]);
            close(p2[0]);
            printf("This is parent process\n");
            
            for(int j = 0; j < count_args; j++)
            {
                read(p1[0], buf_change, sizeof(buf_change));
                sprintf(buf_mul, "%d", atoi(buf_change)*2);
                write(p2[1], buf_mul, sizeof(buf_mul));
            }    
    }
    exit(EXIT_SUCCESS);
}