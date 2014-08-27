#include "karp_rabin.h"
#include <gmp.h>
#include <stdio.h>
#include <assert.h>

int main(void) {
    unsigned int *P = malloc(20 * sizeof(int));
    fingerprinter printer = fingerprinter_build(100, 0);
    gmp_printf("p = %Zd\n", printer->p);
    gmp_printf("r = %Zd\n", printer->r);

    P[0]  = 0; P[1]  = 0; P[2]  = 0; P[3]  = 0; P[4]  = 0;
    P[5]  = 1; P[6]  = 1; P[7]  = 1; P[8]  = 1; P[9]  = 1;
    P[10] = 2; P[11] = 2; P[12] = 2; P[13] = 2; P[14] = 2;
    P[15] = 0; P[16] = 0; P[17] = 0; P[18] = 0; P[19] = 0;

    fingerprint print = init_fingerprint();
    set_fingerprint(printer, P, 20, print);

    gmp_printf("uv finger = %Zd\n", print->finger);
    gmp_printf("uv r_k = %Zd\n", print->r_k);
    gmp_printf("uv r_mk = %Zd\n", print->r_mk);

    fingerprint prefix = init_fingerprint();
    set_fingerprint(printer, P, 5, prefix);

    fingerprint v = init_fingerprint();
    fingerprint_suffix(printer, print, prefix, v);

    fingerprint suffix = init_fingerprint();
    set_fingerprint(printer, &P[5], 15, suffix);
    assert(fingerprint_equals(v, suffix));

    fingerprint u = init_fingerprint();
    fingerprint_prefix(printer, print, suffix, u);
    assert(fingerprint_equals(u, prefix));

    fingerprint uv = init_fingerprint();
    fingerprint_concat(printer, prefix, suffix, uv);
    assert(fingerprint_equals(uv, print));

    fingerprint empty = init_fingerprint();
    fingerprint_suffix(printer, print, empty, v);
    assert(fingerprint_equals(v, print));

    fingerprint_prefix(printer, print, empty, v);
    assert(fingerprint_equals(v, print));

    fingerprint_concat(printer, print, empty, v);
    assert(fingerprint_equals(v, print));

    fingerprint_concat(printer, empty, print, v);
    assert(fingerprint_equals(v, print));

    fingerprint zeroed = init_fingerprint(), z = init_fingerprint();
    P[12] = 0;
    set_fingerprint(printer, P, 20, zeroed);
    mpz_t r_z;
    mpz_init(r_z);
    mpz_powm_ui(r_z, printer->r, 12, printer->p);
    fingerprint_zero(printer, print, 2, r_z, z);
    assert(fingerprint_equals(z, zeroed));

    fingerprint_free(print);
    fingerprint_free(prefix);
    fingerprint_free(suffix);
    fingerprint_free(u);
    fingerprint_free(v);
    fingerprint_free(uv);
    fingerprint_free(empty);

    return 0;
}