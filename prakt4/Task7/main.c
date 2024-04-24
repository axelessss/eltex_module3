#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#define SHM_SIZE 1024
#define KEY "key"
#define SEM_NAME "/my_semaphore"

int main() 
{
    int shmid;
    char *shm_ptr;
    key_t key;
    pid_t pid;
    sem_t *semaphore;

    if((key = ftok(KEY,0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(EXIT_FAILURE);
    }

    semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    if (semaphore == SEM_FAILED) 
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Создание сегмента разделяемой памяти
    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0) 
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    
    // Подключение сегмента к адресному пространству процесса
    if ((shm_ptr = shmat(shmid, NULL, 0)) == (char *) -1) 
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    
    switch(pid = fork())
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);

        case 0:

            while(true)
            {

            }

        default:

            while (true)
            {
                
            }
            
    }
    // Запись данных в разделяемую память
    sprintf(shm_ptr, "Hello, shared memory!");
    
    // Отключение сегмента от адресного пространства процесса
    shmdt(shm_ptr);
    
    // Чтение данных из разделяемой памяти
    shm_ptr = shmat(shmid, NULL, 0);
    printf("Data read from shared memory: %s\n", shm_ptr);
    
    // Отключение сегмента от адресного пространства процесса
    shmdt(shm_ptr);
    
    // Удаление сегмента разделяемой памяти
    shmctl(shmid, IPC_RMID, NULL);
    
    return 0;
}