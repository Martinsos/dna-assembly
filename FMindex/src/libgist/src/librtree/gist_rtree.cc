//	gist_rtree.cc
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/librtree/gist_rtree.cc,v 1.2 1997/12/05 23:09:29 marcel Exp $

#ifdef __GNUG__
#pragma implementation "gist_rtree.h"
#endif

#include <stdio.h>
#include <string.h>
#include <new.h>
#include <math.h> // for fabs, pow
#ifndef LIBGIST
#define SM_SOURCE
#define GIST_C
#include <sm_int_2.h>
#endif
#include "gist.h"
#include "gist_rtree.h"


rt_point::rt_point(int d) : dimension(d) 
{
    // check for valid dimension: must be > 0
    if (dimension <= 0) dimension = 1;
    // initialize coordinates
    coord = new double [dimension];
    for (int i = 0; i < dimension; i++) {
	coord[i] = 0;
    }
}

rt_point::~rt_point() 
{ 
    // delete [] coord; 
    // dont delete; coord might reside on page
}


bool 
rt_point::isEqual(rt_point & k) 
{
    if (k.dim() != dimension) {
	return false;
    }
  
    // compare the coordinates
    for (int i = 0; i < dimension; i++) {
        if (coord[i] != k[i]) {
            return false;
	}
    }
    return true;
}

double
rt_point::dist(rt_point *s) 
{
    assert(dim() == s->dim());

    double sum = 0;

    for (int i = 0; i < dim(); i++) {
        sum += pow(fabs(coord[i] - s->coord[i]), (double)dim());
    }

    double ret = pow(sum, ((double) 1)/((double) dim()));
    return ret;
}


bool 
rt_point::contains(rt_rect & rect)
{
    if (rect.dim() != dimension) {
        return false;
    }

    for (int i = 0; i < dimension; i++) {
        if (rect.lo(i) != rect.hi(i) || rect.lo(i) != coord[i]) {
            // out of range in dimension i
            return false;
        }
    }
    return true;
}
 

bool 
rt_point::contains(rt_point & pt)
{
    return isEqual(pt);
}

rt_rect::rt_rect(int d) : dimension(d) 
{ 
    // check for valid dimension, must be at least 1
    // (1-d rect is a 1-d line and 3-d rect is a 'cube', etc.)
    if (dimension <= 0) dimension = 1;
    // initialize coordinates:
    coord = new double[2*dimension];
    for (int i=0; i<dimension; i++) coord[2*i] = coord[2*i+1] = 0;
}

rt_rect::rt_rect(int d, const double *coord)
    : dimension(d), coord((double *) coord)
{
}

rt_rect::rt_rect(int d, double *coord) : dimension(d), coord(coord)
{
}

rt_rect::rt_rect(rt_rect &r) 
{
    dimension = r.dimension;
    coord = new double[2*dimension];
    for (int i = 0; i < dimension; i++) {
        lo(i) = r.lo(i);
        hi(i) = r.hi(i);
    }
}

rt_rect::~rt_rect() 
{
    // dont delete; coord might resides on disk page
}

void
rt_rect::dealloc()
{
    delete [] coord;
}

double &
rt_rect::lo(int i)
{
    assert(i >= 0 && i < dimension);
    return coord[2*i];
}

double &
rt_rect::hi(int i)
{
    assert(i >= 0 && i < dimension);
    return coord[2*i+1];
}

bool 
rt_rect::isEqual(rt_rect & k) 
{
    if (k.dimension != dimension) return false;
    // check for lo & hi coordinates
    for (int i = 0; i < 2 * dimension; i++) {
	if (coord[i] != k.coord[i]) {
	    return false;
	}
    }
    return true;
}

double 
rt_rect::span() 
{ 
    double result = 1.0;

    for (int i=0; i<dimension; i++) {
	if (lo(i) > hi(i)) {
	    result = 0.0;
	    break;
	}
	result = result * (hi(i) - lo(i));
    }
    return result; 
}

