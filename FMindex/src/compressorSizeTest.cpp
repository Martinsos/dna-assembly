/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#include "FMindex.hpp"

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
    if (argc != 5) {
        cout << "Usage: " << argv[0] << " <filename> <num_characters> <pattern_length> <num_patterns" << endl;
        exit(1);
    }
    string filename = string(argv[1]);
    string patternFile = string(argv[3]);
    
    long N;
    sscanf(argv[2], "%ld", &N);
    
    long p;
    sscanf(argv[3], "%ld", &p);
    long numPatterns;
    sscanf(argv[4], "%ld", &numPatterns);
    
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
    
    int i;
    cin >> i;
    return 0;
}
