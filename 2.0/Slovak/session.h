#ifndef SESSION_H
#define SESSION_H

// ========================================================================
// Zahrnutia pre multiplatformovú prácu so soketmi
// ========================================================================
#include <sys/types.h>   // Pre socklen_t (používa sa pre dĺžku soketu)
#include <stdint.h>      // Pre uint8_t a ďalšie pevne definované typy
#include <string.h>      // Pre funkcie ako memset(), memcpy()
#include <stdio.h>       // Pre štandardné I/O funkcie ako printf()
#include <stdlib.h>      // Pre štandardné knižničné funkcie ako malloc()
#include "ECC.h"         // Knižnica pre eliptické krivky (ECC)
#include "ASCON/ascon.h" // Pre ASCON AEAD šifrovanie
#include "error.h"       // Na spracovanie chýb

// ========================================================================
// Platformovo špecifické zahrnutia pre Windows a Unixové systémy
// ========================================================================
#ifdef _WIN32
    #include <winsock2.h>     // Funkcie pre sokety vo Windows
typedef int socklen_t;
    #include <windows.h>      // Špecifické funkcie Windows
#else
    #include <arpa/inet.h>    // Funkcie pre IP adresy, napr. inet_ntoa()
    #include <netinet/in.h>   // Štruktúry ako sockaddr_in
    #include <unistd.h>       // Systémové volania: close, read, write
    #include <sys/socket.h>   // Funkcie ako socket(), bind(), atď.
    #include <signal.h>
    #include <netdb.h>        // Funkcie ako gethostbyname()
#endif

// ========================================================================
// Definícia vlastných typov
// ========================================================================
typedef unsigned char uch;     // Skratka pre unsigned char
typedef unsigned long long ullh; // Skratka pre unsigned long long

// ========================================================================
// Konštanty pre veľkosti bufferov, nonce a kľúčov
// ========================================================================
#define BUFFER_SIZE 256           // Veľkosť komunikačného buffera
                                  // (v bajtoch)
#define NONCE_SIZE 16             // Veľkosť nonce
                                  // (pre šifrovanie, 16 bajtov)
#define KEY_SIZE 32               // Veľkosť súkromného kľúča
                                  // (32 bajtov, 256 bitov)
#define SHARED_SECRET_SIZE 32     // Veľkosť zdieľaného tajomstva (256 bitov)

// ========================================================================
// Štruktúra na uchovanie kontextu klient-server
// ========================================================================
typedef struct {
    int portno;                      // Číslo portu
    int sockfd;                      // Deskriptor soketu
    struct sockaddr_in serv_addr;   // Štruktúra adresy servera
    struct hostent *server;         // Informácie o serveri
                                    // (napr. hostname)
    int optval;                      // Možnosť opätovného použitia adresy

    uch client_public_key[KEY_SIZE];  // Verejný kľúč klienta
    uch server_public_key[KEY_SIZE];  // Verejný kľúč servera
    uch public_key[KEY_SIZE];         // Verejný kľúč
                                      // (ak sa používa spoločne)

    uch buffer[BUFFER_SIZE];       // Buffer na odosielanie/prijímanie dát
    uch bufferlen;                 // Dĺžka dát v buffere

    uch private_key[KEY_SIZE];     // Súkromný kľúč (pre ECC)
    uch shared_secret[SHARED_SECRET_SIZE];  // Zdieľané tajomstvo

    uch decrypted_msg[BUFFER_SIZE];  // Buffer dešifrovanej správy
    ullh decrypted_msglen;           // Dĺžka dešifrovanej správy

    uch *nsec;                       // Bezpečnostný parameter
                                     // (môže byť NULL)
    uch encrypted_msg[BUFFER_SIZE];  // Buffer šifrovanej správy
    ullh encrypted_msglen;           // Dĺžka šifrovanej správy

    uch npub[NONCE_SIZE];            // Nonce (pre šifrovanie, 16 bajtov)
    struct sockaddr_in cli_addr;     // Adresa klienta
    socklen_t clilen;                // Dĺžka adresy klienta
    int newsockfd;                   // Soket pre akceptované spojenia
} ClientServerContext;

// ========================================================================
// Hlavičky funkcií
// ========================================================================
void initializeContext(ClientServerContext *ctx);      // Inicializácia
                                                       // kontextu
void generate_private_key(uch private_key[32]);        // Generovanie
                                                       // náhodného
                                                       // súkromného kľúča
void hexdump(const uch *data, size_t length);          // Výpis dát vo
                                                       // formáte hex

#endif // SESSION_H
