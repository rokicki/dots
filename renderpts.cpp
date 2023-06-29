#include <iostream>
#include <vector>
using namespace std ;
vector<double> x, y ;
void mapit(vector<double> &vv, double &off, double &mul, double rmin, double rmax) {
   double minv=vv[0], maxv=vv[0] ;
   for (auto v: vv) {
      minv = min(v, minv) ;
      maxv = max(v, maxv) ;
   }
   mul = (rmax - rmin) / (maxv - minv) ;
   off = rmin - mul * minv ;
}
int main(int argc, char *argv[]) {
   double xv, yv ;
   const char *p = "0.20" ;
   if (argc > 1)
      p = argv[1] ;
   while (cin >> xv >> yv) {
      x.push_back(xv) ;
      y.push_back(yv) ;
   }
   double xoff, xmul, yoff, ymul ;
   mapit(x, xoff, xmul, 36, 576) ;
   mapit(y, yoff, ymul, 36, 756) ;
   xmul = ymul = min(xmul, ymul) ;
   cout << 
"%!PS\n"
"/p { " << p << " 0 360 arc closepath fill } def\n" ;
   for (int i=0; i<(int)x.size(); i++) {
      cout << xmul*x[i]+xoff << " " << ymul*y[i]+yoff << " p" << endl ;
   }
   cout << "showpage" << endl ;
}
