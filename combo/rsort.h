#ifndef RSORT_H
#define RSORT_H
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <vector>
#include <iostream>
namespace rsort {
   template<typename T> struct sortdata {
      static constexpr int keylength = sizeof(T) ;
      static constexpr int signbyte = sizeof(T)-1 ;
      static int byteorder(int i) { return i ; }
      static constexpr bool revneg = true ;
   } ;
   template<> struct sortdata<long double> {
      static constexpr int keylength = 10 ;
      static constexpr int signbyte = keylength-1 ;
      static int byteorder(int i) { return i ; }
      static constexpr bool revneg = true ;
   } ;
   using uchar = unsigned char;
   template<typename KEY, typename ENT> class rsort {
public:
      rsort(): wsize(0), w(0) {}
      void genhisto(uchar *a, int n) {
         memset(histo, 0, sizeof(histo)) ;
         for (int i=0; i<n; i++, a += sizeof(ENT))
            for (int j=0; j<sortdata<KEY>::keylength; j++)
               histo[j][a[j]]++ ;
         int xorv = 0 ;
         for (int i=0; i<sortdata<KEY>::keylength; i++) {
            if (i == sortdata<KEY>::signbyte)
               xorv = 128 ;
            else
               xorv = 0 ;
            int s = 0 ;
            for (int j=0; j<256; j++) {
               int s2 = s + histo[i][j ^ xorv] ;
               histo[i][j ^ xorv] = s ;
               s = s2 ;
            }
         }
         negcnt = histo[sortdata<KEY>::signbyte][0] ;
      }
      void sort(ENT *a, int sz) {
         if (sz > wsize) {
            wsize = sz + (sz / 3) + 1000 ;
            if (w)
               free(w) ;
            w = (ENT *)calloc(wsize, sizeof(ENT)) ;
         }
         genhisto((uchar *)a, sz) ;
         for (int ii=0; ii<sortdata<KEY>::keylength; ii++) {
            int i = sortdata<KEY>::byteorder(ii) ;
            ENT *p = a ;
            for (int j=0; j<sz; j++) {
               int k = ((uchar *)p)[i] ;
               w[histo[i][k]++] = *p++ ;
            }
            std::swap(a, w) ;
         }
         if (negcnt && sortdata<KEY>::revneg)
            std::reverse(a, a+negcnt) ;
      }
      void sort(std::vector<ENT> &a) {
         sort(a.data(), a.size()) ;
      }
      int histo[sortdata<KEY>::keylength][256] ;
      int negcnt ;
      int wsize ;
      ENT *w ;
   } ;
} ;
#endif
