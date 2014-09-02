/*
    m_match.h
    Implementation of the mapped matching algorithm by Amir, Farach and Muthukrishnan and used by Jalsenius, Porat and Sach.
    Adapted from my previous work on parameterised matching: https://github.com/djmylt/parameterised_matching
    More information is available here: http://www.cs.rutgers.edu/~farach/pubs/ParametrizedMatching.pdf http://arxiv.org/pdf/1109.5269v3.pdf
    This variant of the KMP algorithm is from lecture slides courtesy of Dr. Raphaël Clifford: (private link) https://www.cs.bris.ac.uk/Teaching/Resources/COMS21103/material-dima/string_matching.pdf
*/

#ifndef M_MATCH
#define M_MATCH

#include <stdlib.h>

/*
    compare_pi_tj
    Implements the Compare(p_i, t_j) function by Amir et al.
    Parameters:
        int  i      - Index of pattern
        int  t_pred - Last occurance of T[j]
        int  i_pred - Last occurance of P[i]
    Returns int:
        1 if P[i] \cong T[j]
        0 otherwise
*/
int compare_pi_tj(int i, int t_pred, int i_pred) {
    return ((i_pred == t_pred) || ((i_pred == 0) && (t_pred > i)));
}

/*
    compare_pi_pj
    Implements the Compare(p_i, p_j) function by Amir et al.
    Parameters:
        int  i - Index i of pattern
        int  j - Index j of pattern
        int* A - Predecessor table
        int  i_pred - Last occurance of P[i]
        int  j_pred - Last occurance of P[j]
    Returns int:
        1 if P[i] \cong P[j]
        0 otherwise
*/
int compare_pi_pj(int i, int j, int i_pred, int j_pred) {
    return ((i_pred == j_pred) || ((i_pred == 0) && (j_pred > i)));
}

/*
    typedef struct mmatch_state
    Structure to hold current state of algorithm.
    Components:
        int *k         - Length before predecessor changes from zero
        int *c         - Value after predecessor changes
        int m          - Length of pattern
        int i          - Current index of pattern
        int *failure   - Failure table for pattern
        int period     - Length of period
        int has_break  - Does the period break?
        int pred_break - The predecessor of the character that breaks the period
*/
typedef struct {
    int *k, *c, m, i, *failure, period, has_break, pred_break;
} mmatch_state;

/*
    get_pred
    Returns the predecessor of the character.
    Parameters:
        mmatch_state state - The state of the algorithm
        int          i     - The index of the pattern
    Returns int:
        How long ago was the last time this character occured.
        0 if this is the first time we've seen the character.
*/
int get_pred(mmatch_state state, int i) {
    if ((state.has_break) && (i == state.m - 1)) return state.pred_break;
    else {
        int index = i % state.period;
        return (i < state.k[index]) ? 0 : state.c[index];
    }
}

/*
    mmatch_build
    Creates an initial state for m-match algorithm.
    Parameters:
        int *p_pred - Predecessor list for pattern
        int m       - Minimum length of pattern
        int p_len   - Maximum length of pattern
    Returns mmatch_state:
        Initial state for algorithm
*/
mmatch_state mmatch_build(int *p_pred, int m, int p_len) {
    int i, j, k;
    mmatch_state state;
    state.failure = malloc(m * sizeof(int));
    i = -1;
    state.failure[0] = 1;
    for (j = 1; j < m; j++) {
        while (i > -1 && !compare_pi_pj(i + 1, j, p_pred[i + 1], p_pred[j])) i -= state.failure[i];
        if (compare_pi_pj(i + 1, j, p_pred[i + 1], p_pred[j])) i++;
        state.failure[j] = j - i;
    }

    state.has_break = 0;
    state.period = state.failure[m - 1];
    state.k = malloc(state.period * sizeof(int));
    state.c = malloc(state.period * sizeof(int));

    for (j = 0; j < state.period; j++) {
        k = j;
        state.c[j] = 0;
        state.k[j] = 0;
        while ((k < m) && state.c[j] == 0) {
            if (p_pred[k] != 0) {
                state.c[j] = p_pred[k];
                state.k[j] = k;
            }
            k += state.period;
        }
    }

    j = m;
    while ((j < p_len) && (state.failure[j - 1] < m)) {
        while (i > -1 && !compare_pi_pj(i + 1, j, p_pred[i + 1], p_pred[j])) i -= state.failure[i];
        if (compare_pi_pj(i + 1, j, p_pred[i + 1], p_pred[j])) i++;
        state.failure[j] = j - i;
        if (state.failure[j] >= m) {
            state.has_break = 1;
            state.pred_break = p_pred[j];
        } else if ((state.c[j % state.period] == 0) && (p_pred[j] != 0)) {
            state.c[j % state.period] = p_pred[j];
            state.k[j % state.period] = j;
        }
        j++;
    }
    state.m = j;
    state.i = -1;
    return state;
}

/*
    mmatch_stream
    Returns whether an m-match occurs for character T_j.
    Parameters:
        mmatch_state *state - The current state of the algorithm
        int          t_pred - The predecessor of T[j]
        int          j      - The current index of the text
    Returns int:
        j  if P m-matches T[j - m + 1:j]
        -1 otherwise
*/
int mmatch_stream(mmatch_state *state, int t_pred, int j) {
    int* failure = state->failure;
    int i = state->i, result = -1;
    while (i > -1 && !compare_pi_tj(i + 1, t_pred, get_pred(*state, i + 1))) i -= failure[i];
    if (compare_pi_tj(i + 1, t_pred, get_pred(*state, i + 1))) i++;
    if (i == state->m - 1) {
        result = j;
        i -= failure[i];
    }
    state->i = i;
    return result;
}

/*
    mmatch_free
    Frees an mmatch_state from memory.
    Parameters:
        mmatch_state *state - The state to free
*/
void mmatch_free(mmatch_state *state) {
    free(state->failure);
    free(state->k);
    free(state->c);
}

#endif
