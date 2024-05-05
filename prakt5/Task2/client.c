#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char* argv[]) 
{
    int sockfd; /* Дескриптор сокета */
    int n, len; /* Переменные для различных длин и количества символов */
    char sendline[1000], recvline[1000]; /* Массивы для отсылаемой и принятой строки */
    char connection[] = "connect";
    struct sockaddr_in servaddr, cliaddr; /* Структуры для адресов сервера и клиента */
    pid_t pid;
    
    /* Сначала проверяем наличие второго аргумента в
    командной строке. При его отсутствии ругаемся и прекращаем работу */
    if(argc != 3)
    {
        printf("Usage: a.out <IP address>\n");
        exit(EXIT_FAILURE);
    }

    /* Создаем UDP сокет */
    if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror(NULL); /* Печатаем сообщение об ошибке */
        exit(EXIT_FAILURE);
    }

    /* Заполняем структуру для адреса клиента */
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(0);
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Настраиваем адрес сокета */
    if(bind(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) < 0)
    {
        perror(NULL);
        close(sockfd); /* По окончании работы закрываем дескриптор сокета */
        exit(EXIT_FAILURE);
    }

    /* Заполняем структуру для адреса сервера */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    if(inet_aton(argv[1], &servaddr.sin_addr) == 0) 
    {
        printf("Invalid IP address\n");
        close(sockfd); /* По окончании работы закрываем дескриптор сокета */
        exit(EXIT_FAILURE);
    }

    connect(sockfd, (struct sockaddr* ) &servaddr, sizeof(servaddr));

    if(sendto(sockfd, connection, strlen(connection)+1, 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    switch(pid = fork())
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        
        case 0:
            while(true)
            {
                /* Вводим строку, которую отошлем серверу */
                printf("String => ");
                fgets(sendline, 1000, stdin);

                /* Отсылаем датаграмму */
                if(sendto(sockfd, sendline, strlen(sendline)+1, 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
                {
                    perror(NULL);
                    close(sockfd);
                    kill(getppid(), SIGKILL);
                    exit(EXIT_FAILURE);
                }
            }
            exit(EXIT_SUCCESS);

        default:
            while(true)
            {
                /* Ожидаем ответа и читаем его. Максимальная допустимая длина датаграммы – 1000 символов, адрес отправителя нам не нужен */
                if((n = recvfrom(sockfd, recvline, 1000, 0, (struct sockaddr *) NULL, NULL)) < 0)
                {
                    perror(NULL);
                    close(sockfd);
                    kill(pid, SIGKILL);
                    exit(EXIT_FAILURE);
                }

                /* Печатаем пришедший ответ*/
                printf("Received: %s\n", recvline);
            }
    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}