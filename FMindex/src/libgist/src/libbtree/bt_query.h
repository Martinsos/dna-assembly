//	bt_query.h
// Copyright (c) 1997, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/libbtree/bt_query.h,v 1.1.1.1 1997/11/13 02:43:55 marcel Exp $


#ifndef BT_QUERY_H
#define BT_QUERY_H

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

#endif
