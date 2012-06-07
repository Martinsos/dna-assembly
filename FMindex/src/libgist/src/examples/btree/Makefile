#         Makefile for GiST test program
#

GISTHOME = ../../..

INCS = -I$(GPPINCL) -I$(GISTHOME)/src/libgist \
	-I$(GISTHOME)/src/libbtree -I$(GISTHOME)/src/examples/test -I.

LDFLAGS = -L$(GISTHOME)/lib

BTLIBS = -lbtree -lgist -lg++

DEST = .

BTPROG = $(DEST)/btree

BTSRCS = bt_gram.y bt_scan.l bt_driver.cpp
BTOBJS = bt_gram.tab.o bt_scan.o bt_driver.o
COMMANDOBJ = ../test/command.o

all: BTREE 

pure: BTREEP

BTREE: $(BTOBJS) $(GISTHOME)/lib/libgist.a $(GISTHOME)/lib/libbtree.a $(COMMANDOBJ)
	$(LINKER) -o $(BTPROG) $(LDFLAGS) $(BTOBJS) $(COMMANDOBJ) $(BTLIBS)

BTREEP: $(BTOBJS) $(GISTHOME)/lib/libgist.a $(GISTHOME)/lib/libbtree.a $(COMMANDOBJ)
	purify -cache-dir=/tmp \
	$(LINKER) -o $(BTPROG) $(LDFLAGS) $(BTOBJS) $(COMMANDOBJ) $(BTLIBS)

.depend:
	$(MAKE) depend
	touch .depend

bt_gram.tab.cpp: bt_gram.y
	$(YACC) $(YACCFLAGS) $^
	mv bt_gram.tab.c $@
	mv bt_gram.tab.h bt_gram_tab.h

bt_scan.cpp: bt_scan.l
	$(LEX) $(LEXFLAGS) $^ >$@

%.o: %.cpp
	$(CC) -c $(CFLAGS) $(INCS) $*.cpp

clean:
	rm -f $(BTOBJS) $(BTPROG) core bt_gram.tab.cpp bt_scan.cpp bt_gram_tab.h bt_gram.output core *~ #*# *pure* TAGS
	makedepend
#	rm -f $(RTOBJS) $(RSTOBJS) $(RTPROG) $(RSTPROG) core *~ #*# *pure* TAGS

depend:
	makedepend $(DEFINES) $(INCS) $(RTSRCS) $(RSTSRCS)

install: $(PROGRAM)
	echo Installing $(PROGRAM) in $(DEST)
	install -s $(PROGRAM) $(DEST)

# DO NOT DELETE THIS LINE -- make depend depends on it.
