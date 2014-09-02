#include "parameterised_matching.h"
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    int *results = malloc(100 * sizeof(int)), matches, i;

    matches = parameterised_match("aaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 100, "aaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaa", 65, "abc", 3, 0, results);
    for (i = 0; i < matches - 1; i++) printf("%d, ", results[i]);
    if (matches) printf("%d\n", results[matches - 1]);
    else printf("No matches\n");

    matches = parameterised_match("aaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 200, "cccccaaaaacccccbbbbbcccccaaaaacccccbbbbbcccccaaaaacccccbbbbbccccc", 65, "abc", 3, 0, results);
    for (i = 0; i < matches - 1; i++) printf("%d, ", results[i]);
    if (matches) printf("%d\n", results[matches - 1]);
    else printf("No matches\n");

    matches = parameterised_match("aaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 200, "aaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaabb", 65, "abc", 3, 0, results);
    for (i = 0; i < matches - 1; i++) printf("%d, ", results[i]);
    if (matches) printf("%d\n", results[matches - 1]);
    else printf("No matches\n");

    return 0;
}