#include "Opp.hpp"

#include <algorithm>
#include <iostream>

using namespace std;

Opp::Opp(const string &T)
{
	M.push_back(T);
	for (int i = 1; i < T.length(); i++)
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

void Opp::printOpp()
{
	for (int i = 0; i < M.size(); i++)
		cout << M[i] << endl;
}
/*
int main()
{
	Opp opp = Opp("mississipi");
	opp.printOpp();
	OppRows oppR = opp.findRows("si");
	cout << oppR.getFirst() << " " << oppR.getLast() << endl;
	return 0;
}
*/
