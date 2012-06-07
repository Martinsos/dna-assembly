//	gist_file.cc
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist_file.cc,v 1.2 1997/12/05 23:08:41 marcel Exp $

#ifdef __GNUG__
#pragma implementation "gist_file.h"
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#ifdef UNIX
#include <unistd.h>
#include <sys/errno.h>
#else
#include <io.h>
#include <errno.h>
#endif

#ifdef UNIX
#define O_BINARY 0
#endif

#include "gist_defs.h"
#include "gist_file.h"

static char magic[] = "Gist data file";

const int gist_file::invalidIdx = -1;

gist_file::gist_file() :
    isOpen(false), fileHandle(0), fileSize(0), htab(), freePg(0)
{
    // set up page descriptors
    buffers = x.buf;
    for (int i = 0; i < GISTBUFS; i++) {
	descrs[i].pageNo = 0;
	descrs[i].isDirty = false;
	descrs[i].pinCount = 0;
        descrs[i].page = &(buffers[i * SM_PAGESIZE]);
    }
}

gist_file::~gist_file()
{
    if (isOpen) close();
}

rc_t
gist_file::create(const char *filename)
{
    assert(!isOpen);
    fileHandle = ::open(filename, O_RDWR | O_BINARY);
    if (fileHandle >= 0) { // filename exists
        ::close(fileHandle);
        return (eFILEERROR);
    }

    fileHandle = ::open(filename, O_BINARY | O_RDWR | O_CREAT | O_TRUNC,
        S_IREAD | S_IWRITE);

    if (fileHandle < 0) {
        return (fileHandle);  // error: couldn't create
    }

    isOpen = true;

    /* Reserve page 0 */
    char page[SM_PAGESIZE];
    memset(page, 0, SM_PAGESIZE);
    memcpy(page, magic, sizeof(magic));
    write(fileHandle, page, SM_PAGESIZE);
    fileSize = 1;
    return(RCOK);
}

rc_t
gist_file::open(const char *filename)
{
    if (isOpen) {
	return (eFILEERROR);
    }

    fileHandle = ::open(filename, O_RDWR | O_BINARY);

    if (fileHandle < 0) {
        return (eFILEERROR); // error: couldn't open
    }

    // Verify that the magic words are there
    char page[SM_PAGESIZE];
    read(fileHandle, page, SM_PAGESIZE);
    if (memcmp(page, magic, sizeof(magic))) { 
        ::close(fileHandle);
        return(eFILEERROR); // error: magic words not found
    }

    // success!
    isOpen = true;
    fileSize = (lseek(fileHandle, 0, SEEK_END) + 1) / SM_PAGESIZE;
        // + 1: 0-based offset
    return(RCOK);
}

rc_t
gist_file::flush()
{
    if (!isOpen) {
        return (eFILEERROR);
    }

    for (int i = 0; i < GISTBUFS; i++) {
        if (descrs[i].isDirty) {
	    // write out dirty buffer
	    W_DO(_write_page(descrs[i].pageNo, descrs[i].page));
	    descrs[i].isDirty = false;
	}
    }
    return RCOK;
}

rc_t
gist_file::close()
{
    W_DO(flush());
    isOpen = false;
    return(::close(fileHandle));
}

int
gist_file::findFreeBuffer()
{
    int i;

    // free buffers have pin count = 0 (first look for virgin buffers)
    for (i = 0; i < GISTBUFS; i++) {
        if (descrs[i].pinCount == 0 && descrs[i].pageNo == 0) {
	    return i;
	}
    }
    for (i = 0; i < GISTBUFS; i++) {
        if (descrs[i].pinCount == 0) {
	    return i;
	}
    }
    return invalidIdx;
}

rc_t
gist_file::_write_page(shpid_t pageNo, char *page)
{
    int status = lseek(fileHandle, pageNo * SM_PAGESIZE, SEEK_SET);
    if (status < 0) return (eFILEERROR);
    status = write(fileHandle, page, SM_PAGESIZE);
    if (status < 0) return (eFILEERROR);
    return RCOK;
}

