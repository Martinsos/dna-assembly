/**
 * Class Compressor
 */

#include "Compressor.hpp"

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

Compressor::Compressor(char eof)
{
    BWTEof = eof;
}

string Compressor::getBWT(const string& T)
{
    // Get suffix array of T + BWTEof
    
    // Construct BWT from SA

    return "dummy";
}

class SASort 
{
    public:
        SASort(const string& T_) : T(T_) {}

        bool operator() (Index i, Index j)
        {
            T.compare(i, 5, T, j, 4);
            return true;    
        }
    private:
        const string& T;
};

vector<int> Compressor::getSuffixArray(const string& T)
{
    // Construct initial unsorted SA
    vector<Index> SA (T.length());
    for (Index i = 0; i < SA.size(); i++)
        SA[i] = i;
    
    // Sort SA regarding T
    SASort cmpObj(T);
    sort(SA.begin(), SA.end(), cmpObj);
     
    return SA;
}   
