# Dokumentácia pre server s kryptografickou ochranou komunikácie ASCON+Curve25519

## Popis programu

Tento program implementuje serverovú časť chráneného klient-server spojenia s použitím algoritmu generovania súkromných kľúčov pomocou Curve25519 výmeny kľúčov podľa algoritmu Diffie-Hellman (DH), následne vytvára spoločný kľúč pomocou Curve25519 a šifrovanie pomocou algoritmu AEAD (Authenticated Encryption with Associated Data) s použitím ASCON128a. Program podporuje prácu na operačných systémoch Windows aj Linux.

### Kľúčové vlastnosti:
- Inicializácia a správa kontextu pre serverovú časť.
- Generovanie súkromných kľúčov pomocou Curve25519.
- Výmena kľúčov s klientom s použitím algoritmu Diffie-Hellman.
- Generovanie spoločného kľúča pomocou Curve25519.
- Chránený prenos dát cez TCP-socket.
- Šifrovanie a dešifrovanie správ pomocou algoritmu ASCON128a.
- Podpora rôznych platforiem (Windows, Linux).

## Knižnice a závislosti

- **`session.h`** — Obsahuje všetky používané knižnice projektu, vrátane Curve25519, ASCON, generovanie súkromného kľúča pomocou procesora.

## Parametre spustenia

Program prijíma jeden povinný parameter — číslo portu na počúvanie:

./server port

### Parametre:
- **`port`** — Číslo portu na počúvanie prichádzajúcich spojení.

## Inicializácia

Program vykonáva niekoľko krokov na nastavenie socketov a kryptografických operácií:

1. **Inicializácia kontextu** — Inicializuje sa štruktúra `ClientServerContext`, ktorá spravuje parametre a stav serverovej časti.
2. **Inicializácia socketov (Windows/Linux)** — V závislosti od operačného systému sa nastavujú sockety na komunikáciu s klientom. Na Windows sa používa Winsock, na Linuxe — štandardné sockety.
3. **Vytvorenie socketu a priradenie k adrese** — Server vytvára socket a priraďuje ho k IP adrese a portu.
4. **Počúvanie pripojení** — Socket začína počúvať prichádzajúce pripojenia od klientov.

## Výmena kľúčov

1. **Generovanie súkromného kľúča** — Server generuje svoj súkromný kľúč pre algoritmus Diffie-Hellman.
2. **Odovzdanie verejného kľúča klientovi** — Server odovzdáva svoj verejný kľúč klientovi.
3. **Prijatie verejného kľúča od klienta** — Server prijíma verejný kľúč klienta.
4. **Výpočet spoločného tajomstva** — Server používa svoj súkromný kľúč a verejný kľúč klienta na výpočet spoločného tajomstva, ktoré sa bude používať na šifrovanie a dešifrovanie správ.

## Šifrovanie a dešifrovanie

1. **Šifrovanie správ** — Keď server prijme správu od klienta, táto je dešifrovaná pomocou spoločného tajomstva.
2. **Odovzdanie zašifrovaného odpovedi klientovi** — Server šifruje svoju odpoveď a odovzdáva ju klientovi.
3. **Dešifrovanie správ** — Keď server prijme zašifrovanú správu, táto je dešifrovaná a zobrazená na obrazovke.

## Hlavná komunikácia

Program vykonáva hlavný cyklus komunikácie, ktorý zahŕňa:
- Prijímanie zašifrovaných správ od klienta.
- Dešifrovanie správ pomocou spoločného tajomstva.
- Kontrolu príkazu na ukončenie komunikácie ("bye").
- Odpoveď na správu klienta so šifrovaním odpovede a jej odoslaním klientovi.

## Ukončenie práce

Po ukončení komunikácie:
- Uzavrú sa pripojenie s klientom a serverový socket.
- Na Windows sa vyčistia zdroje Winsock.

## Chyby

Ak nastanú chyby v akomkoľvek kroku (vytváranie socketu, priradenie, výmena kľúčov, šifrovanie a dešifrovanie), program vypíše príslušné chybové hlásenie a ukončí vykonávanie s chybovým kódom.

## Príklad použitia

Príklad spustenia programu:

```bash
./server 8080
```

## Sever.c

