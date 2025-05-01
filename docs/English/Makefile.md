# 📄 Build System Documentation 

## 🔍 Overview

This `Makefile` defines the rules for compiling and linking a 
client-server cryptographic application. It compiles the ECC and
ASCON cryptographic modules, builds static libraries from them,
and links them into `client` and `server` executables.

---

## 🔧 Compiler and Flags

```make
CC = gcc
CFLAGS = -Wall -Wextra -O2 -fstack-protector-strong -fPIE -D_FORTIFY_SOURCE=2
LDFLAGS = -pie -Wl,-z,relro -Wl,-z,now
```
- `CC`: The C compiler used (GCC).

- `CFLAGS`: Compiler flags for warnings, optimizations, and security:
  - `-Wall -Wextra`: Enable common and extra warnings.
  - `O2`: Apply level 2 optimizations.
  - `fstack-protector-strong`: Adds stack protection against overflows.
  - `fPIE + -pie`: Position-independent executables (for ASLR).
  - `D_FORTIFY_SOURCE=2`: Adds compile-time and runtime checks.

## 🎯 Target Executable Names
```make
SERVER_TARGET = server
CLIENT_TARGET = client
```
Defines the names of the final compiled binaries.

---

## 📁 Directory Configuration
```make
ASCON_DIR = ASCON
```
The ASCON cryptographic code resides in the ASCON directory.

---

## 🧩 Source and Object Files
### ECC
```make
ECC_SRC = ECC.c
ECC_OBJ = ECC.o
```
- Single ECC source file.
- Compiled to a single object file.

### ASCON
```make
ASCON_SRC = $(ASCON_DIR)/aead.c
ASCON_OBJ = $(ASCON_SRC:.c=.o)
```
- ASCON encryption/decryption source.
- Object file path is automatically derived.

### Server / Client Sources
```make
SERVER_SRC = server.c session.c drng.c error.c
CLIENT_SRC = client.c session.c drng.c error.c
```
- **Server uses**: `server.c`, shared `session.c`, and RNG `drng.c`, and errors `error.c`.
- **Client uses**: `client.c`, `session.c`, and `drng.c`, and errors `error.c`.
```make
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
COMMON_OBJ = session.o drng.o error.o
``` 
- Auto-generates object file names for each source file.

---
## 📚 Static Libraries
```make
LIBECC = libecc.a
LIBASCON = libascon.a
LIBRARIES = $(LIBECC) $(LIBASCON)
```
- Output static libraries from object files.
- Used when linking final executables.

---

## 🧷 Platform-Specific Configuration
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

Handles platform-dependent:

- Linker flags (Windows needs Winsock ws2_32)
- Remove (rm vs del)
- Null device redirection

## 🛠 Default Rule
```make
all: $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET) postbuild
```
Builds:
- Static libraries
- Server and client binaries
- Performs cleanup of intermediate files
- 
---

## 📦 Building Static Libraries
```make
$(LIBECC): $(ECC_OBJ)
	ar rcs $@ $^

$(LIBASCON): $(ASCON_OBJ)
	ar rcs $@ $^

    ar rcs: Archives object files into .a static libraries.
```
--- 
## 🔗 Linking Executables
```make
$(SERVER_TARGET): $(SERVER_OBJ) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(CLIENT_TARGET): $(CLIENT_OBJ) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
```

- Links each executable with its object files and required libraries.
- 
---
## 🧱 Object File Compilation Rule
```make
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
```
Pattern rule: `compiles .c` files to `.o` files.

---
## 🧼 Post-Build Cleanup
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
- Deletes intermediate and library files after successful build. Uses OS-specific paths and tools.
## 🧽 Cleaning Targets

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
- Removes all compiled files and executables.

### **`make distclean`**

```make
distclean:
ifeq ($(OS), Windows_NT)
	-$(RM) *~ *.bak
else
	$(RM) *~ *.bak
endif
```
- Removes backup and temporary files.
