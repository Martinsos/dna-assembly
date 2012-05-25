#ifndef COMPRESSOR_HPP
#define COMPRESSOR_HPP

#include "Alphabet.hpp"
#include "BitArray.hpp"

#include <string>
#include <vector>
#include <list>
#include <map>
#include <deque>

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
 *
 * Note: RLE and VLPC are implemented together in getVarLengthPrefixEncoding()
 */
class Compressor
{   
    public:
        /** Constructor
         *  Initializes stuff
         *
         *  @param eof          eof character used in compression
         *  @param alpha        alphabet of input text
         *  @param bucketSize   size of a bucket
         */
        Compressor(char eof, Alphabet alpha, int bucketSize);   // OPTIMIZIRATI

        /** Compresses input text to binary format
         *  Pipeline: T | BWT | MTF | RLE | VLPC
         *
         *  @param T Original text
         *  @return  Compressed T
         */
        BitArray compress(string& T);
        
        /* --------------------------------- Test methods --------------------------------------- */

        /** Decodes given BitArray back to letters
         *  Used for testing purposes
         *
         *  @param bits Encoded text in binary
         *  @return     Decoded text
         */
        string decode(const BitArray& bits);

        /** Getter for MTFStates
         */
        vector< list<char> > getMTFStates();
    public:
        /* -------------------------------- Data structures -------------------------------------- */

        char BWTEof;            // Character appended to T, end-of-file
        Alphabet alphabet;      // Alphabet used in encoding
        BitArray Z;             // Encoded text
        
        // OPP data structures
        int n;                  // Length of input text + EOF character
        int bucketSize;         // Size of a bucket (l in article)         
        int superBucketSize;    // Size of a super bucket (l^2 in article)
        
        /** For bucket i, MTFStates[i] keeps the state of MTF table just before encoding it
         *  getMTF() initializes it
         */
        vector< list<char> > MTFStates;

        /** MZ[i] - number of zeroes missing at the beginning of bucket i
         *  Important to know when decoding particular bucket
         */
        vector<int> MZ;

        /** Number of occurrences - superbucket
         *  sbNO[i][c] - number of occurrences of character c in superbuckets 0 - i (included)
         */
        vector< map<char, int> > sbNO;
        /** For each superbucket stores size in bits of superbuckets 0 - i (included)
         */ 
        vector<int> sbW;

        /** Number of occurrences - bucket
         *  bNO[i][c] - number of occurrences of character c in buckets x - i (included)
         *  x - first bucket in current superbucket
         */
        vector< map<char, int> > bNO;
        /** For each bucket stores size in bits of buckets x - i (included)
         *  x - first bucket int current superbucket
         */
        vector<int> bW;

        /* --------------------------------------- Methods ----------------------------------------- */ 

        /** Returns how many times c appears in first q letters of L (BWT of input text)
         *
         *  @param c    Char whose appearance is counted
         *  @param q    Determines how far to look in L
         *  @returns    Number of occurrences of c in first q letters of L
         */
        int occ(char c, int q); 

        /** Counts occurrences of c in first h letters in bucket starting at BZStart
         *
         *  @param c                Character whose occurrence is counted
         *  @param h                Number of letters to look at from start of bucket
         *  @param BZStart          Index where bucket starts in Z
         *  @param MTFState         State of MTFList before encoding this bucket
         *  @param missingZeroes    Missing zeroes at beginning of given bucket
         *  @returns                Occurrences of c in first h letters of given bucket
         */
        int S(char c, int h, int BZStart, list<char> MTFState, int missingZeroes);

        /** Initializes bNO and sbNO structures
         *
         *  @param T Input text
         */
        void initNOs(const string& T);

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
        vector<Index> getSuffixArray(const string& T);  // ISTO OPTIMIZIRATI

        /** Apply move-to-front encoding to input text
         *
         *  Side effect: Initialize MTFStates structure for each bucket
         *
         *  @param T Input text
         *  @return  MTF of T
         */
        vector<int> getMTF(const string& L);

        /** Apply variable-length prefix encoding to input string
         *  Also applies run-length encoding on-fly
         *
         *  Side effect: initializes structures sbW, bW and MZ
         *
         *  @param MTF MTF code of T
         *  @return    Encoded T over alphabet {0, 1}
         */
        BitArray getVarLengthPrefixEncoding(const vector<int>& MTF);    // OPTIMIZIRATI

        /** Encode zero sequence to binary according to algorithm in article
         *  
         *  @param m Length of zero sequence
         *  @return encoded zero sequence
         */
        deque<bool> zeroSeqEncode(int m);

        /** Converts non-zero MTF digit to binary prefix code
         *
         *  @param i    MTF digit
         *  @returns    Binary representation of i
         */
        vector<bool> MTFToBin(int i);

        /** Returns binary representation of given number
         *
         *  @param  Input number
         *  @return Binary representation of a
         */
        vector<bool> intToBin(int a);

        /** Checks if last digit in bucket is reached (0-based indexing)
         *
         *  @param pos  Position we are currently looking at
         *  @return     True if end of bucket is reached
         */
        bool isBucketEnd(int pos);

        /** Checks if last digit in superBucket is reached (0-based indexing)
         *
         *  @param pos  Position we are currently looking at
         *  @return     True if end of superBucket is reached
         */
        bool isSuperBucketEnd(int pos);
};
#endif // COMPRESSOR_HPP
