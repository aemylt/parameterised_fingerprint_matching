#include "hash_lookup.h"
#include "karp_rabin.h"
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>

typedef struct {
    int location;
    fingerprint T_f;
} viable_occurance;

typedef struct {
    int pred, z;
    mpz_t r_z;
} zero_item;

typedef struct {
    int row_size, count, zero_start, zero_end;
    fingerprint P;
    viable_occurance *VOs;
    zero_item *to_zero;
} pattern_row;

void shift_row(pattern_row *P_i) {
    int i;
    for (i = 1; i < P_i->count; i++) {
        P_i->VOs[i - 1].location = P_i->VOs[i].location;
        fingerprint_assign(P_i->VOs[i].T_f, P_i->VOs[i - 1].T_f);
    }
    P_i->count--;
}

void add_occurance(fingerprint T_f, int location, pattern_row *P_i) {
    P_i->VOs[P_i->count].location = location;
    fingerprint_assign(T_f, P_i->VOs[P_i->count++].T_f);
}

int parameterised_match(char *T, int n, char *P, int m, char *sigma, int s_sigma, int alpha, int *results) {
    int i, j, k, *predecessor = malloc(m * sizeof(int)), lookup, lm = 0, matches = 0, index, pattern_start;
    for (i = 0; i < s_sigma; i++) predecessor[i] = -1;

    hash_lookup p_pred = hashlookup_build(sigma, predecessor, s_sigma), t_pred = hashlookup_build(sigma, predecessor, s_sigma);

    for (i = 0; i < m; i++) {
        lookup = hashlookup_search(p_pred, P[i]);
        predecessor[i] = (lookup == -1) ? 0 : i - lookup;
        hashlookup_edit(&p_pred, P[i], i);
    }
    hashlookup_free(&p_pred);

    fingerprinter printer = fingerprinter_build(n, alpha);

    while ((1 << lm) < m) lm++;
    lm -= 1;

    j = 1;
    i = 0;
    pattern_row *P_i = malloc(lm * sizeof(pattern_row));
    while ((j << 2) < m) {
        P_i[i].row_size = j;
        P_i[i].count = 0;
        P_i[i].P = init_fingerprint();
        set_fingerprint(printer, &predecessor[j], j, P_i[i].P);
        P_i[i].VOs = malloc(j * sizeof(viable_occurance));
        for (k = 0; k < j; k++) P_i[i].VOs[k].T_f = init_fingerprint();
        P_i[i].to_zero = malloc(s_sigma * sizeof(zero_item));
        P_i[i].zero_start = 0;
        P_i[i].zero_end = 0;
        for (k = 0; k < s_sigma; k++) mpz_init(P_i[i].to_zero[k].r_z);
        j <<= 1;
        i++;
    }
    P_i[i].row_size = m - j;
    P_i[i].count = 0;
    P_i[i].P = init_fingerprint();
    set_fingerprint(printer, &predecessor[j], m - j, P_i[i].P);
    free(predecessor);

    P_i[i].VOs = malloc((m - j) * sizeof(viable_occurance));
    for (k = 0; k < (m - j); k++) P_i[i].VOs[k].T_f = init_fingerprint();
    P_i[i].to_zero = malloc(s_sigma * sizeof(zero_item));
    for (k = 0; k < s_sigma; k++) mpz_init(P_i[i].to_zero[k].r_z);
    P_i[i].zero_start = 0;
    P_i[i].zero_end = 0;

    fingerprint T_f = init_fingerprint(), T_cur = init_fingerprint(), T_prev = init_fingerprint(), tmp = init_fingerprint();
    mpz_t r_z;
    mpz_init(r_z);

    for (i = 0; i < n; i++) {
        lookup = hashlookup_search(t_pred, T[i]);
        lookup = (lookup == -1) ? 0 : i - lookup;

        hashlookup_edit(&t_pred, T[i], i);
        set_fingerprint(printer, &lookup, 1, T_cur);
        fingerprint_concat(printer, T_prev, T_cur, tmp);
        mpz_set(r_z, T_prev->r_k);
        fingerprint_assign(tmp, T_prev);

        for (j = lm - 1; j >= 0; j--) {
            if (lookup > ((j == lm - 1) ? m - P_i[j].row_size: P_i[j].row_size)) {
                P_i[j].to_zero[P_i[j].zero_end].pred = lookup;
                P_i[j].to_zero[P_i[j].zero_end].z = i;
                mpz_set(P_i[j].to_zero[P_i[j].zero_end].r_z, r_z);
                if (++P_i[j].zero_end == s_sigma) P_i[j].zero_end = 0;
                if (P_i[j].zero_end == P_i[j].zero_start) if (++P_i[j].zero_start == s_sigma) P_i[j].zero_start = 0;
            }
            if ((P_i[j].count > 0) && (i - P_i[j].VOs[0].location == P_i[j].row_size)) {
                fingerprint_assign(T_prev, T_cur);
                index = P_i[j].zero_start;
                fingerprint_assign(T_cur, tmp);
                while (index != P_i[j].zero_end) {
                    pattern_start = i - ((j == lm - 1) ? m : (P_i[j].row_size << 1));
                    if (P_i[j].to_zero[index].z < i - P_i[j].row_size) {
                        if (++P_i[j].zero_start == s_sigma) P_i[j].zero_start = 0;
                    } else if (P_i[j].to_zero[index].z - P_i[j].to_zero[index].pred < pattern_start) fingerprint_zero(printer, T_cur, P_i[j].to_zero[index].pred, P_i[j].to_zero[index].r_z, tmp);
                    if (++index == s_sigma) index = 0;
                    fingerprint_assign(tmp, T_cur);
                }
                fingerprint_suffix(printer, T_cur, P_i[j].VOs[0].T_f, T_f);
                if (fingerprint_equals(P_i[j].P, T_f)) {
                    if (j == lm - 1) results[matches++] = i;
                    else add_occurance(T_prev, P_i[j].VOs[0].location + P_i[j].row_size, &P_i[j + 1]);
                }
                shift_row(&P_i[j]);
            }
        }
        add_occurance(T_prev, i, &P_i[0]);
    }

    hashlookup_free(&t_pred);
    for (i = 0; i < lm; i++) {
        fingerprint_free(P_i[i].P);
        for (k = 0; k < P_i[i].row_size; k++) fingerprint_free(P_i[i].VOs[k].T_f);
        free(P_i[i].VOs);
        for (k = 0; k < s_sigma; k++) mpz_clear(P_i[i].to_zero[k].r_z);
        free(P_i[i].to_zero);
    }
    free(P_i);
    mpz_clear(r_z);
    return matches;
}