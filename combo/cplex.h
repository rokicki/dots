#include <iostream>
template<typename F> struct pt {
   pt() : y(0), x(0) {}
   pt<F> &operator+=(pt<F> &b) {
      y += b.y ;
      x += b.x ;
      return *this ;
   }
   pt<F> operator+(const pt<F> &b) {
      return {y+b.y,x+b.x} ;
   }
   pt<F> &operator-=(pt<F> &b) {
      y -= b.y ;
      x -= b.x ;
      return *this ;
   }
   pt<F> operator-(const pt<F> &b) {
      return {y-b.y,x-b.x} ;
   }
   pt<F> operator*=(const pt<F> &b) {
      F xx = x * b.x - y * b.y ;
      y = x * b.y + y * b.x ;
      x = xx ;
      return *this ;
   }
   pt<F> operator*(const pt<F> &b) {
      return {x*b.y+y*b.x, x*b.x-y*b.y} ;
   }
   F y, x;
} ;
template<typename F> std::ostream& operator<<(std::ostream& os, const pt<F>& v) {
   os << "(" << v.x << "," << v.y << ")" ;
   return os ;
}

