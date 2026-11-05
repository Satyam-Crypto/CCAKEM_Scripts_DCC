# CCAKEM: build, tests, benchmarks, and KAT generation

This directory contains the C implementation of CCAKEM and programs for correctness testing, deterministic test-vector generation, timing measurements, and NIST-format known-answer test (KAT) generation.

The [Makefile](Makefile) builds **CCAKEM512, CCAKEM768, and CCAKEM1024**. The `nistkat` targets exercise the same CCAKEM implementation using a deterministic random-number generator.

## Requirements

- GNU Make and a C compiler such as GCC or Clang.
- OpenSSL development headers and `libcrypto` for the `nistkat` targets.
- An x86-64 machine for the current speed programs: `test/cpucycles.h`.

The commands below assume a Linux shell and start in this `CCAKEM` directory.
## Components

| Files | Purpose |
| --- | --- |
| `kem.c`, `kem.h` | KEM key generation, encapsulation, and decapsulation; deterministic key-generation and encapsulation entry points. |
| `indcpa.c`, `indcpa.h` | Underlying IND-CPA key generation, encryption, decryption, and matrix generation. |
| `params.h` | Parameter selection, polynomial dimensions, moduli, and serialized sizes. |
| `poly.c`, `polyvec.c` and their headers | Polynomial/vector operations, serialization, and compression. |
| `poly_mult.c`, `toomcook.c` and their headers | Polynomial multiplication and reduction, including Toom-Cook/Karatsuba routines. |
| `cbd.c`, `cbd.h` | Centered binomial noise sampling. |
| `fips202.c`, `fips202.h`, `symmetric-shake.c`, `symmetric.h` | Keccak/SHA-3/SHAKE primitives and CCAKEM symmetric helpers. |
| `verify.c`, `verify.h` | Comparison and conditional-copy helpers. |
| `randombytes.c`, `randombytes.h` | Operating-system randomness used by correctness tests and speed programs. |
| `test/test_ccakem.c` | Correctness tests, including invalid secret keys and modified ciphertexts. |
| `test/test_vectors.c` | Deterministic vector generation using its own SHAKE128-based `randombytes`. |
| `test/test_speed.c`, `test/cpucycles.*`, `test/speed_print.*` | Timing experiments and median/average reporting. |
| `nistkat/PQCgenKAT_kem.c`, `nistkat/rng.c`, `nistkat/rng.h` | NIST-format request/response generation with an OpenSSL-based AES-256 deterministic RNG. |

## Parameter sets

The Makefile selects `CCAKEM_K` separately for each executable, so there is no
need to edit `params.h` to build all three variants. The suffix is the dimension
`CCAKEM_N * CCAKEM_K`, where `CCAKEM_N = 256`.

Current sizes from `params.h`, in bytes:

| Variant | `CCAKEM_K` | Public key | Secret key | Ciphertext | Shared secret |
| --- | ---: | ---: | ---: | ---: | ---: |
| CCAKEM512 | 2 | 736 | 1504 | 768 | 32 |
| CCAKEM768 | 3 | 1184 | 2400 | 1088 | 32 |
| CCAKEM1024 | 4 | 1568 | 3168 | 1568 | 32 |

## Build

```bash
make -j2 all
```

Running `make` without a target also builds `all`. These targets **compile
programs only**; they do not run tests, benchmarks, or KAT generation.

| Command | Executables built (for each suffix `512`, `768`, `1024`) |
| --- | --- |
| `make test` | `test/test_ccakem<SUFFIX>` and `test/test_vectors<SUFFIX>` |
| `make speed` | `test/test_speed<SUFFIX>` |
| `make nistkat` | `nistkat/PQCgenKAT_kem<SUFFIX>`; links with `-lcrypto` |
| `make all` | All twelve executables above |

To build one program or select a compiler:

```bash
make test/test_ccakem512
make CC=gcc all
```

OpenSSL is not required for `make test` or `make speed`. If compiler options are
changed, use `make -B <target>` to force rebuilding existing binaries.

## Run correctness tests

```bash
./test/test_ccakem512
./test/test_ccakem768
./test/test_ccakem1024
```

Each executable runs 1,000 iterations, checking shared-secret agreement,
replacement of the secret key with random bytes, and modification of a
ciphertext byte. Success prints the secret-key, public-key, and ciphertext sizes
and exits with status zero. A failed check prints `ERROR ...` and exits nonzero.

## Generate deterministic test vectors

```bash
mkdir -p results
./test/test_vectors512 > results/test_vectors512.txt
./test/test_vectors768 > results/test_vectors768.txt
./test/test_vectors1024 > results/test_vectors1024.txt
```

Each program produces 10,000 cases containing public keys, secret keys,
ciphertexts, shared secrets, and decapsulation results for random ciphertexts.
It also checks agreement of the valid shared secrets. Output is substantial,
so the examples save it to files. These text dumps are separate from the
NIST-format `.req` and `.rsp` files below.

## Run speed benchmarks

```bash
./test/test_speed512
./test/test_speed768
./test/test_speed1024
```

To save measurements:

```bash
mkdir -p results
./test/test_speed512 > results/cpucycle512.txt
./test/test_speed768 > results/cpucycle768.txt
./test/test_speed1024 > results/cpucycle1024.txt
```

Each program uses 100,000 iterations per enabled operation and reports median
and average timing after subtracting estimated counter-read overhead. Enabled
operations are matrix generation, IND-CPA key generation/encryption/decryption,
KEM key generation, deterministic encapsulation, randomized encapsulation, and
decapsulation.

The output label is `cycles/ticks`. The default `rdtsc` counter measures timestamp
counter ticks; record the CPU, compiler, build flags, and system conditions when
comparing measurements.

## Generate NIST-format KAT files

From the project root:

```bash
make nistkat
(
    cd nistkat || exit 1
    ./PQCgenKAT_kem512
    ./PQCgenKAT_kem768
    ./PQCgenKAT_kem1024
)
```

This creates the following files, with 100 cases per parameter set:

| Variant | Request file | Response file |
| --- | --- | --- |
| CCAKEM512 | `nistkat/PQCkemKAT_512.req` | `nistkat/PQCkemKAT_512.rsp` |
| CCAKEM768 | `nistkat/PQCkemKAT_768.req` | `nistkat/PQCkemKAT_768.rsp` |
| CCAKEM1024 | `nistkat/PQCkemKAT_1024.req` | `nistkat/PQCkemKAT_1024.rsp` |

The generator writes to its **current working directory**, overwriting files
with the same names. Each response contains `count`, `seed`, `pk`, `sk`, `ct`,
and `ss`, with a header naming the CCAKEM variant. It checks that decapsulation
reproduces the encapsulated shared secret and returns zero on success. 

## Clean generated files

```bash
make clean
```

