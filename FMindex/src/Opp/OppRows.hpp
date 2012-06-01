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

    /**
     * is set to empty
     */
    OppRows();
    /** Constructor, initializes class members
     */
    OppRows(int f, int l, bool e);
    
    /**
     * If there are no rows returns true, otherwise false.
     * If OppRows is empty, getFirst() and getLast() can return anything.
     */
    bool isEmpty() const;
    
    /** Returns first row in conceptual matrix prefixed with certain string
     */
    int getFirst() const;

    /** Returns last row in conceptual matrix prefixed with certain string
     */
    int getLast() const;

    /** Prints itself to stdout
     *  For testing
     */
    void print() const;
  
  private:
    int first, last;
    bool empty;
};
#endif // OPP_ROWS