```c
#include "session.h"

int main(int argc, char *argv[]) {

    // ====================================================================
    // Inicializácia kontextu pre klient-server komunikáciu
    // ====================================================================
    ClientServerContext ctx; // Deklarácia štruktúry
    initializeContext(&ctx); // Inicializácia štruktúry kontextu pre správu
                              // nastavení komunikácie

    // ====================================================================
    // Platformovo špecifická inicializácia socketov pre Windows
    // ====================================================================
    #ifdef _WIN32
        WSADATA wsaData 
      // WSADATA wsaData - štruktúra používaná na uloženie informácií
      // o verzii Winsock knižnice a ďalších údajoch potrebných pre sieťovú komunikáciu vo Windows.
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);  
        
      // MAKEWORD(2, 2) — makro, ktoré vytvorí 16-bitové číslo
      // reprezentujúce verziu Winsock knižnice. Tu je požadovaná verzia 2.2.
         
      // &wsaData — ukazovateľ na štruktúru WSADATA, kam budú zapísané 
      // informácie o verzii a parametroch po inicializácii Winsock.
        
        if (result != 0) {
            fprintf(stderr, "Chyba WSAStartup: %d\n", result);
            exit(1); // Ukončenie programu v prípade chyby inicializácie Winsock
        }
    #endif

    // ====================================================================
    // Kontrola argumentov na číslo portu
    // ====================================================================
    if (argc < 2) {
        fprintf(stderr, "Použitie: %s port\n", argv[0]); 
        // Výpis správy o nesprávnom použití
        
        #ifdef _WIN32
            WSACleanup();  // Vyčistenie Winsock pred ukončením
        #endif
        exit(0); // Ukončenie programu, ak číslo portu nebolo zadané
    }

    ctx.portno = atoi(argv[1]);  // Uloženie čísla portu z argumentov
                                 // príkazového riadku

    // ====================================================================
    // Generovanie náhodného privátneho kľúča pre server
    // ====================================================================
    generate_private_key(ctx.private_key); // Generovanie privátneho kľúča servera

    // Výpis vygenerovaného privátneho kľúča pre ladenie
    printf("Vygenerovaný privátny kľúč pre server: ");
    print_hex(ctx.private_key, 32);

    // ====================================================================
    // Vytvorenie socketu pre server
    // ====================================================================
    #ifdef _WIN32
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Vytvorenie TCP socketu
                                                      // pre Windows
   // socket — systémové volanie, ktoré vytvorí nový socket a vráti
   // jeho deskriptor, ktorý sa potom používa na komunikáciu cez sieťové rozhranie.

   // AF_INET — rodina adries, ktorá označuje použitie IPv4.

   // SOCK_STREAM — typ socketu pre prenos dát prostredníctvom TCP spojení.

   // 0 — OS automaticky vyberie vhodný protokol (pre SOCK_STREAM to je TCP).
   
        if ((unsigned long long)ctx.sockfd ==
            (unsigned long long)INVALID_SOCKET) {
            error("Chyba otvorenia socketu"); // Chyba pri vytváraní socketu
            WSACleanup();  // Vyčistenie Winsock pred ukončením
            exit(1); // Ukončenie pri neúspechu
        }
    #else
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Vytvorenie TCP socketu
                                                      // pre Linux/Unix
        if (ctx.sockfd < 0) {
            error("Chyba otvorenia socketu"); // Chyba pri vytváraní socketu
        }
    #endif

    // ====================================================================
    // Príprava štruktúry adresy servera
    // ====================================================================
    memset((char *)&ctx.serv_addr, 0, sizeof(ctx.serv_addr));  // Vymazanie
                                                               // štruktúry adresy
    ctx.serv_addr.sin_family = AF_INET;  // Použitie internetovej adresnej rodiny
    ctx.serv_addr.sin_addr.s_addr = INADDR_ANY;  // Pripojenie na všetky dostupné
                                                 // sieťové rozhrania
    ctx.serv_addr.sin_port = htons(ctx.portno);  // Nastavenie portu servera
                                                 // (v sieťovom bajtovom poradí)

    // ====================================================================
    // Priradenie socketu k adrese
    // ====================================================================
    int optval = 1; // Nastavenie voľby socketu pre povolenie opätovného použitia adresy
    #ifdef _WIN32
        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR,
                       (const char *)&optval, sizeof(optval))
        == SOCKET_ERROR) {
            error("Chyba setsockopt(SO_REUSEADDR)"); // Chyba nastavenia socketu
            closesocket(ctx.sockfd);
            WSACleanup();  
            exit(1);
        }

        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr,
                 sizeof(ctx.serv_addr)) == SOCKET_ERROR) {
            error("Chyba pri priradení adresy"); 
            closesocket(ctx.sockfd);
            WSACleanup();  
            exit(1);
        }
    #else
        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR,
                       &optval, sizeof(optval)) < 0) {
            error("Chyba setsockopt(SO_REUSEADDR)"); 
            close(ctx.sockfd);  
            exit(1);
        }

        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr,
                 sizeof(ctx.serv_addr)) < 0) {
            error("Chyba pri priradení adresy"); 
            close(ctx.sockfd);  
            exit(1);
        }
    #endif

    // ====================================================================
    // Čakanie na prichádzajúce spojenia od klientov
    // ====================================================================
    #ifdef _WIN32
        if (listen(ctx.sockfd, 5) == SOCKET_ERROR) {
            error("Chyba pri počúvaní"); 
            WSACleanup();  
            exit(1);
        }
    #else
        if (listen(ctx.sockfd, 5) < 0) {
            error("Chyba pri počúvaní"); 
        }
    #endif

    // ====================================================================
    // Prijatie spojenia od klienta
    // ====================================================================
    ctx.clilen = sizeof(ctx.cli_addr);  
    #ifdef _WIN32
        ctx.newsockfd = accept(ctx.sockfd,
                           (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if ((unsigned long long)ctx.newsockfd ==
            (unsigned long long)INVALID_SOCKET) {
            error("Chyba pri prijatí spojenia"); 
            WSACleanup();  
            exit(1);
        }
    #else
        ctx.newsockfd = accept(ctx.sockfd,
                           (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if (ctx.newsockfd < 0) {
            error("Chyba pri prijatí spojenia"); 
        }
    #endif
    printf("Spojenie prijaté\n");

    // ====================================================================
    // Proces výmeny kľúčov podľa Diffie-Hellmana
    // ====================================================================
    unsigned char public_key[32];
    crypto_scalarmult_base(public_key, ctx.private_key);  // Generovanie
                                                          // verejného kľúča servera

    // Odoslanie verejného kľúča servera klientovi
    int n = send(ctx.newsockfd, (char *)public_key, sizeof(public_key), 0);
    if (n < 0) {
        error("Chyba odoslania verejného kľúča klientovi"); 
    }

    // Prijatie verejného kľúča od klienta
    unsigned char client_public_key[32];
    n = recv(ctx.newsockfd, (char *)client_public_key,
             sizeof(client_public_key), 0);
    if (n < 0) {
        error("Chyba prijatia verejného kľúča od klienta"); 
    }

    // Výpis prijatého verejného kľúča klienta
    printf("Prijatý verejný kľúč klienta: ");
    print_hex(client_public_key, 32);

    // Výpočet spoločného tajného kľúča pomocou Diffie-Hellman protokolu
    crypto_scalarmult(ctx.shared_secret, ctx.private_key,
                      client_public_key);

    printf("Spoločný tajný kľúč: ");
    print_hex(ctx.shared_secret, 32);

    // ====================================================================
    // Hlavný cyklus komunikácie s klientom
    // ====================================================================
    while (1) {
        // Čítanie šifrovanej správy od klienta
        memset(ctx.encrypted_msg, 0, sizeof(ctx.encrypted_msg)); 
        #ifdef _WIN32
            n = recv(ctx.newsockfd, (char *)ctx.encrypted_msg,
                   sizeof(ctx.encrypted_msg), 0);
        #else
            n = read(ctx.newsockfd, ctx.encrypted_msg,
                 sizeof(ctx.encrypted_msg));
        #endif
        if (n < 0) error("Chyba čítania od klienta"); 
        ctx.encrypted_msglen = n; 

        // Dešifrovanie prijatej správy pomocou spoločného tajomstva
        if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                                ctx.nsec,
                                ctx.encrypted_msg, ctx.encrypted_msglen,
                                ctx.ad, ctx.adlen,
                                ctx.npub, ctx.shared_secret) != 0) {
            fprintf(stderr, "Chyba dešifrovania\n"); 
            break;
        }
        ctx.decrypted_msg[ctx.decrypted_msglen] = '\0';
        printf("Klient: %s\n", ctx.decrypted_msg);

        // Kontrola, či klient chce ukončiť komunikáciu
        if (strcasecmp((char *)ctx.decrypted_msg, "bye") == 0) {
            printf("Klient ukončil komunikáciu.\n");
            break;
        }

        // Odpoveď servera
        printf("Ja: ");
        memset(ctx.buffer, 0, sizeof(ctx.buffer));  
        if (fgets((char *)ctx.buffer, sizeof(ctx.buffer), stdin) == NULL)
        {
            error("Chyba vstupu"); 
        }

        ctx.bufferlen = strlen((char *)ctx.buffer); 

        // Odstránenie znaku novej riadky na konci
        if (ctx.bufferlen > 0 && ctx.buffer[ctx.bufferlen - 1] == '\n')
        {
            ctx.buffer[ctx.bufferlen - 1] = '\0'; 
        }

        ctx.bufferlen = strlen((char *)ctx.buffer);

        // Šifrovanie odpovede servera
        if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                                (unsigned char *)ctx.buffer,
                                ctx.bufferlen,
                                ctx.ad, ctx.adlen, ctx.nsec, ctx.npub,
                                ctx.shared_secret) != 0) {
            fprintf(stderr, "Chyba šifrovania\n"); 
            break;
        }

        #ifdef _WIN32
        n = send(ctx.newsockfd, (char *)ctx.encrypted_msg,
                 ctx.encrypted_msglen, 0);
        #else
        n = write(ctx.newsockfd, ctx.encrypted_msg,
                  ctx.encrypted_msglen);
        #endif
        if (n < 0) error("Chyba zápisu klientovi");

        if (strcasecmp((char *)ctx.buffer, "bye") == 0) {
            printf("Ukončili ste komunikáciu.\n");
            break;
        }
    }

    // ====================================================================
    // Uzatvorenie socketov a vyčistenie zdrojov
    // ====================================================================
    #ifdef _WIN32
        closesocket(ctx.newsockfd);  
        closesocket(ctx.sockfd);  
        WSACleanup();  
    #else
        close(ctx.newsockfd);  
        close(ctx.sockfd);  
    #endif

    return 0; 
}
