#include "Trie.hpp"

#include <algorithm>

using namespace std;


Trie::Trie()
{
	root_ = new TrieNode(0, 0);
}  

Trie::~Trie()
{
	deleteSubtree(root_);
}

vector<Index> Trie::buildTrieLZ78(const string &T, char LZsep)
{
	LZsep_ = LZsep;
	TrieNode* currNode = root_;
	vector<Index> wordLengths;
	Index wordLength = 0;	// length of LZ word
	Index startPos = 1;		// starting position of LZ word
	
	for (Index i = 0; i < T.length(); i++)
	{
		wordLength++;
		if (currNode->children.find(T[i]) != currNode->children.end())  // if child with this letter exists
		{
			currNode = currNode->children[T[i]];
			if (i == T.length()-1)  // if we are looking at last word in T and same word already exists
			{
				currNode->children[Trie::end] = new TrieNode(startPos, wordLength);		// add special node
				wordLengths.push_back(wordLength);
			}
		}
		else // if child with this letter doesn't exist, we found a new word! We create new node.
		{
			currNode->children[T[i]] = new TrieNode(startPos, wordLength);
			wordLengths.push_back(wordLength);
			
			startPos += wordLength;
			wordLength = 0;
			currNode = root_;
		}
	}
	
	return wordLengths;
}

void Trie::mapRowsToNodes(const Opp &oppTLZR)
{
	OppRows rows = oppTLZR.findRows(string(1,LZsep_));	// get rows starting with $
	offsetN_ = rows.getFirst();
	
	Index rowI = rows.getFirst();  // we use it as global counter for mapRowsToNodesRec() */
	mapRowsToNodesRec(root_, rowI);
	
	return;
}

void Trie::mapRowsToNodesRec(TrieNode *node, Index &rowI) 
{
	if (node != root_)	// we don't do any mapping for root
	{		
		N_[ rowI - offsetN_ ] = node;	// map row to node;
		rowI++;
	}
	
	map<char, TrieNode*>::iterator it;
	for (it = node->children.begin(); it != node->children.end(); it++)  // map rows for all children
		mapRowsToNodesRec(it->second, rowI);
}

vector<Index> Trie::getLocationsFromSubtree(Index row, Index lengthP)
{
	vector<Index> locations;	// solution goes here
	TrieNode *node = getNodeAtRow(row);	
	getLocationsFromSubtreeRec(node, node->length, lengthP, locations);		// recursion that iterates through tree
	return locations;
}

void Trie::getLocationsFromSubtreeRec(TrieNode *node, Index rootLength, Index lengthP, vector<Index> &locations)
{
	// Pazi: kad obilazis podstablo sa korijenom K, ako je Trie::end direktno dijete od K onda njega nemoj brojati.
	// ovdje treba koristiti i duljinu od P
	
	/* Mozda ne bi bilo lose da posao trazenja internih pojavljivanja cijeli prebacim u Trie, 
	 * dakle da Trie sam pita Opp za redove prefiksirane sa $PR i onda te redove upotrijebi za
	 * obilazenje odgovarajucih podstabala. U tom slucaju bih mogao napraviti da se Opp registrira
	 * kod Trie, posto ga koristi vise puta.
	 */
	 
	 locations.push_back(node->location + rootLength - lengthP);	// add location
	 
	 map<char, TrieNode*>::iterator it;
	 for (it = node->children.begin(); it != node->children.end(); it++)  // do same for all children
		if ( !(it->first == Trie::end && rootLength == node->length) )	// if child is not Trie::end and also directly child of root of subtree, call recursion
			getLocationsFromSubtreeRec(it->second, rootLength, lengthP, locations);
}

TrieNode* Trie::getNodeAtRow(Index rowI)
{
	return N_[rowI - offsetN_];
}

void Trie::deleteSubtree(TrieNode *node)
{
	// delete all subtrees
	map<char, TrieNode*>::iterator it;
	for (it = node->children.begin(); it != node->children.end(); it++)
		deleteSubtree(it->second);
	
	delete node;
}


// FOR TESTING
#include <iostream>
int main()
{
	Trie trie = Trie();
	vector<Index> ls = trie.buildTrieLZ78("aabaaabababababbabbbaab", '$');
	
	for (int i = 0; i < ls.size(); i++)
		cout << ls[i] << " ";
	cout << endl;
	
	return 0;
}
