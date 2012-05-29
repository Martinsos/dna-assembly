
#include "Compressor.hpp"

#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <set>

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

void dumpVector(vector<int> a)
{
    for (int i = 0; i < a.size(); i++)
        cout << a[i] << " ";
    cout << endl;
}

int main()
{   
    string T = "pipemississippi";

    // Initialize compressor
    char eof = '#';
    set<char> letters;
    letters.insert('i');
    letters.insert('p');
    letters.insert('s');
    letters.insert('m');
    letters.insert('e');
    letters.insert(eof);
    Alphabet alpha(letters);
    
    int bucketSize = 4;
    Compressor myCompressor(eof, alpha, bucketSize);
    
    cout << "==Input text=============================================" << endl;
    cout << T << endl;
    cout << "velicina: " << T.length() << endl;
    
    cout << "==Variable length prefix encoding========================" << endl;
    BitArray compressedText = myCompressor.compress(T);
    cout << "Number of bits: " << compressedText.size() << endl;
    compressedText.print();

    cout << "==Decoded text (for testing)=============================" << endl;
    string decodedText = myCompressor.decode(compressedText);
    cout << decodedText << endl;

    cout << endl;
    cout << "==MTF States for each bucket=============================" << endl;
    vector< list<char> > MTFStates = myCompressor.getMTFStates();
    for (int i = 0; i < MTFStates.size(); i++)
    {
        cout << "Bucket " << i + 1 << ": ";
        dumpList(MTFStates[i]);
    }

    /*
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
    */
    
    cout << endl;
    cout << "==Buckets W==============================================" << endl;
    for (int i = 0; i < myCompressor.bW.size(); i++)
    {
        cout << "Bucket " << i + 1 << ": " << myCompressor.bW[i] << endl;
    }

    cout << endl;
    cout << "==Super Buckets W========================================" << endl;
    for (int i = 0; i < myCompressor.sbW.size(); i++)
    {
        cout << "Super Bucket " << i + 1 << ": " << myCompressor.sbW[i] << endl;
    }
    cout << endl;

    cout << "==Missing zeroes MZ======================================" << endl;
    for (int i = 0; i < myCompressor.MZ.size(); i++)
        cout << "MZ " << i + 1 << ": " << myCompressor.MZ[i] << endl;
    cout << endl;

    cout << "==OCC Testing============================================" << endl;
    int q = 7;
    char c = 's';
    cout << "Pojavljivanje slova " << c << " u prvih " << q << " znakova: " << myCompressor.occ(c, q) << endl;
    cout << "-------------------------------------------------------------------------" << endl;

    return 0;
}
