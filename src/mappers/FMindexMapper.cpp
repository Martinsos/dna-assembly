/**
 * @author Martin Sosic (sosic.martin@gmail.com) & Matija Sosic (matija.sosic@gmail.com)
 */

#include "FMindex/FMindex.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <ctime>
#include <utility>
#include <cstdio>

#include "util/SmithWaterman.h"
#include "util/Utility.h"

using namespace std;
using namespace fer::util;

template <class T>
void removeUnique(vector<T>& v) {
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
}

string getReversedComplement(const string& S);
pair<double, pair<int, int> > getBestCandidate(const vector< pair<int, int> >& candidates, 
                                               const vector< pair<int, int> >& candidatesRc, 
                                               const string& P, const string& dna);
void getCandidatesStrict(string P, FMindex* fmIndex, vector< pair<int, int> >& candidates);
void getCandidatesFast(string P, FMindex* fmIndex, vector< pair<int, int> >& candidates);
void getCandidatesFastRecursive(string P, FMindex* fmIndex, vector< pair<int, int> >& candidates);
void getCandidatesWindow(string P, FMindex* fmIndex, vector< pair<int, int> >& candidates);
void getCandidatesWindowRecursive(string P, FMindex* fmIndex, vector< pair<int, int> >& candidates);

int main(int argc, char** argv)
{   
    FMindex* fmIndex;
    clock_t begin, end, begin2, end2;
    double locatingTime = 0.0;    
    double fullTime = 0.0;
    int numFound = 0;
    string dummy;
    
    
  //-- check arguments --//
    if (argc != 5) {
        cout << "Usage: " << argv[0] << " <dna_filepath> <patterns_filepath> <output_filepath> <num_patterns>" << endl;
        exit(1);
    }
    string dnaFilepath = string(argv[1]);
    string patternsFilepath = string(argv[2]);
    string outputFilepath = string(argv[3]);
    int numPatterns;
    sscanf(argv[4], "%d", &numPatterns);
  //-- --// 
    
    
    
  //-- open and read dna, then build index upon it. --//
    // open file
    ifstream dnaFile(dnaFilepath.c_str());
    if (!dnaFile.is_open()) {
        cout << dnaFilepath << " could not be opened!" << endl;
        exit(0);
    }
    
    // read file
    string dna = "";
    char c;
     // skip first line, it is header
    getline(dnaFile, dummy);
    while (dnaFile.good())     // loop while extraction from file is possible
    {        
        dnaFile.get(c);
        if ( !(c=='A' || c=='C' || c=='T' || c=='G' || c=='N') )    // read only characters that are A, C, T, G
            continue;
        if (!dnaFile) break;
        dna.append(1, c);    
    }
    dnaFile.close();
    
    // build FMindex
    begin = clock();
    fmIndex = new FMindex(dna);                                                        
    end = clock();
    printf("FMindex building time: %.5lf\n", (double)(end-begin) / CLOCKS_PER_SEC);
  //-- --//
     
     
     
  //-- open and read patterns, do locate for each pattern, write result to output file --//
    begin2 = clock();
    // open patterns file
    ifstream patternsFile(patternsFilepath.c_str());
    if (!patternsFile.is_open()) {
        cout << patternsFilepath << " could not be opened!" << endl;
        exit(0);
    }
    // open output file
    ofstream outputFile(outputFilepath.c_str());
    if (!outputFile.is_open()) {
        cout << outputFilepath << " could not be opened!" << endl;
        exit(0);
    }        

    int num = 0;
    // read from file
    while (patternsFile.good()) {    // loop while extraction from file is possible
        num++;
        string P, Prc;
        // read pattern
        getline (patternsFile, P);
        if (P.length() == 0)
            continue;
        // get reversed complement
        Prc = getReversedComplement(P); 
        
        // find candidates for our pattern
       begin = clock();
        vector< pair<int, int> > candidates;
        getCandidatesFastRecursive(P, fmIndex, candidates);
        removeUnique(candidates);
        
        vector< pair<int, int> > candidatesRc;
        getCandidatesFastRecursive(Prc, fmIndex, candidatesRc);
        removeUnique(candidatesRc);
       end = clock();
       locatingTime += (double)(end-begin) / CLOCKS_PER_SEC; 
        
        
        // find best candidate and store it into variable bestCandidate
        pair<double, pair<int, int> > bestCandidate   = getBestCandidate(candidates, candidatesRc, P, dna);
        if (bestCandidate.second.first != -1)
            numFound++;
        
        // write to file
        char buffer[1024];
        sprintf(buffer, "from %d to %d score %lf", bestCandidate.second.first, bestCandidate.second.second, bestCandidate.first);
        outputFile << buffer << endl;
        
        printf("\r%d/%d", num, numPatterns);
        fflush(stdout);
    }
    printf("\n");

    outputFile.close();
    patternsFile.close();
    end2 = clock();
    fullTime += (double)(end2-begin2) / CLOCKS_PER_SEC; 
  //-- --//
    
    printf("Number of patterns found: %d\n", numFound);
    printf("Time spent finding candidates: %.8lf\n", locatingTime);
    printf("Time spent on second part (finding candidates + SW): %.8lf\n", fullTime);
    
    delete fmIndex;
    return 0;
}


