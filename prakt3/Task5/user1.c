#include <sys/types.h>
#include <sys/ipc.h>
#include <mqueue.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define QUEUE_NAME "/myqueue1"
#define PRIORITY 1
#define SIZE 256
#define LAST_MESSAGE 255 /* Тип сообщения для прекращения работы программы 2 */
#define N 100

bool quit = false;

void handler(int sig)
{
    quit = true;
}

int main(int argc, char* argv[])
{
    pid_t pid;
    mqd_t ds;
    char text[SIZE];
    struct mq_attr attr, old_attr;
    int prio;
    attr.mq_maxmsg = 32;
    attr.mq_msgsize = SIZE;

    if ((ds = mq_open(QUEUE_NAME, O_CREAT | O_RDWR , 0777, &attr)) == (mqd_t)-1)
    {
        perror("Creating queue error");
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
                if (quit)
                    break;

                if (mq_receive(ds, text, SIZE, &prio) == -1)
                    continue;
            
                if(prio == 2)
                    printf("User2: %s\n", text);
                
            }
            if (mq_close(ds) == -1)
                    perror("Closing queue error");
        
            if (mq_unlink(QUEUE_NAME) == -1)
                perror("Removing queue error");

            exit(EXIT_SUCCESS);
        
        default:
            pid_t chpid = pid;
            
            signal(SIGINT, handler);

            while(true)
            {
                prio = 1;

                if(quit)
                    break;
                
                fgets(text, sizeof(text), stdin);

                if (mq_send(ds, text, strlen(text), PRIORITY) == -1)
                {
                    perror("Sending message error");
                    exit(EXIT_FAILURE);
                }
            }

            if (mq_close(ds) == -1)
                perror("Closing queue error");

            exit(EXIT_SUCCESS);
            
    }

    exit(EXIT_SUCCESS);
} 