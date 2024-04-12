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
#include <signal.h>

int file;
bool denied = false;

void sig_handler(int sig)
{
    switch(sig)
    {
        case SIGUSR1:
            denied = true;
            break;
        
        case SIGUSR2:
            denied = false;
            break;
    }
}

int main(int argc, char *argv[])
{
    pid_t pid;
    char buf_read[80];
    char buf_write[80];
    int count_args;
    int arg_num = 0;
    int arg_num2 = 0;
    int file_pointer;
    int arg;

    srand(time(NULL));

    switch(pid = fork()) 
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            printf("This is child process\n");

            signal(SIGUSR1, sig_handler);
            signal(SIGUSR2, sig_handler);

            while(true)
            {
                if(denied)
                    continue;

                if(file = open("contacts.txt", O_RDONLY) == -1) 
                {
                    close(file);
                    continue;
                }

                lseek(file, sizeof(buf_read)*arg_num2, SEEK_SET);

                if(file_pointer = read(file, buf_read, sizeof(buf_read)) <= 0)
                {
                    close(file);
                    continue;
                }

                printf("Readen: %s", buf_read);
                arg_num2++;
                close(file);
                sleep(1);
            }
            exit(EXIT_SUCCESS);

        default:
            printf("This is parent process\n");
            
            while(true)
            {
                sleep(1);
                kill(pid, SIGUSR1);
                if(file = open("contacts.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR) == -1) 
                    continue;

                lseek(file, sizeof(buf_read)*arg_num, SEEK_SET);
                sprintf(buf_write, "%d", rand()%100);
                printf("%s", buf_write);
                arg_num++;
                write(file, buf_write, sizeof(buf_write));
                
                close(file); 
                //sleep(1);
                kill(pid, SIGUSR2); 
            }
    }
    exit(EXIT_SUCCESS);
}