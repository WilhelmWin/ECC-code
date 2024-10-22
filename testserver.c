#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SA struct sockaddr

int main(int argc, char *argv[]) {
    int sockfd, connfd, len, n;
    struct sockaddr_in servaddr, cli;
    char buffer[256];

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // Создаем сокет
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }
    printf("Socket created successfully\n");

    // Инициализация структуры сокета
    bzero(&servaddr, sizeof(servaddr));
    int portno = atoi(argv[1]);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(portno);

    // Привязываем сокет
    if (bind(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Socket bind failed");
        exit(1);
    }
    printf("Socket successfully binded\n");

    // Прослушивание входящих соединений
    if (listen(sockfd, 5) != 0) {
        perror("Listen failed");
        exit(1);
    }
    printf("Server listening...\n");

    len = sizeof(cli);
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        perror("Server accept failed");
        exit(1);
    }
    printf("Client connected\n");

    // Основной цикл общения с клиентом
    while (1) {
        bzero(buffer, 256);
        n = read(connfd, buffer, 255);
        if (n <= 0) {
            perror("Error reading from client or client disconnected");
            break;
        }
        printf("Client: %s", buffer);

        // Если получено "Bye", завершаем соединение
        if (strncmp(buffer, "Bye", 3) == 0) {
            printf("Client has disconnected.\n");
            break;
        }

        // Ответ от сервера
        printf("Me: ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);

        n = write(connfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Error writing to client");
            break;
        }
    }

    close(connfd);
    close(sockfd);
    return 0;
}
