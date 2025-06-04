#include "session.h"
#include "error.h"

int main(int argc, char *argv[]) {

    // ====================================================================
    // Inicializácia štruktúry pre prácu klient-server
    // ====================================================================
    ClientServerContext ctx;
    initializeContext(&ctx);  // Inicializácia kontextu: vyplnenie
                              // polí predvolenými hodnotami

    // ====================================================================
    // Inicializácia Winsock (len pre Windows)
    // ====================================================================
#ifdef _WIN32
    WSADATA wsaData;
    // Inicializácia Winsock pre Windows platformu
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        error("Zlyhala inicializácia WSAStartup");
    }
#endif

    // ====================================================================
    // Kontrola vstupných argumentov
    // ====================================================================
    if (argc < 3) {
        error("Kontrola...\n"
              "Používateľ si neprečítal dokumentáciu k používaniu klienta.\n"
              "Chýba IP adresa alebo port.\n"
              "Formát použitia klienta:\n"
              "./client <meno hostiteľa> <port>\n"
              "Odchod do zabudnutia");
    }
    if (argc >= 4) {
        error("Kontrola...\n"
              "Používateľ si neprečítal dokumentáciu k používaniu klienta.\n"
              "Pridaných príliš veľa parametrov\n"
              "Formát použitia klienta:\n"
              "./client <meno hostiteľa> <port>\n"
              "Odchod do zabudnutia");
    }

    // ====================================================================
    // Generovanie súkromného kľúča (Curve25519)
    // ====================================================================
    generate_private_key(ctx.private_key);  // Generovanie súkromného kľúča
                                            // pomocou Curve25519
    printf("Generovanie súkromného kľúča servera:\n");
    hexdump(ctx.private_key, 32);  // Výpis vygenerovaného kľúča

    // ====================================================================
    // Konverzia portu na int
    // ====================================================================
    ctx.portno = atoi(argv[2]);  // Konverzia portu z argumentu na typ
                                 // integer

    // ====================================================================
    // Vytvorenie TCP socketu
    // ====================================================================
    ctx.sockfd = socket(AF_INET, SOCK_STREAM, 0);  // Vytvorenie TCP IPv4
                                                   // socketu
    if (ctx.sockfd < 0) {
        error("ERROR: socket sa nepodarilo otvoriť");
    }
    printf("Socket úspešne otvorený\n");

    // ====================================================================
    // Prevod mena hostiteľa na IP adresu
    // ====================================================================
    ctx.server = gethostbyname(argv[1]);  // Prevod mena hostiteľa na IP
    if (ctx.server == NULL) {
        error("Myši v serverovni\n"
              "Hostiteľa zožrali\n"
              "Nemožno sa pomýliť v porte alebo IP?");
    }
    printf("Hostiteľ nájdený\n");

    // ====================================================================
    // Nastavenie adresy servera
    // ====================================================================
    memset((char *)&ctx.serv_addr, 0, sizeof(ctx.serv_addr));
    ctx.serv_addr.sin_family = AF_INET;  // Vyberáme IPv4 rodinu
    memcpy(&ctx.serv_addr.sin_addr.s_addr,
           ctx.server->h_addr,
           ctx.server->h_length);  // Kopírujeme IP adresu do štruktúry
    ctx.serv_addr.sin_port = htons(ctx.portno);  // Konverzia portu
                                                 // na sieťový
                                                 // byte order a uloženie

    // ====================================================================
    // Nastavenie spojenia
    // ====================================================================
    if (connect(ctx.sockfd, (struct sockaddr *)&ctx.serv_addr,
                sizeof(ctx.serv_addr)) < 0) {
      error("Chyba pripojenia.\n"
            "Buď je váš internet nedostupný,\n"
            "alebo server si dal obedňajší spánok.\n"
            "Skúste to neskôr!\n"
            "Výsledok je nasledovný:");
    }
    printf("Spojenie úspešné\n");
// ====================================================================
// Výmena kľúčov (pomocou X25519)
// ====================================================================

crypto_scalarmult_base(ctx.public_key, ctx.private_key);
// Generovanie verejného kľúča klienta pomocou X25519

// Odoslanie verejného kľúča serveru
#ifdef _WIN32
int n = send(ctx.sockfd, (char *)ctx.public_key, sizeof(ctx.public_key),
             0);
// Konverzia na const char * (Windows)
#else
int n = write(ctx.sockfd, ctx.public_key, sizeof(ctx.public_key));
// Odoslanie v UNIX-like systémoch
#endif

// Kontrola úspešného odoslania verejného kľúča
if (n < 0) {
    error("Chyba pri odosielaní verejného kľúča");
}

// Prijatie verejného kľúča servera
n = recv(ctx.sockfd, (char *)ctx.server_public_key,
        sizeof(ctx.server_public_key), 0);  // Prijatie verejného kľúča
                                            // od servera
if (n < 0) {
    error("Chyba pri prijímaní verejného kľúča od servera");
}

// Výpis prijatého verejného kľúča servera (na ladenie)
printf("Verejný kľúč servera prijatý:\n");
hexdump(ctx.server_public_key, 32);  // Výpis verejného kľúča
                                     // servera v hexadecimálnom
                                     // formáte

