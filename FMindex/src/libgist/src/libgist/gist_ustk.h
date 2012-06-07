//	gist_ustk.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist_ustk.h,v 1.1.1.1 1997/11/13 02:43:58 marcel Exp $


#ifndef GIST_USTK_H
#define GIST_USTK_H

#include "gist_p.h"

struct gist_ustk_entry {
    gist_p page;
    int idx;
};

class gist_ustk {
protected:
    enum { // MSVC 5.0 doesn't allow "const int ..." here
        max_ustk_sz = 10
    };

    gist_ustk_entry _stack[max_ustk_sz];
    int _top;

public:

    gist_ustk() : _top(0) {}
    ~gist_ustk();

    void push(const gist_p &page, int2 idx);

    // return entry n levels down from top of stack
    // (n = 0: top)
    gist_ustk_entry *top(int fromTop = 0);

    gist_ustk_entry *pop();

    bool is_full();
    bool is_empty();
    bool is_root(int fromTop);
};

#endif
