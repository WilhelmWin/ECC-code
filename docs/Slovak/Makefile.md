# 📄 Dokumentácia k build systému

## 🔍 Prehľad

Tento `Makefile` definuje pravidlá na 
kompiláciu a prelinkovanie klient-server 
kryptografickej aplikácie. Kompiluje kryptografické 
moduly ECC a ASCON, vytvára z nich statické knižnice 
a linkuje ich do spustiteľných súborov `client` a `server`.

---

## 🔧 Kompilátor a príznaky

```make
CC = gcc
CFLAGS = -Wall -Wextra -O2 -fstack-protector-strong -fPIE -D_FORTIFY_SOURCE=2
LDFLAGS = -pie -Wl,-z,relro -Wl,-z,now
```
- `CC`: Použitý C kompilátor (GCC).
- `CFLAGS`: Príznaky pre kompilátor týkajúce sa výstrah, optimalizácie 
a bezpečnosti:
  - `Wall -Wextra`: Povolenie bežných a dodatočných výstrah.
  - `O2`: Optimalizácia na úrovni 2.
  - `fstack-protector-strong`: Ochrana zásobníka proti pretečeniu.
  - `fPIE + -pie`: Vytváranie pozíciovo nezávislých spustiteľných súborov (pre ASLR).
  - `D_FORTIFY_SOURCE=2`: Dopĺňa kontroly pri kompilácii a behu programu.

## 🎯 Názvy cieľových spustiteľných súborov
```make
SERVER_TARGET = server
CLIENT_TARGET = client
```
- Definuje názvy výsledných spustiteľných súborov.
## 📁 Konfigurácia adresárov
```make
ASCON_DIR = ASCON
```
- Kód pre ASCON kryptografiu sa nachádza v priečinku ASCON.
## 🧩 Zdrojové a objektové súbory
### ECC
```make
ECC_SRC = ECC.c
ECC_OBJ = ECC.o
```
- Jeden zdrojový súbor pre ECC.

- Kompiluje sa do jedného objektového súboru.

### ASCON
```make
ASCON_SRC = $(ASCON_DIR)/aead.c
ASCON_OBJ = $(ASCON_SRC:.c=.o)
```
- Zdrojový súbor pre šifrovanie/dešifrovanie pomocou ASCON.

- Cesta k objektovému súboru sa generuje automaticky.

### Server / Client
```make
SERVER_SRC = server.c session.c drng.c error.c
CLIENT_SRC = client.c session.c drng.c erroc.c
```
- Server používa: `server.c`, zdieľaný session.c, generátor 
náhodných čísel `drng.c` a errors `error.c`.

- Client používa: `client.c`,` session.c `, `drng.c` a `error.c`.
```make
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
COMMON_OBJ = session.o drng.o error.o
```
- Objektové súbory sa generujú automaticky z príslušných zdrojových súborov.

## 📚 Statické knižnice
```make
LIBECC = libecc.a
LIBASCON = libascon.a
LIBRARIES = $(LIBECC) $(LIBASCON)
```
- Výstupné statické knižnice zo skompilovaných objektových súborov.

- Používajú sa pri linkovaní finálnych spustiteľných súborov.

## 🧷 Konfigurácia podľa platformy
```make
ifeq ($(OS), Windows_NT)
    LDFLAGS = -lws2_32
    RM = del /f /q
    NULL = nul
else
    LDFLAGS =
    RM = rm -f
    NULL = /dev/null
endif
```
Rieši platformovo špecifické nastavenia:

- Linkovacie príznaky (Windows vyžaduje Winsock ws2_32)

- Príkaz na mazanie súborov (`rm` vs `del`)

- Presmerovanie na nulové zariadenie

## 🛠 Predvolený cieľ
```make
all: $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET) postbuild
```
Zabezpečí:

- Skompilovanie statických knižníc

- Zlinkovanie binárnych súborov servera a klienta

- Čistenie dočasných súborov po buildovaní

## 📦 Vytváranie statických knižníc
```make
$(LIBECC): $(ECC_OBJ)
	ar rcs $@ $^

$(LIBASCON): $(ASCON_OBJ)
	ar rcs $@ $^

    ar rcs: Archivuje objektové súbory do .a statických knižníc.
```
## 🔗 Linkovanie spustiteľných súborov
```make
$(SERVER_TARGET): $(SERVER_OBJ) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(CLIENT_TARGET): $(CLIENT_OBJ) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
```
- Linkuje každý spustiteľný súbor s jeho objektmi a požadovanými knižnicami.

## 🧱 Pravidlo pre kompiláciu objektových súborov
```make
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
```
- Vzorové pravidlo: kompiluje `.c` súbory do `.o` súborov.
## 🧼 Vyčistenie po buildovaní
```make
postbuild:
ifeq ($(OS), Windows_NT)
	-$(RM) ECC.o session.o drng.o error.o
	-$(RM) ASCON\\aead.o ASCON\\printstate.o
	-$(RM) server.o client.o
	-$(RM) $(LIBRARIES)
else
	$(RM) $(ECC_OBJ) $(COMMON_OBJ) $(ASCON_OBJ) $(SERVER_OBJ)
	$(RM) $(CLIENT_OBJ) $(LIBRARIES)
endif
```
- Odstraňuje dočasné a knižnicové súbory po úspešnom buildovaní.

- Používa rozdielne príkazy podľa OS.

## 🧽 Čistiace príkazy
### **`make clean`**
```make
clean:
ifeq ($(OS), Windows_NT)
	-$(RM) ASCON\\aead.o ASCON\\printstate.o
	-$(RM) $(LIBRARIES)
	-$(RM) $(SERVER_TARGET) $(CLIENT_TARGET) *.exe
else
	$(RM) $(ASCON_DIR)/*.o $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET)
	$(RM) $(ECC_OBJ) $(COMMON_OBJ)  $(SERVER_OBJ) $(CLIENT_OBJ)
endif
```
- Vymaže všetky kompilované súbory a spustiteľné súbory.

### **`make distclean`**
```make
distclean:
ifeq ($(OS), Windows_NT)
	-$(RM) *~ *.bak
else
	$(RM) *~ *.bak
endif
```
- Odstráni záložné a dočasné súbory (napr. *~, *.bak).

