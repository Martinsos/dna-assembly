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
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <filename> <num_characters>" << endl;
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
    while (file.good() && N > 0)     // loop while extraction from file is possible
    {
        file.get(c);
        if (c < 2)
            continue;
        if (!file) break;
        T.append(1, c);    
        N--;
    }
    file.close();
    
    cout << T << endl;
    cout << T.length() << endl;
    clock_t begin, end, begin2, end2;
    
    begin = clock();
    FMindex* fmIndex = new FMindex(T);                                                        
    end = clock();
    printf("Vrijeme izgradnje: %.5lf\n", (double)(end-begin) / CLOCKS_PER_SEC);
    
    
    string P = "";
    while (true)
    {        
        getline(cin, P);
        if (P == "KRAJ") break;
        
        
        cout << "Brutfors: " << endl;
        vector<Index> lokacije;
        begin = clock();
        for (Index i = 0; i+P.length() <= T.length(); i++)
            if (T.substr(i, P.length()) == P)
                lokacije.push_back(i+1);
        end = clock();
  //      cout << "lokacije:" << endl;
  //      for (Index i = 0; i < lokacije.size(); i++)
  //          cout << lokacije[i] << endl;
        cout << "Broj pojavljivanja(svih)" << endl;
        cout << lokacije.size() << endl;
        printf("Vrijeme lociranja: %.5lf\n", (double)(end-begin) / CLOCKS_PER_SEC);
        
        cout << endl;
        cout << "FMindex: " << endl;
  //      cout << "lokacije:" << endl;
        begin = clock();
        vector<Index> locs = fmIndex->getLocations(P);
        end = clock();                              
        sort(locs.begin(), locs.end());
  //      for (Index i = 0; i < locs.size(); i++)
  //          cout << locs[i] << endl;
        cout << "Broj pojavljivanja(svih)" << endl;
        Index numOcc = fmIndex->getCount(P);
        begin2 = clock();
        cout << fmIndex->getCount(P) << endl;
        end2 = clock();
        printf("Vrijeme lociranja:     %.5lf\n", (double)(end-begin) / CLOCKS_PER_SEC);
        printf("Vrijeme samo brojanja: %.5lf\n", (double)(end2-begin2) / CLOCKS_PER_SEC);
        
        cout << "lokacije size: " << lokacije.size() << endl; 
        cout << "locs size: " << locs.size() << endl;
        bool isto = true;
        if (lokacije.size() != numOcc || lokacije.size() != locs.size())
            isto = false;
        else {
            bool isto = true;
            for (int i = 0; i < lokacije.size(); i++)
                if (lokacije[i] != locs[i])
                    isto = false;
        }
        if (isto)
            cout << "ISTO JE!" << endl;
        else
            cout << "NIJE ISTO!" << endl;
    }
    
    return 0;
}
