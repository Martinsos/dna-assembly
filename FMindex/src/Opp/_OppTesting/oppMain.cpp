/** 
 *  author: Matija Sosic
 *  e-mail: matija.sosic@gmail.com
 *
 *  Main for testing opp class
 */

#include "Opp.hpp"
#include "../StringView.hpp"

#include <string>
#include <iostream>
#include <ctime>

using namespace std;

int main()
{
    // Create instance of Opp
    string T = "abbabab";
    
    string veliki = "";
    for (int i = 0; i < 2; i++)
        veliki += T;

    cout << "velicina ulaznog teksta: " << veliki.length() << endl;

    clock_t begin = clock();
    Opp myOpp(veliki);
    clock_t end = clock();

    cout << (double)(end - begin) / CLOCKS_PER_SEC << endl;
    cout << "gotov konstruktor" << endl;

    // Get result
    StringView sw("ab");
    OppRows result = myOpp.findRows(sw);
    // Output result
    cout << result.getFirst() << endl;
    cout << result.getLast() << endl;
    cout << result.isEmpty() << endl;

    cout << "gotov sam" << endl;
    return 0;
}
