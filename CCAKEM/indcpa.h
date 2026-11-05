#ifndef INDCPA_H
#define INDCPA_H

#include <stdint.h>
#include "params.h"
#include "polyvec.h"

#define gen_matrix CCAKEM_NAMESPACE(gen_matrix)
void gen_matrix(polyvec *a, const uint8_t seed[CCAKEM_SYMBYTES], uint8_t transposed);

#define indcpa_keypair_derand CCAKEM_NAMESPACE(indcpa_keypair_derand)
void indcpa_keypair_derand(uint8_t pk[CCAKEM_INDCPA_PUBLICKEYBYTES],
                           uint8_t sk[CCAKEM_INDCPA_SECRETKEYBYTES],
                           const uint8_t coins[CCAKEM_SYMBYTES]);

#define indcpa_enc CCAKEM_NAMESPACE(indcpa_enc)
void indcpa_enc(uint8_t c[CCAKEM_INDCPA_BYTES],
                const uint8_t m[CCAKEM_INDCPA_MSGBYTES],
                const uint8_t pk[CCAKEM_INDCPA_PUBLICKEYBYTES],
                const uint8_t coins[CCAKEM_SYMBYTES]);

#define indcpa_dec CCAKEM_NAMESPACE(indcpa_dec)
void indcpa_dec(uint8_t m[CCAKEM_INDCPA_MSGBYTES],
                const uint8_t c[CCAKEM_INDCPA_BYTES],
                const uint8_t sk[CCAKEM_INDCPA_SECRETKEYBYTES]);

#endif
