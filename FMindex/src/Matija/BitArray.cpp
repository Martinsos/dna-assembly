
#include "BitArray.hpp"
#include <iostream>

using namespace std;

/** Constructor
 */
BitArray::BitArray(vector<bool> bits)
{
    this->bits = bits;
}

/** Output bits to stdout
 */
void BitArray::print()
{
    for (int i = 0; i < bits.size(); i++)
        cout << bits[i];
    cout << endl;
}

