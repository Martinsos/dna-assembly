# 	Makefile for src/libgist
#	$Header: /usr/local/devel/GiST/libgist/src/libgist/Makefile,v 1.2 1997/12/05 23:08:33 marcel Exp $

GISTHOME = ../..

INCS = -I$(GPPINCL) -I.

DEST = $(GISTHOME)/lib

LIBRARY = $(DEST)/libgist.a

SRCS = gist_file.cc \
	gist_p.cc \
	gist_htab.cc \
	vec_t.cc \
	gist.cc \
	gist_unordered.cc \
	gist_unorderedn.cc
       
OBJS = $(SRCS:.cc=.o)

%.o: %.cc
	$(CC) -c $(CFLAGS) $(INCS) $*.cc

$(LIBRARY): $(OBJS)
	rm -f $(LIBRARY)
	$(AR) rs $(LIBRARY) $(OBJS)

clean:
	rm -f $(OBJS) $(LIBRARY) *~ #*# *pure*
	makedepend

depend:
	makedepend $(DEFINES) $(INCS) $(SRCS)

.depend:
	$(MAKE) depend
	touch .depend

install: $(PROGRAM)
	echo Installing $(PROGRAM) in $(DEST)
	install -s $(PROGRAM) $(DEST)

# DO NOT DELETE THIS LINE -- make depend depends on it.
