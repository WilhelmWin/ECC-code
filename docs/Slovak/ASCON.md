# 📄 Dokumentácia: Implementácia AEAD na základe ASCON-128a
Dátum publikácie originálu:

- 9 február 2014

Autori:

- [Martin Schläffer](https://github.com/mschlaeffer)

Originálna dokumentácia:

- [Originálny git ASCON](https://github.com/ascon/ascon-c/tree/main/crypto_aead/asconaead128/ref)

Ako bola získaná:

- Poskytnutá vedeckým vedúcim Milošom Drutarovským počas úpravy spôsobu šifrovania.

Čo bolo zmenené?

- Z [súborov](https://github.com/WilhelmWin/ECC-code/tree/master/2.0/Source/ASCON) bolo odstránené zobrazenie, konkrétne printstate.c, printstate.h, a taktiež boli odstránené konštanty pre iné realizácie.

## 🔐 Popis

ASCON-128a je algoritmus AEAD
(Authenticated Encryption with Associated Data),
oficiálne zvolený ako štandard NIST pre ľahké šifrovanie.
##  📦 Používané knižnice:

- `ascon.h` - obsahuje deklarácie štruktúr, funkcií a makier,
    nevyhnutných na prácu s vnútorným stavom ASCON
    a vykonávanie permutácií (`P12`, `P8`), ako aj inicializáciu,
    absorpciu, šifrovanie/dešifrovanie a finalizáciu.

- `word.h` - Zabezpečuje prácu s bajtami a 64-bitovými slovami na 
rôznych platformách, vrátane operácií načítania, uloženia a čistenia bajtov. Obsahuje aj makrá pre padding
    a základnú prácu s big-endian/malým endian.

- `constants.h` - Definuje všetky potrebné konštanty algoritmu
    ASCON-128a.

##  ⚠️ Kľúčové vlastnosti:

- **Bezpečnosť**: Konštantné porovnávanie tagu znižuje riziko útokov 
tretími stranami.

- **Prúdové spracovanie**: Algoritmus spracováva dáta po blokoch,
    čo ho robí vhodným pre zabudované systémy.

## 📘 Štruktúry a konštanty
```c
typedef struct {
uint64_t x[5];  // Stav ASCON: päť 64-bitových slov
} ascon_state_t;
```
Hlavné makrá:

- `ASCON_128A_IV` — iniciačný vektor pre ASCON-128a.

- `ASCON_128A_RATE` — rýchlosť spracovania v bajtoch (16 bajtov = 128 bitov).

- `CRYPTO_ABYTES` — veľkosť autentizačného tagu (zvyčajne 16 bajtov).

# 🔧 Funkcie
## 🔐 int crypto_aead_encrypt(...)

- Funkcia vykonáva šifrovanie vstupnej správy `m`
    s použitím kľúča k a nonce npub.

### Parametre:

| Parameter     | Typ                  | Popis                                      |
|---------------|----------------------|--------------------------------------------|
| `c`           | `unsigned char*`      | Buffer pre šifrotext a tag                 |
| `clen`        | `unsigned long long*` | Veľkosť šifrotextu (výstup)                |
| `m`           | `const unsigned char*`| Vstupná správa                            |
| `mlen`        | `unsigned long long`  | Veľkosť vstupnej správy                   |
| `npub`        | `const unsigned char*`| Verejný nonce (16 bajtov)                 |
| `k`           | `const unsigned char*`| Kľúč (16 bajtov)                          |
### Štádiá:
1. Inicializácia stavu ASCON.

2. Aplikácia 12 kôl permutácie (`P12`).

3. Spracovanie správy po 16 bajtoch za raz (`ASCON_128A_RATE`).

4. Pridanie odstupov (padding) pre posledný blok.

5. Finalizácia: aplikácia kľúča, permutácia, generovanie tagu.

6. Zápis šifrotextu + tagu do c.

### Vrátená hodnota:

- `0` — úspešné vykonanie.

## 🔓 int crypto_aead_decrypt(...)

- Funkcia vykonáva dešifrovanie šifrotextu a kontrolu autentizačného tagu.

### Parametre:

| Parameter     | Typ                  | Popis                                      |
|---------------|----------------------|--------------------------------------------|
| `c`           | `unsigned char*`      | Buffer pre šifrotext a tag                 |
| `clen`        | `unsigned long long*` | Veľkosť šifrotextu (výstup)                |
| `m`           | `const unsigned char*`| Vstupná správa                            |
| `mlen`        | `unsigned long long`  | Veľkosť vstupnej správy                   |
| `npub`        | `const unsigned char*`| Verejný nonce (16 bajtov)                 |
| `k`           | `const unsigned char*`| Kľúč (16 bajtov)                          |

### Štádiá:

1. Kontrola dĺžky `(clen >= 16)`.

2. Inicializácia stavu ASCON.

3. Spracovanie celých blokov šifrotextu.

4. Spracovanie posledného neúplného bloku s paddingom.

5. Finalizácia: extrakcia a porovnanie tagu.

### Kontrola tagu:

Vykonáva sa po bajtoch porovnanie prijatého a vygenerovaného
tagu v konštantnom čase, čím sa zabráni únikom prostredníctvom
časových útokov.

### Vrátená hodnota:

- `0` — ak tag autentizácie sa zhoduje.

- `1` — ak došlo k chybe (napr. ak tag nesúhlasí).

---
## 📑 Používané makrá a funkcie

| Makro / Funkcia       | Účel                                         |
|----------------------|----------------------------------------------|
| `LOADBYTES(ptr, len)` | Načíta `len` bajtov z `ptr` do 64-bitového slova. |
| `STOREBYTES(ptr, word, len)` | Uloží `len` bajtov zo 64-bitového `word` do `ptr`. |
| `CLEARBYTES(word, len)` | Vymaže všetky bajty okrem najnižších `len`. |
| `PAD(len)`            | Padding bajtov založený na dĺžke bloku.      |
| `P12(&s)`             | 12 kôl permutácie ASCON.                    |
| `P8(&s)`              | 8 kôl permutácie ASCON.                     |
| `DSEP()`              | Vráti konštantu pre oddelenie domén (Domain Separation). |

---
## 🧪 Príklad použitia

### **`int crypto_aead_encrypt(...)`**
```c
        if (crypto_aead_decrypt(ctx.decrypted_msg, &ctx.decrypted_msglen,
                                ctx.nsec,
                                ctx.encrypted_msg, ctx.encrypted_msglen,
                                ctx.npub, ctx.shared_secret) != 0) {
            error("ASCON problem\nDecryption error");
        }
```
### **`int crypto_aead_decrypt(...)`**
```c
        if (crypto_aead_encrypt(ctx.encrypted_msg, &ctx.encrypted_msglen,
                                ctx.buffer,
                                ctx.bufferlen, ctx.npub,
                                ctx.shared_secret) != 0) {
            error("ASCON problem\nEncryption error");
        }
```