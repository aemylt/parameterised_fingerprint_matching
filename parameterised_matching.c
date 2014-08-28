#include "parameterised_matching.h"
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    int *results = malloc(100 * sizeof(int)), matches, i;

    matches = parameterised_match("aaaaabbbbbaaaaacccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 100, "aaaaabbbbbaaaaaccccc", 20, "abc", 3, 0, results);
    for (i = 0; i < matches - 1; i++) printf("%d, ", results[i]);
    if (matches) printf("%d\n", results[matches - 1]);
    else printf("No matches\n");

    matches = parameterised_match("aaaaabbbbbaaaaacccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbaaaaaccccc", 100, "aaaaabbbbbaaaaaccccc", 20, "abc", 3, 0, results);
    for (i = 0; i < matches - 1; i++) printf("%d, ", results[i]);
    if (matches) printf("%d\n", results[matches - 1]);
    else printf("No matches\n");

    return 0;
}