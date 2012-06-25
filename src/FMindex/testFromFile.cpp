/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#include "FMindex.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <ctime>

using namespace std;

// FOR TESTING
int main(int argc, char** argv)
{   
    if (argc != 5) {
        cout << "Usage: " << argv[0] << " <filename> <num_characters> <pattern_length> <num_patterns>" << endl;
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
    
  
    
    srand ( time(NULL) );
  
    vector<string> patterns;
    for (int i = 0; i < numPatterns; i++)
    {
        long pos = trunc(rand() * N / (double)RAND_MAX);
        if (pos + p >= T.length()) {
            i--;
            continue;
        }
        
        patterns.push_back(T.substr(pos, p));
    }
  
    
    clock_t begin, end, begin2, end2;
    
    begin = clock();
    FMindex* fmIndex = new FMindex(T);                                                        
    end = clock();
    printf("Vrijeme izgradnje: %.5lf\n", (double)(end-begin) / CLOCKS_PER_SEC);
    
   
    double locating = 0.0;
    double counting = 0.0;
    for (int i = 0; i < patterns.size(); i++)
    {        
        begin = clock();
        vector<Index> locs = fmIndex->getLocations(patterns[i]);
        end = clock();
        locating += (double)(end-begin) / CLOCKS_PER_SEC;                              

        begin = clock();
        Index count = fmIndex->getCount(patterns[i]);
        end = clock();
        counting += (double)(end-begin) / CLOCKS_PER_SEC; 
    }
    
    printf("Ukupno vrijeme lociranja za %d uzoraka duljine %ld: %.8lf\n", (int)patterns.size(), p, locating);
    printf("Ukupno vrijeme brojanja  za %d uzoraka duljine %ld: %.8lf\n", (int)patterns.size(), p, counting);
    printf("Prosjecno vrijeme lociranja po uzorku: %.8lf\n", locating / patterns.size());
    printf("Prosjecno vrijeme brojanja  po uzorku: %.8lf\n", counting / patterns.size());

    delete fmIndex;
    return 0;
}
