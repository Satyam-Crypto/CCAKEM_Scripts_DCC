#include <stdint.h>
#include "params.h"
#include "poly.h"
#include "cbd.h"
#include "symmetric.h"
#include "verify.h"

/*************************************************
* Name:        poly_compress
*
* Description: Compression and subsequent serialization of a polynomial
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (of length CCAKEM_POLYCOMPRESSEDBYTES)
*              - const poly *a: pointer to input polynomial
**************************************************/
void poly_compress_lwr(uint8_t r[CCAKEM_POLYCOMPRESSEDBYTES], const poly *a)
{
  unsigned int i,j;
  int16_t u;
  uint8_t t[8];

#if (LOG_P == 4)    /*i.e., (CCAKEM_POLYCOMPRESSEDBYTES == 128)*/

  for(i=0;i<CCAKEM_N/8;i++) {
    for(j=0;j<8;j++) {
      u  = a->coeffs[8*i+j];
      u += (u >> 15) & CCAKEM_Q;
      // compress to 4 bits
      t[j] = (u + (1U << (LOG_Q-5))) >> (LOG_Q-4);
      t[j] &= 0xf;
    }

    r[0] = t[0] | (t[1] << 4);
    r[1] = t[2] | (t[3] << 4);
    r[2] = t[4] | (t[5] << 4);
    r[3] = t[6] | (t[7] << 4);
    r += 4;
  }
#elif (LOG_P == 5)          /*(CCAKEM_POLYCOMPRESSEDBYTES == 160)*/
  for(i=0;i<CCAKEM_N/8;i++) {
    for(j=0;j<8;j++) {
      u  = a->coeffs[8*i+j];
      u += (u >> 15) & CCAKEM_Q;
      t[j] = (u + (1U << (LOG_Q-6))) >> (LOG_Q-5);
      t[j] &= 0x1f;
    }

    r[0] = (t[0] >> 0) | (t[1] << 5);
    r[1] = (t[1] >> 3) | (t[2] << 2) | (t[3] << 7);
    r[2] = (t[3] >> 1) | (t[4] << 4);
    r[3] = (t[4] >> 4) | (t[5] << 1) | (t[6] << 6);
    r[4] = (t[6] >> 2) | (t[7] << 3);
    r += 5;
  }
#else
#error "LOG_P needs to be in {4,5} and CCAKEM_POLYCOMPRESSEDBYTES needs to be in {128, 160}"
#endif
}

/*************************************************
* Name:        poly_decompress
*
* Description: De-serialization and subsequent decompression of a polynomial;
*              approximate inverse of poly_compress
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: pointer to input byte array
*                                  (of length CCAKEM_POLYCOMPRESSEDBYTES bytes)
**************************************************/
void poly_decompress_lwr(poly *r, const uint8_t a[CCAKEM_POLYCOMPRESSEDBYTES])
{
  unsigned int i;

#if (LOG_P == 4)            /*(CCAKEM_POLYCOMPRESSEDBYTES == 128)*/
  for(i=0;i<CCAKEM_N/2;i++) {
    r->coeffs[2*i+0] = ((uint16_t)(a[0] & 15)) << (LOG_Q - 4);
    r->coeffs[2*i+1] = ((uint16_t)(a[0] >> 4)) << (LOG_Q - 4);
    a += 1;
  }
#elif (LOG_P == 5)            /*(CCAKEM_POLYCOMPRESSEDBYTES == 160)*/
  unsigned int j;
  uint8_t t[8];
  for(i=0;i<CCAKEM_N/8;i++) {
    t[0] = (a[0] >> 0);
    t[1] = (a[0] >> 5) | (a[1] << 3);
    t[2] = (a[1] >> 2);
    t[3] = (a[1] >> 7) | (a[2] << 1);
    t[4] = (a[2] >> 4) | (a[3] << 4);
    t[5] = (a[3] >> 1);
    t[6] = (a[3] >> 6) | (a[4] << 2);
    t[7] = (a[4] >> 3);
    a += 5;

    for(j=0;j<8;j++)
      r->coeffs[8*i+j] = (uint32_t)(t[j] & 31) << (LOG_Q - 5);
  }
#else
#error "LOG_P needs to be in {4,5} and CCAKEM_POLYCOMPRESSEDBYTES needs to be in {128, 160}"
#endif
}

