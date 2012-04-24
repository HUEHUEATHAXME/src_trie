#ifndef TRIE_H
#define TRIE_H

typedef struct trie trie_t;
struct trie;

/* Comparison function. Returns a value:
   > 0  , if a > b
   == 0 , if a == b
   < 0  , if a < b */
typedef int (*cmpfunc_t)(void *a, void *b);

/* Digit function.  Returns value of bits in position 'bitpos' of key.
   'radixbits' specifies the # bits per increment of 'bitpos' */
typedef int (*digitfunc_t)(void *key, int bitpos, int radixbits);

/* Create new TRIE */
trie_t *trie_new(cmpfunc_t cmpfunc, digitfunc_t digitfunc);

/* Insert item 'value' into TRIE with key 'key' */
int trie_insert(trie_t *t, void *key, void *value);

/* Search TRIE for item with key 'key' */
void *trie_search(trie_t *t, void *key);

/* Plot TRIE */
void trie_print(trie_t *t);

#endif  /* TRIE_H */