//------------------------------------------------------------------------------------------------//


string getReversedComplement(const string& S) {
    string R = S;
    reverse(R.begin(), R.end());
    for (int i = 0; i < R.length(); i++) {
        if      (R[i] == 'A') R[i] = 'T';
        else if (R[i] == 'T') R[i] = 'A';
        else if (R[i] == 'G') R[i] = 'C';
        else if (R[i] == 'C') R[i] = 'G';
    }
    return R;
}

/**
 * Uses SW to pick best candidate, returns score and candidate: (score, (from, to))
 */
pair<double, pair<int, int> > getBestCandidate(const vector< pair<int, int> >& candidates, const vector< pair<int, int> >& candidatesRc, const string& P, const string& dna) {
    pair<double, pair<int, int> > bestCandidate = make_pair(-1.0, make_pair(-1, -1));
    if (candidates.size() == 0 && candidatesRc.size() == 0) 
        return bestCandidate;

    // use SW to get scores for candidates, take the best one
     // convert vector of pairs to char**
    int candidatesStrLen = candidates.size()+candidatesRc.size();
    char* candidatesStr[candidatesStrLen];
    for (int i = 0; i < candidates.size(); i++) {
        int start = candidates[i].first;
        int len = candidates[i].second - candidates[i].first + 1;
        
        char* candidate = new char[len+1];
        for (int j = 0; j < len; j++)
            candidate[j] = dna[start+j];
        candidate[len] = '\0';
        candidatesStr[i] = candidate;
    }
    for (int i = 0; i < candidatesRc.size(); i++) {
        int start = candidatesRc[i].first;
        int len = candidatesRc[i].second - candidatesRc[i].first + 1;
        string candRcStr = dna.substr(start, len);
        string candStr = getReversedComplement(candRcStr);
        
        char* candidate = new char[len+1];
        for (int j = 0; j < len; j++)
            candidate[j] = candStr[j];
        candidate[len] = '\0';
        candidatesStr[i+candidates.size()] = candidate;
    }
    
    // calculate scores
    vector<double> scores;
    char pattern[P.length()+1]; for (int i = 0; i < P.length(); i++) pattern[i] = P[i]; pattern[P.length()]='\0';
    smithWaterman(&scores, pattern, candidatesStr, (int)candidatesStrLen);
    
    // find best score ergo best candidate
    for (int i = 0; i < scores.size(); i++) {
        if (bestCandidate.second.first == -1 || scores[i] > bestCandidate.first)
            if (i < candidates.size())
                bestCandidate = make_pair(scores[i], candidates[i]);
            else
                bestCandidate = make_pair(scores[i], candidatesRc[i-candidates.size()]);
    }
    
    // cleanup    
    for (int i = 0; i < candidatesStrLen; i++)
        delete[] candidatesStr[i];
    
    return bestCandidate;
}


/**
 * Returns all position in FMindex that are possible candidates for location of P.
 * Adds pairs to vector, where each pair is: (index of first character, index of last character), starting from 0.
 * Allows only one error, finds exactly does with one error.
 */
