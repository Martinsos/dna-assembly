all: simpleHashingMapper

SimpleHashingMapper.o: SimpleHashingMapper.cpp
	$(GXX) $(FLAGS) -c SimpleHashingMapper.cpp -o SimpleHashingMapper.o

simpleHashingMapper: SimpleHashingMapper.o
	$(GXX) $(FLAGS) -o simpleHashingMapper SimpleHashingMapper.o