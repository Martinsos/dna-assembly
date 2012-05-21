
#include "Compressor.hpp"

#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <map>

using namespace std;

void dumpList(list<char>& a)
{
    list<char>::iterator it;
    for (it = a.begin(); it != a.end(); it++)
        cout << *it << " "; 
    cout << endl;
}

void dumpMap(map<char, int>& m)
{
   map<char, int>::iterator it;
   for (it = m.begin(); it != m.end(); it++)
        cout << it->first << " " << it->second << endl;
   cout << "-----------------------------" << endl;
}

int main()
{   
    string T = "pipemississippi";

    // Initialize compressor
    char eof = '#';
    vector<char> letters;
    letters.push_back('i');
    letters.push_back('p');
    letters.push_back('s');
    letters.push_back('m');
    letters.push_back('e');
    letters.push_back(eof);
    Alphabet alpha(letters);
    
    int bucketSize = 4;
    Compressor myCompressor(eof, alpha, bucketSize);
    
    cout << "==Input text=============================================" << endl;
    cout << T << endl << endl;

    cout << "==Variable length prefix encoding========================" << endl;
    BitArray compressedText = myCompressor.compress(T);
    compressedText.print();

    cout << endl;
    cout << "==MTF States for each bucket=============================" << endl;
    vector< list<char> > MTFStates = myCompressor.getMTFStates();
    for (int i = 0; i < MTFStates.size(); i++)
    {
        cout << "Bucket " << i + 1 << ": ";
        dumpList(MTFStates[i]);
    }

    cout << endl;
    cout << "==Buckets NO=============================================" << endl;
    for (int i = 0; i < myCompressor.bNO.size(); i++)
    {
        cout << "Bucket " << i + 1 << ": " << endl;
        dumpMap(myCompressor.bNO[i]);
    }

    cout << endl;
    cout << "==SuperBuckets NO========================================" << endl;
    for (int i = 0; i < myCompressor.sbNO.size(); i++)
    {
        cout << "SuperBucket " << i + 1 << ": " << endl;
        dumpMap(myCompressor.sbNO[i]);
    }

    return 0;
}
