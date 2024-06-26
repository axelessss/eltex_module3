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

int main(int argc, char* argv[])
{
    int sockfd; /* Дескриптор сокета */
    int clilen, n; /* Переменные для различных длин и количества символов */
    char line[1000]; /* Массив для принятой и отсылаемой строки */
    struct sockaddr_in servaddr, cliaddr; /* Структуры для адресов сервера и клиента */

    if(argc != 2)
    {
        perror("Usage: a.out <Port>\n");
        exit(EXIT_FAILURE);
    }

    // Заполняем структуру для адреса сервера 
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));// Порт, указанный в параметрах запуска
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

    while(true)
    {
        // Основной цикл обслуживания
        clilen = sizeof(cliaddr);
        // Ожидаем прихода запроса от клиента и читаем его 
        if((n = recvfrom(sockfd, line, 999, 0, (struct sockaddr *) &cliaddr, &clilen)) <= 0)
        {
            perror(NULL);
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        // Печатаем принятый текст на экране 
        printf("%s\n", line);

        // Принятый текст отправляем обратно по адресу отправителя 
        if(sendto(sockfd, line, strlen(line), 0, (struct sockaddr *) &cliaddr, clilen) <= 0)
        {
            perror(NULL);
            close(sockfd);
            exit(EXIT_FAILURE);
        } // Уходим ожидать новую датаграмму
    }
    exit(EXIT_SUCCESS);
}