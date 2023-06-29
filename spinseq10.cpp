#include "combo/front.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <iomanip>
#include <math.h>
#include <sys/time.h>
#include <cstring>
#include "combo/fps.h"
#ifdef __linux__
#include <x86intrin.h>
#endif
using namespace std ;
using ll = long long ;
using uchar = unsigned char ;
#ifdef USEQUAD
#include <quadmath.h>
using doub = __float128 ;
const char *ftype = "quadmath" ;
#define DIGITS 33
doub geps = 1e-22 ;
const char *setfromstring(const char *s, doub &v) {
   v = strtoflt128(s, 0) ;
   return s ;
}
double getdouble(doub v) {
   return (double)v ;
}
ostream &operator<<(ostream& __out, doub d) {
   char buf[100] ;
   quadmath_snprintf(buf, 100, "%.33Qe", d) ;
   return __out << buf ;
}
#else
#ifdef USEDOUBLE
using doub = double ;
const char *ftype = "double" ;
#define DIGITS 15
doub geps = 1e-10 ;
const char *setfromstring(const char *s, doub &v) {
   v = atof(s) ;
   return s ;
}
double getdouble(doub v) {
   return (double)v ;
}
#else
#ifdef DOUB
using doub = fixpt<DOUB> ;
#define STRINGIFY2(a,b) #a "," #b
#define STRINGIFY(a) STRINGIFY2(a)
const char *ftype = "fixpt<" STRINGIFY(DOUB) ">" ;
doub geps = shift_to_double(10-doub::N*32) ;
#else
using doub = long double ;
const char *ftype = "long double" ;
const char *setfromstring(const char *s, doub &v) {
   v = strtold(s, 0) ;
   return s ;
}
double getdouble(doub v) {
   return (double)v ;
}
#define DIGITS 18
doub geps = 1e-12 ;
#endif
#endif
#endif
doub ar, br ;
int an, bn ;
ll deepest = 0 ;
static double start ;
using pt = pair<doub, doub> ;
doub closene = 1000 ;
struct ent {
   pt p ;
   int m ;
   const bool operator <(const ent &b)const {
      if (p.second != b.p.second)
         return p.second < b.p.second ;
      return p.first < b.p.first ;
   }
} ;
double walltime() {
   struct timeval tv ;
   gettimeofday(&tv, 0) ;
   return tv.tv_sec + 0.000001 * tv.tv_usec ;
}
double duration() {
   double now = walltime() ;
   double r = now - start ;
   start = now ;
   return r ;
}
template<typename T> T d2(const T &x, const T &y) {
   return x*x+y*y ;
}
doub dd(const pt &a, const pt &b) {
   return d2(a.second-b.second, a.first-b.first) ;
}
double maxtime = 1000000000000 ;
ll maxord = 10000000000000 ;
ll maxlev = 100000000 ;
ll target = 65536 ;
int pointtype = 1 ;
int verbose = 0 ;
int userandom = 0 ;
vector<vector<int>> seqs ; // a, a', b, b'
vector<vector<pt>> seqpts ;
vector<int> seqrot ;
vector<pt> seqoff ;
set<vector<int>> seqseen ;
doub san, can, sbn, cbn, arar, brbr, hiyes, lono ;
vector<pt> roots ;
doub side(const pt &a, const pt &b, const pt &c) {
   if (b == c)
      return 0 ;
   pt ab{b.second-a.second, b.first-a.first}, ac{c.second-a.second, c.first-a.first} ;
   return ab.second*ac.first-ab.first*ac.second ;
}
vector<pt> convexhull(vector<pt> &pts) {
   vector<pt> r ;
   int lowpt = 0 ;
   for (int i=1; i<(int)pts.size(); i++)
      if (pts[i].first < pts[lowpt].first ||
          (pts[i].first == pts[lowpt].first && pts[i].second < pts[lowpt].second))
         lowpt = i ;
   swap(pts[0], pts[lowpt]) ;
   r.push_back(pts[0]) ;
   sort(pts.begin()+1, pts.end(), [&pts](const pt &a, const pt &b) {
           doub t = side(pts[0], a, b) ;
           if (t != 0)
              return t > 0 ;
           return dd(pts[0], a) > dd(pts[0], b) ;
        }) ;
   r.push_back(pts[1]) ;
   for (int i=2; i<(int)pts.size(); i++) {
      while (1) {
         doub t = side(r[r.size()-2], r[r.size()-1], pts[i]) ;
         if (t < 0) {
            r.pop_back() ;
         } else if (t > 0) {
            r.push_back(pts[i]) ;
            break ;
         } else if (dd(pts[i], r[r.size()-2]) > dd(r[r.size()-1], r[r.size()-2])) {
            r.pop_back() ;
            r.push_back(pts[i]) ;
            break ;
         } else {
            break ;
         }
      }
   }
   cout << "From " << pts.size() << " to " << r.size() << endl ;
   return r ;
}
void checkseq(const vector<int> &seq) {
   if (seqseen.find(seq) != seqseen.end())
      return ;
   seqseen.insert(seq) ;
   seqs.push_back(seq) ;
   pt p = {0, 0} ;
   vector<pt> pts ;
   pts.push_back({roots[0].second, roots[0].first}) ;
   pts.push_back({-roots[0].second, -roots[0].first}) ;
   int rot = 0 ;
   for (auto m: seq) {
      doub nx=0, ny=0 ;
      switch (m) {
case 0: // a
         nx = (p.second + 1) * can + p.first * san - 1 ;
         ny = p.first * can - (p.second + 1) * san ;
         rot = (rot + an - 1) % an ;
         pts.push_back({roots[rot].first * ny - roots[rot].second * (nx - 1),
                        roots[rot].first * (nx - 1) + roots[rot].second * ny}) ;
         break ;
case 1: // a'
         nx = (p.second + 1) * can - p.first * san - 1 ;
         ny = p.first * can + (p.second + 1) * san ;
         rot = (rot + 1) % an ;
         pts.push_back({roots[rot].first * ny - roots[rot].second * (nx - 1),
                        roots[rot].first * (nx - 1) + roots[rot].second * ny}) ;
         break ;
case 2:
         nx = (p.second - 1) * cbn + p.first * sbn + 1 ;
         ny = p.first * cbn - (p.second - 1) * sbn ;
         rot = (rot + an - 1) % an ;
         pts.push_back({roots[rot].first * ny - roots[rot].second * (nx + 1),
                        roots[rot].first * (nx + 1) + roots[rot].second * ny}) ;
         break ;
case 3:
         nx = (p.second - 1) * cbn - p.first * sbn + 1 ;
         ny = p.first * cbn + (p.second - 1) * sbn ;
         rot = (rot + 1) % an ;
         pts.push_back({roots[rot].first * ny - roots[rot].second * (nx + 1),
                        roots[rot].first * (nx + 1) + roots[rot].second * ny}) ;
         break ;
default:
         cout << "Bad move " << m << endl ;
      }
      p = {ny, nx} ;
   }
   // now we compute the convex hull of these points.
   pts = convexhull(pts) ;
   //
   seqpts.push_back(pts) ;
   seqrot.push_back((an - rot) % an) ;
   seqoff.push_back(p) ;
}
void parseaddseq(string &s) {
   vector<int> seq ;
   int v = 0 ;
   for (int i=0; i<=(int)s.size(); i++) {
      if (i >= (int)s.size() || s[i] == ' ') {
         seq.push_back(v) ;
         v = 0 ;
      } else if (s[i] == 'a')
         v = 0 ;
      else if (s[i] == 'b')
         v = 2 ;
      else if (s[i] == '\'')
         v |= 1 ;
      else {
         cerr << "Bad char in seq " << s[i] << endl ;
         exit(10) ;
      }
   }
   checkseq(seq) ;
   reverse(seq.begin(), seq.end()) ;
   for (auto &v: seq)
      v ^= 1 ;
   checkseq(seq) ;
}
int slowpart(const doub &dd) {
   if (dd > lono)
      return 1 ;
   if (dd < arar) {
      if (dd > hiyes) {
         hiyes = dd ;
         cout << "New hiyes at " << hiyes << endl << flush ;
      }
      return 0 ;
   }
   if (dd < lono) {
      lono = dd ;
      cout << "New lono at  " << lono << endl << flush ;
   }
   return 1 ;
}
/*
 *   We break out the common case here, and hope the compiler inlines this
 *   code.  If this code is not inlined, the performance drops a lot.
 */
