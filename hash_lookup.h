/*
    hash_lookup.h
    A dictionary for storing key-value pairs.
    Utilises the C Minimum Perfect Hashing library (http://cmph.sourceforge.net/)
*/

#ifndef HASH_LOOKUP
#define HASH_LOOKUP

#include <cmph.h>
#include <stdlib.h>

/*
    typedef struct hash_lookup
    Structure for holding the pairs.
    Components:
        cmph_t *hash   - The hash function
        int    *values - The values
        char   *keys   - The keys
        int    num     - The number of items
*/
typedef struct {
    cmph_t *hash;
    int *values, num;
    char *keys;
} hash_lookup;

/*
    hashlookup_build
    Constructs a hash_lookup object.
    Components:
        char **keys - A list of the keys as strings 1 character long
        int *values - A list of the values for each key
        int num - The number of key-value pairs
    Returns hash_lookup:
        The constructed dictionary
*/
hash_lookup hashlookup_build(char *key_string, int *values, int num) {
    hash_lookup lookup;
    lookup.num = num;

    if (num > 1) {
        int i;
        char **keys = malloc(num * sizeof(char*));
        for (i = 0; i < num; i++) {
            keys[i] = malloc(sizeof(char) << 1);
            keys[i][0] = key_string[i];
            keys[i][1] = '\0';
        }
        cmph_io_adapter_t *source = cmph_io_vector_adapter(keys, num);
        cmph_config_t *config = cmph_config_new(source);
        cmph_config_set_algo(config, CMPH_CHD);
        lookup.hash = cmph_new(config);
        cmph_config_destroy(config);
        cmph_io_vector_adapter_destroy(source);
        lookup.keys = malloc(num * sizeof(char));
        lookup.values = malloc(num * sizeof(int));

        unsigned int id;
        char key;
        for (i = 0; i < num; i++) {
            key = key_string[i];
            id = cmph_search(lookup.hash, &key, 1);
            lookup.keys[id] = key;
            lookup.values[id] = values[i];
        }
    } else if (num == 1) {
        lookup.keys = malloc(sizeof(char));
        lookup.keys[0] = key_string[0];
        lookup.values = malloc(sizeof(int));
        lookup.values[0] = values[0];
    }

    return lookup;
}

/*
    hashlookup_search
    Searches the dictionary for the corresponding value to a key.
    Parameters:
        hash_lookup lookup - The dictionary to search
        char        key    - The key to search for
    Returns int:
        values[lookup.hash(key)] if key \in keys
        -1 otherwise
*/
int hashlookup_search(hash_lookup lookup, char key) {
    if (lookup.num == 0) return -1;
    if (lookup.num == 1) return (key == lookup.keys[0]) ? lookup.values[0] : -1;
    int id = cmph_search(lookup.hash, &key, 1);
    return ((id < lookup.num) && (key == lookup.keys[id])) ? lookup.values[id] : -1;
}

/*
    hashlookup_edit
    Sets values[lookup.hash(key)] to value if key is in the dictionary.
    Parameters:
        hash_lookup lookup - The dictionary to edit
        char        key    - The key to change
        char        value  - The value to change it to
    Returns void:
        Parameter lookup modified by reference
        values[lookup.hash(key)] = value if key \in keys
*/
void hashlookup_edit(hash_lookup *lookup, char key, int value) {
    if ((lookup->num == 1) && (key == lookup->keys[0])) lookup->values[0] = value;
    else if (lookup->num > 1) {
        int id = cmph_search(lookup->hash, &key, 1);
        if ((id < lookup->num) && (key == lookup->keys[id])) lookup->values[id] = value;
    }
}

/*
    hashlookup_free
    Frees the dictionary.
    Parameters:
        hash_lookup *lookup - The dictionary to free
*/
void hashlookup_free(hash_lookup *lookup) {
    if (lookup->num > 0) {
        if (lookup->num > 1) cmph_destroy(lookup->hash);
        free(lookup->values);
        free(lookup->keys);
    }
}

#endif