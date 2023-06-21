#ifndef CYC_H
#include <array>
#include <iostream>
/*
 *   Should work for: 5, 7, 8, 10, 11, 12, 13, 14, 16, 17, 19, 20
 *   Not yet implemented:  9, 15, 18
 */
constexpr int phi(int n) {
   switch (n) {
case 5: case 8: case 10: case 12: return 4 ;
case 7: case 9: case 14: case 18: return 6 ;
case 15: case 16: case 20: return 8 ;
case 11: case 13: case 17: case 19: return n-1 ;
default: return -1 ;
   }
} ;
template<int N> struct cyc {
   static const int PHI = phi(N);
   cyc() {
      for (int i=0; i<PHI; i++)
         a[i] = 0 ;
   }
   cyc move(int m) {
      cyc rt(*this), r ;
      int d = 1 - (m >> 1) * 2 ;
      rt.a[0] += d ;
      if (m & 1)
         rotleft(rt, r) ;
      else
         rotright(rt, r) ;
      r.a[0] -= d ;
      return r ;
   }
   static void rotleft(const cyc &rt, cyc &r) ;
   static void rotleftprime(const cyc &rt, cyc &r) {
      int off = rt.a[PHI-1] ;
      r.a[0] = -off ;
      for (int i=1; i<PHI; i++)
         r.a[i] = rt.a[i-1] - off ;
   }
   static void rotleft2(const cyc &rt, cyc &r) {
      int off = rt.a[PHI-1] ;
      r.a[0] = -off ;
      for (int i=1; i<PHI; i++)
         r.a[i] = rt.a[i-1] ;
   }
   static void rotleftprime2(const cyc &rt, cyc &r) {
      int off = rt.a[PHI-1] ;
      r.a[0] = -off ;
      r.a[1] = rt.a[0] + off ;
      for (int i=2; i<PHI; i += 2) {
         r.a[i] = rt.a[i-1] - off ;
         r.a[i+1] = rt.a[i] + off ;
      }
   }
   static void rotleftprime4(const cyc &rt, cyc &r) {
      int off = rt.a[PHI-1] ;
      r.a[0] = -off ;
      r.a[1] = rt.a[0] ;
      r.a[2] = rt.a[1] + off ;
      r.a[3] = rt.a[2] ;
      for (int i=4; i<PHI; i += 4) {
         r.a[i] = rt.a[i-1] - off ;
         r.a[i+1] = rt.a[i] ;
         r.a[i+2] = rt.a[i+1] + off ;
         r.a[i+3] = rt.a[i+2] ;
      }
   }
   static void rotright(const cyc &rt, cyc &r) ;
   static void rotrightprime(const cyc &rt, cyc &r) {
      int off = rt.a[0] ;
      r.a[PHI-1] = -off ;
      for (int i=1; i<PHI; i++)
         r.a[i-1] = rt.a[i] - off ;
   }
   static void rotright2(const cyc &rt, cyc &r) {
      int off = -rt.a[0] ;
      r.a[PHI-1] = off ;
      for (int i=1; i<PHI; i++)
         r.a[i-1] = rt.a[i] ;
   }
   static void rotrightprime2(const cyc &rt, cyc &r) {
      int off = rt.a[0] ;
      r.a[PHI-1] = -off ;
      r.a[0] = rt.a[1] + off ;
      for (int i=2; i<PHI; i += 2) {
         r.a[i-1] = rt.a[i] - off ;
         r.a[i] = rt.a[i+1] + off ;
      }
   }
   static void rotrightprime4(const cyc &rt, cyc &r) {
      int off = rt.a[0] ;
      r.a[PHI-1] = -off ;
      r.a[0] = rt.a[1] ;
      r.a[1] = rt.a[2] + off ;
      r.a[2] = rt.a[3] ;
      for (int i=4; i<PHI; i += 4) {
         r.a[i-1] = rt.a[i] - off ;
         r.a[i] = rt.a[i+1] ;
         r.a[i+1] = rt.a[i+2] + off ;
         r.a[i+2] = rt.a[i+3] ;
      }
   }
   cyc operator+(const cyc &b) const {
      cyc r ;
      for (int i=0; i<PHI; i++)
         r.a[i] = a[i] + b.a[i] ;
      return r ;
   }
   std::array<int, PHI> a ;
} ;
template<> void cyc<5>::rotleft(const cyc &rt, cyc &r) {
   cyc::rotleftprime(rt, r) ;
}
template<> void cyc<5>::rotright(const cyc &rt, cyc &r) {
   cyc::rotrightprime(rt, r) ;
}
template<> void cyc<7>::rotleft(const cyc &rt, cyc &r) {
   cyc::rotleftprime(rt, r) ;
}
template<> void cyc<7>::rotright(const cyc &rt, cyc &r) {
   cyc::rotrightprime(rt, r) ;
}
template<> void cyc<8>::rotleft(const cyc &rt, cyc &r) {
   cyc::rotleft2(rt, r) ;
}
template<> void cyc<8>::rotright(const cyc &rt, cyc &r) {
   cyc::rotright2(rt, r) ;
}
template<> void cyc<10>::rotleft(const cyc &rt, cyc &r) {
   cyc::rotleftprime2(rt, r) ;
}
template<> void cyc<10>::rotright(const cyc &rt, cyc &r) {
   cyc::rotrightprime2(rt, r) ;
}
template<> void cyc<11>::rotleft(const cyc &rt, cyc &r) {
   cyc::rotleftprime(rt, r) ;
}
template<> void cyc<11>::rotright(const cyc &rt, cyc &r) {
   cyc::rotrightprime(rt, r) ;
}
template<> void cyc<12>::rotleft(const cyc &rt, cyc &r) {
   cyc::rotleftprime4(rt, r) ;
}
template<> void cyc<12>::rotright(const cyc &rt, cyc &r) {
   cyc::rotrightprime4(rt, r) ;
}
template<> void cyc<13>::rotleft(const cyc &rt, cyc &r) {
   cyc::rotleftprime(rt, r) ;
}
template<> void cyc<13>::rotright(const cyc &rt, cyc &r) {
   cyc::rotrightprime(rt, r) ;
}
template<> void cyc<14>::rotleft(const cyc &rt, cyc &r) {
   cyc::rotleftprime2(rt, r) ;
}
template<> void cyc<14>::rotright(const cyc &rt, cyc &r) {
   cyc::rotrightprime2(rt, r) ;
}
template<> void cyc<16>::rotleft(const cyc &rt, cyc &r) {
   cyc::rotleft2(rt, r) ;
}
template<> void cyc<16>::rotright(const cyc &rt, cyc &r) {
   cyc::rotright2(rt, r) ;
}
template<> void cyc<17>::rotleft(const cyc &rt, cyc &r) {
   cyc::rotleftprime(rt, r) ;
}
template<> void cyc<17>::rotright(const cyc &rt, cyc &r) {
   cyc::rotrightprime(rt, r) ;
}
template<> void cyc<19>::rotleft(const cyc &rt, cyc &r) {
   cyc::rotleftprime(rt, r) ;
}
template<> void cyc<19>::rotright(const cyc &rt, cyc &r) {
   cyc::rotrightprime(rt, r) ;
}
template<> void cyc<20>::rotleft(const cyc &rt, cyc &r) {
   cyc::rotleftprime4(rt, r) ;
}
template<> void cyc<20>::rotright(const cyc &rt, cyc &r) {
   cyc::rotrightprime4(rt, r) ;
}
template<int N> std::ostream& operator<<(std::ostream& os, const cyc<N>& v) { 
   os << "[" << v.a[0] ;
   for (int i=1; i<cyc<N>::PHI; i++)
      os << " " << v.a[i] ;
   os << "]" ;
   return os ;
}
template<int N> bool operator<(const cyc<N> &a, const cyc<N> &b) {
   return a.a < b.a ;
}
template<int N> bool operator==(const cyc<N> &a, const cyc<N> &b) {
   return a.a == b.a ;
}
template<int N> bool operator!=(const cyc<N> &a, const cyc<N> &b) {
   return a.a != b.a ;
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
   ocyc operator+(const ocyc &b) const {
      ocyc r ;
      for (int i=0; i<ocyc<N>::PHI; i++)
         r.a[i] = this->a[i] + b.a[i] ;
      r.dir = dir ;
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
template<int N> bool operator==(const ocyc<N> &a, const ocyc<N> &b) {
   return a.a == b.a && a.dir == b.dir ;
}
template<int N> bool operator!=(const ocyc<N> &a, const ocyc<N> &b) {
   return a.a != b.a || a.dir != b.dir ;
}
#define CYC_H
#endif
