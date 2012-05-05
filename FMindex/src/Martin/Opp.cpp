#include "Opp.hpp"

#include <algorithm>

using namespace std;

Opp::Opp(const string &T)
{
    string T_ = T+"#";
	M.push_back(T_);
	for (int i = 1; i < T_.length(); i++)
	{
		string last = M[M.size()-1];
		M.push_back(last.substr(1)+last[0]);  // rotate to left
	}
	
	sort(M.begin(), M.end());
}

OppRows Opp::findRows(const string &P) const
{
	Index first = 0;
	Index last = 0;
	for (int i = 0; i < M.size(); i++)
		if ( M[i].substr(0, P.length()) == P )
		{
			if (first == 0)
				first = i+1;
			last = i+1;
		}
	if (first == 0)
		return OppRows(0,0,true);
	return OppRows(first,last,false);		
}

vector<OppRows> Opp::findRowsForSuffixes(const string &P) const
{
    vector<OppRows> rows;
    for (int i = P.length()-1; i >= 0; i--)
        rows.push_back( findRows(P.substr(i, P.length()-i)) );
    return rows;
}

#include <iostream>
void Opp::printOpp()
{
	for (int i = 0; i < M.size(); i++)
		cout << i+1 << " " << M[i] << endl;
}

