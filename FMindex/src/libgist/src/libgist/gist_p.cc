//	gist_p.cc
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist_p.cc,v 1.2 1997/12/05 23:08:47 marcel Exp $

#ifdef __GNUG__
#pragma implementation "gist_p.h"
#endif

#ifdef LIBGIST
#include "gist_defs.h"
#else 
#define SM_SOURCE
#define GIST_C
#include <sm_int_2.h>
#endif

#include "gist_p.h"

#ifndef LIBGIST
#include <page.h>

MAKEPAGECODE(gist_p, keyed_p);

void
gist_p::ntoh()
{
    /*
     *  BUGBUG -- fill in when appropriate 
     */
    W_FATAL(eINTERNAL);
}

rc_t
gist_p::format(
    const lpid_t& 	pid,
    tag_t 		tag, 
    uint4_t 		flags)
{
    gistctrl_t hdr;
    hdr.level = 1;
    vec_t v(&hdr, sizeof(hdr));
    W_DO(keyed_p::format(pid, tag, flags, v));
    return RCOK;
}
#endif

gistctrl_t::gistctrl_t()
{
}

rc_t 		
gist_p::format(
    const lpid_t 	    pid,
    const gistctrl_t	    *hdr)
{
#ifdef LIBGIST
    (void) memset(_pp, '\017', sizeof(*_pp)); // trash the whole page

    _pp->pid = pid;
    _pp->space.init((int) (data_sz + 2*sizeof(slot_t)));
    _pp->end = _pp->nslots = _pp->nvacant = 0;
    cvec_t hdrV((void *) hdr, sizeof(*hdr));
    insert_expand(0, 1, &hdrV);
    this->descr->isDirty = true;

    return RCOK;
#else // Shore code
    cvec_t hdrv((void *) hdr, sizeof(*hdr));
    W_DO(keyed_p::format(pid, t_gist_p, t_virgin, hdrv));
    return RCOK;
#endif
}

rc_t		
gist_p::insert(const keyrec_t &tup)
{
    cvec_t key(tup.key(), tup.klen());
    cvec_t data(tup.elem(), tup.elen());
    shpid_t child = tup.child();
    return (insert(key, data, nrecs(), child));
}

rc_t	
gist_p::insert(
    const cvec_t& 	    key, 
    const cvec_t& 	    el, 
    int 		    slot, 
    shpid_t 		    child)
{
#ifdef LIBGIST
    keyrec_t::hdr_s hdr;
    hdr.klen = key.size();
    hdr.elen = el.size();
    hdr.child = child;
    cvec_t v(&hdr, sizeof(hdr));
    v.put(key);
    v.put(el);
    // non-root node: slot also corrected for BP, not just for header
    int correction = (this->is_root() ? 1 : 2);
    W_DO(insert_expand(slot + correction, 1, &v));
    this->descr->isDirty = true;
    return RCOK;
#else
    int correction = (this->is_root() ? 0 : 1);
    return (keyed_p::insert(key, el, slot + correction, child));
#endif
}

bool
gist_p::is_root() const
{
    gistctrl_t *hdr = (gistctrl_t *) get_hdr();
    return (_pp->pid ==  hdr->root);
}

rc_t		
gist_p::remove(int slot)
{
#ifdef LIBGIST
    // non-root node: slot also corrected for BP, not just for header
    int correction = (this->is_root() ? 1 : 2);
    W_DO(remove_compress(slot + correction, 1));
    this->descr->isDirty = true;
    return RCOK;
#else
    int correction = (this->is_root() ? 0 : 1);
    return (keyed_p::remove(slot + correction));
#endif
}

rc_t		
gist_p::copy(gist_p& rsib)
{
#ifdef LIBGIST
    int n = nrecs(); // no BP to take into account: this is the root
    cvec_t* tp = new cvec_t[n];
    assert(tp);
    
    for (int i = 0; i < n; i++) {
	tp[i].put(tuple_addr(1 + i), tuple_size(1 + i));
    }

    //  insert all of tp into slot 1 of rsib 
    //  (slot 0 reserved for header)
    rc_t rc = rsib.insert_expand(1, n, tp);
    delete[] tp;
    this->descr->isDirty = true;
    rsib.descr->isDirty = true;
    return rc;
#else // Shore code
    return(keyed_p::shift(0, &rsib));
#endif
}

