/*
    m_match.h
    Implementation of the mapped matching algorithm by Amir, Farach and Muthukrishnan.
    More information is available here: http://www.cs.rutgers.edu/~farach/pubs/ParametrizedMatching.pdf
    This variant of the KMP algorithm is from lecture slides courtesy of Dr. Raphaël Clifford: (private link) https://www.cs.bris.ac.uk/Teaching/Resources/COMS21103/material-dima/string_matching.pdf
*/

#ifndef M_MATCH
#define M_MATCH

#include <stdlib.h>

/*
    int compare_pi_tj(int i, char t_j, rbtree text, int j, int* A)
    Implements the Compare(p_i, t_j) function by Amir et al.
    Parameters:
        int    i    - Index of pattern
        char   t_j  - Current character in text
        rbtree text - Red/Black tree of when each character in the alphabet last occured in the text
        int    j    - Index of text
        int*   A    - Predecessor table
    Returns int:
        1 if p_i \cong t_j
        0 otherwise
*/
int compare_pi_tj(int i, int t_pred, int* A) {
    return ((A[i] == t_pred) || ((A[i] == 0) && (t_pred > i)));
}

/*
    int compare_pi_pj(int i, char* p, int j, int* A)
    Implements the Compare(p_i, p_j) function by Amir et al.
    Parameters:
        int   i - Index i of pattern
        int   j - Index j of pattern
        int*  A - Predecessor table
    Returns int:
        1 if p_i \cong p_j
        0 otherwise
*/
int compare_pi_pj(int i, int j, int* A) {
    return ((A[i] == A[j]) || ((A[i] == 0) && (A[j] > i)));
}

/*
    typedef struct mmatch_state_t *mmatch_state
    Structure to hold current state of algorithm.
    Components:
        int*   A       - Predecessor table for pattern
        int    m       - Length of pattern
        int    i       - Current index of pattern
        int*   failure - Failure table for pattern
        rbtree text    - Predecessor tree for text
*/
typedef struct {
    int* A;
    int m;
    int i;
    int* failure;
} mmatch_state;

/*
    mmatch_state mmatch_build(char* P, int m)
    Creates an initial state for m-match algorithm.
    Parameters:
        char* P - Pattern
        int   m - Length of pattern
    Returns mmatch_state:
        Initial state for algorithm
*/
mmatch_state mmatch_build(int* A, int m) {
    int i, j;
    mmatch_state state;
    state.A = malloc(m * sizeof(int));
    for (i = 0; i < m; i++) state.A[i] = A[i];
    state.failure = malloc(m * sizeof(int));
    i = -1;
    state.failure[0] = -1;
    for (j = 1; j < m; j++) {
        while (i > -1 && !compare_pi_pj(i + 1, j, A)) i = state.failure[i];
        if (compare_pi_pj(i + 1, j, A)) i++;
        state.failure[j] = i;
    }
    state.m = m;
    state.i = -1;
    return state;
}

/*
    int mmatch_stream(mmatch_state state, char T_j)
    Returns whether an m-match occurs for character T_j.
    Parameters:
        mmatch_state state - The current state of the algorithm
        char         T_j   - The next character in the text
        int          j     - The current index of the text
    Returns int:
        j  if P m-matches T[j - m + 1:j]
        -1 otherwise
*/
int mmatch_stream(mmatch_state *state, int t_pred, int j) {
    int* A = state->A;
    int* failure = state->failure;
    int i = state->i, result = -1;
    while (i > -1 && !compare_pi_tj(i + 1, t_pred, A)) i = failure[i];
    if (compare_pi_tj(i + 1, t_pred, A)) i++;
    if (i == state->m - 1) {
        result = j;
        i = failure[i];
    }
    state->i = i;
    return result;
}

void mmatch_free(mmatch_state *state) {
    free(state->A);
    free(state->failure);
}

#endif
