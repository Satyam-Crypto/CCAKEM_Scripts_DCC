CC ?= /usr/bin/cc
CFLAGS += -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls \
  -Wshadow -Wpointer-arith -O3 -fomit-frame-pointer -z noexecstack
NISTFLAGS += -Wno-unused-result -O3 -fomit-frame-pointer
RM = /bin/rm

SOURCES = kem.c indcpa.c polyvec.c poly.c poly_mult.c toomcook.c cbd.c verify.c
SOURCESKECCAK = $(SOURCES) fips202.c symmetric-shake.c
HEADERS = params.h kem.h indcpa.h polyvec.h poly.h poly_mult.h toomcook.h cbd.h verify.h symmetric.h
HEADERSKECCAK = $(HEADERS) fips202.h

.PHONY: all speed clean

all: test speed nistkat

test: \
  test/test_ccakem512 \
  test/test_ccakem768 \
  test/test_ccakem1024 \
  test/test_vectors512 \
  test/test_vectors768 \
  test/test_vectors1024 \

speed: \
  test/test_speed512 \
  test/test_speed768 \
  test/test_speed1024 \

nistkat: \
	nistkat/PQCgenKAT_kem512 \
	nistkat/PQCgenKAT_kem768 \
	nistkat/PQCgenKAT_kem1024 \

test/test_ccakem512: $(SOURCESKECCAK) $(HEADERSKECCAK) test/test_ccakem.c randombytes.c
	$(CC) $(CFLAGS) -DCCAKEM_K=2 $(SOURCESKECCAK) randombytes.c test/test_ccakem.c -o $@

test/test_ccakem768: $(SOURCESKECCAK) $(HEADERSKECCAK) test/test_ccakem.c randombytes.c
	$(CC) $(CFLAGS) -DCCAKEM_K=3 $(SOURCESKECCAK) randombytes.c test/test_ccakem.c -o $@

test/test_ccakem1024: $(SOURCESKECCAK) $(HEADERSKECCAK) test/test_ccakem.c randombytes.c
	$(CC) $(CFLAGS) -DCCAKEM_K=4 $(SOURCESKECCAK) randombytes.c test/test_ccakem.c -o $@

test/test_vectors512: $(SOURCESKECCAK) $(HEADERSKECCAK) test/test_vectors.c
	$(CC) $(CFLAGS) -DCCAKEM_K=2 $(SOURCESKECCAK) test/test_vectors.c -o $@

test/test_vectors768: $(SOURCESKECCAK) $(HEADERSKECCAK) test/test_vectors.c
	$(CC) $(CFLAGS) -DCCAKEM_K=3 $(SOURCESKECCAK) test/test_vectors.c -o $@

test/test_vectors1024: $(SOURCESKECCAK) $(HEADERSKECCAK) test/test_vectors.c
	$(CC) $(CFLAGS) -DCCAKEM_K=4 $(SOURCESKECCAK) test/test_vectors.c -o $@

test/test_speed512: $(SOURCESKECCAK) $(HEADERSKECCAK) test/cpucycles.h test/cpucycles.c test/speed_print.h test/speed_print.c test/test_speed.c randombytes.c
	$(CC) $(CFLAGS) -DCCAKEM_K=2 $(SOURCESKECCAK) randombytes.c test/cpucycles.c test/speed_print.c test/test_speed.c -o $@

test/test_speed768: $(SOURCESKECCAK) $(HEADERSKECCAK) test/cpucycles.h test/cpucycles.c test/speed_print.h test/speed_print.c test/test_speed.c randombytes.c
	$(CC) $(CFLAGS) -DCCAKEM_K=3 $(SOURCESKECCAK) randombytes.c test/cpucycles.c test/speed_print.c test/test_speed.c -o $@

test/test_speed1024: $(SOURCESKECCAK) $(HEADERSKECCAK) test/cpucycles.h test/cpucycles.c test/speed_print.h test/speed_print.c test/test_speed.c randombytes.c
	$(CC) $(CFLAGS) -DCCAKEM_K=4 $(SOURCESKECCAK) randombytes.c test/cpucycles.c test/speed_print.c test/test_speed.c -o $@

nistkat/PQCgenKAT_kem512: $(SOURCESKECCAK) $(HEADERSKECCAK) nistkat/PQCgenKAT_kem.c nistkat/rng.c nistkat/rng.h
	$(CC) $(NISTFLAGS) -DCCAKEM_K=2 -o $@ $(SOURCESKECCAK) nistkat/rng.c nistkat/PQCgenKAT_kem.c $(LDFLAGS) -lcrypto

nistkat/PQCgenKAT_kem768: $(SOURCESKECCAK) $(HEADERSKECCAK) nistkat/PQCgenKAT_kem.c nistkat/rng.c nistkat/rng.h
	$(CC) $(NISTFLAGS) -DCCAKEM_K=3 -o $@ $(SOURCESKECCAK) nistkat/rng.c nistkat/PQCgenKAT_kem.c $(LDFLAGS) -lcrypto

nistkat/PQCgenKAT_kem1024: $(SOURCESKECCAK) $(HEADERSKECCAK) nistkat/PQCgenKAT_kem.c nistkat/rng.c nistkat/rng.h
	$(CC) $(NISTFLAGS) -DCCAKEM_K=4 -o $@ $(SOURCESKECCAK) nistkat/rng.c nistkat/PQCgenKAT_kem.c $(LDFLAGS) -lcrypto

clean:
	-$(RM) -f *.gcno *.gcda *.lcov *.o
	-$(RM) -f test/test_ccakem512
	-$(RM) -f test/test_ccakem768
	-$(RM) -f test/test_ccakem1024
	-$(RM) -f test/test_vectors512
	-$(RM) -f test/test_vectors768
	-$(RM) -f test/test_vectors1024
	-$(RM) -f test/test_speed512
	-$(RM) -f test/test_speed768
	-$(RM) -f test/test_speed1024
	-$(RM) -f nistkat/PQCgenKAT_kem512
	-$(RM) -f nistkat/PQCgenKAT_kem768
	-$(RM) -f nistkat/PQCgenKAT_kem1024
	-$(RM) -f nistkat/*.req
	-$(RM) -f nistkat/*.rsp
