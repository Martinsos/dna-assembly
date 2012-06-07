/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#include "RTQ.hpp"

using namespace std;

RTQ::RTQ (const vector< pair<Index,Index> >& Q, const vector< pair<Index,Index> >& V)
{
    // insert all points in dictionary
    for (int i = 0; i < Q.size(); i++)
        D.insert(Q[i].first, Q[i].second, Information(V[i].first, V[i].second));
    cout << "velicina rtq: " << D.size() << endl;
}

vector< pair<Index,Index> > RTQ::query(Index xMin, Index xMax, Index yMin, Index yMax)
{
    vector< pair<Index,Index> > result;
    
    // do a range search, list is returned
    leda::list<leda::dic2_item> L = D.range_search(xMin, xMax, yMin, yMax);
    leda::dic2_item item;
    forall(item, L) {   // extract information and add it to vector
        Information inf = D.inf(item);      // OTHER WAY: Information inf = leda::LEDA_ACCESS(Information, item->inf());
        result.push_back(make_pair(inf.v, inf.k));
    }
    
    return result;
}
