# Dokumentácia pre klienta s kryptografickou ochranou komunikácie ASCON+Curve25519

## Popis programu

Tento program implementuje klientskú časť zabezpečenej komunikácie medzi klientom a serverom s použitím algoritmu generovania privátnych kľúčov pomocou Curve25519 pre výmenu kľúčov podľa algoritmu Diffie-Hellman a následné vytvorenie spoločného kľúča pomocou Curve25519. Komunikácia je šifrovaná pomocou AEAD (Authenticated Encryption with Associated Data) s algoritmom ASCON128a. Program podporuje prevádzku na operačných systémoch Windows aj Linux.

### Kľúčové vlastnosti:
- Inicializácia a správa kontextu pre klientsku časť.
- Generovanie privátnych kľúčov pomocou Curve25519.
- Výmena kľúčov so serverom pomocou algoritmu Diffie-Hellman.
- Vytvorenie spoločného kľúča pomocou Curve25519.
- Zabezpečený prenos dát cez TCP-socket.
- Šifrovanie a dešifrovanie správ s použitím algoritmu ASCON128a.
- Podpora rôznych platforiem (Windows, Linux).

## Knižnice a závislosti

- **session.h** — Obsahuje všetky knižnice použité v projekte, vrátane Curve25519, ASCON, generovanie privátneho kľúča pomocou procesora.

## Parametre spustenia

Program prijíma dva povinné parametre — názov hostiteľa (IP adresa servera) a číslo portu na pripojenie:

```bash
./client hostname port
```

## Parametre:

- **hostname** — IP adresa servera, ku ktorému sa klient pripája.
- **port** — Číslo portu na pripojenie k serveru.

## Inicializácia

Program vykonáva niekoľko krokov na nastavenie socketov a kryptografických operácií:

1. **Inicializácia kontextu** — Inicializuje sa štruktúra `ClientServerContext`, ktorá spravuje parametre a stav klientskej časti.

2. **Inicializácia socketov (Windows/Linux)** — V závislosti od operačného systému sa nastavujú sokety na komunikáciu so serverom. Na Windows sa používa Winsock, na Linuxe — štandardné sokety.

3. **Kontrola vstupných argumentov** — Program kontroluje správnosť zadaných argumentov (názov hostiteľa a port).

4. **Generovanie privátneho kľúča** — Klient generuje svoj privátny kľúč pre algoritmus Diffie-Hellman.

5. **Vytvorenie TCP-socketu a pripojenie k serveru** — Vytvorí sa socket a klient sa pokúša pripojiť k serveru.

## Výmena kľúčov

1. **Generovanie verejného kľúča** — Klient generuje svoj verejný kľúč pomocou algoritmu X25519.

2. **Odoslanie verejného kľúča serveru** — Klient odosiela svoj verejný kľúč na server.

3. **Prijatie verejného kľúča od servera** — Klient prijíma verejný kľúč od servera.

4. **Výpočet spoločného tajomstva** — Klient používa svoj privátny kľúč a verejný kľúč servera na výpočet spoločného tajomstva, ktoré sa bude používať na šifrovanie a dešifrovanie správ.

## Šifrovanie a dešifrovanie

1. **Šifrovanie správ** — Keď klient zadá správu, táto sa zašifruje pomocou spoločného tajomstva.

2. **Odoslanie zašifrovanej správy serveru** — Klient odosiela zašifrovanú správu na server.

3. **Prijatie zašifrovanej odpovede od servera** — Klient prijíma zašifrovanú odpoveď od servera.

4. **Dešifrovanie odpovede** — Klient dešifruje odpoveď od servera pomocou spoločného tajomstva a zobrazuje ju na obrazovke.

## Hlavná slučka komunikácie

Program vykonáva hlavnú slučku komunikácie, ktorá zahŕňa:

1. Zadanie správ používateľom a ich šifrovanie.

2. Odoslanie zašifrovaných správ serveru.

3. Prijatie a dešifrovanie odpovede od servera.

4. Kontrolu príkazu na ukončenie komunikácie ("bye").

5. Ukončenie komunikácie pri prijatí príkazu "bye" od klienta alebo servera.

## Ukončenie práce

Po ukončení komunikácie:

1. Zatvorí sa spojenie so serverom a klientský socket.

2. Pre Windows sa vyčistia prostriedky Winsock.

## Chyby

Ak nastanú chyby v akomkoľvek kroku (vytvorenie socketu, pripojenie, výmena kľúčov, šifrovanie a dešifrovanie), program vypíše príslušné chybové hlásenie a ukončí vykonávanie s chybovým kódom.

## Príklad použitia

Príklad spustenia programu:

```bash
./client 127.0.0.1 8080
```

## Client.c

