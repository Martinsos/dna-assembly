#ifndef FER_SWWRAPPER
#define FER_SWWRAPPER

#include <vector>
#include <utility>

namespace fer { namespace util {

void smithWaterman(std::vector<double>* score,
		   char* text,
		   char* patterns[],
		   const int noPatterns);

}}

#endif
