GISTHOME = ../../..

INCS = -I$(GPPINCL) -I$(GISTHOME)/src/libgist -I. \
	-I$(GISTHOME)/src/librtree

SRCS = command.cpp 
       
OBJS = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CC) -c $(CFLAGS) $(INCS) $*.cpp

all: $(OBJS)

clean:
	rm -f $(OBJS) *~ #*# *pure*
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
