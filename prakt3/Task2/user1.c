#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define LAST_MESSAGE 255 /* Тип сообщения для прекращения работы программы 2 */
#define N 100

int main(int argc, char* argv[])
{
    int msqid; 
    char pathname[] = "key"; 
    pid_t pid;
    key_t key; 
    int count,len, maxlen; 
    char input;

    srand(time(NULL));

    struct mymsgbuf
    {
        long mtype;
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
            while(true)
            {
                maxlen = 81;

                if(len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, 0, 0) < 0)
                {
                    printf("Can\'t receive message from queue\n");
                    exit(EXIT_FAILURE);
                }

                if(mybuf.mtype == LAST_MESSAGE)
                {
                    msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                    exit(EXIT_SUCCESS);
                }   

                if(mybuf.mtype == 2)
                    printf("User2: %s\n", mybuf.mtext);
            }
            exit(EXIT_SUCCESS);
        
        default:
            pid_t chpid = pid;
            while(true)
            {                
                mybuf.mtype = 1;
                printf(">> ");
                fgets(mybuf.mtext, sizeof(mybuf.mtext), stdin);
        
                len = strlen(mybuf.mtext)+1;

                if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0)
                {
                    printf("Can\'t send message to queue1\n");
                    msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                    exit(EXIT_FAILURE);
                }

                printf("\nSent: %s\n", mybuf.mtext);
            }
            
    }

    exit(EXIT_SUCCESS);
} 