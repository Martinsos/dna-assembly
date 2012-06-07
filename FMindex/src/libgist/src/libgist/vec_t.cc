//	vec_t.cc
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libgist/vec_t.cc,v 1.1.1.1 1997/11/13 02:43:58 marcel Exp $


#include <stdlib.h>
#include <memory.h>
#include <iostream.h>
#include "vec_t.h"

cvec_t& cvec_t::put(const void* p, size_t l)  
{
    assert(_cnt+1 <= max_small);

    // to make zvecs work:
    _base[_cnt].ptr = (unsigned char*)p; 
    _base[_cnt].len = l;
    if(l>0) {
	_cnt++;
   	assert(_cnt <= max_small); 
	_size += l;
    }
    return *this;
}

size_t cvec_t::copy_to(void* p, size_t limit) const 
{
    char* s = (char*) p;
    for (int i = 0; i < _cnt && limit > 0; i++) {
	size_t n = limit > _base[i].len ? _base[i].len : limit;
	if (_base[i].ptr == NULL) {
	    memset(s, '\0', n);
	} else {
	    memcpy(s, _base[i].ptr, n);
	}
	assert(limit >= n);
	limit -= n;
	s += n;
    }
    return s - (char*)p;
}

cvec_t& cvec_t::put(const cvec_t& v)
{
    assert(_cnt+v._cnt <= max_small);
    for (int i = 0; i < v._cnt; i++)  {
	_base[_cnt + i].ptr = v._base[i].ptr;
	_base[_cnt + i].len = v._base[i].len;
    }
    _cnt += v._cnt;
    assert(_cnt <= max_small); 
    _size += v._size;

    return *this;
}

cvec_t::~cvec_t() 
{
}
