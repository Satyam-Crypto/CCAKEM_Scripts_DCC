#ifndef PARAMS_H
#define PARAMS_H

#ifndef CCAKEM_K
#define CCAKEM_K 3	/* Change this for different security strengths */
#endif


/* Don't change parameters below this line */
#if   (CCAKEM_K == 2)
#define CCAKEM_NAMESPACE(s) pq_ccakem512_ref_##s
#elif (CCAKEM_K == 3)
#define CCAKEM_NAMESPACE(s) pq_ccakem768_ref_##s
#elif (CCAKEM_K == 4)
#define CCAKEM_NAMESPACE(s) pq_ccakem1024_ref_##s
#else
#error "CCAKEM_K must be in {2,3,4}"
#endif

#define CCAKEM_N 256
#define CCAKEM_DIM CCAKEM_N*CCAKEM_K

#define CCAKEM_SYMBYTES 32   /* size in bytes of hashes, and seeds */
#define CCAKEM_SSBYTES  32   /* size in bytes of shared key */


#if CCAKEM_K == 2
#define LOG_Q 11
#define LOG_P 4
#define CCAKEM_MUS 1        /*assuming mu_s and mu_r are same*/ 
#define CCAKEM_MUE 3        /*assuming mu_e and mu_e1 are same*/
#define CCAKEM_DU 10        /*equivalent to du in the paper*/
#define CCAKEM_POLYBYTES		32*LOG_Q   /* LOG_Q*CCAKEM_N/8*/
#define CCAKEM_POLYCOMPRESSEDBYTES    32*LOG_P  /* LOG_P*CCAKEM_N/8*/
#define CCAKEM_POLYVECCOMPRESSEDBYTES (CCAKEM_K * 32 * CCAKEM_DU)
#elif CCAKEM_K == 3
#define LOG_Q 12
#define LOG_P 4
#define CCAKEM_MUS 2        /*assuming mu_s and mu_r are same*/ 
#define CCAKEM_MUE 3        /*assuming mu_e and mu_e1 are same*/
#define CCAKEM_DU 10        /*equivalent to du in the paper*/
#define CCAKEM_POLYBYTES		32*LOG_Q
#define CCAKEM_POLYCOMPRESSEDBYTES    32*LOG_P
#define CCAKEM_POLYVECCOMPRESSEDBYTES (CCAKEM_K * 32 * CCAKEM_DU)
#elif CCAKEM_K == 4
#define LOG_Q 12
#define LOG_P 5
#define CCAKEM_MUS 2        /*assuming mu_s and mu_r are same*/ 
#define CCAKEM_MUE 3        /*assuming mu_e and mu_e1 are same*/
#define CCAKEM_DU 11        /*equivalent to du in the paper*/
#define CCAKEM_POLYBYTES		32*LOG_Q
#define CCAKEM_POLYCOMPRESSEDBYTES    32*LOG_P
#define CCAKEM_POLYVECCOMPRESSEDBYTES (CCAKEM_K * 32 * CCAKEM_DU)
#endif

#define CCAKEM_Q (1U << LOG_Q)
#define QMASK ((1U << LOG_Q) - 1)


#define CCAKEM_POLYVECBYTES	(CCAKEM_K * CCAKEM_POLYBYTES)

#define CCAKEM_INDCPA_MSGBYTES       (CCAKEM_SYMBYTES)
#define CCAKEM_INDCPA_PUBLICKEYBYTES (CCAKEM_POLYVECBYTES + CCAKEM_SYMBYTES)
#define CCAKEM_INDCPA_SECRETKEYBYTES (CCAKEM_POLYVECBYTES)
#define CCAKEM_INDCPA_BYTES          (CCAKEM_POLYVECCOMPRESSEDBYTES + CCAKEM_POLYCOMPRESSEDBYTES)

#define CCAKEM_PUBLICKEYBYTES  (CCAKEM_INDCPA_PUBLICKEYBYTES)
/* 32 bytes of additional space to save H(pk) */
#define CCAKEM_SECRETKEYBYTES  (CCAKEM_INDCPA_SECRETKEYBYTES + CCAKEM_INDCPA_PUBLICKEYBYTES + 2*CCAKEM_SYMBYTES)
#define CCAKEM_CIPHERTEXTBYTES (CCAKEM_INDCPA_BYTES)

#endif
