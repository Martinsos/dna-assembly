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
    
        /** Returns number of bits in BitArray
         */
        int size() const;

        /** Returns bit at given position as char
         *
         *  @param position in bit array
         *  @return bit at given position
         */
        char bitCharAt(int pos) const;
        
        /** Constructor
         *
         *  @param bits Bits to be stored
         */
        BitArray(vector<bool> bits);

        /** Empty constructor
         */
        BitArray();

    private:
        vector<bool> bits;
};

#endif // BITARRAY_HPP
