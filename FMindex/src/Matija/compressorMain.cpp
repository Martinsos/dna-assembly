
#include "Compressor.hpp"

#include <string>
#include <iostream>

using namespace std;

int main()
{   
    string T = "mississippi";

    Compressor myCompressor('#');
    string L = myCompressor.getBWT(T); 
    
    cout << L << endl;

    return 0;
}
