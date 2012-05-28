#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <vector>

#include "util/SmithWaterman.h"
#include "util/CircularArray.h"
#include "util/Utility.h"
using namespace std;
using namespace fer::util;

const int kChunkLen = 13;
const int kMaxLengthOffset = 0;

vector<size_t> forwardIndex[1<<(2*kChunkLen)];

void insert(vector<size_t>* ind, CircularArray<char>& c, const size_t pos) {
  // 0 gresaka
  size_t hash = 0;
  for (size_t i = 0; i < c.getSize(); ++i) hash = hash*4+getBaseId(c[i]);
  ind[hash].push_back(pos);

  // 1 greska
  for (size_t i = 0; i < c.getSize(); ++i) {
    char stari = c[i];
    int stariId = getBaseId(stari);

    for (int k = 0; k < 4; ++k) {
      char novi = reverseBaseId(k);
      if (novi == stari) continue;

      size_t noviHash = hash + (1<<(2*(c.getSize()-i-1)))*(k-stariId);
      ind[noviHash].push_back(pos);
    }
  }

  //  2 greske, za sada onemoguceno zbog toga sto zahtijeva previse memorije
  //
  // for (size_t i = 0; i < c.getSize(); ++i) {
  //   for (size_t j = i+1; j < c.getSize(); ++j) {
  //     char stari1 = c[i], stari2 = c[j];
  //     int stariId1 = getBaseId(stari1), stariId2 = getBaseId(stari2);

  //     for (int k = 0; k < 4; ++k) {
  // 	char novi1 = reverseBaseId(k);
  // 	if (novi1 == stari1) continue; // jer cemo postici maks jednu gresku, a to je pokriveno
  // 	for (int l = 0; l < 4; ++l) {
  // 	  char novi2 = reverseBaseId(l);
  // 	  if (novi2 == stari2) continue;

  // 	  size_t noviHash = hash 
  // 	    + (1<<(2*(c.getSize()-i-1)))*(k-stariId1)
  // 	    + (1<<(2*(c.getSize()-j-1)))*(l-stariId2);

  // 	  ind[noviHash].push_back(pos);
  // 	}
  //     }
  //   }
  // }
}

void dumpajTablicu(vector<size_t>* ind, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    printf("## %d:", (int)i);
    for (int j = 0; j < ind[i].size(); ++j) {
      printf(" %d", (int)ind[i][j]);
    }
    puts("");
  }
}

char dna[20*1024*1024];
int dnaSize;

// ucita DNA i napravi index
void readDna(char* dnaPath) {
  static char buffer[1024];

  fprintf(stderr, "Citam DNA iz %s\n", dnaPath);
  FILE* dnaInputFile = fopen(dnaPath, "rt");

  *buffer = '\0';
  char* p = buffer;
  dnaSize = 0;

  CircularArray<char> circPrefix(kChunkLen+1);

  for (size_t i = 0;; ++i, ++p) {
    if (*p == '\0') {
      if (fscanf(dnaInputFile, "%s", buffer) != 1) break;
      p = buffer;
    } 
    
    if (circPrefix.getSize() >= kChunkLen) {
      circPrefix.pop_front();
    }

    dna[dnaSize++] = *p;
    circPrefix.push_back(*p);

    if (circPrefix.getSize() == kChunkLen) {
      insert(forwardIndex, circPrefix, i-kChunkLen+1);
    }
  }

  dna[dnaSize] = '\0';
  fclose(dnaInputFile);

  //dumpajTablicu(forwardIndex, 1<<(2*kPrefixLen));

  for (int i = 0; i < (1<<(2*kChunkLen)); ++i) {
    sort(forwardIndex[i].begin(), forwardIndex[i].end());
  }
}