void getCandidatesStrict(string P, FMindex* fmIndex, vector< pair<int, int> >& candidates) {
    // P with no error  
    vector<int> locations = fmIndex->getLocations(P);
    for (int i = 0; i < locations.size(); i++)
        candidates.push_back(make_pair(locations[i]-1, locations[i]-1+P.length()-1));
    
    // P with error
    char bases[] = {'A', 'T', 'G', 'C'};
    for (int i = 0; i < P.length(); i++) {
        char originalBase = P[i];
        for (int baseI = 0; baseI < 4; baseI++) // simulate error
            if (originalBase != bases[baseI]) {
                P[i] = bases[baseI];    // make error
                vector<int> locations = fmIndex->getLocations(P); // find candidates
                for (int j = 0; j < locations.size(); j++)
                    candidates.push_back(make_pair(locations[j]-1, locations[j]-1+P.length()-1));    // add candidates to vector
                P[i] = originalBase;
            }
    }
}

/**
 * Returns all position in FMindex that are possible candidates for location of P.
 * Adds pairs to vector, where each pair is: (index of first character, index of last character), starting from 0.
 * Find preffix and suffix or middle.
 */
void getCandidatesFast(string P, FMindex* fmIndex, vector< pair<int, int> >& candidates) {
    int k = 30*P.length()/100; // length of prefix(L) and suffix(R)
    int e = 7*P.length()/100;  // extra space allowed
    int len = P.length();
    string L = P.substr(0, k);  // left part(prefix)
    string R = P.substr(P.length()-k); // right part(suffix)
    string M = P.substr(k, P.length()-2*k); // middle part
    
    // find locations of L and R
    vector<int> locsL = fmIndex->getLocations(L);
    sort(locsL.begin(), locsL.end());
    for (int i = 0; i < locsL.size(); i++) locsL[i]--;    // because getLocations start from 1 and we need from 0
    vector<int> locsR = fmIndex->getLocations(R);
    sort(locsR.begin(), locsR.end());
    for (int i = 0; i < locsR.size(); i++) locsR[i]--;
    
    // find possible pairs of L and R
    int iL = 0; int iR = 0;
    while (iL < locsL.size() && iR < locsR.size()) {
        int endL = locsL[iL]+k-1;
        int startR = locsR[iR];
        int gap = startR-endL-1; 
        
        while ((gap < 0) || (gap < len-2*k-e)) { // while R is too close (gap is too small)
            iR++;
            if (iR >= locsR.size())
                break;
            startR = locsR[iR];
            gap = startR-endL-1;
        }
        if (iR >= locsR.size())
            break;
        // now R at iR is distant enough
        
        // we go throuth all Rs that are close enough
        for (int iRcurr = iR; (gap <= len-2*k+e);) {
            // we found good pair of L and R: add candidate
            int from = locsL[iL];
            int to = locsR[iRcurr]+k-1;
                candidates.push_back(make_pair(from, to));
            
            // go to next R
            iRcurr++;
            if (iRcurr >= locsR.size()) break;
            startR = locsR[iRcurr];
            gap = startR-endL-1;
        }
        
        iL++;
    }
    
    // find possible middles
    vector<int> locsM = fmIndex->getLocations(M);
    for (int i = 0; i < locsM.size(); i++) locsM[i]--;
    // add them to candidates
    for (int i = 0; i < locsM.size(); i++) {
        int from = locsM[i]-k;
        int to = from+P.length()-1;
            candidates.push_back(make_pair(from, to));
    }
}

/**
 * Returns all position in FMindex that are possible candidates for location of P.
 * Adds pairs to vector, where each pair is: (index of first character, index of last character), starting from 0.
 * Find preffix and suffix or middle but recursively.
 */
