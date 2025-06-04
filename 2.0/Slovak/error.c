#include "session.h"
#include "error.h"

// ========================================================================
// Funkcia na spracovanie všeobecných chýb
// ========================================================================
void error(char *msg) {
#ifdef _WIN32
    WSACleanup();  // Uvoľnenie Windows Sockets API pred ukončením
#endif
    perror(msg);    // Vypíše zadanú chybovú správu so systémovým
                    // vysvetlením
    exit(1);        // Ukončí program s chybovým kódom 1
}

// ========================================================================
// Funkcia na spracovanie chýb servera: zatvorí sokety a ukončí program
// ========================================================================
void error_server(const char *msg, int sockfd, int newsockfd) {
    // Zatvorenie hlavného serverového soketu, ak je platný
    if (sockfd >= 0) {
#ifdef _WIN32
        closesocket(sockfd);  // Windows: zatvorenie soketu pomocou
                              // closesocket
#else
        close(sockfd);        // Unix/Linux: zatvorenie soketu pomocou
                              // close
#endif
    }

    // Zatvorenie soketu klientského pripojenia, ak je platný
    if (newsockfd >= 0) {
#ifdef _WIN32
        closesocket(newsockfd);
#else
        close(newsockfd);
#endif
    }

#ifdef _WIN32
    WSACleanup();  // Uvoľnenie prostredia Winsock
#endif

    perror(msg);    // Vypíše chybovú správu
    exit(1);        // Ukončí program s chybou
}

// ========================================================================
// Spracovanie signálov: bezpečné ukončenie servera pri Ctrl+C / Ctrl+Z
// ========================================================================

#ifdef _WIN32  // === Pre Windows systémy ===

static ClientServerContext *internal_ctx = NULL;  // Statický ukazovateľ
                                                  // na uloženie kontextu
                                                  // v module

// Spracovanie signálov konzoly ako Ctrl+C, Ctrl+Break, zatvorenie konzoly
BOOL WINAPI handle_signal(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT ||
      signal == CTRL_CLOSE_EVENT) {
        if (internal_ctx) {
            // Zatvorenie serverového aj klientského soketu
            closesocket(internal_ctx->sockfd);
            closesocket(internal_ctx->newsockfd);
            printf("Signál prijatý. Zatváram serverový soket...\n");
        }
        WSACleanup();  // Uvoľnenie prostredia soketov
        exit(1);       // Ukončenie programu
    }
    return FALSE;  // Ak signál nebol spracovaný, vráti FALSE
}

// Registrácia spracovania signálu (špecifické pre Windows)
void register_signal_handler(ClientServerContext *ctx) {
    internal_ctx = ctx;  // Uloženie kontextu pre neskoršie použitie

    if (!SetConsoleCtrlHandler(handle_signal, TRUE)) {
        error("Nepodarilo sa zaregistrovať spracovanie signálu\n");
    }
}

#else  // === Pre Unix/Linux systémy ===

// Spracovanie signálu SIGINT alebo iných ukončovacích signálov
void handle_signal(int sig, siginfo_t *si, void *ucontext) {
    (void)ucontext;  // Nepoužité, vyžadované pre kompatibilitu

    // Získanie kontextu servera zo signálovej štruktúry
    ClientServerContext *ctx =
        (ClientServerContext *)si->si_value.sival_ptr;

    // Zatvorenie serverového soketu
    close(ctx->sockfd);

    // Informovanie užívateľa
    printf("Prijatý signál %d. Zatváram server...\n", sig);

    exit(1);
}

#endif

#ifdef _WIN32
// Globálny ukazovateľ na kontext klient-server
static ClientServerContext *g_ctx = NULL;

// Spracovanie konzolových signálov (Ctrl+C alebo Ctrl+Break)
BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT) {
        if (g_ctx) {
            const char *msg = "bye";

            if (crypto_aead_encrypt(g_ctx->encrypted_msg,
                                    &g_ctx->encrypted_msglen,
                                    (const unsigned char *)msg,
                                    strlen(msg),
                                    g_ctx->npub,
                                    g_ctx->shared_secret) != 0) {
                error("Chyba šifrovania\n");
            }

            int bytes_sent = send(g_ctx->sockfd,
                                  (const char *)g_ctx->encrypted_msg,
                                  (int)g_ctx->encrypted_msglen, 0);
            if (bytes_sent == SOCKET_ERROR) {
                error("Chyba pri odoslaní: %d\n");
            } else {
                printf("Odoslaných %d bajtov\n", bytes_sent);
            }

            printf("Signál prijatý. Odoslaná správa "
                   " 'bye' serveru a zatváram klienta...\n");
            closesocket(g_ctx->sockfd);
        }
        exit(1);
    }
    return TRUE;
}

void setup_signal_handler(ClientServerContext *ctx) {
    g_ctx = ctx;  // Uloženie kontextu pre použitie v spracovaní signálu

    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        error("Nepodarilo sa nastaviť spracovanie konzolového signálu\n");
    }
}
#else

// ========================================================================
// Pre Linux: Ak klient stlačí Ctrl+Z
// ========================================================================

void handle_signal_client(int sig, siginfo_t *si, void *ucontext) {
    (void)ucontext;

    ClientServerContext *ctx =
        (ClientServerContext *)si->si_value.sival_ptr;

    const char *msg = "bye";

    if (crypto_aead_encrypt(ctx->encrypted_msg, &ctx->encrypted_msglen,
                            (unsigned char *)msg, strlen(msg),
                            ctx->npub, ctx->shared_secret) != 0) {
        error("Chyba šifrovania");
    }

    ssize_t bytes_sent = send(ctx->sockfd, ctx->encrypted_msg,
                              ctx->encrypted_msglen, 0);
    if (bytes_sent == -1) {
        error("Chyba pri odoslaní");
    } else {
        printf("Odoslaných %ld bajtov\n", bytes_sent);
    }

    printf("Prijatý signál %d. "
           "Odoslaná správa 'bye' serveru a zatváram klienta...\n", sig);

    close(ctx->sockfd);
    exit(1);
}

#endif  // Koniec platformovo špecifického kódu