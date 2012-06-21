//	gist_defs.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/gist_defs.h,v 1.2 1997/12/05 23:08:38 marcel Exp $


#ifndef GIST_DEFS_H
#define GIST_DEFS_H

#ifdef LIBGIST

#include <assert.h>


#define w_assert3(x) assert((x))
#define RC(x) (x)
#define RC_AUGMENT(x) (x)
#define _err_num(x) (x)

#define GISTBUFS 7

enum latch_mode_t { LATCH_NL = 0, LATCH_SH = 1, LATCH_EX = 2 };

typedef short   int2;
typedef long    int4;
typedef unsigned char	uint1;
typedef unsigned short	uint2;
typedef unsigned long	uint4;
typedef uint4 smsize_t;

#ifdef NO_BOOL
typedef char            bool;
const char false = '\0';
const char true = '\1';
#endif

typedef uint2    snum_t;
typedef unsigned int shpid_t;
typedef int rc_t;
#define RCOK 0
#define SM_PAGESIZE 8192
#define ROOTPAGE 1
#define eEOF 0x80014
#define eBADSLOTNUMBER 0x8001d
#define eRECWONTFIT 0x80023
#define eNOTOPEN 0x80024
#define eFILEERROR 0x80025
#define eNOBUFFERS 0x80026
#define eOUTOFBOUNDS 0x80027
#define eOUTOFSPACE 0x80005

#define ALIGNON 0x8
#define ALIGNON1 (ALIGNON-1)
#define align(sz) ((uint4)((sz + ALIGNON1) & ~ALIGNON1))

#define W_DO(x) 	\
{			\
    rc_t _e = (x);	\
    if (_e) return _e;	\
}
#define X_DO(x, anchor)	W_DO(x)
    

class lsn_t {
private:
    uint4	_file;		// log file number in log directory
    uint4	_rba;		// relative byte address of (first
				// byte) record in file
};

struct vid_t {
    uint2 vol;
    vid_t(uint2 v) : vol(v) {}
    friend bool operator==(const vid_t& v1, const vid_t& v2)  {
	return v1.vol == v2.vol;
    }
};

struct stid_t {
    vid_t	vol;
    snum_t	store;
    stid_t();
    stid_t(vid_t vid, snum_t snum);
    bool operator==(const stid_t& s) const;
};

inline stid_t::stid_t() : vol(0), store(0)
{}

inline stid_t::stid_t(vid_t v, snum_t s) : vol(v), store(s)
{}

inline bool stid_t::operator==(const stid_t& s) const
{
    return (vol == s.vol) && (store == s.store);
}

struct lpid_t {
    stid_t	_stid;
    shpid_t	page;
    
    lpid_t();
    lpid_t(const stid_t& s, shpid_t p);
    lpid_t(vid_t v, snum_t s, shpid_t p);

    //vid_t	vol()   const {return _stid.vol;}
    //snum_t	store() const {return _stid.store;}
    const stid_t& stid() const {return _stid;}

    bool operator==(const lpid_t& p) const;
    //bool operator!=(const lpid_t& p) const;
    //bool operator<(const lpid_t& p) const;
    //bool operator<=(const lpid_t& p) const;
    //bool operator>(const lpid_t& p) const;
    //bool operator>=(const lpid_t& p) const;
};

inline lpid_t::lpid_t() : page(0) {}

inline lpid_t::lpid_t(const stid_t& s, shpid_t p) : _stid(s), page(p)
{}

inline lpid_t::lpid_t(vid_t v, snum_t s, shpid_t p) :
	_stid(v, s), page(p)
{}

inline bool lpid_t::operator==(const lpid_t& p) const
{
    return (page == p.page) && (stid() == p.stid());
}



struct slot_t {
    int2 offset;
    uint2 length;
};

struct tid_t {
    uint4 hi, lo;
};

#if 0
class page_s_space_t {
public:
    tid_t	_tid;		// youngest xct contributing to _nrsvd
    int2	_nfree;		// free space counter
    int2	_nrsvd;		// reserved space counter
    int2	_xct_rsvd;	// amt of space contributed by _tid to _nrsvd
    int2	_rflag;
};

enum {
    data_sz = (SM_PAGESIZE - 
	       2 * sizeof(lsn_t) 
	       - sizeof(lpid_t) -
	       2 * sizeof(shpid_t) 
	       - sizeof(page_s_space_t) -
	       4 * sizeof(int2) - 
	       2 * sizeof(int4) -
	       2 * sizeof(slot_t)),
    max_slot = data_sz / sizeof(slot_t) + 2
};
#endif

#else // and now for Shore

#define _err_num(x) ((x).err_num())

#endif

#endif // GIST_DEFS_H
