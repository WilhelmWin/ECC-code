# ========================================================================
# Compiler and flags
# ========================================================================

CC = gcc
CFLAGS = -Wall -Wextra -O2 -fstack-protector-strong -fPIE -D_FORTIFY_SOURCE=2
LDFLAGS = -pie -Wl,-z,relro -Wl,-z,now

# ========================================================================
# Executable names
# ========================================================================

SERVER_TARGET = server       # Server application
CLIENT_TARGET = client       # Client application

# ========================================================================
# Directories
# ========================================================================

ASCON_DIR = ASCON            # Directory containing ASCON code

# ========================================================================
# Source and object files
# ========================================================================

ECC_SRC = ECC.c              # ECC module source file
ECC_OBJ = ECC.o              # Object file for ECC

ASCON_SRC = $(ASCON_DIR)/aead.c        # ASCON source file
ASCON_OBJ = $(ASCON_SRC:.c=.o)         # Corresponding object file

SERVER_SRC = server.c session.c drng.c error.c # Server source files
CLIENT_SRC = client.c session.c drng.c error.c # Client source files

SERVER_OBJ = $(SERVER_SRC:.c=.o)       # Object files for server
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)       # Object files for client
COMMON_OBJ = session.o drng.o error.o  # Common object files

# ========================================================================
# Static libraries
# ========================================================================

LIBECC = libecc.a             # ECC static library
LIBASCON = libascon.a         # ASCON static library
LIBRARIES = $(LIBECC) $(LIBASCON)  # List of libraries for linking

# ========================================================================
# Linker flags and delete command based on OS
# ========================================================================

ifeq ($(OS), Windows_NT)
    LDFLAGS = -lws2_32        # For Windows: add socket library
    RM = del /f /q            # Delete command for Windows
    NULL = nul                # Null device on Windows
else
    LDFLAGS =                 # For Unix-like systems: no extra flags
    RM = rm -f                # Delete command for Unix
    NULL = /dev/null          # Null device on Unix
endif

# ========================================================================
# Default rule (build entire project)
# ========================================================================

all: $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET) postbuild

# ========================================================================
# Build static libraries
# ========================================================================

$(LIBECC): $(ECC_OBJ)
	ar rcs $@ $^

$(LIBASCON): $(ASCON_OBJ)
	ar rcs $@ $^

# ========================================================================
# Build executables
# ========================================================================

$(SERVER_TARGET): $(SERVER_OBJ) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(CLIENT_TARGET): $(CLIENT_OBJ) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ========================================================================
# Pattern rule for compiling .c to .o
# ========================================================================

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ========================================================================
# Post-build cleanup (temporary files)
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
# Clean commands
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
