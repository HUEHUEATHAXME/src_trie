#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "trie.h"

int data[1024];

void fatal_error(char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    vprintf(msg, ap);
    
    exit(1);
}


static int cmpfunc(void *a, void *b)
{
    return (*(int*)a) - (*(int*)b);
}

static int digitfunc(void *key, int bitpos, int radixbits)
{
    int _key, bitstart, bitmask;

    assert(key != NULL);

    _key = *((int*)key);

    // radixbits specifies # bits per increment of bitpos.
    // bitpos*radixbits == position of first bit asked for
    bitstart = bitpos*radixbits;
    assert(bitstart+radixbits < 32);

    // Create mask with 'radixbits' set, e.g.:
    // radixbits = 1 => bitmask = (1 << 1)-1 = 1 = 1 (binary)
    // radixbits = 2 => bitmask = (1 << 2)-1 = 3 = 11 (binary)
    // radixbits = 3 => bitmask = (1 << 3)-1 = 7 = 111 (binary)
    bitmask = (1 << radixbits)-1;

    // Extract bits from key
    return (_key >> bitstart) & bitmask;
}


int main(int argc, char **argv)
{
    int i;
    trie_t *t;
    void *value;

    for (i = 0; i < 1024; i++) {
        data[i]= i;
    }

    t = trie_new(cmpfunc, digitfunc);
    if (t == NULL)
        fatal_error("Unable to create Trie\n");

    for (i = 0; i < 32; i++) {
        printf("Inserting %d\n", i);
        trie_insert(t, &data[i], data+i);
    }

    for (i = 0; i < 32; i++ ) {
        value = trie_search(t, &data[i]);
        if (value != NULL)
            printf("Found key %d\n", data[i]);
        else 
            printf("Unable to find key %d\n", data[i]);
    }   

    trie_print(t);
}
