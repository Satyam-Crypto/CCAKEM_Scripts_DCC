#ifndef POLYVEC_H
#define POLYVEC_H

#include <stdint.h>
#include "params.h"
#include "poly.h"

typedef struct{
  poly vec[CCAKEM_K];
} polyvec;

#define polyvec_compress CCAKEM_NAMESPACE(polyvec_compress)
void polyvec_compress(uint8_t r[CCAKEM_POLYVECCOMPRESSEDBYTES], const polyvec *a);
#define polyvec_decompress CCAKEM_NAMESPACE(polyvec_decompress)
void polyvec_decompress(polyvec *r, const uint8_t a[CCAKEM_POLYVECCOMPRESSEDBYTES]);

#define polyvec_tobytes CCAKEM_NAMESPACE(polyvec_tobytes)
void polyvec_tobytes(uint8_t r[CCAKEM_POLYVECBYTES], const polyvec *a);
#define polyvec_frombytes CCAKEM_NAMESPACE(polyvec_frombytes)
void polyvec_frombytes(polyvec *r, const uint8_t a[CCAKEM_POLYVECBYTES]);

#define polyvec_add CCAKEM_NAMESPACE(polyvec_add)
void polyvec_add(polyvec *r, const polyvec *a, const polyvec *b);

#endif