double
rt_rect::margin()
{
    double result = 0.0;

    for (int i = 0; i < dimension; i++) {
	if (hi(i) > lo(i)) {
	    result += hi(i) - lo(i);
	}
    }
    return result;
}


bool 
rt_rect::expand(rt_point & pt) 
{
    if (pt.dim() != dimension) return false;
    for (int i = 0; i < dimension; i++) {
	// adjust upper/lower bounds for dimension i
	if (pt[i] < lo(i)) lo(i) = pt[i];
	if (pt[i] > hi(i)) hi(i) = pt[i];
    }
    return true;
}


bool 
rt_rect::expand(rt_rect & rect) 
{
    if (rect.dim() != dimension) return false;
    for (int i = 0; i < dimension; i++) {
	// adjust upper/lower bounds for dimension i
	// assuming rect.Lo(i) <= rect.Hi(i)
	if (rect.lo(i) < lo(i)) {
	    lo(i) = rect.lo(i);
	}
	if (rect.hi(i) > hi(i)) {
	    hi(i) = rect.hi(i);
	}
    }
    return true;
}


bool
rt_rect::intersect(rt_rect & rect)
{
    if (rect.dim() != dimension) return false;
    for (int i = 0; i < dimension; i++) {
	// adjust upper/lower bounds for dimension i
	// assuming rect.Lo(i) <= rect.Hi(i)
	if (rect.lo(i) > lo(i)) lo(i) = rect.lo(i);
	if (rect.hi(i) < hi(i)) hi(i) = rect.hi(i);
    }
    return true;
}

bool 
rt_rect::overlaps(rt_rect & rect)
{
    if (rect.dim() != dimension) return false;

    for (int i = 0; i < dimension; i++) {
	// check each dimension for overlapping
	if (hi(i) < rect.lo(i) || lo(i) > rect.hi(i))  {
	    // non-overlapping in one-dimension
	    return false;
	}
    }
    // overlapping in all dimensions
    return true;
}

bool 
rt_rect::contains(rt_rect & rect)
{
    if (rect.dim() != dimension) return false;

    for (int i = 0; i < dimension; i++) {
	if (rect.lo(i) < lo(i) || rect.hi(i) > hi(i)) {
	    // out of range in dimension i
	    return false;
	}
    }
    return true;
}


bool 
rt_rect::contains(rt_point & pt)
{
    if (pt.dim() != dimension) return false;

    for (int i = 0; i < dimension; i++) {
	if (pt[i] > hi(i) || pt[i] < lo(i)) {
	    // out of range in dimension i
	    return false;
	}
    }
    return true;
}


double
rt_rect::dist(rt_rect *s) 
{
    assert(dim() == s->dim());

    double sum = 0;

    for (int i = 0; i < dim(); i++) {
	double ctr1 = hi(i) - lo(i);
	double ctr2 = s->hi(i) - s->lo(i);
	sum += pow(fabs(ctr1 - ctr2), (double) dim());
    }
    return(pow(sum, ((double) 1/((double) dim()))));
}

rt_query_t::~rt_query_t() 
{
    if (value != NULL) {
	if (argType == rt_pointarg) {
	    rt_point *pt =  (rt_point *) value;
	    pt->dealloc();
	    delete pt;
	} else {
	    rt_rect *rect =  (rt_rect *) value;
	    rect->dealloc();
	    delete rect;
	}
    }
}

rt_ext_t::rt_ext_t(CmpFctTbl tbl, ExpandFct exp, DistanceFct distance,
    DimFct d, RectFct rf)
{
    // copy function tables
    for (int i = 0; i < numLvl; i++) {
        for (int j = 0; j < rt_query_t::rt_numarg; j++) {
	    for (int k = 0; k < rt_query_t::rt_numoper; k++) {
	        cmpFcts[i][j][k] = tbl[i][j][k];
	    }
	}
    }
    expand = exp;
    dist = distance;
    dim = d;
    copyRect = rf;
}