void getCandidatesFastRecursive(string P, FMindex* fmIndex, vector< pair<int, int> >& candidates) {
    int k = 20*P.length()/100; // length of prefix(L) and suffix(R)
    int e = 7*P.length()/100;  // extra space allowed
    int len = P.length();
    string L = P.substr(0, k);  // left part(prefix)
    string R = P.substr(P.length()-k); // right part(suffix)
    string M = P.substr(k, P.length()-2*k); // middle part
    
    // find locations of L and R
    vector< pair<int, int> > locsL;
    getCandidatesFast(L, fmIndex, locsL);
    sort(locsL.begin(), locsL.end());

    vector< pair<int, int> > locsR;
    getCandidatesFast(R, fmIndex, locsR);
    sort(locsR.begin(), locsR.end());
    
    // find possible pairs of L and R
    int iL = 0; int iR = 0;
    while (iL < locsL.size() && iR < locsR.size()) {
        int endL = locsL[iL].second;
        int startR = locsR[iR].first;
        int gap = startR-endL-1; 
        
        while ((gap < 0) || (gap < len-2*k-e)) { // while R is too close (gap is too small)
            iR++;
            if (iR >= locsR.size())
                break;
            startR = locsR[iR].first;
            gap = startR-endL-1;
        }
        if (iR >= locsR.size())
            break;
        // now R at iR is distant enough
        
        // we go through all Rs that are close enough
        for (int iRcurr = iR; (gap <= len-2*k+e);) {
            // we found good pair of L and R: add candidate
            int from = locsL[iL].first;
            int to = locsR[iRcurr].second;
            if (to-from+1 <= P.length()+e && to-from+1 >= P.length()-e)
                candidates.push_back(make_pair(from, to));
            
            // go to next R
            iRcurr++;
            if (iRcurr >= locsR.size()) break;
            startR = locsR[iRcurr].first;
            gap = startR-endL-1;
        }
        
        iL++;
    }
    
    // find possible middles
    vector< pair<int, int> > locsM;
    getCandidatesFast(M, fmIndex, locsM);
    // add them to candidates
    for (int i = 0; i < locsM.size(); i++) {
        int from = locsM[i].first-k;
        int to = locsM[i].second+k;
        if (to-from+1 <= P.length()+e && to-from+1 >= P.length()-e)
            candidates.push_back(make_pair(from, to));
    }
}

/**
 * Returns all position in FMindex that are possible candidates for location of P.
 * Adds pairs to vector, where each pair is: (index of first character, index of last character), starting from 0.
 * Moves window that tries to guess where is error.
 */
void getCandidatesWindow(string P, FMindex* fmIndex, vector< pair<int, int> >& candidates) {
    int k = 50*P.length()/100; // length of window
    int step = max(k*50/100, 1); // step by which window moves
    int e = 5*P.length()/100;  // extra space allowed
    int len = P.length();
    
    // move window step by step (avoid case when there is nothing left or right of window)
    for (int winStart = step; winStart+k < P.length(); winStart+=step) {
        int winEnd = winStart+k-1;
        string L = P.substr(0, winStart);   // part left of window
        int l = L.length();
        string R = P.substr(winEnd+1);      // part right of window
        int r = R.length();
        string W = P.substr(winStart, winEnd-winStart+1);   // window

        // find locations of L and R
        vector<int> locsL = fmIndex->getLocations(L);
        sort(locsL.begin(), locsL.end());
        for (int i = 0; i < locsL.size(); i++) locsL[i]--;    // because getLocations start from 1 and we need from 0
        vector<int> locsR = fmIndex->getLocations(R);
        sort(locsR.begin(), locsR.end());
        for (int i = 0; i < locsR.size(); i++) locsR[i]--;

        // find possible pairs of L and R
        int iL = 0; int iR = 0;
        while (iL < locsL.size() && iR < locsR.size()) {
            int endL = locsL[iL]+l-1;
            int startR = locsR[iR];
            int gap = startR-endL-1; 
            
            while ((gap < 0) || (gap < len-(l+r)-e)) { // while R is too close (gap is too small)
                iR++;
                if (iR >= locsR.size())
                    break;
                startR = locsR[iR];
                gap = startR-endL-1;
            }
            if (iR >= locsR.size())
                break;
            // now R at iR is distant enough
            
            // we go through all Rs that are close enough
            for (int iRcurr = iR; (gap <= len-(l+r)+e);) {
                // we found good pair of L and R: add candidate
                int from = locsL[iL];
                int to = locsR[iRcurr]+r-1;
                    candidates.push_back(make_pair(from, to));
                
                // go to next R
                iRcurr++;
                if (iRcurr >= locsR.size()) break;
                startR = locsR[iRcurr];
                gap = startR-endL-1;
            }
            
            iL++;
        }
    /*    
        // find possible middles -> reversed window
        vector<int> locsW = fmIndex->getLocations(W);
        for (int i = 0; i < locsW.size(); i++) locsW[i]--;
        // add them to candidates
        for (int i = 0; i < locsW.size(); i++) {
            int from = locsW[i]-l;
            int to = from+P.length()-1;
            candidates.push_back(make_pair(from, to));
        }*/
    }
    
    // case when window is at beginning -> there is nothing left
    string R = P.substr(k);
    vector<int> locsR = fmIndex->getLocations(R);
    for (int i = 0; i < locsR.size(); i++) locsR[i]--;  // because it should start from 0 not 1
    for (int i = 0; i < locsR.size(); i++) {
        int from = locsR[i]-k;
        int to = from+P.length()-1;
        candidates.push_back(make_pair(from, to));
    }
    
    // case when window is at end -> there is nothing right
    string L = P.substr(P.length()-k);
    vector<int> locsL = fmIndex->getLocations(L);
    for (int i = 0; i < locsL.size(); i++) locsL[i]--;  // because it should start from 0 not 1
    for (int i = 0; i < locsL.size(); i++) {
        int from = locsL[i];
        int to = from+P.length()-1;
        candidates.push_back(make_pair(from, to));
    }
}

