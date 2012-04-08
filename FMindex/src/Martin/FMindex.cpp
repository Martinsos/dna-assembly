#include "FMindex.hpp"

#include <algorithm>

using namespace std;


FMindex::FMindex(const string &T)
{
	// Set LZ separator.
	LZsep_ = '$';
	
	// We use LZ78 parsing to parse string T and build a trie. 
	vector<string> TLZ = trie_.buildTrieLZ78(T);
	
	// set dictionary size.
	dictSize_ = TLZ.size();
	
	// create string TLZR
	string TLZR = "";
	for (int i = 0; i < TLZ.size(); i++)
		TLZR += TLZ[i] + LZsep_;
	reverse(TLZR.begin(), TLZR.end());
	
	// TODO: create Opp(TLZR) and Opp(T).
	
	// TODO: create links beetwen matrix rows and trie.
}
