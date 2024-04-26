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
    char buf_read[80];
    char buf_write[80];
    int arg_num = 0;
    int arg_num2 = 0;
    int file;
    int file_pointer;

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
       
    while(true)
    {
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

        lseek(file, sizeof(buf_read)*arg_num2, SEEK_SET);

        if(file_pointer = read(file, buf_read, sizeof(buf_read)) <= 0)
        {
            printf("not read\n");
            close(file);
            semop(sem_id, &rel_res, 1);
            sleep(1);
            continue;
        }

        printf("Readen by child: %s\n", buf_read);
        arg_num2++;
        close(file);
        semop(sem_id, &rel_res, 1);
        sleep(1);
    }

    exit(EXIT_SUCCESS);
}