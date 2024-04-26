#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>

#define N 100
#define NUM_PROCESSES 2

bool quit = false;

union semun 
{
    int val; /* значение для SETVAL */
    struct semid_ds *buf; /* буферы для IPC_STAT, IPC_SET */
    unsigned short *array; /* массивы для GETALL, SETALL */
    /* часть, особенная для Linux: */
    struct seminfo *__buf; /* буфер для IPC_INFO */
};

void handler(int sig)
{
    quit = true;
}

int main(int argc, char *argv[])
{
    char buf_read[N];
    int arg_num = 0;
    int file;
    int file_pointer;

    key_t key;
    int sem_id;
    union semun arg;
    struct sembuf sbless = {1, -1, SEM_UNDO}, sbmore = {1, 1, SEM_UNDO};
    struct sembuf lock_res = {0, -1, 0};
    struct sembuf rel_res = {0, 1, 0};

    srand(time(NULL));

    key = ftok("key", getpid());

    if ((sem_id = semget(key, 2, IPC_CREAT |  0666)) == -1) 
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    arg.val = 1;
    if (semctl(sem_id, 0, SETVAL, arg) == -1) 
    {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    arg.val = NUM_PROCESSES;
    if (semctl(sem_id, 1, SETVAL, arg) == -1) 
    {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handler);

    

        if (semop(sem_id, &sbmore, 1) == -1) 
        {
            perror("semop");
            exit(EXIT_FAILURE);
        }

    

        while(true)
        {
            printf("val: %d", semctl(sem_id, 1, GETVAL));
            if (semctl(sem_id, 1, GETVAL) <= 0) 
            {
                printf("Reached maximum number of processes. Waiting...\n");
                // Ждем, пока какой-то из процессов не освободит семафор
                if (semop(sem_id, &sbmore, 1) == -1) 
                {
                    perror("semop");
                }
                sleep(1);
                continue;
            }

            if(quit)
                break;

            if (semop(sem_id, &lock_res, 1) == -1) 
                perror("semop:lock_res");
                    
            file = open("contacts.txt", O_RDONLY);
            if(file == -1) 
            {
                printf("not open for read");
                semop(sem_id, &rel_res, 1);
                sleep(1);
                continue;
            }

            lseek(file, sizeof(buf_read)*arg_num, SEEK_SET);

            if(file_pointer = read(file, buf_read, sizeof(buf_read)) <= 0)
            {
                printf("not read\n");
                close(file);
                semop(sem_id, &rel_res, 1);
                sleep(1);
                continue;
            }

            //printf("Readen: %s\n", buf_read);
            arg_num++;
            close(file);
            semop(sem_id, &rel_res, 1);
            sleep(1);
        }

        if (semop(sem_id, &sbless, 1) == -1) 
        {
            perror("semop");
            exit(EXIT_FAILURE);
        }
    
    exit(EXIT_SUCCESS);
}