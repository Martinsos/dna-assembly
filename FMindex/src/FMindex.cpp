/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#include "FMindex.hpp"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>
#include <utility>
#include <ctime>

#include "StringView.hpp"

using namespace std;


/**
 * Possible problem: buildTrie() returns vector<Index>,
 * if it is very big maybe it would be better to pass reference to buildTrie().
 */
FMindex::FMindex(string &T)
{
    clock_t begin, end;    
    
	// Set LZ separator.
	LZsep_ = 1;
    // Remember size of T
    n_ = T.length();
    // calculate lengthThreshold_
    if (n_ >= 16)
        lengthThreshold_ = (Index) trunc(log2(log2(n_)));
    else
        lengthThreshold_ = 16;  // if word T is shorter than 16, set lengthThreshold_ to 16 so every word P is considered short

//cout << "gradim trie" << endl;
	// We use LZ78 parsing to parse string T and build a trie. 
    // oppTLZR is also created.
begin = clock();   
	trie_ = new Trie();
	vector<Index> wordLengths = trie_->buildTrieLZ78(T, LZsep_, oppTLZR_);  // oppTLZR_ is created!
printf("Vrijeme izgradnje za Trie: %.5lf\n", (double)(clock()-begin) / CLOCKS_PER_SEC);
//cout << "trie izgradjen" << endl;		
	// create Opp(T)
begin = clock(); 
	oppT_ = new Opp(T);
printf("Vrijeme izgradnje za oppT: %.5lf\n", (double)(clock()-begin) / CLOCKS_PER_SEC);

    // create RTQ
begin = clock(); 
    buildRTQ(T, wordLengths);
printf("Vrijeme izgradnje za RTQ: %.5lf\n", (double)(clock()-begin) / CLOCKS_PER_SEC);
//cout << "RTQ gotov" << endl;    
    // create shortPatterns
    memorizeShortPatterns(T, wordLengths);
}

FMindex::~FMindex()
{
	delete trie_;
	delete oppT_;
	delete oppTLZR_;
    delete shortPatterns_;
    delete rtQ_;
}

vector<Index> FMindex::getInternal(const string &P)                                      
{
	vector<Index> locations;

	// reverse string P
	StringView PR = StringView(P);
    PR.reverse();
	
	// find rows of conceptual matrix that start with $PR
    PR.addPrefix(string(1, LZsep_));
	OppRows rows = oppTLZR_->findRows(PR);
	
	// if no rows were found, return empty vector
	if (rows.isEmpty())
		return locations;
	
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
    vector<OppRows> suffixesOfP = findSuffixesOfP(P);   // suffixesOfP[i] -> indexes of rows in OppT that start with P[1,p]
    // create prefixesOfP
    vector<OppRows> prefixesOfP = findPrefixesOfP(P);   // prefixesOfP[i] -> indexes of rows in OppTLZR that start with P[1,p]
    
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
            vector< pair<Index, Index> > vs = this->rtQ_->query(   prefixRows.getFirst(), prefixRows.getLast(), 
                                                    suffixRows.getFirst(), suffixRows.getLast() );
            // use obtained values v to calculate locations
            for (int j = 0; j < vs.size(); j++)
                if (h + vs[j].second < P.length())  // h+k must be < p, because if not then the word is not overlapping.
                    locations.push_back(vs[j].first - h);
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
    vector<OppRows> oppRows = this->oppT_->findRowsForSuffixes(StringView(P));
    reverse(oppRows.begin(), oppRows.end());
    return oppRows;
}
    
vector<OppRows> FMindex::findPrefixesOfP(const string& P)                   
{
    StringView PR = StringView(P);
	PR.reverse();
    return oppTLZR_->findRowsForSuffixesWithPrefix(PR, LZsep_);
}

void FMindex::buildRTQ(const string& T, const vector<Index>& wordLengths)
{
    // create Q and V on heap because they could be big
    vector< pair<Index, Index> >* Q = new vector< pair<Index, Index> >();
    vector< pair<Index, Index> >* V = new vector< pair<Index, Index> >();
    
    vector<OppRows> suffixes = this->oppT_->findRowsForSuffixes(T);   // rows for all suffixes
    reverse(suffixes.begin(), suffixes.end());
    
    map<string, OppRows> prefixes;  // rows for all prefixes (LZ words)
    prefixes[""] = OppRows(0,0,true);   // I am building dictionary here

    // build Q and V
    Index wordStart = 0;    // position of first character in word
clock_t begin, end;
double ukupno = 0.0;
    for (Index i = 0; i < (Index)wordLengths.size()-1; i++)   // for all words except last
    { 
        Index wordEnd = wordStart + wordLengths[i] - 1;  // position in T of last character of word
        for (Index k = 0; k < lengthThreshold_ && k < wordLengths[i]; k++)  // for each of last log(log n) positions in word
        {
            string prefix = T.substr(wordStart, wordLengths[i]-k);
            reverse(prefix.begin(), prefix.end());
            
            string prevPrefix = prefix.substr(1);
            OppRows currRows = oppTLZR_->findRowsDoStep(prefixes[prevPrefix], prefix[0]);
            prefixes[prefix] = currRows;
        
            // calculate (x,y) and add it to Q : x -> prefix, y -> suffix
begin = clock();
            Index x = this->oppTLZR_->findRowsDoStep(currRows, LZsep_).getFirst();
            Index y = suffixes[wordEnd-k+1].getFirst();
end = clock();
ukupno += (double)(clock()-begin) / CLOCKS_PER_SEC;
            Q->push_back(make_pair(x, y));
            
            // calculate v and it to V
            Index v = wordStart + wordLengths[i] + 1 - k;
            V->push_back(make_pair(v, k));
        }
        wordStart += wordLengths[i];
    }

printf("Vrijeme izvođenja za findove: %.5lf\n", ukupno);                                

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
    OppRows rows = oppT_->findRows(StringView(P));
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




