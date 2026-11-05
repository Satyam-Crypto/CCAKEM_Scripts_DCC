#ifndef TOOMCOOK_H
#define TOOMCOOK_H

#include "params.h"
#include <stdint.h>

#define poly_mul_acc CCAKEM_NAMESPACE(poly_mul_acc)
void poly_mul_acc(const int16_t a[CCAKEM_N], const int16_t b[CCAKEM_N],
                  int16_t res[CCAKEM_N]);

#endif
