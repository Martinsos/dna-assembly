/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#ifndef OPP_HPP
#define OPP_HPP

#include <string>
#include <vector>

using namespace std;

typedef int Index;


class OppRows
{
  public:
    OppRows(Index f, Index l, bool e)
    {
		first = f;
		last = l;
		empty = e;
	}
    
    /**
     * If there are no rows returns true, otherwise false.
     * If OppRows is empty, getFirst() and getLast() can return anything.
     */
    bool isEmpty() { return empty; }
    
    Index getFirst() { return first; }
    Index getLast() { return last; }
  
  private:
    Index first, last;
    bool empty;
};

class Opp
{
  public:
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
	
	void printOpp();   // FOR TESTING
	
  private:
	vector<string> M;
};

#endif // OPP_HPP
