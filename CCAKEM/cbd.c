#include <stdint.h>
#include "params.h"
#include "cbd.h"

/*************************************************
* Name:        load32_littleendian
*
* Description: load 4 bytes into a 32-bit integer
*              in little-endian order
*
* Arguments:   - const uint8_t *x: pointer to input byte array
*
* Returns 32-bit unsigned integer loaded from x
**************************************************/
#if CCAKEM_MUS == 2 || CCAKEM_MUE == 2
static uint32_t load32_littleendian(const uint8_t x[4])
{
  uint32_t r;
  r  = (uint32_t)x[0];
  r |= (uint32_t)x[1] << 8;
  r |= (uint32_t)x[2] << 16;
  r |= (uint32_t)x[3] << 24;
  return r;
}
#endif

/*************************************************
* Name:        load24_littleendian
*
* Description: load 3 bytes into a 32-bit integer
*              in little-endian order.
*
* Arguments:   - const uint8_t *x: pointer to input byte array
*
* Returns 32-bit unsigned integer loaded from x (most significant byte is zero)
**************************************************/
#if CCAKEM_MUS == 3 || CCAKEM_MUE == 3
static uint32_t load24_littleendian(const uint8_t x[3])
{
  uint32_t r;
  r  = (uint32_t)x[0];
  r |= (uint32_t)x[1] << 8;
  r |= (uint32_t)x[2] << 16;
  return r;
}
#endif


/*************************************************
* Name:        cbd1
*
* Description: Given an array of uniformly random bytes, compute
*              polynomial with coefficients distributed according to
*              a centered binomial distribution with parameter mu=1
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *buf: pointer to input byte array
**************************************************/
#if CCAKEM_MUS == 1 || CCAKEM_MUE == 1
static void cbd1(poly *r, const uint8_t buf[CCAKEM_N/4])
{
  unsigned int i,j;
  uint8_t t;
  int16_t a,b;

  for(i = 0; i < CCAKEM_N/4; i++) {
    t = buf[i];

    for(j = 0; j < 4; j++) {
      a = (t >> (2*j)) & 0x1;
      b = (t >> (2*j+1)) & 0x1;
      r->coeffs[4*i + j] = a - b;
    }
  }
}
#endif


/*************************************************
* Name:        cbd2
*
* Description: Given an array of uniformly random bytes, compute
*              polynomial with coefficients distributed according to
*              a centered binomial distribution with parameter mu=2
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *buf: pointer to input byte array
**************************************************/
#if CCAKEM_MUS == 2 || CCAKEM_MUE == 2
static void cbd2(poly *r, const uint8_t buf[2*CCAKEM_N/4])
{
  unsigned int i,j;
  uint32_t t,d;
  int16_t a,b;

  for(i=0;i<CCAKEM_N/8;i++) {
    t  = load32_littleendian(buf+4*i);
    d  = t & 0x55555555;
    d += (t>>1) & 0x55555555;

    for(j=0;j<8;j++) {
      a = (d >> (4*j+0)) & 0x3;
      b = (d >> (4*j+2)) & 0x3;
      r->coeffs[8*i+j] = a - b;
    }
  }
}
#endif

/*************************************************
* Name:        cbd3
*
* Description: Given an array of uniformly random bytes, compute
*              polynomial with coefficients distributed according to
*              a centered binomial distribution with parameter mu=3.
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *buf: pointer to input byte array
**************************************************/
#if CCAKEM_MUS == 3 || CCAKEM_MUE == 3
static void cbd3(poly *r, const uint8_t buf[3*CCAKEM_N/4])
{
  unsigned int i,j;
  uint32_t t,d;
  int16_t a,b;

  for(i=0;i<CCAKEM_N/4;i++) {
    t  = load24_littleendian(buf+3*i);
    d  = t & 0x00249249;
    d += (t>>1) & 0x00249249;
    d += (t>>2) & 0x00249249;

    for(j=0;j<4;j++) {
      a = (d >> (6*j+0)) & 0x7;
      b = (d >> (6*j+3)) & 0x7;
      r->coeffs[4*i+j] = a - b;
    }
  }
}
#endif

void poly_cbd_mus(poly *r, const uint8_t buf[CCAKEM_MUS*CCAKEM_N/4])
{
#if CCAKEM_MUS == 1
  cbd1(r, buf);
#elif CCAKEM_MUS == 2
  cbd2(r, buf);
#elif CCAKEM_MUS == 3
  cbd3(r, buf);
#else
#error "This implementation requires mu_s in {1, 2,3}"
#endif
}

void poly_cbd_mue(poly *r, const uint8_t buf[CCAKEM_MUE*CCAKEM_N/4])
{
#if CCAKEM_MUE == 1
  cbd1(r,buf);
#elif CCAKEM_MUE == 2
  cbd2(r, buf);
#elif CCAKEM_MUE == 3
  cbd3(r,buf);
#else
#error "This implementation requires mu_e in {1,2,3}"
#endif
}
