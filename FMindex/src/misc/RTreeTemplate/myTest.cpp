/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#include "RTree.hpp"

#include <iostream>
#include <vector>

using namespace std;

class Data {
  public:
    int a;
    int b;
    
    Data(int a, int b) {
        this->a = a;
        this->b = b;
    }
};

/**
 * This function is called each time that matching rectangle is found.
 */
bool MySearchCallback(Data* id, void* arg) 
{
  vector<Data*>* result = (vector<Data*>*) arg;
  result->push_back(id);
  return true; // keep going
}

/**
 * Implementation of range tree that is used here is somewhat specific because it does not
 * search for n-dimensional points but for n-dimensional rectangles (is that the correct term?).
 * However, if we need only points we can just work with rectangles of area == 0.
 * Another unusual thing is that Search() function does not return matches but instead takes callback function as argument.
 * Each time match is found, it calls that callback function. Second argument of callback function can be used to pass
 * for example vector to be filled with matching point.
 * Great thing is that for each rectangle we can also store some data (it can be int or pointer).
 */
int main() {
	/** template arguments:
     *      A: data (for example pointer to Data class), must be int or pointer 
     *      B: type used for coordinates
     *      C: number of dimensions
     *      D: type used for internal calculations (must be double or float)
     */
    RTree<Data*, int, 2, float> rtree;
    
    vector<int*> points;
    for (int i = 0; i < 100000; i++) {
        int* a = new int[2];
        a[0] = a[1] = i;
        points.push_back(a);
        /**
         * first two arguments are arrays of same length as there are dimensions.
         * We are inserting a rectangle. I need point, so I insert rectangle that is actually a point (area == 0).
         */
        rtree.Insert(a, a, new Data(i,i));
    }
    
    vector<Data*> result;
    int min[] = {3,2};
    int max[] = {3,4};
    rtree.Search(min, max, MySearchCallback, &result);
    
    for (int i = 0; i < result.size(); i++)
        cout << result[i]->a << " " << result[i]->b << endl;
    
    
    int bzvz;
    cin >> bzvz;
    
    for (int i = 0; i < points.size(); i++)
        delete[] (points[i]);
	return 0;
}
