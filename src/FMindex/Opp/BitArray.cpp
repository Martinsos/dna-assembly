/** 
 *  author: Matija Sosic
 *  e-mail: matija.sosic@gmail.com
 *
 *  class BitArray
 */

#include "BitArray.hpp"
#include <iostream>

using namespace std;

/** Empty constructor
 */
BitArray::BitArray()
{
    
}

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

void BitArray::empty() 
{
    bits.clear();    
}

/** Returns size
 */
int BitArray::size() const
{
    return bits.size();
}

/** Returns bit at given position as char
 */ 
char BitArray::bitCharAt(int pos) const
{
    if (bits[pos] == true) return '1';
    return '0';
}
