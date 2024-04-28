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
#define N 128

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
    int nums[N];
    int data_count = 0;

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
    
    switch(pid = fork())
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);

        case 0:
            int min, max;
            signal(SIGINT, handler);
            while(true)
            {
                if(quit)
                    break;

                sem_wait(semaphore);

                // Подключение сегмента к адресному пространству процесса
                if ((shm_ptr = shmat(shmid, NULL, 0)) == (char *) -1) 
                {
                    perror("shmat");
                    exit(EXIT_FAILURE);
                }

                max = 0;
                min = shm_ptr[1];

                for (int i = 1; i < shm_ptr[0]; i++)
                {
                    if(shm_ptr[i] < min)
                        min = shm_ptr[i];
                    
                    if(shm_ptr[i] > max)
                        max = shm_ptr[i];
                }
    
                shm_ptr[shm_ptr[0]] = max;
                shm_ptr[shm_ptr[0]+1] = min;

                // Отключение сегмента от адресного пространства процесса
                if (shmdt(shm_ptr) == -1) 
                {
                    perror("shmdt");
                    exit(1);
                }
                sem_post(semaphore);
                sleep(1);
            }
            exit(EXIT_SUCCESS);

        default:
            signal(SIGINT, handler);

            while(true)
            {
                if(quit)
                    break;

                sem_wait(semaphore);

                // Подключение сегмента к адресному пространству процесса
                if ((shm_ptr = shmat(shmid, NULL, 0)) == (char *) -1) 
                {
                    perror("shmat");
                    exit(EXIT_FAILURE);
                }

                count = rand()%100;
                shm_ptr[0] = count;

                for(int i = 1; i < count; i++)
                    shm_ptr[i] = rand()%100;
                
                // Отключение сегмента от адресного пространства процесса
                if (shmdt(shm_ptr) == -1) 
                {
                    perror("shmdt");
                    exit(EXIT_FAILURE);
                }
                sem_post(semaphore);

                sem_wait(semaphore);

                if ((shm_ptr = shmat(shmid, NULL, 0)) == (char *) -1) 
                {
                    perror("shmat");
                    exit(EXIT_FAILURE);
                }

                printf("Max: %d\n", shm_ptr[shm_ptr[0]]);
                printf("Min: %d\n\n", shm_ptr[shm_ptr[0]+1]);

                 // Отключение сегмента от адресного пространства процесса
                if (shmdt(shm_ptr) == -1) 
                {
                    perror("shmdt");
                    exit(EXIT_FAILURE);
                }
                sem_post(semaphore);
                sleep(1);
            }
    }
    // Удаление сегмента разделяемой памяти
    shmctl(shmid, IPC_RMID, NULL);
    unlink(SEM_NAME);
    exit(EXIT_SUCCESS);
}