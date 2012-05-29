#ifndef OPP_HPP
#define OPP_HPP

#include <string>

#include "Compressor.hpp"
#include "OppRows.hpp"

using namespace std;

class Opp
{
  private:
    /* Data structures */
    Compressor* compressor;

    /* Methods */

  public:
    /** Constructor
     *  Gets and stores coded T
     */
	Opp(const string &T);
	
	/**
	 * Implements backward_search algorithm.
	 * Finds all rows in T that are prefixed by P.
	 * Returns indexes of first and last row (rows are contiguous).
	 * First row is indexed with 1.
	 */	
	OppRows findRows(const string &P) const;
    
    /**
     * Same as method findRows() but returns rows for all suffixes of P, not just P.
     * IMPORTANT: same time complexity as oppRows (O(p+occ), not |P| * O(oppRows) as you would expect).
     * @return vector[m] corresponds to P[p-m,p]
     */
    vector<OppRows> findRowsForSuffixes(const string &P) const;
	
	/**
     * Same as method findRows() but returns rows for all suffixes of P prefixed with character C.
     * Example: C = '$', P = "abcd", returns rows for: $d, $cd, $bcd, $abcd
     * IMPORTANT: same time complexity as oppRows (O(p+occ), not |P| * O(oppRows) as you would expect).
     * @return vector[m] corresponds to C+P[p-m,p]
     */
    vector<OppRows> findRowsForSuffixesWithPrefix(const string &P, char C) const;
};

#endif // OPP_HPP
