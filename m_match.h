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
        int  t_pred - Last occurance of T
        int* A      - Predecessor table
    Returns int:
        1 if p_i \cong t_j
        0 otherwise
*/
#define compare_pi_tj(i, t_pred, A) ((A[i] == t_pred) || ((A[i] == 0) && (t_pred > i)))

/*
    compare_pi_pj
    Implements the Compare(p_i, p_j) function by Amir et al.
    Parameters:
        int  i - Index i of pattern
        int  j - Index j of pattern
        int* A - Predecessor table
    Returns int:
        1 if p_i \cong p_j
        0 otherwise
*/
#define compare_pi_pj(i, j, A) ((A[i] == A[j]) || ((A[i] == 0) && (A[j] > i)))

/*
    typedef struct mmatch_state
    Structure to hold current state of algorithm.
    Components:
        int* A       - Predecessor table for pattern
        int  m       - Length of pattern
        int  i       - Current index of pattern
        int* failure - Failure table for pattern
*/
typedef struct {
    int* A;
    int m;
    int i;
    int* failure;
} mmatch_state;

/*
    mmatch_build
    Creates an initial state for m-match algorithm.
    Parameters:
        int *A    - Predecessor list for pattern
        int m     - Minimum length of pattern
        int p_len - Maximum length of pattern
    Returns mmatch_state:
        Initial state for algorithm
*/
mmatch_state mmatch_build(int *A, int m, int p_len) {
    int i, j;
    mmatch_state state;
    state.A = malloc(p_len * sizeof(int));
    for (i = 0; i < m; i++) state.A[i] = A[i];
    state.failure = malloc(p_len * sizeof(int));
    i = -1;
    state.failure[0] = 1;
    for (j = 1; j < m; j++) {
        while (i > -1 && !compare_pi_pj(i + 1, j, A)) i -= state.failure[i];
        if (compare_pi_pj(i + 1, j, A)) i++;
        state.failure[j] = j - i;
    }

    while ((j < p_len) && (state.failure[j - 1] < m)) {
        state.A[j] = A[j];
        while (i > -1 && !compare_pi_pj(i + 1, j, A)) i -= state.failure[i];
        if (compare_pi_pj(i + 1, j, A)) i++;
        state.failure[j] = j - i;
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
    int* A = state->A;
    int* failure = state->failure;
    int i = state->i, result = -1;
    while (i > -1 && !compare_pi_tj(i + 1, t_pred, A)) i -= failure[i];
    if (compare_pi_tj(i + 1, t_pred, A)) i++;
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
    free(state->A);
    free(state->failure);
}

#endif
