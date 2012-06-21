GISTHOME = ../..

INCS = -I$(GPPINCL) -I$(GISTHOME)/src/libgist -I.

DEST = $(GISTHOME)/lib

LIBRARY = $(DEST)/libbtree.a

SRCS = gist_btree.cc
       
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
