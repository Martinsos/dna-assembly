#         Makefile for GiST test program
#
# Generalized Search Tree

GISTHOME = ../../..

INCS = -I$(GPPINCL) -I$(GISTHOME)/src/libgist \
	-I$(GISTHOME)/src/librtree -I$(GISTHOME)/src/examples/test -I.

LDFLAGS = -L$(GISTHOME)/lib

RTLIBS = -lrtree -lgist -lg++

DEST = .

RTPROG = $(DEST)/rtree

RTSRCS = rt_gram.y rt_scan.l rt_driver.cpp
RTOBJS = rt_gram.tab.o rt_scan.o rt_driver.o
COMMANDOBJ = ../test/command.o

all: RTREE 

pure: RTREEP

RTREE: $(RTOBJS) $(GISTHOME)/lib/libgist.a $(GISTHOME)/lib/librtree.a $(COMMANDOBJ)
	$(LINKER) -o $(RTPROG) $(LDFLAGS) $(RTOBJS) $(COMMANDOBJ) $(RTLIBS)

RTREEP: $(RTOBJS) $(GISTHOME)/lib/libgist.a $(GISTHOME)/lib/librtree.a $(COMMANDOBJ)
	purify -cache-dir=/tmp \
	$(LINKER) -o $(RTPROG) $(LDFLAGS) $(RTOBJS) $(COMMANDOBJ) $(RTLIBS)

.depend:
	$(MAKE) depend
	touch .depend

rt_gram.tab.cpp: rt_gram.y
	$(YACC) $(YACCFLAGS) $^
	mv rt_gram.tab.c $@
	mv rt_gram.tab.h rt_gram_tab.h

rt_scan.cpp: rt_scan.l
	$(LEX) $(LEXFLAGS) $^ > $@

%.o: %.cpp
	$(CC) -c $(CFLAGS) $(INCS) $*.cpp

clean:
	rm -f $(RTOBJS) $(RSTOBJS) $(RTPROG) $(RSTPROG) core rt_gram.tab.cpp rt_scan.cpp rt_gram_tab.h rt_gram.output core *~ #*# *pure* TAGS
	makedepend
#	rm -f $(RTOBJS) $(RSTOBJS) $(RTPROG) $(RSTPROG) core *~ #*# *pure* TAGS

depend:
	makedepend $(DEFINES) $(INCS) $(RTSRCS) $(RSTSRCS)

install: $(PROGRAM)
	echo Installing $(PROGRAM) in $(DEST)
	install -s $(PROGRAM) $(DEST)

# DO NOT DELETE THIS LINE -- make depend depends on it.
