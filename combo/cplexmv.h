#include <math.h>
#include <ostream>
#include "cyc.h"
double gcos(double v) {
    return cos(v) ;
}
long double gcos(long double v) {
    return cosl(v) ;
}
double gsin(double v) {
    return sin(v) ;
}
long double gsin(long double v) {
    return sinl(v) ;
}
double gatan2(double y, double x) {
    return atan2(y, x) ;
}
long double gatan2(long double y, long double x) {
    return atan2l(y, x) ;
}
double gsqrt(double v) {
    return sqrt(v) ;
}
long double gsqrt(long double v) {
    return sqrtl(v) ;
}
template<typename F> F gpi() {
}
template<> double gpi<double>() {
    return M_PI;
}
template<> long double gpi<long double>() {
    return 3.14159265358979323846264338327950288419716939937510L;
}
template<int N, typename F> struct cplexmv {
    cplexmv(): y(0), x(0) {}
    cplexmv(const F &x_, const F &y_) : y(y_), x(x_) {}
    static cplexmv* roots() {
        static cplexmv a[N];
        if (a[0].x == 0)
            for (int i=0; i<N; i++)
               a[i] = {gcos(2*gpi<F>()*i/N), gsin(2*gpi<F>()*i/N)};
        return a;
    }
    cplexmv move(int mv) const {
        auto a = roots();
        F off = 1-(2&mv);
        int ang = (mv & 1) ? 1 : N-1 ;
        return ((*this)+off)*a[ang]-off ;
    }
    F r2formove(int mv) const {
        F off = 1-(2&mv);
        return (x+off)*(x+off)+y*y;
    }
    cplexmv &operator+=(const cplexmv &b) {
        y += b.y ;
        x += b.x ;
        return *this ;
    }
    cplexmv operator+(const cplexmv &b) const {
        return {x+b.x,y+b.y} ;
    }
    cplexmv operator+(const F &b) const {
        return {x+b,y} ;
    }
    cplexmv &operator-=(const cplexmv &b) {
        y -= b.y ;
        x -= b.x ;
        return *this ;
    }
    cplexmv operator-(const cplexmv &b) const {
        return {x-b.x,y-b.y} ;
    }
    cplexmv operator-(const F &b) const {
        return {x-b,y} ;
    }
    cplexmv operator*=(const cplexmv &b) {
        F xx = x * b.x - y * b.y ;
        y = x * b.y + y * b.x ;
        x = xx ;
        return *this ;
    }
    cplexmv operator*(const cplexmv &b) const {
        return {x*b.x-y*b.y, x*b.y+y*b.x} ;
    }
    cplexmv operator*(int m) const {
        return {x*m, y*m} ;
    }
    F degrees() const {
        return 180 / gpi<F>() * gatan2(y, x) ;
    }
    F y, x;
};
template<int N, typename F> std::ostream& operator<<(std::ostream& os, const cplexmv<N, F>& v) {
   os << "(" << v.x << "," << v.y << ")" ;
   return os ;
}
template<int N, typename F> cplexmv<N, F> expand(const cyc<N> &c) {
    cplexmv<N, F> v ;
    auto a = v.roots() ;
    for (int i=0; i<N-1; i++)
       v += a[i] * c.a[i];
    return v;
}