int cmprec(const doub &a, const doub &b) {
   doub dd = d2(a, b) ;
   if (dd < hiyes) // this one hits almost always
      return 0 ;
   return slowpart(dd) ;
}
int fastseqcheck(const pt &p1, int seqi) {
   auto &pts = seqpts[seqi] ;
   for (int i=0; i<(int)pts.size(); i++) {
      auto &cp = pts[i] ;
      if (cmprec(cp.second + p1.second, cp.first + p1.first)) {
         if (i > 0)
            swap(pts[i], pts[i>>1]) ;
         return 0 ;
      }
   }
   return 1 ;
}
void fastseqapply(const pt &p1, int seqi, pt &p2) {
   int rot = seqrot[seqi] ;
   const pt &poff = seqoff[seqi] ;
   p2 = {
    p1.first * roots[rot].first - p1.second * roots[rot].second + poff.first,
    p1.second * roots[rot].first + p1.first * roots[rot].second + poff.second} ;
}
int main(int argc, char *argv[]) {
   cout << "-" ;
   for (int i=0; i<argc; i++)
      cout << " " << argv[i] ;
   cout << endl ;
   while (argc > 1 && argv[1][0] == '-') {
      argc-- ;
      argv++ ;
      switch (argv[0][1]) {
case '2':
case '3':
         pointtype = argv[0][1] - '0' ;
         break ;
case 't':
         maxtime = stold(argv[1]) ;
         argc-- ;
         argv++ ;
         break ;
case 'r':
         userandom = 1 ;
         break ;
case 'o':
         maxord = atoll(argv[1]) ;
         argc-- ;
         argv++ ;
         break ;
case 'e':
         setfromstring(argv[1], geps) ;
         argc-- ;
         argv++ ;
         break ;
case 'v':
         verbose++ ;
         break ;
default:
         cerr << "No such argument" << endl ;
         exit(10) ;
      }
   }
   srand48(an * 11 + bn * 113 + time(0)) ;
   duration() ;
#ifdef DIGITS
   cout << setprecision(DIGITS) ;
#endif
   doub rr ;
   const char *rrstring = "?" ;
   hiyes = 0 ;
   lono = 1000000 ;
   if (argc == 3) {
      an = atol(argv[1]) ;
      bn = an ;
      setfromstring(argv[2], rr) ;
      rrstring = argv[2] ;
      ar = rr ;
      br = rr ;
   } else {
      cerr << "Wrong number of arguments" << endl ;
      exit(0) ;
   }
   if (getdouble(ar) <= 1) {
      cout << "Exiting with success because of tiny r" << endl ;
      exit(0) ;
   }
   const doub PI = calculateBigPi<doub>() ;
   for (int i=0; i<an; i++)
      roots.push_back(rootofunity<doub>(i, an, PI)) ;
   auto root1 = roots[1] ;
   san = root1.second ;
   can = root1.first ;
   sbn = root1.second ;
   cbn = root1.first ;
   string seqlin ;
   while (getline(cin, seqlin))
      parseaddseq(seqlin) ;
   doub ss = san * san + can * can ;
   cout << "Values sine " << san << " cosine " << can << " ss " << ss << endl ;
   char finalchar = '=' ;
   ll ord = 0 ;
   ll finalval = -1 ;
   ll maxd = 0 ;
   arar = ar * ar ;
   brbr = br * br ;
   ll hiw = 0 ;
   cout << "Geps is " << geps << endl ;
   cout << "Geps^2 is " << geps*geps << endl ;
   front::front<doub, ent> front(geps) ;
   {
      ord = 0 ;
      doub x = 0, y = 0 ;
      doub close = 100 ;
      doub ox = x ;
      doub oy = y ;
      vector<ent> zero ;
      zero.push_back({{oy,ox}, -1}) ;
      ord = 1 ;
      front.start(zero) ;
      for (int d=1; ; d++) {
       maxd = d ;
       if (verbose > 2 || (ll)front.w.size() > hiw) {
          hiw = front.w.size() ;
          cout << "At depth " << d << " lev " << front.w.size() << " tot " << ord << " hiyes " << hiyes << " lono " << lono << endl ;
       }
       if (front.w.size() == 0)
          break ;
       for (auto &e: front.w) {
         if (abs(getdouble(e.p.first)) < 0.00003 && abs(getdouble(e.p.second)) < 0.00003)
            cout << (d-1) << " " << e.p.first << " " << e.p.second << endl ;
         if (d > 1 && d2(e.p.second, e.p.first) < close) {
            close = d2(e.p.second, e.p.first) ;
            cout << "Depth " << d << " got " << e.p.second << " " << e.p.first << " " << close << endl ;
         }
         pt nv ;
         for (int i=0; i<(int)seqs.size(); i++) {
           if ((e.m ^ i) != 1 && fastseqcheck(e.p, i)) {
              fastseqapply(e.p, i, nv) ;
              front.wn.push_back({nv, i}) ;
           }
         }
       }
       front.pruneback() ;
       ord += front.w.size() ;
       if (ord > maxord) {
          finalchar = '>' ;
          cout << "Trial " << 1 << " " << ox << " " << oy << " " << ord << endl << flush ;
          goto bugout ;
       }
       if (ord > target) {
          cout << "> " << target << " in " << duration() << endl << flush ;
          target += target / 10 ;
       }
      }
bugout:
      if (userandom && verbose > 1)
         cout << "Trial " << 1 << " " << ord << " deep " << deepest << endl << flush ;
      if (ord > deepest) {
         deepest = ord ;
         finalval = ord ;
         if (userandom)
            cout << "Trial " << 1 << " " << ox << " " << oy << " " << ord << endl << flush ;
      }
   }
   if (finalval > maxord)
      finalchar = '>' ;
   if (verbose && finalchar == '=') {
      cout << "Antipodes:" << endl ;
      for (auto p: front.wp)
         cout << " " << p.p.second << " " << p.p.first << endl ;
   }
   cout << "FINAL " << rrstring << " " << finalchar << " " << finalval << " " << 1 << " maxd " << maxd << " hiyes " << hiyes << " lono " << lono << endl ;
   if (finalval > maxord || finalchar == '!')
      exit(101) ;
   else
      exit(0) ;
}
