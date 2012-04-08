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
  Trie& trie_;
  Index dictSize_;	// size of LZ78 dictionary
  char LZsep_ = '$';	// separator for LZ words
  
  /**
   * Parses string S using LZ78 parsing.
   * Returns new string with special sign LZsep separating LZ78 words,
   *  returns number of LZ78 words through reference d and also
   *  returns trie through reference trie.
   */
  string parseLZ78(const string &S, const char &LZsep, Index &d, Trie &trie);
  
  vector<Index> getInternal(const string &P);
  vector<Index> getOverlaping(const string &P);
};
