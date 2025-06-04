#ifndef SESSION_H
#define SESSION_H

// ========================================================================
// Zahrnutia pre platformovo nezávislú komunikáciu cez sockety
// ========================================================================
#include <sys/types.h>   // Pre socklen_t (používané pre dĺžku socketu)
#include <stdint.h>      // Pre uint8_t a ďalšie typy pevnej šírky
#include <string.h>      // Pre funkcie so stringami ako memset(), memcpy()
#include <stdio.h>       // Pre štandardné I/O funkcie ako printf()
#include <stdlib.h>      // Pre štandardné knižničné funkcie ako malloc()
#include "ECC.h"         // Zahrnutie knižnice eliptických kriviek (ECC)
#include "ASCON/ascon.h" // Pre ASCON AEAD šifrovanie

// ========================================================================
// Platformovo špecifické záhlavia pre Windows a Unix-like systémy
// ========================================================================
#ifdef _WIN32
#include <winsock2.h>    // Pre Windows socket funkcie
#include <windows.h>     // Pre Windows špecifickú funkcionalitu

typedef int socklen_t;

#else
    #include <arpa/inet.h>   // Pre inet_ntoa() a ďalšie IP-funkcie

    #include <netinet/in.h>  // Pre sockaddr_in a ďalšie štruktúry

    #include <unistd.h>      // Pre close, read, write a ďalšie Unix
                             // systémové volania

    #include <sys/socket.h>  // Pre socket funkcie (socket, bind, atď.)
    #include <signal.h>
    #include <netdb.h>       // Pre gethostbyname a ďalšie sieťové funkcie
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_mixer.h>
#endif

// ========================================================================
// Konštanty pre veľkosti bufferov, nonce a kľúčov
// ========================================================================
#define BUFFER_SIZE 256
#define NONCE_SIZE 16
#define KEY_SIZE 32
#define SHARED_SECRET_SIZE 32

// ========================================================================
// Štruktúra na uchovávanie kontextu klient-server komunikácie
// ========================================================================
typedef struct {
    int portno;                        // Číslo portu pre komunikáciu
    int sockfd;                        // Deskriptor socketu
    struct sockaddr_in serv_addr;     // Adresa servera
    struct hostent *server;           // Informácie o serveri (hostname)
    int optval;                       // Socketové voľby

    uint8_t client_public_key[KEY_SIZE];
    uint8_t server_public_key[KEY_SIZE];
    uint8_t public_key[KEY_SIZE];

    uint8_t buffer[BUFFER_SIZE];      // Všeobecný buffer pre komunikáciu
    uint8_t bufferlen;                // Dĺžka platných dát v buffri

    uint8_t private_key[KEY_SIZE];     // ECC súkromný kľúč
    uint8_t shared_secret[SHARED_SECRET_SIZE];  // Zdieľaný kľúč (X25519)

    uint8_t decrypted_msg[BUFFER_SIZE];  // Výstupný buffer pre dešifrované
                                         // správy
    uint64_t decrypted_msglen;           // Dĺžka dešifrovaných dát

    uint8_t *nsec;                       // Voliteľný bezpečnostný parameter
    uint8_t encrypted_msg[BUFFER_SIZE];  // Buffer pre zašifrované správy
    uint64_t encrypted_msglen;           // Dĺžka zašifrovaných dát

    uint8_t npub[NONCE_SIZE];            // Nonce (ASCON, 128-bit)

    struct sockaddr_in cli_addr;         // Pre server, akceptovanie
                                         // klientov
    socklen_t clilen;
    int newsockfd;                      // Prijatý socket klienta
} ClientServerContext;

// ========================================================================
// Prototypy funkcií
// ========================================================================
void initializeContext(ClientServerContext *ctx);   // Funkcia na
                                                    // inicializáciu
                                                    // kontextu
void generate_private_key(uint8_t private_key[32]); // Funkcia na
                                                    // generovanie náhodného
                                                    // súkromného kľúča
void hexdump(const uint8_t *data, size_t length);   // Funkcia na výpis
                                                    // dát v hex formáte
void play_music(const char *music_file, int loops); // prehrávanie hudby

#endif // SESSION_H
