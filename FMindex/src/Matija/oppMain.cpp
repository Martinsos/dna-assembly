/* 
 * Main for testing opp class
 */

#include "Opp.hpp"

#include <string>
#include <iostream>

using namespace std;

int main()
{
    // Create instance of Opp
    string T = "mississippi";
    Opp myOpp(T);
    cout << "gotov konstruktor" << endl;
    
    // Get result
    OppRows result = myOpp.findRows("ssi");
    // Output result
    cout << result.getFirst() << endl;
    cout << result.getLast() << endl;
    cout << result.isEmpty() << endl;

    vector<OppRows> results = myOpp.findRowsForSuffixesWithPrefix("ssi", 'm');
    for (int i = 0; i < results.size(); i++)
        results[i].print();

    cout << "gotov sam" << endl;
    return 0;
}
