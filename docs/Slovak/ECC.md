# Dokumentácia súbor ECC

### Pôvodný dátum publikácie:
- 21. februára 2014

### Autori:
- Wesley Janssen

### Pôvodná dokumentácia:
- [Curve25519 v 18 tweetoch](https://www.cs.ru.nl/bachelors-theses/2014/Wesley_Janssen___4037332___Curve25519_in_18_tweets.pdf)

### Ako bola získaná:
- Poskytol ju vedecký vedúci Miloš Drutarovský počas úvodných stretnutí týkajúcich sa projektu.

### Čo bolo zmenené?
- [Súbor](https://github.com/WilhelmWin/ECC-code/tree/master/2.0/Source/Curve25519) 
bol prispôsobený pre klient-server štruktúru.

---
## 🔍 Popis

Tento súbor obsahuje definície typov, makrá a prototypy funkcií 
pre operácie s eliptickými krivkami (ECC), vrátane základnej 
aritmetiky (sčítanie, odčítanie, násobenie), skalárneho násobenia 
a výpočtu inverzných prvkov v Galoisovom poli (GF). Tieto operácie
sú základné pre vykonávanie kryptografických výpočtov pomocou
eliptických kriviek, ako je výmena kľúčov podľa Diffieho-Hellmana 
a generovanie verejných kľúčov.

### Kľúčové vlastnosti:
- Operácie aritmetiky v Galoisovom poli (sčítanie, odčítanie, 
násobenie, inverzia).
- Skalárne násobenie a s ním spojené algoritmy.
- Balenie a rozbalenie bodov eliptickej krivky.
- Podmienený výber hodnôt.
- Skalárne násobenie s použitím základného bodu pre generovanie 
verejného kľúča.

### Použité knižnice:
- Hlavičkový súbor je určený na prácu s kryptografickou implementáciou,
ktorá používa dátové typy `long long int` (lli) pre veľké čísla a polia 
`unsigned char` na reprezentáciu bajtov.
- `ECC.h` -  komunikácia s vonkajším svetom
- `session.h` -  pre dve premenné v príslušnej sekcii na prácu s krivkou
### Závislosti od platformy:
Tento kód je nezávislý od platformy a môže byť použitý ako v Linuxe, tak 
aj vo Windows s minimálnymi zmenami.

### Argumenty a funkcionalita:
Funkcie v tomto hlavičkovom súbore vykonávajú rôzne operácie pre 
kryptografiu pomocou eliptických kriviek, vrátane skalárneho násobenia 
a aritmetiky v Galoisovom poli. Niektoré dôležité funkcie zahŕňajú:
- `add`, `sub`, `mul`, `inv` pre aritmetiku v Galoisovom poli.
- `crypto_scalarmult` pre skalárne násobenie s použitím bodu `p` a
skaláru `n`.
- `crypto_scalarmult_base` pre skalárne násobenie s použitím základného
bodu krivky.
- `pack` a `unpack` pre konverziu medzi bajtovými poliami a prvkami 
Galoisovho poľa.

### Premenné

- **`static gf _121665 = {0xDB41, 1}`**:
  - **static** - určené na použitie iba v rámci Curve25519
  - **gf** - Galoisovo pole
  - **_121665** - fixné číslo a názov premennej
  - **{0xDB41, 1}** - inicializácia premennej
- **`static uch base[32] = {9};`**: Základný bod na Curve25519 pre 
klienta a server. Musí byť identický pre oboch.

---
## Príklad použitia:
### 1. Pre vykonanie skalárneho násobenia:
```c
    unsigned char q[32];
    const unsigned char n[32] = {...}; // skalárna hodnota
    const unsigned char p[32] = {...}; // bod eliptickej krivky
    crypto_scalarmult(q, n, p); // q = n * p
```
### 2. Pre rozbalenie bajtového poľa do prvku Galoisovho poľa:
```c
    gf o;
    unpack(o, n); // n — to je bajtové pole
```
### 3. Pre zabalenie prvku Galoisovho poľa do bajtového poľa:
```c
    uch o[32];
    pack(o, n); // n — to je prvok Galoisovho poľa
```
---
# Popis funkcií:
## `car(gf o)`
- Táto funkcia vykonáva operáciu prenosu pre prvky Galoisovho poľa, 
aby sa zabezpečilo, že všetky prvky sa nachádzajú 
v rozsahu `[0, 2^16-1]`. Pridáva prenos, ak prvok presahuje 16 bitov, 
a opravuje nasledujúci prvok, ak je to potrebné.

1. Prechádza všetkými 16 prvkami poľa `o`.

2. Pridáva 2^16 k každému prvku, ak je to potrebné.

3 Po výpočte prenosu opraví prvky tak, aby sa zachovali v rozsahu.

## `add(gf o, gf a, gf b)`
- Táto funkcia vykonáva sčítanie dvoch prvkov Galoisovho poľa po prvkoch 
a ukladá výsledok do `o`.
1. Používa sčítanie po prvkoch medzi poľami `a` a `b`.
2. Výsledok je uložený do poľa `o`.

## `sub(gf o, gf a, gf b)`
- Táto funkcia vykonáva odčítanie prvkov Galoisovho poľa po prvkoch a 
ukladá výsledok do `o`.
1. Používa odčítanie po prvkoch medzi poľami `a` a `b`.
2. Výsledok je uložený do poľa `o`.

## `mul(gf o, gf a, gf b)`
- Táto funkcia vykonáva násobenie dvoch prvkov Galoisovho poľa pomocou 
dlhého násobenia a zohľadňuje parametre krivky.
1. Používa násobenie po prvkoch medzi poľami `a` a `b`.
2. Po násobení sa výsledok uloží do poľa `c`, následne sa opraví pre 
parametre krivky.
3. Vykoná sa operácia prenosu pre opravu získaných prvkov.

## `inv(gf o, gf i)`
- Táto funkcia počíta inverzný prvok pre Galoisovo pole pomocou metódy
umocnenia na druhú a násobenia.
1. Najprv skopíruje vstupný prvok `i` do dočasného poľa `c`.
2. Potom sa vykoná `253` iterácií, kde sa každé kolo umocní na druhú a 
pri potreba sa vynásobí prvkom `i` pre výpočet inverzného prvku.
3. Výsledkom je, že pole `c` obsahuje inverzný prvok pre `i`.

## `sel(gf p, gf q, int b)`
- Táto funkcia vykonáva podmienený výber medzi dvoma prvkami Galoisovho
poľa na základe hodnoty `b` (0 alebo 1).
Ak je `b` rovné `1`, vyberie sa `p`, ak `0` — `q`.

    Používa bitové operácie XOR na výber medzi poľami `p` a `q`.

## `mainloop(lli x[32], uch *z)`
- Tento cyklus vykonáva skalárne násobenie po prvkoch, spracováva každý 
bit skalára a vykonáva operácie na eliptickej krivke.
1. Inicializuje niekoľko polí pre prácu s prvkami poľa.
2. Proces skalárneho násobenia zahŕňa aplikáciu operácií podmieneného 
výberu, sčítania, 
odčítania a násobenia pre každý bit skalára, čo je súčasťou algoritmu 
eliptickej krivky.

## `unpack(gf o, const uch *n)`
- Táto funkcia prevádza bajtové pole n na prvok Galoisovho poľa `o`, 
rozširujúc každý bajt na 16-bitové prvky.
1. Pre každý bajt z poľa `n` sa vytvorí 16-bitový prvok a uloží sa do 
poľa `o`.

## `pack(uch *o, gf n)`
- Táto funkcia prevádza prvok Galoisovho poľa `n` späť na bajtové pole 
`o`.
1. Vykonajú sa niektoré korekčné operácie pre parametre krivky.
2. Každý prvok Galoisovho poľa sa prevedie na dva bajty a uloží sa do 
poľa `o`.

## `crypto_scalarmult(uch *q, const uch *n, const uch *p)`
- Táto funkcia vykonáva skalárne násobenie bodu `p` na skalár `n`, 
pomocou algoritmu eliptickej krivky.
1. Bajtové pole `n` sa skopíruje do poľa `z`.
2. Bod `p` sa rozbalí do Galoisovho poľa, potom sa vykoná hlavný cyklus 
skalárneho násobenia.
3. Po vykonaní násobenia sa výsledok inverzuje, potom sa vynásobí sám so 
sebou a výsledok sa zabalí do `q`.

### V našom prípade klienta a servera

- q: Spoločný kľúč

- n: Privátny kľúč hostiteľa

- p: Verejný kľúč konektora

## `crypto_scalarmult_base(uch *q, const uch *n)`
- Vykonáva skalárne násobenie s použitím základného bodu eliptickej 
krivky. Výsledok je `q = n * G`, 
kde `G` je základný bod a `n` je skalár. Výsledok sa uloží do `q`.

### V našom prípade klienta a servera
- q: Verejný kľúč hostiteľa
- n: Privátny kľúč hostiteľa
---
# ECC.c


## `car` (Operácia prenesenia pre prvky eliptických kriviek)

```c
sv car(gf o)
{
    int i;
    lli c;
    for (i = 0; i < 16; i++) {
        o[i] += (1 << 16);  // Zabezpečí správny rozsah všetkých prvkov
        c = o[i] >> 16;  // Prenos (carry), ak je hodnota prvku väčšia než 16 bitov
        o[(i + 1) * (i < 15)] += c - 1 + 37 * (c - 1) * (i == 15);
        // Upraví nasledujúci prvok, ak je to potrebné
        o[i] -= c << 16;  // Udrží aktuálny prvok v rámci rozsahu
    }
}

```
- `for (i = 0; i < 16; i++)`: Tento cyklus prechádza všetkými 16 prvkami 
poľa o, ktoré reprezentuje prvok v Galoisovom poli GF.

- `o[i] += (1 << 16);`: Pre každý prvok sa pridá 65536 (čo je ekvivalentné
`2^16`), aby sa zabezpečil správny rozsah hodnôt.

- `c = o[i] >> 16;`: Získa sa prenos (carry) z vyšších bitov, ak 
hodnota prvku presahuje 16 bitov.

- `o[(i + 1) * (i < 15)] += c - 1 + 37 * (c - 1) * (i == 15);`: Ak 
ide o prvok, ktorý nie je posledný `(i < 15)`, prenos sa pridá k 
nasledujúcemu prvku.

- `o[i] -= c << 16;`: Obnoví sa aktuálny prvok, pričom sa odstráni
prenos, aby zostal v rozsahu 0..65535.

## `add` (Sčítanie dvoch prvkov GF)
```c
sv add(gf o, gf a, gf b)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = a[i] + b[i];  // Sčítanie po jednotlivých prvkoch
}

```

- `for (i = 0; i < 16; i++)`: Jednoduchý cyklus pre počiatočné 
sčítanie dvoch prvkov Galoisovho poľa
  `a a b` a uloženie výsledku do poľa `o`. Pre každý index `i` sa
vykoná sčítanie príslušných prvkov.

## `sub` (Odčítanie dvoch prvkov GF)
```c
sv sub(gf o, gf a, gf b)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = a[i] - b[i];  // Sčítanie po jednotlivých prvkoch
}

```
- `for (i = 0; i < 16; i++)`: Podobne ako pri sčítaní, tento cyklus 
vykonáva odčítanie prvkov Galoisovho
poľa `b` od prvkov poľa a a ukladá výsledok do poľa `o`.

## `mul` (Násobenie dvoch prvkov GF)
```c
sv mul(gf o, gf a, gf b)
{
    lli i, j, c[31];
    for (i = 0; i < 31; i++)
        c[i] = 0;  // Inicializácia poľa pre prenos
    for (i = 0; i < 16; i++)
        for (j = 0; j < 16; j++)
            c[i + j] += a[i] * b[j];  // Násobenie a akumulácia výsledkov
    for (i = 16; i < 31; i++)
        c[i - 16] += 38 * c[i];  // Upravenie podľa špecifických parametrov krivky
    for (i = 0; i < 16; i++)
        o[i] = c[i];  // Uloženie výsledku do výstupného poľa
    car(o);  // Prenosová operácia na úpravu výsledku
    car(o);  // Ďalšia prenosová operácia pre bezpečnosť
}

```
- `c[31]`: Vytvára sa pole pre uloženie medzi výsledkov násobenia 
dvoch prvkov Galoisovho poľa.

- `for (i = 0; i < 16; i++)` a vnorený cyklus for `(j = 0; j < 16; j++)`: 
Tieto dva cykly vykonávajú počiatočné násobenie každého prvku poľa `a` s 
každým prvkom poľa `b`. 
Výsledky násobenia sa pridávajú do poľa c na pozíciách `i` + `j`.

- `c[i - 16] += 38 * c[i];`: Po vykonaní hlavného násobenia sa pridáva 
korekcia pre 
parametre krivky (zohľadňujúc jej špecifikácie).

- `car(o);`: Používa sa prenosová operácia na úpravu výsledku.

## `inv` (Inverzia prvku GF)
```c
// Funkcia pre výpočet inverzného prvku v GF
sv inv(gf o, gf i)
{
    gf c;
    int a;
    for (a = 0; a < 16; a++)
        c[a] = i[a];  // Skopíruje vstup do dočasného poľa
    for (a = 253; a >= 0; a--) {
        sq(c, c);  // Zvyšuje prvok na druhú
        if (a != 2 && a != 4)  // Preskočí niektoré iterácie kvôli efektivite
            mul(c, c, i);  // Násobí inverzný prvok, ak je to potrebné
    }
    for (a = 0; a < 16; a++)
        o[a] = c[a];  // Uloží konečný inverzný výsledok
}

```
- `for (a = 253; a >= 0; a--)`: Tento cyklus vykonáva výpočet
inverzného prvku v Galoisovom poli pomocou metódy zvyšovania na 
druhú a násobenia (Edwardsov algoritmus). Každý krok v cykle je buď z
vyšovanie na druhú, alebo násobenie s prvkom `i`.

- `if (a != 2 && a != 4)`: V určitých iteráciách sa násobenie preskočí,
aby sa zlepšila efektivita.

## `sel` (Podmienený výber medzi dvoma prvkami GF)
```c
// Funkcia pre podmienený výber medzi p a q na základe b
sv sel(gf p, gf q, int b)
{
    lli t, i, b1 = ~(b - 1); // b1 sa používa pre bitové operácie
    for (i = 0; i < 16; i++) {
        t = b1 & (p[i] ^ q[i]);  // XOR prvkov a aplikácia masky
        p[i] ^= t;  // Vyberie p alebo q na základe flagu b
        q[i] ^= t;
    }
}

```
- `for (i = 0; i < 16; i++)`: Prechádza všetkými prvkami a vykonáva 
bitovú operáciu 
XOR medzi prvkami `p` a `q` na základe flagu `b`. Tento flag určuje, 
ktorý z dvoch polí (`p` alebo `q`) bude vybraný.

## `mainloop` (Hlavná slučka pre násobenie skalárom)
```c
// Hlavný cyklus pre skalarne násobenie
sv mainloop(lli x[32], uch *z)
{
    gf a, b, c, d, e, f;
    lli p, i;
    
    // Inicializácia b s vstupným skalárom, nastavenie ostatných prvkov na 0
    for (i = 0; i < 16; i++) {
        b[i] = x[i];  
        d[i] = a[i] = c[i] = 0;  
    }
    
    a[0] = d[0] = 1;  // Nastavenie počiatočných hodnôt pre a a d
    
    // Cyklus po bitoch skalára
    for (i = 254; i >= 0; --i) {
        p = (z[i >> 3] >> (i & 7)) & 1;  // Extrahuje i-tý bit zo skalára
        
        // Podmienečný výber medzi a a b podľa bitu
        sel(a, b, p);  
        sel(c, d, p);  

        // Vykonávanie operácií eliptickej krivky
        add(e, a, c);  
        sub(a, a, c);
        add(c, b, d);
        sub(b, b, d);
        
        // Zvyšovanie prvkov na druhú
        sq(d, e);  
        sq(f, a);

        // Násobenie a s výsledkami
        mul(a, c, a);  
        mul(c, b, e);
        add(e, a, c);
        sub(a, a, c);

        // Operácie s elementmi
        sq(b, a);
        sub(c, d, f);
        mul(a, c, _121665);  // Násobenie konštantou (_121665)
        add(a, a, d);  // Pridanie výsledkov
        mul(c, c, a);  // Viac operácií eliptickej krivky
        mul(a, d, f);
        mul(d, b, x);  // Násobenie bodom skaláru
        sq(b, e);

        // Konečný podmienený výber podľa bitu
        sel(a, b, p);  
        sel(c, d, p);
    }
    
    // Uloženie výsledku do x
    for (i = 0; i < 16; i++) {
        x[i] = a[i];  
        x[i + 16] = c[i];  // Uloženie druhej časti výsledku
    }
}

```
- `for (i = 254; i >= 0; --i)`: Toto je hlavná slučka, ktorá spracováva 
každý bit skalára a vykonáva operácie eliptickej krivky na základe 
tohto bitu. 
Každá iterácia vykonáva množstvo operácií, ako sú sčítanie, odčítanie, 
násobenie 
a zvyšovanie na druhú, čo je typické pre rýchly algoritmus násobenia 
skalárom na eliptickej krivke.

## `crypto_scalarmult` a `crypto_scalarmult_base`

- Tieto funkcie vykonávajú operáciu násobenia bodu skalárom. 
Funkcia crypto_scalarmult prijíma bod `p` a skalár `n`, 
zatiaľ čo `crypto_scalarmult_base` používa základný bod (napríklad v 
kryptografii to môže byť bod na krivke, ktorý je vopred vybraný).