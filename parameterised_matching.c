#include "parameterised_matching.h"
#include <stdlib.h>
#include <stdio.h>

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

/*
    get_char
    Retrieves the i-th character from string T.
    Needed due to type conversions.
    Parameters:
        void** T - List of elements
        int i - Index
    Returns void*:
        T[i]
*/
void* get_char(void** T, int i) {
    return (void*)((char*)T)[i];
}

int main(void) {
    int *results = malloc(100 * sizeof(int)), matches, i;

    matches = parameterised_match((void**)"aaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 100, (void**)"aaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaa", 65, 0, compare_char, get_char, results);
    for (i = 0; i < matches - 1; i++) printf("%d, ", results[i]);
    if (matches) printf("%d\n", results[matches - 1]);
    else printf("No matches\n");

    matches = parameterised_match((void**)"aaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 200, (void**)"cccccaaaaacccccbbbbbcccccaaaaacccccbbbbbcccccaaaaacccccbbbbbccccc", 65, 0, compare_char, get_char, results);
    for (i = 0; i < matches - 1; i++) printf("%d, ", results[i]);
    if (matches) printf("%d\n", results[matches - 1]);
    else printf("No matches\n");

    matches = parameterised_match((void**)"aaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 200, (void**)"aaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaabb", 65, 0, compare_char, get_char, results);
    for (i = 0; i < matches - 1; i++) printf("%d, ", results[i]);
    if (matches) printf("%d\n", results[matches - 1]);
    else printf("No matches\n");

    matches = parameterised_match((void**)"aaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaabbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaabbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 200, (void**)"aaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaaaaabbbbbaaaaacccccaabbb", 65, 0, compare_char, get_char, results);
    for (i = 0; i < matches - 1; i++) printf("%d, ", results[i]);
    if (matches) printf("%d\n", results[matches - 1]);
    else printf("No matches\n");

    matches = parameterised_match((void**)"aaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaaaaaaaaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaaaaaaaaaaaabbbbbaaaaabbbbb", 200, (void**)"aaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaaaaaaa", 80, 0, compare_char, get_char, results);
    for (i = 0; i < matches - 1; i++) printf("%d, ", results[i]);
    if (matches) printf("%d\n", results[matches - 1]);
    else printf("No matches\n");

    matches = parameterised_match((void**)"aaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbb", 200, (void**)"aaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbb", 80, 0, compare_char, get_char, results);
    for (i = 0; i < matches - 1; i++) printf("%d, ", results[i]);
    if (matches) printf("%d\n", results[matches - 1]);
    else printf("No matches\n");

    matches = parameterised_match((void**)"aaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbaaaaabbbbbaaaaabbbbbaaaaabbbbb", 200, (void**)"aaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbaaaaabbbbb", 80, 0, compare_char, get_char, results);
    for (i = 0; i < matches - 1; i++) printf("%d, ", results[i]);
    if (matches) printf("%d\n", results[matches - 1]);
    else printf("No matches\n");

    matches = parameterised_match((void**)"aaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaaabbbbbbbbbbbbbbaaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaaabbbbbbbbbbbbbb", 200, (void**)"aaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbbbbbbaaaaaaaaaaabbbb", 90, 0, compare_char, get_char, results);
    for (i = 0; i < matches - 1; i++) printf("%d, ", results[i]);
    if (matches) printf("%d\n", results[matches - 1]);
    else printf("No matches\n");

    return 0;
}