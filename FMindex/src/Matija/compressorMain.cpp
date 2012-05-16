
#include "Compressor.hpp"

#include <string>
#include <iostream>

using namespace std;

int main()
{   
    // Initialize compressor
    char eof = '#';
    vector<char> letters;
    letters.push_back('i');
    letters.push_back('p');
    letters.push_back('s');
    letters.push_back('m');
    letters.push_back('e');
    letters.push_back(eof);
    Alphabet alpha(letters);

    Compressor myCompressor(eof, alpha);

    string T = "pipemississippi";
   /* string L = myCompressor.getBWT(T); 
    vector<int> MTF = myCompressor.getMTF(L);

    cout << "==MTF=====================" << endl;
    cout << MTF.size() << endl;
    
    cout << L << endl;
    for (int i = 0; i < MTF.size(); i++)
        cout << MTF[i] << " ";
    cout << endl;
    */
    cout << "==Variable length prefix encoding========================" << endl;
    BitArray VLPC = myCompressor.compress(T);
    VLPC.print();

    return 0;
}
