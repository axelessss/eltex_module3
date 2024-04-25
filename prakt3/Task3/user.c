#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define N 100
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
    int msqid; 
    char pathname[] = "key"; 
    pid_t pid;
    key_t key; 
    int count,len, maxlen; 
    char input[N];
    char output[N][N];
    char message[N];
    char *arg;
    char uid[N];

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
            pid_t chpid = getpid(); 

            sprintf(uid, "%d", chpid);
            signal(SIGUSR1, handler);
            signal(SIGUSR2, handler);

            while(true)
            {
                if(!connected)
                    continue;
                
                maxlen = 81;

                if(len = msgrcv(msqid, (struct msgbuf *) &mybuf, N, 0, 0) < 0)
                {
                    printf("Can\'t receive message from queue\n");
                    exit(EXIT_FAILURE);
                }

                if(mybuf.mtype > 1)
                {
                    count = 0;
                    strcpy(message, mybuf.mtext);
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
                        printf("%s\n", mybuf.mtext); 
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
                printf("input: %d\n", strcmp(input, "connect"));
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