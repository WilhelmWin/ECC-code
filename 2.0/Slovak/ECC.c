#include "ECC.h"

// Konštantný parameter eliptickej krivky (121665), používa sa pri
// násobení
static gf _121665 = {0xDB41, 1};

// Základný bod eliptickej krivky (používa sa v X25519)
static uch base[32] = {9};

// ========================================================================
// Prenosová operácia (carry) pre prvky eliptickej krivky (GF)
// ========================================================================

// Zabezpečuje, že všetky prvky poľa ostávajú v rozsahu [0, 2^16-1].
// Ak hodnota prekročí 16 bitov, prebytok sa prenesie do ďalšieho prvku.
// Je to dôležité pre správnosť aritmetiky v konečnom poli.
sv car(gf o)
{
    int i;
    lli c;
    for (i = 0; i < 16; i++) {
        o[i] += (1 << 16);  // Pridanie 2^16 na zabránenie záporným
                            // hodnotám
        c = o[i] >> 16;     // Výpočet prenosu (ak o[i] > 2^16)
        o[(i + 1) * (i < 15)] += c - 1 + 37 * (c - 1) * (i == 15);
        // Prenos do ďalšieho prvku, s korekciou pre posledný prvok
        o[i] -= c << 16;    // Odstránenie prenesenej časti, zanechanie
                            // spodných 16 bitov
    }
}

// ========================================================================
// Sčítanie dvoch prvkov Galoisovho poľa (GF)
// ========================================================================

// Po prvkoch sčíta dva polia a a b, výsledok uloží do o.
sv add(gf o, gf a, gf b)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = a[i] + b[i];
}

// ========================================================================
// Odčítanie dvoch prvkov poľa GF
// ========================================================================

// Po prvkoch odpočíta: o = a - b
sv sub(gf o, gf a, gf b)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = a[i] - b[i];
}

// ========================================================================
// Násobenie dvoch prvkov poľa GF (modulárne)
// ========================================================================

// Vykonáva „dlhé násobenie“ dvoch polí a výsledok redukuje podľa
// parametrov krivky.
sv mul(gf o, gf a, gf b)
{
    lli i, j, c[31];
    for (i = 0; i < 31; i++)
        c[i] = 0;
    for (i = 0; i < 16; i++)
        for (j = 0; j < 16; j++)
            c[i + j] += a[i] * b[j];  // Násobenie a akumulácia
    for (i = 16; i < 31; i++)
        c[i - 16] += 38 * c[i];  // Redukcia: násobenie zvyškov konštantou
    for (i = 0; i < 16; i++)
        o[i] = c[i];  // Medzivýsledok
    car(o);  // Normalizácia
    car(o);  // Znova pre istotu
}

// ========================================================================
// Inverzia (umocnenie na -1 v GF)
// ========================================================================

// Vypočíta inverzný prvok pomocou umocňovania a násobenia,
// optimalizované pre dané pole.
sv inv(gf o, gf i)
{
    gf c;
    int a;
    for (a = 0; a < 16; a++)
        c[a] = i[a];  // Kopírovanie vstupu
    for (a = 253; a >= 0; a--) {
        sq(c, c);  // Druhá mocnina
        if (a != 2 && a != 4)  // Preskočenie niektorých bitov kvôli
                               // optimalizácii
            mul(c, c, i);  // Násobenie so vstupom
    }
    for (a = 0; a < 16; a++)
        o[a] = c[a];  // Uloženie výsledku
}

// ========================================================================
// Podmienený výber medzi dvoma prvkami GF podľa príznaku
// ========================================================================

// Ak b == 1, vyberie sa p; inak q. Používa sa na ochranu proti analýze
// cez bočné kanály.
sv sel(gf p, gf q, int b)
{
    lli t, i, b1 = ~(b - 1);  // Premena b na bitovú masku: 0 -> 0x0,
                              // 1 -> 0xFFFFFFFF
    for (i = 0; i < 16; i++) {
        t = b1 & (p[i] ^ q[i]);  // XOR a maskovanie
        p[i] ^= t;  // Aktualizácia podľa príznaku
        q[i] ^= t;
    }
}

