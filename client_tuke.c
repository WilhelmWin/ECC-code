#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
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
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    printf("Socket successfully open\n");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    printf("Host found\n");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    printf("Connection successful\n");

    // --- Обмен ключами по Диффи-Хеллману ---
    int private_key = 6;  // Приватный ключ клиента (секретное число)
    int public_key = powerMod(G, private_key, P);  // Публичный ключ клиента

    // Отправляем публичный ключ серверу
    n = write(sockfd, &public_key, sizeof(public_key));
    if (n < 0) error("Error sending public key");

    // Получаем публичный ключ сервера
    int server_public_key;
    n = read(sockfd, &server_public_key, sizeof(server_public_key));
    if (n < 0) error("Error receiving public key from server");

    // Вычисляем общий секретный ключ
    int shared_secret = powerMod(server_public_key, private_key, P);
    printf("Shared secret key: %d\n", shared_secret);

    // --- Основной цикл общения с сервером ---
    while (1) {
        printf("Me: ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);

        // Шифруем сообщение перед отправкой
        char encrypted_msg[256];
        encryptDecrypt(buffer, encrypted_msg, shared_secret);

        n = write(sockfd, encrypted_msg, strlen(encrypted_msg));
        if (n < 0) error("Error writing to server");

        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        if (n < 0) error("Error reading from server");

        // Расшифровываем сообщение от сервера
        char decrypted_msg[256];
        encryptDecrypt(buffer, decrypted_msg, shared_secret);
        printf("Server: %s\n", decrypted_msg);

        if (strncmp(decrypted_msg, "Bye", 3) == 0)
            break;
    }

    close(sockfd);
    return 0;
}
