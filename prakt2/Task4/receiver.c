#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    int fd_fifo; /*дескриптор FIFO*/
    char buffer[]="Текстовая строка для fifo\n";
    char buf[100];

    /*Если файл существует, удалим его*/
    unlink("/tmp/fifo0001.1");

    /*Создаем FIFO*/
    if((mkfifo("/tmp/fifo0001.1", O_RDWR)) == -1)
    {
        fprintf(stderr, "Невозможно создать fifo\n");
        exit(EXIT_FAILURE);
    }

    /*Открываем fifo для чтения и записи*/
    if((fd_fifo=open("/tmp/fifo0001.1", O_RDWR)) == -1)
    {
        fprintf(stderr, "Невозможно открыть fifo\n");
        exit(EXIT_FAILURE);
    }

    if(read(fd_fifo, &buf, sizeof(buf)) == -1)
        fprintf(stderr, "Невозможно прочесть из FIFO\n");
    else
        printf("Прочитано из FIFO: %s\n",buf);

    exit(EXIT_SUCCESS);
}