rc_t
gist_file::_read_page(shpid_t pageNo, char *page)
{
    int status = lseek(fileHandle, pageNo * SM_PAGESIZE, SEEK_SET);
    if (status < 0) return (eFILEERROR);
    status = read(fileHandle, page, SM_PAGESIZE);
    if (status < 0) return (eFILEERROR);
    return RCOK;
}

gist_file::page_descr *
gist_file::pinPage(shpid_t page)
{
    page_descr *descr;

    if (page >= fileSize) {
        return (NULL);
    }
    int index = htab[page];
    if (index == invalidIdx) {
        // this is not in the buffer pool; load it
	index = findFreeBuffer();
	if (index == invalidIdx) return (NULL);

	// prepare the descr
	descr = &descrs[index];
	// write out page if dirty
	if (descr->isDirty) { 
	    if (_write_page(descr->pageNo, descr->page) != RCOK) {
	        return NULL;
	    }
	}
	htab.remove(descr->pageNo); // mapping is now obsolete
	descr->pageNo = page;
	descr->isDirty = false;
	descr->pinCount = 1;

	// load the page
	if (_read_page(descr->pageNo, descr->page) != RCOK) {
	    return NULL;
	}
	// record the mapping
	htab.add(page, index);
    } else {
        descr = &descrs[index];
	descr->pinCount++;
    }
    return descr;
}

void
gist_file::unpinPage(page_descr *descr)
{
    assert(descr != NULL);
    descr->pinCount--;
}

void
gist_file::setDirty(page_descr *descr, bool isDirty)
{
    assert(descr != NULL);
    descr->isDirty = isDirty;
}

gist_file::page_descr *
gist_file::allocPage()
{
    shpid_t page;
    int index;
    page_descr *descr;

    if (!isOpen) return (NULL);
    page = findFreePage();
    index = findFreeBuffer();
    if (index == invalidIdx) return (NULL);
    descr = &descrs[index];
    // write out page if dirty
    if (descr->isDirty) { 
	if (_write_page(descr->pageNo, descr->page) != RCOK) {
	    return NULL;
	}
    }
    htab.remove(descr->pageNo); // mapping is now obsolete
    // set up descriptor
    descr->pageNo = page;
    descr->isDirty = false;
    descr->pinCount = 1;
    htab.add(page, index); // record new mapping
    (void) memset(descr->page, 0, SM_PAGESIZE); // create blank page
    fileSize++;

    return (descr);
}

rc_t
gist_file::freePage(page_descr *descr)
{
    W_DO(returnPage(descr->pageNo));

    descr->isDirty = false;
    descr->pinCount = 0;
    // don't reduce the fileSize;
    // it specifies the extent of the file, not the # of used pages
    return RCOK;
}


shpid_t 
gist_file::findFreePage()
{
    char buf[SM_PAGESIZE];
    shpid_t res;

    assert(isOpen);

    if (freePg != 0) {
	// Reclaim this page;
	// it contains a pointer to the next free page
	res = freePg;
	W_DO(_read_page(freePg, buf));
	memcpy(&freePg, buf+sizeof(magic), sizeof(shpid_t));
	W_DO(_write_page(0, buf));
	return res;
    } else {
	// add a page to the end of the file
	return(fileSize);
	//freePg = lseek(fileHandle, 0, SEEK_END) / SM_PAGESIZE;
	//memset(buf, 0, SM_PAGESIZE);
	//write(fileHandle, buf, SM_PAGESIZE);
    }
}

rc_t
gist_file::returnPage(shpid_t page)
{
    char buf[SM_PAGESIZE];
    shpid_t temp;

    // error checking: page must be valid and file must be open
    assert(isOpen);

    // Get the old head of the list
    W_DO(_read_page(0, buf));
    memcpy(&temp, buf+sizeof(magic), sizeof(shpid_t));

    // Write the new head of the list and update state
    memcpy(buf+sizeof(magic), &page, sizeof(shpid_t));
    W_DO(_write_page(0, buf));
    freePg = page;

    // In our new head, put link to old head
    memcpy(buf+sizeof(magic), &temp, sizeof(shpid_t));
    W_DO(_write_page(page, buf));

    return RCOK;
}
