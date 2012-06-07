//	gist.cc
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist.cc,v 1.2 1997/12/05 23:08:35 marcel Exp $

#ifndef LIBGIST
#define SM_SOURCE
#define GIST_C
#endif

#ifdef __GNUG__
#pragma implementation "gist.h"
#pragma implementation "gist_cursor.h"
#pragma implementation "gist_ustk.h"
#endif

#include <math.h>
#include <stdlib.h>
#ifdef LIBGIST
#include <stdio.h>
#else 
#include <sm_int_2.h>
#include <crash.h>
#endif

#include "gist.h"
#include "gist_ext.h"

#ifndef WIN32
#include <values.h> // for MAXDOUBLE
#else 
#define MAXDOUBLE 1.79769313486231570e+308 // taken from values.h on a Solaris system
#endif

// didn't know where else to put these
const double gist_penalty_t::max_penalty = MAXDOUBLE;
gist_ext_t* gist_ext_t::gist_ext_list[gist_ext_t::gist_numext];

/* 
 * gist_m
 */

#ifdef LIBGIST
gist_m::gist_m() : ext(NULL), isOpen(false), file()
{
}

gist_m::~gist_m()
{
    close();
}
#endif

rc_t
gist_m::_new_page(
    const lpid_t& 	root,
    const lpid_t& 	closeby,
    gist_p&		p,
    int2 		level)
{
#ifndef LIBGIST
    w_assert3(closeby);
    lpid_t pid;
    W_DO(io->alloc_pages(root.stid(), closeby, 1, &pid));
    W_DO(p.fix(pid, LATCH_EX, p.t_virgin));

    gistctrl_t hdr;
    hdr.root = root;
    hdr.level = level;
    //W_DO(p.set_hdr(hdr));
    W_DO(p.format(pid, &hdr));
    return RCOK;
#else
    p.descr = file.allocPage();
    w_assert3(p.descr);
    p._pp = (page_s *) p.descr->page;

    gistctrl_t hdr;
    hdr.root = root;
    hdr.level = level;
    W_DO(p.format(lpid_t(0, 0, p.descr->pageNo), &hdr));
    return RCOK;
#endif
}

