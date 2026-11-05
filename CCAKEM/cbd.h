#ifndef CBD_H
#define CBD_H

#include <stdint.h>
#include "params.h"
#include "poly.h"

#define poly_cbd_mus CCAKEM_NAMESPACE(poly_cbd_mus)
void poly_cbd_mus(poly *r, const uint8_t buf[CCAKEM_MUS*CCAKEM_N/4]);

#define poly_cbd_mue CCAKEM_NAMESPACE(poly_cbd_mue)
void poly_cbd_mue(poly *r, const uint8_t buf[CCAKEM_MUE*CCAKEM_N/4]);

#endif
