/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#include <iostream>
#include <utility>

#include "LEDA/geo/d2_dictionary.h" 

using namespace std;

using leda::d2_dictionary;
using leda::list;
using leda::dic2_item;

class Information {
  public:
    int v;
    int k;
    
    Information(int k, int v) {
        this-> k = k;
        this-> v = v;
    }
};

int main()
{
    d2_dictionary<int, int, Information> D;
    D.insert(1, 2, Information(1,-1));
    D.insert(2, 5, Information(2,-2));
    D.insert(4, 7, Information(3,-3));
    D.insert(0, 0, Information(4,-4));
    D.insert(2, 6, Information(5,-5));
    D.insert(-1, 2, Information(6,-6));
    
    list<dic2_item> L = D.range_search(1,3,2,7);
    
    dic2_item x;
    forall(x, L) {
        Information inf = leda::LEDA_ACCESS(Information, x->inf());
        cout << inf.k << " " << inf.v << endl;
    }
    
    cout << "kraj" << endl;
    return 0;
}
