//	gist_btree.cc
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libbtree/gist_btree.cc,v 1.2 1997/12/05 23:07:50 marcel Exp $

#ifdef __GNUG__
#pragma implementation "gist_btree.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef LIBGIST
#define SM_SOURCE
#define GIST_C
#include <sm_int_2.h>
#endif
#include "gist_btree.h"
#ifndef WIN32
#include <values.h> // for HIBITI
#else 
#define HIBITI (1U << (8 * (int) sizeof(int)) - 1)
#endif

#define MININT (HIBITI)

#ifndef WIN32 // Windows doesn't seem to have values.h
#include <values.h> // for HIBITI
#else 
#define HIBITI (1U << (8 * (int) sizeof(int)) - 1)
#endif

#ifdef WIN32 // Windows doesn't seem to have values.h
#define MAXINT ((int) ~(HIBITI))
#endif

ubt_ext_t::ubt_ext_t(CmpFct comp, ExtractFct extr,
    ComputeBpFct compute, PrintFct pr) :
    compare(comp), extract(extr), computeBps(compute), print(pr)
{
}

bool
ubt_ext_t::consistent(
    const void  *query,
    const void *pred,
    int plen,
    int level)
{
    bt_query_t *q = (bt_query_t *) query;

    if (level == 1) {
        switch (q->oper) {
	case bt_query_t::bt_nooper: 
	    return true;
	case bt_query_t::bt_eq: 
	    return (compare(pred, q->val1) == 0 ? true : false);
	case bt_query_t::bt_lt: 
	    return (compare(pred, q->val1) < 0 ? true : false);
	case bt_query_t::bt_le: 
	    return (compare(pred, q->val1) <= 0 ? true : false);
	case bt_query_t::bt_gt: 
	    return (compare(pred, q->val1) > 0 ? true : false);
	case bt_query_t::bt_ge: 
	    return (compare(pred, q->val1) >= 0 ? true : false);
	case bt_query_t::bt_betw: 
	    return ((compare(pred, q->val1) >= 0) && (compare(pred, q->val2) <= 0) ?
	        true : false);
	default: assert(0); // can't be
	}
    } else {
        switch (q->oper) {
	case bt_query_t::bt_nooper: 
	    return true;
	case bt_query_t::bt_eq: 
	    return (compare(extract(pred, 0), q->val1) <= 0 &&
	        compare(extract(pred, 1), q->val1) >= 0 ? true : false);
	case bt_query_t::bt_lt: 
	    return (compare(extract(pred, 0), q->val1) < 0 ? true : false);
	case bt_query_t::bt_le: 
	    return (compare(extract(pred, 0), q->val1) <= 0 ? true : false);
	case bt_query_t::bt_gt: 
	    return (compare(extract(pred, 1), q->val1) > 0 ? true : false);
	case bt_query_t::bt_ge: 
	    return (compare(extract(pred, 1), q->val1) >= 0 ? true : false);
	case bt_query_t::bt_betw: 
	    return (compare(extract(pred, 0), q->val2) <= 0 &&
	        compare(extract(pred, 1), q->val1) >= 0 ? true : false);
	default: assert(0); // can't be
	}
    }
    return false; // just to make MSVC 5.0 happy
}

void
ubt_ext_t::penalty(
    const void *pred,
    int plen,
    int level,
    const void *key,
    int klen,
    gist_penalty_t &p)
{
    if (compare(extract(pred, 0), key) <=0 && compare(extract(pred, 1), key) > 0) {
        p = 0.0;
    } else {
        p = 1.0;
    }
}

void
ubt_ext_t::union_key(
    void *bp,
    int &len,
    int level,
    const void *newPred,
    int newLen,
    bool &changed)
{
    changed = false; // B-trees are partitioning trees
}

void
ubt_ext_t::union_page(
    void *bp,
    int &len,
    int level, 
    gist_predcursor_t &pcursor,
    bool &changed)
{
    changed = false; // B-trees are partitioning trees
}


static ubt_ext_t::CmpFct localCompare;

