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

int main(int argc, char* argv[])
{
    int msqid; 
    char pathname[] = "key"; 
    key_t key; 
    int count,len; 
    char input;

    srand(time(NULL));

    count = atoi(argv[1]);

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

    for(int i = 0; i < count; i++)
    {
        
        mybuf.mtype = rand()%254;
        sprintf(mybuf.mtext, "%d", rand()%1000);
        

        len = strlen(mybuf.mtext)+1;

        if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0)
        {
            printf("Can\'t send message to queue1\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(EXIT_FAILURE);
        }
        sleep(1);
    }

    mybuf.mtype = LAST_MESSAGE;
    len = 0;

    if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0)
    {
        printf("Can\'t send message to queue2\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
} 