/**
 * Class Compressor
 */

#include "Compressor.hpp"

#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

/** Constructor
 */
Compressor::Compressor(char eof, Alphabet alpha) : alphabet(alpha)
{
    BWTEof   = eof;
}

/** Class for comparing string suffixes
 *
 */
class SASort 
{
    public:
        SASort(const string& T_) : T(T_) {}

        // Compare suffixes of T, starting at pos1 and pos2
        bool operator() (Index pos1, Index pos2)
        {
            int cmp = T.compare(pos1, T.length() - pos1, T, pos2, T.length() - pos2);
            return (cmp < 0);    
        }
    private:
        const string& T;
};

/** Return compressed original text
 *
 *  Main (and only) public class
 *
 */
BitArray Compressor::compress(string& T)
{
    return getVarLengthPrefixEncoding( getMTF(getBWT(T)) );
}

vector<int> Compressor::getSuffixArray(const string& T)
{
    // Construct initial unsorted suffix list
    vector<Index> SA (T.length());
    for (Index i = 0; i < SA.size(); i++)
        SA[i] = i;
    
    // Sort suffixes regarding T
    SASort cmpObj(T);
    sort(SA.begin(), SA.end(), cmpObj);
     
    return SA;
}   

/** Calculates BWT for input text
 *
 *  Appends BWTEof to the end of T - that's why T is not const
 */
string Compressor::getBWT(string& T)
{
    T.append(1, BWTEof);
    vector<Index> SA = getSuffixArray(T);

    // Calculate BWT from SA
    string bwt = "";
    for (int i = 0; i < SA.size(); i++)
        if (SA[i] == 0)
            bwt += string(1, T[T.length() - 1]);
        else
            bwt += string(1, T[SA[i] - 1]);

    return bwt;
}

/** Move-to-front encoding
 *
 * Starts with sorted list of alphabet characters.
 * To each character of input string L is assigned it's position in list (0-based). When character
 * is accessed, it is moved to the front of list (so its code is 0).
 * This way we get consecutive 0's for rows of same character.
 */
vector<int> Compressor::getMTF(const string& L)
{
    // Construct initial MTF list
    list<char> MTFList = alphabet.toSortedList();
    
    // Generate MTF code for L
    vector<int> MTF (0); 
    for (int i = 0; i < L.length(); i++)
    {
        // Find position of L[i] in MTFList and encode it
        list<char>::iterator it;
        int pos;
        for (pos = 0, it = MTFList.begin(); it != MTFList.end(); it++, pos++)
            if (*it == L[i])
            {
                MTF.push_back(pos);
                MTFList.push_front(*it);
                MTFList.erase(it);
                break;
            }
    }
    return MTF;
}

/** Variable-length prefix coding
 *
 *  Note: This method also applies run-length encoding on-fly, taking MTF code as input. It was inconvenient
 *  to separate RLE and VLPC into separate methods.
 *
 *  Run-length encoding:
 *  Replace any sequence 0^m with the number (m + 1) written in binary (0', 1'), least significant bit first,
 *  discard the most significant bit (as it is always 1). 
 *  Resulting text is defined over {0', 1', 1, 2, 3, ...}
 *
 *  Variable-length prefix coding:
 *  {0', 1'} -> {10, 11}
 *  i(other) -> floor(log(i + 1)) 0's followed by binary representation of i + 1
 *  Result is defined over {0, 1}
 */

void dumpVector(vector<bool> a);

BitArray Compressor::getVarLengthPrefixEncoding(const vector<int>& MTF)
{
    vector<bool> vlpc(0);

    // Iterate over MTF
    for (int i = 0; i < MTF.size(); i++)
    {
        if (MTF[i] != 0)
        {   
            // Add leading zeros
            int leadingZeros = (int)trunc(log2(MTF[i] + 1));
            for (int j = 0; j < leadingZeros; j++)
                vlpc.push_back(0);

            // Add binary rep of MTF[i]
            vector<bool> bin = intToBin(MTF[i] + 1);
            for (int j = 0; j < bin.size(); j++)
                vlpc.push_back(bin[j]);
        }
        else
        {
            // Encode 0 sequence
            int zeroSeqLength = 0;
            for(; MTF[i] == 0 && i < MTF.size(); zeroSeqLength++, i++); i--;
            
            // To binary
            vector<bool> bin = intToBin(zeroSeqLength + 1);
            for (int j = bin.size() - 1; j >= 1; j--)
            {
                if (bin[j] == 0) { vlpc.push_back(1); vlpc.push_back(0); }
                if (bin[j] == 1) { vlpc.push_back(1); vlpc.push_back(1); }
            }
        }
    }
    return BitArray(vlpc);
}

/** For testing
 */
void dumpVector(vector<bool> a)
{
    for (int i = 0; i < a.size(); i++)
        cout << a[i];
    cout << " ";
}

/** Convert integer value to binary
 */
vector<bool> Compressor::intToBin(int a)
{
    vector<bool> binary(0);
    while (a > 0)
    {
        binary.push_back(a % 2);
        a /= 2;
    }
    reverse (binary.begin(), binary.end());
    return binary;
}