/**
 * Returns all position in FMindex that are possible candidates for location of P.
 * Adds pairs to vector, where each pair is: (index of first character, index of last character), starting from 0.
 * Moves window that tries to guess where is error.
 */
void getCandidatesWindowRecursive(string P, FMindex* fmIndex, vector< pair<int, int> >& candidates) {
    int k = 40*P.length()/100; // length of window
    int step = max(k*50/100, 1); // step by which window moves
    int e = 5*P.length()/100;  // extra space allowed
    int len = P.length();
    
    // move window step by step (avoid case when there is nothing left or right of window)
    for (int winStart = step; winStart+k < P.length(); winStart+=step) {
        int winEnd = winStart+k-1;
        string L = P.substr(0, winStart);   // part left of window
        int l = L.length();
        string R = P.substr(winEnd+1);      // part right of window
        int r = R.length();
        string W = P.substr(winStart, winEnd-winStart+1);   // window

        // find locations of L and R
        vector< pair<int, int> > locsL;
        getCandidatesFast(L, fmIndex, locsL);
        sort(locsL.begin(), locsL.end());
    
        vector< pair<int, int> > locsR;
        getCandidatesFast(R, fmIndex, locsR);
        sort(locsR.begin(), locsR.end());

        // find possible pairs of L and R
        int iL = 0; int iR = 0;
        while (iL < locsL.size() && iR < locsR.size()) {
            int endL = locsL[iL].second;
            int startR = locsR[iR].first;
            int gap = startR-endL-1; 
            
            while ((gap < 0) || (gap < len-(l+r)-e)) { // while R is too close (gap is too small)
                iR++;
                if (iR >= locsR.size())
                    break;
                startR = locsR[iR].first;
                gap = startR-endL-1;
            }
            if (iR >= locsR.size())
                break;
            // now R at iR is distant enough
            
            // we go through all Rs that are close enough
            for (int iRcurr = iR; (gap <= len-(l+r)+e);) {
                // we found good pair of L and R: add candidate
                int from = locsL[iL].first;
                int to = locsR[iRcurr].second;
                if (to-from+1 <= P.length()+e && to-from+1 >= P.length()-e)
                    candidates.push_back(make_pair(from, to));
                
                // go to next R
                iRcurr++;
                if (iRcurr >= locsR.size()) break;
                startR = locsR[iRcurr].first;
                gap = startR-endL-1;
            }
            
            iL++;
        }
        
        // find possible middles -> reversed window
        vector< pair<int, int> > locsW;
        getCandidatesFast(W, fmIndex, locsW);
        // add them to candidates
        for (int i = 0; i < locsW.size(); i++) {
            int from = locsW[i].first-l;
            int to = locsW[i].second+r;
            if (to-from+1 <= P.length()+e && to-from+1 >= P.length()-e)
                candidates.push_back(make_pair(from, to));
        }
    }
    
    // case when window is at beginning -> there is nothing left
    string R = P.substr(k);
    vector< pair<int, int> > locsR;
    getCandidatesFast(R, fmIndex, locsR);
    for (int i = 0; i < locsR.size(); i++) {
        int from = locsR[i].first-k;
        int to = locsR[i].second;
        if (to-from+1 <= P.length()+e && to-from+1 >= P.length()-e)
            candidates.push_back(make_pair(from, to));
    }
    
    // case when window is at end -> there is nothing right
    string L = P.substr(P.length()-k);
    vector< pair<int, int> > locsL;
    getCandidatesFast(L, fmIndex, locsL);
    for (int i = 0; i < locsL.size(); i++) {
        int from = locsL[i].first;
        int to = locsL[i].second+k;
        if (to-from+1 <= P.length()+e && to-from+1 >= P.length()-e)
            candidates.push_back(make_pair(from, to));
    }
}
