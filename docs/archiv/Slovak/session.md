# Dokumentácia pre súbor `Session`

## Prehľad

Táto sekcia má za cieľ vysvetliť kód v **`session.h`** a **`session.c`**.

### Kľúčové vlastnosti tohto kódu:
- Zahrnutie systémových knižníc v C.
- Zahrnutie knižníc špecifických pre platformu na prácu so soketmi pre Windows a Linux.
- Definovanie veľkostí pomocou `#define`.
- Definovanie štruktúry obsahujúcej všetky premenné používané klientom a serverom.
- Funkcie na inicializáciu štruktúry.
- Funkcie na výpis kľúčov v hexadecimálnom formáte.
- Funkcia na spracovanie chýb.
- Funkcia na generovanie kľúčov pomocou funkcie `drng` zo súboru.
- Funkcia na výpis dát v hexadecimálnom formáte.

## Súbor: `session.h`

### Použité knižnice:
- **`netinet/in.h`**: Pre štruktúry soketov a funkcie na systémoch podobných Unixu.
- **`sys/types.h`, `sys/socket.h`, `unistd.h`**: Pre soketové operácie a systémové volania na systémoch podobných Unixu.
- **`stdint.h`**: Pre typy celých čísel s pevnou šírkou ako `uint8_t`.
- **`ECC.h`**: Knižnica pre kryptografiu Curve25519.
- **`ASCON/crypto_aead.h`**: Knižnica pre šifrovanie ASCON.
- **`string.h`**: Pre funkcie na manipuláciu so reťazcami ako `memset()`, `memcpy()`.
- **`stdio.h`**: Pre štandardné I/O funkcie ako `printf()`.
- **`stdlib.h`**: Pre funkcie štandardnej knižnice ako `malloc()`.
- **`winsock2.h`**: Pre soketové operácie na Windows.
- **`arpa/inet.h`**: Pre operácie s IP na systémoch podobných Unixu.

### Platformové závislosti:
- **Windows**: Vyžaduje `winsock2.h` a `windows.h` pre soketové operácie a systémové funkcie.
- **Systémy podobné Unixu**: Vyžaduje `arpa/inet.h` a `unistd.h` pre soketové operácie a systémové volania.

---

## Konštanty a definície

### Veľkosti bufferov a kľúčov:
- **`BUFFER_SIZE`**: Definuje veľkosť bufferu pre komunikáciu (256 bajtov).
- **`NONCE_SIZE`**: Definuje veľkosť nonce (16 bajtov).
- **`PRIVATE_KEY_SIZE`**: Definuje veľkosť privátneho kľúča pre ECC (32 bajty).
- **`SHARED_SECRET_SIZE`**: Definuje veľkosť zdieľaného tajomstva (32 bajty).

---

## Štruktúry

### `ClientServerContext`

Štruktúra `ClientServerContext` obsahuje všetky potrebné dáta pre klient-server reláciu.

