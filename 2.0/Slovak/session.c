#include "session.h"
#include "drng.h" // pre rdrand_get_bytes
#include "error.h"       // Na spracovanie chýb
// ========================================================================
// Funkcia na inicializáciu kontextu pre klient-server komunikáciu
// ========================================================================
void initializeContext(ClientServerContext *ctx) {
    // Inicializácia čísla portu na 0 (zatiaľ nebolo nastavené)
    ctx->portno = 0;

    // Inicializácia soketového deskriptora na 0 (soket ešte nebol otvorený)
    ctx->sockfd = 0;

    // Vynulovanie štruktúry adresy servera
    memset(&ctx->serv_addr, 0, sizeof(ctx->serv_addr));

    ctx->optval = 1; // Povolenie opätovného použitia adresy

    // Nastavenie ukazovateľa na server na NULL
    ctx->server = NULL;

    // Vynulovanie komunikačného buffera
    memset(ctx->buffer, 0, sizeof(ctx->buffer));

    // Nastavenie dĺžky buffera na 0
    ctx->bufferlen = 0;

    // Vynulovanie verejného kľúča klienta
    memset(ctx->client_public_key, 0, sizeof(ctx->client_public_key));

    // Vynulovanie verejného kľúča servera
    memset(ctx->server_public_key, 0, sizeof(ctx->server_public_key));

    // Opätovné vynulovanie verejného kľúča servera (duplicitné)
    memset(ctx->server_public_key, 0, sizeof(ctx->server_public_key));

    // Vynulovanie súkromného kľúča
    memset(ctx->private_key, 0, sizeof(ctx->private_key));

    // Vynulovanie zdieľaného tajomstva
    memset(ctx->shared_secret, 0, sizeof(ctx->shared_secret));

    // Vynulovanie buffera pre dešifrovanú správu
    memset(ctx->decrypted_msg, 0, sizeof(ctx->decrypted_msg));

    // Nastavenie dĺžky dešifrovanej správy na 0
    ctx->decrypted_msglen = 0;

    // Nastavenie bezpečnostného parametra nonce na NULL
    ctx->nsec = NULL;

    // Vynulovanie buffera pre zašifrovanú správu
    memset(ctx->encrypted_msg, 0, sizeof(ctx->encrypted_msg));

    // Nastavenie dĺžky zašifrovanej správy na 0
    ctx->encrypted_msglen = 0;

    // Nastavenie pevnej hodnoty pre nonce
    memcpy(ctx->npub, "simple_nonce_123", NONCE_SIZE);
}

// ========================================================================
// Funkcia na výpis dát v hexadecimálnom formáte
// ========================================================================
void hexdump(const uch *data, size_t length) {
    printf("\n");
    for (size_t i = 0; i < length; i++) {
        printf("%02x", data[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    if (length % 16 != 0)
        printf("\n");
    printf("\n");
}

// ========================================================================
// Funkcia na generovanie náhodného súkromného kľúča (256 bitov / 32 bajtov)
// ========================================================================
void generate_private_key(uch private_key[32]) {
    if (rdrand_get_bytes(32, (unsigned char *) private_key) < 32) {
        error("Nie je možné získať náhodné hodnoty");
    }

    printf("Súkromný kľúč: \n");
    hexdump(private_key, 32);
}
