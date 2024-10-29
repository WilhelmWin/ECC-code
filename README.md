void error(const char *msg)
Funkcia pre chyby. vypíše správu na konzolu pomocou funkcie perror() a ukončí sa s nenulovým stavom.
msg - správa, ktorá sa má vypísať .
void encryptDecrypt(char *input, char *output, int key)
Funkcia na šifrovanie a dešifrovanie reťazca pomocou operácie XOR. V tejto funkcii sa pre oba procesy používa rovnaká metóda, pretože šifrovanie XOR je samovratné.
vstup - reťazec, ktorý sa má zašifrovať alebo dešifrovať.
Výstup - vyrovnávacia pamäť, do ktorej sa zapíše výsledok.
kľúč - kľúč na šifrovanie/dešifrovanie.
Ako príklad, ak je vstupný reťazec „hello“ a kľúč je 5, zašifrovaný bude „mjqqqt“.
int powerMod(int base, int exp, int mod)
Funkcia na výpočet (base^exp) % mod. Implementuje algoritmus rozšírenia stupňa a následného rozšírenia modulo.
base - základ (číslo, ktoré sa zvyšuje na stupeň).
exp - exponent stupňa.
mod - modulus.
Pre powerMod(5, 3, 23) je výsledok 10, pretože 5^3 mod 23 = 10


int main(int argc, char *argv[])

- Vytvorí sa zásuvka, ktorá je viazaná na zadaný port.
- Server začne počúvať spojenia.
- Klient čaká na pripojenie, po ktorom sa spustí proces výmeny kľúčov Diffie-Hellman.
- Ak je výmena kľúčov úspešná, vypočíta sa spoločný tajný kľúč.
- Základná komunikačná slučka: server a klient si môžu vymieňať správy zašifrované pomocou operácie XOR so spoločným tajomstvom ako kľúčom.

Vytvorenie zásuvky:
Pomocou funkcie socket() sa vytvorí soket TCP. Ak sa soket nepodarilo vytvoriť, zavolá sa funkcia error().

Väzba socketov:
Pripojí zásuvku k zadanému portu. Ak sa väzba nepodarí, zavolá sa funkcia error().

Počúvanie spojení:
Spustí funkciu listen(), ktorá čaká na prichádzajúce spojenia. Server prejde do stavu čakania na spojenie od klienta.

Prijatie spojenia:
Prijíma spojenie od klienta pomocou funkcie accept() a vracia nový deskriptor súboru.

Výmena kľúčov Diffie-Hellman:
Vypočíta verejný kľúč servera a odošle ho klientovi.

Prijíma verejný kľúč klienta.
Vypočíta zdieľaný tajný kľúč, ktorý sa použije ako kľúč na šifrovanie XOR.

Výmena zašifrovaných správ:
Hlavná slučka prijme od klienta zašifrovanú správu, dešifruje ju, zobrazí ju na obrazovke, potom zadá odpoveď servera, zašifruje odpoveď a odošle ju klientovi.

Premenné
sockfd, connfd - deskriptory súborov pre sokety.
servaddr, cli - štruktúry obsahujúce informácie o serveri a klientovi.
buffer - vyrovnávacia pamäť na prijímanie a odosielanie správ.
private_key, public_key - súkromný a verejný kľúč servera.
client_public_key, shared_secret - verejný kľúč klienta a zdieľaný tajný kľúč používaný na šifrovanie a dešifrovanie správ.

Ukončenie
Cyklus sa preruší, keď server dostane od klienta správu „Bye“, po ktorej sa soket uzavrie a program sa ukončí.
