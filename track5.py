import cmath, math
n = 5
pr = [cmath.exp(complex(0, -2*cmath.pi*i/n)) for i in range(n)]
def a(v, k): return (v+1)*pr[k]-1
def b(v, k): return (v-1)*pr[k]+1
movemap = {"a" :lambda v:a(v,1), "a'":lambda v:a(v,n-1),
           "b" :lambda v:b(v,1), "b'":lambda v:b(v,n-1)}
def domove(v, m): return movemap[m](v)
def minr(v): return max(abs(v+1),abs(v-1))
def doseqcheck(v, seq):
   m = minr(v)
   for x in seq:
      v = domove(v, x)
      m = max(m, minr(v))
   return v, m
def sign(v):
   if v < 0: return -1
   if v > 0: return 1
   return 0
seqs = ["a' a' b' a' b'", "a b a b b", "a b a b' a' b'"]
p = complex(0, 0)
curr = 0
E = pr[1] - pr[2]
F = 1 - pr[1] + pr[2] - pr[3]
G = 2 * F - E
while True:
   (bestr, besti, bestp) = (1000, -1, complex(0, 0))
   region = -1
   if p.real < -F.real: region = 1
   elif p.real < -G.real: region = 2
   else: region = 3
   for i in range(len(seqs)):
      (p2, testr) = doseqcheck(p, seqs[i].split())
      if testr < bestr:
         (bestr, besti, bestp) = (testr, i, p2)
   if bestr > curr:
      curr = bestr
      print("Required radius is now", curr)
   t = abs(bestp - p) * sign((bestp - p).real)
   print("At",p,"region",region,"choose",seqs[besti],"trans",t)
   p = bestp
