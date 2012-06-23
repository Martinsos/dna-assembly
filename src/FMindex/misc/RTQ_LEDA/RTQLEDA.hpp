/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#ifndef RTQLEDA_HPP
#define RTQLEDA_HPP

#include "RTQ.hpp"

// http://www.algorithmic-solutions.info/leda_manual/d2_dictionary.html
#include "LEDA/geo/d2_dictionary.h"

using namespace std;

class Information; /* implementation is at bottom */

/**
 * Implementation of RTQ using LEDA library.
 * Memory usage: O(|Q| * log(|Q|)).
 */
class RTQLEDA : public RTQ
{
  public:  
    /**
     * @Override
     */
    void build (const vector< pair<Index,Index> >& Q, const vector< pair<Index,Index> >& V);
    
    /**
     * @Override
     * Time complexity: Ideal would be O(log(log |Q|) + q), but this implementation is (log(|Q|) + q), 
     *  where q is the number of points retrieved by a query.
     */
    vector< pair<Index,Index> > query(Index xMin, Index xMax, Index yMin, Index yMax);
    
  private:
    leda::d2_dictionary<Index, Index, Information> D;
};

/**
 * Data structure used for storing information about points from Q.
 */
class Information {
  public:
    Index v;
    Index k;
    
    Information(Index v, Index k) {
        this->v = v;
        this->k = k;
    }
};

#endif // RTQLEDA_HPP
