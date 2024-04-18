#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <mqueue.h>
#include <unistd.h>
#define QUEUE_NAME "/myqueue1"
#define PRIORITY 1
#define SIZE 256
#define LAST_MESSAGE 255

bool stop = false;

void handler(int sig)
{
    stop = true;
}

int main(int argc, char* argv[])
{
    pid_t pid;
    mqd_t ds;
    char text[SIZE];
    struct mq_attr queue_attr;
    queue_attr.mq_maxmsg = 32;
    queue_attr.mq_msgsize = SIZE;
    char input;

    srand(time(NULL));

    switch(pid = fork())
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        
        case 0:
            if ((ds = mq_open(QUEUE_NAME, O_CREAT | O_RDWR , 0777, &queue_attr)) == (mqd_t)-1)
            {
                perror("Creating queue error");
                exit(EXIT_FAILURE);
            }

            signal(SIGINT, handler);

            while(true)
            {
                if(stop)
                    break;
                
                sprintf(text, "%d", rand()%1000);

                if (mq_send(ds, text, strlen(text), PRIORITY) == -1)
                {
                    perror("Sending message error");
                    exit(EXIT_FAILURE);
                }
                sleep(1);
            }
            
            strcpy(text, "");
            
            if(mq_send(ds, text, strlen(text), LAST_MESSAGE) == -1)
                perror("Last message error");

            if (mq_close(ds) == -1)
                perror("Closing queue error");

            exit(EXIT_SUCCESS);

        default:
            getchar();
            kill(pid, SIGINT);
    }
    exit(EXIT_SUCCESS);
} 