static bool
alwaysTrue(const void *p, int l, const void *q)
{
    return true;
}

static bool
pt_equal_pt(const void *c1, int l, const void *p2)
{
    // create first point from data on disk page
    rt_point p1((int) l/sizeof(double), (const double *) c1);
    return (p1.isEqual(*((rt_point *) p2)));
}

static bool
pt_contained_rect(const void *c1, int l, const void *r2)
{
    rt_point p1((int) l/sizeof(double), (const double *) c1);
    return (((rt_rect *) r2)->contains(p1));
}

static bool
pt_equal_rect(const void *c1, int l, const void *r2)
{
    rt_point p1((int) l/sizeof(double), (const double *) c1);
    return (p1.contains(*((rt_rect *) r2)));
}

static bool
rect_contains_pt(const void *c1, int l, const void *p2)
{
    // create rect from data on disk page
    rt_rect r1((int) l/(2*sizeof(double)), (const double *) c1);
    return (r1.contains(*((rt_point *) p2)));
}

static bool 
rect_contains_rect(const void *c1, int l, const void *r2)
{
    rt_rect r1((int) l/(2*sizeof(double)), (const double *) c1);
    return (r1.contains(*((rt_rect *) r2)));
}

static bool
rect_overlaps_rect(const void *c1, int l, const void *r2)
{
    rt_rect r1((int) l/(2*sizeof(double)), (const double *) c1);
    return (r1.overlaps(*((rt_rect *) r2)));
}

rt_ext_t::CmpFctTbl pointTbl = {
    // level = 1 : predicates are points
    {
        // argType == point: nooper, equal, overlap, contains, contained
	{alwaysTrue, pt_equal_pt, pt_equal_pt, pt_equal_pt, pt_equal_pt},
	// argType == rect
	{alwaysTrue, pt_equal_rect, pt_contained_rect, pt_equal_rect, pt_contained_rect}
    },
    // level > 1: predicates are bounding rectangles
    {
        // argType == point: nooper, equal, overlap, contains, contained
	{alwaysTrue, rect_contains_pt, rect_contains_pt, rect_contains_pt, rect_contains_pt},
	// argType == rect
	{alwaysTrue, rect_contains_rect, rect_overlaps_rect, rect_contains_rect,
	    rect_overlaps_rect}
    }
};

static void
pt_expand(rt_rect &r, const void *pt, int len)
{
    rt_point p(len/sizeof(double), (double *) pt);
    r.expand(p);
}

static void
rect_expand(rt_rect &r, const void *rect, int len)
{
    rt_rect r2(len/(2*sizeof(double)), (const double *) rect);
    r.expand(r2);
}

static double
pt_dist(const void *pt1, const void *pt2, int len)
{
    rt_point p1(len/sizeof(double), (double *) pt1);
    rt_point p2(len/sizeof(double), (double *) pt2);
    return (p1.dist(&p2));
}

static double
rect_dist(const void *rect1, const void *rect2, int len)
{
    rt_rect r1(len/(2*sizeof(double)), (const double *) rect1);
    rt_rect r2(len/(2*sizeof(double)), (const double *) rect2);
    return (r1.dist(&r2));
}

static int
pt_dim(int sz)
{
    return (sz/sizeof(double));
}

static void 
rect_copyRect(rt_rect &rect, const void *r, int len)
{
    int d = len / (2 * sizeof(double));
    const double *coord = (const double *) r;
    for (int i = 0; i < d; i++) {
        rect.lo(i) = coord[2*i];
        rect.hi(i) = coord[2*i+1];
    }
}

static void
pt_copyRect(rt_rect &r, const void *pt, int len)
{
    int d = len / sizeof(double);
    const double *coord = (const double *) pt;
    for (int i = 0; i < d; i++) {
        r.lo(i) = coord[i];
        r.hi(i) = coord[i];
    }
}

static int
rect_dim(int sz)
{
    return (sz/(2*sizeof(double)));
}

