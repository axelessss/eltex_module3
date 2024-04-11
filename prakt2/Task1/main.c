#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int p[2];
    char buf[80];
    char buf_recv[80];
    int count_args;
    int arg_num;

    srand(time(NULL));

    if(argc < 2||!atoi(argv[1])||atoi(argv[1]) <= 0)
    {
        printf("Необходимо указать количество чисел(целое число, большее 0)\n");
        exit(EXIT_FAILURE);
    }
    
    if(pipe(p)==-1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    

    count_args = atoi(argv[1]);

    switch(pid = fork()) 
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            printf("This is child process\n");
            close(p[0]);

            for(int i = 0; i < count_args; i++)
            {
                sprintf(buf, "%d", rand()%100);

                if(write(p[1], buf, sizeof(int))==0)
                    continue;
                else
                    printf("Send %d: %s\n", i, buf);
            }            
            exit(EXIT_SUCCESS);

        default:
            printf("This is parent process\n");
            
            close(p[1]);
            int file = open("contacts.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
            while(arg_num < count_args)
            {
                if(read(p[0], buf_recv, sizeof(buf_recv))==0)
                    break;
                    
                else
                {
                    printf("Readen %d: %d\n", arg_num, atoi(buf_recv));
                    write(file, buf_recv, sizeof(buf_recv));
                    arg_num++;
                }
                
            }
            
            
            close(file);
    }
    exit(EXIT_SUCCESS);
}