#ifndef BITARRAY_HPP
#define BITARRAY_HPP

#include <vector>

using namespace std;
/**
 * Class representing array of bits
 * Here used for storing BW_RLX compression of original text
 */
class BitArray
{
    public:
        /** Output bits to stdout
         */
        void print();
        
        /** Constructor
         *
         *  @param bits Bits to be stored
         */
        BitArray(vector<bool> bits);

    private:
        vector<bool> bits;
};

#endif // BITARRAY_HPP
