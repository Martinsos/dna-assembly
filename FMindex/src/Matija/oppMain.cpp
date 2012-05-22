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
    Opp myOpp("string for constructor");
    
    // Get result
    OppRows result = myOpp.findRows("some prefix");
    // Output result
    cout << result.getFirst() << endl;
    cout << result.getLast() << endl;
    cout << result.isEmpty() << endl;

    cout << "gotov sam" << endl;
    return 0;
}
