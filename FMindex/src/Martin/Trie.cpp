#include "Trie.hpp"

#include <iostream>
#include <algorithm>

using namespace std;

const char Trie::end;

Trie::Trie()
{
	root_ = new TrieNode(0, 0);
}  

Trie::~Trie()
{
	deleteSubtree(root_);
}

vector<Index> Trie::buildTrieLZ78(const string &T, char LZsep, Opp* &oppTLZR)
{
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
				currNode->children[Trie::end] = new TrieNode(startPos, wordLength);		// add special node
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
    oppTLZR = new Opp(TLZR);
     
    // Maps rows from Opp(TLZR) to nodes of Trie
    this->mapRowsToNodes(*oppTLZR);
     
	return wordLengths;
}

void Trie::mapRowsToNodes(const Opp &oppTLZR)
{
    N_.resize(size_);
    offsetN_ = oppTLZR.findRows(string(1,LZsep_)).getFirst();

	string word = ""; 
	mapRowsToNodesRec(root_, word, oppTLZR);
}

void Trie::mapRowsToNodesRec(TrieNode *node, string& word, const Opp &oppTLZR) 
{
	if (node != root_)	// don't do any mapping for root
	{		
        Index rowI;
        if (word[0] == Trie::end)
            rowI = oppTLZR.findRows(string(1,LZsep_)+word.substr(1)).getLast();
        else
            rowI = oppTLZR.findRows(string(1,LZsep_)+word).getFirst();
		N_[ rowI - offsetN_ ] = node;	// map row to node;
	}
    map<char, TrieNode*>::iterator it;
	for (it = node->children.begin(); it != node->children.end(); it++)  // map rows for all children
    {
        word = string(1,it->first)+word;
        mapRowsToNodesRec(it->second, word, oppTLZR);
        word = word.substr(1);	
    }
}

vector<Index> Trie::getLocationsFromSubtree(Index row, Index lengthP)
{
	vector<Index> locations;	// solution goes here
	TrieNode* node = getNodeAtRow(row);
    cout << "Row:" << row << ", Location: " << node->location << ", length: " << node->length << endl;
	getLocationsFromSubtreeRec(node, node->length, lengthP, locations);		// recursion that iterates through tree
	return locations;
}

void Trie::getLocationsFromSubtreeRec(TrieNode* node, Index rootLength, Index lengthP, vector<Index> &locations)
{
	// Pazi: kad obilazis podstablo sa korijenom K, ako je Trie::end direktno dijete od K onda njega nemoj brojati.
	// ovdje treba koristiti i duljinu od P
	 
	 locations.push_back(node->location + rootLength - lengthP);	// add location
	 
	 map<char, TrieNode*>::iterator it;
	 for (it = node->children.begin(); it != node->children.end(); it++)  // do same for all children
		if ( !(it->first == Trie::end && rootLength == node->length) )	// if child is not Trie::end and also directly child of root of subtree, call recursion
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

/*
// FOR TESTING
#include <iostream>
int main()
{
	Trie trie = Trie();
    Opp* oppTLZR;
    string T = "aabaaabababababbabbbaab";
    char LZsep = '$';
    
	vector<Index> wordLengths = trie.buildTrieLZ78(T, LZsep, oppTLZR);
	
	for (int i = 0; i < wordLengths.size(); i++)
		cout << wordLengths[i] << " ";
	cout << endl;
       
    oppTLZR->printOpp();
    cout << endl;
    
    cout << oppTLZR->findRows(string(1,LZsep)).getFirst() << endl;
    cout << oppTLZR->findRows(string(1,LZsep)).getLast() << endl;
	
    delete oppTLZR;
    
	return 0;
}
*/
