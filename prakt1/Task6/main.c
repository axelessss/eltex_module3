#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define N 500

int main()
{
    DIR *dir;
    struct dirent *ent;
    struct stat st;
    int file_stat;

    int count_args;
    char command[N];
    char *arg;
    char args[2][N];
    char cwd[1024];

    // Открываем каталог
    dir = opendir(".");
    
    if (dir == NULL) 
    {
        perror("opendir");
        return -1;
    }

    while(true)
    {
        count_args = 0;
        printf("\n>> ");
        gets(command);

        if(strcmp(command, "exit") == 0)
            break;

        arg = strtok(command, " ");

        while(arg != NULL)
        {
            strcpy(args[count_args], arg);
            arg = strtok(NULL, " ");
            count_args++;
        }

        if(strcmp(args[0], "ls") == 0)
        {
            if(dir == NULL)
            {
                printf("error occured while reading directory\n");
                continue;
            }
            else
            {
                switch (count_args)
                {
                    case 1:
                        while ((ent = readdir(dir)) != NULL) 
                            printf("%s  ", ent->d_name);
                        break;
            
                    case 2:
                        if(strcmp(args[1], "-l") == 0)
                            while ((ent = readdir(dir)) != NULL)
                            {
                                if(stat(&ent->d_name, &st)==-1)
                                {
                                    printf("error occured while reading %s\n", ent->d_name);
                                    continue;
                                }
                                    printf("Размер файла: %ld байт\n", st.st_size);
                                    printf("Время последней модификации: %ld секунд\n", st.st_mtime);
                            }
                        else
                            printf("wrong attributes ");
                        break;
                
                /*case 3:
                    if(strcmp(args[1], "-l") == 0)
                        while ((ent = readdir(dir)) != NULL)
                        {
                            
                            if(stat(&ent->d_name, &st)==-1)
                            {
                                printf("error occured while reading %s\n", ent->d_name);
                                continue;
                            }
                                printf("Размер файла: %ld байт\n", st.st_size);
                                printf("Время последней модификации: %ld секунд\n", st.st_mtime);
                        }
                    else
                        printf("wrong attributes ");*/
                    default:
                        break;
                }
            }
        }

        if(strcmp(args[0], "cd") == 0)
        {
            switch(count_args)
            {
                case 1:
                    dir = opendir("/");
                    break;
                
                case 2:
                    if (getcwd(cwd, sizeof(cwd)) == NULL) 
                    {
                        perror("getcwd() error");
                        continue;
                    } 

                    dir = opendir(args[1]);
                    if (dir == NULL) 
                    {
                        perror("No such file or directory\n");
                        dir = opendir(cwd);
                        continue;
                    }
            }
        }   
    }
    // Закрываем каталог
    closedir(dir);
    return 0;
}