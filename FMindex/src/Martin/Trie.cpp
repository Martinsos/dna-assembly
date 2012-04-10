#include "Trie.hpp"

#include <algorithm>

using namespace std;


Trie::Trie()
{
	root_ = new TrieNode(0);
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
				currNode->children[Trie::end] = new TrieNode(startPos);		// add special node
				wordLengths.push_back(wordLength);
			}
		}
		else // if child with this letter doesn't exist, we found a new word! We create new node.
		{
			currNode->children[T[i]] = new TrieNode(startPos);
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
	offsetN_ = oppTLZR.findRows(string(1,LZsep_)).getFirst();
	
	string LZwordR = "";	
	mapRowsToNodesRec(oppTLZR, root_, LZwordR);
	
	return;
}

void Trie::mapRowsToNodesRec(const Opp &oppTLZR, TrieNode *node, string &LZwordR) 
{
	if (LZwordR != "")	// we don't do anything for root
	{		
		Index rowI = oppTLZR.findRows(LZsep_ + LZwordR).getFirst();
		N_[ rowI - offsetN_ ] = node;
	}
	
	map<char, TrieNode*>::iterator it;
	for (it = node->children.begin(); it != node->children.end(); it++)
	{
		LZwordR = it->first + LZwordR;
		mapRowsToNodesRec(oppTLZR, it->second, LZwordR);
		LZwordR = LZwordR.substr(1);
	}
	
	return;
}

vector<Index> Trie::getSubtreeAtRow(Index row)
{
	// TODO
	return vector<Index>();
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
