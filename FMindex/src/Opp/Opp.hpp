#ifndef OPP_HPP
#define OPP_HPP

#include <string>
#include <map>

#include "../StringView.hpp"
#include "Alphabet.hpp"
#include "Compressor.hpp"
#include "OppRows.hpp"

using namespace std;

class Opp
{
  private:
    /* Data structures */

    /** Size of input text
     */
    int textSize;

    /**
     * Inner instance of compressor
     */
    Compressor* compressor;

    /** Keeps entry for each letter of alphabet.
     *  C[c] stores the total number of text characters which are alphabetically smaller than c.
     */
    map<char, int> C;

    // ----------------- findRows (backward_search) methods --------------------- //
    /** Returns C[c]
    */
    int getCFor(char c) const;

    /** Returns C[c + 1]
    */
    int getCForNext(char c) const;

  public:
    /** Constructor
     *  Gets and stores coded T.
     *  Opp will do some manipulations on given T but when function is over it guarantees T will be same as it was when given.
     */
	Opp(string& T);
	
    /** Destructor
     *  Deletes compressor instance
     */
    ~Opp();

	/**
	 * Implements backward_search algorithm.
	 * Finds all rows in T that are prefixed by P.
	 * Returns indexes of first and last row (rows are contiguous).
	 * First row is indexed with 1.
	 */	
	OppRows findRows(const StringView& P) const;
    
    /**
     * Same as method findRows() but returns rows for all suffixes of P, not just P.
     * IMPORTANT: same time complexity as oppRows (O(p+occ), not |P| * O(oppRows) as you would expect).
     * @return vector[m] corresponds to P[p-m,p]
     */
    vector<OppRows> findRowsForSuffixes(const StringView& P) const;
	
	/**
     * Same as method findRows() but returns rows for all suffixes of P prefixed with character C.
     * Example: C = '$', P = "abcd", returns rows for: $d, $cd, $bcd, $abcd
     * IMPORTANT: same time complexity as oppRows (O(p+occ), not |P| * O(oppRows) as you would expect).
     * @return vector[m] corresponds to C+P[p-m,p]
     */
    vector<OppRows> findRowsForSuffixesWithPrefix(const StringView& P, char C) const;

    /** Does only on step of function findRows for only one character
     *  Starts from given first and last (oppRows)
     *  If oppRows is empty, returns first and last row of conceptual matrix
     *
     *  @param init  define initial first and last row
     *  @param c     char for which step is done
     *  @return      calculated first and last
     */
    OppRows findRowsDoStep(const OppRows& init, char c) const;
};

#endif // OPP_HPP
