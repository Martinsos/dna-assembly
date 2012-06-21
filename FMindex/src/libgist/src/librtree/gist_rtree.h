//	gist_rtree.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/librtree/gist_rtree.h,v 1.2 1997/12/05 23:09:31 marcel Exp $

#ifdef __GNUG__
#pragma interface
#endif

#ifndef GIST_RTREE_H
#define GIST_RTREE_H

#include "gist.h"
#include "gist_ext.h"
#include "gist_unordered.h"
#include "gist_unorderedn.h"

class rt_point;

class rt_rect {
public:

    rt_rect(int d); // preset with 0 coords
    rt_rect(int d, const double *coord); // coords left as-is
    rt_rect(int d, double *coord); // coords left as-is, can be changed
    rt_rect(rt_rect &r);
    ~rt_rect();

    void dealloc();
    bool isEqual(rt_rect & k);
    int dim() { return dimension; }
    double span();
    double margin();
    double dist(rt_rect *s);
    double & lo(int i);
    double & hi(int i);

    // expanding this rect to include another point or rect
    // return TRUE if successful, FALSE otherwise
    bool expand(rt_point & pt);
    bool expand(rt_rect & rect);

    // restricting this rect to its overlap with another rect
    // return TRUE if successful, FALSE otherwise
    bool intersect(rt_rect & rect);

    // test relationship with another rect or point:
    bool overlaps(rt_rect & rect);
    bool contains(rt_rect & rect);
    bool contains(rt_point & pt);

    int dimension; 
    double *coord;
};

class rt_point {
public:

    rt_point(int d);
    rt_point(int d, double *coord) : dimension(d), coord(coord) {}
    rt_point(int d, const double *coord) : dimension(d), coord((double *) coord) {}
    ~rt_point();

    // accessors:
    double & operator [] (int i) { 
	assert(i>=0 && i<dimension);
	return coord[i];
    }

    void dealloc() { delete [] coord; }
    bool isEqual(rt_point & k);
    int dim() { return dimension; }
    double span() { return 0; }
    double margin() { return 0; }
    double dist(rt_point *s);
    double & crd(int i) { assert(i>=0 && i<dimension); return coord[i]; }
    //double & lo(int i) { return crd(i); }
    //double & hi(int i) { return crd(i); }

    // test relationship with another point or rect
    bool contains(rt_rect & rect);
    bool contains(rt_point & pt);

    int dimension;
    double *coord;  // keep coordinates in all dimensions
};

class rt_query_t {
public:
    enum rt_oper {
        rt_nooper,
	rt_equal,
	rt_overlap,
	rt_contains,
	rt_contained,
	rt_numoper
    };

    enum rt_arg {
        rt_pointarg,
	rt_rectarg,
	rt_numarg
    };

    // assumes that points and rects were created with new(dim), not new(dim, coord)!
    rt_query_t(rt_oper oper, rt_arg arg, void *val) : oper(oper), argType(arg), value(val) {}
    ~rt_query_t() ;

    rt_oper oper;
    rt_arg argType;
    void *value;
};

class rt_ext_t : public gist_unordered_ext_t {
public:

    enum { // MSVC 5.0 thinks that "const int ..." is illegal
        numLvl = 2 // only 2 levels to distinguish: leaf/non-leaf
    };

    typedef bool (*CmpFct)(const void *, int, const void *);
        // any point/rect comparison function

    // function table: one for each operator, possible argument type
    // and leaf/internal level
    typedef CmpFct CmpFctTbl[numLvl][rt_query_t::rt_numarg][rt_query_t::rt_numoper];

    CmpFctTbl cmpFcts; // for consistent()

    // for penalty()/pickSplit()/union_key():
    // expand rect with rect/pt
    typedef void (*ExpandFct)(rt_rect &r, const void *item, int len); 
    ExpandFct expand;

    // for pickSplit()
    typedef double (*DistanceFct)(const void *item1, const void *item2, int len);
    DistanceFct dist; // compute the distance between two rects/pts
    typedef int (*DimFct)(int dataSz); // returns dimension of points/rects
    DimFct dim;
    typedef void (*RectFct)(rt_rect &r, const void *item, int len);
    RectFct copyRect; // creates rect from pt/rect

    rt_ext_t(CmpFctTbl tbl, ExpandFct exp, DistanceFct distance, DimFct dim,
       RectFct rf);

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
	int rightEntries[] /*out*/,
	int &numRightEntries /*out*/,
	const void *oldBp,
	int oldLen,
	void *leftBp /*out*/,
	int &leftLen /*in/out*/,
	void *rightBp /*out*/,
	int &rightLen /*int/out*/);

#ifndef LIBGIST
    void* eqQuery(
        const void* key,
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

extern gist_unorderedn_ext_t rt_point_ext;
extern gist_unorderedn_ext_t rt_rect_ext;

#endif // RTREE_H
