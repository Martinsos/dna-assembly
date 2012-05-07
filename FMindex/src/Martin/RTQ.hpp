#ifndef RTQ_HPP
#define RTQ_HPP

#include <string>
#include <vector>
#include <utility>

using namespace std;

typedef int Index;

/**
 * Geometric data structure that contains set of 2D-grid points Q
 * and set of values V where each value v(x,y) corresponds to one point from Q.
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
    RTQ (const vector< pair<Index,Index> >& Q, const vector<Index>& V);        // TODO
    
    /**
     * Returns values v(x,y) from V of all points (x,y) from Q such that
     *  (xMin <= x <= xMax) and (yMin <= y <= yMax).
     * Time complexity: O(log(log |Q|) + q) 
     *  where q is the number of points retrieved by a query.
     */
    vector<Index> query(Index xMin, Index xMax, Index yMin, Index yMax);    // TODO
};

#endif // RTQ_HPP
