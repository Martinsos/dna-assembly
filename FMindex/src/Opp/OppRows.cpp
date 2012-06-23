/** 
 *  author: Matija Sosic
 *  e-mail: matija.sosic@gmail.com
 *
 *  class OppRows
 */

#include <iostream>

#include "OppRows.hpp"

OppRows::OppRows()
{
    first = last = 0;
    empty = true;
}

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
bool OppRows::isEmpty() const
{
    return empty;   
}

/** Returns first prefixed row
 */
int OppRows::getFirst() const
{
   return first; 
}

/** Returns last prefixed row
 */
int OppRows::getLast() const
{
   return last; 
}

/** Prints to stdout
 */
void OppRows::print() const
{
    cout << "First:   " << first << endl;
    cout << "Last:    " << last << endl;
    cout << "isEmpty: " << empty << endl;
    cout << "-----------------------" << endl;
}
