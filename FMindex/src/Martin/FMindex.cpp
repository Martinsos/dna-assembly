#include "FMindex.hpp"

#include <iostream>
#include <algorithm>

using namespace std;


/**
 * Possible problem: buildTrie() returns vector<Index>,
 * if it is very big maybe it would be better to pass reference to buildTrie().
 */
FMindex::FMindex(const string &T)
{
	// Set LZ separator.
	LZsep_ = '$';
	
	// We use LZ78 parsing to parse string T and build a trie. 
    // oppTLZR is also created.
	trie_ = new Trie();
	vector<Index> wordLengths = trie_->buildTrieLZ78(T, LZsep_, oppTLZR_);
	
	// set dictionary size.
	dictSize_ = wordLengths.size();
		
	// create Opp(T)
	oppT_ = new Opp(T);
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
	OppRows rows = oppTLZR_->findRows(LZsep_ + PR);
	
	// if no rows were found, return empty vector
	if (rows.isEmpty())
		return locations;
	
    cout << LZsep_ + P << endl;
    cout << rows.getFirst() << endl;
    cout << rows.getLast() << endl;
	// for each row find subtree in trie, read all locations and add them to solution.
	for (Index rowI = rows.getFirst(); rowI <= rows.getLast(); rowI++)
	{
		vector<Index> locs = trie_->getLocationsFromSubtree(rowI, P.length());
		locations.insert( locations.end(), locs.begin(), locs.end() );
	}
	
trie_->printTrie();
	return locations;
}

vector<Index> FMindex::getLocations(const string &P)
{
    return getInternal(P);
}

// FOR TESTING
int main()
{
    string T = "aabaaabababababbabbbaab";
    string P = "ab";
    FMindex fmIndex = FMindex (T);
    
    vector<Index> locs = fmIndex.getLocations(P);
    
    cout << endl;
    cout << "lokacije:" << endl;
    for (int i = 0; i < locs.size(); i++)
        cout << locs[i] << endl;
    
    return 0;
}
