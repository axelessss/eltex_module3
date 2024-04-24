#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>

#define SEM_NAME "/my_semaphore"
#define FIFO_NAME "/tmp/fifo0001.1"
#define N 128

int main()
{
    sem_t *semaphore;
    int fd_fifo; /*дескриптор FIFO*/
    char buf[N];

    semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    if (semaphore == SEM_FAILED) 
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    /*Если файл существует, удалим его*/
    unlink(FIFO_NAME);

    /*Создаем FIFO*/
    if((mkfifo(FIFO_NAME, O_RDWR)) == -1)
    {
        fprintf(stderr, "Невозможно создать fifo\n");
        exit(EXIT_FAILURE);
    }

    /*Открываем fifo для чтения и записи*/
    if((fd_fifo=open(FIFO_NAME, O_RDWR)) == -1)
    {
        fprintf(stderr, "Невозможно открыть fifo\n");
        exit(EXIT_FAILURE);
    }

    while(true)
    {
        sem_wait(semaphore);
        if(read(fd_fifo, &buf, sizeof(buf)) == -1)
        {
            fprintf(stderr, "Невозможно прочесть из FIFO\n");
            exit(EXIT_FAILURE);
        }

        else
            printf("Прочитано из FIFO: %s\n",buf);

        sem_post(semaphore);
    }

    sem_close(semaphore);
    exit(EXIT_SUCCESS);
}