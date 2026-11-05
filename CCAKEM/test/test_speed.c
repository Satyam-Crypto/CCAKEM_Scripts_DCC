#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "../kem.h"
#include "../params.h"
#include "../indcpa.h"
#include "../polyvec.h"
#include "../poly.h"
#include "../randombytes.h"
#include "cpucycles.h"
#include "speed_print.h"

#define NTESTS 100000

uint64_t t[NTESTS];
uint8_t seed[CCAKEM_SYMBYTES] = {0};

int main(void)
{
  unsigned int i;
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
  uint8_t key[CRYPTO_BYTES];
  uint8_t coins32[CCAKEM_SYMBYTES];
  uint8_t coins64[2*CCAKEM_SYMBYTES];
  polyvec matrix[CCAKEM_K];
  //poly ap;

  printf("Number of experiments (NTESTS): %d\n\n", (int)NTESTS);

  randombytes(coins32, CCAKEM_SYMBYTES);
  randombytes(coins64, 2*CCAKEM_SYMBYTES);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    gen_matrix(matrix, seed, 0);
  }
  print_results("gen_a: ", t, NTESTS);

  // for(i=0;i<NTESTS;i++) {
  //   t[i] = cpucycles();
  //   poly_getnoise_mus(&ap, seed, 0);
  // }
  // print_results("poly_getnoise_mus: ", t, NTESTS);

  // for(i=0;i<NTESTS;i++) {
  //   t[i] = cpucycles();
  //   poly_getnoise_mue(&ap, seed, 0);
  // }
  // print_results("poly_getnoise_mue: ", t, NTESTS);


  // for(i=0;i<NTESTS;i++) {
  //   t[i] = cpucycles();
  //   poly_tomsg(ct,&ap);
  // }
  // print_results("poly_tomsg: ", t, NTESTS);

  // for(i=0;i<NTESTS;i++) {
  //   t[i] = cpucycles();
  //   poly_frommsg(&ap,ct);
  // }
  // print_results("poly_frommsg: ", t, NTESTS);

  // for(i=0;i<NTESTS;i++) {
  //   t[i] = cpucycles();
  //   poly_compress_lwr(ct,&ap);
  // }
  // print_results("poly_compress: ", t, NTESTS);

  // for(i=0;i<NTESTS;i++) {
  //   t[i] = cpucycles();
  //   poly_decompress_lwr(&ap,ct);
  // }
  // print_results("poly_decompress: ", t, NTESTS);

  // for(i=0;i<NTESTS;i++) {
  //   t[i] = cpucycles();
  //   polyvec_compress(ct,&matrix[0]);
  // }
  // print_results("polyvec_compress: ", t, NTESTS);

  // for(i=0;i<NTESTS;i++) {
  //   t[i] = cpucycles();
  //   polyvec_decompress(&matrix[0],ct);
  // }
  // print_results("polyvec_decompress: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    indcpa_keypair_derand(pk, sk, coins32);
  }
  print_results("indcpa_keypair: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    indcpa_enc(ct, key, pk, seed);
  }
  print_results("indcpa_enc: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    indcpa_dec(key, ct, sk);
  }
  print_results("indcpa_dec: ", t, NTESTS);

  // for(i=0;i<NTESTS;i++) {
  //   t[i] = cpucycles();
  //   crypto_kem_keypair_derand(pk, sk, coins64);
  // }
  // print_results("ccakem_keypair_derand: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    crypto_kem_keypair(pk, sk);
  }
  print_results("ccakem_keypair: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    crypto_kem_enc_derand(ct, key, pk, coins32);
  }
  print_results("ccakem_encaps_derand: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    crypto_kem_enc(ct, key, pk);
  }
  print_results("ccakem_encaps: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    crypto_kem_dec(key, ct, sk);
  }
  print_results("ccakem_decaps: ", t, NTESTS);

  return 0;
}
