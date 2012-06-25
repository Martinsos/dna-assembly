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

// FOR TESTING
int main()
{   
    string T;
    cout << "Enter string T (index will be built upon it): " << endl;
    getline(cin, T);
    FMindex* fmIndex = new FMindex(T);                                                        
        
    string P = "";
    while (true)
    {      
        cout << "Enter string P (or type END to end): " << endl;  
        getline(cin, P);
        if (P == "END") break;
        
        Index numOcc = fmIndex->getCount(P);
        cout << "Number of occurences: " << numOcc << endl;
        
        vector<Index> locs = fmIndex->getLocations(P);
        sort(locs.begin(), locs.end());
        if (numOcc > 0) {
            cout << "Positions of occurences of P in T(starting from 1): " << endl;
            for (Index i = 0; i < locs.size(); i++)
                cout << locs[i] << endl;
        }
        cout << "--------------------------------------------------" << endl;
    }
    
    delete fmIndex;
    return 0;
}