// Výpočet spoločného tajomstva podľa protokolu Diffie-Hellman (X25519)
crypto_scalarmult(ctx.shared_secret, ctx.private_key,
                  ctx.server_public_key);
// Spoločné tajomstvo sa vypočíta na základe súkromného kľúča klienta a
// verejného kľúča servera

printf("Vypočítané spoločné tajomstvo:\n");
hexdump(ctx.shared_secret, 32);  // Výpis spoločného tajomstva v
                                 // hexadecimálnom formáte
// ====================================================================
// Kontrola na Ctrl+Z a Ctrl+C
// ====================================================================

#ifdef __linux__
    // Definujeme štruktúru sigaction pre spracovanie signálov
    struct sigaction sa;

    // Nastavíme príznak pre získanie podrobných informácií o signáli
    sa.sa_flags = SA_SIGINFO;

    // Nastavíme handler funkciu, ktorá sa zavolá pri prijatí signálu
    sa.sa_sigaction = handle_signal;

    // Inicializujeme masku signálov na prázdnu množinu (signály sa
    // počas spracovania neblokujú)
    sigemptyset(&sa.sa_mask);

    // Registrovanie handlera pre signál SIGTSTP (zvyčajne Ctrl+Z)
    sigaction(SIGTSTP, &sa, NULL);
#endif
// ====================================================================
// Začiatok slučky na výmenu zašifrovaných správ
// ====================================================================
while (1) {
    // Získanie vstupu od používateľa
    printf("Som: ");
    memset(ctx.buffer, 0, sizeof(ctx.buffer));  // Vyčistenie buffera
                                                // pred
                                                // načítaním správy od
                                                // používateľa

    if (fgets((char *)ctx.buffer, sizeof(ctx.buffer), stdin) == NULL) {
        error("Chyba pri čítaní vstupu");  // Kontrola chyby pri čítaní
                                           // zo stdin
    }

    // Odstránenie znaku nového riadku, ak je prítomný
    size_t len = strlen((char *)ctx.buffer);
    if (len > 0 && ctx.buffer[len - 1] == '\n') {
        ctx.buffer[len - 1] = '\0';  // Odstránenie nového riadku na
                                     // konci reťazca
    }

    ctx.bufferlen = strlen((char *)ctx.buffer);  // Uloženie dĺžky
                                                 // zadanej správy

    // Šifrovanie správy
    if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                            ctx.buffer, ctx.bufferlen,
                            ctx.npub,
                            ctx.shared_secret) != 0) {
        error("Chyba pri šifrovaní správy");
    }

    // Odoslanie zašifrovanej správy
#ifdef _WIN32
    n = send(ctx.sockfd, (const char *)ctx.encrypted_msg,
             ctx.encrypted_msglen, 0);  // Odoslanie zašifrovanej
                                        // správy (Windows)
#else
    n = write(ctx.sockfd, ctx.encrypted_msg, ctx.encrypted_msglen);
// Odoslanie zašifrovanej správy (Linux/Unix)
#endif
    if (n < 0)
        error("Chyba pri odosielaní správy serveru");

    // Ak používateľ zadal "bye", ukončíme komunikáciu
    if (strcasecmp((char *)ctx.buffer, "bye") == 0) {
        printf("Ukončili ste rozhovor.\n");
        break;  // Ukončenie slučky
    }

    // Vyčistenie buffera pred prijatím správy
    memset(ctx.encrypted_msg, 0, sizeof(ctx.encrypted_msg));

#ifdef _WIN32
    n = recv(ctx.sockfd, (char *)ctx.encrypted_msg,
             sizeof(ctx.encrypted_msg), 0);  // Prijatie
                                             // zašifrovanej
                                             // odpovede (Windows)
#else
    n = read(ctx.sockfd, ctx.encrypted_msg,
             sizeof(ctx.encrypted_msg));     // Prijatie
                                             // zašifrovanej
                                             // odpovede (Linux/Unix)
#endif
    if (n < 0)
        error("Chyba pri prijímaní správy od servera");

    ctx.encrypted_msglen = n;  // Uloženie skutočnej dĺžky
                               // prijatej správy

    // Dešifrovanie odpovede
    if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                            ctx.nsec,
                            ctx.encrypted_msg, ctx.encrypted_msglen,
                            ctx.npub, ctx.shared_secret) != 0) {
        error("Chyba pri dešifrovaní správy");
    }

    // Pridanie koncového nulového znaku k dešifrovanej správe
    ctx.decrypted_msg[ctx.decrypted_msglen] = '\0';

    // Výpis dešifrovanej správy servera
    printf("Server: %s\n", ctx.decrypted_msg);

    // Ak server poslal "bye", ukončíme komunikáciu
    if (strcasecmp((char *)ctx.decrypted_msg, "bye") == 0) {
        printf("Server ukončil rozhovor.\n");
        break;  // Ukončenie slučky
    }
}

// ====================================================================
// Uvoľnenie zdrojov a zatvorenie soketu
// ====================================================================
#ifdef _WIN32
closesocket(ctx.sockfd);  // Zatvorenie soketu vo Windows
WSACleanup();             // Uvoľnenie zdrojov Winsock
#else
close(ctx.sockfd);        // Zatvorenie soketu v Linux/Unix
#endif


exit(0);
}
