#ifndef CYC_H
#include <array>
#include <iostream>
template<int N> struct cyc {
   cyc() {
      for (int i=0; i<N-1; i++)
         a[i] = 0 ;
   }
   cyc move(int m) {
      cyc r ;
      int v[N] ;
      int d = 1 - (m >> 1) * 2 ;
      if (m & 1) {
         v[0] = -d ;
         v[1] = a[0] + d ;
         for (int i=1; i<N-1; i++)
            v[i+1] = a[i] ;
      } else {
         v[N-2] = 0 ;
         v[N-1] = a[0] + d ;
         for (int i=1; i<N-1; i++)
            v[i-1] = a[i] ;
         v[0] -= d ;
      }
      int o = v[N-1] ;
      for (int i=0; i<N-1; i++)
         r.a[i] = v[i] - o ;
      return r ;
   }
   std::array<int, N-1> a ;
} ;
template<int N> std::ostream& operator<<(std::ostream& os, const cyc<N>& v) { 
   os << "[" << v.a[0] ;
   for (int i=1; i<N-1; i++)
      os << " " << v.a[i] ;
   os << "]" ;
   return os ;
}
template<int N> bool operator<(const cyc<N> &a, const cyc<N> &b) {
   return a.a < b.a ;
}
template<int N> struct ocyc: cyc<N> {
   ocyc() {
      dir = 0 ;
   }
   ocyc move(int m) {
      ocyc r ;
      r.a = ((cyc<N>*)this)->move(m).a ;
      r.dir = (dir + 1 - (2 * (m & 1)) + N) % N ;
      return r ;
   }
   int dir ;
} ;
template<int N> std::ostream& operator<<(std::ostream& os, const ocyc<N>& v) { 
   os << (cyc<N>)(v) << "@" << v.dir ;
   return os ;
}
template<int N> bool operator<(const ocyc<N> &a, const ocyc<N> &b) {
   if (a.a != b.a)
      return a.a < b.a ;
   return a.dir < b.dir ;
}
#define CYC_H
#endif
