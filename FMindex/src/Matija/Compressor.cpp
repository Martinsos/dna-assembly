/**
 * Class Compressor
 */

#include "Compressor.hpp"

#include <algorithm>

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
 *  Appends BWTEof to the end of T - that's why T is not const
 *
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

vector<int> Compressor::getMTF(const string& L)
{
    // Construct initial MTF list
    list<char> MTFList = alphabet.toList();
    
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
