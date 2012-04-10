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
	map<char, TrieNode*> children;
	
	TrieNode(Index loc) { location = loc; }
};

class Trie
{
  public:
	Trie();
	
	~Trie();
  
	/**
	 * Uses LZ78 parsing to parse string into words. Builds trie from this words.
	 * Returns vector of numbers where each number is length of LZword.
	 *  For example, if "aabab" is parsed like this: "a$ab$ab$" it would return [1,2,2]
	 * It is possible that last word is same as some other word.
	 */
  	vector<Index> buildTrieLZ78(const string &T, char LZsep);
  	
  	/**
  	 * Uses given Opp structure to map certain rows of conceptual matrix to nodes of trie.
  	 * Mapping will be stored in array N.
  	 */
  	void mapRowsToNodes(const Opp &oppTLZR);		// TOTEST
  	
  	/**
  	 * Given index of row in conceptual matrix, maps that index to
  	 * a subtree using array N, goes through that subtree and returns
  	 * locations of string P in string T.
  	 */
  	vector<Index> getSubtreeAtRow(Index row);		// TODO
  
  private:
	static const char end = 0;	// used to represent last word if it is same like some other word

	TrieNode *root_;			// root is only node that doesn't contain location (location = 0)
	vector<TrieNode*> N_;		// maps rows of conceptual matrix to nodes of trie
	Index offsetN_;				// offset beetwen vector N and first row in matrix prefixed with $
	char LZsep_;				// separator used to separate LZ words
	
	/**
	 * Maps row prefixed with reverse of given word to given node. Repeats for children.
	 */
	void mapRowsToNodesRec(const Opp &oppTLZR, TrieNode *node, string &LZword);		// TOTEST
	
	/**
	 * Maps row of conceptual matrix to node of trie.
	 */
	TrieNode* getNodeAtRow(Index rowI);
	
	/**
	 * Does delete on whole subtree rooted in given node. 
	 */ 
	void deleteSubtree(TrieNode* node);
};
const char Trie::end;

#endif // TRIE_HPP
