# ========================================================================
# Kompilátor a príznaky
# ========================================================================

CC = gcc
CFLAGS = -Wall -Wextra -O2 -fstack-protector-strong -fPIE -D_FORTIFY_SOURCE=2
LDFLAGS = -pie -Wl,-z,relro -Wl,-z,now

# ========================================================================
# Názvy spustiteľných súborov
# ========================================================================

SERVER_TARGET = server       # Serverová aplikácia
CLIENT_TARGET = client       # Klientská aplikácia

# ========================================================================
# Priečinky
# ========================================================================

ASCON_DIR = ASCON            # Priečinok obsahujúci ASCON kód

# ========================================================================
# Zdrojové a objektové súbory
# ========================================================================

ECC_SRC = ECC.c              # Zdrojový súbor pre ECC modul
ECC_OBJ = ECC.o              # Objektový súbor pre ECC

ASCON_SRC = $(ASCON_DIR)/aead.c        # Zdrojový súbor pre ASCON
ASCON_OBJ = $(ASCON_SRC:.c=.o)         # Zodpovedajúci objektový súbor

SERVER_SRC = server.c session.c drng.c error.c # Serverové zdrojové súbory
CLIENT_SRC = client.c session.c drng.c error.c # Klientské zdrojové súbory

SERVER_OBJ = $(SERVER_SRC:.c=.o)       # Objektové súbory pre server
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)       # Objektové súbory pre klienta
COMMON_OBJ = session.o drng.o error.o  # Spoločné objektové súbory

# ========================================================================
# Statické knižnice
# ========================================================================

LIBECC = libecc.a             # Statická knižnica ECC
LIBASCON = libascon.a         # Statická knižnica ASCON
LIBRARIES = $(LIBECC) $(LIBASCON)  # Zoznam knižníc pre linkovanie

# ========================================================================
# Linkovacie príznaky a príkazy na mazanie podľa OS
# ========================================================================

ifeq ($(OS), Windows_NT)
    LDFLAGS = -lws2_32        # Pre Windows: pridať knižnicu pre sockety
    RM = del /f /q            # Príkaz na mazanie pre Windows
    NULL = nul                # Null zariadenie vo Windows
else
    LDFLAGS =                 # Pre Unix systémy: bez ďalších príznakov
    RM = rm -f                # Príkaz na mazanie pre Unix
    NULL = /dev/null          # Null zariadenie v Unix systémoch
endif

# ========================================================================
# Predvolené pravidlo (kompilácia celého projektu)
# ========================================================================

all: $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET) postbuild

# ========================================================================
# Kompilácia statických knižníc
# ========================================================================

$(LIBECC): $(ECC_OBJ)
	ar rcs $@ $^

$(LIBASCON): $(ASCON_OBJ)
	ar rcs $@ $^

# ========================================================================
# Kompilácia spustiteľných súborov
# ========================================================================

$(SERVER_TARGET): $(SERVER_OBJ) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(CLIENT_TARGET): $(CLIENT_OBJ) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ========================================================================
# Šablóna pre kompiláciu .c → .o
# ========================================================================

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ========================================================================
# Upratovanie po kompilácii (dočasné súbory)
# ========================================================================

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

# ========================================================================
# Príkazy na čistenie
# ========================================================================

clean:
ifeq ($(OS), Windows_NT)
	-$(RM) ASCON\\aead.o ASCON\\printstate.o
	-$(RM) $(LIBRARIES)
	-$(RM) $(SERVER_TARGET) $(CLIENT_TARGET) *.exe
else
	$(RM) $(ASCON_DIR)/*.o $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET)
	$(RM) $(ECC_OBJ) $(COMMON_OBJ)  $(SERVER_OBJ) $(CLIENT_OBJ)
endif

distclean:
ifeq ($(OS), Windows_NT)
	-$(RM) *~ *.bak
else
	$(RM) *~ *.bak
endif
