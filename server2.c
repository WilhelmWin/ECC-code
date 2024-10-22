#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define SA struct sockaddr
void error(char *msg)
{ //tato funkcia sa vyuziva ked systemove volanie zlyha
    perror(msg); //vypise spravu o chybe a ukonci program server
    exit(0);
}
// Function designed for chat between client and server.
void func(int connfd)
{
    int size;
    char buffer[256];
    int n;
    // infinite loop for chat
    while (1) {
        bzero(buffer, 256);

        // read the message from client and copy it in buffer
        read(connfd, buffer, sizeof(buffer));
        size=strlen(buffer);    
        // print buffer which contains the client contents
        printf("From client: %sTo client : \nBit: %d\n", buffer, size);
        bzero(buffer, 256);
        n = 0;
        // copy server message in the buffer
        fgets(buffer, 256, stdin);
        if (buffer[n]!='\n') {
            n = write(connfd, buffer, strlen(buffer));
        }

   
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buffer, 4) == 0) {
            printf("\nServer Exit...\n");
            break;
        }
    }
}
   
// Driver function
int main(int argc, char *argv[])
{
    int sockfd, connfd, len, portno;
    struct sockaddr_in servaddr, cli;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // domain, type, protocol
    if (sockfd == -1) {
        error("Socket creation failed:");
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr)); //like memset
   
    // assign IP, PORT
    portno = atoi(argv[1]);// making from char=>int
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // convert value between host and network byte
    servaddr.sin_port = htons(portno); // unsigned short integer netshort from Network byte order
   
    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        error("Socket bind failed:");
    }
    else
        printf("Socket successfully binded..\n");
   
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        error("Listen failed:");
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
    printf("Write 'exit' to close conection or press 'Enter' to waiting client\n");
char buff[100];
char str[100]="exit\n";
    fgets(buff, 100, stdin);
    if (strcmp(buff,str)==0){
        exit(0);
    }
    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);

    if (connfd < 0) {
        error("Server accept failed:");
    }
    else
        printf("Server accept the client:\n");
   
    // Function for chatting between client and server
    func(connfd);
   
    // After chatting close the socket
    close(sockfd);
}
