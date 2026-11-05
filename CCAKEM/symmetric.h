#ifndef SYMMETRIC_H
#define SYMMETRIC_H

#include <stddef.h>
#include <stdint.h>
#include "params.h"

#include "fips202.h"

typedef keccak_state xof_state;

#define ccakem_shake128_absorb CCAKEM_NAMESPACE(ccakem_shake128_absorb)
void ccakem_shake128_absorb(keccak_state *s,
                           const uint8_t seed[CCAKEM_SYMBYTES],
                           uint8_t x,
                           uint8_t y);

#define ccakem_shake256_prf CCAKEM_NAMESPACE(ccakem_shake256_prf)
void ccakem_shake256_prf(uint8_t *out, size_t outlen, const uint8_t key[CCAKEM_SYMBYTES], uint8_t nonce);

#define ccakem_shake256_rkprf CCAKEM_NAMESPACE(ccakem_shake256_rkprf)
void ccakem_shake256_rkprf(uint8_t out[CCAKEM_SSBYTES], const uint8_t key[CCAKEM_SYMBYTES], const uint8_t input[CCAKEM_CIPHERTEXTBYTES]);

#define XOF_BLOCKBYTES SHAKE128_RATE

#define hash_h(OUT, IN, INBYTES) sha3_256(OUT, IN, INBYTES)
#define hash_g(OUT, IN, INBYTES) sha3_512(OUT, IN, INBYTES)
#define xof_absorb(STATE, SEED, X, Y) ccakem_shake128_absorb(STATE, SEED, X, Y)
#define xof_squeezeblocks(OUT, OUTBLOCKS, STATE) shake128_squeezeblocks(OUT, OUTBLOCKS, STATE)
#define prf(OUT, OUTBYTES, KEY, NONCE) ccakem_shake256_prf(OUT, OUTBYTES, KEY, NONCE)
#define rkprf(OUT, KEY, INPUT) ccakem_shake256_rkprf(OUT, KEY, INPUT)

#endif /* SYMMETRIC_H */
