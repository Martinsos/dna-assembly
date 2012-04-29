#ifndef FMINDEX_HPP
#define FMINDEX_HPP

#include "Opp.hpp"
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
     * Builds FMindex for string ""
     */
    FMindex();
	
	~FMindex();
	
	/**
	 * Returns all locations of string P in string T.
	 * First character of string T is on position 1.
	 */
	vector<Index> getLocations(const string &P);		// HALF-IMPLEMENTED
	
	/**
	 * Returns number of occurrences of string P in string T.
	 */
	Index getCount(const string &P);
	
  private:
	Opp *oppT_, *oppTLZR_;
	Trie *trie_;
	
	char LZsep_;		// separator for LZ words
	
  public:                                                                           // PUBLIC BECAUSE I NEED IT TO TEST, SHOULD BE PRIVATE
	/**
	 * Finds internal occurrences of string P in string T.
	 * Internal means that whole P is inside one LZ78 word of TLZ (TLZ: T parsed using LZ78).
	 */
	vector<Index> getInternal(const string &P);         // TOTEST
    
	vector<Index> getOverlaping(const string &P);		// TODO
};

#endif // FMINDEX_HPP
