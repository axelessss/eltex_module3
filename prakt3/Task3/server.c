#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define N 128

struct mymsgbuf
{
    int mtype;
    char mtext[N];
} mybuf;

int main(int argc, char* argv[])
{
    int msqid; 
    char pathname[] = "key";
    char id[N];
    key_t key; 
    int len, maxlen; 

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
        if(len = msgrcv(msqid, (struct msgbuf *) &mybuf, N, 0, 0) < 0)
        {
            continue;
        }

        //if(len == 0)
          //  continue;

        printf("\nReceived from user %s\n", mybuf.mtext);
        //sprintf(id, "User %s", mybuf.mtext);
        //strcpy(mybuf.mtext, id);
        mybuf.mtype = 2;
        //len = strlen(mybuf.mtext)+1;
        //printf("%s\n", mybuf.mtext);
        if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0)
        {
            printf("Can\'t send message to queue1\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(EXIT_FAILURE);
        }    
    }
    exit(EXIT_SUCCESS);
} 