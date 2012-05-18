#!/bin/bash
cpps="FMindex.cpp Opp.cpp RTQ.cpp Trie.cpp test.cpp"
g++ $cpps -I./LEDA/incl -L./LEDA -lleda -lX11
