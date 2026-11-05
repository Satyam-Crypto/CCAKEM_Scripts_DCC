#include <stdint.h>
#include "params.h"
#include "poly.h"
#include "polyvec.h"

/*************************************************
* Name:        polyvec_compress
*
* Description: Compress and serialize vector of polynomials
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (needs space for CCAKEM_POLYVECCOMPRESSEDBYTES)
*              - const polyvec *a: pointer to input vector of polynomials
**************************************************/
void polyvec_compress(uint8_t r[CCAKEM_POLYVECCOMPRESSEDBYTES], const polyvec *a)
{
  unsigned int i,j,k;
  uint64_t d0;

#if (CCAKEM_POLYVECCOMPRESSEDBYTES == (CCAKEM_K * 352))
  uint16_t t[8];
  for(i=0;i<CCAKEM_K;i++) {
    for(j=0;j<CCAKEM_N/8;j++) {
      for(k=0;k<8;k++) {
        t[k]  = a->vec[i].coeffs[8*j+k];
        t[k] += ((int16_t)t[k] >> 15) & CCAKEM_Q;
        d0 = (t[k] + (1U << (LOG_Q - 12))) >> (LOG_Q - 11);
        t[k] = d0 & 0x7ff;
      }

      r[ 0] = (t[0] >>  0);
      r[ 1] = (t[0] >>  8) | (t[1] << 3);
      r[ 2] = (t[1] >>  5) | (t[2] << 6);
      r[ 3] = (t[2] >>  2);
      r[ 4] = (t[2] >> 10) | (t[3] << 1);
      r[ 5] = (t[3] >>  7) | (t[4] << 4);
      r[ 6] = (t[4] >>  4) | (t[5] << 7);
      r[ 7] = (t[5] >>  1);
      r[ 8] = (t[5] >>  9) | (t[6] << 2);
      r[ 9] = (t[6] >>  6) | (t[7] << 5);
      r[10] = (t[7] >>  3);
      r += 11;
    }
  }
#elif (CCAKEM_POLYVECCOMPRESSEDBYTES == (CCAKEM_K * 320))
  uint16_t t[4];
  for(i=0;i<CCAKEM_K;i++) {
    for(j=0;j<CCAKEM_N/4;j++) {
      for(k=0;k<4;k++) {
        t[k]  = a->vec[i].coeffs[4*j+k];
        t[k] += ((int16_t)t[k] >> 15) & CCAKEM_Q;
        d0 = (t[k] + (1U << (LOG_Q-11))) >> (LOG_Q-10);
        t[k] = d0 & 0x3ff;
      }

      r[0] = (t[0] >> 0);
      r[1] = (t[0] >> 8) | (t[1] << 2);
      r[2] = (t[1] >> 6) | (t[2] << 4);
      r[3] = (t[2] >> 4) | (t[3] << 6);
      r[4] = (t[3] >> 2);
      r += 5;
    }
  }
#else
#error "CCAKEM_POLYVECCOMPRESSEDBYTES needs to be in {320*CCAKEM_K, 352*CCAKEM_K}"
#endif
}

/*************************************************
* Name:        polyvec_decompress
*
* Description: De-serialize and decompress vector of polynomials;
*              approximate inverse of polyvec_compress
*
* Arguments:   - polyvec *r:       pointer to output vector of polynomials
*              - const uint8_t *a: pointer to input byte array
*                                  (of length CCAKEM_POLYVECCOMPRESSEDBYTES)
**************************************************/
void polyvec_decompress(polyvec *r, const uint8_t a[CCAKEM_POLYVECCOMPRESSEDBYTES])
{
  unsigned int i,j,k;

#if (CCAKEM_POLYVECCOMPRESSEDBYTES == (CCAKEM_K * 352))
  uint16_t t[8];
  for(i=0;i<CCAKEM_K;i++) {
    for(j=0;j<CCAKEM_N/8;j++) {
      t[0] = (a[0] >> 0) | ((uint16_t)a[ 1] << 8);
      t[1] = (a[1] >> 3) | ((uint16_t)a[ 2] << 5);
      t[2] = (a[2] >> 6) | ((uint16_t)a[ 3] << 2) | ((uint16_t)a[4] << 10);
      t[3] = (a[4] >> 1) | ((uint16_t)a[ 5] << 7);
      t[4] = (a[5] >> 4) | ((uint16_t)a[ 6] << 4);
      t[5] = (a[6] >> 7) | ((uint16_t)a[ 7] << 1) | ((uint16_t)a[8] << 9);
      t[6] = (a[8] >> 2) | ((uint16_t)a[ 9] << 6);
      t[7] = (a[9] >> 5) | ((uint16_t)a[10] << 3);
      a += 11;

      for(k=0;k<8;k++)
        r->vec[i].coeffs[8*j+k] = (uint32_t)(t[k] & 0x7FF) << (LOG_Q - 11);
    }
  }
#elif (CCAKEM_POLYVECCOMPRESSEDBYTES == (CCAKEM_K * 320))
  uint16_t t[4];
  for(i=0;i<CCAKEM_K;i++) {
    for(j=0;j<CCAKEM_N/4;j++) {
      t[0] = (a[0] >> 0) | ((uint16_t)a[1] << 8);
      t[1] = (a[1] >> 2) | ((uint16_t)a[2] << 6);
      t[2] = (a[2] >> 4) | ((uint16_t)a[3] << 4);
      t[3] = (a[3] >> 6) | ((uint16_t)a[4] << 2);
      a += 5;

      for(k=0;k<4;k++)
        r->vec[i].coeffs[4*j+k] = (uint32_t)(t[k] & 0x3FF) << (LOG_Q - 10);
    }
  }
#else
#error "CCAKEM_POLYVECCOMPRESSEDBYTES needs to be in {320*CCAKEM_K, 352*CCAKEM_K}"
#endif
}

/*************************************************
* Name:        polyvec_tobytes
*
* Description: Serialize vector of polynomials
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (needs space for CCAKEM_POLYVECBYTES)
*              - const polyvec *a: pointer to input vector of polynomials
**************************************************/
void polyvec_tobytes(uint8_t r[CCAKEM_POLYVECBYTES], const polyvec *a)
{
  unsigned int i;
  for(i=0;i<CCAKEM_K;i++)
    poly_tobytes(r+i*CCAKEM_POLYBYTES, &a->vec[i]);
}

/*************************************************
* Name:        polyvec_frombytes
*
* Description: De-serialize vector of polynomials;
*              inverse of polyvec_tobytes
*
* Arguments:   - uint8_t *r:       pointer to output byte array
*              - const polyvec *a: pointer to input vector of polynomials
*                                  (of length CCAKEM_POLYVECBYTES)
**************************************************/
void polyvec_frombytes(polyvec *r, const uint8_t a[CCAKEM_POLYVECBYTES])
{
  unsigned int i;
  for(i=0;i<CCAKEM_K;i++)
    poly_frombytes(&r->vec[i], a+i*CCAKEM_POLYBYTES);
}

/*************************************************
* Name:        polyvec_add
*
* Description: Add vectors of polynomials
*
* Arguments: - polyvec *r: pointer to output vector of polynomials
*            - const polyvec *a: pointer to first input vector of polynomials
*            - const polyvec *b: pointer to second input vector of polynomials
**************************************************/
void polyvec_add(polyvec *r, const polyvec *a, const polyvec *b)
{
  unsigned int i;
  for(i=0;i<CCAKEM_K;i++)
    poly_add(&r->vec[i], &a->vec[i], &b->vec[i]);
}