struct SortItem {
    const void *ptr;
    int idx;
    int sz;
};

static int
qsort_cmp(const void *a, const void *b)
{
    SortItem *x = (SortItem *) a;
    SortItem *y = (SortItem *) b;

    return localCompare(x->ptr, y->ptr);
}

void
ubt_ext_t::pickSplit(
    gist_predcursor_t &pcursor,
    int level,
    int rightEntries[],
    int &numRight,
    const void *oldBp,
    int oldLen,
    void *leftBp,
    int &leftLen,
    void *rightBp,
    int &rightLen)
{
    SortItem items[gist_p::max_scnt];
    int itemCnt = pcursor.numElems;
    int totalCnt = 0;
    int i;

    for (i = 0; i < itemCnt; i++) {
        // copy info from predcursor
	items[i].ptr = pcursor.elems[i].key;
	items[i].idx = i;
	items[i].sz = pcursor.elems[i].keyLen;
	totalCnt += items[i].sz;
    }

    // sort the data;
    // wouldn't be necessary if we kept the data in sorted order, but then this 
    // wouldn't really be a Gist, would it
    localCompare = compare;
    qsort((void *) items, itemCnt, sizeof(SortItem), qsort_cmp);

    // now try to distribute the total byte count evenly
    int total = 0;
    i = 0;
    while (total < totalCnt/2 && i < itemCnt) {
	total += items[i].sz;
        i++;
    }
    i--; // the preceding pos holds the offending item
    // everything starting from pos i goes right
    numRight = itemCnt - i;
    for (int j = 0; j < numRight; j++) {
        rightEntries[j] = items[i+j].idx;
    }

    // item i is the dividing line; we use it to compute the new BPs
    computeBps(oldBp, leftBp, leftLen, rightBp, rightLen, items[i].ptr);
}

#ifndef	LIBGIST
void *
ubt_ext_t::eqQuery(
    const void* key,
    int keyLen)
{
    bt_query_t *q = new bt_query_t(bt_query_t::bt_eq, (void *) key, NULL);
    return (void *) q;
}
#endif

bool
ubt_ext_t::check(
    const void *bp, 
    int bplen, 
    const void *pred, 
    int predlen, 
    int level)
{
    if (level == 1) {
        return ((compare(extract(bp, 0), pred) <= 0 &&
	    compare(extract(bp, 1), pred) >= 0) ? true : false);
    } else {
        // predicates are pairs
	return ((compare(extract(bp, 0), extract(pred, 0)) <= 0 &&
	    compare(extract(bp, 1), extract(pred, 1)) >= 0) ? true : false);
    }
}

void
ubt_ext_t::printPred(
    const void *pred,
    int plen,
    int level)
{
    if (level == 1) {
        print(pred);
    } else {
        print(extract(pred, 0));
        print(extract(pred, 1));
    }
}

void
ubt_ext_t::printData(
    const void *data,
    int dlen)
{
    printf("%d", *((int *) data));
}


static int
int_cmp(const void *a, const void *b)
{
    int x = *((int *) a);
    int y = *((int *) b);
    if (x < y) { 
        return -1;
    } else if (x > y) {
        return 1;
    } else {
        return 0;
    }
    // this doesn't work when a is close to -2^31
    //int res = (*((int *) a) - *((int *) b));
}

static const void *
int_extract(const void *i, int slot)
{
    return (const void *) &(((int *) i)[slot]);
}

static void
int_bps(const void *oldBp, void *leftBp, int &leftLen,
    void *rightBp, int &rightLen, const void *splitItem)
{
    leftLen = rightLen = 2 * sizeof(int);

    const int *old = (int *) oldBp;
    int *left = (int *) leftBp;
    int *right = (int *) rightBp;
    int *split = (int *) splitItem;
    if (old == NULL) {
        // left and right boundaries are infinity
	left[0] = MININT;
	right[1] = MAXINT;
    } else {
        left[0] = old[0];
	right[1] = old[1];
    }
    left[1] = right[0] = *split;
}

