# ECC Client-Server Secure Communication Project

## Description

This project implements a secure communication channel between a client and a server based on an elliptic curve key exchange algorithm (Curve25519) and AEAD encryption (ASCON). The project supports cross-platform operation on Windows and Linux.

## Key Features

- Key generation using the Curve25519 elliptic curve  
- Key exchange using the Diffie-Hellman protocol  
- Data encryption and decryption using the ASCON-128a AEAD scheme  
- Client-server architecture using sockets  
- Cross-platform compatibility (Windows/Linux)  

## Project Structure

- [1.0/](https://github.com/WilhelmWin/ECC-code/tree/master/1.0) — Version 1.0 of the project  
- [2.0/](https://github.com/WilhelmWin/ECC-code/tree/master/1.0) — Version 2.0 with multilingual support and source original archiv used code
- [docs/](https://github.com/WilhelmWin/ECC-code/tree/master/docs) — Documentation in multiple languages
- - [English](https://github.com/WilhelmWin/ECC-code/tree/master/docs/English) 
- - [Russian](https://github.com/WilhelmWin/ECC-code/tree/master/docs/Russian)  
- - [Slovak](https://github.com/WilhelmWin/ECC-code/tree/master/docs/Slovak)  
- [LICENSE.md](https://github.com/WilhelmWin/ECC-code/tree/master/LICENSE.md) — Project license  

## Requirements

- C compiler (gcc/clang/Visual Studio)  
- Development environment supporting C99 or later  
- Socket libraries (Winsock for Windows, POSIX sockets for Linux)  
- Make (for building on Linux) or equivalent setup on Windows  

## Build and Run

### Linux

```bash
cd 1.0/
make
./server 8080   # Run the server on port 8080
./client localhost 8080   # Run the client connecting to localhost on port 8080
```
## Windows

- Use Visual Studio or MinGW to compile.  
- Winsock support is included in the code for Windows.  
- Build and run `server.exe 8080` and `client.exe localhost 8080` similarly.  

## Usage

- Start the server with the port number argument, e.g.:  
  `./server 8080`  

- Start the client with the server address and port number, e.g.:  
  `./client localhost 8080`  

- The key exchange will take place and a secure channel will be established.  
- The client and server can exchange encrypted messages.  

## Main Components

- **ASCON/** — Implementation of ASCON-128a AEAD encryption  
- **ECC.c / ECC.h** — Functions for working with elliptic curves (Curve25519)  
- **client.c / server.c** — Client and server code with network logic  
- **session.c / session.h** — Session key management and exchange  
- **drng.c / drng.h** — Random number generation and cryptographic security  



## License

This project is licensed under the CC0 1.0 Universal License. See [`LICENSE.md`](https://github.com/WilhelmWin/ECC-code/tree/master/LICENSE.md) for details.
