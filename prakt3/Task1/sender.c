#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#define LAST_MESSAGE 255 /* Тип сообщения для прекращения работы программы 2 */

bool stop = false;

void handler(int sig)
{
    stop = true;
}

int main(int argc, char* argv[])
{
    int msqid; 
    char pathname[] = "key"; 
    key_t key; 
    pid_t pid;
    int count,len; 
    char input;

    srand(time(NULL));

    struct mymsgbuf
    {
        long mtype;
        char mtext[81];
    } mybuf;

    switch(pid = fork())
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        
        case 0:
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

            signal(SIGINT, handler);

            while(true)
            {
                if(stop)
                    break;
                
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
        
        default:
            getchar();
            kill(pid, SIGINT);
    }
    exit(EXIT_SUCCESS);
} 