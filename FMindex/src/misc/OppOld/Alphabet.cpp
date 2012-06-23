/**
 * class Alphabet
 */

#include "Alphabet.hpp"

#include <algorithm>
#include <iostream>

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

/** Returns alphabet as lexicographically sorted list
 */
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

// -------------------------------- Test methods --------------------------------- //

/** Returns size of alphabet
 */
int Alphabet::size()
{
    return letters.size();
}

/** Empty alphabet - delete all memory usage by this structure
 *  This makes this structure unusable, so this method is used only for testing
 */
void Alphabet::empty()
{
    letters.clear();
    MTFCodes.clear();
}

/** Prints letters of alphabet to stdout
 */
void Alphabet::printLetters()
{
    set<char>::iterator it;
    for (it = letters.begin(); it != letters.end(); it++)
    {
        cout << *it << " ";
    }
    cout << endl;
}
