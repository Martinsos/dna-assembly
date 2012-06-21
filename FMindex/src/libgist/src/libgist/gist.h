//	gist.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist.h,v 1.2 1997/12/05 23:08:37 marcel Exp $


#ifndef GIST_H
#define GIST_H


#ifdef __GNUG__
#pragma interface
#endif

#include "gist_cursor.h"
#include "gist_p.h"
#include "gist_ext.h"
#include "gist_ustk.h"
#include "gist_defs.h" // global constants

#ifdef LIBGIST
#include "vec_t.h" 
#endif

class gist_cursor_t;
class gist_lstk;
class gist_ustk;
class gist_p;


#ifdef LIBGIST
class gist_m {
#else // Shore code
class gist_m : public smlevel_2 {
#endif

public:

#ifndef LIBGIST
    gist_m() {};
    ~gist_m() {};

    // SHORE interface
    static rc_t 		create(
        stid_t 			    stid,
	lpid_t& 		    root);

    static rc_t 		insert(
	const lpid_t& 		    root,
	gist_ext_t*		    ext,
	const vec_t& 		    key, 
	const vec_t& 		    elem);

    static rc_t 		remove(
        const lpid_t&		    root,
	gist_ext_t*		    ext,
	const vec_t& 		    key,
	const vec_t& 		    elem);

    static rc_t 		print(
        const lpid_t&		    root,
	gist_ext_t*		    ext);

    static rc_t			fetch_init(
	const lpid_t& 		    root,
	gist_ext_t*		    ext,
	gist_cursor_t& 		    cursor,
	const void*		    query);

    static rc_t			fetch(
	gist_cursor_t& 		    cursor,
	void* 		    	    key,
	smsize_t&		    klen,
	void* 			    el,
	smsize_t&		    elen, 
	bool& 		    	    eof);

    static bool 		is_empty(
        const lpid_t& 		    root);

#else // LIBGIST
    gist_m();
    ~gist_m();

    // create an empty Gist
    rc_t			create(
	const char*		    filename,
	gist_ext_t*		    ext);

    // open an existing Gist
    rc_t			open(
	const char*		    filename,
	gist_ext_t*		    ext);

    rc_t			close();
    rc_t			flush(); 

    // insert a single key
    rc_t			insert(
	const void*		    key, 
	const int	    	    keyLen,
	const void*		    dataPtr,
	const int		    dataPtrLen);

    // remove all keys matching the query
    rc_t			remove(
	const void*	    	    query);

    // initialize the cursor
    rc_t			fetch_init(
	gist_cursor_t& 	    	    cursor,
	const void*		    query);

    // fetch the next data item
    rc_t			fetch(
	gist_cursor_t& 	    	    cursor,
	void*			    key,
	smsize_t&		    keyLen,
	void*			    dataPtr,
	smsize_t&		    dataPtrLen,
	bool& 	   	 	    eof);

    // returns true if tree contains no data
    bool 			is_empty();

    // checks structural and data integrity of the tree;
    // outputs violations to stderr
    rc_t 			check();

    // dumps the content of a page to stdout;
    // if 0 is specified as the pgno, dumps out the tree
    rc_t 			dump(
        shpid_t 	    	    pgno = 0);

protected: 
    // State
    gist_ext_t *ext;
    bool isOpen; // true if opened successfully
    gist_file file;

#define rootNo 1
    //const shpid_t rootNo = 1;

#endif // ifndef LIBGIST

private:

    // 
    enum {
        bpUpdated = 0x01,
	itemInserted = 0x02,
	leftItemUpdated = 0x04,
	rightItemInserted = 0x08
    };

    // 8-byte aligned temp storage for arbitrary predicate
    union AlignedPred {
        double d;
	char pred[gist_p::max_tup_sz];
    };

#ifndef LIBGIST
    static
#endif
    rc_t 			_locate_leaf(
	const lpid_t& 		    root,
	gist_ext_t*		    ext,
	gist_ustk& 		    stack,
	const vec_t&		    key,
	const vec_t&		    data);

#ifndef LIBGIST
    static
#endif
    rc_t 			_fetch(
	gist_cursor_t&		    cursor,
	void*			    key,
	smsize_t&		    keyLen,
	void*		  	    dataPtr,
	smsize_t&		    dataPtrLen,
	shpid_t&		    leafNo,
	int&			    idx,
	bool&			    eof);

#ifndef LIBGIST
    static
#endif
    rc_t 			_insert_leaf(
	gist_ext_t* 		    ext,
	gist_p&			    page,
	const vec_t&		    key,
	const vec_t&		    data,
	bool&			    bpChanged);

#ifndef LIBGIST
    static
#endif
    rc_t 			_apply_update(
	gist_ext_t*		    ext,
	gist_p&			    page,
	int& 			    leftIdx,
	const vec_t&		    leftBp,
	const vec_t&		    rightBp,
	shpid_t 		    rightChild,
	bool&			    bpChanged);

#ifndef LIBGIST
    static
#endif
    rc_t 			_split_node(
	gist_ext_t*		    ext,
	gist_p&			    left,
	gist_p&			    right,
	int 			    rightEntries[],
	int 			    numRight,
	bool 			    leftHasBp,
	cvec_t&			    leftBp,
	cvec_t&			    rightBp);

#ifndef LIBGIST
    static
#endif
    rc_t 			_split(
	const lpid_t& 		    root,
	gist_ext_t*		    ext,
	gist_p&			    page,
	gist_ustk&		    stack,
	int 			    stkIdx,
	int 			    rightEntries[], 
	int 			    numRight, 
	gist_p&			    rightPage,
	vec_t&			    leftBp,
	vec_t&			    rightBp);

#ifndef LIBGIST
    static
#endif
    rc_t 			_update_parent(
	const lpid_t& 		    root,
	gist_ext_t*		    ext,
	gist_ustk&		    stack,
	int 			    stkIdx,
	const vec_t&		    leftBp,
	const vec_t&		    rightBp,
	shpid_t 		    rightChild);

#ifndef LIBGIST
    static
#endif
    rc_t 			_new_page(
	const lpid_t& 		    root,
	const lpid_t& 		    closeby,
        gist_p&			    p,
	int2 			    level);

#ifndef LIBGIST
    static
#endif
    rc_t 			_fix_page(
        gist_p&			    page,
        lpid_t 			    pageNo,
	latch_mode_t		    mode);

#ifndef LIBGIST
    static
#endif
    void 			_unfix_page(
        gist_p&			    page);

#ifdef LIBGIST
    struct reachInfo {
	shpid_t parent;
        int count;
    };

    void 			_check_node(
        gist_p&			    node);

    rc_t 			_check_sub(
        shpid_t 		    pgno,
	const vec_t&		    pred,
	shpid_t			    parent,
	int			    slot,
	reachInfo 		    reached[]);
#endif

#ifndef LIBGIST
    static
#endif
    void 			_dump_pg(
	gist_ext_t*		    ext,
        gist_p&			    page);

#ifndef LIBGIST
    static
#endif
    rc_t 			_dump_sub(
	gist_ext_t*		    ext,
        lpid_t 		    	    pgno);

};

#endif // GIST_H
