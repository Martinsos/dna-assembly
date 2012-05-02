#ifndef OPP_HPP
#define OPP_HPP

#include <string>

using namespace std;

typedef unsigned int Index;

/* Result */
class OppRows
{
  public:
    OppRows(Index first, Index last, bool Empty)
    {
        this->first = first;
        this->last  = last;
        this->empty = empty;
    }
    
    /**
     * If there are no rows returns true, otherwise false.
     * If OppRows is empty, getFirst() and getLast() can return anything.
     */
    bool isEmpty()
    {
        return empty;
    }
    
    Index getFirst()
    {
        return first;
    }

    Index getLast()
    {
        return last;
    }

  private:
    Index first, last;
    bool empty;
};

class Opp
{
  private:
    /* Data structures */
    string Z;   // Contains compressed input text

    /* Methods */

  public:
    /**
     * Get BWT of T
     */
	Opp(const string &T);
	
	/**
	 * Implements backward_search algorithm.
	 * Finds all rows in T that are prefixed by P.
	 * Returns indexes of first and last row (rows are contiguous).
	 * First row is indexed with 1.
	 */	
	OppRows findRows(const string &P);
};

#endif // OPP_HPP
