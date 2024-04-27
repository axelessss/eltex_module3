#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
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
    char buf_read[N];
    int arg_num = 0;
    int file;
    int file_pointer;
    int number_proc;

    sem_write = sem_open(SEM_WRITE, O_CREAT);
    sem_read = sem_open(SEM_READ, O_CREAT);

    if (sem_write == SEM_FAILED || sem_read == SEM_FAILED) 
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    
    signal(SIGINT, handler);

    sem_wait(sem_read);

    sem_getvalue(sem_read, &number_proc);
    printf("value: %d\n", number_proc);

    while(true)
    {
        if(quit)
            break;

        file = open("contacts.txt", O_RDONLY);

        if(file == -1) 
        {
            printf("not open for read");
            sleep(1);
            continue;
        }

        lseek(file, sizeof(buf_read)*arg_num, SEEK_SET);

        if(file_pointer = read(file, buf_read, sizeof(buf_read)) <= 0)
        {
            printf("not read\n");
            close(file);
            sleep(1);
            continue;
        }

        printf("Readen: %s\n", buf_read);
        arg_num++;
        close(file);
        sleep(1);
    }
    
    sem_post(sem_read);
    exit(EXIT_SUCCESS);
}