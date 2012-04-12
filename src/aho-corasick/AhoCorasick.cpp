/**
 * @author Filip Pavetic (fpavetic@gmail.com)
 */

#include "AhoCorasick.h"
using namespace std;

namespace fer { namespace datastruct {

// TODO(fpavetic): napraviti ljepsom ovu funkciju
void AhoCorasick::matchOnce(vector<char>* found, 
			    char* const text, 
			    const size_t len) {
  fill(found->begin(), found->end(), 0);

  AhoTrieNode* x = root_;

  queue<AhoTrieNode*> q;
  vector<char> visited(nodeCounter_);

  for (size_t i = 0; i < len; ) {
    AhoTrieNode* n=x->next(text[i], NULL);
    
    if (n == NULL) {
      if (x == root_) ++i;
      x = x->failureLink;
      q.push(x);
    } else {
      x = n;
      ++i;

      if (x->patternId != -1) {
	if (!(*found)[x->patternId] && !visited[x->nodeId]) { 
	  q.push(x); 
	  visited[x->nodeId] = 1; 
	}
        (*found)[x->patternId] = 1;
      }
    }
  }

  for (; !q.empty(); q.pop()) {
    AhoTrieNode* x = q.front();
    
    if (x->patternId != -1) (*found)[x->patternId] = 1;

    if (x->failureLink != root_ && !visited[x->failureLink->nodeId]) {
      visited[x->failureLink->nodeId] = 1;
      q.push(x->failureLink);
    }
    if (x != root_ && x->parent != root_ && !visited[x->parent->nodeId]) {
      visited[x->parent->nodeId] = 1;
      q.push(x->parent);
    }
  }

  queue<int> nq;
  for (int i = 0; i < found->size(); ++i) if ((*found)[i]) nq.push(i);
  
  for (; !nq.empty(); nq.pop()) {
    int u = nq.front();
    for (list<int>::iterator it = patternEquivalenceGraph_[u].begin();
	 it != patternEquivalenceGraph_[u].end(); ++it) {
      if (!(*found)[*it]) {
	(*found)[*it] = 1;
	nq.push(*it);
      }
    }
  }
}

void AhoCorasick::ahoTrieBuild(const vector<string>& patterns) {
  for (size_t i = 0; i < patterns.size(); ++i) {
    ahoTrieInsert(root_, patterns[i], i);
  }
}

void AhoCorasick::ahoTrieInsert(AhoTrieNode* x,
				const string& pattern, 
				int patternId) {
  for (int i = 0; i < pattern.size(); ++i) {
    x = x->next(pattern[i], &nodeCounter_);
  }
  if (x->patternId != -1) {
    patternEquivalenceGraph_[x->patternId].push_back(patternId);
    patternEquivalenceGraph_[patternId].push_back(x->patternId);
  }
  x->patternId = patternId;  
}

bool AhoCorasick::ahoTrieContains(const string& pattern) {
  AhoTrieNode* x = root_;
  for (int i = 0; i < pattern.size(); ++i) {
    x = x->next(pattern[i], NULL);
  }
  return x->patternId != -1;
}

void AhoCorasick::ahoTrieDestroy(AhoTrieNode* x) {
  if (x != NULL) {
    ahoTrieDestroy(x->child);
    ahoTrieDestroy(x->peer);
    delete x;
  }
}

void AhoCorasick::ahoTriePrint(AhoTrieNode* x, 
			       int dep, 
			       char upLink) {
  if (x == NULL) return;
	    
  for (int i = 0; i < dep*2; ++i) putchar(' ');
  if (upLink != -1) {
    printf("-> %c -> ", upLink);
  }
  printf("%p (id: %3d) (%d %p)\n", 
	 x, x->nodeId, x->patternId, x->failureLink);

  if (x->child != NULL) {
    ahoTriePrint(x->child, dep+1, x->childLink);
    for (AhoTrieNode* c = x->child; c->peer != NULL; c = c->peer) {
      ahoTriePrint(c->peer, dep+1, c->peerLink);
    }
  }  
}


AhoCorasick::AhoTrieNode* AhoCorasick::getFailureLink(
    AhoTrieNode* p, char xLink) {
  if (p == root_) return p;

  for (;;) {
    AhoTrieNode* f = p->failureLink;
    AhoTrieNode* nf = f->next(xLink, NULL);
    
    
    if (nf != NULL) {
      return nf;
    } else if (p == root_) {
      return root_;
    } else {
      p = p->failureLink;
    }
  }

  fprintf(stderr, "This shouldn't happen!\n");
  return p;
}

void AhoCorasick::buildFailureLinks() {
// This seems dangerous, in case of class changing address.
// If we'd want to support that, copy constructors are ought to be
// implemented. // TODO
  root_->failureLink = root_; 
  root_->parent = NULL;

  queue<AhoTrieNode*> q;
  q.push(root_);

  for ( ; !q.empty(); q.pop()) {
    AhoTrieNode* x = q.front();
    
    if (x->child == NULL) { 
      // a leaf which should have been update earlier
      continue;
    }

    AhoTrieNode* c = x->child;
    c->failureLink = getFailureLink(x, x->childLink);
    
    q.push(c);

    for (c = x->child; c->peer != NULL; c = c->peer) {
      c->peer->failureLink = getFailureLink(x, c->peerLink);
      q.push(c->peer);
    }
  }
}

} } 
