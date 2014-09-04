#ifndef PARAMETERISED_MATCHING
#define PARAMETERISED_MATCHING

#include "rbtree.c"
#include "karp_rabin.h"
#include "m_match.h"
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
    int row_size, period, count, zero_start, zero_end;
    fingerprint P, period_f;
    viable_occurance VOs[2];
    zero_item *to_zero;
} pattern_row;

/*
    compare_char
    Compares two characters. Used for the Red/Black Tree
    Parameters:
        void* leftp  - First character
        void* rightp - Second character
    Returns int:
        -1 if leftp <  rightp
         0 if leftp == rightp
         1 otherwise
*/
int compare_char(void* leftp, void* rightp) {
    char left = (char)leftp;
    char right = (char)rightp;
    if (left < right) return -1;
    else if (left > right) return 1;
    else return 0;
}

void shift_row(fingerprinter printer, pattern_row *P_i, fingerprint tmp) {
    if (P_i->count <= 2) {
        fingerprint_assign(P_i->VOs[1].T_f, P_i->VOs[0].T_f);
        P_i->VOs[0].location = P_i->VOs[1].location;
    } else {
        fingerprint_concat(printer, P_i->VOs[0].T_f, P_i->period_f, tmp);
        fingerprint_assign(tmp, P_i->VOs[0].T_f);
        P_i->VOs[0].location += P_i->period;
    }
    P_i->count--;
}

void add_occurance(fingerprinter printer, fingerprint T_f, int location, pattern_row *P_i, fingerprint tmp) {
    if (P_i->count < 2) {
        fingerprint_assign(T_f, P_i->VOs[P_i->count].T_f);
        P_i->VOs[P_i->count].location = location;
        P_i->count++;
    } else {
        if (P_i->count == 2) {
            P_i->period = P_i->VOs[1].location - P_i->VOs[0].location;
            fingerprint_suffix(printer, P_i->VOs[1].T_f, P_i->VOs[0].T_f, P_i->period_f);
        }
        fingerprint_suffix(printer, T_f, P_i->VOs[1].T_f, tmp);
        int period = location - P_i->VOs[1].location;
        if ((period == P_i->period) && (fingerprint_equals(tmp, P_i->period_f))) {
            fingerprint_assign(T_f, P_i->VOs[1].T_f);
            P_i->VOs[1].location = location;
            P_i->count++;
        } else printf("Warning: Error in Period occured at location %d. VO discarded.\n", location);
    }
}

int parameterised_match(char *T, int n, char *P, int m, int alpha, int *results) {
    int i, j, k, *predecessor = malloc(m * sizeof(int)), lookup, lm = 0, matches = 0, index, s_sigma = 0;
    rbtree p_pred = rbtree_create(), t_pred = rbtree_create();

    for (i = 0; i < m; i++) {
        predecessor[i] = i - (int)rbtree_lookup(p_pred, (void*)P[i], (void*)i, compare_char);
        if (!predecessor[i]) s_sigma++;
        rbtree_insert(p_pred, (void*)P[i], (void*)i, compare_char);
    }
    rbtree_destroy(p_pred);

    fingerprinter printer = fingerprinter_build(n, alpha);

    while ((1 << lm) < m) lm++;

    j = 3 * s_sigma * lm;
    if (j > m) j = m;
    mmatch_state mmatch = mmatch_build(predecessor, j, m);

    j = mmatch.m;

    if (j == m) {
        for (i = 0; i < n; i++) {
            lookup = i - (int)rbtree_lookup(t_pred, (void*)T[i], (void*)i, compare_char);
            rbtree_insert(t_pred, (void*)T[i], (void*)i, compare_char);
            if (mmatch_stream(&mmatch, lookup, i) == i) results[matches++] = i;
        }
        rbtree_destroy(t_pred);
        mmatch_free(&mmatch);
        return matches;
    }

    i = 0;
    pattern_row *P_i = malloc(lm * sizeof(pattern_row));
    while ((j << 2) < m) {
        P_i[i].row_size = j;
        P_i[i].count = 0;
        P_i[i].P = init_fingerprint();
        P_i[i].period_f = init_fingerprint();
        set_fingerprint(printer, &predecessor[j], j, P_i[i].P);
        P_i[i].VOs[0].T_f = init_fingerprint();
        P_i[i].VOs[1].T_f = init_fingerprint();
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
    P_i[i].period_f = init_fingerprint();
    set_fingerprint(printer, &predecessor[j], m - j, P_i[i].P);
    free(predecessor);

    P_i[i].VOs[0].T_f = init_fingerprint();
    P_i[i].VOs[1].T_f = init_fingerprint();
    P_i[i].to_zero = malloc(s_sigma * sizeof(zero_item));
    for (k = 0; k < s_sigma; k++) mpz_init(P_i[i].to_zero[k].r_z);
    P_i[i].zero_start = 0;
    P_i[i].zero_end = 0;

    lm = i + 1;
    P_i = realloc(P_i, lm * sizeof(pattern_row));

    fingerprint T_f = init_fingerprint(), T_cur = init_fingerprint(), T_prev = init_fingerprint(), tmp = init_fingerprint();
    mpz_t r_z;
    mpz_init(r_z);

    for (i = 0; i < n; i++) {
        lookup = i - (int)rbtree_lookup(t_pred, (void*)T[i], (void*)i, compare_char);
        rbtree_insert(t_pred, (void*)T[i], (void*)i, compare_char);
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
                    if (P_i[j].to_zero[index].z <= i - P_i[j].row_size) {
                        if (++P_i[j].zero_start == s_sigma) P_i[j].zero_start = 0;
                    } else if (P_i[j].to_zero[index].z - P_i[j].to_zero[index].pred <= i - ((j == lm - 1) ? m : (P_i[j].row_size << 1))) {
                        fingerprint_zero(printer, T_cur, P_i[j].to_zero[index].pred, P_i[j].to_zero[index].r_z, tmp);
                    }
                    if (++index == s_sigma) index = 0;
                    fingerprint_assign(tmp, T_cur);
                }
                fingerprint_suffix(printer, T_cur, P_i[j].VOs[0].T_f, T_f);
                if (fingerprint_equals(P_i[j].P, T_f)) {
                    if (j == lm - 1) results[matches++] = i;
                    else add_occurance(printer, T_prev, P_i[j].VOs[0].location + P_i[j].row_size, &P_i[j + 1], tmp);
                }
                shift_row(printer, &P_i[j], tmp);
            }
        }
        if (mmatch_stream(&mmatch, lookup, i) == i) add_occurance(printer, T_prev, i, &P_i[0], tmp);
    }

    mmatch_free(&mmatch);
    rbtree_destroy(t_pred);
    for (i = 0; i < lm; i++) {
        fingerprint_free(P_i[i].P);
        fingerprint_free(P_i[i].period_f);
        fingerprint_free(P_i[i].VOs[0].T_f);
        fingerprint_free(P_i[i].VOs[1].T_f);
        for (k = 0; k < s_sigma; k++) mpz_clear(P_i[i].to_zero[k].r_z);
        free(P_i[i].to_zero);
    }
    free(P_i);
    mpz_clear(r_z);
    return matches;
}

#endif