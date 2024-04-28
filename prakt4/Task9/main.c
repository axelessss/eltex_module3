#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

#define SHM_SIZE 1024
#define SHM_NAME "/my_shared_memory"
#define SEM_NAME "/my_semaphore"
#define N 128

bool quit = false;
void handler(int sig)
{
    quit = true;
}

int main(int argc, char *argv[]) 
{
    int shm_fd;
    int *shm_ptr;
    pid_t pid;
    sem_t *semaphore;
    int i = 0;
    int count;
    int nums[N];
    int data_count = 0;

    srand(time(NULL));

    semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    if (semaphore == SEM_FAILED) 
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Создание или открытие сегмента разделяемой памяти
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);

    if (shm_fd == -1) 
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Установка размера разделяемой памяти
    if (ftruncate(shm_fd, SHM_SIZE+1) == -1) 
    {
        perror("ftruncate");
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

                sem_trywait(semaphore);

                shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

                if (shm_ptr == MAP_FAILED) 
                {
                    perror("mmap");
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

                // Отмена отображения разделяемой памяти и закрытие дескриптора файла
                if (munmap(shm_ptr, SHM_SIZE) == -1) 
                {
                    perror("munmap");
                    exit(EXIT_FAILURE);
                }
            
                if (close(shm_fd) == -1) 
                {
                    perror("close");
                    exit(EXIT_FAILURE);
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

                sem_trywait(semaphore);

                shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

                if (shm_ptr == MAP_FAILED) 
                {
                    perror("mmap");
                    exit(EXIT_FAILURE);
                }

                count = rand()%100;
                shm_ptr[0] = count;

                for(int i = 1; i < count; i++)
                    shm_ptr[i] = rand()%100;
                
                // Отмена отображения разделяемой памяти и закрытие дескриптора файла
                if (munmap(shm_ptr, SHM_SIZE) == -1) 
                {
                    perror("munmap");
                    exit(EXIT_FAILURE);
                }
            
                if (close(shm_fd) == -1) 
                {
                    perror("close");
                    exit(EXIT_FAILURE);
                }
                sem_post(semaphore);

                sem_trywait(semaphore);

                shm_ptr = mmap(0, SHM_SIZE+1, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

                if (shm_ptr == MAP_FAILED) 
                {
                    perror("mmap");
                    exit(EXIT_FAILURE);
                }

                printf("Max: %d\n", shm_ptr[shm_ptr[0]]);
                printf("Min: %d\n\n", shm_ptr[shm_ptr[0]+1]);
                data_count++;

                // Отмена отображения разделяемой памяти и закрытие дескриптора файла
                if (munmap(shm_ptr, SHM_SIZE) == -1) 
                {
                    perror("munmap");
                    exit(EXIT_FAILURE);
                }
            
                if (close(shm_fd) == -1) 
                {
                    perror("close");
                    exit(EXIT_FAILURE);
                }
                sem_post(semaphore);
                sleep(1);
            }
            printf("\n Readen segments of data: %d\n", data_count);
    }

    // Удаление сегмента разделяемой памяти
    if (shm_unlink(SHM_NAME) == -1) 
    {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }

    unlink(SEM_NAME);
    exit(EXIT_SUCCESS);
}