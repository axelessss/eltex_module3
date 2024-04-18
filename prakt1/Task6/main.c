#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>

#define N 200

void cut_path(char *path)
{
    char new_path[N];
    char *last_occurence = strrchr(path, '/');
    long last_index = last_occurence - path;

    for(int i = 0; i < last_index; i++)
    {
        new_path[i] = path[i];
    }
    new_path[last_index] = '\0';

    strcpy(path, new_path);
}

int main()
{
    DIR *dir;
    struct dirent *ent;
    struct stat st;
    int file_stat;

    int count_args;
    char command[N];
    char *arg;
    char home_dir[] = "/home/akooleus";
    char this_dir[N];
    char prev_dir[N];
    char args[2][N];
    char cwd[1024];
    char path[1024];
    long pos;


    dir = opendir(home_dir);
    strcpy(this_dir, home_dir);
    strcpy(prev_dir, home_dir);

    if (dir == NULL) 
    {
        perror("opendir");
        return -1;
    }

    while(true)
    {
        count_args = 0;
        printf("\n%s >> ", this_dir);
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
                pos = telldir(dir);
                switch (count_args)
                {
                    case 1:
                        while ((ent = readdir(dir)) != NULL) 
                            printf("%s  ", ent->d_name);
                        
                        break;
            
                    case 2:
                        if(strcmp(args[1], "-l") == 0)
                        {
                            while ((ent = readdir(dir)) != NULL)
                            {
                                if(stat(&ent->d_name, &st)==-1)
                                {
                                    printf("error occured while reading %s\n", ent->d_name);
                                    break;
                                }

                                printf("Информация о файле: %s ", ent->d_name);
                                printf("Размер: %ld байт ", st.st_size);
                                printf("Последний доступ: %ld ", st.st_atime);
                                printf("Последняя модификация: %ld ", st.st_mtime);
                                printf("\n");
                            }
                        }
                        else
                            printf("wrong attributes ");
                        break;
                
                case 3:
                    if(strcmp(args[1], "-l") == 0)
                    {
                        while ((ent = readdir(dir)) != NULL)
                        {
                            if(strcmp(ent->d_name, args[2]) == 0)
                            {
                                if(stat(&ent->d_name, &st)==-1)
                                    printf("error occured while reading %s\n", ent->d_name);
                                    
                                else
                                {
                                    printf("Информация о файле: %s ", ent->d_name);
                                    printf("Размер: %ld байт ", st.st_size);
                                    printf("Последний доступ: %ld ", st.st_atime);
                                    printf("Последняя модификация: %ld ", st.st_mtime);
                                    printf("\n");
                                }
                            }
                        }
                    }
                    else
                        printf("wrong attributes ");

                    default:
                        break;
                }
                seekdir(dir, pos);
            }
        }

        if(strcmp(args[0], "cd") == 0)
        {
            switch(count_args)
            {
                case 1:
                    strcpy(prev_dir, this_dir);
                    strcpy(this_dir, home_dir);
                    dir = opendir(this_dir);
                    break;
                
                case 2:
                    if(strcmp(args[1], "-") == 0)
                    {
                        dir = opendir(prev_dir);
                        strcpy(this_dir, prev_dir);
                        strcpy(prev_dir, this_dir);
                    }

                    else if(strcmp(args[1], ".")==0)
                    {
                        strcpy(prev_dir, this_dir);
                    }

                    else if(strcmp(args[1], "..") == 0)
                    {
                        strcpy(prev_dir, this_dir);
                        cut_path(this_dir);
                        
                        dir = opendir(this_dir);
                        if (dir == NULL) 
                        {
                            perror("No such file or directory\n");
                            opendir(prev_dir);
                        }  
                    }

                    else
                    {
                        strcpy(prev_dir, this_dir);
                        strcat(this_dir, args[1]);
                        dir = opendir(this_dir);
                        if (dir == NULL) 
                        {
                            strcpy(this_dir, prev_dir);
                            perror("No such file or directory\n");
                            opendir(prev_dir);
                        }

                        else
                        {
                            strcpy(prev_dir, this_dir);
                        }
                    }
            }
        }   
    }
    closedir(dir);
    return 0;
}