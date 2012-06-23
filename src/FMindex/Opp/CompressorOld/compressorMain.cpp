
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

    cout << "==Variable length prefix encoding========================" << endl;
    BitArray compressedText = myCompressor.compress(T);
    compressedText.print();

    return 0;
}
