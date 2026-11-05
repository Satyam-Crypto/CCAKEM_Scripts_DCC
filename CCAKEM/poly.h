#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"

/*
 * Elements of R_q = Z_q[X]/(X^n + 1). Represents polynomial
 * coeffs[0] + X*coeffs[1] + X^2*coeffs[2] + ... + X^{n-1}*coeffs[n-1]
 */
typedef struct{
  int16_t coeffs[CCAKEM_N];
} poly;

#define poly_compress_lwr CCAKEM_NAMESPACE(poly_compress)
void poly_compress_lwr(uint8_t r[CCAKEM_POLYCOMPRESSEDBYTES], const poly *a);
#define poly_decompress_lwr CCAKEM_NAMESPACE(poly_decompress)
void poly_decompress_lwr(poly *r, const uint8_t a[CCAKEM_POLYCOMPRESSEDBYTES]);

#define poly_tobytes CCAKEM_NAMESPACE(poly_tobytes)
void poly_tobytes(uint8_t r[CCAKEM_POLYBYTES], const poly *a);
#define poly_frombytes CCAKEM_NAMESPACE(poly_frombytes)
void poly_frombytes(poly *r, const uint8_t a[CCAKEM_POLYBYTES]);

#define poly_frommsg CCAKEM_NAMESPACE(poly_frommsg)
void poly_frommsg(poly *r, const uint8_t msg[CCAKEM_INDCPA_MSGBYTES]);
#define poly_tomsg CCAKEM_NAMESPACE(poly_tomsg)
void poly_tomsg(uint8_t msg[CCAKEM_INDCPA_MSGBYTES], const poly *r);

#define poly_getnoise_mus CCAKEM_NAMESPACE(poly_getnoise_mus)
void poly_getnoise_mus(poly *r, const uint8_t seed[CCAKEM_SYMBYTES], uint8_t nonce);

#define poly_getnoise_mue CCAKEM_NAMESPACE(poly_getnoise_mue)
void poly_getnoise_mue(poly *r, const uint8_t seed[CCAKEM_SYMBYTES], uint8_t nonce);

#define poly_add CCAKEM_NAMESPACE(poly_add)
void poly_add(poly *r, const poly *a, const poly *b);
#define poly_sub CCAKEM_NAMESPACE(poly_sub)
void poly_sub(poly *r, const poly *a, const poly *b);

#endif
