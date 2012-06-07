//	vec_t.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/vec_t.h,v 1.1.1.1 1997/11/13 02:43:58 marcel Exp $


#ifndef VEC_T_H
#define VEC_T_H

#include "gist_defs.h"
#include <string.h>

#define MAX_SMALL_VEC_SIZE 8

#define CADDR_T const unsigned char *

struct vec_pair_t {
    CADDR_T	ptr;
    size_t	len;
};

struct VEC_t {
    int		_cnt;
    size_t	_size;
    vec_pair_t*	_base;	// pointer to beginning of _pair or malloced
			// space
    vec_pair_t	_pair[MAX_SMALL_VEC_SIZE];
};

class cvec_t : protected VEC_t {
    friend class vec_t; // so vec_t can look at VEC_t
public:
    enum dummy_enumid { max_small = MAX_SMALL_VEC_SIZE };
    cvec_t() {
	_cnt = 0;
	_size = 0;
	_base = &_pair[0];
    }
    cvec_t(const cvec_t& v1, const cvec_t& v2) {
	_base= &_pair[0];
	set(v1, v2);
    }
    cvec_t(const void* p, size_t l) {
	_base = &_pair[0];
	set(p, l);
    }
    ~cvec_t();

    bool _is_large() const {return _base != &_pair[0];}
    cvec_t& put(const void* p, size_t l);
    cvec_t& put(const cvec_t& v);
    cvec_t& reset()  {
        _cnt = _size = 0;
        return *this;
    }
    cvec_t& set(const cvec_t& v1, const cvec_t& v2)  {
	return reset().put(v1).put(v2);
    }
    cvec_t& set(const void* p, size_t l)  {
        return reset().put(p, l);
    }

    size_t size() const	{
	return _size;
    }

    size_t copy_to(void* p, size_t limit = 0x7fffffff) const;
    
};

class vec_t : public cvec_t {
public:
    vec_t() : cvec_t()	{};
    vec_t(const cvec_t& v1, const cvec_t& v2) : cvec_t(v1, v2)  {};
    vec_t(const void* p, size_t l) : cvec_t(p, l)	{};
    //vec_t(const vec_t& v, size_t offset, size_t limit)
    //: cvec_t(v, offset, limit)	{};


#if 0
    /*
     *  copy_from() does not change vec_t itself, but overwrites
     *  the data area to which the vec points
     *  (temporarily made const for VAS compatibility)
     */
    const vec_t& copy_from(
	const void* p,
	size_t limit,
	size_t offset = 0) const;	// offset tells where
				//in the vec to begin to copy
    
    vec_t& copy_from(const cvec_t& v);
    vec_t& copy_from(
	const cvec_t& v,
	size_t offset,		// offset in v
	size_t limit,		// # bytes
	size_t myoffset = 0);	// offset in this
#endif

    void*	ptr(int index) const { return (index >= 0 && index < _cnt) ? 
					(void*)_base[index].ptr : NULL; }
    size_t	len(int index) const { return (index >= 0 && index < _cnt) ? 
					_base[index].len : 0; }
    //static vec_t& pos_inf;
    //static vec_t& neg_inf;
private:
    // disabled
    //vec_t(const vec_t&)  {}
    //operator=(vec_t);
};

#endif
