all: simpleHashingMapper

simpleHashingMapper: SimpleHashingMapper.cpp ../util/SmithWaterman.o
	g++ $(INCLUDES)\
	-L/usr/local/cuda/lib64 -lcuda \
	-L/usr/local/cuda/lib64	-lcudart \
	-L$(PREFIX)/external/swSharp/lib -lswsharp \
	-o simpleHashingMapper \
		../util/SmithWaterman.o \
		$(PREFIX)/external/swSharp/lib/libswsharp.a \
		SimpleHashingMapper.cpp -g -ggdb