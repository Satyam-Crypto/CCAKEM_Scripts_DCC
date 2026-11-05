#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "params.h"
#include "indcpa.h"
#include "polyvec.h"
#include "poly.h"
#include "poly_mult.h"
#include "symmetric.h"
#include "randombytes.h"

/*************************************************
* Name:        pack_pk
*
* Description: Serialize the public key as concatenation of the
*              serialized vector of polynomials pk
*              and the public seed used to generate the matrix A.
*
* Arguments:   uint8_t *r: pointer to the output serialized public key
*              polyvec *pk: pointer to the input public-key polyvec
*              const uint8_t *seed: pointer to the input public seed
**************************************************/
static void pack_pk(uint8_t r[CCAKEM_INDCPA_PUBLICKEYBYTES],
                    polyvec *pk,
                    const uint8_t seed[CCAKEM_SYMBYTES])
{
  polyvec_tobytes(r, pk);
  memcpy(r+CCAKEM_POLYVECBYTES, seed, CCAKEM_SYMBYTES);
}

/*************************************************
* Name:        unpack_pk
*
* Description: De-serialize public key from a byte array;
*              approximate inverse of pack_pk
*
* Arguments:   - polyvec *pk: pointer to output public-key polynomial vector
*              - uint8_t *seed: pointer to output seed to generate matrix A
*              - const uint8_t *packedpk: pointer to input serialized public key
**************************************************/
static void unpack_pk(polyvec *pk,
                      uint8_t seed[CCAKEM_SYMBYTES],
                      const uint8_t packedpk[CCAKEM_INDCPA_PUBLICKEYBYTES])
{
  polyvec_frombytes(pk, packedpk);
  memcpy(seed, packedpk+CCAKEM_POLYVECBYTES, CCAKEM_SYMBYTES);
}

/*************************************************
* Name:        pack_sk
*
* Description: Serialize the secret key
*
* Arguments:   - uint8_t *r: pointer to output serialized secret key
*              - polyvec *sk: pointer to input vector of polynomials (secret key)
**************************************************/
static void pack_sk(uint8_t r[CCAKEM_INDCPA_SECRETKEYBYTES], polyvec *sk)
{
  polyvec_tobytes(r, sk);
}

/*************************************************
* Name:        unpack_sk
*
* Description: De-serialize the secret key; inverse of pack_sk
*
* Arguments:   - polyvec *sk: pointer to output vector of polynomials (secret key)
*              - const uint8_t *packedsk: pointer to input serialized secret key
**************************************************/
static void unpack_sk(polyvec *sk, const uint8_t packedsk[CCAKEM_INDCPA_SECRETKEYBYTES])
{
  polyvec_frombytes(sk, packedsk);
}


#define gen_a(A,B)  gen_matrix(A,B,0)
#define gen_at(A,B) gen_matrix(A,B,1)

/*************************************************
* Name:        gen_matrix
*
* Description: Deterministically generate matrix A (or the transpose of A)
*              from a seed. Entries of the matrix are polynomials that look
*              uniformly random. Performs rejection sampling on output of
*              a XOF
*
* Arguments:   - polyvec *a: pointer to ouptput matrix A
*              - const uint8_t *seed: pointer to input seed
*              - int transposed: boolean deciding whether A or A^T is generated
**************************************************/

#define GEN_MATRIX_NBLOCKS ((CCAKEM_K*CCAKEM_K*CCAKEM_N*LOG_Q/8 + XOF_BLOCKBYTES)/XOF_BLOCKBYTES)

