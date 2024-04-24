#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>

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
    char buffer[]="Текстовая строка для fifo\n";
    char buf[100];

    key_t key;
    int sem_id;
    union semun arg;
    struct sembuf lock_res = {0, -1, 0};
    struct sembuf rel_res = {0, 1, 0};
    struct sembuf push[2] = {{1, -1, IPC_NOWAIT}, {2, 1, IPC_NOWAIT}};
    struct sembuf pop[2] = {{1, 1, IPC_NOWAIT}, {2, -1, IPC_NOWAIT}};

    key = ftok("key", getpid());

    if ((sem_id = semget(key, 1, IPC_CREAT | 0666)) == -1) 
    {
        perror("semget");
        return 1;
    }

    arg.val = 1;
    semctl(sem_id, 0, SETVAL, arg);

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

    while(true)
    {
        if (semop(sem_id, &lock_res, 1) == -1) 
            perror("semop:lock_res");

        if(read(fd_fifo, &buf, sizeof(buf)) == -1)
        {
            fprintf(stderr, "Невозможно прочесть из FIFO\n");
            exit(EXIT_FAILURE);
        }

        else
            printf("Прочитано из FIFO: %s\n",buf);
        
        semop(sem_id, &rel_res, 1);
    }
    exit(EXIT_SUCCESS);
}