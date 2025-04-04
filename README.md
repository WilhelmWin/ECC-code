# Secure Client-Server Communication Using ECC and Diffie-Hellman

## Overview

Based on the available information, study and utilize cryptographic algorithms for key exchange using the elliptic curve C25519, which are implemented in the compact cryptographic library TweetNaCl. Design, implement, and test secure communication for a client-server configuration in both Linux and Windows environments, where the cryptographic key exchange is realized using functions from the TweetNaCl library. Choose an appropriate symmetric encryption algorithm for encrypting the communication. Perform experiments using the C language and the GCC compiler for the Windows and Linux operating systems, and verify the functionality of the implemented solution across different operating systems. Also, prepare a set of demonstration applications and appropriate documentation on key exchange using the C25519 curve.

## Key Features

- **Elliptic Curve Cryptography (ECC)** for key generation and Diffie-Hellman key exchange.
- **Secure Communication** using a shared secret for encryption/decryption.
- **Custom Encryption Algorithm** (based on the Ascon algorithm).
- **Cross-Platform Support**: Works on both Windows and Linux.
- **Modular Codebase** using static libraries.
- **Graceful Termination** with a predefined "end word".

## Technologies Used

- ECC and Diffie-Hellman key exchange
- Custom encryption with Ascon
- TCP/IP socket communication (Winsock for Windows, standard sockets for Unix)
- C language with GCC and platform-specific Makefile

## File Descriptions

### Applications

- **`server.c`**: Initializes the server, generates ECC keys, establishes connection, performs Diffie-Hellman key exchange, and manages encrypted communication.
- **`client.c`**: Connects to the server, generates ECC keys, performs Diffie-Hellman key exchange, and handles encrypted message exchange.

### Cryptographic Modules

- **`keygen2.c` / `keygen2.h`**: Generates ECC private keys and random numbers.
- **`ECC.c` / `ECC.h`**: Contains core ECC operations and Diffie-Hellman computations.
- **`ASCON.c` / `ASCON.h`**: Implements encryption and decryption based on Ascon.
- **`drng.c` / `drng.h`**: Deterministic random number generation.
- **`common.c` / `common.h`**: Socket utilities, message handling, error reporting, etc.
- **`variables.h`**: Shared constants and macros.

## Building the Project

This project uses a **Makefile** that supports both Linux and Windows platforms. All source files are compiled using **GCC**. The Makefile automatically links the necessary static libraries and applies platform-specific flags.

### Makefile Features

- Builds static libraries: `libkeygen.a`, `libecc.a`, `libascon.a`, `libcommon.a`
- Cross-platform support (Windows uses `-lws2_32` for Winsock)
- Targets:
  - `make`: Builds all components
  - `make server`: Builds only the server
  - `make client`: Builds only the client
  - `make clean`: Removes object files and executables
  - `make distclean`: Removes all generated and backup files

## Linux/Unix Compilation

```bash
make
```
### To build only the server or client:

```bash
make server
make client
```
## Windows Compilation (MinGW or similar)
Make sure `make` and `gcc` are installed, then run:

```bash
make
```
💡 On Windows, the Makefile automatically links with `ws2_32` for Winsock support.

## Cleaning Build Artifacts

To clean compiled files:

```bash
make clean
```
To remove all build files including backups:

```bash
make distclean
```

## How to use?

Running the Applications

### Running the Server

```bash
./server <port_number>
```
The server will start and listen for incoming client connections on the specified port.

### Running the Client

```bash
./client <server_ip_address> <port_number>
```
The client connects to the server and begins secure communication using the shared secret key.

### Example Communication Flow

- Server generates its ECC private and public key.

- Client generates its ECC key pair.

- Both exchange public keys.

- Using Diffie-Hellman, they compute a shared secret.

- Messages are encrypted/decrypted using this shared secret.

- Communication continues until the predefined "end word" is sent to terminate.

## Troubleshooting

- Ensure both server and client use the same port.

- Make sure public keys are exchanged before sending encrypted messages.

- Check firewall, IP address, and network configuration.

- On Windows, verify that Winsock is initialized properly.

- Use `make clean` and recompile if there are build issues.

## How Interaction Looks

![How Interaction Looks](/images/img1.png)

![How Interaction Looks](/images/img2.png)



## The Final Word or the Problem or DDoS

The code implements a "final word" function, where communication between the server and client is terminated using a special word. In the early stages, this idea seemed sound, but upon further analysis, the main problem was identified. If this final word, even in its encrypted form, is intercepted by a third party, it could be used to attempt a forced termination of the server or client. The primary issue lies in the insufficient protection of this word in the current implementation. If someone learns or already knows the word, they can intentionally terminate communication between the server and client, representing a significant vulnerability. Despite this, this part of the code has been left for further study, as it illustrates an important security issue and raises questions about protecting key words in encryption systems and communication protocols.
