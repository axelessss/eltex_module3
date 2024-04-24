#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <semaphore.h>

#define SEM_NAME "/my_semaphore"

int main(int argc, char *argv[])
{
    sem_t *semaphore;

    pid_t pid;
    int p[2];
    char buf_read[80];
    char buf_write[80];
    int arg_num = 0;
    int arg_num2 = 0;
    int file;
    int file_pointer;

    srand(time(NULL));

    semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    if (semaphore == SEM_FAILED) 
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

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
                sem_wait(semaphore); 
                getpid();

                file = open("contacts.txt", O_RDONLY);
                if(file == -1) 
                {
                    printf("not open for read");
                    sem_post(semaphore);
                    sleep(1);
                    continue;
                }

                lseek(file, sizeof(buf_read)*arg_num2, SEEK_SET);

                if(file_pointer = read(file, buf_read, sizeof(buf_read)) <= 0)
                {
                    printf("not read\n");
                    close(file);
                    sem_post(semaphore);
                    sleep(1);
                    continue;
                }

                write(p[1], buf_read, sizeof(buf_read));
                printf("Readen by child: %s\n", buf_read);
                arg_num2++;
                close(file);
                sem_post(semaphore);
                sleep(1);
            }
            exit(EXIT_SUCCESS);

        default:
            printf("chpid %d\n", pid);
            close(p[1]);

            while(true)
            {
                sleep(1);
                sem_wait(semaphore);
                file = open("contacts.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

                if(file == -1) 
                {
                    printf("not open for write\n");
                    sem_post(semaphore);
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

                sem_post(semaphore);
            }
    }
    exit(EXIT_SUCCESS);
}