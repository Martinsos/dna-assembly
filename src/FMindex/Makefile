CPP = g++
FLAGS =

FMINDEX_OBJ = FMindex.o RTQRTree.o Trie.o StringView.o
OPP_OBJ = Opp/Opp.o Opp/OppRows.o Opp/Compressor.o Opp/Alphabet.o Opp/BitArray.o


all: $(FMINDEX_OBJ) $(OPP_OBJ)


# executables
testFromFile: $(FMINDEX_OBJ) $(OPP_OBJ) testFromFile.cpp
	$(CPP) $(FLAGS) $(FMINDEX_OBJ) $(OPP_OBJ) testFromFile.cpp -o testFromFile

test: $(FMINDEX_OBJ) $(OPP_OBJ) test.cpp
	$(CPP) $(FLAGS) $(FMINDEX_OBJ) $(OPP_OBJ) test.cpp -o test

# FMindex
FMindex.o: FMindex.cpp FMindex.hpp StringView.hpp
	$(CPP) -c $(FLAGS) FMindex.cpp -o FMindex.o

RTQRTree.o: RTQRTree.cpp RTQRTree.hpp
	$(CPP) -c $(FLAGS) RTQRTree.cpp -o RTQRTree.o

Trie.o: Trie.cpp Trie.hpp StringView.hpp
	$(CPP) -c $(FLAGS) Trie.cpp -o Trie.o

StringView.o: StringView.cpp StringView.hpp
	$(CPP) -c $(FLAGS) StringView.cpp -o StringView.o

RTQRTree.hpp: RTQ.hpp RTree.hpp

Trie.hpp: Opp/Opp.hpp

FMindex.hpp: Opp/Opp.hpp Trie.hpp RTQ.hpp RTQRTree.hpp


# Opp
Opp/Opp.o: Opp/Opp.cpp Opp/Opp.hpp
	$(CPP) -c $(FLAGS) Opp/Opp.cpp -o Opp/Opp.o

Opp/OppRows.o: Opp/OppRows.cpp Opp/OppRows.hpp
	$(CPP) -c $(FLAGS) Opp/OppRows.cpp -o Opp/OppRows.o

Opp/Compressor.o: Opp/Compressor.cpp Opp/Compressor.hpp
	$(CPP) -c $(FLAGS) Opp/Compressor.cpp -o Opp/Compressor.o

Opp/Alphabet.o: Opp/Alphabet.cpp Opp/Alphabet.hpp
	$(CPP) -c $(FLAGS) Opp/Alphabet.cpp -o Opp/Alphabet.o

Opp/BitArray.o: Opp/BitArray.cpp Opp/BitArray.hpp
	$(CPP) -c $(FLAGS) Opp/BitArray.cpp -o Opp/BitArray.o

Opp/Opp.hpp: StringView.hpp Opp/Alphabet.hpp Opp/Compressor.hpp Opp/OppRows.hpp

Opp/Compressor.hpp: Opp/Alphabet.hpp Opp/BitArray.hpp


# utility
clean:
	\rm $(FMINDEX_OBJ) $(OPP_OBJ)