// ========================================================================
// Hlavná slučka násobenia skalárom – hlavná ECC operácia
// ========================================================================

// Implementuje algoritmus násobenia skalárom pomocou výberov a operácií
// v poli.
sv mainloop(lli x[32], uch *z)
{
    gf a, b, c, d, e, f;
    lli p, i;
    for (i = 0; i < 16; i++) {
        b[i] = x[i];       // Počiatočný stav
        d[i] = a[i] = c[i] = 0;
    }
    a[0] = d[0] = 1;  // Inicializácia bodov
    for (i = 254; i >= 0; --i) {
        p = (z[i >> 3] >> (i & 7)) & 1;  // Získanie i-teho bitu skalára z
        sel(a, b, p);
        sel(c, d, p);
        add(e, a, c);
        sub(a, a, c);
        add(c, b, d);
        sub(b, b, d);
        sq(d, e);
        sq(f, a);
        mul(a, c, a);
        mul(c, b, e);
        add(e, a, c);
        sub(a, a, c);
        sq(b, a);
        sub(c, d, f);
        mul(a, c, _121665);
        add(a, a, d);
        mul(c, c, a);
        mul(a, d, f);
        mul(d, b, x);
        sq(b, e);
        sel(a, b, p);
        sel(c, d, p);
    }
    for (i = 0; i < 16; i++) {
        x[i] = a[i];       // Výsledok do x
        x[i + 16] = c[i];  // Druhá časť výsledku
    }
}

// ========================================================================
// Rozbalenie bajtového poľa do GF prvku
// ========================================================================

// Prevedie 32 bajtov na 16 16-bitových prvkov (little-endian).
sv unpack(gf o, const uch *n)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = n[2 * i] + ((lli)n[2 * i + 1] << 8);
}

// ========================================================================
// Zbalenie GF prvku späť do bajtového poľa (32 bajtov)
// ========================================================================

// Normalizuje hodnoty a prevedie späť do bajtového formátu.
sv pack(uch *o, gf n)
{
    int i, j, b;
    gf m;
    car(n);
    car(n);
    car(n);
    for (j = 0; j < 2; j++) {
        m[0] = n[0] - 0xffed;
        for (i = 1; i < 15; i++) {
            m[i] = n[i] - 0xffff - ((m[i - 1] >> 16) & 1);
            m[i - 1] &= 0xffff;
        }
        m[15] = n[15] - 0x7fff - ((m[14] >> 16) & 1);
        b = (m[15] >> 16) & 1;
        m[14] &= 0xffff;
        sel(n, m, 1 - b);
    }
    for (i = 0; i < 16; i++) {
        o[2 * i] = n[i] & 0xff;
        o[2 * i + 1] = n[i] >> 8;
    }
}

// ========================================================================
// Hlavná funkcia: násobenie skaláru bodom q = n * p
// ========================================================================

// Prijíma skalár n a bod p, a vypočíta q = n * p na eliptickej krivke.
int crypto_scalarmult(uch *q, const uch *n, const uch *p)
{
    uch z[32];
    lli x[32];
    int i;
    for (i = 0; i < 31; ++i)
        z[i] = n[i];
    z[31] = (n[31] & 127) | 64;  // Nastavenie bitov podľa X25519
    z[0] &= 248;
    unpack(x, p);
    mainloop(x, z);
    inv(x + 16, x + 16);  // Inverzia súradnice Z
    mul(x, x, x + 16);    // Delenie X/Z
    pack(q, x);           // Zabalenie výsledku
    return 0;
}

// ========================================================================
// Násobenie skaláru základným bodom: q = n * G
// ========================================================================

// Jednoduchý obal nad hlavnou funkciou so zafixovaným základným bodom.
int crypto_scalarmult_base(uch *q, const uch *n)
{
    return crypto_scalarmult(q, n, base);
}
