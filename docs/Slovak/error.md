# 📄 Dokumentácia súboru error.h
## 🔍 Popis

Tento súbor obsahuje funkcie na spracovanie chýb a signálov v serverovej časti programu. 
Obsahuje funkcie na bezpečné ukončenie programu pri výskyte chýb alebo signálov, ako je Ctrl+C alebo Ctrl+Z.
## ⚠️ Kľúčové vlastnosti:

- Spracovanie chýb pri problémoch so soketmi alebo iných kritických chybách.

- Ukončenie pripojení a správne ukončenie programu pri prijatí signálov od operačného systému.

- Podpora pre platformy Windows a Linux (Unix-like systémy).

- Zastavenie servera pri prijatí signálov prerušenia (Ctrl+C, Ctrl+Z).

## 📦 Použité knižnice:

- Windows: Na prácu so soketmi sa používa Winsock2 API.

- Linux: Na spracovanie signálov sa používa štandardná knižnica `<signal.h>`.

## Argumenty a funkcionalita:

Funkcie v tomto hlavičkovom súbore spracovávajú rôzne scenáre chýb a signálov:

- `error`: Spracováva chyby tým, že vypíše správu a ukončí program.

- `error_server`: Spracováva chyby na serveri tým, že zatvorí sokety a ukončí program.

- `register_signal_handler`: Registruje spracovateľa signálov pre Windows.

- `handle_signal`: Spracováva signály v systémoch Linux (napr. Ctrl+C alebo Ctrl+Z).

# Príklad použitia:
### 1. Spracovanie chyby na serveri:
```c
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) {
   error_server("Unable to open socket", sockfd, -1);
   }
```
### 2. Registrácia spracovateľa signálov na Windows:
```c
   ClientServerContext ctx;
   register_signal_handler(&ctx);
```
### 3. Spracovanie signálu na Linux:
```c
   struct sigaction sa;
   sa.sa_flags = SA_SIGINFO;
   sa.sa_sigaction = handle_signal;
   sigaction(SIGINT, &sa, NULL);
```

---
# Popis funkcií:
### error(char *msg)

Táto funkcia spracováva chyby tým, že vypíše správu a ukončí program.

- `msg`: Správa o chybe, ktorá bude vypísaná na obrazovku.

- Návratová hodnota: Funkcia ukončí program s chybovým kódom `1`.

### `error_server(const char *msg, int sockfd, int newsockfd)`

Táto funkcia spracováva chyby na serveri tým, že zatvorí sokety a ukončí program.

- `msg`: Správa o chybe, ktorá bude vypísaná na obrazovku.

- `sockfd`: Systémový soket servera.

- `newsockfd`: Systémový soket nového pripojenia.

- Návratová hodnota: Funkcia ukončí program s chybovým kódom `1`.

### `register_signal_handler(ClientServerContext *ctx)`

Táto funkcia registruje spracovateľa signálov pre platformu Windows. Spracovateľ signálov ukončí server po prijatí 
Ctrl+C alebo iného ukončovacieho signálu.

- ctx: Kontext klienta/servera obsahujúci informácie o soketoch.

- Návratová hodnota: Žiadna.

### `handle_signal`(int sig, siginfo_t *si, void *ucontext)

Táto funkcia spracováva signály v Linuxe, ako napríklad Ctrl+C alebo Ctrl+Z. Zatvára sokety a ukončí server.

- `sig`: Signál, ktorý bol prijatý.

- `si`: Štruktúra obsahujúca dodatočné informácie o signále.

- `ucontext`: Kontext súvisiaci s prijatým signálom.

- Návratová hodnota: Žiadna.

---
# error.c
## 1. `error` (Funkcia na spracovanie chýb)
```c
void error(char *msg) {
#ifdef _WIN32
   WSACleanup();  // Vyčistiť Winsock pred ukončením
#endif
   perror(msg);  // Zobraziť chybu pomocou perror
   exit(1);  // Ukončiť program s chybovým kódom 1
}
```
- `#ifdef _WIN32`: Ak je kód kompilovaný pre Windows, volá sa `WSACleanup()`, 
aby sa správne ukončil systém Winsock (systém na prácu so soketmi vo Windows).

- `perror(msg)`: Vypíše správu o chybe, ktorá bola odovzdaná ako `msg`,
do štandardného chybového výstupu pomocou funkcie perror. To pomáha používateľovi pochopiť, aká chyba nastala.

- `exit(1)`: Ukončí program s chybovým kódom 1, čo zvyčajne naznačuje neúspešné ukončenie programu.