#ifdef LIBGIST
bool 
gist_m::is_empty()
{
    lpid_t root(0, 0, rootNo);
#else
bool
gist_m::is_empty(
    const lpid_t& 	root)
{
#endif
    gist_p page;
    W_DO(_fix_page(page, root, LATCH_SH));
    return (page.nrecs() == 0);
}

#ifdef LIBGIST
rc_t
gist_m::open(
    const char*		filename,
    gist_ext_t*		extension)
{
    ext = extension;
    W_DO(file.open(filename));
    isOpen = true;
    return RCOK;
}
#endif

#ifdef LIBGIST
rc_t
gist_m::close()
{
    W_DO(file.close());
    isOpen = false;
    return RCOK;
}
#endif

#ifdef LIBGIST
rc_t
gist_m::flush()
{
    W_DO(file.flush());
    return RCOK;
}
#endif

#ifdef LIBGIST
rc_t
gist_m::create(
    const char*		filename,
    gist_ext_t*		extension)
{
    ext = extension;
    W_DO(file.create(filename));
    isOpen = true;

    // create the root page
    gist_p root;
    lpid_t rootPid(0, 0, rootNo);
    lpid_t noPid(0, 0, 0);
    _new_page(rootPid, noPid, root, 1); // this is a leaf
    w_assert3(rootNo == root.pid().page);
    _unfix_page(root);
    W_DO(file.flush());
    return RCOK;
}
#else 
rc_t
gist_m::create(
    stid_t		stid,
    lpid_t&		root)
{
    lsn_t anchor;
    xct_t* xd = xct();
    w_assert3(xd);
    anchor = xd->anchor();

    X_DO(io->alloc_pages(stid, lpid_t::eof, 1, &root), anchor);

    gist_p page;
    X_DO(page.fix(root, LATCH_EX, page.t_virgin), anchor);

    gistctrl_t hdr;
    hdr.root = root;
    hdr.level = 1;
    X_DO(page.set_hdr(hdr), anchor);

    SSMTEST("gist.1");
    xd->compensate(anchor);

    return RCOK;
}
#endif

rc_t
gist_m::_locate_leaf(
    const lpid_t& 	root, // root of index
    gist_ext_t* 	ext, // extension function table
    gist_ustk&		stack, // out: path to leaf
    const vec_t&	key, // key to insert
    const vec_t&	data) // data to insert
{
    lpid_t currPid = root;
    gist_p page;
    int index;

    for (;;) {
	W_DO(_fix_page(page, currPid, LATCH_EX));

	if (!page.is_leaf()) {
	    ext->findMinPen(page, key, data, index);
	    stack.push(page, index);
	} else {
	    // we found a leaf
	    stack.push(page, 0);
	    return (RCOK); // leave the leaf latched
	}

	// follow child pointer
	currPid.page = page.rec(index).child();
	// no page.unfix(); we leave the pages fixed for now
    }
}

rc_t
gist_m::_insert_leaf(
    gist_ext_t* 	ext, // extension function table
    gist_p&		page, // leaf to insert on
    const vec_t&	key, // key of new entry
    const vec_t&	data, // data of new entry
    bool&		bpChanged) // out: true if the BP changed
{
    int cnt = page.nrecs();

    // make sure that at least enough additional space needed
    // for the BP is available
    if (!page.is_root()) {
        // compute additional space for BP: 
	// first extract and update the BP
	const keyrec_t &bpTup = page.rec(gist_p::bpSlot);
	w_assert3(bpTup.klen() <= gist_p::max_tup_sz);
	unsigned int oldLen = bpTup.klen();
	AlignedPred y;
	char *newBp = y.pred; // force 8-byte alignment
	(void) memcpy(newBp, bpTup.key(), bpTup.klen()); // bp will fit
	vec_t bpv(newBp, bpTup.klen());
	ext->unionKey(bpv, page.level(), key, bpChanged);
	w_assert3(bpv.len(0) <= gist_p::max_tup_sz);
	w_assert3(bpChanged || oldLen == bpv.len(0));

	// compute the additional space
	unsigned int total = align(bpv.len(0) + sizeof(keyrec_t)) -
	    align(page.rec_size(gist_p::bpSlot));
	if (total > page.usable_space()) return RC(eRECWONTFIT); // can't even fit new BP

	// extract and update bp
	if (bpChanged) {
	    // write bp back
#ifndef LIBGIST
            // all structure modifications are executed as atomic actions
	    lsn_t anchor;
	    xct_t* xd = xct();
	    w_assert3(xd);
	    if (xd) {
	        anchor = xd->anchor();
	    }
#endif
	    X_DO(page.remove(gist_p::bpSlot), anchor);
	    X_DO(page.insert(bpv, cvec_t(), gist_p::bpSlot, 0), anchor);
#ifndef LIBGIST
            // end atomic action
	    if (xd) {
	        SSMTEST("gist.2");
		xd->compensate(anchor);
	    }
#endif
	}
    } else {
	// the root doesnt have a BP
	bpChanged = false;
    }

    // insert new item 
    rc_t status;
    { 
#ifndef LIBGIST
	// switch logging off for leaf data insertion; this is logged logically later on
	xct_log_switch_t log_off(OFF);
#endif
	status = ext->insert(page, key, data, 0);
    }
    return (status);
}

rc_t
gist_m::_apply_update(
    gist_ext_t*		ext,
    gist_p&		page, // parent to update
    int& 		leftIdx, // in/out: slot index of entry for orig. child page
    const vec_t&	leftBp, // BP of orig. child page
    const vec_t&	rightBp, // BP of new child (or NULL if orig. child not split)
    shpid_t 		rightChild, // pointer to new child
    bool&		bpChanged) // out: true if the BP changed
{
    // make sure at least the additional space needed for the BP update
    // is available
    if (!page.is_root()) {
        // compute additional space for BP: 

	// first extract and update the BP
	const keyrec_t &bpTup = page.rec(gist_p::bpSlot);
	w_assert3(bpTup.klen() <= gist_p::max_tup_sz);
	unsigned int oldLen = bpTup.klen();
	AlignedPred y;
	char *newBp = y.pred; // force 8-byte alignment
	(void) memcpy(newBp, bpTup.key(), bpTup.klen());
	vec_t bpv(newBp, bpTup.klen());
	ext->unionKey(bpv, page.level(), leftBp, bpChanged);
	w_assert3(bpv.len(0) <= gist_p::max_tup_sz);
	w_assert3(bpChanged || oldLen == bpv.len(0));
	if (rightBp.size() != 0 ) {
	    bool changedAgain;
	    ext->unionKey(bpv, page.level(), rightBp, changedAgain);
	    w_assert3((int) bpv.len(0) <= gist_p::max_tup_sz);
	    w_assert3(changedAgain || oldLen == bpv.len(0));
	    bpChanged = bpChanged || changedAgain;
	}

	// compute the additional space
	unsigned int total = align(bpv.len(0) + sizeof(keyrec_t)) - 
	    align(page.rec_size(gist_p::bpSlot));

	if (total > page.usable_space()) return RC(eRECWONTFIT);

	if (bpChanged) {
	    // write bp back
	    W_DO(page.remove(gist_p::bpSlot));
	    rc_t status = page.insert(bpv, cvec_t(), gist_p::bpSlot, 0);
	    w_assert3(_err_num(status) != eRECWONTFIT);
	    if (status) return RC_AUGMENT(status);
	}
    } else {
        bpChanged = false;
    }

    // update the entry for the original page
    W_DO(ext->updateKey(page, leftIdx, leftBp));

    if (rightBp.size() != 0) {
	// insert the entry for the new right sibling
	return ext->insert(page, rightBp, vec_t(), rightChild);
    }

    return RCOK;
}

rc_t
gist_m::_split(
    const lpid_t& 	root,
    gist_ext_t*		ext,
    gist_p&		page, // in/out: page to split
    gist_ustk&		stack, // records ancestors
    int 		stkIdx, // our pos on stack
    int 		rightEntries[], // slot indices of entries for the right sibling
    int 		numRight, // how many right entries
    gist_p&		rightChild, // out: new right sibling
    vec_t&		leftBp, // BP for page
    vec_t&		rightBp) // BP for rightChild
{
    if (stack.is_root(stkIdx)) {
        // do root split
	gist_p leftChild;

	// both children will be at the level the root is at now
	W_DO(_new_page(root, page.pid(), leftChild, page.level()));
	W_DO(_new_page(root, page.pid(), rightChild, page.level()));

	// copy all entries on root into leftChild
	W_DO(page.copy(leftChild));

	// create a blank root page
	gistctrl_t hdr;
	hdr.root = root;
	hdr.level = page.level() + 1; // added one level
	W_DO(page.format(root, &hdr));

	// split the left child
	W_DO(_split_node(ext, leftChild, rightChild, rightEntries, numRight, false, leftBp,
	    rightBp));

	// add the two new entries to the otherwise empty root
	W_DO(ext->insert(page, leftBp, vec_t(), leftChild.pid().page));
	W_DO(ext->insert(page, rightBp, vec_t(), rightChild.pid().page));

	// unfix root and return left child as split page
	// we neednt fix the stack, because _update_parent will not be called
	_unfix_page(page);
	page = leftChild;
    } else {
        // do regular split
	W_DO(_new_page(root, page.pid(), rightChild, page.level()));
	W_DO(_split_node(ext, page, rightChild, rightEntries, numRight, true, leftBp, rightBp));
	W_DO(_update_parent(root, ext, stack, stkIdx+1, leftBp, rightBp,
	    rightChild.pid().page));
    }

    return RCOK;
}

rc_t
gist_m::_split_node(
    gist_ext_t*		ext,
    gist_p&		left, // page to split (remove entries from)
    gist_p&		right, // page to add entries to
    int 		rightEntries[], // slot indices of entries to be moved right
    int 		numRight, // number of entries to move right
    bool 		leftHasBp, // false if left is copy of root (doesnt have BP)
    cvec_t&		leftBp, // BP of page to split
    cvec_t&		rightBp) // BP of right sibling
{
    // first, move entries to new right sibling
    W_DO(right.insert(rightBp, cvec_t(), gist_p::bpSlot, 0));
        // we promised there'd be a BP
    int correction = (leftHasBp ? 0 : 1);
    AlignedPred x, y;
    char *data = y.pred;
    vec_t keyv;
    vec_t datav;

    // if the left page doesnt have a BP (and its not a root, either), we have to 
    // compensate for the automatic adjustment of the slot index done in nrecs(),
    // rec() etc. by subtracting 1
    if (!leftHasBp) {
        for (int i = 0; i < numRight; i++) {
	    rightEntries[i]--;
	}
    }

    // distribute the entries;
    for (int i = 0; i < numRight; i++) {
	int idx = rightEntries[i];
	if (idx < left.nrecs()) {
	    // item exists on page; copy it
	    keyv.set(x.pred, gist_p::max_tup_sz);
	    ext->getKey(left, idx, keyv);
	    (void) memcpy(data, left.rec(idx).elem(), left.rec(idx).elen());
	    datav.set(data, left.rec(idx).elen());
	    W_DO(ext->insert(right, keyv, datav, left.rec(idx).child()));
	}
    }

    // Clean up the original page: remove the moved entries and replace the BP.
    // First, remove the positions of new node entries from rightEntries.
    if (rightEntries[numRight-1] >= left.nrecs()) numRight--;
    if (rightEntries[numRight-1] >= left.nrecs()) numRight--;
    W_DO(ext->remove(left, rightEntries, numRight));
    if (leftHasBp) {
	W_DO(left.remove(gist_p::bpSlot));
    }
    W_DO(left.insert(leftBp, cvec_t(), gist_p::bpSlot, 0));

    return RCOK;
}

static int
intCmp(
    const void*		a,
    const void*		b)
{
    return *((int *) a) - *((int *) b);
}

#ifdef LIBGIST
rc_t
gist_m::insert(
    const void*		key, 
    const int	    	keyLen,
    const void*		dataPtr,
    const int	    	dataPtrLen)
{
    lpid_t root(0, 0, rootNo);
    vec_t keyv(key, keyLen);
    vec_t datav(dataPtr, dataPtrLen);

#else // Shore code

rc_t
gist_m::insert(
    const lpid_t& 	    root,
    gist_ext_t*		    ext,
    const vec_t& 	    keyv, 
    const vec_t& 	    datav)
{

#endif

    gist_ustk stack;
    bool wasSplit = false; // true if leaf is split

    W_DO(_locate_leaf(root, ext, stack, keyv, datav));
    gist_p &leaf = stack.top()->page;

    // insert item/update BP
    bool bpChanged = false;
    rc_t status = _insert_leaf(ext, leaf, keyv, datav, bpChanged);
    if (status && _err_num(status) != eRECWONTFIT) {
        return RC_AUGMENT(status);
    }

    if (_err_num(status) == eRECWONTFIT) {// something didnt fit; we must split the page

	// find out how to split
	int rightEntries[gist_p::max_scnt];
	int numRight = gist_p::max_scnt;
	AlignedPred x, y;
	vec_t leftv(x.pred, gist_p::max_tup_sz);
	vec_t rightv(y.pred, gist_p::max_tup_sz);
	vec_t bpv;
	if (!leaf.is_root()) {
	    // set up a pointer to the old BP
	    bpv.set(leaf.rec(gist_p::bpSlot).key(), leaf.rec(gist_p::bpSlot).klen());
	}
	bool newGoesRight, dummyBool;
	W_DO(ext->pickSplit(leaf, rightEntries, numRight, bpv, leftv, rightv, vec_t(keyv, datav),
	    newGoesRight, vec_t(), dummyBool));
	w_assert3(leftv.len(0) <= gist_p::max_tup_sz);
	w_assert3(rightv.len(0) <= gist_p::max_tup_sz);
	w_assert3(numRight <= gist_p::max_scnt);

#ifndef LIBGIST
        // perform split as atomic action, so we don't need to lock the index until EOT;
	lsn_t anchor;
	xct_t* xd = xct();
	w_assert3(xd);
	if (xd) {
	    anchor = xd->anchor();
	}
#endif

        // do the split
	gist_p rightSib;
	X_DO(_split(root, ext, leaf, stack, 0, rightEntries, numRight, rightSib, leftv,
	    rightv), anchor);
	wasSplit = true;

#ifndef LIBGIST
        // end the atomic action here
	if (xd) {
	    SSMTEST("gist.3");
	    xd->compensate(anchor);
	}
#endif

	// try again; the bp is already updated due to the split
	gist_p &p = newGoesRight ? rightSib : leaf;
	{
#ifndef LIBGIST
	    // we don't log this physically; a logical log record is written a little later
	    xct_log_switch_t log_off(OFF);
#endif
	    W_DO(ext->insert(p, keyv, datav, 0));
	}
#ifndef LIBGIST
	W_DO(log_gist_insert(leaf, ext, keyv, datav));
#endif
	_unfix_page(leaf);
	_unfix_page(rightSib);
#ifndef LIBGIST
    } else {
	// the insertion was successful, we should log this
	W_DO(log_gist_insert(leaf, ext, keyv, datav));
#endif
    }

    if (bpChanged && !wasSplit) {
	vec_t bpv(leaf.rec(gist_p::bpSlot).key(), leaf.rec(gist_p::bpSlot).klen());
#ifndef LIBGIST
        // all structure modifications are performed as atomic actions
	lsn_t anchor;
	xct_t* xd = xct();
	w_assert3(xd);
	if (xd) {
	    anchor = xd->anchor();
	}
#endif
        X_DO(_update_parent(root, ext, stack, 1, bpv, vec_t(), 0), anchor);
#ifndef LIBGIST
        // end atomic action here
	if (xd) {
	    SSMTEST("gist.4");
	    xd->compensate(anchor);
	}
#endif
	_unfix_page(leaf);
    }

    // unfix pages that haven't been unfixed before in calls to _split
    // and _update_parent
    while (!stack.is_empty()) {
        gist_ustk_entry *e = stack.pop();
	if (e->page.is_fixed()) {
	    _unfix_page(e->page);
	}
    }

    return RCOK;
}

rc_t
gist_m::_update_parent(
    const lpid_t& 	root,
    gist_ext_t*		ext,
    gist_ustk&		stack, // records ancestors
    int 		stkIdx, // parent level on stack
    const vec_t&	leftPred, // BP of orig. child page
    const vec_t&	rightPred, // BP of child's new sibling
    shpid_t 		rightChild) // pointer to child's new sibling
{
    gist_p &page = stack.top(stkIdx)->page;

    // save the pointer to the original child page
    const shpid_t childPtr = page.rec(stack.top(stkIdx)->idx).child();

    // apply the updates to the parent page (update bp and left item's pred, insert new item)
    bool bpChanged;
    rc_t status = _apply_update(ext, page, stack.top(stkIdx)->idx, leftPred, rightPred,
        rightChild, bpChanged);
    if (_err_num(status) != eRECWONTFIT) {
	// either we're done or there was some error;
	// in any case, we're finished with this node
	if (!status && bpChanged) {
	    vec_t bpv(page.rec(gist_p::bpSlot).key(), page.rec(gist_p::bpSlot).klen());
	    W_DO(_update_parent(root, ext, stack, stkIdx+1, bpv, vec_t(), 0));
	}
	_unfix_page(page);
        return RC_AUGMENT(status);
    }

    // something didnt fit, we have to split the page

    // we don't need the entry for the original child anymore
    W_DO(ext->remove(page, &(stack.top(stkIdx)->idx), 1));

    // get split info
    int rightEntries[gist_p::max_scnt];
    int numRight;
    AlignedPred x, y;
    vec_t leftv(x.pred, gist_p::max_tup_sz);
    vec_t rightv(y.pred, gist_p::max_tup_sz);
    vec_t bpv;
    bool leftGoesRight;
    bool rightGoesRight;
    if (!page.is_root()) {
	bpv.set(page.rec(gist_p::bpSlot).key(), page.rec(gist_p::bpSlot).klen());
    }
    W_DO(ext->pickSplit(page, rightEntries, numRight, bpv, leftv, rightv, leftPred,
        leftGoesRight, rightPred, rightGoesRight));
    w_assert3(leftv.len(0) <= gist_p::max_tup_sz);
    w_assert3(rightv.len(0) <= gist_p::max_tup_sz);
    w_assert3(numRight <= gist_p::max_scnt);

    // do the split
    gist_p rightSib;
    W_DO(_split(root, ext, page, stack, stkIdx, rightEntries, numRight, rightSib, leftv,
        rightv));

    // insert left and right items
    gist_p &p = leftGoesRight ? rightSib : page;
    W_DO(ext->insert(p, leftPred, vec_t(), childPtr));
    p = rightGoesRight ? rightSib : page;
    W_DO(ext->insert(p, rightPred, vec_t(), rightChild));
    _unfix_page(page);
    _unfix_page(rightSib);
    return RCOK;
}

#ifdef LIBGIST
rc_t
gist_m::remove(const void *query)
{
#else
rc_t
gist_m::remove(
    const lpid_t&	root,
    gist_ext_t*		ext,
    const vec_t& 	keyv,  
    const vec_t& 	elem)
{
    // construct a query for the given key
    void* query = ext->eqQuery(keyv);
#endif
    gist_cursor_t cursor;
    gist_p page;
    bool eof = false;
    int idx;
    shpid_t prevLeaf = 0, leaf; // assumes: 0 is not a valid page no
    AlignedPred x, y;
    char *key = x.pred, *data = y.pred;
    smsize_t klen = gist_p::max_tup_sz, dlen = gist_p::max_tup_sz;
    int matches[gist_p::max_scnt];
    int numMatches = 0;

    // run a query to find matching items
#ifdef LIBGIST
    W_DO(fetch_init(cursor, query));
#else 
    W_DO(fetch_init(root, ext, cursor, query));
#endif
    for (;;) {
        W_DO(_fetch(cursor, key, klen, data, dlen, leaf, idx, eof));
	if (eof) {
	    // delete the stuff on the last page we visited
	    if (page.is_fixed()) {
		{ 
#ifndef LIBGIST
                   // this is logged logically a little later
		   xct_log_switch_t log_off(OFF);
#endif
		    W_DO(ext->remove(page, matches, numMatches));
		}
#ifndef LIBGIST
		// writing logical log record
		W_DO(log_gist_remove(page, matches[0], ext, keyv, elem));
#endif
		_unfix_page(page);
	    }
#ifndef LIBGIST
	    free(query);
#endif
	    return RCOK;
	}
	if (leaf != prevLeaf) {
	    // we must switch to a new leaf node
	    prevLeaf = leaf;
	    if (page.is_fixed()) {
		// do the deletions
		W_DO(ext->remove(page, matches, numMatches));
	        _unfix_page(page);
	    }
#ifdef LIBGIST
	    W_DO(_fix_page(page, lpid_t(0, 0, leaf), LATCH_EX));
#else 
	    W_DO(_fix_page(page, lpid_t(root.stid(), leaf), LATCH_EX));
#endif
	    numMatches = 0;
	}
#ifdef LIBGIST
	// we remove all leaf entries that match the query
	matches[numMatches] = idx;
	numMatches++;
#else
	// Shore:
	// we only remove those leaf entries that match the query and contain the 
	// same data ptr (or the parameter data ptr is empty)
	cvec_t dv(data, dlen);
	if (elem.size() == 0 || elem == dv) {
	    matches[numMatches] = idx;
	    numMatches++;
	}
#endif
    }
}

#ifdef LIBGIST
rc_t
gist_m::fetch_init(
    gist_cursor_t&	cursor,
    const void*		query)
{
    lpid_t root(0, 0, rootNo);
    cursor.ext = ext;
#else 
rc_t
gist_m::fetch_init(
    const lpid_t& 	root,
    gist_ext_t*		extension,
    gist_cursor_t& 	cursor,
    const void*		query)
{
    cursor.ext = extension;
#endif
    cursor.stid = root.stid();
    cursor.query = query;
    // push root page on the fetch stack
    if (!cursor.stack.is_empty()) {
        cursor.stack.empty_all();
    }
    cursor.stack.push(root.page);

    return RCOK;
}

rc_t
gist_m::_fix_page(
    gist_p&		page,
    lpid_t 		pageNo,
    latch_mode_t	mode)
{
#ifdef LIBGIST
    page.descr = file.pinPage(pageNo.page);
    if (page.descr == NULL) {
        return eOUTOFSPACE;
    }
    page._pp = (page_s *) page.descr->page;
    return RCOK;
#else // Shore code
    W_DO(page.fix(pageNo, mode));
    return RCOK;
#endif
}

void
gist_m::_unfix_page(
    gist_p&		page)
{
#ifdef LIBGIST
    file.unpinPage(page.descr);
    page._pp = NULL;
#else // Shore code
    page.unfix();
#endif
}

#ifdef LIBGIST
rc_t
gist_m::fetch(
    gist_cursor_t&	cursor,
    void*		key,
    smsize_t&		klen,
    void*		el,
    smsize_t&		elen,
    bool&		eof)
#else // Shore code
rc_t
gist_m::fetch(
    gist_cursor_t& 	cursor,
    void* 	   	key,
    smsize_t&		klen,
    void* 		el,
    smsize_t&		elen, 
    bool& 	    	eof)
#endif
{
    shpid_t dummyPage;
    int dummyIdx;

    return _fetch(cursor, key, klen, el, elen, dummyPage, dummyIdx, eof);
}

rc_t
gist_m::_fetch(
    gist_cursor_t&	cursor,
    void*		key,
    smsize_t&		keyLen,
    void*		dataPtr,
    smsize_t&		dataPtrLen,
    shpid_t&		leafNo,
    int&		idx,
    bool&		eof)
{
    int matches[gist_p::max_scnt];
    int numMatches;
    AlignedPred x;
    char *k = x.pred; // for 8-byte alignment
    eof = false;

    while (!cursor.stack.is_empty()) {
	gist_lstk_entry e;
	cursor.stack.pop(e);

        if (e.typ == gist_lstk_entry::eItem) {
	    // this is a leaf entry 
	    if (keyLen < e.val.item.keyLen || dataPtrLen < e.val.item.dataPtrLen) {
		// not enough space to copy the items
		delete e.val.item.key;
		delete e.val.item.dataPtr;
	        return RC(eRECWONTFIT);
	    }

            //copy key and data ptr
	    (void) memcpy(key, e.val.item.key, e.val.item.keyLen);
	    keyLen = e.val.item.keyLen;
	    (void) memcpy(dataPtr, e.val.item.dataPtr, e.val.item.dataPtrLen);
	    dataPtrLen = e.val.item.dataPtrLen;
	    leafNo = e.val.item.page;
	    idx = e.val.item.idx;
	    delete e.val.item.key;
	    delete e.val.item.dataPtr;
            return (RCOK);
        }

        // this is a node-type entry
        // fix the page, process it, then unfix it
	gist_p page;
	AlignedPred x;
	vec_t keyv;
	W_DO(_fix_page(page, lpid_t(cursor.stid, e.val.node.page), LATCH_SH));
	cursor.ext->search(page, cursor.query, matches, numMatches);
	// process the matches in reverse, otherwise B-trees won't work
	// (semi-disgusting hack: sorting order is not part of the Gist framework)
	for (int i = numMatches-1; i >= 0; i--) {
	    const keyrec_t& tup = page.rec(matches[i]);
	    if (page.is_leaf()) {
		keyv.set(x.pred, gist_p::max_tup_sz);
		cursor.ext->getKey(page, matches[i], keyv);
		cursor.stack.push((void *) keyv.ptr(0), keyv.len(0), (void *) tup.elem(),
		    tup.elen(), page.pid().page, matches[i]);
	    } else {
	        cursor.stack.push(tup.child());
	    }
	    
	}
	_unfix_page(page);
    }

    eof = true;
    return(RCOK);
}

#ifdef LIBGIST
void
gist_m::_check_node(gist_p &node)
{
    int recCnt = node.nrecs();
    const keyrec_t &bpTup = node.rec(gist_p::bpSlot);
    vec_t bpv(bpTup.key(), bpTup.klen());
    AlignedPred x;
    vec_t predv;

    for (int i = 0; i < recCnt; i++) {
	const keyrec_t &tup = node.rec(i);
	predv.set(x.pred, gist_p::max_tup_sz);
	ext->getKey(node, i, predv);
	if (!ext->check(bpv, predv, node.level())) {
	    fprintf(stderr, "node %d: pred %d not contained in BP\n",
		node.pid().page, i);
	}
    }
}

rc_t
gist_m::_check_sub(
    shpid_t 		pgno,
    const vec_t&	pred,
    shpid_t		parent,
    int			slot,
    reachInfo 		reached[])
{
    gist_p page;
    W_DO(_fix_page(page, lpid_t(0, 0, pgno), LATCH_SH));

    // check BP
    if (pred.size() != 0) {
	// compare the parent entry (pred) with our BP
	w_assert3(!page.is_root());
	const keyrec_t &bpTup = page.rec(gist_p::bpSlot);
	if (bpTup.klen() != pred.len(0)) {
	    // lengths don't match, this doesn't sound right
	    fprintf(stderr,
		"node %d: BP has different length (%d) than parent (%d)\n",
		pgno, (int) bpTup.klen(), (int) pred.len(0));
	} else {
	    // let's compare them
	    if (memcmp(bpTup.key(), pred.ptr(0), pred.len(0)) != 0) {
		fprintf(stderr,
		    "node %d: BP doesn't match parent entry (in %d, %d)\n",
		    pgno, parent, slot);
	    }
	}

	// make sure that the BP contains all other predicates
	_check_node(page);
    }

    if (page.level() > 1) {
	// keep track of nodes reached
	int recCnt = page.nrecs();
	for (int i = 0; i < recCnt; i++) {
	    const keyrec_t &tup = page.rec(i);
	    shpid_t childPtr = tup.child();
	    if (reached[childPtr].count > 0) {
	        // someone else is pointing to this already
		fprintf(stderr, "additional reference to %d from %d, slot %d\n",
		    childPtr, pgno, i);
	    } else {
	        reached[childPtr].parent = pgno; // that's us
	    }
	    reached[childPtr].count++;
	    W_DO(_check_sub(tup.child(), vec_t(tup.key(), tup.klen()), pgno, i, reached));
	}
    }

    _unfix_page(page);
    return RCOK;
}

rc_t
gist_m::check()
{
    w_assert3(isOpen);
    int i;

    int nodeCnt = file.size();
    reachInfo *reached = new reachInfo[nodeCnt];
    for (i = 0; i < nodeCnt; i++) {
	reached[i].count = 0;
	reached[i].parent = 0;
    }

    reached[rootNo].count = 1;
    reached[rootNo].parent = 0;
    W_DO(_check_sub(rootNo, vec_t(), 0, 0, reached));

    for (i = 1; i < nodeCnt; i++) {
	if (reached[i].count != 1) {
	    // something's fishy
	    fprintf(stderr, "node %d referenced %d times (orig. parent: %d)\n",
	    i, reached[i].count, reached[i].parent);
	}
    }
    return RCOK;
}
#endif

void
gist_m::_dump_pg(
    gist_ext_t*		ext,
    gist_p&		page)
{
    int lvl = page.level();
    // dump header
    printf("[%d] level: %d  #slots: %d  avail: %d\n",
        page.pid().page, lvl, page.nrecs(), (int) page.usable_space());
    if (!page.is_root()) {
        // dump BP
	printf("\tbp: <");
	const keyrec_t &bp = page.rec(gist_p::bpSlot);
	vec_t bpv(bp.key(), bp.klen());
	ext->printPred(bpv, lvl+1); // really a parent pred
	printf(">\n");
    }
    // dump content
    int recCnt = page.nrecs();
    AlignedPred x;
    vec_t keyv;
    for (int i = 0; i < recCnt; i++) {
        const keyrec_t &tup = page.rec(i);
	printf("\t[%d] <", i);
	keyv.set(x.pred, gist_p::max_tup_sz);
	ext->getKey(page, i, keyv);
	ext->printPred(keyv, lvl);
	printf("> len: %d ", (int) tup.klen());
	if (lvl == 1) {
	    printf(" data: ");
	    ext->printData(vec_t(tup.elem(), tup.elen()));
	    printf(" len: %d\n", (int) tup.elen());
	} else {
	    printf(" child: %d\n", tup.child());
	}
    }
}

rc_t
gist_m::_dump_sub(
    gist_ext_t*		ext,
    lpid_t 		pgno)
{
    gist_p page;

    W_DO(_fix_page(page, pgno, LATCH_SH));
    _dump_pg(ext, page);
    int recCnt = page.nrecs();
    if (page.level() > 1) {
	for (int i = 0; i < recCnt; i++) {
	    const keyrec_t &tup = page.rec(i);
	    W_DO(_dump_sub(ext, lpid_t(pgno.stid(), tup.child())));
	}
    }
    _unfix_page(page);
    return RCOK;
}

#ifdef LIBGIST
rc_t
gist_m::dump(
    shpid_t 		pgno)
{
    if (pgno != 0) {
        gist_p page;
	W_DO(_fix_page(page, lpid_t(0, 0, pgno), LATCH_SH));
	_dump_pg(ext, page);
	_unfix_page(page);
    } else {
        W_DO(_dump_sub(ext, lpid_t(0, 0, rootNo)));
    }
    return RCOK;
}
#else // Shore code
rc_t
gist_m::print(
    const lpid_t&	root,
    gist_ext_t*		ext)
{
    W_DO(_dump_sub(ext, root));
    return RCOK;
}
#endif // ifdef LIBGIST


/*
 * gist_ustk
 */

gist_ustk::~gist_ustk()
{
}

void 
gist_ustk::push(
    const gist_p&	page,
    int2 		idx)
{
    w_assert3(_top < max_ustk_sz);
    _stack[_top].page = page;
    _stack[_top].idx = idx;
    _top++;
}

gist_ustk_entry *
gist_ustk::top(
    int 		fromTop)
{
    w_assert3(_top - fromTop > 0);
    return (&_stack[_top-fromTop-1]);
}

gist_ustk_entry *
gist_ustk::pop()
{
    w_assert3(_top > 0);
    _top--;
    return (&_stack[_top]);
}

bool
gist_ustk::is_full()
{
    return _top >= max_ustk_sz;
}

bool
gist_ustk::is_empty()
{
    return _top == 0;
}

bool
gist_ustk::is_root(int fromTop)
{
    return fromTop == _top - 1;
}


/* 
 * gist_lstk
 */

bool
gist_lstk::is_full()
{
    return _top >= lstk_chunk+lstk_chunk*lstk_chunk;
}

bool
gist_lstk::is_empty()
{
    return _top == 0;
}

gist_lstk::gist_lstk()
{
    _top = 0;
    for (int i = 0; i < lstk_chunk; i++) {
	_indirect[i] = NULL;
    }
}

gist_lstk_entry::~gist_lstk_entry()
{
}

gist_lstk::~gist_lstk()
{
    while (_top > 0) {
	gist_lstk_entry e;
	(void) pop(e);
	if (e.typ == gist_lstk_entry::eItem) {
	    delete e.val.item.key;
	    delete e.val.item.dataPtr;
	}
    }
}

void
gist_lstk::push(
    const shpid_t 	pid)
{
    gist_lstk_entry e;

    e.typ = gist_lstk_entry::eNode;
    e.val.node.page = pid;
    _push(&e);
}

void
gist_lstk::push(
    void*		key,
    smsize_t 		keyLen,
    void*		dataPtr,
    smsize_t 		dataPtrLen,
    shpid_t 		page,
    int 		idx)
{
    gist_lstk_entry e;

    e.typ = gist_lstk_entry::eItem;
    e.val.item.key = key;
    e.val.item.keyLen = keyLen;
    e.val.item.dataPtr = dataPtr;
    e.val.item.dataPtrLen = dataPtrLen;
    e.val.item.page = page;
    e.val.item.idx = idx;
    _push(&e);
}

gist_lstk_entry &
gist_lstk_entry::operator=(
    gist_lstk_entry&	e)
{
    this->typ = e.typ;
    if (e.typ == eNode) {
        this->val.node.page = e.val.node.page;
    } else {
        this->val.item.key = e.val.item.key;
        this->val.item.keyLen = e.val.item.keyLen;
        this->val.item.dataPtr = e.val.item.dataPtr;
        this->val.item.dataPtrLen = e.val.item.dataPtrLen;
        this->val.item.page = e.val.item.page;
        this->val.item.idx = e.val.item.idx;
    }
    return *this;
}

void
gist_lstk::pop(
    gist_lstk_entry&	e)
{
    w_assert3(! is_empty());
    if (_top <= lstk_chunk) {
	_top--;
	e = _stk[_top];
    } else {
	_top--;
	e = _indirect[_top/lstk_chunk-1][_top%lstk_chunk];
	if (_top >= lstk_chunk && _top % lstk_chunk == 0) {
	    delete _indirect[_top/lstk_chunk - 1];
	    _indirect[_top/lstk_chunk - 1] = 0;
	}
    }
}

void 
gist_lstk::_push(
    const gist_lstk_entry*	e)
{
    gist_lstk_entry *target;

    w_assert3(!is_full());
    if (_top < lstk_chunk) {
	target = &_stk[_top];
	_top++;
    } else {
	int2 pos = _top / lstk_chunk - 1;
	int2 off = _top % lstk_chunk;
	if (!_indirect[pos]) {
	    _indirect[pos] = new gist_lstk_entry[lstk_chunk];
	}
	target = &_indirect[pos][off];
	_top++;
    }

    target->typ = e->typ;
    if (e->typ == gist_lstk_entry::eNode) {
	target->val.node.page = e->val.node.page;
    } else {
	// create copies of key and data ptr
	target->val.item.keyLen = e->val.item.keyLen;
	target->val.item.key = (void *) new char[e->val.item.keyLen];
	(void) memcpy(target->val.item.key, e->val.item.key, e->val.item.keyLen);
	target->val.item.dataPtrLen = e->val.item.dataPtrLen;
	target->val.item.dataPtr = (void *) new char[e->val.item.dataPtrLen];
	(void) memcpy(target->val.item.dataPtr, e->val.item.dataPtr, e->val.item.dataPtrLen);
	target->val.item.page = e->val.item.page;
	target->val.item.idx = e->val.item.idx;
    }
}

void
gist_lstk::empty_all()
{
    while (_top > 0) {
	_top--;
	if (_top >= lstk_chunk && _top % lstk_chunk == 0) {
	    delete _indirect[_top/lstk_chunk - 1];
	}
    }
}

/*
 * gist_cursor_t
 */

gist_cursor_t::gist_cursor_t()
{
}

gist_cursor_t::~gist_cursor_t()
{
}