#### Členovia štruktúry:
- **`int portno`**: Portové číslo pre komunikáciu, používa sa `int` pre správnu funkčnosť.
- **`int sockfd`**: Deskriptor soketu (súborový deskriptor).
- **`struct sockaddr_in serv_addr`**: Štruktúra adresy servera (IP adresa a port servera).
- **`struct hostent *server`**: Informácie o serveri pre klienta (IP adresa a port servera).
- **`unsigned char buffer[BUFFER_SIZE]`**: Buffer na odosielanie/prijímanie dát (256 bajtov).
- **`unsigned char bufferlen`**: Dĺžka bufferu, určená funkciami v klientovi alebo serveri.
- **`unsigned char private_key[PRIVATE_KEY_SIZE]`**: Privátny kľúč generovaný pomocou **drng**.
- **`unsigned char shared_secret[SHARED_SECRET_SIZE]`**: Zdieľaný kľúč pre šifrovanie a dešifrovanie pomocou ASCON.
- **`unsigned char decrypted_msg[BUFFER_SIZE]`**: Buffer pre dešifrované správy.
- **`unsigned long long decrypted_msglen`**: Dĺžka dešifrovanej správy.
- **`unsigned char *nsec`**: Nastavené na NULL, ukazovateľ na ďalšie bezpečnostné dáta. NIE JE POUŽÍVANÉ.
- **`unsigned char encrypted_msg[BUFFER_SIZE]`**: Buffer pre zašifrované správy.
- **`unsigned long long encrypted_msglen`**: Dĺžka zašifrovanej správy.
- **`const unsigned char *ad`**: Ukazovateľ na "asociované dáta". NIE JE POUŽÍVANÉ.
- **`unsigned long long adlen`**: Dĺžka asociovaných dát. NIE JE POUŽÍVANÉ.
- **`unsigned char npub[NONCE_SIZE]`**: Nonce, jedinečný zdieľaný "kľúč" pre správne použitie ASCON. MUSÍ BYŤ ROVNAKÝ (ako zdieľané tajomstvo).
- **`struct sockaddr_in cli_addr`**: Štruktúra adresy klienta (IP adresa a port).
- **`socklen_t clilen`**: Dĺžka štruktúry adresy klienta pre komunikáciu so serverom.
- **`int newsockfd`**: Soket pre prijaté pripojenia.

---

## Ďalšie premenné
- **`typedef unsigned char uch;`**: Alias pre `unsigned char`.
- **`static gf _121665 __attribute__((unused)) = {0xDB41, 1};`**:
    - **static**: Obmedzuje rozsah premennej na Curve25519.
    - **gf**: Galoisovo pole (používané v operáciách eliptických kriviek).
    - **_121665**: Fixné číslo pre Curve25519, používané v špecifických výpočtoch eliptických kriviek.
    - **`__attribute__((unused))`**: Hovorí kompilátoru, aby ignoroval varovania o nepoužívanej premennej.
    - **`{0xDB41, 1}`**: Inicializácia premennej s fixnou hodnotou.
- **`static const uch base[32] = {9};`**: Počiatočný bod pre Curve25519, zdieľaný medzi klientom a serverom. MUSÍ BYŤ IDENTICKÝ.

---

## Prototypy funkcií

### `void initializeContext(ClientServerContext *ctx);`

Inicializuje štruktúru `ClientServerContext`. Táto funkcia nastaví všetky potrebné štruktúry a buffery pre klientsku alebo serverovú reláciu.

### `void print_hex(uch *data, int length);`

Vypíše dáta v hexadecimálnom formáte.

### `void error(char *msg);`

Spracováva chyby, zvyčajne vytlačením chybovej správy a ukončením programu.

### `void generate_private_key(uch private_key[32]);`

Generuje náhodný 256-bitový privátny kľúč pre ECC.

### `void hexdump(const uch *data, size_t length);`

Vytlačí dump dát v hexadecimálnom formáte.

---

## Užívateľské typy

### `typedef unsigned char uch;`

Alias pre `unsigned char`.

---

## Session.c

