//	gist_p.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist_p.h,v 1.1.1.1 1997/11/13 02:43:57 marcel Exp $


#ifndef GIST_P_H
#define GIST_P_H

#ifdef __GNUG__
#pragma interface
#endif

#ifdef LIBGIST

#include "gist_defs.h"
#include "vec_t.h"
#include "gist_file.h"

#else // Shore code

#include <keyed.h>

#endif // ifdef LIBGIST

class gist_m;

#ifdef LIBGIST
/*
 * Basic page structure for all pages.
 */
struct page_s {
    struct slot_t {
	int2 offset;		// -1 if vacant
	uint2 length;
    };

    class space_t {
    public:
	space_t()	{};
	~space_t()	{};

	void init(int);
	int nfree() const;
	
	int			usable();
				// slot_bytes means bytes for new slots
	rc_t			acquire(int amt, int slot_bytes);
	void 			release(int amt);

    private:
   
	void _check_reserve();
	
	int2	_nfree;		// free space counter
    };

    enum {
	data_sz = (SM_PAGESIZE 
		   - sizeof(lpid_t)
		   - sizeof(space_t)
		   - 3 * sizeof(int2)
		   - 2 * sizeof(slot_t)
		   - 2 * sizeof(int2)),
	max_slot = data_sz / sizeof(slot_t) + 2
    };

    lpid_t	pid;			// id of the page
    space_t 	space;			// space management
    uint2	end;			// offset to end of data area
    int2	nslots;			// number of slots
    int2	nvacant;		// number of vacant slots
    char 	data[data_sz];		// must be aligned
    slot_t	reserved_slot[1];	// 2nd slot (declared to align
					// end of _data)
    slot_t	slot[1];		// 1st slot
};

class keyrec_t {
public:
    struct hdr_s {
	uint2	klen;
	uint2	elen;
	shpid_t	child;
    };

    const char* key() const;
    const char* elem() const;
    const char* sep() const;
    smsize_t klen() const;
    smsize_t elen() const;
    smsize_t slen() const;
    smsize_t rlen() const;
    shpid_t child() const;
    
private:
    hdr_s 	_hdr;
    char*	_body()	const;
};

#endif // ifdef LIBGIST

struct gistctrl_t {
    lpid_t	root;
    int2	level;	// leaf if 1, non-leaf if > 1

    gistctrl_t();
};



#ifdef LIBGIST
class gist_p {
#else // Shore code
class gist_p : public keyed_p {
#endif
public:
    
#ifdef LIBGIST
    typedef page_s::slot_t slot_t;

    enum {
	data_sz = page_s::data_sz,
	max_slot = data_sz / sizeof(slot_t) + 2
    };
#endif

    enum {
	max_tup_sz = data_sz / 3 - sizeof(slot_t) - sizeof(keyrec_t),
	    // 1 BP and 2 entries minimum
	max_scnt = (data_sz - sizeof(gistctrl_t)) / 
	    (sizeof(keyrec_t) + sizeof(slot_t)) + 1
	    // max # of slots on a page
    };

#ifdef LIBGIST
    				gist_p();
    				~gist_p();
#else
    MAKEPAGE(gist_p, keyed_p, 1);
#endif

    rc_t 			format(
	const lpid_t 		    pid,
	const gistctrl_t	    *hdr);

    rc_t			insert(
        const keyrec_t		    &tup);

    rc_t			copy(gist_p& rsib);

    rc_t			set_hdr(const gistctrl_t& new_hdr);
    int 			level() const;
    void			root(lpid_t& r) const;
    shpid_t			root() const;

    bool 			is_leaf() const;
    bool 			is_node() const;
    bool			is_root() const;

    
    // Gist-specific record access:
    // masks the BP, so that the first entry on the page has idx = 0;
    // the BP can be accessed with idx = -1
    enum { // MSVC 5.0 thinks "const int ..." is illegal
        bpSlot = -1
    };
    const keyrec_t& 		rec(int idx) const;
    int 			rec_size(int idx) const;
    int 			nrecs() const;

    // the slot index is automatically corrected if a BP is present on the page
    rc_t			insert(
	const cvec_t& 		    key, 
	const cvec_t& 		    el, 
	int 			    slot, 
	shpid_t 		    child = 0);
    rc_t			remove(int slot);

#ifdef LIBGIST
    //
    // functions provided by one of gist_p's ancestor classes (page_p, keyed_p) :
    //

public:

    bool 			is_fixed() const;
    gist_p& 			operator=(const gist_p& p);
    smsize_t			usable_space();
    const lpid_t& 		pid() const;

private:
    rc_t			insert_expand(
	int 			    idx,
	int 			    cnt, 
	const cvec_t 		    tp[]);
    
    rc_t			remove_compress(int idx, int cnt);
    rc_t			overwrite(
	int 			    idx,
	int 			    start,
	const cvec_t& 		    data);

    // state
    page_s*                     _pp;
    gist_file::page_descr*	descr;

    friend class gist_m;
    // what I really meant was:
    //friend rc_t gist_m::_fix_page(gist_p &page, shpid_t pageNo);
    //friend rc_t gist_m::_unfix_page(gist_p &page);

    void			_compress(int idx = -1);

    // formerly from page_p
    inline smsize_t		used_space();
    smsize_t			contig_space();
    
    smsize_t 			tuple_size(int idx) const;
    void* 			tuple_addr(int idx) const;
    inline bool 		is_tuple_valid(int idx) const;
    const void* 		get_hdr() const;


#endif // ifdef LIBGIST

};

#ifdef LIBGIST
inline smsize_t 
gist_p::used_space()
{
    return (data_sz + 2 * sizeof(slot_t) - _pp->space.usable()); 
}

inline bool
gist_p::is_tuple_valid(int idx) const
{
    return idx >= 0 && idx < _pp->nslots && _pp->slot[-idx].offset >=0;
}

#endif // ifdef LIBGIST

inline void
gist_p::root(lpid_t& r) const
{
    gistctrl_t* hdr = (gistctrl_t *) get_hdr();
    r = hdr->root;
}

inline shpid_t
gist_p::root() const
{
    gistctrl_t* hdr = (gistctrl_t *) get_hdr();
    return hdr->root.page;
}

#endif /*GIST_P_H*/
