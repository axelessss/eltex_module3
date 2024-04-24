#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>

#define N 100

union semun 
{
    int val; /* значение для SETVAL */
    struct semid_ds *buf; /* буферы для IPC_STAT, IPC_SET */
    unsigned short *array; /* массивы для GETALL, SETALL */
    /* часть, особенная для Linux: */
    struct seminfo *__buf; /* буфер для IPC_INFO */
};

int main()
{
    int fd_fifo; /*дескриптор FIFO*/
    char buffer[N];
    char buf[N];
    char fifo_path[] = "/tmp/fifo0001.1";

    key_t key;
    int sem_id;
    union semun arg;
    struct sembuf lock_res = {0, -1, 0};
    struct sembuf rel_res = {0, 1, 0};

    srand(time(NULL));
    
    key = ftok("key", getpid());

    if ((sem_id = semget(key, 1, IPC_CREAT | 0666)) == -1) 
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    arg.val = 1;
    semctl(sem_id, 0, SETVAL, arg);

    if (mkfifo(fifo_path, O_RDWR) == -1 && errno != EEXIST) 
    {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    /*Открываем fifo для чтения и записи*/
    if((fd_fifo=open(fifo_path, O_RDWR)) == -1)
    {
        fprintf(stderr, "Невозможно открыть fifo\n");
        exit(EXIT_FAILURE);
    }

    while(true)
    {
        if (semop(sem_id, &lock_res, 1) == -1) 
            perror("semop:lock_res");

        sprintf(buffer, "%d", rand()%1000);
        printf("Отправлено: %s\n", buffer);
        write(fd_fifo, buffer, strlen(buffer));
        semop(sem_id, &rel_res, 1);

        sleep(2);
    }

    exit(EXIT_SUCCESS);
}