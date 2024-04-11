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

#define N 100

int main()
{
    int fd_fifo; /*дескриптор FIFO*/
    char buffer[N];
    char buf[N];
    char fifo_path[] = "/tmp/fifo0001.1";

    srand(time(NULL));
    /*// Удаляем FIFO
    if (unlink(fifo_path) == -1) 
    {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    else 
    {
        printf("FIFO успешно удален.\n");
    }*/

    //unlink(fifo_path);
    
    // Создаем FIFO
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

    // Изменяем права доступа к FIFO
    if (chmod(fifo_path, O_RDWR) == -1) 
    {
        perror("chmod");
        exit(EXIT_FAILURE);
    } 
    
    else 
    {
        printf("Права доступа к FIFO успешно изменены.\n");
    }
    
    // Проверяем доступность FIFO для чтения
    if (access(fifo_path, R_OK) == 0) {
        printf("У вас есть права на чтение FIFO.\n");
    } else {
        perror("access");
        printf("У вас нет прав на чтение FIFO.\n");
    }

    // Проверяем доступность FIFO для записи
    if (access(fifo_path, W_OK) == 0) {
        printf("У вас есть права на запись в FIFO.\n");
    } else {
        perror("access");
        printf("У вас нет прав на запись в FIFO.\n");
    }

    /*Открываем fifo для чтения и записи*/
    if((fd_fifo=open(fifo_path, O_RDWR)) == -1)
    {
        fprintf(stderr, "Невозможно открыть fifo\n");
        exit(EXIT_FAILURE);
    }

    while(true)
    {
        sprintf(buffer, "%d", rand()%1000);
        printf("Отправлено: %s\n", buffer);
        write(fd_fifo, buffer, strlen(buffer));
        sleep(2);
    }

    exit(EXIT_SUCCESS);
}