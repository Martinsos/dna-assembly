#ifndef FER_ASSEMBLY_UTILITY
#define FER_ASSEMBLY_UTILITY

#include <algorithm>
#include <string>

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

inline char baseComplement(char base) {
  if (base == 'A') return 'T';
  if (base == 'T') return 'A';
  if (base == 'C') return 'G';
  if (base == 'G') return 'C';
  return 'N';
}

std::string reverseComplementChain(std::string chain) {
  std::string revCom;
  for (std::string::iterator it = chain.begin(); it != chain.end(); ++it) {
    revCom += baseComplement(*it);
  }
  std::reverse(revCom.begin(), revCom.end());
  return revCom;
}

#endif
