/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#include <iostream>
#include "Opp.hpp"

using namespace std;

int main() {
    Opp opp("$o$pej$il$ej$ar$t$un$ui$l$ai$mr$g$io$r$k$o$m$e$j$i$n$a$v");
    opp.printOpp();
    OppRows rows = opp.findRows("$v");
    cout << rows.getFirst() << " " << rows.getLast() << endl;
    return 0;
}
