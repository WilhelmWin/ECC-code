#ifndef ERROR_H          // Ochranná direktíva na zabránenie viacnásobného zahrnutia
#define ERROR_H

// ========================================================================
// Knižnice
// ========================================================================
#include "session.h"     // Obsahuje definíciu ClientServerContext,
// používané na správu soketov

#ifdef _WIN32
#include <winsock2.h>    // Pre programovanie soketov vo Windows (Winsock2)
#include <windows.h>     // Pre Windows-špecifické typy a spracovanie signálov
#else
#include <signal.h>      // Pre sigaction, siginfo_t a signálové konštanty na Unix/Linux systémoch
#endif


// ========================================================================
// Funkcie na spracovanie chýb
// ========================================================================

// Funkcia na spracovanie všeobecnej fatálnej chyby:
// - Vypíše chybovú správu
// - Uvolní Winsock vo Windows, ak je to potrebné
// - Ukončí program s kódom 1
void error(char *msg);

// Funkcia na spracovanie chyby súvisiacej so serverom:
// - Prijíma správu a dva deskriptory soketov
// - Zavrie oba sokety, ak sú platné
// - Vykoná čistenie (napr. WSACleanup vo Windows)
// - Ukončí program
void error_server(const char *msg, int sockfd, int newsockfd);


// ========================================================================
// Funkcie na spracovanie signálov Ctrl+Z / Ctrl+C
// ========================================================================

#ifdef _WIN32
// Registruje spracovanie signálov vo Windows na zachytenie Ctrl+C alebo udalostí zatvorenia
// - Prijíma ukazovateľ na ClientServerContext, aby vedel, ktoré sokety zavrieť
void register_signal_handler(ClientServerContext *ctx);
#else
// Spracovanie signálov pre Unix/Linux systémy:
// - Prijíma metadáta signálu
// - Extrahuje kontext soketu zo siginfo_t
// - Zavrie serverový soket a ukončí program
void handle_signal(int sig, siginfo_t *si, void *ucontext);
#endif


#ifdef _WIN32
// Registruje konzolového spracovateľa signálov vo Windows.
// Musí byť zavolané počas inicializácie klienta s platným ukazovateľom na kontext.
void setup_signal_handler(ClientServerContext *ctx);
#else
// Funkcia spracovania signálov pre Unix/Linux systémy.
// Mala by byť registrovaná pomocou sigaction, pričom kontext sa odovzdáva cez sigqueue alebo globálnu premennú.
void handle_signal_client(int sig, siginfo_t *si, void *ucontext);
#endif


#endif // ERROR_H  // Koniec ochranného mechanizmu
