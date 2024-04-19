#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define N 128

int main(int argc, char* argv[])
{
    int msqid; 
    char pathname[] = "key"; 
    pid_t pid;
    key_t key; 
    int count,len, maxlen; 
    char input[N];
    char output[N][N];
    char *arg;
    bool connected = false;

    struct mymsgbuf
    {
        int mtype;
        char mtext[N];
    } mybuf;

    if((key = ftok(pathname,0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(EXIT_FAILURE);
    }

    if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get msqid\n");
        exit(EXIT_FAILURE);
    }

    switch(pid = fork())
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);

        case 0:
            pid_t ppid = getppid(); 

            while(true)
            {
                maxlen = 81;

                if(len = msgrcv(msqid, (struct msgbuf *) &mybuf, N, 0, 0) < 0)
                {
                    printf("Can\'t receive message from queue\n");
                    exit(EXIT_FAILURE);
                }

                if(mybuf.mtype > 1)
                {
                    count = 0;
                    arg = strtok(mybuf.mtext, " ");

                    while(arg != NULL)
                    {
                        strcpy(output[count], arg);
                        arg = strtok(NULL, " ");
                        count++;
                    }

                    if(ppid != atoi(output[0]))
                        printf("%s\n", mybuf.mtext);
                }
            }
            exit(EXIT_SUCCESS);
        
        default:
            pid_t chpid = pid;
            pid_t thispid = getpid();

            while(true)
            {                
                mybuf.mtype = 1;
                printf(">> ");
                fgets(input, sizeof(input), stdin);

                sprintf(mybuf.mtext, "%d - %s", thispid, input);

                len = strlen(mybuf.mtext)+1;

                if(strcmp(mybuf.mtext, "connect") == 0)
                    connected = true;

                if(!connected)
                {
                    printf("No connection. Connect to server using \"connect\"");
                    continue;
                }

                if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0)
                {
                    printf("Can\'t send message to queue1\n");
                    msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                    exit(EXIT_FAILURE);
                }
                if(strcmp(mybuf.mtext, "disconnect") == 0)
                {
                    kill(pid, SIGINT);
                    break;
                }
                //printf("\nSent: %s\n", mybuf.mtext);
            }    
    }
    exit(EXIT_SUCCESS);
} 