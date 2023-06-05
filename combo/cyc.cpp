/*
 *   Test cyclotomics.
 */
#include <iostream>
#include <iomanip>
#include <map>
#include "cyc.h"
#include "cplexmv.h"
using ll = long long ;
using doub = long double ;
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
   doub r = atof(argv[1]) ;
   doub rr = r * r ;
   cyclo a ;
   cyclo z ;
   cyclo lowa ;
   point ap ;
   doub low = 100000000 ;
   world[a] = {ap, -1, 1.0} ;
   int lm = 0 ;
   for (ll i=0; ; i++) {
      doub dd = ap.x * ap.x + ap.y * ap.y ;
      if (!(a == z) && !(a == lowa) && dd < low) {
         low = dd ;
         lowa = a ;
         cout << "New low at " << i << " " << low << endl ;
         cout << a << endl ;
         cout << ap << endl ;
         cout << expand<7, doub>(a) << endl ;
      }
      int m = -1 ;
      while (1) {
         m = 3&(int)(lm+1+3*drand48()) ;
         if (ap.r2formove(m) < rr)
            break ;
      }
      lm = m ;
      a = a.move(m) ;
      ap = ap.move(m) ;
   }
}