void gen_matrix(polyvec *A, const uint8_t seed[CCAKEM_SYMBYTES], uint8_t transposed)
{
    uint8_t buf[GEN_MATRIX_NBLOCKS * XOF_BLOCKBYTES];
    xof_state state;
    size_t pos = 0;
    uint16_t mask = CCAKEM_Q - 1;

    xof_absorb(&state, seed, 0, 0);
    xof_squeezeblocks(buf, GEN_MATRIX_NBLOCKS, &state);

    for (unsigned i = 0; i < CCAKEM_K; i++) {
        for (unsigned j = 0; j < CCAKEM_K; j++) {

            unsigned ii = transposed ? j : i;
            unsigned jj = transposed ? i : j;

            poly *p = &A[ii].vec[jj];

#if LOG_Q == 11
            for (unsigned k = 0; k < CCAKEM_N / 8; k++) {

                p->coeffs[8*k+0] =
                    ( buf[pos+0] | (buf[pos+1] << 8) ) & mask;

                p->coeffs[8*k+1] =
                    ( (buf[pos+1] >> 3) | (buf[pos+2] << 5) ) & mask;

                p->coeffs[8*k+2] =
                    ( (buf[pos+2] >> 6)
                    | (buf[pos+3] << 2)
                    | (buf[pos+4] << 10) ) & mask;

                p->coeffs[8*k+3] =
                    ( (buf[pos+4] >> 1) | (buf[pos+5] << 7) ) & mask;

                p->coeffs[8*k+4] =
                    ( (buf[pos+5] >> 4) | (buf[pos+6] << 4) ) & mask;

                p->coeffs[8*k+5] =
                    ( (buf[pos+6] >> 7)
                    | (buf[pos+7] << 1)
                    | (buf[pos+8] << 9) ) & mask;

                p->coeffs[8*k+6] =
                    ( (buf[pos+8] >> 2) | (buf[pos+9] << 6) ) & mask;

                p->coeffs[8*k+7] =
                    ( (buf[pos+9] >> 5) | (buf[pos+10] << 3) ) & mask;

                pos += 11;
            }

#elif LOG_Q == 12
            for (unsigned k = 0; k < CCAKEM_N; k += 2) {

                uint16_t c0 = buf[pos] | (buf[pos+1] << 8);
                uint16_t c1 = (buf[pos+1] >> 4) | (buf[pos+2] << 4);

                p->coeffs[k]     = c0 & mask;
                p->coeffs[k + 1] = c1 & mask;

                pos += 3;
            }

#else
#error "Unsupported LOG_Q. Currently modeled for LOG_Q == 11 or 12"
#endif

        }
    }
}

/*************************************************
* Name:        indcpa_keypair_derand
*
* Description: Generates public and private key for the CPA-secure
*              public-key encryption scheme underlying the KEM
*
* Arguments:   - uint8_t *pk: pointer to output public key
*                             (of length CCAKEM_INDCPA_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key
*                             (of length CCAKEM_INDCPA_SECRETKEYBYTES bytes)
*              - const uint8_t *coins: pointer to input randomness
*                             (of length CCAKEM_SYMBYTES bytes)
**************************************************/

void indcpa_keypair_derand(uint8_t pk[CCAKEM_INDCPA_PUBLICKEYBYTES],
  uint8_t sk[CCAKEM_INDCPA_SECRETKEYBYTES],
  const uint8_t coins[CCAKEM_SYMBYTES])
{
  unsigned int i;
  uint8_t buf[2*CCAKEM_SYMBYTES];
  const uint8_t *publicseed = buf;
  const uint8_t *noiseseed = buf + CCAKEM_SYMBYTES;
  uint8_t nonce = 0;

  polyvec a[CCAKEM_K], e, pkpv, skpv;

  memcpy(buf, coins, CCAKEM_SYMBYTES);
  buf[CCAKEM_SYMBYTES] = CCAKEM_K;
  hash_g(buf, buf, CCAKEM_SYMBYTES + 1);

  gen_a(a, publicseed);

  /* sample secret and error */
  for(i = 0; i < CCAKEM_K; i++)
    poly_getnoise_mus(&skpv.vec[i], noiseseed, nonce++);

  for(i = 0; i < CCAKEM_K; i++)
    poly_getnoise_mue(&e.vec[i], noiseseed, nonce++);

  /* pkpv = A * s  (Toom-Cook multiplication) */
  for(i = 0; i < CCAKEM_K; i++)
    polyvec_mul_acc_toom(&pkpv.vec[i], &a[i], &skpv);

  /* pkpv = A*s + e */
  polyvec_add(&pkpv, &pkpv, &e);
  polyvec_mod_reduce(&pkpv);

  /* pack keys */
  pack_sk(sk, &skpv);
  pack_pk(pk, &pkpv, publicseed);
}

