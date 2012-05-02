
#include "Compressor.hpp"
#include <iostream>

using namespace std;

int main()
{
    Compressor myCompressor('#');
    vector<Index> SA = myCompressor.getSuffixArray("matija");
    
    for (int i = 0; i < SA.size(); i++)
        cout << SA[i] << ' ';
    cout << endl;

    return 0;
}
