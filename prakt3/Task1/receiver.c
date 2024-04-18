#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define LAST_MESSAGE 255

int main()
{
    int msqid; 

    char pathname[] = "key"; 

    key_t key; 

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

        printf("message type = %ld, info = %s\n", mybuf.mtype, mybuf.mtext);
    }
} 