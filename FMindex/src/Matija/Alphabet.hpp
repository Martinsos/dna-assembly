#ifndef ALPHABET_HPP
#define ALPHABET_HPP

#include <vector>
#include <list>

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

        /** Returns vector representation of alphabet
         */
        vector<char> toSortedVector();
        
        /** Returns linked list representation of alphabet
         */
        list<char> toSortedList();

    private:
        vector<char> letters;
};

#endif // ALPHABET_HPP
