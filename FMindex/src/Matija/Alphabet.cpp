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

vector<char> Alphabet::toVector()
{
    return letters;
}

list<char> Alphabet::toList()
{
    return list<char>(letters.begin(), letters.end());
}
