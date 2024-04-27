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

union semun 
{
    int val; /* значение для SETVAL */
    struct semid_ds *buf; /* буферы для IPC_STAT, IPC_SET */
    unsigned short *array; /* массивы для GETALL, SETALL */
    /* часть, особенная для Linux: */
    struct seminfo *__buf; /* буфер для IPC_INFO */
};

int main(int argc, char *argv[])
{
    char buf_write[N];
    int arg_num = 0;
    int file;

    key_t key;
    int sem_id;
    union semun arg;
    struct sembuf lock_res = {0, -1, 0};
    struct sembuf rel_res = {0, 1, 0};

    srand(time(NULL));

    key = ftok("key", 0);

    if ((sem_id = semget(key, 2, IPC_CREAT |  0666)) == -1) 
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    arg.val = 1;
    semctl(sem_id, 0, SETVAL, arg);

    arg.val = NUM_PROCESSES;
    semctl(sem_id, 1, SETVAL, arg);

    while(true)
    {
        sleep(1);

        if (semop(sem_id, &lock_res, 1) == -1) 
            perror("semop:lock_res");

        file = open("contacts.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

        if(file == -1) 
        {
            printf("not open for write\n");
            semop(sem_id, &rel_res, 1);
            continue;
        }

        lseek(file, sizeof(buf_write)*arg_num, SEEK_SET);
        sprintf(buf_write, "%d", rand()%100);

        arg_num++;
        write(file, buf_write, strlen(buf_write));
                
        close(file);
        semop(sem_id, &rel_res, 1);
    }
    
    exit(EXIT_SUCCESS);
}