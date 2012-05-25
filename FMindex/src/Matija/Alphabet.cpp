/**
 * class Alphabet
 */

#include "Alphabet.hpp"

#include <algorithm>

using namespace std;

/** Constructor
 *
 *  Intializes MTFCodes map
 */
Alphabet::Alphabet(vector<char> letters_)
{
    letters = letters_; 
    sort(letters.begin(), letters.end());
}

/** Returns size of alphabet
 */
int Alphabet::size()
{
    return letters.size();
}

vector<char> Alphabet::toSortedVector()
{
    return letters;
}

list<char> Alphabet::toSortedList()
{
    return list<char>(letters.begin(), letters.end());
}

/** Checks if given code exists for this alphabet
 */
bool Alphabet::containsCode(const string& codedWord)
{
    return MTFCodes.find(codedWord) != MTFCodes.end();
}

/** Decodes given binary code to MTF number
 */
int Alphabet::decodeToMTF(const string& codedWord)
{
    return MTFCodes[codedWord];
}

/** Puts key - value to MTFCodes map
 */
void Alphabet::putCode(string binCode, int MTFCode)
{
    MTFCodes[binCode] = MTFCode;
}
