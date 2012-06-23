/** 
 *  author: Matija Sosic
 *  e-mail: matija.sosic@gmail.com
 *
 *  class Alphabet
 */

#include "Alphabet.hpp"

#include <algorithm>
#include <iostream>

using namespace std;

// -------------------------------- Public methods --------------------------------- //

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

    // Map letters to their indices
    mapLettersToIdx();
}

/** Constructor
 */
Alphabet::Alphabet(set<char> letters_)
{
    letters = letters_; 

    // Map letters to their indices
    mapLettersToIdx();
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

/** Puts (key -> value) to MTFCodes map
 */
void Alphabet::putCode(string binCode, int MTFCode)
{
    MTFCodes[binCode] = MTFCode;
}

/** Returns index in alphabet of given letter
 */
int Alphabet::getLetterIdx(char c)
{
    return lettersIdx[c];
}

/** Returns size of alphabet
 */
int Alphabet::size()
{
    return letters.size();
}

// -------------------------------- Test methods --------------------------------- //

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

// -------------------------------- Private methods --------------------------------- //

/** Map each letter to its index in set letters
 */
void Alphabet::mapLettersToIdx()
{
    set<char>::iterator it; int idx;
    for(idx = 0, it = letters.begin(); it != letters.end(); idx++, it++)
    {
        lettersIdx[*it] = idx;
    }
}