void processReads(char* readsPath) {
  static char buffer[128];

  fprintf(stderr, "Citam readove iz %s\n", readsPath);

  int found = 0, total = 0;
  int totalCombos = 0;
  int maxCombos = 0;
  int maxReadId = 0;
  string sta;

  FILE* readsInputFile = fopen(readsPath, "rt");
  for (int readId = 0; fscanf(readsInputFile, "%s", buffer) == 1; ++readId) {
    int bufferLen = (int)strlen(buffer);
    
    if (bufferLen < kChunkLen) { // ignore very short reads
      continue;
    }

    int combos = 0; // za statistiku
    pair<double, pair<int, int> > bestScore = make_pair(-1000000, make_pair(-1, -1));

    for (int pass = 0; pass < 2; ++pass) {  // pass == 0 -> ocitanje, pass == 1 -> revers komplement
      if (pass == 1) {
	for (int i = 0; i < bufferLen; ++i) {
	  buffer[i] = baseComplement(buffer[i]);
	}
	reverse(buffer, buffer+bufferLen);
      }

      int prefixHash = 0;
      for (int i = 0; i < kChunkLen; ++i) {
	prefixHash = (prefixHash << 2) + getBaseId(buffer[i]);
      }
      
      int suffixHash = 0;
      for (int i = bufferLen-kChunkLen; i < bufferLen; ++i) {
	suffixHash = (suffixHash << 2) + getBaseId(buffer[i]);
      }

      int suffixLo = 0;
      int suffixHi = 0;
      vector<pair<int, int> > kandidati;

      for (int i = 0; i < forwardIndex[prefixHash].size(); ++i) {
	printf(":: %d %d\n", forwardIndex[prefixHash][i], (int)forwardIndex[prefixHash].size());
	// TODO: zamijeniti ove binary searchem lijenim pomacima
	suffixLo = lower_bound(forwardIndex[suffixHash].begin(), forwardIndex[suffixHash].end(),
			       forwardIndex[prefixHash][i]+bufferLen-kChunkLen-kMaxLengthOffset)-forwardIndex[suffixHash].begin();
	suffixHi = upper_bound(forwardIndex[suffixHash].begin(), forwardIndex[suffixHash].end(),
			       forwardIndex[prefixHash][i]+bufferLen-kChunkLen+kMaxLengthOffset)-forwardIndex[suffixHash].begin();
	
	/*
	  while (suffixHi < forwardIndex[suffixHash].size() &&
	  forwardIndex[suffixHash][suffixHi] - forwardIndex[prefixHash][i] <= bufferLen+kMaxLengthOffset) {
	  ++suffixHi;
	  }
	  if (0 < suffixHi && suffixHi < forwardIndex[suffixHash].size()) {
	  --suffixHi;
	  }
	  
	  while (suffixLo < forwardIndex[suffixHash].size() &&
	  forwardIndex[suffixHash][suffixLo]-forwardIndex[prefixHash][i] <= bufferLen-kMaxLengthOffset) {
	  ++suffixLo;
	  }
	  if (0 < suffixLo && suffixLo < forwardIndex[suffixHash].size()) {
	  --suffixLo;
	  }
	*/
	assert(suffixHi >= suffixLo);
	assert(suffixHi <= forwardIndex[suffixHash].size());
	
	for (int j = suffixLo; j < suffixHi; ++j) {
	  assert(forwardIndex[suffixHash][j] + kChunkLen - forwardIndex[prefixHash][i] <= 100); // TODO: makni
	  assert(forwardIndex[suffixHash][j] + kChunkLen - forwardIndex[prefixHash][i] >= 50); // TODO: makni
	  printf("%d %d lo=%d hi=%d\n", forwardIndex[prefixHash][i], forwardIndex[suffixHash][j], suffixLo, suffixHi);
	  assert(forwardIndex[suffixHash][j] + kChunkLen - forwardIndex[prefixHash][i] == 70);

	  kandidati.push_back(make_pair(forwardIndex[prefixHash][i],
					forwardIndex[suffixHash][j]+kChunkLen-1));
	}
	
	combos += (kandidati.size() > 0);	
	if (combos > maxCombos) {
	  maxCombos = combos;
	  maxReadId = readId;
	  sta = buffer;
	}
      }
      
      char** kandidatiStr = new char*[(int)kandidati.size()];
    
      for (int i = 0; i < (int)kandidati.size(); ++i) {
	int len = kandidati[i].second - kandidati[i].first + 1;

	kandidatiStr[i] = new char[len+1];
	for (int j = kandidati[i].first; j <= kandidati[i].second; ++j) {
	  kandidatiStr[i][j-kandidati[i].first] = dna[j];
	}
	kandidatiStr[i][len] = '\0';
      }
    
      vector<double> score;
      smithWaterman(&score, buffer,
      	    kandidatiStr, (int)kandidati.size());

      for (int i = 0; i < score.size(); ++i) {
	bestScore = max(bestScore, make_pair(score[i], kandidati[i]));
      }

      for (int i = 0; i < (int)kandidati.size(); ++i) {
	delete[] kandidatiStr[i];
      }
      delete[] kandidatiStr;

      printf("-- %d %s\n", strstr(dna, buffer)-dna, buffer);
    }
    ++total;
    found += (combos > 0);
    totalCombos += combos;

    printf("combos %d\n", combos);
    printf("progress %d/%d %lf\n", found, total, 100.*found/total);
    printf("from %d to %d score %lf\n", bestScore.second.first, bestScore.second.second, bestScore.first);
  }
    
  fclose(readsInputFile);
      
  printf("found=%d/total=%d\n", found, total);
  printf("totalCombos: %d\n", totalCombos);
  printf("maxCombos(%d): %d %s\n", maxReadId, maxCombos, sta.c_str());     
}

int main(int argc, char* argv[]) {
  /*  
  //    koristeno za test&debug, zadrzat cu jos neko vrijeme 

  char* query1 = "AGAGTAAGTCTTTGTATTTTATGCTACTGTACCTCTGGGATTAATTGCTCTTTCCCTCATTGGCCAGTCA";
  char* database1[] = {
    "AGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGACA",
    "AGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGACA"
  };

  char* query2 = "GGCTACCATATTGGACAGCACAGGTCTAAGGATTTCATTCCTGCCACAAGTCCAAACTCCTAGCTTTAAT";
  char* database2[] = {
  "GGCTGGGGCGGGGGGAGGGTGGCGCCGTGCACGCGCAGAAACTCACGTCACGGTGGCGCGGCGCAGAGACGGGTAGAACCTCAGTAAT"
  };

  vector<double> s;

  for (int i = 0; i < 1000000; ++i) {
    printf("%d\n", i);
    smithWaterman(&s, query1, database1, 2);
    smithWaterman(&s, query2, database2, 1);
  }

  return 0;
  */

  readDna(argv[2]);
  processReads(argv[1]);

  return 0;
}
    