static void
int_print(const void *pred)
{
    printf("%d", *((int *) pred));
}

static ubt_ext_t int_ext(int_cmp, int_extract, int_bps, int_print);
gist_unorderedn_ext_t ubt_int_ext(gist_ext_t::ubt_int_ext_id, int_ext);

static int
double_cmp(const void *a, const void *b)
{
    double x = *((double *) a);
    double y = *((double *) b);
    if (x < y) {
        return -1;
    } else if (x > y) {
        return 1;
    } else {
        return 0;
    }
}

static const void *
double_extract(const void *i, int slot)
{
    return (const void *) &(((double *) i)[slot]);
}

static void 
double_bps(const void *oldBp, void *leftBp, int &leftLen,
    void *rightBp, int &rightLen, const void *splitItem)
{
}

static void
double_print(const void *pred)
{
    printf("%f", *((double *) pred));
}

static ubt_ext_t double_ext(double_cmp, double_extract, double_bps, double_print);
gist_unorderedn_ext_t ubt_double_ext(gist_ext_t::ubt_double_ext_id, double_ext);


static int
str_cmp(const void *a, const void *b)
{
    if (*((const char *) a) == 0) return -1; // always smaller
    if (*((const char *) a) == 1) return 1; // always larger
    return (strcmp((const char *) a, (const char *) b));
}

static const void *
str_extract(const void *i, int slot)
{
    if (slot == 0) return i;
    // slot == 1
    const char *s = (const char *) i;
    return (const void *) (s + strlen(s) + 1);
}

static void 
str_bps(const void *oldBp, void *leftBp, int &leftLen,
    void *rightBp, int &rightLen, const void *splitItem)
{
}

static void
str_print(const void *pred)
{
    printf("%s", (char *) pred);
}

static ubt_ext_t str_ext(str_cmp, str_extract, str_bps, str_print);
gist_unorderedn_ext_t ubt_str_ext(gist_ext_t::ubt_str_ext_id, str_ext);
rc_t

bt_ext_t::insert(
    gist_p& page,
    const vec_t& key,
    const vec_t& dataPtr,
    shpid_t child)
{
    const void* data;
    if (page.is_leaf()) {
        data = dataPtr.ptr(0);
    } else {
        // our key also contains a data pointer
	data = (const void *) (((char *) key.ptr(0)) + this->size(key.ptr(0)));
    }
    int slot = _binSearch(page, key.ptr(0), dataPtr.ptr(0), false);
    W_DO(page.insert(key, dataPtr, slot + 1, child));
    return RCOK;
}

rc_t 
bt_ext_t::remove(
    gist_p& page,
    int slots[],
    int numSlots)
{
    for (int i = numSlots - 1; i >= 0; i--) {
        W_DO(page.remove(slots[i]));
    }
    return RCOK;
}

rc_t
bt_ext_t::updateKey(
    gist_p& page,
    int& slot,
    const vec_t& newKey)
{
    return RCOK;
}

void
bt_ext_t::findMinPen(
    const gist_p& page,
    const vec_t& key,
    const vec_t& data,
    int& slot)
{
    slot = _binSearch(page, key.ptr(0), data.ptr(0), false);
    assert(slot != -1);
}

