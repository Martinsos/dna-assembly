#include <string>

using namespace std;

typedef unsigned int Index;

class OppRows
{
  public:
    OppRows(Index first, Index last, bool isEmpty);
    
    /**
     * If there are no rows returns true, otherwise false.
     * If OppRows is empty, getFirst() and getLast() can return anything.
     */
    bool isEmpty();
    
    Index getFirst();
    Index getLast();
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
	OppRows findRows(const string &P);
};
