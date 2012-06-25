CXX = g++

CXXFLAGS = -O3 -Wall -g -DNDEBUG -Wno-deprecated -pedantic -ansi -finline-functions -foptimize-sibling-calls -Wcast-qual -Wwrite-strings -Wsign-promo -Wcast-align -Wno-long-long -fexpensive-optimizations -funroll-all-loops -ffast-math -fomit-frame-pointer -pipe -I./ -L./
%CXXFLAGS = -O3 -DNDEBUG -W -Wall -Wno-deprecated -pedantic -ansi -finline-functions -foptimize-sibling-calls -Wcast-qual -Wwrite-strings -Wsign-promo -Wcast-align -Wno-long-long -fexpensive-optimizations -funroll-all-loops -ffast-math -fomit-frame-pointer -pipe -I./ -L./
%CXXFLAGS = -g -W -Wall -Wno-deprecated
LINKFLAGS = -lm

SRCS1 = \
	bit_array.cpp \
	wat_array.cpp \
	fmIndex.cpp \

SRCS2 = \
	construct.cpp \

SRCS3 = \
	search.cpp \

OBJS1 = $(SRCS1:%.cpp=%.o)
OBJS2 = $(SRCS2:%.cpp=%.o)
OBJS3 = $(SRCS3:%.cpp=%.o)

all: construct search
construct: $(OBJS1) $(OBJS2)
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OBJS1) $(OBJS2) $(LINKFLAGS) -o fmconstruct
search: $(OBJS1) $(OBJS3)
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OBJS1) $(OBJS3) $(LINKFLAGS) -o fmsearch

debug:
	make all CXXFLAGS="-ggdb -W -Wall -pedantic"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) -c $<

clean:
	rm -f fmconstruct fmsearch *.o *~
