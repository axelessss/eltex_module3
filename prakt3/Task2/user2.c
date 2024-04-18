#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define LAST_MESSAGE 255

int main()
{
    int msqid; 
    char pathname[] = "key"; 
    key_t key; 
    pid_t pid;
    int len, maxlen; 

    struct mymsgbuf
    {
        long mtype;
        char mtext[81];
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

                if(mybuf.mtype == 1)
                    printf("User1: %s\n", mybuf.mtext);
            }
            exit(EXIT_SUCCESS);
        
        default:
            pid_t chpid = pid;
            while(true)
            {                
                mybuf.mtype = 2;
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
} 