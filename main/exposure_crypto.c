
#include <string.h>
#include <stdio.h>
#include "mbedtls/md.h"
#include "mbedtls/hkdf.h"
#include "mbedtls/aes.h" // https://tls.mbed.org/api/aes_8h.html
#include "esp_system.h" //esp_fill_random



void testhkdf(void)
{

    mbedtls_md_info_t *sha256_info;
    sha256_info=mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

    mbedtls_hkdf(sha256_info, (uint8_t*) 0x00, 0x00, (uint8_t*)0x00, 0x00, (uint8_t*) 0x00, 0x00, (uint8_t*) 0x00, 0x00);    
}



void DeriveRPI()
{

char *payload = "Hello SHA 256 from ESP32learning";
uint8_t shaResult[32];
esp_fill_random(shaResult, 32);

mbedtls_md_context_t ctx;
mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

const size_t payloadLength = strlen(payload);

mbedtls_md_init(&ctx);
mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
mbedtls_md_starts(&ctx);
mbedtls_md_update(&ctx, (const unsigned char *) payload, payloadLength);
mbedtls_md_finish(&ctx, shaResult);
mbedtls_md_free(&ctx);
}

void encrypt_aes_ecb(char * plainText, char * key, unsigned char * outputBuffer){

  mbedtls_aes_context aes;

  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_enc( &aes, (const unsigned char*) key, strlen(key) * 8 );
  mbedtls_aes_crypt_ecb( &aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)plainText, outputBuffer);
  mbedtls_aes_free( &aes );
}

void encrypt_aes_ctr(char * plainText, char * key, unsigned char * outputBuffer){

  mbedtls_aes_context aes;

  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_enc( &aes, (const unsigned char*) key, strlen(key) * 8 );
  mbedtls_aes_crypt_ctr( &aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)plainText, outputBuffer);
  mbedtls_aes_free( &aes );
}
