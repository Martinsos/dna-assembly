/* 
 * Main for testing opp class
 */

#include "Opp.hpp"
#include "../StringView.hpp"

#include <string>
#include <iostream>

using namespace std;

int main()
{
    // Create instance of Opp
    string T = "$o$pej$il$ej$ar$t$un$ui$l$ai$mr$g$io$r$k$o$m$e$j$i$n$a$v";
    cout << "velicina ulaznog teksta: " << T.length() << endl;

    Opp myOpp(T);
    cout << "gotov konstruktor" << endl;
    
    // Get result
    StringView sw("$v");
    OppRows result = myOpp.findRows(sw);
    // Output result
    cout << result.getFirst() << endl;
    cout << result.getLast() << endl;
    cout << result.isEmpty() << endl;

    cout << "gotov sam" << endl;
    return 0;
}
