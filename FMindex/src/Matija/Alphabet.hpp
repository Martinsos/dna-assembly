#ifndef ALPHABET_HPP
#define ALPHABET_HPP

#include <vector>
#include <list>
#include <string>
#include <map>

using namespace std;

/**
 * Class representing an alphabet
 */
class Alphabet
{
    public:
        /** Constructor
         *  Sorts letters ascending
         */
        Alphabet(vector<char> letters_);

        /** Returns number of letters in alphabet (including EOF)
         *  
         *  @returns    Size of alphabet
         */
        int size();

        /** Returns sorted vector representation of alphabet
         */
        vector<char> toSortedVector();
        
        /** Returns linked list representation of alphabet
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

    private:
        /** Contains letters of alphabet
         */
        vector<char> letters;
        
        /** Maps binary code to MTF code
         *  for numbers {1, alphabet().size - 1}.
         *
         *  Initialized in constructor
         */
        map<string, int> MTFCodes;

        

};

#endif // ALPHABET_HPP
