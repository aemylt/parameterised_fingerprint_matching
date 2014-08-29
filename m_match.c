#include "m_match.h"
#include "hash_lookup.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void stream_test(char *T, int n, int *P, int m, char *sigma, int s_sigma, int *correct) {
    mmatch_state state = mmatch_build(P, m, sigma, s_sigma);
    int j, pred, *predecessor = malloc(s_sigma * sizeof(int));
    for (j = 0; j < s_sigma; j++) predecessor[j] = -1;
    hash_lookup t_pred = hashlookup_build(sigma, predecessor, s_sigma);
    free(predecessor);
    for (j = 0; j < n; j++) {
        pred = hashlookup_search(t_pred, T[j]);
        pred = (pred == -1) ? 0 : j - pred;
        assert(correct[j] == mmatch_stream(&state, pred, j));
        hashlookup_edit(&t_pred, T[j], j);
    }

    mmatch_free(&state);
    hashlookup_free(&t_pred);
}

int main(void) {
    int *pattern = malloc(5 * sizeof(int));
    pattern[0] = 0; pattern[1] = 0; pattern[2] = 2; pattern[3] = 2; pattern[4] = 1;

    int *correct = malloc(20 * sizeof(int));
    correct[0]  = -1; correct[1]  = -1; correct[2]  = -1; correct[3]  = -1; correct[4]  = 4;
    correct[5]  = -1; correct[6]  = -1; correct[7]  = -1; correct[8]  = -1; correct[9]  = -1;
    correct[10] = -1; correct[11] = -1; correct[12] = -1; correct[13] = -1; correct[14] = 14;
    stream_test("ababbaaaaababaa", 15, pattern, 5, "ab", 2, correct);

    correct[0]  = -1; correct[1]  = -1; correct[2]  = -1; correct[3]  = -1; correct[4]  = 4;
    correct[5]  = -1; correct[6]  = -1; correct[7]  = -1; correct[8]  = -1; correct[9]  = -1;
    correct[10] = -1; correct[11] = -1; correct[12] = -1; correct[13] = -1; correct[14] = 14;
    stream_test("cdcddcccccdcdcc", 15, pattern, 5, "cd", 2, correct);

    correct[0]  = -1; correct[1]  = -1; correct[2]  = -1; correct[3]  = -1; correct[4]  = -1; correct[5]  = 5;
    correct[6]  = -1; correct[7]  = -1; correct[8]  = -1; correct[9]  = -1; correct[10] = -1; correct[11] = -1;
    correct[12] = 12; correct[13] = -1; correct[14] = -1; correct[15] = -1; correct[16] = -1; correct[17] = 17;
    stream_test("ababaabbababbababb", 18, pattern, 5, "ab", 2, correct);

    correct[0]  = -1; correct[1]  = -1; correct[2]  = -1; correct[3]  = -1; correct[4]  = -1; correct[5]  = -1;
    correct[6]  = -1; correct[7]  = -1; correct[8]  = -1; correct[9]  = -1; correct[10] = -1; correct[11] = -1;
    correct[12] = -1; correct[13] = -1; correct[14] = -1; correct[15] = -1; correct[16] = -1; correct[17] = -1;
    stream_test("ababababababababab", 18, pattern, 5, "ab", 2, correct);

    free(correct);
    free(pattern);
    return 0;
}
