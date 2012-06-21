/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#include "FMindex.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <ctime>

using namespace std;

bool testFMIndex(); // FOR TESTING

// FOR TESTING
int main()
{   
    string T2 = "ababbaabbabbbbaabaaaaababbaabbbababaaabbababababababbbbabbaaababaaaabbabbabaababababbababbabababbbabbababbbbbaabbba";
    //string T2 = "$o$pej$il$ej$ar$t$un$ui$l$ai$mr$g$io$r$k$o$m$e$j$i$n$a$v";
    string T = "";
    for (int i = 0; i < 1; i++)
        T += T2;
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
        cout << "lokacije:" << endl;
  //      for (Index i = 0; i < lokacije.size(); i++)
  //          cout << lokacije[i] << endl;
        cout << "Broj pojavljivanja(svih)" << endl;
        cout << lokacije.size() << endl;
        printf("Vrijeme lociranja: %.5lf\n", (double)(end-begin) / CLOCKS_PER_SEC);
        
        cout << "FMindex: " << endl;
        cout << "lokacije:" << endl;
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
        
        cout << "locs size: " << locs.size() << endl;
        cout << "lokacije size: " << lokacije.size() << endl; 
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
    
    delete fmIndex;
    return 0;
}
