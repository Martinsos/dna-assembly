/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#ifndef RTQRTREE_HPP
#define RTQRTREE_HPP

#include "RTQ.hpp"
#include "RTree.hpp"

using namespace std;

class Point; /* implementation is at bottom */

/**
 * Implementation of RTQ using RTreeTemplate. // https://github.com/nushoin/RTree
 * Memory usage: O(|Q| * log(|Q|)).
 */
class RTQRTree : public RTQ
{
  public: 
    ~RTQRTree();
   
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
    RTree<Index, Index, 2, float> rTree;
    vector<Point*> Q;   // memory is deleted in destructor. I put Points on heap because if on stack than I have risk that they could move.
    vector< pair<Index,Index> > V;
};

/**
 * Data structure used for storing point from Q.
 */
class Point {
  public:
    Index xy[2];
    
    Point(Index x, Index y) {
        xy[0] = x;
        xy[1] = y;
    }
};

#endif // RTQRTREE_HPP
