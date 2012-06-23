/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#ifndef RTQ_HPP
#define RTQ_HPP

#include <vector>
#include <utility>

using namespace std;

typedef int Index;

/**
 * Interface for RTQ geometric structure.
 * RTQ is geometric data structure that contains set of 2D-grid points Q
 * and set of values (v, k) where each value (v,k) corresponds to one point from Q.
 * Supports orthogonal range queries in Q and returns corresponding data (v,k) for each matched point.
 */
class RTQ
{
  public:
    virtual ~RTQ(){}
  
    /**
     * Builds structure RTQ.
     * @param Q Set of points
     * @param V Set of values, V[i] corresponds(belongs) to Q[i]
     */
    virtual void build (const vector< pair<Index,Index> >& Q, const vector< pair<Index,Index> >& V) = 0;
    
    /**
     * Returns values (v,k) from V of all points (x,y) from Q such that
     *  (xMin <= x <= xMax) and (yMin <= y <= yMax).
     */
    virtual vector< pair<Index,Index> > query(Index xMin, Index xMax, Index yMin, Index yMax) = 0;
};

#endif // RTQ_HPP