rc_t	
gist_p::set_hdr(const gistctrl_t& new_hdr)
{
    cvec_t hdrV(&new_hdr, sizeof(new_hdr));
    W_DO(overwrite(0, 0, hdrV));
    return RCOK;
}

bool 		
gist_p::is_leaf() const
{
    return level() == 1;
}

bool 	
gist_p::is_node() const
{
    return !is_leaf();
}

int 		
gist_p::level() const
{
    gistctrl_t *hdr = (gistctrl_t *) get_hdr();
    return hdr->level;
}
    

int
gist_p::rec_size(int idx) const
{
    // non-root node: slot also corrected for BP, not just for header
    int correction = (this->is_root() ? 1 : 2);
#ifdef LIBGIST
    return tuple_size(idx + correction);
#else 
    return page_p::tuple_size(idx + correction);
#endif
}

int 
gist_p::nrecs() const
{
    // non-root node: slot also corrected for BP, not just for header
    int correction = (this->is_root() ? 1 : 2);
    return _pp->nslots - correction;
}


const keyrec_t& 
gist_p::rec(int idx) const
{
    // non-root node: slot also corrected for BP, not just for header
    int correction = (this->is_root() ? 1 : 2);
#ifdef LIBGIST
    return * (keyrec_t*) tuple_addr(idx + correction);
#else
    return * (keyrec_t*) page_p::tuple_addr(idx + correction);
#endif
}

#ifdef LIBGIST
gist_p::~gist_p()
{
}

gist_p::gist_p() : _pp(0), descr(NULL)
{
}

const void* 	
gist_p::get_hdr() const
{
    return tuple_addr(0);
}

int
page_s::space_t::nfree() const
{
    return _nfree;
}

int
page_s::space_t::usable()
{
    return _nfree;
}

rc_t		
page_s::space_t::acquire(int amt, int slot_bytes)
{
    int avail = usable();
    int total = amt + slot_bytes;

    if (avail < total)  {
	return eRECWONTFIT;
    }
    
    //  Consume the space
    assert(_nfree >= total);
    _nfree -= total;
    return RCOK;
}

void 		
page_s::space_t::release(int amt)
{
    _nfree += amt;
}

const lpid_t&
gist_p::pid() const
{
    return _pp->pid;
}

smsize_t
gist_p::usable_space()
{
    return _pp->space.usable(); 
}

gist_p& 		
gist_p::operator=(const gist_p& p)
{
    _pp = p._pp;
    descr = p.descr;
    return *this;
}

rc_t		
gist_p::insert_expand(
    int 			    idx,
    int 			    cnt, 
    const cvec_t 		    vec[])
{
    assert(idx >= 0 && idx <= _pp->nslots);
    assert(cnt > 0);

    //  Compute the total # bytes needed 
    uint4 total = 0;
    int i;
    for (i = 0; i < cnt; i++)  {
	total += int(align(vec[i].size()) + sizeof(slot_t));
    }

    //  Try to get the space ... could fail with eRECWONTFIT
    W_DO(_pp->space.acquire(total, 0));

    if (contig_space() < total)  {
	_compress();
	assert(contig_space() >= total);
    }

    if (idx != _pp->nslots)    {
	//  Shift left neighbor slots further to the left
	memcpy(&_pp->slot[-(_pp->nslots + cnt - 1)],
	       &_pp->slot[-(_pp->nslots - 1)], 
	       (_pp->nslots - idx) * sizeof(slot_t));
    }

    //  Fill up the slots and data
    register slot_t* p = &_pp->slot[-idx];
    for (i = 0; i < cnt; i++, p--)  {
	p->offset = _pp->end;
	p->length = vec[i].copy_to(_pp->data + p->offset);
	_pp->end += int(align(p->length));
    }

    _pp->nslots += cnt;
    
    return RCOK;
}
    
