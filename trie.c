#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "plot.h"
#include "trie.h"

#define TRIE_RADIX_BITS         1
#define TRIE_RADIX              (1 << TRIE_RADIX_BITS)


typedef struct node node_t;

struct trie {
    cmpfunc_t cmpfunc;
    digitfunc_t digitfunc;
    node_t *root;
};

struct node {
    void *key;
    void *value;
    int id;
    node_t *children[TRIE_RADIX];
};


static node_t *node_new(void *key, void *value)
{
    node_t *node;

    node = (node_t*)calloc(1, sizeof(node_t));
    if (node == NULL)
        goto error;
    node->key = key;
    node->value = value;
    
    return node;
 error:
    return NULL;
}


static char *strnode(char *name, node_t *n)
{
    static int cnt;

    if (n->id == 0)
        n->id = ++cnt;

    if (n->key == NULL)
        sprintf(name, "_%d", n->id);
    else
        sprintf(name, "_%d_%d [color=red]", *((int*)n->key), n->id);
    return name;
}


static inline int isleaf(node_t *node)
{
    int i;

    for (i = 0; i < TRIE_RADIX; i++) {
        if (node->children[i] != NULL)
            break;
    }
    if (i >= TRIE_RADIX)
        return 1;
    else
        return 0;
}

static void _trie_print(plot_t *plot, node_t *current)
{
    int i;
    char from[128];
    char to[128];

    if (current == NULL)
        return;

    strnode(from, current);
    for (i = 0; i < TRIE_RADIX; i++ ) {
        if (current->children[i] != NULL)
            plot_addlink(plot, from, strnode(to, current->children[i]), "");
    }
    
    for (i = 0; i < TRIE_RADIX; i++ ) {
        _trie_print(plot, current->children[i]);
    }    
}


static node_t *_split(trie_t *trie, node_t *old, node_t *new, int bitpos)
{
    int odigit, ndigit;
    node_t *intern;

    intern = node_new(NULL, NULL);
    if (intern == NULL)
        goto error;

    odigit = trie->digitfunc(old->key, bitpos, TRIE_RADIX_BITS);
    ndigit = trie->digitfunc(new->key, bitpos, TRIE_RADIX_BITS);
    if (odigit == ndigit) {
        intern->children[odigit] = _split(trie, old, new, bitpos+1);
        if (intern->children[odigit] == NULL || intern->children[odigit] == old)
            goto error;
    } else {
        intern->children[odigit] = old;
        intern->children[ndigit] = new;
    }

    return intern;
 error:
    if (intern != NULL)
        free(intern);
    return old;
}




void trie_print(trie_t *trie)
{
    plot_t *plot;
    
    plot = plot_new();

    _trie_print(plot, trie->root);
    
    plot_doplot(plot);
}



trie_t *trie_new(cmpfunc_t cmpfunc, digitfunc_t digitfunc)
{
    trie_t *trie;

    trie = (trie_t*)malloc(sizeof(trie_t));
    if (trie == NULL)
        goto error;
    
    trie->cmpfunc = cmpfunc;
    trie->digitfunc = digitfunc;

    return trie;
 error:
    return NULL;
}


static node_t *_trie_insert(trie_t *trie, node_t *root, node_t *new, int bitpos)
{
    int i;
    int digit;

    if (root == NULL)
        return new;

    if (isleaf(root))
        return _split(trie, root, new, bitpos);

    digit = trie->digitfunc(new->key, bitpos, TRIE_RADIX_BITS);
    assert(digit < TRIE_RADIX);
    root->children[digit] = _trie_insert(trie, 
                                         root->children[digit], 
                                         new, bitpos+1);
    return root;
}


int trie_insert(trie_t *trie, void *key, void *value)
{
    node_t *new;

    new = node_new(key, value);
    if (new == NULL)
        goto error;

    trie->root = _trie_insert(trie, trie->root, new, 0);

    // one memory leak: if insert fails we should free 'new'

    return 0;
 error:
    return -1;
}


void *_trie_search(trie_t *trie, node_t *root, void *key, int bitpos)
{       
    int digit;

    if (root == NULL)
        return NULL;

    if (isleaf(root)) {
        if (trie->cmpfunc(root->key, key) == 0)
            return root->value;
        else
            return NULL;
    }

    digit = trie->digitfunc(key, bitpos, TRIE_RADIX_BITS);
    assert(digit < TRIE_RADIX);
    return _trie_search(trie, root->children[digit], key, bitpos+1);
}


void *trie_search(trie_t *trie, void *key)
{
    return _trie_search(trie, trie->root, key, 0);

}