/*************************************************
* Name:        indcpa_enc
*
* Description: Encryption function of the CPA-secure
*              public-key encryption scheme underlying CCAKEM.
*
* Arguments:   - uint8_t *c: pointer to output ciphertext
*                            (of length CCAKEM_INDCPA_BYTES bytes)
*              - const uint8_t *m: pointer to input message
*                                  (of length CCAKEM_INDCPA_MSGBYTES bytes)
*              - const uint8_t *pk: pointer to input public key
*                                   (of length CCAKEM_INDCPA_PUBLICKEYBYTES)
*              - const uint8_t *coins: pointer to input random coins used as seed
*                                      (of length CCAKEM_SYMBYTES) to deterministically
*                                      generate all randomness
**************************************************/
void indcpa_enc(uint8_t c[CCAKEM_INDCPA_BYTES],
  const uint8_t m[CCAKEM_INDCPA_MSGBYTES],
  const uint8_t pk[CCAKEM_INDCPA_PUBLICKEYBYTES],
  const uint8_t coins[CCAKEM_SYMBYTES])
{
  unsigned int i;
  uint8_t seed[CCAKEM_SYMBYTES];
  uint8_t nonce = 0;

  polyvec rpv, pkpv, e1pv, at[CCAKEM_K], upv;
  poly vp, mp;// epp;

  unpack_pk(&pkpv, seed, pk);
  poly_frommsg(&mp, m);

  gen_at(at, seed);

  /* sample noise */
  for(i = 0; i < CCAKEM_K; i++)
  poly_getnoise_mus(&rpv.vec[i], coins, nonce++);

  for(i = 0; i < CCAKEM_K; i++)
  poly_getnoise_mue(&e1pv.vec[i], coins, nonce++);


  /* b = A^T * s */
  for (i = 0; i < CCAKEM_K; i++)
    polyvec_mul_acc_toom(&upv.vec[i], &at[i], &rpv);

  /* v = t * s */
  polyvec_mul_acc_toom(&vp, &pkpv, &rpv);

  /* add noise and message */
  polyvec_add(&upv, &upv, &e1pv);

  poly_add(&vp, &vp, &mp);

  // for(i = 0; i < CCAKEM_N; i++)
  //   v.coeffs[i] = (v.coeffs[i] + (1 << (LOG_Q - LOG_P - 1))) >> (LOG_Q - LOG_P);

  polyvec_mod_reduce(&upv);
  poly_reduce(vp.coeffs);

  // Apply compression & LWR to ciphertext component u and v;
  polyvec_compress(c, &upv);
  poly_compress_lwr(c + CCAKEM_POLYVECCOMPRESSEDBYTES, &vp);
}

/*************************************************
* Name:        indcpa_dec
*
* Description: Decryption function of the CPA-secure
*              public-key encryption scheme underlying CCAKEM.
*
* Arguments:   - uint8_t *m: pointer to output decrypted message
*                            (of length CCAKEM_INDCPA_MSGBYTES)
*              - const uint8_t *c: pointer to input ciphertext
*                                  (of length CCAKEM_INDCPA_BYTES)
*              - const uint8_t *sk: pointer to input secret key
*                                   (of length CCAKEM_INDCPA_SECRETKEYBYTES)
**************************************************/
void indcpa_dec(uint8_t m[CCAKEM_INDCPA_MSGBYTES],
                const uint8_t c[CCAKEM_INDCPA_BYTES],
                const uint8_t sk[CCAKEM_INDCPA_SECRETKEYBYTES])
{
  polyvec upv, skpv;
  poly v, mp;

  // unpack_ciphertext(&upv, &v, c);
  polyvec_decompress(&upv, c);
  poly_decompress_lwr(&v, c + CCAKEM_POLYVECCOMPRESSEDBYTES);

  unpack_sk(&skpv, sk);

  /* mp = s · b or sTu */
  polyvec_mul_acc_toom(&mp, &skpv, &upv);

  poly_sub(&mp, &v, &mp);
  poly_reduce(mp.coeffs);

  poly_tomsg(m, &mp);
}
