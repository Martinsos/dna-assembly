#ifndef TRIE_HPP
#define TRIE_HPP

#include "Opp.hpp"

#include <string>
#include <vector>
#include <map>

using namespace std;

typedef unsigned int Index;


/**
 * Each node represents one word.
 */
class TrieNode
{
  public:  
	Index location;		// starting position of word in string T
	Index length;		// length of word
	map<char, TrieNode*> children;
	
	TrieNode(Index loc, Index len) { location = loc; length = len; }
};

class Trie
{
  public:
	Trie();
	
	~Trie();
  
	/**
     * Takes string T and separator for separating LZ words. Separator must have lower ASCII
     * value than all characters in text T.
	 * Uses LZ78 parsing to parse string T into words. Builds trie from this words.
	 * Returns vector of numbers where each number is length of LZword,
     * also creates and returns Opp(TLZR).
	 *  For example, if "aabab" is parsed like this: "a$ab$ab$" it would return [1,2,2]
	 * It is possible that last word is same as some other word (like in example).
	 */
  	vector<Index> buildTrieLZ78(const string &T, char LZsep, Opp* &oppTLZR);
  	
  	/**
  	 * Given index of row in conceptual matrix and length of string P, maps that index to
  	 * a subtree using array N, goes through that subtree and returns
  	 * locations of string P in string T. Uses length of P to calculate the locations.
  	 */
  	vector<Index> getLocationsFromSubtree(Index row, Index lengthP);		// TOTEST
  
	static const char end = 0;	// used to represent last word if it is same like some other word
    
  private:
	TrieNode *root_;			// root is only node that doesn't contain location (location = 0)
    Index size_;                // size of Trie (without root)
	vector<TrieNode*> N_;		// maps rows of conceptual matrix to nodes of trie
	Index offsetN_;				// offset beetwen vector N and first row in matrix prefixed with $
	char LZsep_;				// separator used to separate LZ words
	
    /**
  	 * Uses given Opp structure to map certain rows of conceptual matrix to nodes of trie.
  	 * Mapping will be stored in array N.
     * WARNING: Not all rows will be mapped.
  	 */
  	void mapRowsToNodes(const Opp &oppTLZR);		// TOTEST
    
	/**
	 * Maps given row to given node and moves forward in both lists.
	 */
	void mapRowsToNodesRec(TrieNode *node, string& word, const Opp &oppTLZR); 	// TOTEST
	
	/**
	 * 
	 */
	void getLocationsFromSubtreeRec(TrieNode* node, Index rootLength, Index lengthP, vector<Index> &locations);	// TOTEST
	
	/**
	 * Maps certain rows (those starting with $) of conceptual matrix to node of trie.
     * If row with index rowI is not mapped, NULL is returned.
	 */
	TrieNode* getNodeAtRow(Index rowI);
	
	/**
	 * Does delete on whole subtree rooted in given node. 
	 */ 
	void deleteSubtree(TrieNode* node);
    
    /**
     * Prints Trie, used for testing.
     */
    public: void printTrie();
    private: void printTrieRec(TrieNode* node);
};

#endif // TRIE_HPP