/*************************************************
* Name:        poly_tobytes
*
* Description: Serialization of a polynomial
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (needs space for CCAKEM_POLYBYTES bytes)
*              - const poly *a: pointer to input polynomial
**************************************************/
void poly_tobytes(uint8_t r[CCAKEM_POLYBYTES], const poly *a)
{
  unsigned int i;

#if (LOG_Q == 11)
  uint16_t t[8];
  for(i = 0; i < CCAKEM_N/8; i++) {
    for(int j = 0; j < 8; j++) {
      t[j] = a->coeffs[8*i + j];
      t[j] += ((int16_t)t[j] >> 15) & CCAKEM_Q;
    }

    r[11*i+0] = (t[0] >> 0);
    r[11*i+1] = (t[0] >> 8) | (t[1] << 3);
    r[11*i+2] = (t[1] >> 5) | (t[2] << 6);
    r[11*i+3] = (t[2] >> 2);
    r[11*i+4] = (t[2] >> 10) | (t[3] << 1);
    r[11*i+5] = (t[3] >> 7) | (t[4] << 4);
    r[11*i+6] = (t[4] >> 4) | (t[5] << 7);
    r[11*i+7] = (t[5] >> 1);
    r[11*i+8] = (t[5] >> 9) | (t[6] << 2);
    r[11*i+9] = (t[6] >> 6) | (t[7] << 5);
    r[11*i+10] = (t[7] >> 3);
  }

#elif (LOG_Q == 12)
  uint16_t t0, t1;

  for(i=0;i<CCAKEM_N/2;i++) {
    // map to positive standard representatives
    t0  = a->coeffs[2*i];
    t0 += ((int16_t)t0 >> 15) & CCAKEM_Q;

    t1  = a->coeffs[2*i+1];
    t1 += ((int16_t)t1 >> 15) & CCAKEM_Q;

    r[3*i+0] = (t0 >> 0);
    r[3*i+1] = (t0 >> 8) | (t1 << 4);
    r[3*i+2] = (t1 >> 4);
  }

#else
#error "Current implementation only supports LOG_Q = 11 or 12"
#endif
}

/*************************************************
* Name:        poly_frombytes
*
* Description: De-serialization of a polynomial;
*              inverse of poly_tobytes
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: pointer to input byte array
*                                  (of CCAKEM_POLYBYTES bytes)
**************************************************/
void poly_frombytes(poly *r, const uint8_t a[CCAKEM_POLYBYTES])
{
  unsigned int i;

#if (LOG_Q == 11)

  for(i = 0; i < CCAKEM_N/8; i++) {
    r->coeffs[8*i+0] = ((uint16_t)a[11*i+0] | ((uint16_t)a[11*i+1] << 8)) & 0x7FF;
    r->coeffs[8*i+1] = (((uint16_t)a[11*i+1] >> 3) | ((uint16_t)a[11*i+2] << 5)) & 0x7FF;
    r->coeffs[8*i+2] = (((uint16_t)a[11*i+2] >> 6) | ((uint16_t)a[11*i+3] << 2) | ((uint16_t)a[11*i+4] << 10)) & 0x7FF;
    r->coeffs[8*i+3] = (((uint16_t)a[11*i+4] >> 1) | ((uint16_t)a[11*i+5] << 7)) & 0x7FF;
    r->coeffs[8*i+4] = (((uint16_t)a[11*i+5] >> 4) | ((uint16_t)a[11*i+6] << 4)) & 0x7FF;
    r->coeffs[8*i+5] = (((uint16_t)a[11*i+6] >> 7) | ((uint16_t)a[11*i+7] << 1) | ((uint16_t)a[11*i+8] << 9)) & 0x7FF;
    r->coeffs[8*i+6] = (((uint16_t)a[11*i+8] >> 2) | ((uint16_t)a[11*i+9] << 6)) & 0x7FF;
    r->coeffs[8*i+7] = (((uint16_t)a[11*i+9] >> 5) | ((uint16_t)a[11*i+10] << 3)) & 0x7FF;
  }

#elif (LOG_Q == 12)

  for(i = 0; i < CCAKEM_N/2; i++) {
    r->coeffs[2*i]   = ((a[3*i+0] >> 0) | ((uint16_t)a[3*i+1] << 8)) & 0xFFF;
    r->coeffs[2*i+1] = ((a[3*i+1] >> 4) | ((uint16_t)a[3*i+2] << 4)) & 0xFFF;
  }

#else
#error "Current implementation only supports LOG_Q = 11 or 12"
#endif
}

