#include "util/SmithWaterman.h"

extern "C" {
    #include "external/swSharp/include/swSharp/sw.h"
    #include "external/swSharp/include/swSharp/sw_prefs.h"
    #include "external/swSharp/include/swSharp/chain.h"
    #include "external/swSharp/include/swSharp/chain_base.h"
}

namespace fer { namespace util {

void smithWaterman(vector<double>* score,
		   const char* queryStr,
		   const char* database[],
		   const int databaseSize) {
  char* swPrefsInit = { NULL };
  SWPrefs* swPrefs = swPrefsCreate(swPrefsInit);

  Chain* query = chainCreateFromBuffer(queryStr, swPrefs);
  ChainBase* database = chainBaseCreateFromBuffer(databaseStr, databaseSize, swPrefs);

  SwSharpData* swSharpData = swSharp(query, database, swPrefs);

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
