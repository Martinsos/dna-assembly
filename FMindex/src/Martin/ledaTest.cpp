#include <iostream>

#include "LEDA/geo/d2_dictionary.h" 

using namespace std;

using leda::d2_dictionary;
using leda::list;
using leda::dic2_item;

int main()
{
    d2_dictionary<int, int, int> D;
    D.insert(1, 2, -1);
    D.insert(2, 5, 9999999);
    D.insert(4, 7, 3);
    D.insert(0, 0, 4);
    D.insert(2, 6, 5);
    D.insert(-1, 2, 6);
    
    list<dic2_item> L = D.range_search(1,3,2,7);
    
    dic2_item x;
    forall(x, L) {
        cout << ((int*)(x->inf())) << endl;
    }
    
    cout << "kraj" << endl;
    return 0;
}
