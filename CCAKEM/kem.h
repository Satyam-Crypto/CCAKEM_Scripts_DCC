#ifndef KEM_H
#define KEM_H

#include <stdint.h>
#include "params.h"

#define CRYPTO_SECRETKEYBYTES  CCAKEM_SECRETKEYBYTES
#define CRYPTO_PUBLICKEYBYTES  CCAKEM_PUBLICKEYBYTES
#define CRYPTO_CIPHERTEXTBYTES CCAKEM_CIPHERTEXTBYTES
#define CRYPTO_BYTES           CCAKEM_SSBYTES

#if   (CCAKEM_K == 2)
#define CRYPTO_ALGNAME "CCAKEM512"
#elif (CCAKEM_K == 3)
#define CRYPTO_ALGNAME "CCAKEM768"
#elif (CCAKEM_K == 4)
#define CRYPTO_ALGNAME "CCAKEM1024"
#endif

#define crypto_kem_keypair_derand CCAKEM_NAMESPACE(keypair_derand)
int crypto_kem_keypair_derand(uint8_t *pk, uint8_t *sk, const uint8_t *coins);

#define crypto_kem_keypair CCAKEM_NAMESPACE(keypair)
int crypto_kem_keypair(uint8_t *pk, uint8_t *sk);

#define crypto_kem_enc_derand CCAKEM_NAMESPACE(enc_derand)
int crypto_kem_enc_derand(uint8_t *ct, uint8_t *ss, const uint8_t *pk, const uint8_t *coins);

#define crypto_kem_enc CCAKEM_NAMESPACE(enc)
int crypto_kem_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk);

#define crypto_kem_dec CCAKEM_NAMESPACE(dec)
int crypto_kem_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk);

#endif
