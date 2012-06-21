/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#include "RTQRTree.hpp"

using namespace std;

RTQRTree::~RTQRTree() {
    for (int i = 0; i < Q.size(); i++)
        delete Q[i];
}

void RTQRTree::build (const vector< pair<Index,Index> >& Q, const vector< pair<Index,Index> >& V)
{
    // clear everything
    this->rTree.RemoveAll();
    this->Q.clear(); this->V.clear(); 
    
    // copy Q and V to local copies
    for (int i = 0; i < Q.size(); i++) {
        this->Q.push_back(new Point(Q[i].first, Q[i].second));
        this->V.push_back(V[i]);
    }
    
    // insert points into RTree
    for (int i = 0; i < Q.size(); i++)    
        this->rTree.Insert(this->Q[i]->xy, this->Q[i]->xy, i);
}

/**
 * This function is called each time a matching point is found.
 * It stores index of matched point into given vector.
 */
static bool searchCallback(Index i, void* indices_) {
    vector<Index>* indices = (vector<Index>*)indices_;
    indices->push_back(i);
    return true; // keep going
}

vector< pair<Index,Index> > RTQRTree::query(Index xMin, Index xMax, Index yMin, Index yMax)
{
    Index min[2] = {xMin, yMin};
    Index max[2] = {xMax, yMax};
    vector<Index> indices;
    
    // do search and store indices of matched points to vector result
    this->rTree.Search(min, max, searchCallback, &indices);
    
    // obtain data (v,k) about points with indices that we found
    vector< pair<Index,Index> > result;
    for (int i = 0; i < indices.size(); i++)
        result.push_back(this->V[indices[i]]);
    
    return result;
}
