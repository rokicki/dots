#ifndef FRONT_H
#define FRONT_H
#include "rsort.h"
#include <algorithm>
#include <vector>
namespace front {
   using ull = unsigned long long ;
   template<typename KEY, typename ENT> class front {
public:
      front(KEY epsarg): w(), wp(), wn(), d(0), eps(epsarg), sorter() {
         epseps = eps * eps ;
      }
      void start(std::vector<ENT> zero) {
         w = zero ;
         fareq = 1000 ;
         sorter.sort(w) ;
      }
      KEY dd(std::pair<KEY, KEY> &a, std::pair<KEY, KEY> &b) {
         KEY d1 = a.first - b.first ;
         KEY d2 = a.second - b.second ;
         return d1 * d1 + d2 * d2 ;
      }
      void pruneback() {
         sorter.sort(wn) ;
         ull wi = 0 ;
         ull wpi = 0 ;
         ull wci = 0 ;
         for (ull i=0; i<(ull)wn.size(); i++) {
            for (ull j=i+1; j<(ull)wn.size() &&
                            wn[i].p.first + eps > wn[j].p.first; j++)
               if (dd(wn[i].p, wn[j].p) < epseps) {
                  fareq = std::max(fareq, dd(wn[i].p, wn[j].p)) ;
                  goto skipit ;
               }
            while (wpi < (ull)wp.size() &&
                   wn[i].p.first > wp[wpi].p.first + eps)
               wpi++ ;
            for (ull j=wpi; j<(ull)wp.size() &&
                            wn[i].p.first + eps > wp[j].p.first; j++)
               if (dd(wn[i].p, wp[j].p) < epseps) {
                  fareq = std::max(fareq, dd(wn[i].p, wp[j].p)) ;
                  goto skipit ;
               }
            while (wci < (ull)w.size() &&
                   wn[i].p.first > w[wci].p.first + eps)
               wci++ ;
            for (ull j=wci; j<(ull)w.size() &&
                            wn[i].p.first + eps > w[j].p.first; j++)
               if (dd(wn[i].p, w[j].p) < epseps) {
                  fareq = std::max(fareq, dd(wn[i].p, w[j].p)) ;
                  goto skipit ;
               }
            wn[wi++] = wn[i] ;
skipit: ; 
         }
         wn.resize(wi) ;
         std::swap(wp, w) ;
         std::swap(w, wn) ;
         wn.clear() ;
      }
      std::vector<ENT> w, wp, wn ;
      int d ;
      KEY eps, epseps, fareq ;
      rsort::rsort<KEY, ENT> sorter;
   } ;
} ;
#endif
