# ========================================================================
# Kompilátor a príznaky
# ========================================================================

CC = gcc
ifeq ($(OS), Windows_NT)
CFLAGS = -Wall -Wextra -O2 -fstack-protector-strong -fPIE
CFLAGS = -D_FORTIFY_SOURCE=2
LDFLAGS = -pie -Wl,-z,relro -Wl,-z,now -lws2_32 -lwinmm
else
CFLAGS = -Wall -Wextra -O2 -fstack-protector-strong -fPIE
CFLAGS = -D_FORTIFY_SOURCE=2 `sdl2-config --cflags`
LDFLAGS = -pie -Wl,-z,relro -Wl,-z,now `sdl2-config --libs` -lSDL2_mixer
endif

# ========================================================================
# Názvy cieľových spustiteľných súborov
# ========================================================================

SERVER_TARGET = server
CLIENT_TARGET = client

# ========================================================================
# Adresáre
# ========================================================================

ASCON_DIR = ASCON

# ========================================================================
# Zdrojové a objektové súbory
# ========================================================================

ECC_SRC = ECC.c
ECC_OBJ = ECC.o

ASCON_SRC = $(ASCON_DIR)/aead.c
ASCON_OBJ = $(ASCON_SRC:.c=.o)

SERVER_SRC = server.c session.c drng.c error.c
CLIENT_SRC = client.c session.c drng.c error.c

SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

COMMON_OBJ = session.o drng.o error.o

# ========================================================================
# Knižnice
# ========================================================================

LIBECC = libecc.a
LIBASCON = libascon.a
LIBRARIES = $(LIBECC) $(LIBASCON)

# ========================================================================
# Linker flagy a príkaz na vymazanie podľa OS
# ========================================================================

ifeq ($(OS), Windows_NT)
    LDFLAGS = -lws2_32 -lwinmm
    RM = del /f /q
    NULL = nul
else
    LDFLAGS += `sdl2-config --libs` -lSDL2_mixer
    RM = rm -f
    NULL = /dev/null
endif

# ========================================================================
# Predvolené pravidlo
# ========================================================================

all: $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET) postbuild

# ========================================================================
# Statické knižnice
# ========================================================================

$(LIBECC): $(ECC_OBJ)
	ar rcs $@ $^

$(LIBASCON): $(ASCON_OBJ)
	ar rcs $@ $^

# ========================================================================
# Spustiteľné súbory
# ========================================================================

$(SERVER_TARGET): $(SERVER_OBJ) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(CLIENT_TARGET): $(CLIENT_OBJ) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ========================================================================
# Vzorové pravidlo pre objektové súbory
# ========================================================================

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ========================================================================
# Čistenie po zostavení
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
# Pravidlá na čistenie
# ========================================================================

clean:
ifeq ($(OS), Windows_NT)
	-$(RM) ASCON\\aead.o ASCON\\printstate.o
	-$(RM) $(LIBRARIES)
	-$(RM) $(SERVER_TARGET) $(CLIENT_TARGET) *.exe
else
	$(RM) $(ASCON_DIR)/*.o $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET)
	$(RM) $(ECC_OBJ) $(COMMON_OBJ) $(SERVER_OBJ) $(CLIENT_OBJ)
endif

distclean:
ifeq ($(OS), Windows_NT)
	-$(RM) *~ *.bak
else
	$(RM) *~ *.bak
endif

