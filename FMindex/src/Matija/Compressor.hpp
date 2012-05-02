#ifndef COMPRESSOR_HPP
#define COMPRESSOR_HPP

#include "Alphabet.hpp"

#include <string>
#include <vector>

typedef int Index;

using namespace std;

/**
 * Compresses input text for use by FMIndex
 * Pipeline: T | BWT | MTF | RLE | VLPC
 *
 * T    - Input text
 * BWT  - Burrows-Wheeler transform
 * MTF  - Move-to-front encoding
 * RLE  - Run-length encoder
 * VLPC - Variable-length prefix code 
 */
class Compressor
{   
    public:
        /** Constructor
         *  Sets eof character used in compression
         *  Sets alphabet of input text
         *
         *  @param eof
         *  @param alpha
         */
        Compressor(char eof, Alphabet alpha);

        /** Compresses input text to binary format
         *  Pipeline: T | BWT | MTF | RLE | VLPC
         *
         *  @param T Original text
         *  @return  Compressed T
         */
        string compress(const string& T);
    //private:
        char BWTEof;        // Character appended to T, end-of-file
        Alphabet alphabet;  // Alphabet used in encoding

        /** Calculate BWT of input text
         *
         *  @param T Input text
         *  @return  Burrows-Wheeler transform of T
         */
        string getBWT(string& T);
        
        /** Get suffix array of input text
         *
         *  @param T Input text
         *  @return  Indices referencing on T 
         */
        vector<Index> getSuffixArray(const string& T);

        /** Apply move-to-front to input text
         *
         *  @param T Input text
         *  @return  MTF of T
         */
        vector<int> getMTF(const string& L);

        /** Apply run-length encoding to input string
         *  Replace 0 sequences with binary numbers
         *  
         *  @param MTF Input string
         *  @return    RLE of MTF
         */
        string getRLE(const string& MTF);

        /** Apply variable-length prefix encoding to input string
         *
         *  @param RLE Input string
         *  @return    Encoded string over alphabet {0, 1}
         */
        string getVLPC(const string& RLE);
};
#endif // COMPRESSOR_HPP
