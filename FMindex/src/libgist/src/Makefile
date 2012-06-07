#	  Makefile for libgist, Gist test programs
#
# Generalized Search Tree

# LINKER = purify -g++=yes -collector=/usr/ccs/bin/ld g++
LINKER = g++
export LINKER

CC = g++
export CC

AR = gar
export AR

# macro definitions
DEFINES = -DUNIX -DLIBGIST
export DEFINES

CFLAGS = -g -Wall -Wno-unused $(DEFINES)
# CFLAGS = -g $(DEFINES)
export CFLAGS

GPPINCL = /usr/sww/lib/g++-include
export GPPINCL

# we recommend bison and flex for this code.
# if you want to use yacc and lex, you'll need to muck with the
# Makefiles a bit.
LEX = flex
export LEX

LEXFLAGS = -It
export LEXFLAGS

YACC = bison
export YACC

YACCFLAGS = -dv
export YACCFLAGS

all: LIBGIST LIBRTREE LIBBTREE TESTCOMMAND RTREE BTREE

pure: LIBGIST LIBRTREE TESTCOMMAND RTREEP

LIBGIST: 
	cd libgist ; $(MAKE) 

LIBRTREE:
	cd librtree ; $(MAKE) 

LIBBTREE:
	cd libbtree ; $(MAKE) 

TESTCOMMAND:
	cd examples/test ; $(MAKE)

RTREE: LIBGIST LIBRTREE TESTCOMMAND
	cd examples/rtree ; $(MAKE)

BTREE: LIBGIST LIBBTREE TESTCOMMAND
	cd examples/btree ; $(MAKE)

BTREEP: LIBGIST LIBBTREE TESTCOMMAND
	cd examples/btree ; $(MAKE) pure

RTREEP: LIBGIST LIBRTREE TESTCOMMAND
	cd examples/rtree ; $(MAKE) pure

clean:
	cd libgist ; $(MAKE) clean 
	cd librtree ; $(MAKE) clean 
	cd libbtree ; $(MAKE) clean 
	cd examples/test ; $(MAKE) clean 
	cd examples/rtree ; $(MAKE) clean 
	cd examples/btree ; $(MAKE) clean 

depend:
	cd libgist ; $(MAKE) depend 
	cd librtree ; $(MAKE) depend 
	cd libbtree ; $(MAKE) depend
	cd examples/test ; $(MAKE) depend 
	cd examples/rtree ; $(MAKE) depend 
	cd examples/btree ; $(MAKE) depend 

tags:
	etags */*.{cc,y,l,h}


