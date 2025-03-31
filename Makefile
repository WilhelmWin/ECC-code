# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Target executable names
SERVER_TARGET = server
CLIENT_TARGET = client
LIBRARIES = libkeygen.a libecc.a libascon.a libcommon.a

# Source files for each component
SOURCES = keygen2.c ECC.c ASCON.c drng.c common.c
SERVER_SOURCES = server.c
CLIENT_SOURCES = client.c
OBJECTS = $(SOURCES:.c=.o)
SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)
CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)

# Header files
HEADERS = keygen2.h ECC.h drng.h ASCON.h common.h variables.h

# Default rule: build everything
all: $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET)

# Rule to build static libraries
libkeygen.a: keygen2.o drng.o
	ar rcs $@ $^

libecc.a: ECC.o
	ar rcs $@ $^

libascon.a: ASCON.o
	ar rcs $@ $^

libcommon.a: common.o ASCON.o
	ar rcs $@ $^

# Rule to build the server executable
$(SERVER_TARGET): $(SERVER_OBJECTS) $(OBJECTS) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to build the client executable
$(CLIENT_TARGET): $(CLIENT_OBJECTS) $(OBJECTS) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile object files from C sources
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove all compiled files
clean:
	$(RM) $(OBJECTS) $(SERVER_OBJECTS) $(CLIENT_OBJECTS) $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET) $(SERVER_TARGET).exe $(CLIENT_TARGET).exe

# Distclean rule to remove all generated files, including backups
distclean: clean
	$(RM) *~ *.bak

# Windows-specific flags
ifeq ($(OS), Windows_NT)
    LDFLAGS = -lws2_32
    RM = del /f /q
endif
