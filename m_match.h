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
    typedef struct failure_list_t failure_list
    Double-Linked List of failure items.
    Components:
        failure_list_t *pred   - Predecessor item in the list
        failure_list_t *succ   - Successor item in the list
        int            start   - Start of range of indices with this failure value
        int            failure - Failure value
*/
typedef struct failure_list_t {
    struct failure_list_t *pred, *succ;
    int start, failure;
} failure_list;

typedef struct zero_list_t {
    struct zero_list_t *pred, *succ;
    int index;
} zero_list;

/*
    typedef struct mmatch_state
    Structure to hold current state of algorithm.
    Components:
        int          *k             - Length before predecessor changes from zero
        int          *c             - Value after predecessor changes
        int          m              - Length of pattern
        int          i              - Current index of pattern
        int          *failure_table - Failure_table table for pattern
        int          period         - Length of period
        int          has_break      - Does the period break?
        int          pred_break     - The predecessor of the character that breaks the period
        int          failure_break  - The failure value of the character that breaks the period
        failure_list *failure       - The current index in the failure list
        failure_list *failure_reset - Pointer to the index in the failure list to reset to if the pattern is not periodic
*/
typedef struct {
    int *k, *c, m, i, *failure_table, period, has_break, pred_break, failure_break;
    failure_list *failure, *failure_reset;
    zero_list *zeros, *zero_reset;
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
    if (!state.period) return state.k[i];
    if ((state.has_break) && (i == state.m - 1)) return state.pred_break;
    int index = i % state.period;
    return (i < state.k[index]) ? 0 : state.c[index];
}

/*
    get_failure
    Gets the failure value for integer i.
    Parameters:
        mmatch_state *state - The state of the algorithm
        int          i      - The index of the pattern
    Returns int:
        Length of the longest prefix that also p-matches a suffix.
        Failure list in parameter state also updated.
*/
int get_failure(mmatch_state *state, int i) {
    if (!state->period) return i - state->failure_table[i];
    if ((state->has_break) && (i == state->m - 1)) {
        state->failure = state->failure_reset;
        return state->failure_break;
    }
    int index = i - state->failure->failure;
    if ((state->failure->pred != NULL) && (index < state->failure->start)) state->failure = state->failure->pred;
    return index;
}

/*
    update_failure
    Updates the failure list.
    Parameters:
        mmatch_state *state - The state of the algorithm
        int          i      - The index of the pattern
    Returns void:
        Failure list in parameter state updated.
*/
void update_failure(mmatch_state *state, int i) {
    if ((state->period) && (state->failure->succ != NULL) && (i >= state->failure->succ->start)) state->failure = state->failure->succ;
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
    int i, j;
    mmatch_state state;
    state.failure_table = malloc(m * sizeof(int));
    state.failure_table[0] = 1;
    state.k = malloc(m * sizeof(int));
    state.k[0] = 0;
    i = -1;
    for (j = 1; j < m; j++) {
        state.k[j] = p_pred[j];
        while (i > -1 && !compare_pi_pj(i + 1, j, p_pred[i + 1], p_pred[j])) i -= state.failure_table[i];
        if (compare_pi_pj(i + 1, j, p_pred[i + 1], p_pred[j])) i++;
        state.failure_table[j] = j - i;
    }

    state.has_break = 0;
    if ((state.failure_table[m - 1] << 1) <= m) {
        int k, failure_j;
        state.period = state.failure_table[m - 1];
        state.k = realloc(state.k, state.period * sizeof(int));
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

        state.failure = malloc(sizeof(failure_list));
        state.failure->failure = 1;
        state.failure->start = 0;
        state.failure->pred = NULL;
        state.failure->succ = NULL;
        j = 0;
        while ((j < m) && (state.failure->failure < state.period)) {
            if (state.failure_table[j] != state.failure->failure) {
                state.failure->succ = malloc(sizeof(failure_list));
                state.failure->succ->pred = state.failure;
                state.failure = state.failure->succ;
                state.failure->failure = state.failure_table[j];
                state.failure->start = j;
                state.failure->succ = NULL;
            }
            j++;
        }
        while ((state.failure->pred != NULL) && (state.failure->pred->start >= i)) state.failure = state.failure->pred;

        state.zeros = malloc(sizeof(zero_list));
        state.zeros->index = 0;
        state.zeros->pred = NULL;
        state.zeros->succ = NULL;
        for (j = 1; j < m; j++) {
            if (p_pred[j] == 0) {
                state.zeros->succ = malloc(sizeof(zero_list));
                state.zeros->succ->pred = state.zeros;
                state.zeros = state.zeros->succ;
                state.zeros->index = j;
                state.zeros->succ = NULL;
            }
        }

        while ((state.zeros->pred != NULL) && (state.zeros->pred->index >= i)) state.zeros = state.zeros->pred;

        j = m;
        failure_j = state.failure_table[j - 1];
        free(state.failure_table);
        while ((j < p_len) && ((failure_j << 1) < m)) {
            while (i > -1 && !compare_pi_pj(i + 1, j, p_pred[i + 1], p_pred[j])) i = get_failure(&state, i);
            if (compare_pi_pj(i + 1, j, p_pred[i + 1], p_pred[j])) {
                i++;
                update_failure(&state, i);
            }
            failure_j = j - i;
            if ((failure_j << 1) >= m) {
                state.has_break = 1;
                state.pred_break = p_pred[j];
                state.failure_break = i;
                state.failure_reset = state.failure;
                state.zero_reset = state.zeros;
            } else if ((state.c[j % state.period] == 0) && (p_pred[j] != 0)) {
                state.c[j % state.period] = p_pred[j];
                state.k[j % state.period] = j;
            }
            j++;
        }

        while (state.zeros->pred != NULL) state.zeros = state.zeros->pred;
        while (state.failure->pred != NULL) state.failure = state.failure->pred;

    } else {
        state.period = 0;
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
    int i = state->i, result = -1;
    while (i > -1 && !compare_pi_tj(i + 1, t_pred, get_pred(*state, i + 1))) i = get_failure(state, i);
    if (compare_pi_tj(i + 1, t_pred, get_pred(*state, i + 1))) {
        i++;
        update_failure(state, i);
    }
    if (i == state->m - 1) {
        result = j;
        i = get_failure(state, i);
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
    free(state->k);
    if (state->period) {
        free(state->c);
        while (state->zeros->pred != NULL) state->zeros = state->zeros->pred;
        while (state->zeros->succ != NULL) {
            state->zeros = state->zeros->succ;
            free(state->zeros->pred);
        }
        free(state->zeros);
        while (state->failure->pred != NULL) state->failure = state->failure->pred;
        while (state->failure->succ != NULL) {
            state->failure = state->failure->succ;
            free(state->failure->pred);
        }
        free(state->failure);
    } else {
        free(state->failure_table);
    }
}

#endif
