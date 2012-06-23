/** 
 *  author: Matija Sosic
 *  e-mail: matija.sosic@gmail.com
 *
 *  class Alphabet
 */

#ifndef ALPHABET_HPP
#define ALPHABET_HPP

#include <vector>
#include <list>
#include <string>
#include <map>
#include <set>

using namespace std;

/**
 * Class representing an alphabet
 */
class Alphabet
{
    public:
        // -------------------------------- Public methods --------------------------------- //
        
        /** Constructor
          * Initializes members textSize, letters and C from T
          *
          * @param T    input text
          * @param eof  EOF sign
          */
        Alphabet(const string& T, char eof);

        /** Constructor
         *  Explicitly takes letters of alphabet, sorts them ascending
         *
         *  @param letters_ Letters of alphabet
         */
        Alphabet(set<char> letters_);

        /** Returns linked list representation of alphabet
         *  Elements are lexicographically sorted
         *
         *  @return Alphabet as sorted list
         */
        list<char> toSortedList();

        /** Checks if given code exists for this alphabet
         *
         *  @param codedWord    Code that we check
         *  @returns            True if code exists
         */
        bool containsCode(const string& codedWord);

        /** Gets MTF code for given binary coded word
         *
         *  @param     codedWord Word to decode
         *  @returns    MTF code of codedWord
         */
        int decodeToMTF(const string& codedWord);

        /** Puts key and value to MTFCodes map
         *  
         *  @param binCode key
         *  @param MTFCode value
         */
        void putCode(string binCode, int MTFCode);

        /** Get index of given letter.
         *  Looks at lettersIdx structre.
         *
         *  @param c    Letter of alphabet whose index we wish to know
         *  @return     Index of c
         */
        int getLetterIdx(char c);

        /** Returns number of letters in alphabet (including EOF)
         *  
         *  @returns    Size of alphabet
         */
        int size();

        // -------------------------------- Test methods --------------------------------- //
        // Not used in implementation

        /** Delete memory usage of this structure
         *  Used for testing
         */
        void empty();

        /** Prints letters of alphabet to stdout
         */
        void printLetters();

    private:
        // -------------------------------- Data structures --------------------------------- //

        /** Contains letters of alphabet
         */
        set<char> letters;

        /** For each letter of alphabet contains its position in letters.
         *  This way we avoid having maps in bNO and sbNO - we can use vector<vector<int>> instead
         *  and reduce memory usage.
         */
        map<char, int> lettersIdx;

        /** Maps binary code of each letter to its MTF code
         *  for numbers {1, alphabet().size - 1}.
         *
         *  Initialized in constructor
         */
        map<string, int> MTFCodes;

        // -------------------------------- Private methods --------------------------------- //

        /** Initializes lettersIdx structure from letters.
         *  Called from constructor.
         *
         *  Precondition:   letters structure is loaded.
         *  Side effect:    Initializes lettersIdx structure
         */
        void mapLettersToIdx();
};

#endif // ALPHABET_HPP
