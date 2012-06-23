/**
 * @author Matija Sosic (matija.sosic@gmail.com)
 */

#include "Opp/Opp.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <ctime>

using namespace std;

// FOR TESTING
int main(int argc, char** argv)
{   
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <filename> <num_characters>" << endl;
        exit(1);
    }
    string filename = string(argv[1]);
    
    long N;
    sscanf(argv[2], "%ld", &N);
    
    ifstream file(filename.c_str());
    if (!file.is_open())
    {
        cout << filename << " se ne moze otvoriti!" << endl;
        exit(0);
    }
    
    string T = "";
    char c;
    long n = N;
    while (file.good() && n > 0)     // loop while extraction from file is possible
    {
        file.get(c);
        if (c < 2)
            continue;
        if (!file) break;
        T.append(1, c);    
        n--;
    }
    file.close();
    
    cout << "Broj ucitanih znakova: " << T.length() << endl;
    
    clock_t begin = clock();
    Opp opp(T);
    clock_t end = clock();

    cout << "Vrijeme izvodenja: " << (double)(end - begin) / CLOCKS_PER_SEC << endl;

    
    // ------------------------ Memory usage testing ------------------------- //
    /*
    char dummy;
    
    // 1. bit array Z
    cout << "Press to delete Z:" << endl;
    cin >> dummy;

    cout << "size of Z is: " << opp.compressor->Z.size() << endl;
    opp.compressor->Z.empty();
    cout << "Z deleted!" << endl;

    // 2. MTFStates
    cout << "Press to delete MTFStates" << endl;
    cin >> dummy;

    cout << "Size of MTFStates is: " << opp.compressor->MTFStates.size() << endl;
    opp.compressor->MTFStates.clear();
    cout << "MTF states deleted!" << endl;

    // 3. Alhpabet
    cout << "Press to delete Alphabet" << endl;
    cin >> dummy;

    cout << "Size of Alphabet is: " << opp.compressor->alphabet.size() << endl;
    cout << "Alphabet letters are: " << endl;
    opp.compressor->alphabet.printLetters();

    opp.compressor->alphabet.empty();
    cout << "Alphabet deleted!" << endl;

    // 4. SuperBucket
    cout << "Press to delete sbNO: " << endl;
    cin >> dummy;

    cout << "size of sbNO is: " << opp.compressor->sbNO.size() << endl;
    opp.compressor->sbNO.clear();
    cout << "sbNO deleted!" << endl;
    // --
    cout << "Press to delete sbW: " << endl;
    cin >> dummy;

    cout << "size of sbW is: " << opp.compressor->sbW.size() << endl;
    opp.compressor->sbW.clear();
    cout << "sbW deleted!" << endl;

    // 5. Bucket
    cout << "Press to delete bNO: " << endl;
    cin >> dummy;

    cout << "size of bNO is: " << opp.compressor->bNO.size() << endl;
    opp.compressor->bNO.clear();
    cout << "bNO deleted!" << endl;
    // --
    cout << "Press to delete sbW: " << endl;
    cin >> dummy;

    cout << "size of bW is: " << opp.compressor->bW.size() << endl;
    opp.compressor->bW.clear();
    cout << "bW deleted!" << endl;
    */
    cout << "gotov!" << endl;

    return 0;
}
