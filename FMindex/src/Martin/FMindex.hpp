#include "Trie.hpp"

#include <vector>
#include <string>

using namespace std;


typedef unsigned int Index; // index of string T

class FMindex
{
  public:
	/**
	 * Builds FMindex for string T.
	 */
	FMindex(const string &T);
	
	/**
	 * Returns all locations of string P in string T.
	 * First character of string T is on position 1.
	 */
	vector<Index> getLocations(const string &P);
	
	/**
	 * Returns number of occurrences of string P in string T.
	 */
	Index getCount(const string &P);
	
  private:
	//OppT, OppTR treba napraviti
	Trie trie_;
	Index dictSize_;	// size of LZ78 dictionary
	char LZsep_;	// separator for LZ words
	  
	vector<Index> getInternal(const string &P);
	vector<Index> getOverlaping(const string &P);
};
