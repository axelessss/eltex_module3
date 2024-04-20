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
#define N 128

bool connected = false;

void handler(int sig)
{
    if(sig == SIGUSR1)
        connected = false;
    else if(sig == SIGUSR2)
        connected = true;
}

int main(int argc, char* argv[])
{
    mqd_t ds; 
    char pathname[] = "key"; 
    pid_t pid;
    char text[SIZE];
    struct mq_attr attr, old_attr;
    int prio;
    attr.mq_maxmsg = 32;
    attr.mq_msgsize = SIZE;
    int count, len, maxlen; 
    char input[N];
    char output[N][N];
    char message[N];
    char *arg;
    char uid[N];


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
            pid_t chpid = getpid(); 

            sprintf(uid, "%d", chpid);
            signal(SIGUSR1, handler);
            signal(SIGUSR2, handler);

            while(true)
            {
                if(!connected)
                    continue;
                
                maxlen = 81;

                if (mq_receive(ds, text, SIZE, &prio) == -1)
                    continue;

                if(prio > 1)
                {
                    count = 0;
                    strcpy(message, text);
                    arg = strtok(message, " ");
                    while(arg != NULL)
                    {
                        strcpy(output[count], arg);
                        arg = strtok(NULL, " ");
                        count++;
                    }

                    if(strcmp(uid, output[1]) == 0)
                        continue;

                    else
                        printf("%s\n", text); 
                }
            }
            exit(EXIT_SUCCESS);
        
        default:

            while(true)
            {                
                mybuf.mtype = 1;
                printf(">> ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';

                sprintf(mybuf.mtext, "%d - %s", pid, input);

                if(strcmp(input, "connect") == 0)
                {
                    if(connected)
                    {
                        printf("Already connected to server\n");
                        continue;
                    }

                    strcpy(mybuf.mtext, input);
                    kill(pid, SIGUSR2);
                    connected = true;
                }

                if(strcmp(input, "disconnect") == 0)
                {
                    if(!connected)
                    {
                        printf("Already disconnected to server\n");
                        continue;
                    }
                    strcpy(mybuf.mtext, input);
                    kill(pid, SIGUSR1);
                    connected = false;
                }

                if(!connected && strcmp(mybuf.mtext, "disconnect") != 0)
                {
                    printf("No connection. Connect to server using \"connect\"");
                    continue;
                }

                len = strlen(mybuf.mtext)+1;

                if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0)
                {
                    printf("Can\'t send message to queue1\n");
                    msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                    exit(EXIT_FAILURE);
                }
            }    
    }
    exit(EXIT_SUCCESS);
} 