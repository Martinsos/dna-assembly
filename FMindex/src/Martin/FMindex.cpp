#include "FMindex.hpp"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>

using namespace std;


/**
 * Possible problem: buildTrie() returns vector<Index>,
 * if it is very big maybe it would be better to pass reference to buildTrie().
 */
FMindex::FMindex(const string &T)
{
	// Set LZ separator.
	LZsep_ = '$';
    // Remember size of T
    n_ = T.length();
    // calculate lengthThreshold_
    if (n_ >= 4)
        lengthThreshold_ = (Index) trunc(log2(log2(n_)));
    else
        lengthThreshold_ = 4;  // if word T is shorter than 4, set lengthThreshold_ to 4 so every word P is considered short
	
	// We use LZ78 parsing to parse string T and build a trie. 
    // oppTLZR is also created.
	trie_ = new Trie();
	vector<Index> wordLengths = trie_->buildTrieLZ78(T, LZsep_, oppTLZR_);  // oppTLZR_ is created!
		
	// create Opp(T)
	oppT_ = new Opp(T);
    
    // TODO: create RTQ
    //buildRTQ();
    
    // create shortPatterns
    memorizeShortPatterns(T, wordLengths);
}

FMindex::FMindex()
{
    FMindex("");
}

FMindex::~FMindex()
{
	delete trie_;
	delete oppT_;
	delete oppTLZR_;
    delete shortPatterns_;
}

vector<Index> FMindex::getInternal(const string &P)                                         // HM: maybe move this to Trie?
{
	vector<Index> locations;
cout << endl;
trie_->printTrie();	
cout << "Size of trie: " << trie_->getSize() << endl;
	// reverse string P
	string PR = P;
	reverse(PR.begin(), PR.end());
	
	// find rows of conceptual matrix that start with $PR
	OppRows rows = oppTLZR_->findRows(LZsep_ + PR);
	
	// if no rows were found, return empty vector
	if (rows.isEmpty())
		return locations;
	
cout << LZsep_ + P << endl;
cout << rows.getFirst() << endl;
cout << rows.getLast() << endl;
	// for each row find subtree in trie, read all locations and add them to solution.
	for (Index rowI = rows.getFirst(); rowI <= rows.getLast(); rowI++)
	{
		vector<Index> locs = trie_->getLocationsFromSubtree(rowI, P.length());
		locations.insert( locations.end(), locs.begin(), locs.end() );
	}
	
	return locations;
}

vector<Index> FMindex::getOverlapping(const string &P)
{
    if (P.length() <= lengthThreshold_)
        return getOverlappingShort(P);
    else
        return getOverlappingLong(P);
}

vector<Index> FMindex::getOverlappingLong(const string &P)
{
    return vector<Index>();
}

vector<Index> FMindex::getOverlappingShort(const string &P)
{
    map< string, vector<Index> >::iterator it;
    it = shortPatterns_->find(P);
    
    if (it != shortPatterns_->end())
        return it->second;
    else
        return vector<Index>();
}

void FMindex::memorizeShortPatterns(const string& T, const vector<Index>& wordLengths)
{
    shortPatterns_ = new map< string, vector<Index> >();
    if (wordLengths.size() == 0)
        return;  
    Index endOfWord = -1;
    Index endOfNextWord = wordLengths[0];
    for (Index i = 1; i < wordLengths.size(); i++)   // for each LZ word except last
    {
        endOfWord = endOfNextWord;
        endOfNextWord += wordLengths[i];
        for (Index patternStart = max(1, endOfWord-lengthThreshold_+2); patternStart <= endOfWord; patternStart++) // pick start of pattern            
        {
            for (Index patternEnd = min(endOfNextWord, patternStart+lengthThreshold_-1); patternEnd > max(endOfWord, patternStart); patternEnd--) // pick end of pattern
            {
                string P = T.substr(patternStart-1, patternEnd-patternStart+1);  // short pattern we found
cout << "kratki uzorak: " << P << " " << patternStart << endl;
                (*shortPatterns_)[P].push_back(patternStart); 
            }
        }
    }
}

