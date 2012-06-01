/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#include "Trie.hpp"

#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdio>

#include "StringView.hpp"

using namespace std;

const char Trie::duplicate;

Trie::Trie()
{
	root_ = NULL;
}  

Trie::~Trie()
{
	deleteSubtree(root_);
}

vector<Index> Trie::buildTrieLZ78(const string &T, char LZsep, Opp* &oppTLZR)
{
    root_ = new TrieNode(0, 0);
	LZsep_ = LZsep;
    size_ = 0;
    
	TrieNode* currNode = root_;
	vector<Index> wordLengths;
	Index wordLength = 0;	// length of LZ word
	Index startPos = 1;		// starting position of LZ word
	
    // Build trie
	for (Index i = 0; i < T.length(); i++)
	{
		wordLength++;
		if (currNode->children.find(T[i]) != currNode->children.end())  // if child with this letter exists
		{
			currNode = currNode->children[T[i]];
			if (i == T.length()-1)  // if we are looking at last word in T and same word already exists
			{
				currNode->children[Trie::duplicate] = new TrieNode(startPos, wordLength);		// add special node
                size_++;
				wordLengths.push_back(wordLength);
			}
		}
		else // if child with this letter doesn't exist, we found a new word! We create new node.
		{
			currNode->children[T[i]] = new TrieNode(startPos, wordLength);
            size_++;
			wordLengths.push_back(wordLength);
			
			startPos += wordLength;
			wordLength = 0;
			currNode = root_;
		}
	}           
	
    // Build Opp(TLZR)
     // create string TLZR
	string TLZR = "";
	Index wordI = 0;
	wordLength = 0;
	for (int i = 0; i < T.length(); i++)
	{
		wordLength++;
		TLZR += T[i];
		
		if (wordLength == wordLengths[wordI])
		{
			TLZR += LZsep;
			wordLength = 0;
			wordI++;
		}
	}  
	reverse(TLZR.begin(), TLZR.end());

     // create Opp(TLZR)
clock_t begin = clock();
    oppTLZR = new Opp(TLZR);
printf("Vrijeme izgradnje za OppTLZR: %.5lf\n", (double)(clock()-begin) / CLOCKS_PER_SEC);    
    // Maps rows from Opp(TLZR) to nodes of Trie
    this->mapRowsToNodes(*oppTLZR);
        
	return wordLengths;
}

Index Trie::getSize()
{
    return size_;
}

void Trie::mapRowsToNodes(const Opp &oppTLZR)
{
    N_.resize(size_);
    offsetN_ = oppTLZR.findRows(StringView(string(1,LZsep_))).getFirst();
	mapRowsToNodesRec(root_, OppRows(0,0,true), ' ', oppTLZR);
}

void Trie::mapRowsToNodesRec(TrieNode *node, const OppRows& prevRows, char c, const Opp &oppTLZR) 
{
    OppRows currRows = prevRows;   // rows for this node, first should be same as last
	if (node != root_)	// don't do any mapping for root
	{		
        Index rowI;
        if (c == Trie::duplicate) {      // watch out if there is an LZ word same as some other
            node->oppRow = oppTLZR.findRowsDoStep(prevRows, LZsep_).getFirst() + 1; // remember oppRow for this node, with $
            rowI = node->oppRow;  
        }
        else {
            currRows = oppTLZR.findRowsDoStep(prevRows, c);
            node->oppRow = oppTLZR.findRowsDoStep(currRows, LZsep_).getFirst();   // remember oppRow for this node, with $ 
            rowI = node->oppRow;
        }
        N_[ rowI - offsetN_ ] = node;	// map row to node;
	}
        
    map<char, TrieNode*>::iterator it;
	for (it = node->children.begin(); it != node->children.end(); it++)  // map rows for all children
        mapRowsToNodesRec(it->second, currRows, it->first, oppTLZR);
}

Index Trie::getOppRowForWord(string word)
{
    TrieNode* currNode = root_;
    for (int i = word.length()-1; i >= 0; i--) {
        currNode = currNode->children[word[i]];
        if (!currNode) return 0;
    }
    return currNode->oppRow;
}

vector<Index> Trie::getLocationsFromSubtree(Index row, Index lengthP)
{
	vector<Index> locations;	// solution goes here
	TrieNode* node = getNodeAtRow(row);

	getLocationsFromSubtreeRec(node, node->length, lengthP, locations);		// recursion that iterates through tree
	return locations;
}

void Trie::getLocationsFromSubtreeRec(TrieNode* node, Index rootLength, Index lengthP, vector<Index> &locations)
{
	 locations.push_back(node->location + rootLength - lengthP);	// add location
	 
	 map<char, TrieNode*>::iterator it;
	 for (it = node->children.begin(); it != node->children.end(); it++)  // do same for all children
		if ( !(it->first == Trie::duplicate && rootLength == node->length) )	// if child is not Trie::duplicate and also directly child of root of subtree, call recursion
			getLocationsFromSubtreeRec(it->second, rootLength, lengthP, locations);
}

TrieNode* Trie::getNodeAtRow(Index rowI)
{
    if (rowI - offsetN_ < 0 || rowI - offsetN_ >= N_.size())
        return NULL;
	return N_[rowI - offsetN_];
}

void Trie::deleteSubtree(TrieNode *node)
{
	// delete all subtrees
	map<char, TrieNode*>::iterator it;
	for (it = node->children.begin(); it != node->children.end(); it++)
		deleteSubtree(it->second);
	
    // delete self
	delete node;
}

void Trie::printTrie()
{
    printTrieRec(root_);
}

void Trie::printTrieRec(TrieNode* node)
{
    cout << "Cvor " << node << " (" << node->location << "," << node->length << ")" << endl;
    map<char, TrieNode*>::iterator it;
	for (it = node->children.begin(); it != node->children.end(); it++)
        cout << " (" << it->first << ", " << it->second << ")" << endl;
    for (it = node->children.begin(); it != node->children.end(); it++)
        printTrieRec(it->second);
}
