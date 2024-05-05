#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#define BUF_SIZE 1000

int main(int argc, char* argv[])
{
    int sockfd; /* Дескриптор сокета */
    int cli1len, cli2len, n; /* Переменные для различных длин и количества символов */
    socklen_t clntAddrSize;
    char line[BUF_SIZE]; /* Массив для принятой и отсылаемой строки */
    struct sockaddr_in servaddr, clntAddr1, clntAddr2; /* Структуры для адресов сервера и клиента */
    pid_t pid;
    int clientCount = 0;

    if(argc != 2)
    {
        perror("Usage: a.out <Port>\n");
        exit(EXIT_FAILURE);
    }

    // Заполняем структуру для адреса сервера 
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Создаем UDP сокет 
    if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("NULL"); // Печатаем сообщение об ошибке 
        exit(EXIT_FAILURE);
    }

    // Настраиваем адрес сокета 
    if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        perror("NULL");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Предполагаем, что clntAddr1 и clntAddr2 - это адреса двух клиентов
    clntAddrSize = sizeof(clntAddr1);
    recvfrom(sockfd, line, BUF_SIZE, 0, (struct sockaddr*)&clntAddr1, &clntAddrSize);
    clientCount++;

    clntAddrSize = sizeof(clntAddr2);
    recvfrom(sockfd, line, BUF_SIZE, 0, (struct sockaddr*)&clntAddr2, &clntAddrSize);
    clientCount++;

    if (clientCount == 2) 
    {
        switch(pid = fork())
        {
            case -1:
                perror("fork failed");
                exit(EXIT_FAILURE);

            case 0:
                // Пересылка сообщений от clntAddr1 к clntAddr2
                while (true) 
                {
                    int strLen = recvfrom(sockfd, line, BUF_SIZE, 0, (struct sockaddr*)&clntAddr1, &clntAddrSize);
                    
                    if (strLen > 0) 
                        sendto(sockfd, line, strLen, 0, (struct sockaddr*)&clntAddr2, sizeof(clntAddr2));
                }

            default:
                // Пересылка сообщений от clntAddr2 к clntAddr1
                while (true) 
                {
                    int strLen = recvfrom(sockfd, line, BUF_SIZE, 0, (struct sockaddr*)&clntAddr2, &clntAddrSize);

                    if (strLen > 0) 
                        sendto(sockfd, line, strLen, 0, (struct sockaddr*)&clntAddr1, sizeof(clntAddr1));
                }
        }
    }
    close(sockfd);
    
    exit(EXIT_SUCCESS);
}