```c
#include "session.h" // Hlavná knižnica pre komunikáciu s vonkajším svetom
#include "drng.h" // Na generovanie súkromného kľúča

// ========================================================================
// Funkcia na inicializáciu kontextu pre komunikáciu klient-server
// ========================================================================
void initializeContext(ClientServerContext *ctx) {
// Inicializuj číslo portu na 0 (port ešte nebol nastavený)
ctx->portno = 0;

    // Inicializuj popisovač zásuvky na 0 (zásuvka ešte nebola otvorená)
    ctx->sockfd = 0;

    // Vynuluj štruktúru adresy servera
    memset(&ctx->serv_addr, 0, sizeof(ctx->serv_addr));

    // Nastav ukazovateľ servera na NULL (server ešte nebol nastavený)
    ctx->server = NULL;

    // Vynuluj buffer používaný na komunikáciu
    memset(ctx->buffer, 0, sizeof(ctx->buffer));

    // Nastav dĺžku bufferu na 0
    ctx->bufferlen = 0;

    // Vynuluj súkromný kľúč
    // (bude použitý pre kryptografické operácie)
    memset(ctx->private_key, 0, sizeof(ctx->private_key));

    // Vynuluj zdieľaný tajný kľúč (používaný na šifrovanie a dešifrovanie)
    memset(ctx->shared_secret, 0, sizeof(ctx->shared_secret));

    // Vynuluj buffer pre dešifrovanú správu
    memset(ctx->decrypted_msg, 0, sizeof(ctx->decrypted_msg));

    // Inicializuj dĺžku dešifrovanej správy na 0
    ctx->decrypted_msglen = 0;

    // Nastav bezpečnostný parameter nonce na NULL
    ctx->nsec = NULL;

    // Vynuluj buffer pre šifrovanú správu
    memset(ctx->encrypted_msg, 0, sizeof(ctx->encrypted_msg));

    // Inicializuj dĺžku šifrovanej správy na 0
    ctx->encrypted_msglen = 0;

    // Nastav asociované dáta (ad) na NULL
    ctx->ad = NULL;

    // Inicializuj dĺžku asociovaných dát na 0
    ctx->adlen = 0;

    // Nastav pevnú hodnotu pre nonce (používané na jedinečnosť šifrovania)
    memcpy(ctx->npub, "simple_nonce_123", NONCE_SIZE);
}

// ========================================================================
// Funkcia na výpis dát v hexadecimálnom formáte
// ========================================================================
void hexdump(const uch *data, size_t length) {
// Prejdi každý bajt v poskytnutých dátach
for (size_t i = 0; i < length; i++) {
// Vytlač bajt v hexadecimálnom formáte
printf("%02x", data[i]);

        // Pridaj nový riadok po každom 16. bajte pre lepšiu čitateľnosť
        if ((i + 1) % 16 == 0)
            printf("\n");
    }

    // Uisti sa, že sa vytlačí nový riadok, ak dĺžka dát nie je 
    //násobkom 16
    if (length % 16 != 0)
        printf("\n");
}

// ====================================================
// Funkcia na generovanie náhodného súkromného kľúča (256 bitov/32 bajty)
// ====================================================
void generate_private_key(uch private_key[32]) {
// Pokús sa získať 32 bajtov náhodných dát pomocou rdrand
// (generátor náhodných čísel)
// Ak dostaneme menej ako 32 bajtov, vytlačíme chybu
if (rdrand_get_bytes(32, (unsigned char *) private_key) < 32) {
// Spracovanie chyby, ak náhodné dáta nie sú k dispozícii
fprintf(stderr, "Náhodné hodnoty nie sú k dispozícii\n");
return;  // Ukonči funkciu, ak náhodné dáta nie sú k dispozícii
}

    // Vytlač vygenerovaný súkromný kľúč v hexadecimálnom formáte
    printf("rdrand128:\n");
    hexdump(private_key, 32);  // Zavolaj funkciu na výpis súkromného kľúča
     v hex formáte
}

// ========================================================================
// Funkcia na spracovanie chýb vytlačením chybovej správy a ukončením 
// ========================================================================
void error(char *msg) {
perror(msg);  // Vytlač chybovú správu pomocou perror
exit(1);  // Ukonči program s chybovým kódom 1
}

// ========================================================================
// Funkcia na výpis danej dátovej poľa v hexadecimálnom formáte
// ========================================================================
void print_hex(uch *data, int length) {
// Prejdi po dátovom poli a vytlač každý bajt v
// hexadecimálnom formáte
for (int i = 0; i < length; i++) {
printf("%02X", data[i]);
}
// Vytlač nový riadok po vytlačení všetkých dát
printf("\n");
}
