// implements driver functions for integer B-trees

#include <stdio.h>
#include "gist.h"
#include "gist_btree.h"

void
printDatum(
    void *key,
    int klen,
    void *data,
    int dlen)
{
    int *k = (int *) key;
    int *d = (int *) data;

    printf("%d: %d\n", *k, *d);
}

char *identStr = "integer B-tree";

gist_ext_t *extension = &bt_int_ext;



