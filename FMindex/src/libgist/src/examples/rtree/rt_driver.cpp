// implements driver functions for 2-dim point R-trees

#include <stdio.h>
#include "gist.h"
#include "command.h"
#include "gist_rtree.h"

void
printDatum(
    void *key,
    int klen,
    void *data,
    int dlen)
{
    double *k = (double *) key;
    int *d = (int *) data;

    printf("(%f, %f): %d\n", k[0], k[1], *d);
}

char *identStr = "2-dim point R-tree";

gist_ext_t *extension = &rt_point_ext;



