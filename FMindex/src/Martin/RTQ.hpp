#ifndef RTQ_HPP
#define RTQ_HPP

using namespace std;

typedef int Index;

/**
 * Geometric data structure that contains set of 2D-grid points Q
 * and set of values v(j,k) where each value corresponds to one point from Q.
 * Supports orthogonal range queries in O(log(log |Q|) + q) 
 * where q is the number of points retrieved by a query.
 */
class RTQ
{
  public:
    /**
     * Builds structure RTQ.
     * @param T
     * @param wordLengths Lengths of LZ78 words, by order.
     */
    RTQ (const string& T, const vector<Index>& wordLengths);
    
    /**
     * Returns values v(j,k) of all points (x,y) from Q such that
     *  (xMin <= x <= xMax) and (yMin <= y <= yMax).
     * Time complexity: O(log(log |Q|) + q) 
     *  where q is the number of points retrieved by a query.
     */
    vector<Index> query(Index xMin, Index xMax, Index yMin, Index yMax);
};

#endif // RTQ_HPP
