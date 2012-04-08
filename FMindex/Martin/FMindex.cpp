#include "FMindex.hpp"

using namespace std;


FMindex::FMindex(const string &T)
{
	// We use LZ78 parsing to parse string T and build a trie. 
	// We also reverse parsed string.
	string TLZR = parseLZ78(T, LZsep_, dictSize_, trie_);
	TLZR = reverse(TLZR.begin(), TLZR.end());
	
	// create Opp(TLZR) and Opp(T)
	
}

string FMindex::parseLZ78(const string &S, const char &LZsep, Index &d, Trie &trie)
{
	d = 0;
	trie = *new Trie();
	string SLZ = "";
	
	// Go through S char by char, add each char to trie and also add it to TLR.
	// When trie signals creation of new word, also add character LZsep to SLZ.
	for (int i = 0; i < S.length(); i++)
	{
		SLZ += S[i];
		if ( trie.add(S[i]) )
		{
			SLZ += "$";
			d += 1;
		}		
	}
	
	return SLZ;
}
