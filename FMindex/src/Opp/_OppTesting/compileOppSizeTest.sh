#!/bin/bash

# Compile testing file for examining space occupance of Opp structure

if [ "$1" == "--debug-mode=yes" ]
then
    echo "compiling in debug mode..."
    cd ../
    g++ -O0 -g -o oppSizeTest.exe Alphabet.cpp BitArray.cpp Compressor.cpp Opp.cpp OppRows.cpp ../StringView.cpp ../compressorSizeTest.cpp
    mv oppSizeTest.exe _OppTesting/
    cd _OppTesting/
else
    echo "compiling in normal mode..."
    cd ../
    g++ -O0 -std=c++0x -o oppSizeTest.exe Alphabet.cpp BitArray.cpp Compressor.cpp Opp.cpp OppRows.cpp ../StringView.cpp ../compressorSizeTest.cpp
    mv oppSizeTest.exe _OppTesting/
    cd _OppTesting/
fi
echo "oppSizeTest.exe successfully created."
