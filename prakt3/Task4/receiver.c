/* Прием сообщения из очереди */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mqueue.h>
#define QUEUE_NAME "/myqueue1"
#define PRIORITY 1
#define SIZE 256
#define LAST_MESSAGE 255

int main()
{
    mqd_t ds;
    char new_text[SIZE];
    struct mq_attr attr, old_attr;
    int prio;
    if ((ds = mq_open(QUEUE_NAME, O_RDWR | O_NONBLOCK, 0777, NULL)) == (mqd_t)-1)
    {
        perror("Creating queue error");
        exit(EXIT_FAILURE);
    }

    while(true)
    {
        if (mq_receive(ds, new_text, SIZE, &prio) == -1)
        {
            continue;
            //perror("cannot receive");
            //exit(EXIT_FAILURE);
        }

        if(prio == LAST_MESSAGE)
        {
            if (mq_close(ds) == -1)
                perror("Closing queue error");
        
            if (mq_unlink(QUEUE_NAME) == -1)
                perror("Removing queue error");

            exit(EXIT_SUCCESS);
        }
        printf("Message: %s, prio = %d\n", new_text, prio);
        //sleep(1);
    }
    exit(EXIT_SUCCESS);
}