rc_t		
gist_p::remove_compress(int idx, int cnt)
{
    assert(idx >= 0 && idx < _pp->nslots);
    assert(cnt > 0 && cnt + idx <= _pp->nslots);

    //	Compute space space occupied by tuples
    register slot_t* p = &_pp->slot[-idx];
    register slot_t* q = &_pp->slot[-(idx + cnt)];
    int amt_freed = 0;
    for ( ; p != q; p--)  {
	assert(p->length < gist_p::max_tup_sz);
	amt_freed += int(align(p->length) + sizeof(slot_t));
    }

    //	Compress slot array
    p = &_pp->slot[-idx];
    q = &_pp->slot[-(idx + cnt)];
    for (slot_t* e = &_pp->slot[-_pp->nslots]; q != e; p--, q--) *p = *q;
    _pp->nslots -= cnt;

    //  Free space occupied
    _pp->space.release(amt_freed);

    return RCOK;
}

rc_t		
gist_p::overwrite(
    int 			    idx,
    int 			    start,
    const cvec_t& 		    vec)
{
    int vecsz = vec.size();
    assert(idx >= 0 && idx < _pp->nslots);
    assert(start >= 0 && vecsz >= 0);

    slot_t& s = _pp->slot[-idx];		// slot in question
    assert(start + vecsz <= s.length);

    //  Put data into the slot
    char* p = _pp->data + s.offset;
    if (vecsz > 0)  {
	assert((int)(s.offset + start + vec.size() <= data_sz));
	// make sure the slot table isn't getting overrun
	assert((char *)(p + start + vec.size()) <= (char *)&_pp->slot[-(_pp->nslots-1)]);
		
	vec.copy_to(p + start);
    }
    return RCOK;
}

void		
gist_p::_compress(int idx)
{

    assert(idx < 0 || idx < _pp->nslots);
    
    //  Copy data area over to scratch
    static char scratch[sizeof(_pp->data)];
    memcpy(&scratch, _pp->data, sizeof(_pp->data));

    //  Move data back without leaving holes
    register char* p = _pp->data;
    int nslots = _pp->nslots;
    for (register i = 0; i < nslots; i++) {
	if (i == idx)  continue; 	// ignore this slot for now
	slot_t& s = _pp->slot[-i];
	if (s.offset != -1)  {
	    assert(s.offset >= 0);
	    memcpy(p, scratch+s.offset, s.length);
	    s.offset = p - _pp->data;
	    p += align(s.length);
	}
    }

    //  Move specified slot
    if (idx >= 0)  {
	slot_t& s = _pp->slot[-idx];
	if (s.offset != -1) {
	    assert(s.offset >= 0);
	    memcpy(p, scratch + s.offset, s.length);
	    s.offset = p - _pp->data;
	    p += align(s.length);
	}
    }

    _pp->end = p - _pp->data;

    //  Page is now compressed with a hole after _pp->end.
}

void*
gist_p::tuple_addr(int idx) const
{
    assert(idx >= 0 && idx < _pp->nslots);
    return (void*) (_pp->data + _pp->slot[-idx].offset);
}

smsize_t
gist_p::tuple_size(int idx) const
{
    assert(idx >= 0 && idx < _pp->nslots);
    return _pp->slot[-idx].length;
}

void
page_s::space_t::init(int nfree) 
{ 
    _nfree = nfree;
}

smsize_t
gist_p::contig_space()	
{ 
    return ((char*) &_pp->slot[-(_pp->nslots-1)]) - (_pp->data + _pp->end); 
}

const char*
keyrec_t::key() const
{
    return _body();
}

const char* 
keyrec_t::elem() const
{
    return _body() + _hdr.klen;
}

const char*
keyrec_t::sep() const
{
    return _body();
}

smsize_t
keyrec_t::klen() const 
{
    return _hdr.klen;
}

smsize_t 
keyrec_t::elen() const
{
    return _hdr.elen;
}

smsize_t 
keyrec_t::slen() const
{
    return _hdr.klen + _hdr.elen;
}

smsize_t
keyrec_t::rlen() const
{
    return _body() + slen() - (char*) this;
}

shpid_t
keyrec_t::child() const
{
    return _hdr.child;
}

bool
gist_p::is_fixed() const
{
    return _pp != 0;
}

char*
keyrec_t::_body() const
{
    return ((char*) &_hdr) + sizeof(_hdr);
}
#endif // ifdef LIBGIST

