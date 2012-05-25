#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <vector>

#include "util/SmithWaterman.h"
using namespace std;
using namespace fer::util;

const int kChunkLen = 12;
const int kMaxLengthOffset = 3;

inline int getBaseId(char a) {
  if (a == 'A') return 0;
  if (a == 'C') return 1;
  if (a == 'T') return 2;
  if (a == 'G') return 3;
  return 0; // 'N'
}

inline char reverseBaseId(int id) {
  if (id == 0) return 'A';
  if (id == 1) return 'C';
  if (id == 2) return 'T';
  if (id == 3) return 'G';
  return -1;
}

template<class T>
struct CircularArray {
  T* arr;
  size_t begin;
  size_t end;
  size_t capacity;

  size_t size;

  CircularArray(size_t capacity_) {
    capacity = capacity_;
    arr = new T[capacity];
    begin = end = size = 0;
  }
  ~CircularArray() {
    delete [] arr;
  }

  T& operator[] (const size_t i) {
    size_t t = begin + i;
    if (t >= capacity) return arr[t-capacity];
    return arr[t];
  }

  void push_back(const T& a) {
    arr[end++] = a;
    ++size;
    if (end >= capacity) end -= capacity;
  }
  void pop_front() {
    ++begin;
    --size;
    if (begin >= capacity) begin -= capacity;
  }

  size_t getSize() {
    return size;
  }
};

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

  //  2 greske
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

int main(int argc, char* argv[]) {
  static char buffer[128];

  char* dnaPath = argv[2];

  fprintf(stderr, "Citam DNA iz %s\n", dnaPath);
  FILE* dnaInputFile = fopen(dnaPath, "rt");

  *buffer = '\0';
  char* p = buffer;

  CircularArray<char> circPrefix(kChunkLen);

  for (size_t i = 0;; ++i, ++p) {
    if (*p == '\0') {
      if (fscanf(dnaInputFile, "%s", buffer) != 1) break;
      p = buffer;
    } 
    
    if (circPrefix.getSize() >= kChunkLen) {
      circPrefix.pop_front();
    }

    circPrefix.push_back(*p);
    
    if (circPrefix.getSize() == kChunkLen) {
      insert(forwardIndex, circPrefix, i-kChunkLen+1);
    }
  }

  fclose(dnaInputFile);


  //dumpajTablicu(forwardIndex, 1<<(2*kPrefixLen));
  for (int i = 0; i < (1<<(2*kChunkLen)); ++i) {
    sort(forwardIndex[i].begin(), forwardIndex[i].end());
  }


  char* readingPath = argv[1];
  fprintf(stderr, "Citam readove iz %s\n", readingPath);

  int found = 0, total = 0;
  int totalCombos = 0;
  int maxCombos = 0;
  string sta;

  FILE* readInputFile = fopen(readingPath, "rt");
  for (int i = 0; fscanf(readInputFile, "%s", buffer) == 1; ++i) {
    int bufferLen = (int)strlen(buffer);
    
    if (bufferLen < kChunkLen) { // ignore very short reads
      continue;
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
    int combos = 0;
    vector<pair<int, int> > kandidati;
    
    for (int i = 0; i < forwardIndex[prefixHash].size(); ++i) {
      if (i && forwardIndex[prefixHash][i-1] + kChunkLen >= forwardIndex[prefixHash][i]) {
	continue;
      }

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
      
      assert(suffixHi >= suffixLo);
      
      bool ima = false;
      if (suffixLo+1 != bufferLen && suffixHi-suffixLo+1 > 0) {
	combos += (suffixHi-suffixLo+1 > 0);
      }
      
      if (combos > maxCombos) {
	maxCombos = combos;
	sta = buffer;
      }

      for (int j = suffixLo; j <= suffixHi; ++j) {
	kandidati.push_back(make_pair(forwardIndex[prefixHash][i],
				      forwardIndex[suffixHash][j]));
      }
    }
    
    char** kandidatiStr = new char*[(int)kandidati.size()];

    for (int i = 0; i < (int)kandidati.size(); ++i) {
      int len = kandidati[i].second - kandidati[i].first + 1;
      kandidatiStr[i] = new char[len+1];
      for (int j = kandidati[i].first; j <= kandidati[i].second; ++j) {
	//	kandidatiStr[i][j-kandidati[i].first] = // jebemu sve, moram imati dna u memoriji
      }
      kandidatiStr[i][len] = '\0';
    }
    
    vector<double> score;
    smithWaterman(&score, buffer,
		  kandidatiStr, (int)kandidati.size());

    for (int i = 0; i < (int)kandidati.size(); ++i) {
      delete[] kandidatiStr[i];
    }
    delete[] kandidatiStr;

    ++total;
    found += (combos > 0);
    totalCombos += combos;
  }
    
    
 fclose(readInputFile);
      
 printf("found=%d/total=%d\n", found, total);
 printf("totalCombos: %d\n", totalCombos);
 printf("maxCombos: %d %s\n", maxCombos, sta.c_str());
      
 return 0;
 }
    
