# Dokumentácia súboru DRNG

### Dátum publikácie originálu:
- 17. októbra 2018
### Autori:
- Intel Corporation
### Originálna dokumentácia:
- [Intel Digital Random Number Generator (DRNG)](https://www.intel.com/content/dam/develop/external/us/en/documents/drng-software-implementation-guide-2-1-185467.pd)
### Ako bola získaná:
- Poskytnutý vedeckým vedúcim Milošom Drutarovským súbor z učebného programu o bezpečnosti počítačových systémov s názvom `bps_ps_sem5`.

### Čo bolo zmenené?

- Všetky [súbory](https://github.com/WilhelmWin/ECC-code/tree/master/2.0/Source/Drng)
  boli presunuté do dvoch súborov `drng.c/drng.h`.
- Hlavná funkcia (main) bola odstránená, rovnako ako všetky ďalšie funkcie, ako napríklad tie, ktoré pracovali na 16 a 32 bitových systémoch, ktoré sa v projekte nevyužívajú, a ponechané boli iba tieto tri funkcie.
---
## 🔍  Popis

Tento súbor obsahuje definície funkcií a makier na prácu s generovaním náhodných čísel, pričom sa využívajú rôzne metódy, vrátane využitia inštrukcie `RDRAND` (ak je podporovaná hardvérovou stránkou). Cieľom je vytvoriť spoľahlivý zdroj náhodných čísel pre kryptografické operácie, ako je generovanie kľúčov a bezpečnostných tokenov.
Kľúčové vlastnosti:

- Použitie inštrukcie `RDRAND` na hardvérové generovanie náhodných čísel.

- Funkcie na opakované pokusy generovania náhodných čísel, ak neprešli kontrolou.

- Utility na konverziu náhodných čísel do bajtových polí.

### Používané knižnice:

- `stdint.h` - na prácu s celočíselnými hodnotami
- `string.h` - na operácie s pamäťou

### Závislosti od platformy:

Tento kód môže využiť inštrukciu `RDRAND`, ak je podporovaná procesorom, a bude správne fungovať v platformovo nezávislom kóde s minimálnymi zmenami pre platformu.

## Argumenty a funkčnosť:

Funkcie v tomto hlavičkovom súbore vykonávajú nasledujúce úkony:
- `rdrand64_step(uint64_t *rand)` - generovanie náhodných čísel pomocou `RDRAND`.

- `rdrand64_retry(unsigned int retries, uint64_t *rand)` - opakované pokusy generovania náhodných čísel v prípade zlyhania.

- `rdrand_get_bytes(unsigned int n, unsigned char *dest)` - utility na balenie a rozbalenie dát.

---
## Príklad použitia:
### 1. Na získanie náhodného čísla:
```c
   uint64_t rand;
   int result = rdrand64_step(&rand);
       if (result == 0) {
          printf("Generované náhodné číslo: %llu\n", rand);
   } else {
           printf("Chyba pri generovaní náhodného čísla\n");
   }
```
### 2. Na získanie náhodných bajtov:
```c
   unsigned char bytes[32];
   unsigned int num_bytes = rdrand_get_bytes(32, bytes);
   printf("Generovaných %u náhodných bajtov\n", num_bytes);
```
### 3. Na vykonanie opakovaných pokusov pri generovaní náhodného čísla:
```c
   uint64_t rand;
   int result = rdrand64_retry(10, &rand);  // Pokúsiť sa až 10-krát
       if (result == 0) {
           printf("Generované náhodné číslo po opakovaných pokusoch: %llu\n", rand);
     } else {
           printf("Nepodarilo sa generovať náhodné číslo po 10 pokusoch\n");
   }
```
--- 
# Popis funkcií:
### `rdrand64_step(uint64_t *rand)`
- Táto funkcia generuje jedno náhodné 64-bitové číslo s 
využitím inštrukcie RDRAND. Vracia 0 pri úspešnom dokončení
a nenulovú hodnotu v prípade chyby.
- Funkcia vykonáva inštrukciu `RDRAND` na generovanie
náhodného 64-bitového čísla. Pri úspešnej generácii je 
číslo uložené do zadaného parametra rand.

### `rdrand64_retry(unsigned int retries, uint64_t *rand)`
- Táto funkcia vykonáva generovanie náhodných čísel s využitím `RDRAND`
a opakuje pokusy až do retries krát, ak generovanie zlyhá.
- Funkcia sa pokúša niekoľkokrát (v rámci stanoveného počtu pokusov) 
získať náhodné číslo. Ak generovanie zlyhá pri každom pokuse, 
funkcia vracia chybu.

### `rdrand_get_bytes(unsigned int n, unsigned char *dest)`
- Táto funkcia generuje pole náhodných bajtov dĺžky n a ukladá ich 
do poľa `dest`.
- Funkcia generuje náhodné bajty pomocou `RDRAND` a kopíruje ich do 
poľa `dest`. Tento proces pokračuje až do vytvorenia `n` bajtov.
---
# drng.c
## 1. `rdrand64_step` (Krok generovania náhodného čísla)
```c
int rdrand64_step(uint64_t *rand)
{
    if (__builtin_ia32_rdrand64_step(rand)) {
        return 0;
    }
    return -1;  // Chyba pri generovaní náhodného čísla
}
```
- V tejto funkcii je použitá zabudovaná funkcia GCC na volanie 
inštrukcie `RDRAND`, ktorá generuje 64-bitové náhodné číslo, ak je dostupné.

## 2. `rdrand64_retry` (Opakované pokusy generovania náhodného čísla)
```c
int rdrand64_retry(unsigned int retries, uint64_t *rand)
{
    int result;
    for (unsigned int i = 0; i < retries; ++i) {
        result = rdrand64_step(rand);
        if (result == 0) {
            return 0;  // Úspech
        }
    }
    return -1;  // Nepodarilo sa generovať číslo po všetkých pokusoch
}
```
- Táto funkcia vykonáva niekoľko pokusov na získanie náhodného čísla 
s využitím funkcie `rdrand64_step`.

## 3. `rdrand_get_bytes` (Generovanie náhodných bajtov)
```c
unsigned int rdrand_get_bytes(unsigned int n, unsigned char *dest)
{
    unsigned int bytes_generated = 0;
    uint64_t rand;
    while (bytes_generated < n) {
        if (rdrand64_step(&rand) == 0) {
            unsigned char *ptr = (unsigned char *)&rand;
            unsigned int to_copy = (n - bytes_generated > sizeof(rand)) ? sizeof(rand) : (n - bytes_generated);
            memcpy(dest + bytes_generated, ptr, to_copy);
            bytes_generated += to_copy;
        }
    }
    return bytes_generated;
}
```
- Funkcia generuje náhodné bajty s využitím inštrukcie `RDRAND` a zapisuje
ich do zadaného poľa `dest`.