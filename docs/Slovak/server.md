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
    initializeContext(&ctx); // Inicializácia štruktúry kontextu na správu
                              // nastavení komunikácie

    // ====================================================================
    // Platformovo špecifická inicializácia socketov pre Windows
    // ====================================================================
    #ifdef _WIN32
        WSADATA wsaData;
      // WSADATA wsaData - je štruktúra používaná na uloženie 
      // informácií o verzii knižnice Winsock a ďalších údajov,
      // potrebných pre prácu so sieťovými pripojeniami vo Windows.
        
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);  
        
      //  MAKEWORD(2, 2) — makro, ktoré vytvára 16-bitové číslo 
      // reprezentujúce verziu Winsock. V tomto prípade 
      // je požadovaná verzia Winsock 2.2 
      // (hlavná verzia 2 a vedľajšia verzia 2).
         
      // &wsaData — ukazovateľ na štruktúru WSADATA, kde budú uložené 
      // informácie o verzii a ďalších parametroch knižnice Winsock po inicializácii.
        
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
        exit(0); // Ukončenie, ak číslo portu nebolo zadané
    }

    ctx.portno = atoi(argv[1]);  // Uloženie čísla portu z argumentov
                                 // príkazového riadka

    // ====================================================================
    // Generovanie náhodného privátneho kľúča pre server
    // ====================================================================
    generate_private_key(ctx.private_key); // Generovanie privátneho kľúča servera

    // Výpis vygenerovaného privátneho kľúča pre ladenie
    printf("Vygenerovaný privátny kľúč pre server: ");
    print_hex(ctx.private_key, 32);
    // ====================================================================
    // Vytvorenie serverového socketu
    // ====================================================================
    #ifdef _WIN32
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Vytvorenie TCP socketu
                                                     // vo Windows
   // socket — systémové volanie, ktoré vytvára nový socket a vracia 
   // jeho deskriptor (identifikátor), ktorý sa následne používa
   // na sieťovú komunikáciu.

   // AF_INET — rodina adries pre socket. AF_INET znamená IPv4.

   // SOCK_STREAM — typ socketu. SOCK_STREAM určuje, že pôjde o 
   // spojenie so zaručeným doručením (TCP).

   // 0 — protokol. Ak je 0, operačný systém automaticky vyberie 
   // vhodný protokol (v tomto prípade TCP).
   
        if ((unsigned long long)ctx.sockfd ==
            (unsigned long long)INVALID_SOCKET) {
            error("Chyba otvorenia socketu"); // Chyba otvorenia socketu
            WSACleanup();  // Vyčistenie Winsock pred ukončením
            exit(1); // Ukončenie pri zlyhaní vytvorenia socketu
        }
    #else
        ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0); // Vytvorenie TCP socketu
                                                     // v Linux/Unix
        if (ctx.sockfd < 0) {
            error("Chyba otvorenia socketu"); // Chyba otvorenia socketu
        }
    #endif

    // ====================================================================
    // Príprava štruktúry serverovej adresy
    // ====================================================================
    memset((char *)&ctx.serv_addr, 0, sizeof(ctx.serv_addr));  // Vymazanie
                                                               // štruktúry adresy
    ctx.serv_addr.sin_family = AF_INET;  // Použitie rodiny internetových adries
    ctx.serv_addr.sin_addr.s_addr = INADDR_ANY;  // Viazanie na všetky dostupné
                                                 // sieťové rozhrania
    ctx.serv_addr.sin_port = htons(ctx.portno);  // Nastavenie portu servera
                                                 // (v sieťovom byteorderi)

    // ====================================================================
    // Viazanie socketu na adresu
    // ====================================================================
    int optval = 1; // Nastavenie socketovej voľby na umožnenie
                    // opätovného použitia adresy
    #ifdef _WIN32
    
   // ctx.sockfd — deskriptor socketu, ku ktorému sa priraďujú voľby.

   // SOL_SOCKET — úroveň socketu.

   // SO_REUSEADDR — možnosť opätovného použitia adresy a portu.

        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR,
                       (const char *)&optval, sizeof(optval))
        == SOCKET_ERROR) {
            error("Chyba setsockopt(SO_REUSEADDR)"); // Chyba nastavenia
                                                      // socketových volieb
            closesocket(ctx.sockfd);
            WSACleanup();  // Vyčistenie Winsock pred ukončením
            exit(1);
        }

        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr,
                 sizeof(ctx.serv_addr)) == SOCKET_ERROR) {
            error("Chyba pri viazaní"); // Chyba viazania socketu
            closesocket(ctx.sockfd);
            WSACleanup();  // Vyčistenie Winsock pred ukončením
            exit(1);
        }
    #else
        if (setsockopt(ctx.sockfd, SOL_SOCKET, SO_REUSEADDR,
                       &optval, sizeof(optval)) < 0) {
            error("Chyba setsockopt(SO_REUSEADDR)"); // Chyba nastavenia
                                                     // socketových volieb
            close(ctx.sockfd);  // Zatvorenie socketu v Linux/Unix
            exit(1);
        }

        if (bind(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr,
                 sizeof(ctx.serv_addr)) < 0) {
            error("Chyba pri viazaní"); // Chyba viazania socketu
            close(ctx.sockfd);  // Zatvorenie socketu v Linux/Unix
            exit(1);
        }
    #endif

    // ====================================================================
    // Čakanie na prichádzajúce spojenia od klientov
    // ====================================================================
    #ifdef _WIN32
        if (listen(ctx.sockfd, 5) == SOCKET_ERROR) {
            error("Chyba pri počúvaní"); // Chyba počúvania spojení
            WSACleanup();  // Vyčistenie Winsock pred ukončením
            exit(1);
        }
    #else
        if (listen(ctx.sockfd, 5) < 0) {
            error("Chyba pri počúvaní"); // Chyba počúvania spojení
        }
    #endif

    // ====================================================================
    // Prijatie spojenia od klienta
    // ====================================================================
    ctx.clilen = sizeof(ctx.cli_addr);  // Nastavenie veľkosti adresy klienta
    #ifdef _WIN32
        ctx.newsockfd = accept(ctx.sockfd,
                           (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if ((unsigned long long)ctx.newsockfd ==
            (unsigned long long)INVALID_SOCKET) {
            error("Chyba pri prijatí spojenia"); // Chyba prijatia spojenia
            WSACleanup();
            exit(1);
        }
    #else
        ctx.newsockfd = accept(ctx.sockfd,
                           (struct sockaddr *)&ctx.cli_addr, &ctx.clilen);
        if (ctx.newsockfd < 0) {
            error("Chyba pri prijatí spojenia"); // Chyba prijatia spojenia
        }
    #endif
    printf("Spojenie prijaté\n");

    // ====================================================================
    // Výmena kľúčov pomocou algoritmu Diffie-Hellman
    // ====================================================================
    diffie_hellman(ctx); // Funkcia na výmenu kľúčov medzi serverom a klientom
                         // cez Diffie-Hellman protokol
                         //
                         // Vytvorí sa spoločný tajný kľúč na šifrovanie komunikácie

    // ====================================================================
    // Inicializácia šifrovacieho a dešifrovacieho kontextu
    // ====================================================================
    initialize_encryption_context(&ctx); 
    // Inicializácia kryptografických operácií — 
    // nastavenie kľúčov, režimu šifrovania, atď.

    // ====================================================================
    // Hlavná slučka pre komunikáciu
    // ====================================================================
    while (1) {
        // Vymazanie buffera pre prijaté dáta
        memset(ctx.buffer, 0, BUFFER_SIZE);

        // Prijatie správy od klienta
        #ifdef _WIN32
            ctx.n = recv(ctx.newsockfd, ctx.buffer, BUFFER_SIZE, 0);
            if (ctx.n == SOCKET_ERROR) {
                error("Chyba pri prijímaní správy"); // Chyba prijatia správy
                break;
            } else if (ctx.n == 0) {
                printf("Klient ukončil spojenie\n"); // Klient ukončil spojenie
                break;
            }
        #else
            ctx.n = recv(ctx.newsockfd, ctx.buffer, BUFFER_SIZE, 0);
            if (ctx.n < 0) {
                error("Chyba pri prijímaní správy"); // Chyba prijatia správy
                break;
            } else if (ctx.n == 0) {
                printf("Klient ukončil spojenie\n"); // Klient ukončil spojenie
                break;
            }
        #endif

        // Dešifrovanie prijatej správy
        decrypt_message(&ctx, (unsigned char*)ctx.buffer, ctx.n);

        // Zobrazenie prijatej a dešifrovanej správy
        printf("Klient: %s\n", ctx.buffer);

        // Vymazanie buffera pre odpoveď
        memset(ctx.buffer, 0, BUFFER_SIZE);

        // Zadanie odpovede zo servera (zo štandardného vstupu)
        printf("Server: ");
        fgets(ctx.buffer, BUFFER_SIZE, stdin);

        // Šifrovanie odpovede
        encrypt_message(&ctx, (unsigned char*)ctx.buffer, strlen(ctx.buffer));

        // Odoslanie šifrovanej odpovede klientovi
        #ifdef _WIN32
            ctx.n = send(ctx.newsockfd, ctx.buffer, strlen(ctx.buffer), 0);
            if (ctx.n == SOCKET_ERROR) {
                error("Chyba pri odosielaní správy"); // Chyba odoslania správy
                break;
            }
        #else
            ctx.n = send(ctx.newsockfd, ctx.buffer, strlen(ctx.buffer), 0);
            if (ctx.n < 0) {
                error("Chyba pri odosielaní správy"); // Chyba odoslania správy
                break;
            }
        #endif
    }

    // ====================================================================
    // Ukončenie spojenia a vyčistenie zdrojov
    // ====================================================================
    #ifdef _WIN32
        closesocket(ctx.newsockfd); // Zatvorenie socketu pre klienta
        closesocket(ctx.sockfd);    // Zatvorenie hlavného socketu
        WSACleanup();               // Uvoľnenie Winsock knižnice
    #else
        close(ctx.newsockfd); // Zatvorenie socketu pre klienta
        close(ctx.sockfd);    // Zatvorenie hlavného socketu
    #endif

    // Vyčistenie kryptografického kontextu
    cleanup_encryption_context(&ctx);

    return 0;
}
