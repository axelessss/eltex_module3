#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 100

int Splitter(char *args_in, char **args, char *splitter)
{
    char *arg;
    char temp[N][N];
    int count_args = 0;
    arg = strtok(args_in, splitter);

    while(arg != NULL)
    {
        strcpy(temp[count_args], arg);
        arg = strtok(NULL, splitter);
        count_args++;
    }
    
    for(int i = 0; i < count_args; i++)
    {
        args[i] = temp[i]; 
    }
    args[count_args] = NULL;

    return count_args;
}

int main(int argc, char *argv[])
{
    pid_t pid, pid2, pid3;
    int rv;
    int count_args;
    char args_in[N];

    int fd_ls[2];
    int fd_head[2];

    // Создаем канал для ls
    if (pipe(fd_ls) == -1) 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Создаем канал для head
    if (pipe(fd_head) == -1) 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    while(true)
    {
        char **args = (char **)malloc(N*sizeof(char*));
        
        char splitter[] = " ";
        printf(">> ");

        gets(args_in);
        
        if(strcmp(args_in, "exit") == 0)
            break;

        if(strstr(args_in, "|") != NULL)
            strcpy(splitter, "|");
        
        count_args = Splitter(args_in, args, splitter);
        
        switch(pid = fork()) 
        {
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0:
                printf("This is child process\n");
                if(strcmp(splitter, " ") == 0)
                {

                    if(count_args == 1)
                        execlp(args[0], " ", NULL);
                    else
                        execvp(args[0], args);
                }

                else if(strcmp(splitter, "|") == 0)
                {
                    if(count_args == 1)
                    {
                        perror("invalid arguments");
                        exit(EXIT_FAILURE);
                    }

                    switch(pid2 = fork())
                    {
                        case -1:
                            perror("fork");
                            exit(EXIT_FAILURE);

                        case 0:
                            if(count_args == 3)
                            {
                                pid3 = fork();
                                
                                    if(pid3 == -1)
                                    {
                                        perror("fork");
                                        exit(EXIT_FAILURE);
                                    }
                                    
                                    else if(pid3 == 0)
                                    {
                                        char **args4 = (char **)malloc(N*sizeof(char*));
                                        char splitter4[] = " ";

                                        int count_args4 = Splitter(args[2], args4, splitter4);

                                        dup2(fd_head[0], STDIN_FILENO);
                                        close(fd_ls[0]);
                                        
                                        close(fd_head[1]);

                                        if(count_args4 == 1)
                                            execlp(args4[0], " ", NULL);
                                        else
                                            execvp(args4[0], args4);

                                        free(args4);
                                        exit(EXIT_SUCCESS);
                                    }

                                    else
                                    {
                                        char **args3 = (char **)malloc(N*sizeof(char*));
                                        char splitter3[] = " ";

                                        int count_args3 = Splitter(args[1], args3, splitter3);
                                        dup2(fd_ls[0], STDIN_FILENO);
                                    
                                        dup2(fd_head[1], STDOUT_FILENO);
                                        
                                        close(fd_ls[1]);
                                        close(fd_head[0]);
                                        
                                        if(count_args3 == 1)
                                            execlp(args3[0], " ", NULL);
                                        else
                                            execvp(args3[0], args3);

                                        waitpid(pid3, NULL, 0);
                                        free(args3);
                                        exit(EXIT_SUCCESS);
                                    }
                                
                            }

                            else
                            {
                                char **args3 = (char **)malloc(N*sizeof(char*));
                                char splitter3[] = " ";

                                int count_args3 = Splitter(args[1], args3, splitter3);
                                
                                dup2(fd_ls[0], STDIN_FILENO);
                                    

                                close(fd_ls[1]);
                                close(fd_head[0]);
                                    
                                if(count_args3 == 1)
                                    execlp(args3[0], " ", NULL);
                                else
                                    execvp(args3[0], args3);

                                free(args3);
                                exit(EXIT_SUCCESS);
                            }

                        default:

                            char **args2 = (char **)malloc(N*sizeof(char*));
                            char splitter2[] = " ";
                            int count_args2 = Splitter(args[0], args2, splitter2);

                            dup2(fd_ls[1], STDOUT_FILENO);
                            close(fd_ls[0]);

                            if(count_args2 == 1)
                                execlp(args2[0], " ", NULL);
                            else
                                execvp(args2[0], args2);

                            free(args2);
                            waitpid(pid2, NULL, 0);
                            exit(EXIT_SUCCESS);
                    }
                }

                else
                {
                    perror("invalid args");
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);

            default:
                printf("This is parent process\n");
                waitpid(pid, NULL, 0);
        }
        free(args);
    }

    exit(EXIT_SUCCESS);
}