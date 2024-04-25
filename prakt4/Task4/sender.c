#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
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
    char buffer[N];

    srand(time(NULL));
    
    semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    if (semaphore == SEM_FAILED) 
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if (mkfifo(FIFO_NAME, O_RDWR) == -1 && errno != EEXIST) 
    {
        perror("mkfifo");
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
        sem_trywait(semaphore);
        sprintf(buffer, "%d", rand()%1000);
        printf("Отправлено: %s\n", buffer);
        write(fd_fifo, buffer, strlen(buffer));
        sem_post(semaphore);
        sleep(2);
    }

    exit(EXIT_SUCCESS);
}