void 
bt_ext_t::search(
    gist_p& page,
    const void* query,
    int matches[],
    int& numMatches)
{
    const bt_query_t* q = (const bt_query_t *) query;
    int start, end; // start and end slot to scan

    numMatches = 0;
    switch (q->oper) {
    case bt_query_t::bt_nooper:
        start = 0;
	end = page.nrecs() - 1;
	break;
    case bt_query_t::bt_eq:
        start = end = _binSearch(page, q->val1, NULL, true);
	if (start == -1) {
	    // we're not going to find anything here
	    return;
	}
	break;
    case bt_query_t::bt_lt:
    case bt_query_t::bt_le:
        start = 0;
	end = _binSearch(page, q->val1, NULL, true);
	break;
    case bt_query_t::bt_gt:
    case bt_query_t::bt_ge:
        start = _binSearch(page, q->val1, NULL, true);
	if (start == -1) start = 0;
	end = page.nrecs() - 1;
	break;
    case bt_query_t::bt_betw: 
        start = _binSearch(page, q->val1, NULL, true);
	if (start == -1) start = 0;
        end = _binSearch(page, q->val2, NULL, true);
	break;
    default: // something's wrong
        assert(0);
    };

    bool hit = false;
    for (int slot = start; slot <= end; slot++) {
        if (page.is_leaf()) {
	    switch (q->oper) {
	    case bt_query_t::bt_nooper:
		hit = true;
		break;
	    case bt_query_t::bt_eq:
	        if (keyCmp(page.rec(slot).key(), q->val1) == 0) hit = true;
		break;
	    case bt_query_t::bt_lt:
	        if (slot != end || keyCmp(page.rec(slot).key(), q->val1) < 0) {
		    hit = true;
		}
		break;
	    case bt_query_t::bt_le:
	        hit = true;
		break;
	    case bt_query_t::bt_gt:
	        if (slot != start) hit = true;
		break;
	    case bt_query_t::bt_ge:
	        if (slot != start || keyCmp(page.rec(slot).key(), q->val1) >= 0) {
		    hit = true;
		}
		break;
	    case bt_query_t::bt_betw:
	        if (slot != start || keyCmp(page.rec(slot).key(), q->val1) >= 0) {
		    hit = true;
		}
		break;
	    default: // something's fishy
	        assert(0);
	    }

	} else { // internal node
	    if (q->oper == bt_query_t::bt_lt && slot == end) {
	        if (keyCmp(page.rec(slot).key(), q->val1) < 0) hit = true;
	    } else {
		hit = true;
	    }
	}
	if (hit) {
	    matches[numMatches] = slot;
	    numMatches++;
	    hit = false;
	}
    }
}

void 
bt_ext_t::getKey(
    gist_p& page,
    int slot,
    vec_t& key)
{
    const keyrec_t& tup = page.rec(slot);
    key.set(tup.key(), this->size(tup.key()));
}

