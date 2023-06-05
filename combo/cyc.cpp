/*
 *   Test cyclotomics.
 */
#include <iostream>
#include <iomanip>
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
int main(int argc, char *argv[]) {
   cout << setprecision(20) ;
   cyclo a ;
   cyclo low ;
   point ap ;
   world[a] = {ap, -1, 1.0} ;
   for (int i=0; i<1000000000; i++) {
      if (a < low) {
         low = a ;
         cout << "New low at " << i << " " << low << endl ;
         cout << ap << endl ;
         cout << expand<7, doub>(a) << endl ;
      }
      int m = (int)(4*drand48()) ;
      a = a.move(m) ;
      ap = ap.move(m) ;
   }
}
