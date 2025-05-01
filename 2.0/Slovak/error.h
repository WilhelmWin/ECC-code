#ifndef ERROR_H            // Začiatok include guard – zabraňuje viacnásobnému zahrnutiu
#define ERROR_H

#include "session.h"       // Zahrňuje definíciu ClientServerContext (používa sa pre správu soketov)

#ifdef _WIN32
#include <windows.h>       // Windows špecifické typy a spracovanie signálov
#include <winsock2.h>      // Práca so sieťami (Winsock2) vo Windows
#else
#include <signal.h>        // Pre sigaction, siginfo_t a signály v systémoch Unix/Linux
#endif

// Funkcia na spracovanie všeobecnej chyby:
// - Vypíše chybové hlásenie
// - Na Windows zavolá WSACleanup() na uvoľnenie Winsock
// - Ukončí program s návratovým kódom 1
void error(char *msg);

// Funkcia na spracovanie chyby na serveri:
// - Prijíma správu a dva deskriptory soketov
// - Zavrie platné sokety
// - Urobí upratovanie (napr. WSACleanup na Windows)
// - Ukončí program
void error_server(const char *msg, int sockfd, int newsockfd);

#ifdef _WIN32
// Registruje obsluhu signálu vo Windows:
// - Zaregistruje funkciu na zachytenie Ctrl+C alebo zatvorenia konzoly
// - Používa ukazateľ na ClientServerContext pre správne zatvorenie soketov
void register_signal_handler(ClientServerContext *ctx);
#else
// Obsluha signálov pre Unix/Linux:
// - Prijíma signál, doplnkové informácie a kontext
// - Získa kontext soketov zo štruktúry siginfo_t
// - Zavrie serverový soket a ukončí program
void handle_signal(int sig, siginfo_t *si, void *ucontext);
#endif

#endif // ERROR_H          // Koniec include guard
