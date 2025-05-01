#include "error.h"

// ========================================================================
// Funkcia na spracovanie všeobecných chýb: vypíše chybu a ukončí program
// ========================================================================
void error(char *msg) {
#ifdef _WIN32
    WSACleanup();  // Vyčistí Winsock pred ukončením (Windows)
#endif
    perror(msg);    // Vypíše chybové hlásenie vrátane systémovej chyby
    exit(1);        // Ukončí program s návratovým kódom 1
}

// ========================================================================
// Funkcia na spracovanie chýb servera: zatvára sockety a ukončuje program
// ========================================================================
void error_server(const char *msg, int sockfd, int newsockfd) {
    // Ak je serverový socket otvorený, zatvoriť ho
    if (sockfd >= 0) {
#ifdef _WIN32
        closesocket(sockfd);  // Windows: zatvorenie pomocou closesocket
#else
        close(sockfd);        // Unix/Linux: zatvorenie pomocou close
#endif
    }

    // Ak je otvorený klientský socket, zatvoriť ho
    if (newsockfd >= 0) {
#ifdef _WIN32
        closesocket(newsockfd);
#else
        close(newsockfd);
#endif
    }

#ifdef _WIN32
    WSACleanup();  // Vyčistenie prostredia Winsock
#endif

    perror(msg);    // Vypíše chybové hlásenie
    exit(1);        // Ukončí program s chybou
}

// ========================================================================
// Spracovanie signálov: bezpečné ukončenie servera cez Ctrl+C / Ctrl+Z
// ========================================================================

#ifdef _WIN32  // === Pre Windows systémy ===

static ClientServerContext *internal_ctx = NULL;  // Statický ukazovateľ na kontext (len v tomto súbore)

// Obsluha signálov ako Ctrl+C, Ctrl+Break alebo zatvorenie konzoly
BOOL WINAPI handle_signal(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT || signal == CTRL_CLOSE_EVENT) {
        if (internal_ctx) {
            // Zatvorenie oboch socketov: server a klient
            closesocket(internal_ctx->sockfd);
            closesocket(internal_ctx->newsockfd);
            printf("Signal received. Closing server socket...\n");
        }
        WSACleanup();  // Vyčistiť Winsock
        exit(1);       // Ukončiť program
    }
    return FALSE;  // Ak signál nebol obslúžený
}

// Registrácia signal handlera (len Windows)
void register_signal_handler(ClientServerContext *ctx) {
    internal_ctx = ctx;  // Uloženie kontextu pre neskoršie použitie

    // Nastavenie vlastnej funkcie na spracovanie signálov
    if (!SetConsoleCtrlHandler(handle_signal, TRUE)) {
        fprintf(stderr, "Failed to register signal handler\n");
        exit(1);
    }
}

#else  // === Pre Unix/Linux systémy ===

// Obsluha signálov ako SIGINT (Ctrl+C) alebo iných ukončovacích signálov
void handle_signal(int sig, siginfo_t *si, void *ucontext) {
    (void)ucontext;  // Nepoužíva sa, ale potrebné pre kompatibilitu

    // Získanie kontextu zo signálu
    ClientServerContext *ctx = (ClientServerContext *)si->si_value.sival_ptr;

    // Zatvorenie socketu servera
    close(ctx->sockfd);

    // Výpis do konzoly
    printf("Received signal %d. Closing server...\n", sig);

    // Ukončenie programu
    exit(1);
}

#endif  // Koniec platformovo špecifického kódu