## 2. `error_server` (Funkcia na spracovanie chýb špecifických pre server)
```c
void error_server(const char *msg, int sockfd, int newsockfd) {
    if (sockfd >= 0) {
#ifdef _WIN32
        closesocket(sockfd);
#else
        close(sockfd);
#endif
    }
    if (newsockfd >= 0) {
#ifdef _WIN32
        closesocket(newsockfd);
#else
        close(newsockfd);
#endif
    }
#ifdef _WIN32
    WSACleanup();
#endif
    perror(msg);
    exit(1);
}
```
### Zatvorenie soketov:

- `if (sockfd >= 0)` a if `(newsockfd >= 0)`: Kontroluje sa, či sú sokety otvorené (majú kladné deskriptory) 
pred ich zatvorením.

- `closesocket(sockfd) / close(sockfd)`: V závislosti od platformy sa zatvára serverový soket. Pre Windows sa používa closesocket(), pre Unix-like systémy sa používa close().

- `WSACleanup()`: Ak kód beží na platforme Windows, ukončuje sa systém Winsock po zatvorení soketov.

### Zobrazenie správy o chybe:

- `perror(msg)`: Vypíše správu o chybe, ktorá bola odovzdaná ako msg, spolu s dodatočnými informáciami
poskytovanými systémom.

### Ukončenie programu:

- `exit(1)`: Ukončí program s chybovým kódom `1`.

## 3. `handle_signal` (Spracovanie signálov na Windows)
```c
#ifdef _WIN32
static ClientServerContext *internal_ctx = NULL;  // statické v module
// Kontrola signálov
BOOL WINAPI handle_signal(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT || signal == CTRL_CLOSE_EVENT) {
        if (internal_ctx) {
            closesocket(internal_ctx->sockfd);
            closesocket(internal_ctx->newsockfd);
            printf("Signal received. Closing server socket...\n");
        }
        WSACleanup();
        exit(1);
    }
    return FALSE;
}
```
- `BOOL WINAPI handle_signal(DWORD signal)`: Funkcia spracovania signálov pre Windows.
Táto funkcia sa volá pri prijatí signálov ako `CTRL_C_EVENT`, `CTRL_BREAK_EVENT` alebo `CTRL_CLOSE_EVENT`,
ktoré sa zvyčajne vyskytnú pri ukončení programu pomocou Ctrl+C alebo uzavretí konzoly.

Zatvorenie soketov:

- `if (internal_ctx)`: Ak existuje klient-server kontext (štruktúra obsahujúca informácie o soketoch),
sokety sa zatvoria pomocou `closesocket()`.

### Vyčistenie:

- `WSACleanup()`: Ukončuje systém Winsock a uvoľňuje všetky zdroje použité pre soketové operácie.

### Ukončenie programu:

- `exit(1)`: Ukončí program s chybovým kódom `1`.

## 4. `register_signal_handler` (Registrácia spracovateľa signálov pre Windows)
```c
void register_signal_handler(ClientServerContext *ctx) {
    internal_ctx = ctx;
    if (!SetConsoleCtrlHandler(handle_signal, TRUE)) {
        fprintf(stderr, "Failed to register signal handler\n");
        exit(1);
    }
}
```
- `internal_ctx = ctx;`: Uloží klient-server kontext do globálnej premennej `internal_ctx`, aby sa mohol použiť vo
funkcii spracovateľa signálov.

- `SetConsoleCtrlHandler(handle_signal, TRUE)`: Registruje funkciu `handle_signal`
ako spracovateľa signálov na Windows. Ak registrácia zlyhá, program vypíše chybu a ukončí sa s chybovým kódom `1`.

## 5. `handle_signal` (Spracovanie signálov na UNIX-like systémoch)
```c
#else
// Kontrola signálov
void handle_signal(int sig, siginfo_t *si, void *ucontext) {
(void)ucontext;
// Prijatie informácií

    ClientServerContext *ctx = (ClientServerContext *)si->si_value.sival_ptr;
    close(ctx->sockfd);  // Zatvorenie portu
    printf("Received signal %d. Closing server...\n", sig);
    exit(1);
}
```
- `void handle_signal(int sig, siginfo_t *si, void *ucontext)`: 
Funkcia spracovania signálov pre Unix-like systémy. Táto funkcia sa volá pri prijatí signálu 
(napríklad od Ctrl+C alebo iného ukončovacieho mechanizmu).

### Zatvorenie soketov:

- `ClientServerContext *ctx = (ClientServerContext *)si->si_value.sival_ptr;`: 
Extrahuje klient-server kontext z údajov signálu `(si_value)`.

- `close(ctx->sockfd)`: Zatvorí soket pomocou systémovej funkcie `close()`.

### Ukončenie programu:

- `exit(1)`: Ukončí program s chybovým kódom `1`.