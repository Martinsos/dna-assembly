/**
 * class Alphabet
 */

#include "Alphabet.hpp"

#include <algorithm>

using namespace std;

/** Constructor
  * Initializes structures from input text T
  */
Alphabet::Alphabet(const string& T, char eof)
{
    // Initialize letters
    for (int i = 0; i < T.size(); i++)
    {
        letters.insert(T[i]);
    }
    letters.insert(eof);
}

/** Constructor
 */
Alphabet::Alphabet(set<char> letters_)
{
    letters = letters_; 
}

/** Returns size of alphabet
 */
int Alphabet::size()
{
    return letters.size();
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
