# ====================================================
# Makefile
# ====================================================
#
# Date: 2025-04-23
#
# Description:
# This Makefile is designed for building the ECC client-server
# application, which uses elliptic curve cryptography (ECC) and
# ASCON encryption. It automates the process of compiling
# source files, creating static libraries, and linking the final
# executables for both the server and client components of the
# project. The Makefile supports both Windows and Linux platforms.
#
# Key Features:
# - Compilation of ECC and ASCON source files into static libraries.
# - Creation of the server and client executables.
# - Platform-specific handling for Windows and Linux.
# - Post-build cleanup to remove object files and temporary files.
# - Clean and distclean rules to remove generated files.
#
# Libraries:
# - libecc.a: Static library for ECC-related operations.
# - libascon.a: Static library for ASCON encryption operations.
#
# Platform Dependencies:
# This Makefile works with both Windows (using MinGW or MSYS)
# and Linux environments. The proper flags and tools are selected
# based on the platform, ensuring compatibility across systems.
#
# Arguments and Functionality:
# - The `make` command will use this Makefile to compile the project
#   and generate the desired executables and libraries.
# - The default target (`all`) will build the necessary libraries
#   and executables for the server and client applications.
# - The `clean` target will remove all object files and executables,
#   ensuring a fresh build.
# - The `distclean` target will remove additional backup files and
#   temporary files created during the build process.
#
# Example Usage:
# To build the entire project, simply run:
#   make
# To clean the build files:
#   make clean
# To remove all temporary files and backups:
#   make distclean
#
# Platform-specific instructions:
# - On Windows, the Makefile uses MinGW/MSYS tools for compiling and
#   linking. The `-lws2_32` library is linked for Winsock support.
# - On Linux, standard GCC flags and tools are used without the
#   need for additional libraries.


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

ECC_SRC = ECC.c
ECC_OBJ = ECC.o

ASCON_SRC = $(ASCON_DIR)/aead.c $(ASCON_DIR)/printstate.c
ASCON_OBJ = $(ASCON_SRC:.c=.o)

SERVER_SRC = server.c session.c drng.c# Добавляем session.c
SERVER_OBJ = $(SERVER_SRC:.c=.o)

CLIENT_SRC = client.c session.c drng.c
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

#===============================================================================
# Libraries
#===============================================================================

LIBECC = libecc.a
LIBASCON = libascon.a
LIBRARIES =  $(LIBECC) $(LIBASCON)

#===============================================================================
# Default rule
#===============================================================================

all: $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET)

#===============================================================================
# Static libraries
#===============================================================================

$(LIBECC): $(ECC_OBJ)
	ar rcs $@ $^

$(LIBASCON): $(ASCON_OBJ)
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
# Post-build cleanup
#===============================================================================
postbuild:
	$(RM) -f $(ECC_OBJ) $(COMMON_OBJ) $(ASCON_OBJ) \
	        $(SERVER_OBJ) $(CLIENT_OBJ) session.o \
	        $(LIBRARIES)

#===============================================================================
# Default rule
#===============================================================================

all: $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET)
	$(MAKE) postbuild

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
