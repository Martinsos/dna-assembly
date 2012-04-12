#include "FMindex.hpp"

#include <algorithm>

using namespace std;


/**
 * Possible problem: buildTrie() returns vector<string>,
 * if it is very big maybe it would be better to pass reference to buildTrie().
 */
FMindex::FMindex(const string &T)
{
	// Set LZ separator.
	LZsep_ = '$';
	
	// We use LZ78 parsing to parse string T and build a trie. 
	trie_ = new Trie();
	vector<Index> wordLengths = trie_->buildTrieLZ78(T, LZsep_);
	
	// set dictionary size.
	dictSize_ = wordLengths.size();
	
	// create string TLZR
	string TLZR = "";
	Index wordI = 0;
	Index wordLength = 0;
	for (int i = 0; i < T.length(); i++)
	{
		wordLength++;
		TLZR += T[i];
		
		if (wordLength == wordLengths[wordI])
		{
			TLZR += LZsep_;
			wordLength = 0;
			wordI++;
		}
	}
	reverse(TLZR.begin(), TLZR.end());
	
	// create Opp(TLZR) and Opp(T).
	oppT_ = new Opp(T);
	oppTLZR_ = new Opp(TLZR);
	
	// create mapping beetwen matrix rows and trie nodes.
	trie_->mapRowsToNodes(*oppTLZR_);
}

FMindex::~FMindex()
{
	delete trie_;
	delete oppT_;
	delete oppTLZR_;
}

vector<Index> FMindex::getInternal(const string &P)
{
	vector<Index> locations;
	
	// reverse string P
	string PR = P;
	reverse(PR.begin(), PR.end());
	
	// find rows of conceptual matrix that start with $PR
	OppRows rows = oppTLZR_->findRows(LZsep_ + P);
	
	// if no rows were found, return empty vector
	if (rows.isEmpty())
		return locations;
	
	// for each row find subtree in trie, read all locations and add them to solution.
	for (Index rowI = rows.getFirst(); rowI <= rows.getLast(); rowI++)
	{
		vector<Index> locs = trie_->getSubtreeAtRow(rowI, P.length());
		locations.insert( locations.end(), locs.begin(), locs.end() );
	}
	
	return locations;
}
