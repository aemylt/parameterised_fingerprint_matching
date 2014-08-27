#include "hash_lookup.h"
#include <assert.h>

int main(void) {
    char **keys = malloc(10 * sizeof(char*));
    int i;
    for (i = 0; i < 10; i++) keys[i] = malloc(sizeof(char));
    keys[0][0] = 'a'; keys[1][0] = 'b'; keys[2][0] = 'X'; keys[3][0] = 'Y'; keys[4][0] = '0';
    keys[5][0] = '1'; keys[6][0] = '?'; keys[7][0] = '$'; keys[8][0] = '~'; keys[9][0] = '@';

    int *values = malloc(10 * sizeof(int));
    for (i = 0; i < 10; i++) values[i] = i * i;

    hash_lookup lookup = hashlookup_build(keys, values, 10);
    for (i = 0; i < 10; i++) assert(hashlookup_search(lookup, keys[i][0]) == values[i]);
    assert(hashlookup_search(lookup, 'Z') == -1);
    hashlookup_free(&lookup);

    lookup = hashlookup_build(keys, values, 1);
    assert(hashlookup_search(lookup, 'a') == 0);
    assert(hashlookup_search(lookup, 'Z') == -1);
    hashlookup_free(&lookup);

    lookup = hashlookup_build(keys, values, 0);
    assert(hashlookup_search(lookup, 'a') == -1);
    assert(hashlookup_search(lookup, 'Z') == -1);
    hashlookup_free(&lookup);

    keys[1][0] = '1';
    lookup = hashlookup_build(keys, values, 2);
    assert(hashlookup_search(lookup, 'a') == 0);
    assert(hashlookup_search(lookup, '1') == 1);
    assert(hashlookup_search(lookup, 'Z') == -1);
    hashlookup_free(&lookup);

    lookup = hashlookup_build(keys, values, 2);
    keys[1][0] = 'Z';
    assert(hashlookup_search(lookup, 'a') == 0);
    assert(hashlookup_search(lookup, '1') == 1);
    assert(hashlookup_search(lookup, 'Z') == -1);
    hashlookup_free(&lookup);

    return 0;
}