/*************************************************
* Name:        poly_frommsg
*
* Description: Convert 32-byte message to polynomial
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *msg: pointer to input message
**************************************************/
void poly_frommsg(poly *r, const uint8_t msg[CCAKEM_INDCPA_MSGBYTES])
{
  unsigned int i,j;

#if (CCAKEM_INDCPA_MSGBYTES != CCAKEM_N/8)
#error "CCAKEM_INDCPA_MSGBYTES must be equal to CCAKEM_N/8 bytes!"
#endif

  for(i=0;i<CCAKEM_N/8;i++) {
    for(j=0;j<8;j++) {
      r->coeffs[8*i+j] = 0;
      cmov_int16(&r->coeffs[8*i+j], (1 << (LOG_Q-1)), (msg[i] >> j) & 1);
    }
  }
}
/*************************************************
* Name:        poly_tomsg
*
* Description: Convert polynomial to 32-byte message
*
* Arguments:   - uint8_t *msg: pointer to output message
*              - const poly *a: pointer to input polynomial
**************************************************/

void poly_tomsg(uint8_t msg[CCAKEM_INDCPA_MSGBYTES], const poly *a)
{
  unsigned int i,j;
  uint32_t t;

  for(i=0;i<CCAKEM_N/8;i++) {
    msg[i] = 0;

    for(j=0;j<8;j++) {
      t = a->coeffs[8*i+j];

#if (LOG_Q == 11) || (LOG_Q == 12)
      t <<= 1;                    // 2t
      t += (1 << (LOG_Q-1));      // + q/2
      t >>= LOG_Q;                // divide by q
      t &= 1;
#else
#error "LOG_Q must be 11 or 12"
#endif

      msg[i] |= t << j;
    }
  }
}

/*************************************************
* Name:        poly_getnoise_mus
*
* Description: Sample a polynomial deterministically from a seed and a nonce,
*              with output polynomial close to centered binomial distribution
*              with parameter CCAKEM_MUS
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *seed: pointer to input seed
*                                     (of length CCAKEM_SYMBYTES bytes)
*              - uint8_t nonce: one-byte input nonce
**************************************************/
void poly_getnoise_mus(poly *r, const uint8_t seed[CCAKEM_SYMBYTES], uint8_t nonce)
{
  uint8_t buf[CCAKEM_MUS*CCAKEM_N/4];
  prf(buf, sizeof(buf), seed, nonce);
  poly_cbd_mus(r, buf);
}

/*************************************************
* Name:        poly_getnoise_mue
*
* Description: Sample a polynomial deterministically from a seed and a nonce,
*              with output polynomial close to centered binomial distribution
*              with parameter CCAKEM_MUE
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *seed: pointer to input seed
*                                     (of length CCAKEM_SYMBYTES bytes)
*              - uint8_t nonce: one-byte input nonce
**************************************************/
void poly_getnoise_mue(poly *r, const uint8_t seed[CCAKEM_SYMBYTES], uint8_t nonce)
{
  uint8_t buf[CCAKEM_MUE*CCAKEM_N/4];
  prf(buf, sizeof(buf), seed, nonce);
  poly_cbd_mue(r, buf);
}

/*************************************************
* Name:        poly_add
*
* Description: Add two polynomials; no modular reduction is performed
*
* Arguments: - poly *r: pointer to output polynomial
*            - const poly *a: pointer to first input polynomial
*            - const poly *b: pointer to second input polynomial
**************************************************/
void poly_add(poly *r, const poly *a, const poly *b)
{
  unsigned int i;
  for(i=0;i<CCAKEM_N;i++)
    r->coeffs[i] = a->coeffs[i] + b->coeffs[i];
}

/*************************************************
* Name:        poly_sub
*
* Description: Subtract two polynomials; no modular reduction is performed
*
* Arguments: - poly *r:       pointer to output polynomial
*            - const poly *a: pointer to first input polynomial
*            - const poly *b: pointer to second input polynomial
**************************************************/
void poly_sub(poly *r, const poly *a, const poly *b)
{
  unsigned int i;
  for(i=0;i<CCAKEM_N;i++)
    r->coeffs[i] = a->coeffs[i] - b->coeffs[i];
}
