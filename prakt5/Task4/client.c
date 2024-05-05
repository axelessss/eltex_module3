#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

#define filename "send_file.txt"
#define h_addr h_addr_list[0]

void error(const char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int my_sock, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buff[80];
    int file;
    int bytes_read;
    int arg_num = 0;

    printf("TCP DEMO CLIENT\n");

    if (argc < 3) 
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // извлечение порта
    portno = atoi(argv[2]);

    // Шаг 1 - создание сокета
    my_sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (my_sock < 0) 
        error("ERROR opening socket");

    // извлечение хоста
    server = gethostbyname(argv[1]);

    if (server == NULL) 
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(EXIT_FAILURE);
    }

    // заполнение структуры serv_addr
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);

    // установка порта
    serv_addr.sin_port = htons(portno);

    // Шаг 2 - установка соединения
    if (connect(my_sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    
    n = recv(my_sock, buff, sizeof(buff) - 1, 0);

    printf("\n%s\n", buff);
    // Открытие файла для чтения
    file = open(filename, O_RDONLY);
    if (file == -1) 
    {
        perror("File not opened");
        exit(EXIT_FAILURE);
    }

    // Шаг 3 - чтение и передача сообщений
    while (true)
    {
        // проверка на "quit"
        if (!strcmp(buff, "quit\n")) 
        {
            // Корректный выход
            printf("Exit...");
            close(my_sock);
            exit(EXIT_SUCCESS);
        }

        // Чтение файла и отправка данных через сокет
        if(bytes_read = read(file, buff, sizeof(buff)) <= 0)
            break;

        send(my_sock, buff, sizeof(buff), 0);
    }
    
    printf("Uploaded\n");
    close(my_sock);
    close(file);
    exit(EXIT_SUCCESS);
}