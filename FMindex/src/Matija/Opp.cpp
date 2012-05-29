/** 
 *  author: Matija Sosic
 *  class Opp  
 */

#include "Opp.hpp"

using namespace std;

/** Constructor
 *  Initializes inner Compressor instance
 */
Opp::Opp(const string& T)
{
    // Do nothing for now
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
    //int first   = C[c] + 1;
    //int last    = C[c + 1];

    return OppRows(1, 3, false);
}

/** Applies findRows() for each suffix of given string
 */
vector<OppRows> Opp::findRowsForSuffixes(const string &P) const
{
    vector<OppRows> ret;
    ret.push_back(OppRows(1, 3, false));
}

/** Like findRowsForSuffixes(), but adds prefix to each suffix before searching
 */
vector<OppRows> Opp::findRowsForSuffixesWithPrefix(const string &P, char C) const
{
    vector<OppRows> ret;
    ret.push_back(OppRows(1, 3, false));
}
