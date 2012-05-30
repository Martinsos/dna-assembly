#!/bin/bash
FMindex_cpps="FMindex.cpp RTQ.cpp Trie.cpp"
Opp_cpps="Opp.cpp"
LEDA="-I./LEDA/incl -L./LEDA -lleda -lX11"
g++ $FMindex_cpps $Opp_cpps test.cpp $LEDA -o FMindex
