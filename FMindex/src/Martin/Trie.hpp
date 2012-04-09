#include <string>
#include <vector>

using namespace std;

class Trie
{
  public:
	Trie();
  
	/**
	 * Uses LZ78 parsing to parse string into words. Builds trie from this words.
	 * Returns parsed string as vector of strings (it is actually LZ7 dictionary).
	 * It is possible that last word is same as some other word.
	 */
  	vector<string> buildTrieLZ78 (const string &T);	
};
