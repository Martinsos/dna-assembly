#include "util/SmithWaterman.h"

extern "C" {
    #include "swSharp/include/swsharp/sw.h"
    #include "swSharp/include/swsharp/sw_prefs.h"
    #include "swSharp/include/swsharp/chain.h"
    #include "swSharp/include/swsharp/chain_base.h"
}

using namespace std;

namespace fer { namespace util {

void smithWaterman(vector<double>* score,
		   char* queryStr,
		   char* databaseStr[],
		   const int databaseSize) {
  score->resize(databaseSize);

  char* swPrefsInit[] = { NULL };
  SWPrefs* swPrefs = swPrefsCreate(swPrefsInit);

  Chain* query = chainCreateFromBuffer(queryStr, swPrefs);
  ChainBase* database = chainBaseCreateFromBuffer(databaseStr, databaseSize, swPrefs);

  SWSharpData* swSharpData = swSharp(query, database, swPrefs);

  for (int i = 0; i < swSharpDataGetSWDataNmr(swSharpData); ++i) {   
    SWData* swData = swSharpDataGetSWData(swSharpData, i);
    
    for (int j = 0; j < swDataGetResultNmr(swData); ++j) {
      SWResult* swResult = swDataGetResult(swData, j);

      // TODO dodati vracanje rezultata
      swResultPrint(swResult, stdout);
    }
  }

  swSharpDataDelete(swSharpData);
  chainDelete(query);
  chainBaseDelete(database);
  swPrefsDelete(swPrefs);
}

}}
