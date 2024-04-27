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
#include <semaphore.h>
#include <signal.h>

#define N 100
#define NUM_PROCESSES 2
#define SEM_WRITE "/sem_write"
#define SEM_READ "/sem_read"

bool quit = false;

void handler(int sig)
{
    quit = true;
}

int main(int argc, char *argv[])
{
    sem_t *sem_write, *sem_read;
    char buf_write[N];
    int arg_num = 0;
    int file;
    
    sem_write = sem_open(SEM_WRITE, O_CREAT, 0666, 1);
    sem_read = sem_open(SEM_READ, O_CREAT, 0666, NUM_PROCESSES);

    if (sem_write == SEM_FAILED || sem_read == SEM_FAILED) 
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    signal(SIGINT, handler);

    while(true)
    {
        if(quit)
            break;

        sleep(1);

        sem_trywait(sem_write);

        file = open("contacts.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

        if(file == -1) 
        {
            printf("not open for write\n");
            sem_post(sem_write);
            continue;
        }

        lseek(file, sizeof(buf_write)*arg_num, SEEK_SET);
        sprintf(buf_write, "%d", rand()%100);

        arg_num++;
        write(file, buf_write, strlen(buf_write));
                
        close(file);
        sem_post(sem_write);
    }
    
    sem_close(sem_write);
    sem_unlink(SEM_WRITE);

    sem_close(sem_read);
    sem_unlink(SEM_READ);

    exit(EXIT_SUCCESS);
}