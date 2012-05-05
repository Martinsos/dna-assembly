/**
 * class Alphabet
 */

#include "Alphabet.hpp"

#include <algorithm>

using namespace std;

Alphabet::Alphabet(vector<char> letters_)
{
    letters = letters_; 
    sort(letters.begin(), letters.end());
}

vector<char> Alphabet::toSortedVector()
{
    return letters;
}

list<char> Alphabet::toSortedList()
{
    return list<char>(letters.begin(), letters.end());
}
