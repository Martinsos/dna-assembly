#include "RTQ.hpp"

using namespace std;

// MOCK IMPLEMENTATION
#include <iostream>

RTQ::RTQ (const vector< pair<Index,Index> >& Q, const vector< pair<Index,Index> >& V)
{
//cout << "Q:" << endl;
//    for (int i = 0; i < Q.size(); i++)
//        cout << "(" << Q[i].first << ", " << Q[i].second << ")" << endl;
//cout << "V:" << endl;
//    for (int i = 0; i < V.size(); i++)
//        cout << "(" << V[i].first << ", " << V[i].second << ")" << endl;
    this->Q = Q;
    this->V = V;
}

vector< pair<Index,Index> > RTQ::query(Index xMin, Index xMax, Index yMin, Index yMax)
{
//cout << "Query: " << endl;
//cout << xMin << endl;
//cout << xMax << endl;
//cout << yMin << endl;
//cout << yMax << endl;
    vector< pair<Index,Index> > result;
    for (int i = 0; i < Q.size(); i++) 
    {
        pair<Index,Index> point = Q[i];
        if (point.first >= xMin && point.first <= xMax && point.second >= yMin && point.second <= yMax)
            result.push_back(this->V[i]);
    }
    return result;
}
