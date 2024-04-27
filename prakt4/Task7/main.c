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
#include <time.h>

#define SHM_SIZE 1024
#define KEY "key"
#define SEM_NAME "/my_semaphore"

bool quit = false;
void handler(int sig)
{
    quit = true;
}

int main(int argc, char *argv[]) 
{
    int shmid;
    char *shm_ptr;
    key_t key;
    pid_t pid;
    sem_t *semaphore;
    int i = 0;
    int count;

    srand(time(NULL));
    

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
            signal(SIGINT, handler);
            while(true)
            {
                if(quit)
                    break;

                sleep(1);
            }
            printf("\nChild: %d", i);
            exit(EXIT_SUCCESS);

        default:
            signal(SIGINT, handler);
            while(true)
            {
                if(quit)
                    break;

                count = rand()%100;
                sleep(1);
            }
            printf("\nParent: %d", i);
    }

    // Запись данных в разделяемую память
    sprintf(shm_ptr, "Hello, shared memory!");
    
    // Отключение сегмента от адресного пространства процесса
    shmdt(shm_ptr);
    
    for(int i = 0; i < 20; i++)
    {
        // Чтение данных из разделяемой памяти
        shm_ptr = shmat(shmid, NULL, 0);
        printf("Data read from shared memory: %s\n", shm_ptr);
        shmdt(shm_ptr);
    }
    
    // Отключение сегмента от адресного пространства процесса
    
    
    // Удаление сегмента разделяемой памяти
    shmctl(shmid, IPC_RMID, NULL);
    
    exit(EXIT_SUCCESS);
}