/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#ifndef FMINDEX_HPP
#define FMINDEX_HPP

#include "Opp/Opp.hpp"
#include "Trie.hpp"
#include "RTQ.hpp"

#include <vector>
#include <string>

using namespace std;

typedef int Index; // index of string T, it has to be signed or errors occur


class FMindex
{
  public:
	/**
	 * Builds FMindex for string T.
     * T will not be changed.
	 */
	FMindex(string &T);
	
	~FMindex();
	
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
    Trie* trie_;
	Opp* oppT_;
    Opp* oppTLZR_;
    map< string, vector<Index> >* shortPatterns_;    // contains locations in T for each existing short pattern that overlaps
    RTQ* rtQ_;  // geometric structure that offers fast orthogonal queries
	
	char LZsep_;		// separator for LZ words
    Index n_;           // size of T: |T|
    Index lengthThreshold_;   // if P is longer than threshold then it is considered long, otherwise short
	
  public:                                                                           // PUBLIC BECAUSE I NEED IT TO TEST, SHOULD BE PRIVATE
	/**
	 * Finds internal occurrences of string P in string T.
	 * Internal means that whole P is inside one LZ78 word of TLZ (TLZ: T parsed using LZ78).
	 */
	vector<Index> getInternal(const string &P);         
    
    /**
	 * Finds overlapping occurrences of string P in string T.
	 * Overlapping means that P is not inside just one LZ78 word of TLZ (TLZ: T parsed using LZ78).
	 */
	vector<Index> getOverlapping(const string &P);
    
    /**
     * Algorithm used when |P| > log(log |T|)
     */
    vector<Index> getOverlappingLong(const string &P);     
    
    /**
     * Algorithm used when |P| <= log(log |T|)
     */
    vector<Index> getOverlappingShort(const string &P);
    
    /**
     * Builds structure shortPatterns.
     * @param T
     * @param wordLengths Lengths of LZ78 words, by order.
     */
    void memorizeShortPatterns(const string& T, const vector<Index>& wordLengths);
    
    /**
     * Returns ranges of rows in OppT for all suffixes of P. 
     * Time complexity: O(p)
     * @return vector[m] corresponds to P[m+1,p]
     */ 
    vector<OppRows> findSuffixesOfP(const string& P);
    
    /**
     * Returns range of rows in OppTLZR for all prefixes of P. 
     * Time complexity: O(p)    // BUT IS O(p^2) currently, I don't know how to make it faster
     * @return vector[m] corresponds to P[1,m+1]
     */ 
    vector<OppRows> findPrefixesOfP(const string& P);  
    
    /**
     * Builds Q and V, creates RTQ(Q,V) and stores it into rtQ_
     */
    void buildRTQ(const string& T, const vector<Index>& wordLengths);  
    
    /**--- UTILITY ---**/
    Index max (Index a, Index b);
    Index min (Index a, Index b);
    /**---------------**/
    
  private:
    void operator = (const FMindex&); // there is no implementation: assignment can not be used
};

#endif // FMINDEX_HPP
