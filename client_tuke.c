#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>



#define SA struct sockaddr
void error(char *msg)
{ //tato funkcia sa vyuziva ked systemove volanie zlyha
    perror(msg); //vypise spravu o chybe a ukonci program server
    exit(0);
}


void charArrayToBinary(char *input, int length, char *output) {
    for (int i = 0; i < length; i++) {
        for (int j = 7; j >= 0; j--) {
            output[i * 8 + (7 - j)] = (input[i] & (1 << j)) ? '1' : '0';
        }
    }
    output[length * 8] = '\0'; // Завершение строки нулевым символом
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n; //pomocne premenne
    struct sockaddr_in serv_addr; //obsahuje adresu servera
    struct hostent *server; //informacie o vzdialenom pocitaci
    char buffer[256]; //buffer pre ulozenie znakov zo socketu
    if (argc < 3) { //kontrola poctu argumentov
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]); //cislo portu servera
//vytvorenie socketu
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    else printf("Socket successfully open\n");
//hostname pc server

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);}
    else printf("Host find\n");
//naplnenie struktury sockaddr_in
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length); //function shall copy n bytes from the area pointed to by s1 to the area pointed to by s2.
    serv_addr.sin_port = htons(portno);
//vytvorenie spojenia
    if (connect(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    else printf("Connection successfull\n");




     //   while ((buff[n++] = getchar()) != '\n')

//spraovanie dat while ((buff[n++] = getchar()) != '\n');
    while(1){
        printf("Me: ");


      //char to binary and reading
        char input[256];
        fgets(input, 255, stdin);
        int length = strlen(input);

        // Переводим массив char в двоичный код
        char binaryOutput[length * 8 + 1]; // Плюс 1 для нулевого символа
        charArrayToBinary(input, length, binaryOutput);

      //end of char to binary

        n = write(sockfd, binaryOutput, length);	//Write to server
        if(n<0)
            error("Error writing to server\n");
        n = read(sockfd, buffer, 255);	//Read incoming data streams
        if(n<0)
            error("Error reading from Server");
        printf("Server : %s \n", buffer);
        if(strncmp(buffer, "Bye", 3) == 0)
            break;
    }

    close(sockfd);
}