rt_ext_t point_ext(pointTbl, &pt_expand, pt_dist, &pt_dim, &pt_copyRect);
gist_unorderedn_ext_t rt_point_ext(gist_ext_t::rt_point_ext_id, point_ext);

static bool
rect_equal_pt(const void *c1, int l, const void *p2)
{
    rt_rect r1((int) l/(2*sizeof(double)), (const double *) c1);
    return (((rt_point *) p2)->contains(r1));
}

static bool
rect_equal_rect(const void *c1, int l, const void *r2)
{
    rt_rect r1((int) l/(2*sizeof(double)), (const double *) c1);
    return (r1.isEqual(*((rt_rect *) r2)));
}

static bool
rect_overlap_rect(const void *c1, int l, const void *r2)
{
    rt_rect r1((int) l/(2*sizeof(double)), (const double *) c1);
    return (r1.overlaps(*((rt_rect *) r2)));
}

static bool
rect_contained_rect(const void *c1, int l, const void *r2)
{
    rt_rect r1((int) l/(2*sizeof(double)), (const double *) c1);
    return (((rt_rect *) r2)->contains(r1));
}

rt_ext_t::CmpFctTbl rectTbl = {
    // level = 1: predicates are rectangles
    {
        // argType = point: nooper, equal, overlap, contains, contained
	{alwaysTrue, rect_equal_pt, rect_contains_pt, rect_contains_pt, rect_equal_pt},
	// argType == rect
	{alwaysTrue, rect_equal_rect, rect_overlap_rect, rect_contains_rect,
	    rect_contained_rect}
    },
    // level > 1: predicates are bounding rectangles
    {
        // argType = point: nooper, equal, overlap, contains, contained
	{alwaysTrue, rect_contains_pt, rect_contains_pt, rect_contains_pt, rect_contains_pt},
	// argType == rect
	{alwaysTrue, rect_contains_rect, rect_overlap_rect, rect_contains_rect,
	    rect_overlap_rect}
    }
};

rt_ext_t rect_ext(rectTbl, rect_expand, rect_dist, rect_dim, rect_copyRect);
gist_unorderedn_ext_t rt_rect_ext(gist_ext_t::rt_point_ext_id, rect_ext);

bool
rt_ext_t::consistent(
    const void *query,
    const void *pred,
    int predLen,
    int level)
{
    rt_query_t *q = (rt_query_t *) query;

    return ((*cmpFcts[level == 1 ? 0 : 1][q->argType][q->oper])(pred, predLen, q->value));
}

void
rt_ext_t::penalty(
    const void *subtreePred,
    int predLen,
    int level,
    const void *newKey,
    int keyLen,
    gist_penalty_t &p)
{
    rt_rect r(predLen/(2*sizeof(double)), (const double *) subtreePred);
    rt_rect rect(r); // make a copy, so we dont overwrite data on the page
    double expansion = -1.0 * rect.span();
    expand(rect, newKey, keyLen);
    expansion += rect.span(); // expansion = after.span - before.span
    rect.dealloc();
    p.p = expansion > 0.0 ? expansion : 0.0;
}

void
rt_ext_t::union_key(
    void *bp,
    int &len,
    int level,
    const void *newPred,
    int newLen,
    bool &changed)
{
    rt_rect rect(len/(2*sizeof(double)), (double *) bp);
    rt_rect rOld(rect);
    if (level == 1) {
	expand(rect, newPred, newLen);
    } else {
        rt_rect rect2(newLen/(2*sizeof(double)), (double *) newPred);
	rect.expand(rect2);
    }
    changed = !rOld.isEqual(rect);
    rOld.dealloc();
}

