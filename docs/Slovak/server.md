# Dokumentácia: Server zabezpečenej výmeny správ
## 1. Popis

Tento modul implementuje zabezpečený server určený
na bezpečnú výmenu správ s klientom. Komunikácia
prebieha cez TCP protokol so šifrovaním založeným na
symetrickom kľúči, ktorý je odvodený pomocou
Diffie-Hellmanovej výmeny a generovaný pomocou Curve25519.
Šifrovanie a dešifrovanie prebieha pomocou ASCON128a
a zdieľaného tajomstva.
Zabezpečená je aj multiplatformová kompatibilita
(Windows a Linux).
## 2. Použitie
```c
./server <port>
```