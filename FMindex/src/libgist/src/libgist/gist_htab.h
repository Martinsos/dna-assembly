//	gist_htab.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist_htab.h,v 1.2 1997/12/05 23:08:45 marcel Exp $

#ifdef __GNUG__
#pragma interface
#endif

#ifndef GIST_HTAB_H
#define GIST_HTAB_H

#include "gist_defs.h"

class gist_htab {

public:

    gist_htab();
    ~gist_htab();

    int operator[](shpid_t page);
    void add(shpid_t key, int value);
    void remove(shpid_t key);

protected:
    
    struct entry {
        shpid_t key;
	int value;
	bool inUse;
    };

    entry tab[GISTBUFS];

    int hash(shpid_t key);
};

#endif // GIST_HTAB_H
