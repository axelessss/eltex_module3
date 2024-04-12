#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>

int main()
{
    int fd_fifo; /*дескриптор FIFO*/
    char buf[100];
    int pid;
    int action;
    int counter = 0;
    /*Если файл существует, удалим его*/
    unlink("/tmp/fifo0001.1");

    /*Создаем FIFO*/
    if((mkfifo("/tmp/fifo0001.1", O_RDWR)) == -1)
    {
        fprintf(stderr, "Невозможно создать fifo\n");
        exit(EXIT_FAILURE);
    }

    /*Открываем fifo для чтения и записи*/
    if((fd_fifo=open("/tmp/fifo0001.1", O_RDWR)) == -1)
    {
        fprintf(stderr, "Невозможно открыть fifo\n");
        exit(EXIT_FAILURE);
    }


    
    if(read(fd_fifo, &buf, sizeof(buf)) == -1)
    {
        fprintf(stderr, "Невозможно прочесть из FIFO\n");
        exit(EXIT_FAILURE);
    }

    else
    {
        pid = atoi(buf);
        printf("PID другого процесса: %d\n", pid);
    }

    while(true)
    {
        printf("Убить процесс? 2-SIGINT 3-SIGQUIT");
        scanf("%d", &action);

        switch(action)
        {
            case 2:
                kill(pid, SIGINT);
                exit(EXIT_SUCCESS);
            
            case 3:
                kill(pid, SIGQUIT);
                exit(EXIT_SUCCESS);
            
            default:
                printf("wrong command");
                break;
        }
    }
    exit(EXIT_SUCCESS);
}