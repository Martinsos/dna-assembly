/** 
 *  author: Matija Sosic
 *  class Opp  
 */

#include <cmath>
#include <iostream>

#include "Opp.hpp"

using namespace std;

/** Constructor
 *  Initializes inner Compressor instance
 */
Opp::Opp(string& T)
{
    // Initialize textSize
    textSize = T.length() + 1; // EOF

    // Initialize compressor
    char eof =  '#';
    Alphabet alphabet(T, '#');
    compressor = new Compressor('#', alphabet, (int)log2(textSize));
    compressor->compress(T);

    // Initialize C
    list<char>::iterator it;
    list<char> letters = alphabet.toSortedList();
    for (it = letters.begin(); it != letters.end(); it++)
        C[*it] = 1; // Because of EOF

    for (int i = 0; i < T.size(); i++)
    {
        char c = T[i];
        map<char, int>::iterator it;
        for (it = C.begin(); it != C.end(); it++)
            if (c < it->first)
                it->second++;
    }

    map<char, int>::iterator mapIt;
    for (mapIt = C.begin(); mapIt != C.end(); mapIt++)
        cout << mapIt->first << " -> " << mapIt->second << endl;
}
    
/** Finds rows of conceptual matrix prefixed by P
 *  If there are no matching rows, set flag in OppRows
 *   
 *  @param P Prefix we are looking for
 *  @return OppRows class. 
 */
OppRows Opp::findRows(const string &P) const
{
    // Setting initial conditions
    int i       = P.length();       
    char c      = P[i - 1];         
    int first   = getCFor(c) + 1;  
    int last    = getCForNext(c);  

    while (first <= last && i >= 2)
    {
        c = P[i - 2]; // Because array is 0-based and here we work with base 1

        first = getCFor(c) + 1 + compressor->occ(c, first - 1);
        last = getCFor(c) + compressor->occ(c, last);

        i--;
    }
    if (last < first)
        return OppRows(0, 0, true); // 0 means undef here

    return OppRows(first, last, false);
}

/** Applies findRows() for each suffix of given string
 */
vector<OppRows> Opp::findRowsForSuffixes(const string &P) const
{
    int i       = P.length();       
    char c      = P[i - 1];        
    int first   = getCFor(c) + 1;   
    int last    = getCForNext(c);   

    vector<OppRows> results;
    if (last < first)
        results.push_back(OppRows(0, 0, true));
    else
        results.push_back(OppRows(first, last, false));
    while (first <= last && i >= 2)
    {
        c = P[i - 2]; // Because array is 0-based and here we work with base 1
cout << "novi c: " << c << endl;
        
        first = getCFor(c) + 1 + compressor->occ(c, first - 1);
        last = getCFor(c) + compressor->occ(c, last);
        i--;

        if (last < first)
            results.push_back(OppRows(0, 0, true));
        else
            results.push_back(OppRows(first, last, false));
    }
    while (results.size() < P.length())
        results.push_back(OppRows(0, 0, true));
    return results;
}

/** Like findRowsForSuffixes(), but adds prefix to each suffix before searching
 */
vector<OppRows> Opp::findRowsForSuffixesWithPrefix(const string &P, char C) const
{
    int i       = P.length();       
    char c      = P[i - 1];        
    int first   = getCFor(c) + 1;   
    int last    = getCForNext(c);   

    vector<OppRows> results;
    while (first <= last && i >= 2)
    {
        // Add special character
        c = C;
        int firstC = getCFor(c) + 1 + compressor->occ(c, first - 1);
        int lastC = getCFor(c) + compressor->occ(c, last);
        if (lastC < firstC)
            results.push_back(OppRows(0, 0, true));
        else
            results.push_back(OppRows(firstC, lastC, false));

        c = P[i - 2]; // Because array is 0-based and here we work with base 1
        cout << "novi c: " << c << endl;
        
        first = getCFor(c) + 1 + compressor->occ(c, first - 1);
        last = getCFor(c) + compressor->occ(c, last);
        i--;
    }
    while (results.size() < P.length())
        results.push_back(OppRows(0, 0, true));
    return results;
}

/** Returns C[c]
*/
int Opp::getCFor(char c) const
{
    return C.find(c)->second;
}

/** Returns C[c + 1]
*/
int Opp::getCForNext(char c) const
{
    map<char, int>::const_iterator it;
    it = C.find(c);
    
    if (it == C.end()) return textSize;
    return (++it)->second;
}
