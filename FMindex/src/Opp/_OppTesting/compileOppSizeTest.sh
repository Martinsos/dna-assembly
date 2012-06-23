#!/bin/bash

# Compile testing file for examining space occupance of Opp structure

if [ "$1" == "--debug-mode=yes" ]
then
    echo "compiling in debug mode..."
    cd Opp/
    g++ -O0 -g -o oppSizeTest.exe Alphabet.cpp BitArray.cpp Compressor.cpp Opp.cpp OppRows.cpp ../StringView.cpp ../compressorSizeTest.cpp
    mv oppSizeTest.exe ../
    cd ../
else
    echo "compiling in normal mode..."
    cd Opp/
    g++ -O0 -o oppSizeTest.exe Alphabet.cpp BitArray.cpp Compressor.cpp Opp.cpp OppRows.cpp ../StringView.cpp ../compressorSizeTest.cpp
    mv oppSizeTest.exe ../
    cd ../
fi
echo "oppSizeTest.exe successfully created."
