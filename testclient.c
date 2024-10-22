#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SA struct sockaddr

int main(int argc, char *argv[]) {
    int sockfd, n;
    struct sockaddr_in servaddr;
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
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
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));

    // Преобразуем адрес хоста
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(1);
    }

    // Подключаемся к серверу
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection Failed");
        exit(1);
    }
    printf("Connected to server\n");

    // Основной цикл общения с сервером
    while (1) {
        // Ввод сообщения от клиента
        printf("Me: ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);

        // Отправляем сообщение серверу
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Error writing to server");
            break;
        }

        // Если отправлено "Bye", завершаем соединение
        if (strncmp(buffer, "Bye", 3) == 0) {
            printf("Disconnected from server.\n");
            break;
        }

        // Чтение ответа от сервера
        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        if (n <= 0) {
            perror("Error reading from server or server disconnected");
            break;
        }
        printf("Server: %s", buffer);
    }

    close(sockfd);
    return 0;
}