```c

#include "session.h"

int main(int argc, char *argv[]) {

    // ====================================================================
    // Inicializácia kontextu pre klient-server komunikáciu
    // ====================================================================
    ClientServerContext ctx;
    initializeContext(&ctx);  // Inicializácia štruktúry kontextu na správu
                              // nastavení komunikácie

    // ====================================================================
    // Inicializácia pre Windows (Winsock)
    // ====================================================================
#ifdef _WIN32
    WSADATA wsaData;
    // Inicializácia Winsock pre Windows
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Nepodarilo sa inicializovať Winsock\n");
        return 1;  // Ukončenie programu, ak sa nepodarí inicializovať Winsock
    }
#endif

    // ====================================================================
    // Kontrola vstupných argumentov (hostname a port)
    // ====================================================================
    if (argc < 3) {
        fprintf(stderr, "Použitie %s hostname port\n", argv[0]);  // Výpis
                                                                  // inštrukcie
        exit(0);  // Ukončenie programu, ak sú argumenty neplatné
    }

    // ====================================================================
    // Generovanie súkromného kľúča pomocou Curve25519
    // ====================================================================
    generate_private_key(ctx.private_key);  // Generovanie súkromného kľúča
                                            // pomocou Curve25519
    printf("Vygenerovaný súkromný kľúč pre klienta: ");
    print_hex(ctx.private_key, 32);  // Výpis vygenerovaného súkromného kľúča v
                                     // hexadecimálnom formáte

    // ====================================================================
    // Prevod čísla portu z reťazca na celé číslo
    // ====================================================================
    ctx.portno = atoi(argv[2]);  // Prevod argumentu portu (reťazec)
                                 // na celé číslo

    // ====================================================================
    // Vytvorenie TCP-soketu
    // ====================================================================
    ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0);  // Vytvorenie IPv4 TCP
                                                   // soketu
    if (ctx.sockfd < 0) {
        error("Chyba pri otváraní soketu");  // Ak sa soket nevytvorí, zobrazí
                                              // sa chybové hlásenie
    }
    printf("Soket úspešne otvorený\n");

    // ====================================================================
    // Rozlíšenie názvu hostiteľa na IP adresu
    // ====================================================================
    ctx.server = gethostbyname(argv[1]);  // Prevod názvu hostiteľa na
                                          // IP adresu
    if (ctx.server == NULL) {
        fprintf(stderr, "Chyba, taký hostiteľ neexistuje\n");  // Zobrazenie
                                                              // chybového
                                                              // hlásenia, ak hostiteľ
                                                              // neexistuje
        exit(0);  // Ukončenie programu, ak sa nepodarí získať IP adresu hostiteľa
    }
    printf("Hostiteľ nájdený\n");

    // ====================================================================
    // Príprava štruktúry pre adresu servera
    // ====================================================================
    memset((char *)&ctx.serv_addr, 0, sizeof(ctx.serv_addr));  // Vymazanie
                                                              // štruktúry
                                                              // adresy servera

    ctx.serv_addr.sin_family = AF_INET;  // Nastavenie rodiny adries na IPv4
    memcpy((char *)&ctx.serv_addr.sin_addr.s_addr,
           (char *)ctx.server->h_addr,
           ctx.server->h_length);  // Kopírovanie IP adresy servera do štruktúry
    ctx.serv_addr.sin_port = htons(ctx.portno);  // Prevod čísla portu do
                                                 // sieťového poriadku
                                                 // bajtov a jeho uloženie

    // ====================================================================
    // Nadviazanie spojenia so serverom
    // ====================================================================
    if (connect(ctx.sockfd,(struct sockaddr *)&ctx.serv_addr,
                sizeof(ctx.serv_addr)) < 0) {

        error("Chyba pri pripojení");  // Pokus o pripojenie k serveru; výpis
                                       // chyby, ak sa pripojenie nepodarí
    }
    printf("Pripojenie úspešné\n");

    // ====================================================================
    // Vykonanie výmeny kľúčov Diffie-Hellman (X25519)
    // ====================================================================
    unsigned char public_key[32];  // Buffor pre verejný kľúč klienta
    crypto_scalarmult_base(public_key, ctx.private_key);  // Generovanie
                                                          // verejného
                                                          // kľúča klienta
                                                          // pomocou X25519

    // Odoslanie verejného kľúča na server
    #ifdef _WIN32
    int n = send(ctx.sockfd, (const char *)public_key, sizeof(public_key),
               0);  // Prevod na const char *
#else
   int n = write(ctx.sockfd, public_key, sizeof(public_key));
#endif
    // Odoslanie vygenerovaného verejného kľúča serveru
    if (n < 0) {
        error("Chyba pri odosielaní verejného kľúča");  // Kontrola úspešnosti
                                                        // odoslania verejného kľúča
    }

    // Prijatie verejného kľúča od servera
    unsigned char server_public_key[32];
    n = recv(ctx.sockfd, (char *)server_public_key,
             sizeof(server_public_key), 0);  // Prijatie verejného kľúča servera
    if (n < 0) {
        error("Chyba pri prijímaní verejného kľúča od servera");
        // Kontrola úspešnosti prijatia verejného kľúča
    }

    // Výpis prijatého verejného kľúča servera (pre debugovanie)
    printf("Prijatý verejný kľúč servera: ");
    print_hex(server_public_key, 32);  // Výpis prijatého verejného kľúča servera

    // Výpočet spoločného tajomstva pomocou výmeny kľúčov Diffie-Hellman
    crypto_scalarmult(ctx.shared_secret, ctx.private_key,
                      server_public_key);  // Výpočet spoločného tajomstva na základe
                                            // súkromného kľúča klienta a verejného
                                            // kľúča servera

    printf("Spoločné tajomstvo: ");
    print_hex(ctx.shared_secret, 32);  // Výpis spoločného tajomstva

    // ====================================================================
    // Spustenie cyklu výmeny zašifrovaných správ
    // ====================================================================
    while (1) {
        // Získanie vstupu od používateľa
        printf("Ja: ");
        memset(ctx.buffer, 0, sizeof(ctx.buffer));  // Vymazanie bufforu
                                                    // pre uloženie
                                                    // správy používateľa
        if (fgets((char *)ctx.buffer, sizeof(ctx.buffer), stdin) == NULL)
        {
            error("Chyba pri čítaní vstupu");  // Čítanie vstupu z klávesnice,
                                               // kontrola na chybu
        }

        // Odstránenie znaku nového riadku, ak je prítomný
        size_t len = strlen((char *)ctx.buffer);
        if (len > 0 && ctx.buffer[len - 1] == '\n') {
            ctx.buffer[len - 1] = '\0';  // Odstránenie znaku nového riadku zo
                                         // vstupného reťazca
        }
        ctx.bufferlen = strlen((char *)ctx.buffer);  // Uloženie dĺžky
                                                     // správy

        // Šifrovanie správy
        if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                                ctx.buffer, ctx.bufferlen,
                                ctx.ad, ctx.adlen, ctx.nsec, ctx.npub,
                                ctx.shared_secret) != 0) {
            fprintf(stderr, "Chyba šifrovania\n");  // Ak sa šifrovanie nepodarí,
                                                    // zobrazí sa chybové hlásenie
            return 1;
        }

        // Odoslanie zašifrovanej správy
#ifdef _WIN32
n = send(ctx.sockfd, (const char *)ctx.encrypted_msg,
                 ctx.encrypted_msglen, 0);  // Odoslanie zašifrovanej správy
                                            // na Windows
#else
        n = write(ctx.sockfd, ctx.encrypted_msg, ctx.encrypted_msglen);
// Odoslanie zašifrovanej správy na Linux/Unix
#endif
        if (n < 0) error("Chyba pri zápise na server");  // Kontrola na chyby
                                                          // pri odosielaní

        // Ak klient zadal "bye", ukončíme komunikáciu
        if (strcasecmp((char *)ctx.buffer, "bye") == 0) {
            printf("Ukončili ste rozhovor.\n");
            break;  // Výstup z cyklu, ak klient zadal "bye"
        }

        // Prijatie zašifrované odpovede od servera
        memset(ctx.encrypted_msg, 0, sizeof(ctx.encrypted_msg));
// Vymazanie bufforu zašifrovanej správy
#ifdef _WIN32
n = recv(ctx.sockfd, (char *)ctx.encrypted_msg,
             sizeof(ctx.encrypted_msg), 0);  // Prijatie zašifrovanej
// odpovede od servera na Windows
#else
        n = read(ctx.sockfd, ctx.encrypted_msg,
             sizeof(ctx.encrypted_msg));  // Prijatie zašifrovanej
   // odpovede od servera na Linux/Unix
#endif
        if (n < 0) error("Chyba pri čítaní zo servera");
// Kontrola na chyby pri prijímaní
        ctx.encrypted_msglen = n;  // Uloženie skutočnej dĺžky prijatých dát

        // Dešifrovanie odpovede
        if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                                ctx.nsec,
                                ctx.encrypted_msg, ctx.encrypted_msglen,
                                ctx.ad, ctx.adlen,
                                ctx.npub, ctx.shared_secret) != 0) {
            fprintf(stderr, "Chyba dešifrovania\n");  // Ak sa dešifrovanie
                                                    // nepodarí, zobrazí
                                                    // sa chybové hlásenie
            return 1;
        }

        ctx.decrypted_msg[ctx.decrypted_msglen] = '\0';  // Ukončenie reťazca
                                                         // dešifrovanej
                                                       // správy
        printf("Server: %s\n", ctx.decrypted_msg);  // Výpis dešifrovanej
                                                    // odpovede od servera

        // Ak server zadal "bye", ukončíme komunikáciu
        if (strcasecmp((char *)ctx.decrypted_msg, "bye") == 0) {
            printf("Server ukončil rozhovor.\n");
            break;  // Výstup z cyklu, ak server zadal "bye"
        }
    }

    // ====================================================================
    // Vyčistenie a zatvorenie soketu
    // ====================================================================
#ifdef _WIN32
    closesocket(ctx.sockfd);  // Zatvorenie soketu na Windows
    WSACleanup();             // Vyčistenie zdrojov Winsock
#else
    close(ctx.sockfd);        // Zatvorenie soketu na Linux/Unix
#endif

    return 0;  // Úspešný výstup z programu
}
