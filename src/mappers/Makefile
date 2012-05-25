all: simpleHashingMapper

simpleHashingMapper: SimpleHashingMapper.cpp
	g++ $(INCLUDES) $(OBJECTS) \
	-L$(PREFIX)/external/swSharp/lib \
	-lswsharp \
	-L/usr/local/cuda/lib64 -lcuda -lcudart \
	-o simpleHashingMapper \
		../util/SmithWaterman.o \
		SimpleHashingMapper.cpp
