#include "FMindex.hpp"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>
#include <utility>

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

    // create RTQ
    buildRTQ(T, wordLengths);
    
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
    vector<Index> locations;
    
    // create suffixesOfP
    vector<OppRows> suffixesOfP = findSuffixesOfP(P);
/*oppT_->printOpp(); 
cout << "suffixesOfP:";
for (int i = 0; i < suffixesOfP.size(); i++)
    cout << " [" << suffixesOfP[i].getFirst() << ", " << suffixesOfP[i].getLast() << "]";
cout << endl;*/
    // create prefixesOfP
    vector<OppRows> prefixesOfP = findPrefixesOfP(P);
/*oppTLZR_->printOpp();
cout << "prefixesOfP:";
for (int i = 0; i < prefixesOfP.size(); i++)
    cout << " [" << prefixesOfP[i].getFirst() << ", " << prefixesOfP[i].getLast() << "]";
cout << endl;*/
    
    // for each h = i*log(log n)+1, i from 0 to trunc(p/log(log n)), do query on RTQ
    Index iMax = (Index)trunc(P.length() / (double)this->lengthThreshold_); 
    Index hMax = iMax * lengthThreshold_ + 1;                        
    for (Index h = 1; h <= hMax && h < suffixesOfP.size(); h+=lengthThreshold_)     // is condition h < suffixesOfP.size() neccesary? I think it is.
    {
        OppRows prefixRows = prefixesOfP[h-1];  // h-1 instead of h because vector prefixesOfP starts from 0 while P starts from 1
        OppRows suffixRows = suffixesOfP[h];    // h instead of h+1 because vector suffixesOfP starts from 0 while P starts from 1
            
        if (!prefixRows.isEmpty() && !suffixRows.isEmpty())
        {
            // query RTQ: [prefix_rows] x [suffix_rows] : values v(x,y) are obtained
            vector<Index> vs = this->rtQ_->query(   prefixRows.getFirst(), prefixRows.getLast(), 
                                                    suffixRows.getFirst(), suffixRows.getLast() );
            // use obtained values v to calculate locations
            for (int j = 0; j < vs.size(); j++)     
                locations.push_back(vs[j] - h);
        }
    }
    
    return locations;  
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
    for (Index i = 1; i < wordLengths.size(); i++)   // for each separator of LZ words
    {
        endOfWord = endOfNextWord;
        endOfNextWord += wordLengths[i];
        for (Index patternStart = max(1, endOfWord-lengthThreshold_+2); patternStart <= endOfWord; patternStart++) // pick start of pattern            
            for (Index patternEnd = min(endOfNextWord, patternStart+lengthThreshold_-1); patternEnd > max(endOfWord, patternStart); patternEnd--) // pick end of pattern
            {
                string P = T.substr(patternStart-1, patternEnd-patternStart+1);  // short pattern we found
                (*shortPatterns_)[P].push_back(patternStart);   // add found pattern to vector
            }
    }
}

vector<OppRows> FMindex::findSuffixesOfP(const string& P) 
{
    vector<OppRows> oppRows = this->oppT_->findRowsForSuffixes(P);
    reverse(oppRows.begin(), oppRows.end());
    return oppRows;
}
    
vector<OppRows> FMindex::findPrefixesOfP(const string& P)
{
    string PR = P;
	reverse(PR.begin(), PR.end());
    return this->oppTLZR_->findRowsForSuffixes(PR);
}

void FMindex::buildRTQ(const string& T, const vector<Index>& wordLengths)   // DANGEROUS: I MAKE MANY COPIES OF STRING -> WE COULD FASTEN THAT UP
{
    // create Q and V on heap because they could be big
    vector< pair<Index, Index> >* Q = new vector< pair<Index, Index> >();
    vector<Index>* V = new vector<Index>();
    
    // build Q and V
    Index wordStart = 0;    // position of first character in word
    for (Index i = 0; i < wordLengths.size()-1; i++)   // for all words except last
    {
        Index wordEnd = wordStart + wordLengths[i] - 1;  // position in T of last character of word
        for (Index k = 0; k < lengthThreshold_ && k < wordLengths[i]; k++)  // for each of last log(log n) positions in word
        {
            Index prefixEnd = wordEnd-k;  // position in T of last character of prefix
            string prefix = T.substr(wordStart, prefixEnd+1);
            string suffix = T.substr(prefixEnd+1);
cout << prefix << " " << suffix << endl;            
            // calculate (x,y) and add it to Q
            // TODO
            // calculate v and it to V
            // TODO
        }
        wordStart += wordLengths[i];
    }
    
    // create RTQ from Q and V
    this->rtQ_ = new RTQ(*Q, *V);
                                
    // delete Q and V
    delete Q;
    delete V;
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
