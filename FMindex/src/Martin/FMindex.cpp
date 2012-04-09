#include "FMindex.hpp"

#include <algorithm>

using namespace std;

/**
 * Possible problem: I buildTrie() returns vector<string>,
 * if it is very big maybe it would be better to pass reference to buildTrie().
 */
FMindex::FMindex(const string &T)
{
	// Set LZ separator.
	LZsep_ = '$';
	
	// We use LZ78 parsing to parse string T and build a trie. 
	trie_ = new Trie();
	vector<string> TLZ = trie_->buildTrieLZ78(T);
	
	// set dictionary size.
	dictSize_ = TLZ.size();
	
	// create string TLZR
	string TLZR = "";
	for (int i = 0; i < TLZ.size(); i++)
		TLZR += TLZ[i] + LZsep_;
	reverse(TLZR.begin(), TLZR.end());
	
	//create Opp(TLZR) and Opp(T).
	oppT_ = new Opp(T);
	oppTLZR_ = new Opp(TLZR);
	
	// TODO: create links beetwen matrix rows and trie.
}

FMindex::~FMindex()
{
	delete trie_;
	delete oppT_;
	delete oppTLZR_;
}

vector<Index> FMindex::getInternal(const string &P)
{
	
}
