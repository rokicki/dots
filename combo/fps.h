#include <math.h>
#include <ostream>
#include <vector>
using ll = long long ;
using ull = unsigned long long ;
using lll = __int128_t ;
using ulll = __uint128_t ;
static constexpr double shift_to_double(int n) {
   double r = 1 ;
   for (; n>0; n--)
      r *= 2.0 ;
   for (; n<0; n++)
      r *= 0.5 ;
   return r ;
}
template<int N_, int U_> struct fixpt {
   static const ulll LOW64 = 0xffffffffffffffffLL ;
   static const ulll HALF =  0x8000000000000000LL ;
   static const int N = N_ ;
   static const int U = U_ ;
   ll a;
   ull b[N-1];
   fixpt() {
      a = 0 ;
      for (int i=0; i<N-1; i++)
         b[i] = 0 ;
   }
   fixpt(ll v) {
      a = v << (64-U) ;
      for (int i=0; i<N-1; i++)
         b[i] = 0 ;
   }
   fixpt(double v) {
      setfromdouble(v) ;
   }
   fixpt(int v) {
      setfromdouble((double)v) ;
   }
   fixpt(const char *s, int dummy) {
      setfromstring(s) ;
   }
   fixpt<N, U> &operator=(ll v) {
      a = v << (64-U) ;
      for (int i=0; i<N-1; i++)
         b[i] = 0 ;
      return *this ;
   }
   fixpt<N, U> &operator+=(const fixpt<N, U> &e) {
      ulll nd = 0;
      for (int i=N-2; i>=0; i--) {
         nd = b[i] + (ulll)e.b[i] + nd;
         b[i] = nd;
         nd >>= 64 ;
      }
      a = a + e.a + nd ;
      return *this ;
   }
   fixpt<N, U> &operator+=(ll v) {
      a += v << (64-U) ;
      return *this ;
   }
   fixpt<N, U> &operator+=(int v) {
      a += ((ll)v) << (64-U) ;
      return *this ;
   }
   fixpt<N, U> &operator-=(const fixpt<N, U> &e) {
      ulll nd = 1;
      for (int i=N-2; i>=0; i--) {
         nd = b[i] + (ulll)~e.b[i] + nd;
         b[i] = nd;
         nd >>= 64 ;
      }
      a = a + ~e.a + nd ;
      return *this ;
   }
   fixpt<N, U> &operator-=(ll v) {
      a -= v << (64-U) ;
      return *this ;
   }
   fixpt<N, U> &operator-=(int v) {
      a -= ((ll)v) << (64-U) ;
      return *this ;
   }
   void negate() {
      a = ~a ;
      for (int i=0; i<N-1; i++)
         b[i] = ~b[i] ;
      bump() ;
   }
   fixpt<N, U> operator-() const {
      fixpt<N, U> r = *this ;
      r.negate() ;
      return r ;
   }
   fixpt<N, U>& squareinplace() {
      if (a < 0)
         negate() ;
      ulll acc = 0 ;
      ulll s = HALF ;
      for (int j=0; j<N-1; j++) {
         s = s + b[j] * (ulll)b[N-2-j] ;
         acc += s >> 64 ;
         s &= LOW64 ;
      }
      ull leftover = 0 ;
      for (int i=N-2; i>=0; i--) {
         ulll s = acc ;
         acc = 0 ;
         for (int j=0; j<i; j++) {
            s = s + b[j] * (ulll)b[i-j-1] ;
            acc += s >> 64 ;
            s &= LOW64 ;
         }
         ulll r = a * (ulll)b[i] + (ulll)b[i] * a + s ;
         if (i < N-2)
            b[i+1] = (r << U) + (leftover >> (64-U)) ;
         leftover = r & LOW64 ;
         acc += r >> 64 ;
      }
      ulll r = a * (ulll)a + acc ;
      b[0] = (r << U) + (leftover >> (64-U)) ;
      a = r >> (64-U) ;
      return *this ;
   }
   fixpt<N, U> square() const {
      fixpt<N, U> r = *this ;
      return r.squareinplace() ;
   }
   fixpt<N, U>& operator*=(fixpt<N, U> e) {
      int neg = 0 ;
      if (e.a < 0) {
         e.negate() ;
         neg = 1 ;
      }
      if (a < 0) {
         negate() ;
         neg = 1 - neg ;
      }
      ulll acc = 0 ;
      ulll s = HALF ;
      for (int j=0; j<N-1; j++) {
         s = s + b[j] * (ulll)e.b[N-2-j] ;
         acc += s >> 64 ;
         s &= LOW64 ;
      }
      ull leftover = 0 ;
      for (int i=N-2; i>=0; i--) {
         ulll s = acc ;
         acc = 0 ;
         for (int j=0; j<i; j++) {
            s = s + b[j] * (ulll)e.b[i-j-1] ;
            acc += s >> 64 ;
            s &= LOW64 ;
         }
         ulll r = a * (ulll)e.b[i] + (ulll)b[i] * e.a + s ;
         if (i < N-2)
            b[i+1] = (r << U) + (leftover >> (64-U)) ;
         leftover = r & LOW64 ;
         acc += r >> 64 ;
      }
      ulll r = a * (ulll)e.a + acc ;
      b[0] = (r << U) + (leftover >> (64-U)) ;
      a = r >> (64-U) ;
      if (neg)
         negate() ;
      return *this ;
   }
   fixpt& operator/=(ull d) {
      ulll r ;
      if (a < 0) {
         r = (d + ((a + 1) % d)) - 1 ;
         a = (a - r) / d ;
      } else {
         r = a % d ;
         a = a / d ;
      }
      for (int i=0; i<N-1; i++) {
         r = (r << 64) + b[i] ;
         b[i] = r / d ;
         r = r % d ;
      }
      if (r + r >= d) {
         bump() ;
      }
      return *this ;
   }
   fixpt<N, U>& operator*=(ull m) {
      ulll r = 0 ;
      for (int i=N-2; i>=0; i--) {
         r = b[i] * (ulll)m + r ;
         b[i] = r ;
         r >>= 64 ;
      }
      a = a * m + r ;
      return *this ;
   }
   void bump() {
      for (int i=N-2; i>=0; i--)
         if (++b[i])
            return ;
      a++ ;
   }
   void setfromdouble(double v) {
      const double scale = shift_to_double(64-U) ;
      v *= scale ;
      a = floor(v) ;
      b[0] = (ull)((v - a) * 0x1.p64) ;
      for (int i=1; i<N-1; i++)
         b[i] = 0 ;
   }
   const char *setfromstring(const char *s) {
      int neg = 0 ;
      if (*s == '-') {
         s++ ;
         neg = 1 ;
      }
      for (int i=0; i<N-1; i++)
         b[i] = 0 ;
      a = 0 ;
      while ('0' <= *s && *s <= '9')
         a = 10 * a + *s++ - '0' ;
      if (*s == '.')
         s++ ;
      ll fulla = a << (64 - U);
      a = 0 ;
      ll GUARD = 1 ;
      while (11*GUARD < (1LL << U))
         GUARD *= 5 ;
      *this *= GUARD ;
      fixpt<N, U> v10 ;
      v10 = GUARD ;
      for (; '0' <= *s && *s <= '9'; s++) {
         v10 /= 10 ;
         for (int i='0'; i<*s; i++) {
            *this += v10 ;
         }
      }
      *this /= GUARD ;
      a += fulla ;
      if (neg)
         negate() ;
      return s ;
   }
   double getdouble() const {
      double v = b[N-2] ;
      for (int i=N-3; i>=0; i--)
         v = b[i] + 0x1p-64 * v ;
      const double scale1 = shift_to_double(U-64) ;
      const double scale2 = shift_to_double(U-128) ;
      return a * scale1 + v * scale2 ;
   }
   bool operator==(const fixpt<N, U> &e) const {
      if (a != e.a)
         return false ;
      for (int i=0; i<N-1; i++)
         if (b[i] != e.b[i])
            return false ;
      return true ;
   }
   bool operator!=(const fixpt<N, U> &e) const {
      if (a != e.a)
         return true ;
      for (int i=0; i<N-1; i++)
         if (b[i] != e.b[i])
            return true ;
      return false ;
   }
   bool operator==(ll e) const {
      if (a != (e << (64-U)))
         return false ;
      for (int i=0; i<N-1; i++)
         if (b[i] != 0)
            return false ;
      return true ;
   }
   bool operator!=(ll e) const {
      if (a != (e << (64-U)))
         return true ;
      for (int i=0; i<N-1; i++)
         if (b[i] != 0)
            return true ;
      return false ;
   }
   bool operator<(const fixpt<N, U> &e) const {
      if (a != e.a)
         return a < e.a ;
      for (int i=0; i<N-1; i++)
         if (b[i] != e.b[i])
            return b[i] < e.b[i] ;
      return false ;
   }
   bool operator<(ll e) const {
      if (a != e)
         return a < e ;
      return false ;
   }
   bool operator>(const fixpt<N, U> &e) const {
      return e < *this ;
   }
} ;
template<typename fp> constexpr int getGoodShift() { return (fp::U / 2 - 1) & ~1 ; }
template<> constexpr int getGoodShift<double>() { return 0 ; }
template<> constexpr int getGoodShift<long double>() { return 0 ; }
#ifdef __linux
template<> constexpr int getGoodShift<__float128>() { return 0 ; }
#endif
template<int N, int U> fixpt<N, U> operator+(const fixpt<N, U> &e, const fixpt<N, U> &f) {
   fixpt<N, U> r = e ;
   r += f ;
   return r ;
}
template<int N, int U> fixpt<N, U> operator+(const fixpt<N, U> &e, ll f) {
   fixpt<N, U> r = e ;
   r.a += f << (64-U) ;
   return r ;
}
template<int N, int U> fixpt<N, U> operator-(const fixpt<N, U> &e, const fixpt<N, U> &f) {
   fixpt<N, U> r = e ;
   r -= f ;
   return r ;
}
template<int N, int U> fixpt<N, U> operator-(const fixpt<N, U> &e, ll f) {
   fixpt<N, U> r = e ;
   r.a -= f << (64-U) ;
   return r ;
}
template<int N, int U> fixpt<N, U> operator*(const fixpt<N, U> &e, const fixpt<N, U> &f) {
   fixpt<N, U> r = e ;
   r *= f ;
   return r ;
}
template<int N, int U> fixpt<N, U> operator*(const fixpt<N, U> &e, ll f) {
   fixpt<N, U> r = e ;
   r *= f ;
   return r ;
}
template<int N, int U> fixpt<N, U> operator/(const fixpt<N, U> &e, ll f) {
   fixpt<N, U> r = e ;
   r /= f ;
   return r ;
}
template<int N, int U> fixpt<N, U> abs(const fixpt<N, U> &e) {
   fixpt<N, U> r = e ;
   if (r.a < 0)
      r.negate() ;
   return r ;
}
template<int N, int U> std::ostream& operator<<(std::ostream& os, const fixpt<N, U>& v) {
   if (v.a < 0) {
      os << "-" << -v ;
      return os ;
   }
   os << (v.a >> (64-U)) << "." ;
   fixpt<N, U> n = v ;
   int digs = 1 + floor((N * 64 - U) * .30103) ;
   for (int i=0; i<digs; i++) {
      n.a &= (1LL << (64-U)) - 1 ;
      n *= 10 ;
      os << (n.a >> (64-U)) ;
   }
   return os;
}
template<typename fp> fp sumForPi(int div, fp a, int denom) {
   if (a == 0)
      return a ;
   return a/div - sumForPi(div+2, a/denom, denom) ;
}
template<typename fp> fp calculateBigPi() {
   const int SH = getGoodShift<fp>() ;
   fp a = 16LL << SH ;
   a /= 5 ;
   fp v1 = sumForPi(1, a, 5*5) ;
   a = 4LL << SH ;
   a /= 239 ;
   return v1 - sumForPi(1, a, 239*239) ;
}
template<typename fp> fp sumForTrig(int div, fp a, fp m2) {
   if (a == 0)
      return a ;
   return a-sumForTrig(div+2, a*m2/(div*(div+1)), m2) ;
}
template<typename fp> std::pair<fp, fp> rootofunity(int i, int n, fp bigpi) {
   const int SH = getGoodShift<fp>() ;
   int q = 4 * i / n ;
   int x = 0 ;
   int negc = 0 ;
   int negs = 0 ;
   switch(q) {
case 0:
      x = 4 * i ;
      break ;
case 1:
      x = 2 * n - 4 * i ;
      negc++ ;
      break ;
case 2:
      x = 4 * i - 2 * n ;
      negc++ ;
      negs++ ;
      break ;
case 3:
      x = 4 * (n - i) ;
      negs++ ;
      break ;
   }
   fp a = bigpi * x / (2 * n) ;
   fp m = a / (1LL << (SH >> 1)) ;
   fp m2 = m * m / (1LL << SH) ;
   m /= 1LL << (SH >> 1) ;
   fp c = 1LL<<SH ;
   fp ci = sumForTrig(1, c, m2) / (1LL << SH) ;
   if (negc)
      ci = - ci ;
   fp si = sumForTrig(2, a, m2) / (1LL << SH) ;
   if (negs)
      si = - si ;
   return {ci, si} ;
}
template<int N, int U> double getdouble(const fixpt<N, U> &v) {
   return v.getdouble() ;
}
template<int N, int U> const char *setfromstring(const char *s, fixpt<N,U> &d) {
   return d.setfromstring(s) ;
}
namespace rsort {
   template<int N, int U> struct sortdata<fixpt<N, U>> {
      static constexpr int keylength = sizeof(fixpt<N, U>) ;
      static constexpr int signbyte = 7 ;
      static int byteorder(int i) { return ((8*N-1)-i)^7 ; }
      static constexpr bool revneg = false ;
   } ;
}
