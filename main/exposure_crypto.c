
#include <string.h>
#include <stdio.h>
#include "mbedtls/md.h"
#include "mbedtls/hkdf.h"
#include "mbedtls/aes.h" // https://tls.mbed.org/api/aes_8h.html
#include "esp_system.h" //esp_fill_random



void hkdf(uint8_t *TEK, char *info, uint8_t *result)
{
    
    mbedtls_md_info_t *sha256_info;
    sha256_info=mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    mbedtls_hkdf(sha256_info, NULL, 0, (unsigned char *)TEK, 16, (unsigned char *)info, strlen(info), (unsigned char *)result, 16);    
}

void encrypt_aes_ecb(uint8_t *plainText, uint8_t *key, uint8_t *outputBuffer){

  mbedtls_aes_context aes;

  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_enc( &aes, (uint8_t *) key, 128 );
  mbedtls_aes_crypt_ecb( &aes, MBEDTLS_AES_ENCRYPT, (uint8_t *)plainText, (uint8_t *)outputBuffer);
  mbedtls_aes_free( &aes );
}

void encrypt_aes_ctr(uint8_t * plainText, uint8_t  * key, uint8_t * nonce, uint8_t* outputBuffer){
    uint8_t stream_block[16];
    size_t nc_off=0;

    mbedtls_aes_context aes;
    mbedtls_aes_init( &aes );
    mbedtls_aes_setkey_enc( &aes, (uint8_t *) key, 128 );
    //esp_aes_crypt_ctr(&aes, plainText_len, 0, nonce_counter, stream_block, plainText, encryptText); //encrypt
    mbedtls_aes_crypt_ctr( &aes, strlen((char *)plainText), &nc_off, nonce, stream_block, plainText, outputBuffer);
    mbedtls_aes_free( &aes );
}
