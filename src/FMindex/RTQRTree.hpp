/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#ifndef RTQRTREE_HPP
#define RTQRTREE_HPP

#include "RTQ.hpp"
#include "RTree.hpp"

using namespace std;

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
    vector<Index*> Q;   // memory is deleted in destructor. Each entry is array of size 2, where first element is x and second element is y
    vector< pair<Index,Index> > V;
};


#endif // RTQRTREE_HPP
