#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netinet/in.h>

#define N 65536
void process_packet(unsigned char* , int, int*, char*);

int main(int argc, char* argv[]) 
{
    int s, fd;
    struct sockaddr_in address;

    if(argc != 2)
    {
        perror("Port required");
        exit(EXIT_FAILURE);
    }

    unsigned char *buffer = (unsigned char *)malloc(N);

    // Создание raw socket
    s = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(s < 0) 
    {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Привязка сокета к интерфейсу
    if(bind(s, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        perror("Ошибка привязки");
        exit(EXIT_FAILURE); 
    }

    fd = open("received_data.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    if (fd == -1) 
    {
        perror("File not opened");
        exit(EXIT_FAILURE);
    }

    while(true) 
    {
        int data_size;
        struct sockaddr saddr;
        socklen_t saddr_size = sizeof(saddr);

        // Получение пакета
        data_size = recvfrom(s, buffer, N, 0, &saddr, &saddr_size);

        if(data_size < 0) 
        {
            perror("Ошибка при получении пакета");
            exit(EXIT_FAILURE); 
        }

        // Обработка пакета
        process_packet(buffer, data_size, &fd, argv[1]);
    }

    free(buffer);
    close(s);
    close(fd);
    exit(EXIT_SUCCESS);
}

void process_packet(unsigned char* buffer, int size, int *fd, char *p) 
{
    char data[N];
    struct iphdr *iph = (struct iphdr*)buffer;
    struct sockaddr_in source, dest;
    int port = atoi(p);
    bzero(&source, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    bzero(&dest, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    if(port == ntons(source.sin_port) && port == ntons(dest.sin_port))
    {
        printf("Захвачен IP пакет: %s -> %s\n", inet_ntoa(source.sin_addr), inet_ntoa(dest.sin_addr));

        sprintf(data, "Захвачен IP пакет: %s -> %s\n", inet_ntoa(source.sin_addr), inet_ntoa(dest.sin_addr));
        write(*fd, data, strlen(data)+1);
    }
}