CPP = g++
FLAGS = 

LEDA_I = -I./LEDA/incl
LEDA_L = -L./LEDA -lleda -lX11

FMINDEX_OBJ = FMindex.o RTQ.o Trie.o StringView.o
OPP_OBJ = Opp/Opp.o Opp/OppRows.o Opp/Compressor.o Opp/Alphabet.o Opp/BitArray.o

all: testFromFile

testFromFile: $(FMINDEX_OBJ) $(OPP_OBJ) testFromFile.cpp
	$(CPP) $(FLAGS) $(LEDA_I) $(LEDA_L) $(FMINDEX_OBJ) $(OPP_OBJ) testFromFile.cpp -o testFromFile

# FMindex
FMindex.o: FMindex.cpp FMindex.hpp StringView.hpp
	$(CPP) -c $(FLAGS) $(LEDA_I) FMindex.cpp -o FMindex.o

RTQ.o: RTQ.cpp RTQ.hpp
	$(CPP) -c $(FLAGS) $(LEDA_I) RTQ.cpp -o RTQ.o

Trie.o: Trie.cpp Trie.hpp StringView.hpp
	$(CPP) -c $(FLAGS) Trie.cpp -o Trie.o

StringView.o: StringView.cpp StringView.hpp
	$(CPP) -c $(FLAGS) StringView.cpp -o StringView.o

Trie.hpp: Opp/Opp.hpp

FMindex.hpp: Opp/Opp.hpp Trie.hpp RTQ.hpp


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


clean:
	\rm $(FMINDEX_OBJ) $(OPP_OBJ)
