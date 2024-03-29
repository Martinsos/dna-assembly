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
        /** Constructor
          * Initializes members textSize, letters and C from T
          *
          * @param T    input text
          * @param eof  EOF sign
          */
        Alphabet(const string& T, char eof);

        /** Constructor
         *  Sorts letters ascending
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
         *  @params     codedWord Word to decode
         *  @returns    MTF code of codedWord
         */
        int decodeToMTF(const string& codedWord);

        /** Puts key and value to MTFCodes map
         *  
         *  @params binCode key
         *  @params MTFCode value
         */
        void putCode(string binCode, int MTFCode);

        // -------------------------------- Test methods --------------------------------- //

        /** Returns number of letters in alphabet (including EOF)
         *  
         *  @returns    Size of alphabet
         */
        int size();
        
        /** Delete memory usage of this structure
         *  Used for testing of memory usage
         */
        void empty();

        /** Prints letters of alphabet to stdout
         */
        void printLetters();

    private:
        /** Contains letters of alphabet
         */
        set<char> letters;

        /** Maps binary code to MTF code
         *  for numbers {1, alphabet().size - 1}.
         *
         *  Initialized in constructor
         */
        map<string, int> MTFCodes;
};

#endif // ALPHABET_HPP
