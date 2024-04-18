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
    int p[2];
    char buf_read[80];
    char buf_write[80];
    int arg_num = 0;
    int arg_num2 = 0;
    int file_pointer;

    srand(time(NULL));

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
            signal(SIGUSR1, sig_handler);
            signal(SIGUSR2, sig_handler);
            
            while(true)
            {
                getpid();
                if(denied)
                {
                    printf("acsess denied\n");
                    sleep(1);
                    continue;
                }

                file = open("contacts.txt", O_RDONLY);
                if(file == -1) 
                {
                    printf("not open for read");
                    sleep(1);
                    continue;
                }

                lseek(file, sizeof(buf_read)*arg_num2, SEEK_SET);

                if(file_pointer = read(file, buf_read, sizeof(buf_read)) <= 0)
                {
                    printf("not read\n");
                    close(file);
                    sleep(1);
                    continue;
                }
                write(p[1], buf_read, sizeof(buf_read));
                printf("Readen by child: %s\n", buf_read);
                arg_num2++;
                close(file);
                sleep(1);
            }
            exit(EXIT_SUCCESS);

        default:
            printf("pid %d\n", pid);
            close(p[1]);

            while(true)
            {
                sleep(1);
                                
                kill(pid, SIGUSR1);
                file = open("contacts.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

                if(file == -1) 
                {
                    printf("not open for write\n");
                    continue;
                }

                lseek(file, sizeof(buf_write)*arg_num, SEEK_SET);
                sprintf(buf_write, "%d", rand()%100);

                arg_num++;
                write(file, buf_write, strlen(buf_write));
                
                close(file);
                kill(pid, SIGUSR2); 

                if(read(p[0], buf_read, sizeof(buf_read))==0)
                    printf("bad value");
                else
                    printf("Readen_from_pipe by parent: %s\n", buf_read);
            }
    }
    exit(EXIT_SUCCESS);
}