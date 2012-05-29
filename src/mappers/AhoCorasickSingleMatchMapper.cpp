// Ovaj je  eksperiment koji je sluzio za provjeru isplati li se traziti egzaktne
// matcheve prefiksa i sufiksa. 
//
// Zakljucio sam da ne jer u premalom broju slucajeva zaista nadjem neke kandidatne
// parove pozicija za pocetak i kraj.

#include "../aho-corasick/AhoCorasick.h"
#include <cstdio>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;
using namespace fer::datastruct;

char kompl(char a) {
  if (a == 'A') return 'T';
  if (a == 'C') return 'G';
  if (a == 'G') return 'C';
  if (a == 'T') return 'A';
  return 'N';
}

string komplement(string a) {
  for (int i = 0; i < a.size(); ++i) a[i] = kompl(a[i]);
  return a;
}

int main(int argc, char* argv[]) {
  char* readingPath = argv[1];

  fprintf(stderr, "Citam readove iz %s\n", readingPath);

  static char buffer[128];
  vector<string> reads;
  FILE* readInputFile = fopen(readingPath, "rt");

  for (int i = 0; fscanf(readInputFile, "%s", buffer) == 1; ++i) {
    string read = buffer;
    read = read.substr(0, 10);
    reverse(read.begin(), read.end());
    read = komplement(read);
    reads.push_back(read);
  }

  fclose(readInputFile);

  AhoCorasick ac(reads);

  // TODO: 1) multiple matchevi
  //       2) nakon sto su nadjeni optimalno poplocavanje

  char* dnaPath = argv[2];

  fprintf(stderr, "Citam DNA iz %s\n", dnaPath);
  FILE* dnaInputFile = fopen(dnaPath, "rt");


  vector<int> foundTotal(reads.size(), -1);
  vector<char> found(reads.size(), 0);
  
  string prev;
  for (int i = 0; fscanf(dnaInputFile, "%s", buffer) == 1; ++i) {
    string curr = prev;
    curr += buffer;
    prev = buffer;

    ac.matchOnce(&found, curr.c_str(), curr.size());

    if (i % 1000 == 0) {
      fprintf(stderr, "%d\n", i);
    }
  }

  fclose(dnaInputFile);

  int foundCnt = 0;
  for (int i = 0; i < found.size(); ++i) foundCnt += found[i];

  fprintf(stderr, "%d/%d found\n", foundCnt, (int)reads.size());

  return 0;
}