void
rt_ext_t::union_page(
    void *bp,
    int &len,
    int level, 
    gist_predcursor_t &pcursor,
    bool &changed)
{
    int dim = len/(2*sizeof(double));
    rt_rect rect(dim, (double *) bp);
    rt_rect rOld(rect);

    for (int i = 0; i < pcursor.numElems; i++) {
	if (level == 1) {
	    // leaves contain points or rectangles
	    expand(rect, pcursor.elems[i].key, pcursor.elems[i].keyLen);
	} else {
	    // internal nodes contain rectangles
	    rt_rect r2(pcursor.elems[i].keyLen/(2*sizeof(double)),
	        (const double *) pcursor.elems[i].key);
	    rect.expand(r2);
	}
    }
    changed = !rOld.isEqual(rect);
    rOld.dealloc();
}

void
rt_ext_t::pickSplit(
    gist_predcursor_t &pcursor,
    int level,
    int rightEntries[] /*out*/,
    int &numRight /*out*/,
    const void *oldBp,
    int oldLen,
    void *leftBp /*out*/,
    int &leftLen /*in/out*/,
    void *rightBp /*out*/,
    int &rightLen /*int/out*/)
{
    int itemCnt = pcursor.numElems;
    int i;

    double maxDist = 0.0;
    int leftSeed, rightSeed;
    // find seeds (most distant items)
    for (i = 0; i < itemCnt; i++) {
        for (int j = 0; j < itemCnt; j++) {
	    if (i != j) {
		double d;
		if (level == 1) {
		    d  = (*dist)((void *) pcursor.elems[i].key,
		        (void *) pcursor.elems[j].key, pcursor.elems[i].keyLen);
		} else { // internal nodes have rects as predicates
		    d = rect_dist((void *) pcursor.elems[i].key,
		        (void *) pcursor.elems[i].key, pcursor.elems[i].keyLen);
		}
		if (d >= maxDist) {
		    // remember these two items
		    // (must be >= if they're all identical) 
		    maxDist = d;
		    leftSeed = i;
		    rightSeed = j;
		}
	    }
	}
    }

    // Create bounding rects from seeds.
    int dimen;
    if (level == 1) {
        dimen = dim(pcursor.elems[0].keyLen);
    } else {
        dimen = pcursor.elems[0].keyLen/(2*sizeof(double)); // always bounding rects
    }
    rt_rect leftCover(dimen, (double *) leftBp);
    leftLen = dimen * 2 * sizeof(double); // storage space for rect
    rt_rect rightCover(dimen, (double *) rightBp);
    rightLen = dimen * 2 * sizeof(double); // storage space for rect
    if (level == 1) {
	copyRect(leftCover, (void *) pcursor.elems[leftSeed].key, pcursor.elems[leftSeed].keyLen);
	copyRect(rightCover, (void *) pcursor.elems[rightSeed].key,
	    pcursor.elems[rightSeed].keyLen);
    } else { // copy rect to rect
	rect_copyRect(leftCover, (void *) pcursor.elems[leftSeed].key,
	    pcursor.elems[leftSeed].keyLen);
	rect_copyRect(rightCover, (void *) pcursor.elems[rightSeed].key,
	    pcursor.elems[rightSeed].keyLen);
    }

    // assign items to nodes (regardless of space util.)
    numRight = 0;
    for (i = 0; i < itemCnt; i++) {
        if (i == leftSeed) continue;
	if (i == rightSeed) {
	    rightEntries[numRight] = i;
	    numRight++;
	    if (level == 1) {
		expand(rightCover, (void *) pcursor.elems[i].key, pcursor.elems[i].keyLen);
	    } else {
		rect_expand(rightCover, (void *) pcursor.elems[i].key, pcursor.elems[i].keyLen);
	    }
	    continue;
	}

	// assign to node that results in smallest span
	rt_rect left(leftCover);
	rt_rect right(rightCover);
	if (level == 1) {
	    expand(left, (void *) pcursor.elems[i].key, pcursor.elems[i].keyLen);
	    expand(right, (void *) pcursor.elems[i].key, pcursor.elems[i].keyLen);
	} else { // expand BP with rect
	    rect_expand(left, (void *) pcursor.elems[i].key, pcursor.elems[i].keyLen);
	    rect_expand(right, (void *) pcursor.elems[i].key, pcursor.elems[i].keyLen);
	}
	rt_rect *toExpand;
	double lspan = left.span();
	double rspan = right.span();
	if (left.span() > right.span()) {
	    // assign to right node
	    toExpand = &rightCover;
	    rightEntries[numRight] = i;
	    numRight++;
	} else {
	    toExpand = &leftCover;
	}
	left.dealloc();
	right.dealloc();
	if (level == 1) {
	    expand(*toExpand, (void *) pcursor.elems[i].key, pcursor.elems[i].keyLen);
	} else {
	    rect_expand(*toExpand, (void *) pcursor.elems[i].key, pcursor.elems[i].keyLen);
	}
    }
}

