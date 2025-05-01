// ========================================================================
//   Podpora kompilátora pre hardvérové inštrukcie
// ========================================================================

// Definícia, ak kompilátor podporuje inštrukciu rdrand
// (používa sa na generovanie náhodných čísel na hardvérovej úrovni)
#undef HAVE_RDRAND

// ========================================================================
//   Hlavičková ochrana pre DRNG (Digitálny generátor náhodných čísel)
// ========================================================================

// Ochrana proti viacnásobnému zahrnutiu rovnakého hlavičkového súboru
// (používa sa na zabránenie chybám z opakovaného zahrnutia)
#ifndef __DRNG__H
#define __DRNG__H

// Zahrnutie štandardných knižníc
#include <stdint.h> // Obsahuje štandardné typy celých čísel
                    // (napr. uint64_t,uint32_t atď.)
#include <string.h> // Pre funkcie spojené s prácou s pamäťou ako memcpy

// ========================================================================
//   Počet pokusov pre RDRAND
// ========================================================================

/* Odporúčaný počet pokusov pri generovaní náhodných čísel pomocou
   inštrukcie RDRAND. Toto číslo je založené na binomickom rozdelení
   pravdepodobnosti – ak generovanie zlyhá, systém sa pokúsi operáciu
   opakovať až 10-krát. */

#define RDRAND_RETRIES 10

// ========================================================================
//   Základné funkcie RDRAND pre generovanie náhodných čísel
// ========================================================================

// Generuje 64-bitové náhodné číslo pomocou inštrukcie RDRAND
//   (iba pre 64-bitové systémy).
//   Funkcia vracia 0 v prípade zlyhania a nenulovú hodnotu pri úspechu.

// Funkcia na získanie jedného 64-bitového náhodného čísla
int rdrand64_step(uint64_t *rand);

// ========================================================================
//   Opakované generovanie pomocou RDRAND
// ========================================================================

/* Pokus o generovanie 64-bitového náhodného čísla s daným
   počtom opakovaní (iba pre 64-bitové systémy). Ak generovanie zlyhá,
   funkcia sa pokúsi zopakovať operáciu až 'retries' krát. */

// Funkcia pre opakované pokusy o generovanie náhodného čísla
int rdrand64_retry(unsigned int retries, uint64_t *rand);

// ========================================================================
//   Generovanie viacerých bajtov pomocou RDRAND
// ========================================================================

/* Generuje viacero náhodných bajtov pomocou inštrukcie RDRAND.
   Parameter 'n' určuje počet bajtov, ktoré sa majú vygenerovať,
   a 'dest' je pole, kam sa výsledok uloží.
   Funkcia vracia počet úspešne vygenerovaných bajtov. */

// Funkcia na generovanie viacerých náhodných bajtov
unsigned int rdrand_get_bytes(unsigned int n, unsigned char *dest);

#endif  /* __DRNG__H */
