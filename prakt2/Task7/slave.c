#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define N 100

int wait_sig = 0;

void listener(int sig) 
{
    switch(sig)
    {   case 2:
            printf("Получен сигнал SIGINT");
            break;
        case 3:
            printf("Получен сигнал SIGQUIT");
            break;
    }
    exit(EXIT_SUCCESS);
}

int main()
{
    int fd_fifo; 
    char buffer[N];
    char fifo_path[] = "/tmp/fifo0001.1";
    int pid = getpid();
    int counter = 0;


    if(pid <= 0)
    {
        printf("error occured");
        exit(EXIT_FAILURE);
    }
    
    if (mkfifo(fifo_path, O_RDWR) == -1) 
    {
        if (errno == EEXIST) 
        {
            printf("FIFO уже существует.\n");
        } 
        else 
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    } 

    else 
    {
        printf("FIFO успешно создан.\n");
    }

    if (chmod(fifo_path, O_RDWR) == -1) 
    {
        perror("chmod");
        exit(EXIT_FAILURE);
    } 
    
    else 
    {
        printf("Права доступа к FIFO успешно изменены.\n");
    }
    
    if (access(fifo_path, R_OK) == 0) 
    {
        printf("У вас есть права на чтение FIFO.\n");
    }

    else 
    {
        perror("access");
        printf("У вас нет прав на чтение FIFO.\n");
    }

    if (access(fifo_path, W_OK) == 0) 
    {
        printf("У вас есть права на запись в FIFO.\n");
    }

    else 
    {
        perror("access");
        printf("У вас нет прав на запись в FIFO.\n");
    }

    if((fd_fifo=open(fifo_path, O_RDWR)) == -1)
    {
        fprintf(stderr, "Невозможно открыть fifo\n");
        exit(EXIT_FAILURE);
    }

    sprintf(buffer, "%d", pid);
    printf("PID: %s\n", buffer);
    write(fd_fifo, buffer, strlen(buffer));

    signal(SIGINT, listener);
    signal(SIGQUIT, listener);

    while(true)
    {
        int file = open("counter.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
        
        sprintf(buffer, "%d\n", counter);
        lseek(file, sizeof(buffer)*counter, SEEK_SET);
        counter++;

        write(file, buffer, sizeof(buffer));
        close(file);
        
        sleep(3);
    }

    exit(EXIT_SUCCESS);
}