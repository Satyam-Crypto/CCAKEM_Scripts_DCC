#ifndef POLY_MUL_H
#define POLY_MUL_H

#include <stdint.h>
#include "params.h"
#include "toomcook.h"
#include "polyvec.h"


#define poly_reduce CCAKEM_NAMESPACE(poly_reduce)
void poly_reduce(int16_t *a);

/* Reduce all polynomials inside a polynomial vector */
#define polyvec_mod_reduce CCAKEM_NAMESPACE(polyvec_mod_reduce)
void polyvec_mod_reduce(polyvec *r);

#define polyvec_mul_acc_toom CCAKEM_NAMESPACE(polyvec_mul_acc_toom)
void polyvec_mul_acc_toom(poly *r,
                         const polyvec *a,
                         const polyvec *b);

#endif /* POLY_MUL_H */