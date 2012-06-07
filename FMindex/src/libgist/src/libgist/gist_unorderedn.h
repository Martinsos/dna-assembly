//	gist_unorderedn.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist_unorderedn.h,v 1.2 1997/12/05 23:08:53 marcel Exp $

#ifdef __GNUG__
#pragma interface
#endif

#ifndef GIST_UNORDEREDN_H
#define GIST_UNORDEREDN_H

/*
 * Gist extension for unordered nodes.
 * The node layout implemented here is the same as that of R-trees: 
 * items are stored in arbitrary order on the page (so that all items must be
 * examined when looking for a matching one or minimum-penalty item).
 * This class does not implement any datatype-specific behavior; this is delegated 
 * to gist_unordered_ext_t (note the missing 'n'), which offers an interface similar
 * to what is described in the original Gist publication.
 */

#include "gist_ext.h"

class gist_unordered_ext_t; // vanilla Gist extension interface

class gist_unorderedn_ext_t : public gist_ext_t {
public:

    gist_unordered_ext_t& ext;

    rc_t insert(
        gist_p& page,
	const vec_t& key,
	const vec_t& data,
	shpid_t child);

    rc_t remove(
        gist_p& page,
	int slots[],
	int numSlots);

    rc_t updateKey(
        gist_p& page,
	int& slot,
	const vec_t& newKey);

    void findMinPen(
        const gist_p& page,
	const vec_t& newKey,
	const vec_t& data,
	int& slot);

    void search(
        gist_p& page,
	const void* query,
	int matches[],
	int& numMatches);

    void getKey(
        gist_p& page,
	int slot,
	vec_t& key);

    rc_t pickSplit(
        gist_p& page,
	int rightEntries[],
	int& numRight,
	const vec_t& oldBp,
	vec_t& leftBp,
	vec_t& rightBp,
	const vec_t& entry1,
	bool& oneGoesRight,
	const vec_t& entry2,
	bool& twoGoesRight);

    void unionPage(
        gist_p& page,
	vec_t& bp,
	bool& bpChanged); 

    void unionKey(
	vec_t& bp,
	int level, 
	const vec_t& pred,
	bool &bpChanged);

#ifndef LIBGIST
    void * eqQuery(
        const vec_t& keyv);
#endif

    bool check(
	const vec_t& bp,
	const vec_t& pred,
	int level);

    void printPred(
	const vec_t& pred,
	int level);

    void printData(
	const vec_t& data);
        

    gist_unorderedn_ext_t(gist_ext_t::gist_ext_ids myId, gist_unordered_ext_t& ext);

};

#endif // ifndef GIST_UNORDEREDN_H
