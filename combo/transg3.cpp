/*
 *   Test cyclotomics.
 */
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>
#include "cyc.h"
#include "cplexmv.h"
#include "duration.h"
const int N = 7 ;
using ll = long long ;
using doub = long double ;
using cyclo = ocyc<N> ;
using point = cplexmv<N, doub> ;
double start ;
using namespace std ;
struct dat {
   point p ;
   int mv ;
   doub minr ;
} ;
map<cyclo, dat> world ;
const char *mvstr[] = {"a", "a'", "b", "b'"} ;
ll target = 65536 ;
int main(int argc, char *argv[]) {
   cout << setprecision(20) ;
   doub r = atof(argv[1]) ;
   doub rr = r * r ;
   cyclo a, z ;
   point ap ;
   ll hiw = -1 ;
   world[a] = {ap, -1, 1.0} ;
   vector<cyclo> lev, plev ;
   lev.push_back(a) ;
   ll tot = 0 ;
   for (int d=0; ; d++) {
      tot += lev.size() ;
      if (tot >= target) {
          cout << "> " << target << " in " << duration() << endl << flush ;
          target += target / 10 ;
      }
      if ((ll)lev.size() > hiw) {
         hiw = lev.size() ;
         cout << "At depth " << d << " " << lev.size() << " tot " << tot << endl << flush ;
      }
      if (lev.size() == 0)
         break ;
      vector<cyclo> nlev ;
      for (auto &a: lev) {
         auto ap = expand<N, doub>(a) ;
         auto it = world.find(a) ;
         doub ra = it->second.minr ;
         if (abs(ap.x) < 1e-4 && abs(ap.y) < 1e-4)
           cout << " " << ap.x << " " << ap.y << endl ;
         for (int m2=0; m2<4; m2+=2) {
            doub m2a = ap.r2formove(m2) ;
            if (m2a < rr)
               for (int m=m2; m<m2+2; m++) {
                  auto b = a.move(m) ;
/*
 cout << "Move is " << m << endl ;
 cout << ap << " -> " <<  ap.move(m) << endl ;
 cout << a << " -> " << b << endl ;
 cout << expand<N, doub>(a) << " -> " << expand<N, doub>(b) << endl ;
 if (abs(expand<N, doub>(b).x - ap.move(m).x) > 1e-5 ||
     abs(expand<N, doub>(b).y - ap.move(m).y) > 1e-5)
    exit(10) ;
 */
                  auto bp = expand<N, doub>(b) ;
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
      for (auto &v: plev)
         world.erase(v) ;
      swap(plev, lev) ;
      swap(lev, nlev) ;
   }
}
