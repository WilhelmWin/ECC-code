#ifndef ECC_H
#define ECC_H

// ========================================================================
// Definície typov
// ========================================================================

// 'lli' je skratka pre 'long long int', používaná na reprezentáciu
// veľkých celých čísel. Takéto čísla sú potrebné pri práci s veľkými
// hodnotami, napríklad v kryptografii nad eliptickými krivkami.
typedef long long int lli;

// 'gf' je pole 16 prvkov typu lli. Predstavuje prvok
// konečného poľa (Galois Field, GF), používaného v operáciách nad
// eliptickými krivkami. Veľkosť 16 zodpovedá poľu Curve25519.
typedef lli gf[16];

// 'uch' je skratka pre 'unsigned char'. Používa sa na
// reprezentáciu bajtov (napr. pri prenose kľúčov, skalárov a bodov
// ako poľ bajtov).
typedef unsigned char uch;

// ========================================================================
// Definície makier
// ========================================================================

// 'sv' je makro pre funkcie, ktoré nevracajú hodnotu (void).
#define sv void

// 'sq(o, i)' je makro na umocnenie poľa 'i' na druhú,
// pričom výsledok sa uloží do 'o'. Používa funkciu 'mul' na
// vynásobenie poľa samého sebou.
#define sq(o, i) mul(o, i, i)

// ========================================================================
// Prototypy funkcií
// ========================================================================

// Prenosová operácia (car)
// Funkcia 'car' rieši pretečenie prvkov konečného poľa,
// aby všetky hodnoty zostali v rozsahu [0, 2^16 - 1]. Je to
// nevyhnutné po aritmetických operáciách na zachovanie správnosti.
sv car(gf o);

// Operácia sčítania (add)
// Funkcia 'add' vykoná po prvkoch súčet dvoch prvkov poľa
// 'a' a 'b' a uloží výsledok do 'o'.
sv add(gf o, gf a, gf b);

// Operácia odčítania (sub)
// Funkcia 'sub' odpočíta prvky poľa 'b' od 'a' a
// uloží výsledok do 'o'.
sv sub(gf o, gf a, gf b);

// Operácia násobenia (mul)
// Funkcia 'mul' vynásobí dva prvky poľa 'a' a 'b'
// a výsledok uloží do 'o'. Používa dlhú aritmetiku
// a následne znižuje výsledok podľa parametrov krivky
// (napr. násobenie 38).
sv mul(gf o, gf a, gf b);

// Operácia inverzie (inv)
// Funkcia 'inv' vypočíta inverzný prvok k 'i' v poli
// a uloží ho do 'o'. Používa sa pri delení a pri výpočte
// súradnice x po skalárnom násobení.
sv inv(gf o, gf i);

// Podmienený výber (sel)
// Funkcia 'sel' vyberie medzi prvkami poľa 'p' a 'q'
// na základe premennej 'b'. Ak 'b' je 1, vyberie sa 'p',
// inak 'q'. Používa sa na ochranu pred časovými útokmi.
sv sel(gf p, gf q, int b);

// Hlavný cyklus skalárneho násobenia (mainloop)
// Funkcia 'mainloop' implementuje skalárne násobenie bodu
// na eliptickej krivke. Vstupy: skalár 'z' a dáta bodu 'x'.
// Výstupy: x[0..15] — x-súradnica výsledného bodu,
// x[16..31] — menovateľ (na následné prevrátenie).
sv mainloop(lli x[32], uch *z);

// Rozbalenie bajtového poľa na prvok poľa (unpack)
// Funkcia 'unpack' prevedie 32-bajtové pole 'n' na prvok poľa 'o',
// pričom každé dva bajty sa interpretujú ako 16-bitová hodnota.
sv unpack(gf o, const uch *n);

// Zbalenie prvku poľa do bajtového poľa (pack)
// Funkcia 'pack' prevedie prvok poľa 'n' do 32-bajtového poľa 'o'.
// Používa sa pri odosielaní alebo ukladaní výsledku skalárneho násobenia.
sv pack(uch *o, gf n);

// Skalárne násobenie bodu (crypto_scalarmult)
// Funkcia 'crypto_scalarmult' vypočíta n * p
// a uloží výsledok do 'q'. Je to kľúčová operácia v protokoloch
// výmeny kľúčov ako X25519 (používaný v TLS, Signal a i.).
int crypto_scalarmult(uch *q, const uch *n, const uch *p);

// Skalárne násobenie základného bodu (crypto_scalarmult_base)
// Funkcia 'crypto_scalarmult_base' vypočíta n * G,
// kde G je základný bod krivky (zvyčajne x = 9).
// Výsledok sa uloží do 'q' a používa sa na generovanie verejných kľúčov.
int crypto_scalarmult_base(uch *q, const uch *n);

#endif // ECC_H
