#ifndef RSORT_H
#define RSORT_H
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <vector>
namespace rsort {
   using uchar = unsigned char;
   template<typename T> constexpr int keylength = sizeof(T);
   template<> constexpr int keylength<long double> = 10;
   template<typename KEY, typename ENT> class rsort {
public:
      rsort(): wsize(0), w(0) {}
      void genhisto(uchar *a, int n) {
         memset(histo, 0, sizeof(histo)) ;
         for (int i=0; i<n; i++, a += sizeof(ENT))
            for (int j=0; j<keylength<KEY>; j++)
               histo[j][a[j]]++ ;
         int xorv = 0 ;
         for (int i=0; i<keylength<KEY>; i++) {
            if (i + 1 == keylength<KEY>)
               xorv = 128 ;
            int s = 0 ;
            for (int j=0; j<256; j++) {
               int s2 = s + histo[i][j ^ xorv] ;
               histo[i][j ^ xorv] = s ;
               s = s2 ;
            }
         }
         negcnt = histo[keylength<KEY>-1][0] ;
      }
      void sort(ENT *a, int sz) {
         if (sz > wsize) {
            wsize = sz + (sz / 3) + 1000 ;
            if (w)
               free(w) ;
            w = (ENT *)calloc(wsize, sizeof(ENT)) ;
         }
         genhisto((uchar *)a, sz) ;
         for (int i=0; i<keylength<KEY>; i++) {
            ENT *p = a ;
            for (int j=0; j<sz; j++) {
               int k = ((uchar *)p)[i] ;
               w[histo[i][k]++] = *p++ ;
            }
            std::swap(a, w) ;
         }
         if (negcnt)
            std::reverse(a, a+negcnt) ;
      }
      void sort(std::vector<ENT> &a) {
         sort(a.data(), a.size()) ;
      }
      int histo[keylength<KEY>][256] ;
      int negcnt ;
      int wsize ;
      ENT *w ;
   } ;
} ;
#endif