vector<Index> FMindex::getLocations(const string &P)
{
    if (P.length() == 0)
        return vector<Index>();
    vector<Index> locs, tmp;
    tmp = getInternal(P);
    locs.insert( locs.end(), tmp.begin(), tmp.end() );
    tmp = getOverlapping(P);
    locs.insert( locs.end(), tmp.begin(), tmp.end() );
    return locs;
}

Index FMindex::getCount(const string &P)
{
    if (P.length() == 0)
        return 0;
    OppRows rows = oppT_->findRows(P);
    if (rows.isEmpty())
        return 0;
    return 1+rows.getLast()-rows.getFirst();
}

Index FMindex::max (Index a, Index b)
{
    if (b > a)
        return b;
    return a;
}

Index FMindex::min (Index a, Index b)
{
    if (b < a)
        return b;
    return a;
}


bool testFMIndex(); // FOR TESTING

// FOR TESTING
int main()
{   
 /*   if (testFMIndex())
        cout << "Test prosao dobro" << endl;
    else
        cout << "Test nije prodjen" << endl;
 */   
    string T = "aabaaabababababbabbbaab";
    
    FMindex fmIndex = FMindex(T);                                                        
    
    string P = "";
    while (true)
    {
        getline(cin, P);
        if (P == "KRAJ") break;
        vector<Index> locs = fmIndex.getLocations(P);                              
        sort(locs.begin(), locs.end());
        
        cout << endl;
        cout << "lokacije:" << endl;
        for (int i = 0; i < locs.size(); i++)
            cout << locs[i] << endl;
        cout << "Broj pojavljivanja(svih)" << endl;
        cout << fmIndex.getCount(P) << endl;
    }
    
    return 0;
}

// auto-test
bool testFMIndex()
{
    FMindex* fmIndex;
    vector<Index> locs;
    string T;
    string P;
    
    ///////////////
    T = "";
    fmIndex = new FMindex (T);
    
    P = "ab";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 0) )
        return false;
    delete fmIndex;
    
    ///////////////
    T = "aabaaabababababbabbbaab";
    fmIndex = new FMindex (T);
    
    P = "ab";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 7 && locs[0] == 2 && locs[1] == 6 && locs[2] == 10 && locs[3] == 12
                            && locs[4] == 14 && locs[5] == 17 && locs[6] == 22) )
        return false;
        
    P = "aba";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 2 && locs[0] == 6 && locs[1] == 10) )
        return false;
        
    P = "bbba";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 0) )
        return false;
    delete fmIndex;
        
    //////////////////    
    T = "aabaaabababababbabbbab";
    fmIndex = new FMindex (T);
    
    P = "ab";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 7 && locs[0] == 2 && locs[1] == 6 && locs[2] == 10 && locs[3] == 12
                            && locs[4] == 14 && locs[5] == 17 && locs[6] == 21) )
        return false;
    
    P = "aba";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 2 && locs[0] == 6 && locs[1] == 10) )
        return false;
        
    P = "bbba";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 0) )
        return false;
    delete fmIndex;
        
    ///////////////////    
    T = "aabbbbcb";
    fmIndex = new FMindex (T);
    
    P = "b";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 5 && locs[0] == 3 && locs[1] == 4 && locs[2] == 5 && locs[3] == 6
                            && locs[4] == 8) )
        return false;
        
    P = "bb";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 1 && locs[0] == 5) )
        return false;
    
    P = "f";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 0) )
        return false;
    delete fmIndex;
        
    ///////////////////    
    T = "a";
    fmIndex = new FMindex (T);
    
    P = "a";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 1 && locs[0] == 1) )
        return false;
        
    P = "b";
    locs = fmIndex->getInternal(P);
    sort(locs.begin(), locs.end());
    if ( !(locs.size() == 0) )
        return false;
    delete fmIndex;
    
    return true;
}
