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
    d2_dictionary<int, int, int> D;
    
    for (int i = 0; i < 100000; i++)
        D.insert(i, i, i);
    
    int bzvz;
    cin >> bzvz;
    
    cout << "kraj" << endl;
    return 0;
}
