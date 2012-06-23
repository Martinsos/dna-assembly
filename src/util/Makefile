all: SmithWaterman.o

SmithWaterman.o : SmithWaterman.cpp SmithWaterman.h
	$(GXX) $(INCLUDES)  \
		-static \
		-o SmithWaterman.o \
		-c \
		$(PREFIX)/external/swSharp/lib/libswsharp.a \
		SmithWaterman.cpp 

clean:
	\rm SmithWaterman.o