rc_t 
bt_ext_t::pickSplit(
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
{
    // first, count the number of bytes used for all keys (also the new ones)
    int totalBytes = 0;
    int slotCnt = page.nrecs();
    int i;
    for (i = 0; i < slotCnt; i++) {
	// for internal entries, we include the data pointers, because
	// these will also be distributed (and who knows, they might
	// even be varying length)
	totalBytes += page.rec(i).klen();
    }
    totalBytes += entry1.len(0);
    slotCnt++;
    totalBytes += entry2.len(0);
    if (entry2.size() != 0) slotCnt++;

    posInfo entries[gist_p::max_scnt];
    keyrec_t::hdr_s hdr1, hdr2;
    _insertEntries(page, entry1, entry2, hdr1, hdr2, entries);
    // now, accumulate entries until you exceed half the total size
    int total = 0;
    i = 0;
    while (total < totalBytes/2 && i < slotCnt) {
        total += entries[i].hdr->klen();
	i++;
    }
    assert(i != slotCnt); // can't be...

    // everything from slot i on goes right ...
    numRight = 0;
    oneGoesRight = false;
    twoGoesRight = false;
    for (int j = i; j < slotCnt; j++) {
	if (entries[j].slot > 0) {
	    rightEntries[numRight] = entries[j].slot;
	    numRight++;
	} else if (entries[j].slot == -1) {
	    oneGoesRight = true;
	} else {
	    assert(entries[j].slot == -2);
	    twoGoesRight = true;
	}
    }

    // the BP of the original node stays the same
    if (oldBp.size() != 0) {
	(void) memcpy(leftBp.ptr(0), oldBp.ptr(0), oldBp.len(0));
	const void *leftptr = leftBp.ptr(0);
	leftBp.set(leftptr, oldBp.len(0));
    } else {
        // this is what used to be the root;
	// the BP becomes -infinity
	void *leftptr = leftBp.ptr(0);
	negInftyKey(leftptr);
	int leftsz = this->size(leftptr);
	void *dataptr = (void *) ((char *) leftptr + leftsz);
	negInftyData(dataptr);
	leftsz += this->size(dataptr);
	leftBp.set(leftptr, leftsz);
    }

    // the BP of the new right sibling is the item at the split point
    void* rightptr = rightBp.ptr(0);
    int rightlen;
    if (entries[i].slot > 0) {
        // take then BP from the page
	const keyrec_t& tup = page.rec(entries[i].slot);
	(void) memcpy(rightptr, tup.key(), tup.klen());
	rightlen = tup.klen();
	if (page.is_leaf()) {
	    // also copy the data ptr
	    (void) memcpy((void *) ((char *) rightptr + rightlen), tup.elem(), tup.elen());
	    rightlen += tup.elen();
	}
    } else {
        const vec_t* e;
	if (entries[i].slot == -1) {
	    e = &entry1;
	} else {
	    assert(entries[i].slot == -2);
	    e = &entry2;
	}
	(void) memcpy(rightptr, e->ptr(0), e->len(0));
	rightlen = e->len(0);
	if (page.is_leaf()) {
	    (void) memcpy((void *) ((char *) rightptr + (int) rightlen), e->ptr(1), e->len(1));
	    rightlen += e->len(1);
	}
    }
    rightBp.set(rightptr, rightlen);

    return RCOK;
}

void 
bt_ext_t::unionPage(
    gist_p& page,
    vec_t& bp,
    bool& bpChanged)
{
    bpChanged = false;
} 

void 
bt_ext_t::unionKey(
    vec_t& bp,
    int level, 
    const vec_t& pred,
    bool &bpChanged)
{
    bpChanged = false;
}

void *
bt_ext_t::eqQuery(
    const vec_t& keyv)
{
    bt_query_t *q = new bt_query_t(bt_query_t::bt_eq, keyv.ptr(0), NULL);
    return (void *) q;
}

bool 
bt_ext_t::check(
    const vec_t& bp,
    const vec_t& pred,
    int level)
{
    return (keyCmp(pred.ptr(0), bp.ptr(0)) >= 0);
}

void 
bt_ext_t::printPred(
    const vec_t& pred,
    int level)
{
    prPred(pred.ptr(0));
    if (level > 1) {
        const void *dataptr = (void *) ((char *) pred.ptr(0) + this->size(pred.ptr(0)));
	printf(", ");
	prData(dataptr);
    }
}

void 
bt_ext_t::printData(
    const vec_t& data)
{
    prData(data.ptr(0));
}
        

int
bt_ext_t::_binSearch(
    const gist_p& page,
    const void* key,
    const void* data,
    bool keyOnly) // true: only compare keys
{
    int hi = page.nrecs() - 1;
    if (hi == -1) {
        // nothing on this page yet
	return -1;
    }
    int lo = 0;
    int mid;
    const void* midkey;
    const void* middata;
    int res;

    for (;;) {
	mid = (hi + lo) / 2;
        const keyrec_t& tup = page.rec(mid);
	midkey = tup.key();
	if (page.is_leaf()) {
	    middata = tup.elem();
	} else {
	    int sz = this->size(midkey);
	    middata = (const void *) (((char *) midkey) + sz);
	}
	res = keyCmp(key, midkey);
	if (!keyOnly && res == 0) {
	    res = dataCmp(data, middata);
	}
	if (res < 0) {
	    // key is smaller than midpoint
	    hi = mid - 1;
	} else if (res > 0) {
	    lo = mid + 1;
	} else {
	    // found an exact match
	    return mid;
	}
	if (hi < lo) return hi;
    }
#if 0 // just for explanatory purposes
    if (res < 0) {
        return mid-1;
	// because mid-1 is our upper bound, but also our lower bound
	// (hi <= lo) 
    } else {
        // res > 0: lo = hi, because mid < hi and lo now = mid + 1
        return hi;
    }
#endif
}

void
bt_ext_t::_insertEntries(
    gist_p& page,
    const vec_t& entry1,
    const vec_t& entry2,
    keyrec_t::hdr_s& hdr1,
    keyrec_t::hdr_s& hdr2,
    posInfo entries[])
{
    int cnt = page.nrecs();
    int numEntries = cnt;
    int k;
    for (k = 0; k < cnt; k++) {
        entries[k].hdr = &page.rec(k);
	entries[k].slot = k;
    }

    // Figure out where entry1/-2 would go.
    const void *data1, *data2;
    if (page.is_leaf()) {
	data1 = entry1.ptr(1);
	data2 = entry2.ptr(1);
    } else {
	// extract data portion from "key"
	data1 = (((char *) entry1.ptr(0)) + this->size(entry1.ptr(0)));
	data2 = (((char *) entry2.ptr(0)) + this->size(entry2.ptr(0)));
    }
    int oneSlot = _binSearch(page, entry1.ptr(0), data1, false) + 1;
    int twoSlot = -1;
    const vec_t* firstEntry = NULL; // new entry with "lower" slot index
    const vec_t* secondEntry = NULL; // new entry with "higher" slot index
    if (entry2.size() != 0) {
	twoSlot = _binSearch(page, entry2.ptr(0), data2, false) + 1;
	if (oneSlot == twoSlot) {
	    // we have to determine which one of the entries goes first
	    int res = keyCmp(entry1.ptr(0), entry2.ptr(0));
	    if (res == 0) {
	        res = dataCmp(data1, data2);
	    }
	    if (res < 0) {
	        firstEntry = &entry1;
		secondEntry = &entry2;
	    } else if (res > 0) {
	        firstEntry = &entry2;
		secondEntry = &entry1;
	    } else {
	        // res == 0: something's wrong
		assert(0);
	    }
	} else if (oneSlot < twoSlot) {
	    firstEntry = &entry1;
	    secondEntry = &entry2;
	} else { // oneSlot > twoSlot
	    firstEntry = &entry2;
	    secondEntry = &entry2;
	}
    } else {
        // we only have entry1
	secondEntry = &entry1;
    }
    int firstSlot = (oneSlot > twoSlot ? twoSlot : oneSlot);
    int secondSlot = (oneSlot > twoSlot ? oneSlot : twoSlot);
    bool oneGoesFirst = (firstEntry == &entry1);

    // insert one entry
    hdr1.klen = secondEntry->len(0);
    numEntries++;
    for (k = numEntries-1; k > secondSlot; k--) {
        entries[k] = entries[k-1];
    }
    entries[secondSlot].hdr = (keyrec_t *) &hdr1;
    entries[secondSlot].slot = (oneGoesFirst ? -2 : -1);

    // insert other entry
    if (entry2.size() != 0) {
	hdr2.klen = firstEntry->len(0);
	numEntries++;
	for (k = numEntries-1; k > firstSlot; k--) {
	    entries[k] = entries[k-1];
	}
	entries[firstSlot].hdr = (keyrec_t *) &hdr2;
	entries[firstSlot].slot = (oneGoesFirst ? -1 : -2);
    }
}

static int
int_size(const void *i)
{
    return sizeof(int);
}

static void
int_printPred(const void *p)
{
    printf("%d", *((int *) p));
}

static void
int_negInfty(void *i)
{
    *((int *) i) = MININT;
}

bt_ext_t bt_int_ext(gist_ext_t::bt_int_ext_id, int_cmp, int_cmp, int_size, int_printPred,
    int_printPred, int_negInfty, int_negInfty);

static int
str_size(const void *s)
{
    char *str = (char *) s;
    if (str[0] == 1) return 1; // this is -infinity, encoded in one byte
    return strlen((char *) s) + 1;
}

static void 
str_printPred(const void *s)
{
    printf("%s", (char *) s);
}

static void
str_negInfty(void *s)
{
    *((char *) s) = 1;
}

bt_ext_t bt_str_ext(gist_ext_t::bt_str_ext_id, str_cmp, int_cmp, str_size, str_printPred,
    int_printPred, str_negInfty, int_negInfty);
