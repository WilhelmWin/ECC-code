#===============================================================================
# Makefile for Secure Client-Server Communication Project
#===============================================================================

# Date: 31.03.2025
# Description:
# This Makefile is used to build a secure client-server communication 
# system using elliptic curve cryptography (ECC) and Diffie-Hellman 
# key exchange protocol. The system consists of a server and client 
# application that communicate securely over a network. The key exchange 
# process is based on ECC, and messages exchanged between the server 
# and client are encrypted using a shared secret derived from Diffie-
# Hellman.

# Components:
# - The server and client executables.
# - Static libraries for key generation, ECC operations, encryption, and 
# common utilities.
# - Platform-specific code for cross-platform compatibility (Windows and 
# Linux/Unix).
#
# This Makefile supports both Windows and Linux/Unix platforms by 
# automatically adjusting the compilation flags and linking with the 
# appropriate libraries.

# Key Features:
# - Static libraries for ECC, key generation, and encryption.
# - Cross-platform support for both Windows and Linux.
# - Platform-specific socket communication (Winsock for Windows and 
# standard Unix sockets for Linux).
# - Modular structure for server, client, and cryptographic components.

# Targets:
# - all: Builds the server and client executables along with the required 
# static libraries.
# - libkeygen.a: Builds the static library for key generation.
# - libecc.a: Builds the static library for elliptic curve cryptography 
# (ECC) operations.
# - libascon.a: Builds the static library for encryption-related operations.
# - libcommon.a: Builds the static library for common utilities (e.g., 
# print functions, error handling).
# - server: Builds the server executable.
# - client: Builds the client executable.
# - clean: Removes all compiled object files, libraries, and executables.
# - distclean: Removes all generated files, including backups and 
# temporary files.

# Compiler:
# - GCC (GNU Compiler Collection) is used for compiling the C source files.

# Flags:
# - -Wall: Enables most warnings.
# - -Wextra: Enables additional warnings.
# - -O2: Optimizes the code for speed.
# - Windows-specific flag -lws2_32 for linking with the Winsock library 
# (on Windows).

# Usage:
# 1. Compile the project using the make command.
# 2. Build the server with make server and the client with make client.
# 3. Run the server and client to establish a secure communication.
# 4. Use make clean to remove all compiled files, and make distclean to 
# remove backups as well.

# Platform-Specific Notes:
# - Windows: Requires the ws2_32.lib library for socket communication 
# and uses Winsock for networking.
# - Linux/Unix: Uses standard socket libraries like sys/socket.h for 
# networking.

#===============================================================================
# Compiler and flags
#===============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -O2

#===============================================================================
# Target executable names
#===============================================================================

SERVER_TARGET = server
CLIENT_TARGET = client

#===============================================================================
# Directories
#===============================================================================

ASCON_DIR = ASCON

#===============================================================================
# Source and object files
#===============================================================================

KEYGEN_SRC = keygen2.c drng.c
KEYGEN_OBJ = keygen2.o drng.o

ECC_SRC = ECC.c
ECC_OBJ = ECC.o

COMMON_SRC = common.c
COMMON_OBJ = common.o

ASCON_SRC = $(ASCON_DIR)/aead.c $(ASCON_DIR)/printstate.c
ASCON_OBJ = $(ASCON_SRC:.c=.o)

SERVER_SRC = server.c
SERVER_OBJ = $(SERVER_SRC:.c=.o)

CLIENT_SRC = client.c
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

#===============================================================================
# Libraries
#===============================================================================

LIBKEYGEN = libkeygen.a
LIBECC = libecc.a
LIBASCON = libascon.a
LIBCOMMON = libcommon.a
LIBRARIES = $(LIBKEYGEN) $(LIBECC) $(LIBASCON) $(LIBCOMMON)

#===============================================================================
# Default rule
#===============================================================================

all: $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET)

#===============================================================================
# Static libraries
#===============================================================================

$(LIBKEYGEN): $(KEYGEN_OBJ)
	ar rcs $@ $^

$(LIBECC): $(ECC_OBJ)
	ar rcs $@ $^

$(LIBASCON): $(ASCON_OBJ)
	ar rcs $@ $^

$(LIBCOMMON): $(COMMON_OBJ)
	ar rcs $@ $^

#===============================================================================
# Executables
#===============================================================================

$(SERVER_TARGET): $(SERVER_OBJ) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(CLIENT_TARGET): $(CLIENT_OBJ) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#===============================================================================
# Pattern rule for object files
#===============================================================================

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#===============================================================================
# Clean rules
#===============================================================================

clean:
	$(RM) -f *.o $(ASCON_DIR)/*.o $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET) *.exe

distclean: clean
	$(RM) -f *~ *.bak

#===============================================================================
# Windows-specific settings
#===============================================================================

ifeq ($(OS), Windows_NT)
    LDFLAGS = -lws2_32
    RM = del /f /q
else
    RM = rm
endif
