/**
 * Aho Corasick tree implementation.
 *
 * Algorithm description is described in two pdfs attached with
 * this code.
 *
 * @author Filip Pavetic (fpavetic@gmail.com)
 */

#include <cstdio>
#include <cstring>
#include <queue>
#include <list>
#include <string>
#include <vector>

namespace fer { namespace datastruct {

class AhoCorasick {
 public:
       /**
	* @param patterns - constructor takes a vector of words patterns and 
	*                   creates AC automata
	*/
   AhoCorasick(const std::vector<std::string>& patterns) : 
                 root_(new AhoTrieNode()), nodeCounter_(0) {
		   patternEquivalenceGraph_.resize(patterns.size());
		   root_->nodeId = nodeCounter_++;
		   ahoTrieBuild(patterns);
		   buildFailureLinks();
		 }
   
   ~AhoCorasick() {
     ahoTrieDestroy(root_);
   }

   void matchOnce(std::vector<char>* found, 
		  char* const text, 
		  const size_t len); 

 private:
   struct AhoTrieNode {
     AhoTrieNode* failureLink;
     AhoTrieNode* parent;
     
     AhoTrieNode* child;
     AhoTrieNode* peer;

     int patternId;
     int nodeId;

     char childLink;
     char peerLink;

   AhoTrieNode() : failureLink(NULL), parent(NULL), child(NULL), peer(NULL), patternId(-1) {}

     /**
      * Iz trenutnog cvora vrati pointer na sljedeci, ili ga stvori (dinamicki alocira)
      * ukoliko ne postoji (ako je nodeIde != NULL)
      */
     AhoTrieNode* next(char t, size_t* nodeCounter) {
       AhoTrieNode* x = this;
       
       if (x->child == NULL) {
	 if (nodeCounter == NULL) return NULL;
	 x->child = new AhoTrieNode;
	 x->child->nodeId = (*nodeCounter)++;
	 x->childLink = t;
	 x->child->parent = x;
	 x = x->child;
       } else if (x->childLink == t) {
	 x = x->child;
       } else {
	 AhoTrieNode* p = x;
	 x = x->child;
	 while (x->peer != NULL) {
	   if (x->peerLink == t) return x->peer;
	   x = x->peer;
	 }
	 if (nodeCounter == NULL) return NULL;
	 x->peer = new AhoTrieNode;
	 x->peer->nodeId = (*nodeCounter)++;
	 x->peerLink = t;
	 x->peer->parent = p;
	 return x->peer;
       }
       
       return x;
     }
   };

   void ahoTrieBuild(const std::vector<std::string>& patterns);

   void ahoTrieInsert(AhoTrieNode* x, const std::string& pattern, int patternId);

   bool ahoTrieContains(const std::string& pattern);

   void ahoTrieDestroy(AhoTrieNode* x);

   void ahoTriePrint(AhoTrieNode* x, int dep = 0, char upLink = -1);

   AhoTrieNode* getFailureLink(AhoTrieNode* p, char xLink);

   void buildFailureLinks();
 private:
   AhoTrieNode* root_;
   std::vector<std::list<int> > patternEquivalenceGraph_;
   size_t nodeCounter_;
};

} }
