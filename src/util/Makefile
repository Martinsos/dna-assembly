all: SmithWaterman.o

SmithWaterman.o : SmithWaterman.cpp SmithWaterman.h
	$(GXX) $(INCLUDES)  \
		-L$(PREFIX)/external/swSharp/lib\
		-lswsharp \
		-static \
		-o SmithWaterman.o \
		-c SmithWaterman.cpp 