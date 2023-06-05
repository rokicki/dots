/*
 *   Test cyclotomics.
 */
#include <iostream>
#include <map>
#include "cyc.h"
#include "cplexmv.h"
using doub = double ;
using cyclo = ocyc<7> ;
using point = cplexmv<7, doub> ;
using namespace std ;
struct dat {
   point p ;
   int mv ;
   doub minr ;
} ;
map<cyclo, dat> world ;
int main() {
   cyclo a ;
   point ap ;
   for (int i=0; i<100; i++) {
      cout << a << endl ;
      cout << ap << endl ;
      cout << expand<7, doub>(a) << endl ;
      int m = (int)(4*drand48()) ;
      a = a.move(m) ;
      ap = ap.move(m) ;
   }
}
