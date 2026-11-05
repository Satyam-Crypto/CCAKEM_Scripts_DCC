#include <stdint.h>
#include <string.h>
#include "poly_mult.h"
#include "polyvec.h"
#include "toomcook.h"
#include "params.h"

void poly_reduce(int16_t *a)
{
    for(int i = 0; i < CCAKEM_N; i++)
        a[i] &= QMASK;
}

void polyvec_mod_reduce(polyvec *r)
{
    unsigned int i;
    for(i=0;i<CCAKEM_K;i++)
        poly_reduce(r->vec[i].coeffs);
}


void polyvec_mul_acc_toom(poly *r, const polyvec *a, const polyvec *b)
{
  unsigned int i;

  memset(r->coeffs, 0, sizeof(r->coeffs));
  for (i = 0; i < CCAKEM_K; i++) {
    poly_mul_acc(a->vec[i].coeffs, b->vec[i].coeffs, r->coeffs);
  }
  poly_reduce(r->coeffs);
}