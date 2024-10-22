#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SA struct sockaddr
#define P 23  // Простое число P (общий параметр для DF)
#define G 5   // Примитивный корень P (общий параметр для DF)

void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Функция для преобразования строки в зашифрованную строку (XOR)
void encryptDecrypt(char *input, char *output, int key) {
    int len = strlen(input);
    for (int i = 0; i < len; i++) {
        output[i] = input[i] ^ key;  // Простейшее XOR шифрование
    }
    output[len] = '\0';  // Завершение строки
}

// Функция для вычисления (base^exp) % mod
int powerMod(int base, int exp, int mod) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result = (result * base) % mod;
    }
    return result;
}

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
    if (sockfd < 0) error("Error opening socket");
    printf("Socket created successfully\n");

    // Инициализация структуры сокета
    bzero(&servaddr, sizeof(servaddr));
    int portno = atoi(argv[1]);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(portno);

    // Привязываем сокет
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0)
        error("Socket bind failed");
    printf("Socket successfully binded\n");

    // Прослушивание входящих соединений
    if ((listen(sockfd, 5)) != 0)
        error("Listen failed");
    printf("Server listening...\n");

    len = sizeof(cli);
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0)
        error("Server accept failed");
    printf("Client connected\n");

    // --- Обмен ключами по Диффи-Хеллману ---
    int private_key = 15;  // Приватный ключ сервера (секретное число)
    int public_key = powerMod(G, private_key, P);  // Публичный ключ сервера

    // Отправляем публичный ключ клиенту
    n = write(connfd, &public_key, sizeof(public_key));
    if (n < 0) error("Error sending public key");
    printf("Sent public key: %d\n", public_key);

    // Получаем публичный ключ клиента
    int client_public_key;
    n = read(connfd, &client_public_key, sizeof(client_public_key));
    if (n < 0) error("Error receiving public key from client");
    printf("Received client's public key: %d\n", client_public_key);

    // Вычисляем общий секретный ключ
    int shared_secret = powerMod(client_public_key, private_key, P);
    printf("Shared secret key: %d\n", shared_secret);

    // --- Основной цикл общения с клиентом ---
    while (1) {
        bzero(buffer, 256);

        // Чтение сообщения от клиента
        n = read(connfd, buffer, 255);
        if (n < 0) error("Error reading from client");

        // Расшифровываем сообщение от клиента
        char decrypted_msg[256];
        encryptDecrypt(buffer, decrypted_msg, shared_secret);
        printf("Client: %s\n", decrypted_msg);

        // Если получено "Bye", завершаем соединение
        if (strncmp(decrypted_msg, "Bye", 3) == 0) {
            printf("Client has disconnected.\n");
            break;
        }

        // Ответ от сервера
        printf("Me: ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);

        // Шифруем сообщение перед отправкой
        char encrypted_msg[256];
        encryptDecrypt(buffer, encrypted_msg, shared_secret);

        n = write(connfd, encrypted_msg, strlen(encrypted_msg));
        if (n < 0) error("Error writing to client");
    }

    close(connfd);
    close(sockfd);
    return 0;
}
