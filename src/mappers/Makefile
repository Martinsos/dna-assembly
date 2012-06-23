all: simpleHashingMapper FMindexMapper

simpleHashingMapper: SimpleHashingMapper.cpp ../util/SmithWaterman.o
	g++ $(INCLUDES)\
	-L/usr/local/cuda/lib64 -lcuda \
	-L/usr/local/cuda/lib64	-lcudart \
	-L$(PREFIX)/external/swSharp/lib -lswsharp \
	-o simpleHashingMapper \
		../util/SmithWaterman.o \
		$(PREFIX)/external/swSharp/lib/libswsharp.a \
		SimpleHashingMapper.cpp -g -ggdb

FMindexMapper: FMindexMapper.cpp ../util/SmithWaterman.o
	g++ $(INCLUDES)\
	-L/usr/local/cuda/lib64 -lcuda \
	-L/usr/local/cuda/lib64	-lcudart \
	-L$(PREFIX)/external/swSharp/lib -lswsharp \
	-o FMindexMapper \
		../util/SmithWaterman.o \
		../FMindex/*.o \
		../FMindex/Opp/*.o \
		$(PREFIX)/external/swSharp/lib/libswsharp.a \
		FMindexMapper.cpp -g -ggdb

clean:
	\rm simpleHashingMapper FMindexMapper
