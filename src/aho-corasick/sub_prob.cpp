/**
 * Rjesenje zadatka www.spoj.pl/problems/SUB_PROB
 * - implementacija Aho Corasick stabla (opisi prilozeni u dva pdf-a)
 * 
 * TODO(prilagoditi ovo da bude klasa za lako koristenje)
 *
 * Filip Pavetic
 */

#include <cstdio>
#include <cstring>
#include <queue>
#include <list>
#include <string>
#include <vector>
using namespace std;

class AhoCorasick {
    public:
        AhoCorasick(const vector<string>& patterns) : root_(new AhoTrieNode()), nodeCounter_(0) {
            patternEquivalenceGraph_.resize(patterns.size());
            root_->nodeId = nodeCounter_++;
            ahoTrieBuild(patterns);
            buildFailureLinks();
        }
        ~AhoCorasick() {
            ahoTrieDestroy(root_);
        }

        void matchOnce(vector<char>* found, char* const text, const size_t len) {
            fill(found->begin(), found->end(), 0);

            AhoTrieNode* x = root_;

            queue<AhoTrieNode*> q;
            vector<char> visited(nodeCounter_);

            for (size_t i = 0; i < len; ) {
                //printf("%u\n", i);
                AhoTrieNode* n=x->next(text[i], NULL);
                //printf("%p %p %d %d %c\n", x, root_, x->patternId, root_->patternId, text[i]);

                if (n == NULL) {
                    if (x == root_) ++i;
                    x = x->failureLink;
                    q.push(x);
                } else {
                    x = n;
                    ++i;

                    if (x->patternId != -1) {
                        //printf("%03d: nasao pattern %d\n", i, x->patternId);
                        if (!(*found)[x->patternId] && !visited[x->nodeId]) { q.push(x); visited[x->nodeId] = 1; }
                        (*found)[x->patternId] = 1;
                    }
                }
            }

            for (; !q.empty(); q.pop()) {
                AhoTrieNode* x = q.front();
                //printf("%p %d %u\n", x, x->nodeId, visited.size());

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

        void ahoTrieBuild(const vector<string>& patterns) {
            for (size_t i = 0; i < patterns.size(); ++i) {
                ahoTrieInsert(root_, patterns[i], i);
            }
        }

        void ahoTrieInsert(AhoTrieNode* x, const string& pattern, int patternId) {
            for (int i = 0; i < pattern.size(); ++i) {
                x = x->next(pattern[i], &nodeCounter_);
            }
            if (x->patternId != -1) {
                patternEquivalenceGraph_[x->patternId].push_back(patternId);
                patternEquivalenceGraph_[patternId].push_back(x->patternId);
            }
            x->patternId = patternId;
            //printf("%d\n", ahoTrieContains(pattern.substr(0, pattern.size())));
        }

        bool ahoTrieContains(const string& pattern) {
            AhoTrieNode* x = root_;
            for (int i = 0; i < pattern.size(); ++i) {
                x = x->next(pattern[i], NULL);
            }
            return x->patternId != -1;
        }

        void ahoTrieDestroy(AhoTrieNode* x) {
            if (x != NULL) {
                ahoTrieDestroy(x->child);
                ahoTrieDestroy(x->peer);
                delete x;
            }
        }

        void ahoTriePrint(AhoTrieNode* x, int dep = 0, char upLink = -1) {
            if (x == NULL) return;

            for (int i = 0; i < dep*2; ++i) putchar(' ');
            if (upLink != -1) {
                printf("-> %c -> ", upLink);
            }
            printf("%p (id: %3d) (%d %p)\n", x, x->nodeId, x->patternId, x->failureLink);

            if (x->child != NULL) {
                ahoTriePrint(x->child, dep+1, x->childLink);
                for (AhoTrieNode* c = x->child; c->peer != NULL; c = c->peer) {
                    ahoTriePrint(c->peer, dep+1, c->peerLink);
                }
            }

        }


        AhoTrieNode* getFailureLink(AhoTrieNode* p, char xLink) {
            if (p == root_) return p;

            for (;;) {
                AhoTrieNode* f = p->failureLink;
                AhoTrieNode* nf = f->next(xLink, NULL);
                //printf("f=%p p=%p nf=%p xLink=%c\n", f, p, nf, xLink);

                if (nf != NULL) {
                    return nf;
                } else if (p == root_) {
                    return root_;
                } else {
                    p = p->failureLink;
                }
            }

            fprintf(stderr, "Ne bi se smjelo dogoditi!\n");
            return p;
        }

        void buildFailureLinks() {
            root_->failureLink = root_; // cini se opasno, ako cijela klasa promijeni adresu,
                                        // ako zelim podrzati to treba implementirati copy constructore
            root_->parent = NULL;

            queue<AhoTrieNode*> q;
            q.push(root_);

            for ( ; !q.empty(); q.pop()) {
                AhoTrieNode* x = q.front();

                if (x->child == NULL) { // list, a njega sam vec ranije trebao updateati
                    continue;
                }

                AhoTrieNode* c = x->child;
                c->failureLink = getFailureLink(x, x->childLink);
                //printf("%p\n", c->failureLink);
                q.push(c);

                for (c = x->child; c->peer != NULL; c = c->peer) {
                    c->peer->failureLink = getFailureLink(x, c->peerLink);
                    //printf("%p\n", c->peer->failureLink);
                    q.push(c->peer);
                }
            }

            //ahoTriePrint(root_);
        }
    private:
        AhoTrieNode* root_;
        vector<list<int> > patternEquivalenceGraph_;
        size_t nodeCounter_;
};

const int kTextMaxLen = 100100;
const int kPatternMaxLen = 2020;

char text[kTextMaxLen], pattern[kPatternMaxLen];
vector<string> patterns;
int m;

int main(void) {
    scanf("%s", text);
    scanf("%d", &m);

    for (int i = 0; i < m; ++i) {
        scanf("%s", pattern);
        patterns.push_back(string(pattern));
    }

    AhoCorasick automat(patterns);

    vector<char> result(m, 0);
    automat.matchOnce(&result, text, (int)strlen(text));

    for (int i = 0; i < m; ++i) {
        puts(result[i] ? "Y": "N");
    }
return 0;
}
