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
    pid_t pid;
    int p[2];
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

    if(pipe(p)==-1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    switch(pid = fork()) 
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            printf("This is child process\n");
            close(p[0]);
            
            while(true)
            {
                if (semop(sem_id, &lock_res, 1) == -1) 
                    perror("semop:lock_res");
                    
                getpid();

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

                write(p[1], buf_read, sizeof(buf_read));
                printf("Readen by child: %s\n", buf_read);
                arg_num2++;
                close(file);
                semop(sem_id, &rel_res, 1);
                sleep(1);
            }
            exit(EXIT_SUCCESS);

        default:
            printf("chpid %d\n", pid);
            close(p[1]);

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

                if(read(p[0], buf_read, sizeof(buf_read))==0)
                    printf("bad value");
                else
                    printf("Readen_from_pipe by parent: %s\n", buf_read);

                semop(sem_id, &rel_res, 1);
            }
    }
    exit(EXIT_SUCCESS);
}