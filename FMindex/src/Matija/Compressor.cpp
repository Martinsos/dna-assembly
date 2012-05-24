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
Compressor::Compressor(char eof, Alphabet alpha, int bucketSize) : alphabet(alpha)
{
    BWTEof   = eof;
    this->bucketSize = bucketSize;
    this->superBucketSize = bucketSize * bucketSize;

    // Initialize MTFCode map for alphabet
    for (int i = 1; i < alphabet.size(); i++)
    {
        vector<bool> bin = MTFToBin(i);
        string binStr = "";
        for (int j = 0; j < bin.size(); j++)
            binStr += bin[j] == 0 ? '0' : '1';

        alphabet.putCode(binStr, i);
        cout << "dodao u mapu: " << binStr << " -> " << i << endl;
    }
}

/** Getter for MTFStates
 */
vector< list<char> > Compressor::getMTFStates()
{
    return MTFStates;    
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

/** Returns compressed original text
 *
 *  Main (and only) public class
 */
BitArray Compressor::compress(string& T)
{
    n = T.length() + 1; // Because we add EOF character
    initNOs(T);
    return getVarLengthPrefixEncoding( getMTF(getBWT(T)) );
}

/** Initializes bNO and sbNO structures
 *  Counts occurrences of each character and stores that in structures
 *  in appropriate format
 *
 *  Side effect: initializes bNO and sbNO structures
 */
void Compressor::initNOs(const string& T)
{
    map<char, int> currBucket;
    map<char, int> currSuperBucket;

    for (int i = 0; i < T.length(); i++)
    {
        currBucket[T[i]]++;
        currSuperBucket[T[i]]++;

        // Reached end of bucket
        if (isBucketEnd(i))
        {
            bNO.push_back(currBucket);
        }
        // Reached end of superbucket
        if (isSuperBucketEnd(i))
        {
            sbNO.push_back(currSuperBucket);
            // Clear value of bNO - start counting from 0 again
            currBucket.clear();
        }
    }
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
 * is accessed, it is moved to the front of list (so its code is now 0).
 * This way we get consecutive 0's for consecutive same characters.
 *
 * Side effect: Initalize MTF structure for each bucket (takes picture before encoding bucket)
 */
vector<int> Compressor::getMTF(const string& L)
{
    // Construct initial MTF list
    list<char> MTFList = alphabet.toSortedList();
    
    // Generate MTF code for L
    vector<int> MTF (0); 
    for (int i = 0; i < L.length(); i++)
    {
        // If I am at the beginning of a bucket, store current MTF state
        if (i % bucketSize == 0) 
        {
            MTFStates.push_back(MTFList);    
        }

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
    
    cout << "==MTF Code==================================" << endl;
    for (int i = 0; i < MTF.size(); i++)
        cout << MTF[i] << " ";
    cout << endl;

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
 *
 *  Side effect: initializes structures sbW, bW and MZ
 */
BitArray Compressor::getVarLengthPrefixEncoding(const vector<int>& MTF)
{
    int currBucketSize = 0;
    int currSuperBucketSize = 0;

    vector<bool> vlpc(0);
    // Iterate over MTF
    for (int i = 0; i < MTF.size(); i++)
    {
        // If I encounter non-zero number
        if (MTF[i] != 0)
        {  
            vector<bool> bin = MTFToBin(MTF[i]);
            for (int j = 0; j < bin.size(); j++)    // BitArray interakcija    
                vlpc.push_back(bin[j]);

            // Update sizes
            int codeLength = bin.size();
            currBucketSize += codeLength;
            currSuperBucketSize += codeLength;

            // Reached end of the bucket
            if (isBucketEnd(i))
            {
                bW.push_back(currBucketSize);
                if (MZ.size() < (i + 1) / bucketSize)
                    MZ.push_back(0);        // Inicijaliziraj nekako drugacije da vec bude sve 0 - mozda
            }
            // Reached end of the superBucket
            if (isSuperBucketEnd(i))
            {
                sbW.push_back(currSuperBucketSize);
                // Restart bucket counter
                currBucketSize = 0;
            }
        }
        else // If I encounter zero-sequence
        {
            // Count number of consecutive zeroes
            int zeroSeqStart = i;
            int zeroSeqLength = 0;
            for(; MTF[i] == 0 && i < MTF.size(); zeroSeqLength++, i++); i--; // i now points to the last 0
            int zeroSeqEnd = zeroSeqStart + zeroSeqLength - 1;

            // Convert zero sequence to binary
            deque<bool> bin = zeroSeqEncode(zeroSeqLength);
            deque<bool>::iterator it;
            for (it = bin.begin(); it != bin.end(); it++)
            {
                if (*it == 0) { vlpc.push_back(1); vlpc.push_back(0); }
                if (*it == 1) { vlpc.push_back(1); vlpc.push_back(1); }
            }

            // Loop over zeroes
            for (int pos = zeroSeqStart; pos <= zeroSeqEnd; pos++)
            {
                // If I reach end of bucket or end of zero sequence
                if (isBucketEnd(pos) || pos == zeroSeqEnd)
                {
                    int zeroesNeeded = pos - zeroSeqStart + 1;

                    // Take minimal amount of digits from bin to cover zeroesNeeded
                    int zeroesTaken = 0;
                    int bitPos = 0;
                    while (zeroesTaken < zeroesNeeded && bin.size() > 0)
                    {
                        zeroesTaken += (bin.front() + 1) * pow(2, bitPos);
                        bin.pop_front();
                        bitPos++;
                    }
                    int zeroesMissing = min(0, zeroesNeeded - zeroesTaken);
                    MZ.push_back(zeroesMissing);

                    int bitsTaken = bitPos * 2;         // Because each bit is encoded with 2 bits in final code
                    currBucketSize += bitsTaken;       
                    currSuperBucketSize += bitsTaken;

                    // Reached end of the bucket
                    if (isBucketEnd(pos))
                    {
                        bW.push_back(currBucketSize);
                    }
                    // Reached end of the superBucket
                    if (isSuperBucketEnd(pos))
                    {
                        sbW.push_back(currSuperBucketSize);
                        // Restart bucket counter
                        currBucketSize = 0;
                    }
                    zeroSeqStart = pos + 1;
                }
            }
        }
    }
    return BitArray(vlpc);
}

/** Encodes zero sequence to binary
 *  O^m - write number (m + 1) in binary, least significant bit first
 *  discard the most significant bit
 */
deque<bool> Compressor::zeroSeqEncode(int m)
{
    m += 1;
    deque<bool> binary;
    while (m > 0)
    {
        binary.push_back(m % 2);
        m /= 2;
    }
    binary.pop_back();  // Delete most significant bit
    return binary;
}

/** Converts non-zero MTF digit to binary prefix code
 */
vector<bool> Compressor::MTFToBin(int i)
{
    vector<bool> vlpc;

    // Add leading zeros
    int leadingZeros = (int)trunc(log2(i + 1));
    for (int j = 0; j < leadingZeros; j++)  
        vlpc.push_back(0);

    // Add binary rep of MTF[i]
    vector<bool> bin = intToBin(i + 1);
    for (int j = 0; j < bin.size(); j++)       
        vlpc.push_back(bin[j]);

    return vlpc;
}

/** Returns binary representation of number
 */
vector<bool> Compressor::intToBin(int a)
{
    vector<bool>binary;
    while (a > 0)
    {
        binary.push_back(a % 2);
        a /= 2;
    }
    reverse(binary.begin(), binary.end());
    return binary;
}

/** Checks if end of bucket is reached
 */
bool Compressor::isBucketEnd(int pos)
{
    if ((pos + 1) % bucketSize == 0 || pos == n - 1)
        return true;
    return false;
}

/** Checks if end of superBucket is reached
 */
bool Compressor::isSuperBucketEnd(int pos)
{
    if ((pos + 1) % superBucketSize == 0 || pos == n - 1)
        return true;
    return false;
}

/** Decodes given BitArray back to letters
 *  Used for testing purposes
 */
vector<int> Compressor::decode(const BitArray& bits) {
    
    vector<int> decoded;
    string word = "";
    for (int i = 0; i < bits.size(); i++)
    {
        char bit = bits.bitCharAt(i);
        if (bit == '0' || word != "") // Normal number
        {
            word += bit; 
            if (alphabet.containsCode(word))
            {
                int mtfCode = alphabet.decodeToMTF(word);
                cout << "prepoznao sam: " << word << " -> " << mtfCode << endl;
                // Decode from MTF here and add to solution
                decoded.push_back(mtfCode);
                word = "";
            }
            continue;
        }
        if (word == "" && bit == '1') // Zero sequence
        {
            cout << "pocinje niz nula" << endl;

            int j = 0;
            int zeroNum = 0;
            while (i < bits.size() && bits.bitCharAt(i) == '1')
            {
                string zeroCode = "1" + string(1, bits.bitCharAt(i + 1)); 
                cout << "nasao: " << zeroCode << endl;
                if (zeroCode == "11")
                   zeroNum += (1 + 1) * pow(2, j); 
                if (zeroCode == "10")
                   zeroNum += (0 + 1) * pow(2, j); 
                
                j++;
                i += 2; // Skip by 2
            }
            i--; // One back, auto ++

            // Decode fom MTF here and add to solution
            for (int k = 0; k < zeroNum; k++)
                decoded.push_back(0);
        }
    }

    return decoded;
}
