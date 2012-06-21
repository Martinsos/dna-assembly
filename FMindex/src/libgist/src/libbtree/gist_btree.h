//	gist_btree.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libbtree/gist_btree.h,v 1.2 1997/12/05 23:07:51 marcel Exp $

#ifdef __GNUG__
#pragma interface
#endif

#ifndef GIST_BTREE_H
#define GIST_BTREE_H

#include <gist.h>
#include <gist_ext.h>
#include <gist_unordered.h>
#include <gist_unorderedn.h>

class bt_query_t {
public:

    enum bt_oper {
        bt_nooper,
	bt_eq,
	bt_lt,
	bt_le,
	bt_gt,
	bt_ge,
	bt_betw, // SQL BETWEEN operator
	bt_numoper
    };

    bt_query_t(bt_oper oper, void *val1, void *val2) : oper(oper), val1(val1), val2(val2) {}
    ~bt_query_t()
    {
        if (val1 != NULL) delete val1;
        if (val2 != NULL) delete val2;
    }

    bt_oper oper;
    void *val1;
    void *val2; // only used as upper interval bound for bt_betw
};


class ubt_ext_t : public gist_unordered_ext_t {
public:

    // comparison function for two keys;
    // returns: < 0 for a < b
    //          = 0 for a == b
    //          > 0 for a > b
    typedef int (*CmpFct)(const void *a, const void *b);
    CmpFct compare;

    // extraction function for the two components of a pair (internal node BP)
    typedef const void *(*ExtractFct)(const void *item, int slot); // slot either 0 or 1
    ExtractFct extract;

    // pickSplit(): compute the new BPs, given the old one and the split item;
    // we needn't differentiate by level, because only the first component of the split 
    // item is of interest to us (if it isn't a leaf predicate anyway)
    typedef void (*ComputeBpFct)(const void *oldBp, void *leftBp, int &leftLen,
        void *rightBp, int &rightLen, const void *splitItem);
    ComputeBpFct computeBps;

    // for printPred()
    typedef void (*PrintFct)(const void *pred);
    PrintFct print;

    ubt_ext_t(CmpFct comp, ExtractFct extr, ComputeBpFct compute, PrintFct pr);

    bool consistent(
        const void  *query,
	const void *pred,
	int predLen,
	int level);

    void penalty(
        const void *subtreePred,
	int predLen,
	int level,
	const void *newKey,
	int keyLen,
	gist_penalty_t &p);

    void union_key(
        void *bp,
	int &len,
	int level,
	const void *newPred,
	int newLen,
	bool &changed);

    void union_page(
        void *bp,
	int &len,
	int level, 
	gist_predcursor_t &pcursor,
	bool &changed);

    void pickSplit(
        gist_predcursor_t &pcursor,
	int level,
	int rightEntries[],
	int &numRightEntries,
	const void *oldBp,
	int oldLen,
	void *leftBp,
	int &leftLen,
	void *rightBp,
	int &rightLen);

#ifndef LIBGIST
    void* eqQuery(
        const void* keyv,
	int keyLen);
#endif

    bool check(
        const void *bp, 
	int bplen, 
	const void *pred, 
	int predlen, 
	int level); 

    void printPred(
        const void *pred,
	int plen,
	int level); 

    void printData(
        const void *data,
	int dlen);

};

extern gist_unorderedn_ext_t ubt_int_ext;
extern gist_unorderedn_ext_t ubt_string_ext;
extern gist_unorderedn_ext_t ubt_double_ext;


class bt_ext_t : public gist_ext_t {
public:

    // generic comparison function
    typedef int (*CmpFct)(const void *a, const void *b);
    CmpFct keyCmp; // for keys
    CmpFct dataCmp; // for data pointers

    // returns the size in bytes of a key stored on a disk page;
    // the goal is to get to the data pointer portion of an internal entry,
    // given the pointer to the key portion
    typedef int (*SizeFct)(const void *e);
    SizeFct size;

    // for printPred()
    typedef void (*PrintFct)(const void *pred);
    PrintFct prPred, prData;

    // for pickSplit() (creating new BPs)
    typedef void (*NegInftyFct)(void *x);
    NegInftyFct negInftyKey, negInftyData;

    bt_ext_t(gist_ext_t::gist_ext_ids id, CmpFct keyCmp, CmpFct dataCmp, SizeFct size,
        PrintFct prPred, PrintFct prData,
        NegInftyFct negInftyKey, NegInftyFct negInftyData) :
        keyCmp(keyCmp), dataCmp(dataCmp), size(size), prPred(prPred),
	prData(prData), negInftyKey(negInftyKey), negInftyData(negInftyData)
    {
	myId = id;
        gist_ext_t::gist_ext_list[myId] = this;
    };

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
	const vec_t& key,
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

    void * eqQuery(
        const vec_t& keyv);

    bool check(
	const vec_t& bp,
	const vec_t& pred,
	int level);

    void printPred(
	const vec_t& pred,
	int level);

    void printData(
	const vec_t& data);
        

private:

    int _binSearch(
        const gist_p& page,
	const void* key,
	const void* data,
	bool keyOnly);


    struct posInfo {
        const keyrec_t* hdr;
	int slot; // slot index on page; or: -1: entry1, -2: entry2
    };

    void _insertEntries(
        gist_p& page,
	const vec_t& entry1,
	const vec_t& entry2,
	keyrec_t::hdr_s& hdr1,
	keyrec_t::hdr_s& hdr2,
	posInfo entries[]);

};

extern bt_ext_t bt_int_ext;
extern bt_ext_t bt_str_ext;

#endif
