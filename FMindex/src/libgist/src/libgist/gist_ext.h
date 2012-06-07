//	gist_ext.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist_ext.h,v 1.2 1997/12/05 23:08:40 marcel Exp $


#ifndef GIST_EXT_H
#define GIST_EXT_H

#include "gist_p.h"
#ifdef LIBGIST
#include "vec_t.h"
#endif

class gist_predcursor_t;

class gist_penalty_t {
public:
    static const double max_penalty;

    gist_penalty_t() {}
    gist_penalty_t(double p) : p(p) {}
    double p; // a double must be enough

    bool operator<(const gist_penalty_t &pen) { return p < pen.p; }
    gist_penalty_t& operator=(const gist_penalty_t &pen) { p = pen.p; return *this; }
};


/*
 * Interface for Gist extension classes.
 * The procedures here are mostly node layout abstractions, only 
 * unionKey(), check(), print*() and eqQuery() are data abstractions.
 * The subclasses of gist_ext_t are supposed to implement the node layout aspects,
 * and offer a cleaner interface that requires the user to specify only the data
 * abstraction routines.
 * The subclass implementer can use any of the gist_p member functions to implement
 * a specific node layout. Also, every page contains a BP (slot index: gist_p::bpSlot),
 * except for the root page.
 * The subclass implementation must guarantee that 
 * - every entry (consisting of some key data and a data/child pointer) on the page is stored as
 *   a keyrec_t (with keyrec_t as a header and the key and
 *   data following) and has a slot associated with it, no matter how much the key is
 *   compressed (the intention is that the data and child pointers are accessible via
 *   gist_p::rec(index).elem() and gist_p::rec(index).child());
 * - slot indices are stable until the next *successful* call to insert(), remove(),
 *   updateKey() and pickSplit()  (i.e., they don't change between a call to pickSplit()
 *   and remove(); also, updateKey() must not change the slot assignments if it returns
 *   with an error);
 * The following things are *not* required:
 * - using a contiguous block of slots in the slot array (used and unused slots may
 *   alternate)
 * - the information to reconstruct a full key need not be stored in a single entry, as
 *   long as getKey() can reconstruct the key given the associated slot index
 * 
 * In order to implement logical undo of data item insertion and deletion in Shore, we
 * need to be able to associate extension objects with log records and access those 
 * extension objects at recovery time. The type gist_ext_ids records IDs for extension
 * objects and the array gist_ext_list maps those IDs to the corresponding objects. 
 * Every new extension object should have an entry in gist_ext_ids, have its own myId
 * set to that entry and "register" itself in gist_ext_list.
 */

class gist_ext_t {
public:

    // list of extension object IDs
    enum gist_ext_ids {
	rt_point_ext_id,
	rt_rect_ext_id,
	ubt_int_ext_id,
	ubt_str_ext_id,
	ubt_double_ext_id,
	bt_int_ext_id,
	bt_str_ext_id,
        gist_numext
    };
    // ID of the particular extension object at hand
    gist_ext_ids myId;
    // array of extension objects 
    static gist_ext_t *gist_ext_list[gist_numext];

    // here are the node layout-specific routines

    // Create a new entry on a page.
    // Return eRECWONTFIT if the new entry does not fit on the page.
    virtual rc_t insert(
        gist_p& page,
	const vec_t& key,
	const vec_t& data,
	shpid_t child)
	= 0;

    // Remove a number of entries, identified by their slot indices.
    virtual rc_t remove(
        gist_p& page,
	int slots[],
	int numSlots)
	= 0;

    // For internal nodes only: update the key for the entry in the given slot.
    virtual rc_t updateKey(
        gist_p& page,
	int& slot, // in/out: old/new slot assignment
	const vec_t& newKey)
	= 0;

    // Find the entry with the lowest insertion penalty for the given key;
    // return the slot index in 'slot'.
    virtual void findMinPen(
        const gist_p& page,
	const vec_t& key,
	const vec_t& data,
	int& slot)
	= 0;

    // Search the page for entries matching the given query;
    // return their slot indices in 'matches'.
    virtual void search(
        gist_p& page,
	const void* query,
	int matches[],
	int& numMatches)
	= 0;

    // Return the key of the entry in the given slot. If a conversion/decompression
    // of the key takes place, the key is written into the memory pointed to by 
    // key.ptr(0).
    // If no conversion takes place, the key points to the buffered page.
    virtual void getKey(
        gist_p& page,
	int slot,
	vec_t& key)
        = 0;

    // Determine how to split the given node. The split info is the slot indices of
    // those entries that go to the right sibling. Make sure that rightEntries only 
    // contains the indices of existing slots. This procedure also calculates the 
    // new BPs (which have to be copied into left-/rightBp).
    // The new entry/-ies that will be inserted after the split are given in
    // entry1 and -2 (if they are leaf entries, the second component will hold the data
    // pointer); those need to be taken into consideration when calculating the BPs.
    // (if there's no second entry to be inserted, entry2.size() == 0). The caller also 
    // needs to know which page to insert each entry on; this information is returned
    // in *GoesRight.
    virtual rc_t pickSplit(
        gist_p& page,
	int rightEntries[],
	int& numRight,
	const vec_t& oldBp,
	vec_t& leftBp,
	vec_t& rightBp,
	const vec_t& entry1,
	bool& oneGoesRight,
	const vec_t& entry2,
	bool& twoGoesRight)
	= 0;

    // Compute the BP of the page after item(s) were deleted, given the prior
    // BP. 
    virtual void unionPage(
        gist_p& page,
	vec_t& bp,
	bool& bpChanged) // set to true if BP changed
	= 0; 


    // here are the datatype-specific routines

    // union the newly inserted pred to the BP;
    // the storage allocated for the bp is max_tup_sz;
    // indicate if BP changed;
    virtual void unionKey(
	vec_t& bp,
	int level, // level of page where BP is on
	const vec_t& pred,
	bool &bpChanged  // out: true if bp changed
    ) = 0;

#ifndef	LIBGIST
    // Construct an equality query for the given key.
    // This is used by gist_m::remove() to convert the key into 
    // a query for traversal. This procedure must not allocate memory
    // beyond what is returned (so that it can be free()'d)
    virtual void * eqQuery(
        const vec_t& keyv) = 0;
#endif

#ifdef LIBGIST
    // Return true if the predicate is contained by the BP.
    // Used during gist_t::check() to verify the integrity of 
    // the node's content.
    virtual bool check(
	const vec_t& bp,
	const vec_t& pred,
	int level) // level of page
	= 0;
#endif

    virtual void printPred(
	const vec_t& pred,
	int level) // level of page where pred is stored
	= 0;

    virtual void printData(
	const vec_t& data)
	= 0;
        
};

#endif // GIST_EXT_H

