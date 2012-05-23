#ifndef FER_SWWRAPPER
#define FER_SWWRAPPER

#include <vector>
#include <utility>

namespace fer { namespace util {

void smithWaterman(vector<double>* score,
		   const char* text,
		   const char* patterns[]);

}}

#endif
