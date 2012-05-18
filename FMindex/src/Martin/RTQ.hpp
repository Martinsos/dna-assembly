#ifndef RTQ_HPP
#define RTQ_HPP

#include <vector>
#include <utility>
#include "LEDA/geo/d2_dictionary.h"

using namespace std;

typedef int Index;

class Information; /* implementation is at bottom */

/**
 * Geometric data structure that contains set of 2D-grid points Q
 * and set of values (v, k) where each value (v,k) corresponds to one point from Q.
 * Supports orthogonal range queries in O(log(log |Q|) + q) 
 * where q is the number of points retrieved by a query.
 */
class RTQ
{
  public:
    /**
     * Builds structure RTQ.
     * @param Q Set of points
     * @param V Set of values, V[i] corresponds(belongs) to Q[i]
     */
    RTQ (const vector< pair<Index,Index> >& Q, const vector< pair<Index,Index> >& V);
    
    /**
     * Returns values (v,k) from V of all points (x,y) from Q such that
     *  (xMin <= x <= xMax) and (yMin <= y <= yMax).
     * Time complexity: Ideal would be O(log(log |Q|) + q), but current implementation is (log(|Q|) + q), 
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

#endif // RTQ_HPP
