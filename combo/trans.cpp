/*
 *   Test cyclotomics.
 */
#include <iostream>
#include <iomanip>
#include <map>
#include "cyc.h"
#include "cplexmv.h"
using ll = long long ;
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
const char *mvstr[] = {"a", "a'", "b", "b'"} ;
int main(int argc, char *argv[]) {
   cout << setprecision(20) ;
   doub r = atof(argv[1]) ;
   doub rr = r * r ;
   cyclo a, z ;
   point ap ;
   world[a] = {ap, -1, 1.0} ;
   vector<cyclo> lev ;
   lev.push_back(a) ;
   doub low = 10000 ;
   for (int d=0; ; d++) {
      if (lev.size() == 0)
         break ;
      vector<cyclo> nlev ;
      for (auto &a: lev) {
         auto ap = expand<7, doub>(a) ;
         doub ra = world[a].minr ;
         doub dc = ap.x * ap.x + ap.y * ap.y ;
         if (a.dir == 0 && dc < 10 * low && !(a == z)) {
            if (dc < low)
               low = dc ;
            cout << " " << d << " " << a << " " << ap << " " << gsqrt(dc) << " " << gsqrt(ra) << " " << ap.degrees() << endl ;
            cyclo t = a ;
            while (t != z) {
               int bmv = world[t].mv ^ 1 ;
               cout << " " << mvstr[bmv] ;
               t = t.move(bmv) ;
            }
            cout << endl ;
         }
         for (int m2=0; m2<4; m2+=2) {
            doub m2a = ap.r2formove(m2) ;
            if (m2a < rr)
               for (int m=m2; m<m2+2; m++) {
                  auto b = a.move(m) ;
                  auto bp = expand<7, doub>(b) ;
                  if (bp.r2formove(m2^2) < rr) {
                     auto ptr = world.find(b) ;
                     if (ptr == world.end()) {
                        world[b] = {bp, m, max(ra, m2a)} ;
                        nlev.push_back(b) ;
                     } else if (max(ra, m2a) < ptr->second.minr) {
                        ptr->second.minr = max(ra, m2a) ;
                        ptr->second.mv = m ;
                     }
                  }
               }
         }
      }
      swap(lev, nlev) ;
   }
}
