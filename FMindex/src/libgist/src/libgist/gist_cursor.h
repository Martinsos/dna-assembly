//	gist_cursor.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist_cursor.h,v 1.1.1.1 1997/11/13 02:43:56 marcel Exp $


#ifndef GIST_CURSOR_H
#define GIST_CURSOR_H

#include "gist_ext.h"
#include "gist_defs.h"

class gist_lstk_entry {
public:
    enum eType { // type of stack entry
        eNode, // internal node
	eItem // leaf item
    };

    ~gist_lstk_entry();
    gist_lstk_entry &operator=(gist_lstk_entry &e);

    eType typ;
    union {
        struct {
	    shpid_t page;
	} node;
	struct {
	    void *key;
	    smsize_t keyLen;
	    void *dataPtr;
	    smsize_t dataPtrLen;
	    shpid_t page;
	    int idx;
	} item;
    } val;
};

class gist_lstk {
private:
    enum { // MSVC 5.0 doesn't allow "const int ..." here
        lstk_chunk = 50
    };

    int2 _top;
    gist_lstk_entry _stk[lstk_chunk];
    gist_lstk_entry *_indirect[lstk_chunk];
    void _push(const gist_lstk_entry *e);

public:	
    gist_lstk();
    ~gist_lstk();

    void push(shpid_t pid);

    // makes copies of key and data
    void push(void *key, smsize_t keyLen, void *dataPtr, smsize_t dataPtrLen,
        shpid_t page, int idx);

    // caller is reponsible for deallocating key and data 
    void pop(gist_lstk_entry &e);

    void empty_all();
    bool is_full();
    bool is_empty();
};

class gist_cursor_t {
public: 
    gist_cursor_t();
    ~gist_cursor_t();

    gist_lstk stack;
    const void *query;
    stid_t stid;
    gist_ext_t* ext;
};

#endif
