/**
 * @author Martin Sosic (sosic.martin@gmail.com) & Matija Sosic (matija.sosic@gmail.com)
 */

#include "FMindex/FMindex.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <ctime>

using namespace std;

int main(int argc, char** argv)
{   
    FMindex* fmIndex;
    clock_t begin, end, begin2, end2;
    double locatingTime = 0.0;    
    double fullTime = 0.0;
    
  //-- check arguments --//
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <dna_filepath> <patterns_filepath> <output_filepath>" << endl;
        exit(1);
    }
    string dnaFilepath = string(argv[1]);
    string patternsFilepath = string(argv[2]);
    string outputFilepath = string(argv[3]);
  //-- --// 
    
    
  //-- open and read dna, then build index upon it. --//
    // open file
    ifstream dnaFile(dnaFilepath.c_str());
    if (!dnaFile.is_open()) {
        cout << dnaFilepath << " could not be opened!" << endl;
        exit(0);
    }
    
    // read file
    string T = "";
    char c;
    while (dnaFile.good())     // loop while extraction from file is possible
    {
        dnaFile.get(c);
        if ( !(c=='A' || c=='C' || c=='T' || c=='G' || c=='N') )    // read only characters that are A, C, T, G
            continue;
        if (!dnaFile) break;
        T.append(1, c);    
    }
    dnaFile.close();
    
    // build FMindex
    begin = clock();
    fmIndex = new FMindex(T);                                                        
    end = clock();
    printf("FMindex building time: %.5lf\n", (double)(end-begin) / CLOCKS_PER_SEC);
  //-- --//
     
     
  //-- open and read patterns, do locate for each pattern, write result to output file --//
  begin2 = clock();
    // open patterns file
    ifstream patternsFile(patternsFilepath.c_str());
    if (!patternsFile.is_open()) {
        cout << patternsFilepath << " could not be opened!" << endl;
        exit(0);
    }
    // open output file
    ofstream outputFile(outputFilepath.c_str());
    if (!outputFile.is_open()) {
        cout << outputFilepath << " could not be opened!" << endl;
        exit(0);
    }        
    
    // read from file
    while (patternsFile.good()) {    // loop while extraction from file is possible
        string P;
        // read pattern
        getline (patternsFile, P);
        
        // locate pattern
        begin = clock();
        vector<Index> locs = fmIndex->getLocations(P);
        end = clock();
        locatingTime += (double)(end-begin) / CLOCKS_PER_SEC; 
    }
    
    outputFile.close();
    patternsFile.close();
  end2 = clock();
  fullTime += (double)(end2-begin2) / CLOCKS_PER_SEC; 
  //-- --//
    
    printf("Time spent only locating: %.8lf\n", locatingTime);
    printf("Time spent on second part (all except building): %.8lf\n", fullTime);
    
    int dummy;
    cin >> dummy;
    
    delete fmIndex;
    return 0;
}
