//	gist_file.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist_file.h,v 1.2 1997/12/05 23:08:43 marcel Exp $


#ifndef GIST_FILE_H
#define GIST_FILE_H

#ifdef __GNUG__
#pragma interface
#endif

#include "gist_htab.h"

class gist_file {

public:
    

    gist_file();
    ~gist_file();

    rc_t create(const char *filename);
    // creates an 'empty' file (except for header info) with given name

    rc_t open(const char *filename);
    // open this file as a Gist file for subsequent operations

    rc_t flush(); 
    // write out all dirtied pages

    rc_t close();
    // flush and empty buffers

    int size() { return fileSize; }

    static const int invalidIdx;

    struct page_descr {
        shpid_t pageNo; // pageno on disk
	char *page; // pointer to in-mem page
	bool isDirty; // true if modified since pinned
	int pinCount; 
    };

    page_descr *pinPage(shpid_t page);
    // bring the page into memory, if not already present,
    // and return its descriptor

    void unpinPage(page_descr *descr);
    // decrement the page's pincount and toss it out if = 0

    page_descr *allocPage();
    // find a fresh page on disk, reserve it, allocate a buffer
    // for it and return that buffer's descriptor

    rc_t freePage(page_descr *descr);
    // dealloc that page; the page descriptor shouldn't be used anymore

    void setDirty(page_descr *descr, bool isDirty);
    // change that page's status

protected:

    bool isOpen; // true if file has been opened
    int fileHandle;
    shpid_t fileSize; // in pages
    gist_htab htab; // maps page no to index into buffer pool
    shpid_t freePg;

    // each file has its own buffer pool;
    // make sure the buffers are aligned on 8-byte boundaries
    page_descr descrs[GISTBUFS]; // descriptors
    union {
        double d;
	char buf[GISTBUFS * SM_PAGESIZE]; // buffers
    } x;
    char *buffers;

    rc_t _write_page(shpid_t pageNo, char *page);
    rc_t _read_page(shpid_t pageNo, char *page);
    int findFreeBuffer();
    shpid_t findFreePage();
    rc_t returnPage(shpid_t page);

};

#endif
