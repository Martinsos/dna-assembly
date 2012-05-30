/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#include "FMindex.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>

using namespace std;

bool testFMIndex(); // FOR TESTING

// FOR TESTING
int main()
{   /*
    if (testFMIndex())
        cout << "Test prosao dobro" << endl;
    else
        cout << "Test nije prodjen" << endl;
    */
    string T2 = "aabaaabababababbabbbab";
    string T = "";
    for (int i = 0; i < 1; i++)
        T += T2;
    
    clock_t begin, end, begin2, end2;
    
    begin = clock();
    FMindex fmIndex = FMindex(T);                                                        
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
        for (Index i = 0; i < lokacije.size(); i++)
            cout << lokacije[i] << endl;
        cout << "Broj pojavljivanja(svih)" << endl;
        cout << lokacije.size() << endl;
        printf("Vrijeme lociranja: %.5lf\n", (double)(end-begin) / CLOCKS_PER_SEC);
        
        cout << "FMindex: " << endl;
        cout << "lokacije:" << endl;
        begin = clock();
        vector<Index> locs = fmIndex.getLocations(P);
        end = clock();                              
        sort(locs.begin(), locs.end());
        for (Index i = 0; i < locs.size(); i++)
            cout << locs[i] << endl;
        cout << "Broj pojavljivanja(svih)" << endl;
        Index numOcc = fmIndex.getCount(P);
        begin2 = clock();
        cout << fmIndex.getCount(P) << endl;
        end2 = clock();
        printf("Vrijeme lociranja:     %.5lf\n", (double)(end-begin) / CLOCKS_PER_SEC);
        printf("Vrijeme samo brojanja: %.5lf\n", (double)(end2-begin2) / CLOCKS_PER_SEC);
        
        bool isto = true;
        if (lokacije.size() != numOcc)
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

// auto-test
bool testFMIndex()
{
    FMindex* fmIndex;
    vector<Index> locs;
    string T;
    string P;
    
    ///////////////
    T = "";
cout << "juhu" << endl;   
    fmIndex = new FMindex (T);
cout << "juhu" << endl;    
    P = "ab";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 0) )
        return false;
    delete fmIndex;
cout << "juhu" << endl;       
    ///////////////
    T = "aabaaabababababbabbbaab";
    fmIndex = new FMindex (T);
    
    P = "ab";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 7 && locs[0] == 2 && locs[1] == 6 && locs[2] == 10 && locs[3] == 12
                            && locs[4] == 14 && locs[5] == 17 && locs[6] == 22) )
        return false;
        
    P = "aba";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 2 && locs[0] == 6 && locs[1] == 10) )
        return false;
        
    P = "bbba";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 0) )
        return false;
    delete fmIndex;
        
    //////////////////    
    T = "aabaaabababababbabbbab";
    fmIndex = new FMindex (T);
    
    P = "ab";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 7 && locs[0] == 2 && locs[1] == 6 && locs[2] == 10 && locs[3] == 12
                            && locs[4] == 14 && locs[5] == 17 && locs[6] == 21) )
        return false;
    
    P = "aba";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 2 && locs[0] == 6 && locs[1] == 10) )
        return false;
        
    P = "bbba";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 0) )
        return false;
    delete fmIndex;
        
    ///////////////////    
    T = "aabbbbcb";
    fmIndex = new FMindex (T);
    
    P = "b";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 5 && locs[0] == 3 && locs[1] == 4 && locs[2] == 5 && locs[3] == 6
                            && locs[4] == 8) )
        return false;
        
    P = "bb";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 1 && locs[0] == 5) )
        return false;
    
    P = "f";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 0) )
        return false;
    delete fmIndex;
        
    ///////////////////    
    T = "a";
    fmIndex = new FMindex (T);
    
    P = "a";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 1 && locs[0] == 1) )
        return false;
        
    P = "b";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 0) )
        return false;
    delete fmIndex;
    
    return true;
}
