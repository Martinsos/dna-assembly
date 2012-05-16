#ifndef RTQ_HPP
#define RTQ_HPP

#include <vector>
#include <utility>

using namespace std;

typedef int Index;

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
    RTQ (const vector< pair<Index,Index> >& Q, const vector< pair<Index,Index> >& V);        // TODO
    
    /**
     * Returns values (v,k) from V of all points (x,y) from Q such that
     *  (xMin <= x <= xMax) and (yMin <= y <= yMax).
     * Time complexity: O(log(log |Q|) + q) 
     *  where q is the number of points retrieved by a query.
     */
    vector< pair<Index,Index> > query(Index xMin, Index xMax, Index yMin, Index yMax);    // TODO
    
  private:
    vector< pair<Index,Index> > Q;  // MOCK IMPLEMENTATION
    vector< pair<Index,Index> > V;  // MOCK IMPLEMENTATION
};

#endif // RTQ_HPP
