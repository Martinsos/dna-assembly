#ifndef OPP_ROWS_HPP
#define OPP_ROWS_HPP

using namespace std;

/**
 * Class that represents result of findRows function (backward_search() in article).
 * Contains first and last row in conceptual matrix that is prefixed by given string.
 */
class OppRows
{
  public:

    /** Constructor, initializes class members
     */
    OppRows(int f, int l, bool e);
    
    /**
     * If there are no rows returns true, otherwise false.
     * If OppRows is empty, getFirst() and getLast() can return anything.
     */
    bool isEmpty();
    
    /** Returns first row in conceptual matrix prefixed with certain string
     */
    int getFirst();

    /** Returns last row in conceptual matrix prefixed with certain string
     */
    int getLast();

    /** Prints itself to stdout
     *  For testing
     */
    void print();
  
  private:
    int first, last;
    bool empty;
};
#endif // OPP_ROWS
