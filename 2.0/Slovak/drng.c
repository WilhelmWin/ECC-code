#include "drng.h"

// ========================================================================
// RDRAND primitíva: funkcie na generovanie náhodných čísel
// pomocou inštrukcie RDRAND (pre rôzne veľkosti).
// ========================================================================

// ========================================================================
// Funkcia: rdrand64_step
// Účel: Získa 64-bitovú náhodnú hodnotu z hardvérového
// generátora RDRAND.
// Parametre:
//   - rand: ukazovateľ na premennú, do ktorej sa zapíše náhodné číslo
// Návratová hodnota:
//   - 1, ak bolo číslo úspešne získané
//   - 0, ak nastala chyba (napr. generátor je zaneprázdnený)
// ========================================================================
int rdrand64_step (uint64_t *rand)
{
    unsigned char ok;

    // Vložený assembler: rdrand zapíše 64-bitovú hodnotu,
    // setc nastaví príznak úspechu do premennej `ok`.
    asm volatile ("rdrand %0; setc %1"
        : "=r" (*rand), "=qm" (ok)); // výstupné operandy

    return (int) ok;
}

// ========================================================================
// Funkcia: rdrand64_retry
// Účel: Opakuje pokusy o získanie náhodného čísla, ak je
// RDRAND dočasne nepripravený.
// Parametre:
//   - retries: maximálny počet pokusov
//   - rand: ukazovateľ na premennú, do ktorej sa zapíše náhodné číslo
// Návratová hodnota:
//   - 1, ak sa podarilo číslo získať
//   - 0, ak všetky pokusy zlyhali
// ========================================================================
int rdrand64_retry (unsigned int retries, uint64_t *rand)
{
    unsigned int count = 0;

    // Opakuj pokusy, kým nedosiahneš limit
    while (count <= retries) {
        if (rdrand64_step(rand)) {
            return 1;  // Náhodné číslo úspešne získané
        }
        ++count;
    }

    return 0;  // Neúspech po všetkých pokusoch
}

// ========================================================================
// Funkcia: rdrand_get_bytes
// Účel: Získa n náhodných bajtov pomocou 64-bitových
// hodnôt z RDRAND.
// Parametre:
//   - n: počet bajtov, ktoré treba získať
//   - dest: ukazovateľ na cieľový buffer
// Návratová hodnota:
//   - počet skutočne získaných bajtov (môže byť < n pri chybe)
// ========================================================================
unsigned int rdrand_get_bytes (unsigned int n, unsigned char *dest)
{
    unsigned char *headstart;      // začiatok bufferu (nemusí byť
                                   // zarovnaný)
    unsigned char *tailstart = NULL; // koncová časť (zvyšné bajty)
    uint64_t *blockstart;         // ukazovateľ na 64-bitovo zarovnanú
                                  // časť
    unsigned int count, ltail, lhead, lblock;
    uint64_t i, temprand;

    // === Krok 1: Určenie zarovnania bufferu ===
    headstart = dest;

    // Ak je buffer zarovnaný na 8 bajtov, môžeme ho priamo použiť
    // ako uint64_t*
    if (((uint64_t)headstart % 8) == 0) {
        blockstart = (uint64_t *)headstart;
        lblock = n;     // celá dĺžka je hlavný blok
        lhead = 0;      // žiadna nezačínajúca nevyrovnaná časť
    } else {
        // Vypočítame adresu najbližšieho ďalšieho zarovnaného bloku
        blockstart = (uint64_t *)(((uint64_t)headstart & ~7ULL) + 8);

        // lblock – dĺžka hlavného bloku bez lhead
        lblock = n - (8 - (unsigned int)((uint64_t)headstart % 8));

        // Počet bajtov v nezačínajúcej (nevyrovnanej) časti
        lhead = (unsigned int)((uint64_t)blockstart - (uint64_t)headstart);
    }

    // Vypočítame dĺžku chvosta (zvyšné bajty po zarovnanom bloku)
    ltail = n - lblock - lhead;

    // Počet 64-bitových hodnôt na generovanie
    count = lblock / 8;

    // tailstart – adresa začiatku koncovej časti
    if (ltail) {
        tailstart = (unsigned char *)((uint64_t)blockstart + lblock);
    }

    // === Krok 2: Spracovanie nezačínajúcej časti, ak existuje ===
    if (lhead) {
        if (!rdrand64_retry(RDRAND_RETRIES, &temprand)) {
            return 0;  // Chyba pri nezačínajúcej časti
        }
        // Skopírujeme len prvých lhead bajtov z temprand
        memcpy(headstart, &temprand, lhead);
    }

    // === Krok 3: Hlavný blok (zarovnaná časť) ===
    for (i = 0; i < count; ++i, ++blockstart) {
        if (!rdrand64_retry(RDRAND_RETRIES, blockstart)) {
            return i * 8 + lhead;  // Chyba — vráť počet
                                   // úspešne získaných bajtov
        }
    }

    // === Krok 4: Chvost (zvyšok, menej než 8 bajtov) ===
    if (ltail) {
        if (!rdrand64_retry(RDRAND_RETRIES, &temprand)) {
            return count * 8 + lhead; // Vráť, koľko
                                      // bajtov sa podarilo
                                      // získať pred chybou
        }
        memcpy(tailstart, &temprand, ltail);
    }

    // Všetkých 'n' bajtov bolo úspešne vygenerovaných
    return n;
}