#ifndef LIBGIST
void *
rt_ext_t::eqQuery(
    const void* key,
    int keyLen)
{
    // dirty trick here! we're banking on the fact that a rect 
    // has '2*dim' doubles, and a point only 'dim' doubles
    rt_query_t::rt_arg arg = (dim(2*sizeof(double)) == 1 ?
        rt_query_t::rt_rectarg : rt_query_t::rt_pointarg);

    // assumption: sizeof(rt_point) == sizeof(rt_rect)
    int combinedSize = sizeof(rt_query_t) + sizeof(rt_point);

    // More dirty tricks! EqQuery is supposed to return something that can be 
    // deallocated with a single free(), but we need something both for the query
    // and the argument. Solution: allocate a single chunk of memory for the two pieces.
    char* res = new char[combinedSize];
    rt_query_t *q = new (res) rt_query_t(rt_query_t::rt_equal, arg,
        res + sizeof(rt_query_t));
    int d = dim(keyLen);
    if (arg == rt_query_t::rt_pointarg) {
        rt_point* pt = new (res + sizeof(rt_query_t)) rt_point(d, (const double *) key);
    } else {
        rt_rect* r = new (res + sizeof(rt_query_t)) rt_rect(d, (const double *) key);
    }
    return (void *) q;
}
#endif

bool
rt_ext_t::check(
    const void *bp, 
    int bplen, 
    const void *pred, 
    int predlen, 
    int level)
{
    if (level > 1) {
	// predicates are guaranteed to be rectangles
	rt_rect rect2(predlen/(2*sizeof(double)), (const double *) pred);
	bool res = rect_contains_rect(bp, bplen, (const void *) &rect2);
	if (!res) {
	    int x = 1;
	}
	return res;
    } else {
        rt_rect br(bplen/(2*sizeof(double)), (const double *) bp);
	bool res =  (*cmpFcts[0][rt_query_t::rt_rectarg][rt_query_t::rt_contained])
	    (pred, predlen, (const void *) &br);
	if (!res) {
	    int x = 1;
	}
	return res;
    }
}

void
_printRect(const double *coord, int dim)
{
    printf("(");
    for (int i = 0; i < dim; i++) {
	printf("%f, %f", coord[2*i], coord[2*i+1]);
	if (i < dim - 1) printf(", ");
    }
    printf(")");
}

void
_printPt(const double *coord, int dim)
{
    printf("(");
    for (int i = 0; i < dim; i++) {
	printf("%f", coord[i]);
	if (i < dim - 1) printf(", ");
    }
    printf(")");
}

void
rt_ext_t::printPred(
    const void *pred,
    int plen,
    int level)
{
    if (level == 1) {
        // is this a rect or a pt?
	if (dim(plen) == (int) (plen/sizeof(double))) {
	    // it's a point
	    _printPt((const double *) pred, dim(plen));
	} else {
	    _printRect((const double *) pred, dim(plen));
	}
    } else {
        // we're dealing with rectangles
	int dim = plen / (2 * sizeof(double));
	_printRect((const double *) pred, dim);
    }
}

void
rt_ext_t::printData(
    const void *data,
    int dlen)
{
    printf("%d", *((int *) data));
}

