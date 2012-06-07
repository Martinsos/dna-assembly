//	gist_unordered.cc
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist_unordered.cc,v 1.2 1997/12/05 23:08:48 marcel Exp $


#ifdef LIBGIST
#include <assert.h>
#else 
#define SM_SOURCE
#define GIST_C
#include <sm_int_2.h>
#endif
#include "gist_p.h"

#ifdef __GNUG__
#pragma implementation "gist_unordered.h"
#endif
#include "gist_unordered.h"

void
gist_predcursor_t::add(
    const void*		data,
    int 		len)
{
    w_assert3(numElems < maxElems);
    elems[numElems].key = data;
    elems[numElems].keyLen = len;
    numElems++;
}

void
gist_predcursor_t::add(
    gist_p&		page)
{
    int cnt = page.nrecs();
    w_assert3(numElems + cnt <= maxElems);
    for (int i = 0; i < cnt; i++) {
        elems[numElems].key = (void *) page.rec(i).key();
        elems[numElems].keyLen = page.rec(i).klen();
	numElems++;
    }
}

gist_predcursor_t::~gist_predcursor_t()
{
}

gist_predcursor_t::gist_predcursor_t() : numElems(0)
{
}

