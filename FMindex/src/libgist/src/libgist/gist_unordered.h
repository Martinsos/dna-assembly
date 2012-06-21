//	gist_unordered.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist_unordered.h,v 1.2 1997/12/05 23:08:50 marcel Exp $


#ifndef GIST_UNORDERED_EXT_H
#define GIST_UNORDERED_EXT_H

#include "gist_defs.h"
#include "gist_ext.h"
#include "gist_p.h"

class gist_predcursor_t {
public:

    struct entry {
        const void *key;
	int keyLen;
    };

    // + 2: for new entries
    enum { // MSVC 5.0 doesn't allow "const int ..." here
        maxElems = gist_p::max_scnt + 2
    };
    int numElems;
    entry elems[maxElems];

    gist_predcursor_t();
    ~gist_predcursor_t(); 

    // make cursor return predicate
    void add(const void* data, int len);

    // make cursor return keys on page (except for BP)
    void add(gist_p& page);

    // start from the beginning
    void reset();
};


class gist_unordered_ext_t  {
public:

    // consistent: evaluates pred w.r.t. query
    virtual bool consistent(
        const void* query,
	const void* pred,
	int predLen,
	int level) = 0; // at what level was that predicate found (leaf = 1)

    // returns insertion penalty of new key into subtree
    virtual void penalty(
        const void* subtreePred,
	int predLen,
	int level, // at what level was that predicate found
	const void* newKey,
	int keyLen,
	gist_penalty_t& p) = 0;

    // union the newly inserted pred to the BP;
    // the length of the old bp is 
    // indicate if BP changed;
    virtual void union_key(
        void* bp, // in: old BP, out: updated BP
	int& len, // in: length of old BP; out: length of changed BP
	int level, // level of page where BP is on
	const void* newPred, // leaf-type predicate
	int newLen, // length of predicate
	bool& changed  // out: true if bp changed
    ) = 0;

    // union all the keys on a single page
    // done after an item was deleted;
    // indicate if BP changed;
    virtual void union_page(
        void* bp, int &len, int level, // level of BP
	gist_predcursor_t& pcursor,
	bool& changed) = 0;

    // Determine which entries go on the new sibling after a split 
    // (the indices of the entries that should be moved right are stored in rightEntries,
    // their number in numRight; the indices are returned by gist_predcursor_t::fetch() and do
    // not have to be stored in the order delivered by fetch() in rightEntries);
    // compute the new BPs for both nodes
    // (can't do it with union, because we might need to know the BP of the 
    // other sibling).
    // If the old BP is NULL, we're splitting the root; the bounds should
    // be assumed to be infinite in this case.
    virtual void pickSplit(
        gist_predcursor_t& pcursor, // used to retrieve entries for page
	int level, // level of page (1 = leaf)
	int rightEntries[], // out: store indices of entries to go on right sibling
	int& numRightEntries, // out: store number of entries to go right
	const void* oldBp, // pre-split BP of original page
	int oldLen, // length of pre-split BP
	void* leftBp, // out: new BP of original page
	int& leftLen, // out: length of new left BP
	void* rightBp, // out: new BP of new right sibling
	int& rightLen) // out: length of new right BP
	= 0;

#ifndef LIBGIST
    // Construct an equality predicate for the given key 
    virtual void *eqQuery(
        const void* key,
	int keyLen) = 0;
#endif

    // Return true if the predicate is contained by the BP.
    // Used during gist_t::check() to verify the integrity of 
    // the node's content.
    virtual bool check(
        const void* bp, // BP on page
	int bplen, // length of BP
	const void* pred, // single entry's predicate
	int predlen, // length of predicate
	int level) // level of page
	= 0;

    virtual void printPred(
        const void* pred,
	int plen,
	int level) // level of page where pred is stored
	= 0;

    virtual void printData(
        const void* data,
	int dlen)
	= 0;
        
};

#endif // GIST_UNORDERED_EXT_H
