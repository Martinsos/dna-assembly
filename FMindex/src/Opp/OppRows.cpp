
#include <iostream>

#include "OppRows.hpp"

/** Constructor
 */
OppRows::OppRows(int f, int l, bool e)
{
    first = f;
    last = l;
    empty = e;
}

/** Returns true if result is initialized.
 */
bool OppRows::isEmpty()
{
    return empty;   
}

/** Returns first prefixed row
 */
int OppRows::getFirst()
{
   return first; 
}

/** Returns last prefixed row
 */
int OppRows::getLast()
{
   return last; 
}

/** Prints to stdout
 */
void OppRows::print()
{
    cout << "First:   " << first << endl;
    cout << "Last:    " << last << endl;
    cout << "isEmpty: " << empty << endl;
    cout << "-----------------------" << endl;
}
