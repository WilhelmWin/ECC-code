#===============================================================================
# Makefile for Secure Client-Server Communication Project
#===============================================================================

# Date: 31.03.2025
# Description:
# This Makefile is used to build a secure client-server communication system
# using elliptic curve cryptography (ECC) and Diffie-Hellman key exchange 
# protocol. The system consists of a server and client application that 
# communicate securely over a network. The key exchange process is based 
# on ECC, and messages exchanged between the server and client are encrypted
# using a shared secret derived from Diffie-Hellman.

# Components:
# - The server and client executables.
# - Static libraries for key generation, ECC operations, encryption, and common utilities.
# - Platform-specific code for cross-platform compatibility (Windows and Linux/Unix).
#
# This Makefile supports both Windows and Linux/Unix platforms by automatically
# adjusting the compilation flags and linking with the appropriate libraries.

# Key Features:
# - Static libraries for ECC, key generation, and encryption.
# - Cross-platform support for both Windows and Linux.
# - Platform-specific socket communication (Winsock for Windows and standard Unix sockets for Linux).
# - Modular structure for server, client, and cryptographic components.

# Targets:
# - `all`: Builds the server and client executables along with the required static libraries.
# - `libkeygen.a`: Builds the static library for key generation.
# - `libecc.a`: Builds the static library for elliptic curve cryptography (ECC) operations.
# - `libascon.a`: Builds the static library for encryption-related operations.
# - `libcommon.a`: Builds the static library for common utilities (e.g., print functions, error handling).
# - `server`: Builds the server executable.
# - `client`: Builds the client executable.
# - `clean`: Removes all compiled object files, libraries, and executables.
# - `distclean`: Removes all generated files, including backups and temporary files.

# Compiler:
# - GCC (GNU Compiler Collection) is used for compiling the C source files.

# Flags:
# - `-Wall`: Enables most warnings.
# - `-Wextra`: Enables additional warnings.
# - `-O2`: Optimizes the code for speed.
# - Windows-specific flag `-lws2_32` for linking with the Winsock library (on Windows).

# Usage:
# 1. Compile the project using the `make` command.
# 2. Build the server with `make server` and the client with `make client`.
# 3. Run the server and client to establish a secure communication.
# 4. Use `make clean` to remove all compiled files, and `make distclean` to remove backups as well.

# Platform-Specific Notes:
# - Windows: Requires the `ws2_32.lib` library for socket communication and uses Winsock for networking.
# - Linux/Unix: Uses standard socket libraries like `sys/socket.h` for networking.

#===============================================================================
# Compiler and flags
#===============================================================================

CC = gcc  # Set the compiler to GCC
CFLAGS = -Wall -Wextra -O2  # Enable most warnings and optimization

#===============================================================================
# Target executable names
#===============================================================================

SERVER_TARGET = server  # Define the server target executable
CLIENT_TARGET = client  # Define the client target executable
LIBRARIES = libkeygen.a libecc.a libascon.a libcommon.a  # List of static libraries

#===============================================================================
# Source files for each component
#===============================================================================

SOURCES = keygen2.c ECC.c ASCON.c drng.c common.c  # Source files for cryptography and common operations
SERVER_SOURCES = server.c  # Source file for the server
CLIENT_SOURCES = client.c  # Source file for the client
OBJECTS = $(SOURCES:.c=.o)  # Object files for common components
SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)  # Object files for the server
CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)  # Object files for the client

#===============================================================================
# Header files
#===============================================================================

HEADERS = keygen2.h ECC.h drng.h ASCON.h common.h variables.h  # Header files for the components

#===============================================================================
# Default rule: Build everything
#===============================================================================

all: $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET)  # Build all the targets

#===============================================================================
# Rule to build static libraries
#===============================================================================

libkeygen.a: keygen2.o drng.o
	ar rcs $@ $^

libecc.a: ECC.o
	ar rcs $@ $^

libascon.a: ASCON.o
	ar rcs $@ $^

libcommon.a: common.o ASCON.o
	ar rcs $@ $^

#===============================================================================
# Rule to build the server executable
#===============================================================================

$(SERVER_TARGET): $(SERVER_OBJECTS) $(OBJECTS) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#===============================================================================
# Rule to build the client executable
#===============================================================================

$(CLIENT_TARGET): $(CLIENT_OBJECTS) $(OBJECTS) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#===============================================================================
# Rule to compile object files from C sources
#===============================================================================

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

#===============================================================================
# Clean rule to remove all compiled files
#===============================================================================

clean:
	$(RM) $(OBJECTS) $(SERVER_OBJECTS) $(CLIENT_OBJECTS) $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET) $(SERVER_TARGET).exe $(CLIENT_TARGET).exe

#===============================================================================
# Distclean rule to remove all generated files, including backups
#===============================================================================

distclean: clean
	$(RM) *~ *.bak

#===============================================================================
# Windows-specific flags
#===============================================================================

ifeq ($(OS), Windows_NT)
    LDFLAGS = -lws2_32  # Link with Winsock library on Windows
    RM = del /f /q  # Windows remove command
endif
