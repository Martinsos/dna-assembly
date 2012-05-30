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
    string T = "aabaaabababababbabbbab";
    Opp myOpp(T);
    cout << "gotov konstruktor" << endl;
    
    // Get result
    OppRows result = myOpp.findRows("baba");
    // Output result
    cout << result.getFirst() << endl;
    cout << result.getLast() << endl;
    cout << result.isEmpty() << endl;

    cout << "gotov sam" << endl;
    return 0;
}
