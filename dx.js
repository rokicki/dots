"use strict";
/*
 *   Display parameters.
 */
var width = 800;
var height = 600;
var marg = 10;
var zoom = 1;
var sc = 0 ;
var xoff = 0 ;
var yoff = 0 ;
var newxoff = 0, newyoff = 0;
var obeyscale = 1 ; // obey hires
var scale = 1 ;
var color_type = true;
var radiusscale = 1 ; // how much do we scale radius parameters by
/*
 *   User interface parameters.
 */
var pttype = 1 ;
/*
 *   Internal rendering parameters.
 */
var x0=0, y0=0 ;
var fullscreen=false;
var deepest = 0;
var renderlim = 300;
var useinfcolor = false;
var pallete = [];
const eps = 1e-9;
const epseps = eps * eps;
var san, can, sbn, cbn, ox, oy, ord, p, drawit, mindist, incirc, plen ;
var ii, wi, state=0, useinfcolor = false ;
var dofull = true;
var stoptime;
var setmax;
var max_zoom = 25;
var timelim = 13; // max work milliseconds per anim call
var largeayes, largebyes, smallano, smallbno, wps, ws, wns, wne ;
var mincirc = 2 ;
// this should always be smaller than mincirc
var circeps = 0.5 ;
var colormul = [154, 230, 214] ; // these should all be less than 239
/*
 *   Model parameters.
 */
var ar, br, an, bn, sep, ax, ay, bx, by, arar, brbr, sepd2 ;
/*
 *   Rendering engine.
 */
function drawcirc(ctx, x, y, r) {
// ctx.beginPath();
// ctx.arc(Math.floor(x), Math.floor(y), r, 0, 2*Math.PI, false);
// ctx.stroke();
}
var rbm, ords, intwidth, intwidth32 ;
function saybitcount() {
   var needed = intwidth * height;
   var bc = 0 ;
   for (var i=0; i<needed; i++) {
      var w = rbm[i];
      if (w == 0) {
      } else if (w == 0xffffffff) {
         bc += 32 ;
      } else {
         w = (w & 0x55555555) + ((w >> 1) & 0x55555555) ;
         w = (w & 0x33333333) + ((w >> 2) & 0x33333333) ;
         w = (w & 0x0f0f0f0f) + ((w >> 4) & 0x0f0f0f0f) ;
         bc += (w + (w >> 8) + (w >> 16) + (w >> 24)) & 0xff ;
      }
   }
   console.log(bc);
}
function shiftRendered(width, height, dx, dy) {
   if (dx === 0 && dy === 0)
      return ;
   var needed = intwidth * height ;
// saybitcount();
   var totshift = dx + dy * intwidth32 ;
   // shift whole map by this
   if ((totshift & 31) == 0) { // word shift
      totshift >>= 5 ;
      if (totshift < 0) { // go forward
         for (var i=0; i<needed+totshift; i++) {
            rbm[i] = rbm[i-totshift] ;
         }
         for (var i=needed+totshift; i<needed; i++) {
            rbm[i] = 0 ;
         }
      } else { // go backwards
         for (var i=needed-1; i>=totshift; i--) {
            rbm[i] = rbm[i-totshift] ;
         }
         for (var i=totshift-1; i>=0; i--) {
            rbm[i] = 0 ;
         }
      }
   } else {
      var totw = totshift >> 5 ;
      var sh = totshift & 31 ;
      var shmask = ~(-1 << sh) ;
      if (totshift < 0) {
         for (var i=0; i<needed+totw; i++) {
            rbm[i] = (rbm[i-totw] << sh) +
                     ((rbm[i-totw-1] >> (32 - sh)) & shmask) ;
         }
         rbm[needed+totw] = (rbm[needed-1] >> (32 - sh)) & shmask ;
         for (var i=needed+totw+1; i<needed; i++) {
            rbm[i] = 0 ;
         }
      } else {
         for (var i=needed-1; i>totw; i--) {
            rbm[i] = (rbm[i-totw] << sh) +
                     ((rbm[i-totw-1] >> (32 - sh)) & shmask) ;
         }
         rbm[totw] = rbm[0] << sh ;
         for (var i=totw-1; i>=0; i--) {
            rbm[i] = 0 ;
         }
      }
   }
   // if we moved right wrt x, clear relevant left columns
   if (dx > 0) {
      if (dx >= 32) {
         for (var y=0; y<height; y++) {
            for (var i=0; 32*(i+1)<=dx; i++) {
               rbm[y*intwidth+i] = 0 ;
            }
         }
      }
      if (dx & 31) { // clear specific columns
         var x = dx >> 5 ;
         var mask = (-1 << (dx & 31)) ;
         for (var y=0; y<height; y++) {
            rbm[y*intwidth+x] &= mask ;
         }
      }
   // if we moved left wrt x, clear relevant right columns
   } else if (dx < 0) {
      dx -= (intwidth32 - width) ;
      if (dx <= -32) {
         for (var y=0; y<height; y++) {
            for (var i=intwidth-1; 32*(i+1)>=intwidth32+dx; i--) {
               rbm[y*intwidth+i] = 0 ;
            }
         }
      }
      if (dx & 31) { // clear specific columns
         var x = (intwidth32 + dx) >> 5 ;
         var mask = ~(-1 << (dx & 31)) ;
         for (var y=0; y<height; y++) {
            rbm[y*intwidth+x] &= mask ;
         }
      }
   }
   // set all the bits off the bitmap, if necessary.
   if ((width & 31) != 0) {
      var rightbits = (-1) << (width & 31) ;
      for (var i=intwidth-1; i<needed; i += intwidth) {
         rbm[i] |= rightbits ;
      }
   }
// saybitcount();
}
function checkRendered(width, height) {
   intwidth = (width + 31) >> 5 ;
   intwidth32 = intwidth * 32 ;
   var needed = intwidth * height ;
   if (rbm === undefined || rbm.length < needed) {
      rbm = new Uint32Array(needed) ;
      ords = new Int32Array(needed*32) ;
   } else {
      console.log("This should not happen here in the command line version");
      for (var i=0; i<needed; i++) {
         rbm[i] = 0 ;
      }
   }
   // set all the bits off the bitmap, if necessary.
   if ((width & 31) != 0) {
      var rightbits = (-1) << (width & 31) ;
      for (var i=intwidth-1; i<needed; i += intwidth) {
         rbm[i] = rightbits ;
      }
   }
}
var gat = 0;
var xcent = 0, calch, calcw, firstpts ;
// typ is undefined or 0 (don't rescale, not a pan), 1 (pan), 2 (rescale)
// 0 and 2 redraw from scratch (clear); 1 copies what was there.
// type=3 means, respect the zoom we got
function doanim(typ) {
    var oax = ax ;
    var oay = ay ;
    if (sc == 0 || typ == 2 || typ == 3) {
       calch = Math.max(ar, br) * 2;
       calcw = ar + br + sep;
       if (typ == 2) { // set zoom to something that works here
          zoom = 
              Math.min(((height-2*marg))/calch, ((width-2 * marg))/calcw)/
              Math.min(((height-2*marg))/4.3, ((width-2 * marg))/6.3);
          xoff = 0;
          yoff = 0;
       } else if (typ != 3) {
          zoom = 1;
          xoff = 0;
          yoff = 0;
       }
       xcent = 0.5 * (sep + br - ar) ;
    }
    sc = Math.min(((height-2*marg)*zoom)/4.3, ((width-2 * marg)*zoom)/6.3);
    if (newxoff != 0 || newyoff != 0) {
       xoff = - newxoff * sc / (scale * zoom);
       yoff = newyoff * sc / (scale * zoom) ;
       newxoff = 0;
       newyoff = 0;
    }
    firstpts = 0 ;
    ay = scale * yoff * zoom + height / 2;
    ax = scale * xoff * zoom + width / 2 - xcent * sc;
    ax = Math.floor(ax) + 0.5;
    ay = Math.floor(ay) + 0.5;
    arar = ar * ar ;
    brbr = br * br ;
    sepd2 = sep * 0.5 ;
    by = ay;
    bx = ax + sep * sc;
    san = Math.sin(Math.PI * 2 / an);
    can = Math.cos(Math.PI * 2 / an);
    sbn = -Math.sin(Math.PI * 2 / bn);
    cbn = Math.cos(Math.PI * 2 / bn);
    if (typ === 1) {
  //   ctx.globalCompositeOperation = "copy";
  //   ctx.drawImage(ctx.canvas, ax-oax, ay-oay, width, height);
  //   ctx.globalCompositeOperation = "source-over";
       shiftRendered(width, height, ax-oax, ay-oay) ;
       //ctx.clearRect(0, 0, canvas.width, canvas.height); // debug
       //checkRendered(width, height) ;
       //ctx.setTransform(otrans);
       //ctx.clearRect(0, 0, canvas.width, canvas.height);
    } else {
 //    ctx.clearRect(0, 0, canvas.width, canvas.height);
       checkRendered(width, height) ;
    }
 // drawcirc(ctx, ax, ay, ar * sc);
 // drawcirc(ctx, bx, by, br * sc);
    deepest = 0;
    renderlim = 300;
    gat = 0;
    state = 0;
    setmax = false;
    return false; // suppress submit
}

//   Radix sort on pairs of doubles using typed arrays.
//   Accepts a work array that has to be as large as the
//   input array; we ping-pong between the two.
//   This version also sorts an auxilliary array in the same way.
function coordinateSort(p, st, e, aux) {
   var n = e - st ;
   var cnts = new Array(8 * 256) ;
   cnts.fill(0) ;
   var bytes = new Uint8Array(p.buffer, st * 8, n * 8) ;
   var wbytes = new Uint8Array(p.buffer, e * 8, n * 8) ;
   for (var i=0; i<bytes.length; i += 16) {
      for (var j=0; j<8; j++) {
         cnts[j*256+bytes[i+j]] += 2 ;
      }
   }
   for (var i=0; i<8; i++) {
      var xor = 0 ;
      if (i == 7) {
         xor = 128 ;
      }
      var s = 0 ;
      for (var j=0; j<256; j++) {
         var ind = i*256 + (j ^ xor) ;
         var s2 = s + cnts[ind] ;
         cnts[ind] = s ;
         s = s2 ;
      }
   }
   var s2 = e ;
   var e2 = s2 + n ;
   var negcnt = cnts[7*256] - 2 ;
   for (var i = 0; i < 8; i++) {
      for (var j = 0; j < n; j += 2) {
         var off = i*256 + bytes[i+j*8] ;
         var dest = cnts[off] ;
         cnts[off] = dest + 2 ;
         p[s2 + dest] = p[st + j] ;
         p[s2 + dest + 1] = p[st + j + 1] ;
         aux[(s2 + dest) >> 1] = aux[(st + j) >> 1] ;
      }
      var t = e ;
      e = e2 ;
      e2 = t ;
      t = st ;
      st = s2 ;
      s2 = t ;
      var t3 = bytes ;
      bytes = wbytes ;
      wbytes = t3 ;
   }
   for (var i = 0; i + i < negcnt; i += 2) {
      var t2 = p[st + i];
      p[st + i] = p[st + negcnt - i] ;
      p[st + negcnt - i] = t2 ;
      t2 = p[st + i + 1];
      p[st + i + 1] = p[st + negcnt - i + 1] ;
      p[st + negcnt - i + 1] = t2 ;
      var t4 = aux[(st + i) >> 1] ;
      aux[(st + i) >> 1] = aux[(st + negcnt - i) >> 1] ;
      aux[(st + negcnt - i) >> 1] = t4 ;
   }
}
function dd(pp, i, j) {
   var dy = pp[i] - pp[j] ;
   var dx = pp[i + 1] - pp[j + 1] ;
   return dx * dx + dy * dy ;
}
function drawrbmcirc(x, y, r) {
   r -= circeps;
   var rr = Math.floor(Math.max(0, r-0.2)); // be conservative
   for (var yy=Math.max(0, y-rr); yy<Math.min(height, y+rr+1); yy++) {
      var xd = Math.floor(Math.sqrt(rr*rr-(yy-y)*(yy-y)));
      var xx = Math.max(0, x-xd);
      var hix = Math.min(width, x+xd+1);
      while (xx < hix) {
         var off = yy * intwidth32 + xx ;
//       if (hix - xx >= 32 && (xx & 31) === 0) {
//          rbm[off>>5] = 0xffffffff ;
//          xx += 32;
//       } else {
            ords[off] = ord;
            rbm[off>>5] |= 1<<(off&31);
            xx++;
//       }
      }
   }
// if (y >= 0 && y < height && x >= 0 && x < width) {
//    var off = y * intwidth32 + x ;
//    rbm[off>>5] |= 1<<(off&31);
// }
// ctx.beginPath();
// ctx.arc(x+0.5, y+0.5, r, 0, 2*Math.PI, false);
// ctx.fill();
// ctx.beginPath();
// ctx.arc(x+0.5, y+0.5, r, 0, 2*Math.PI, false);
// ctx.stroke();
}
//
//  Frontier-based BFS.
//
const MAXCOORDS = 4800000 ;
const jumblelev = 100000 ;
var p = new Float64Array(MAXCOORDS*2) ;
var hint = new Uint8Array(MAXCOORDS) ;
// state 0: pick a new point and initialize
// state 1: chase through points, and accumulate
// state 2: render
function define_color(){
   var cr = ord * colormul[0] % 251;
   var cg = ord * colormul[1] % 241;
   var cb = ord * colormul[2] % 239;
   pallete[ord] = "rgb(" + cr + "," + cg + "," + cb + ")";
}
// find a pixel on the screen that's not rendered yet
function findunsetbit() {
   while (true) {
      var off = gat;
      gat = (gat + 10007) % (intwidth32 * height);
      if (gat == 0) {
         console.log("Done");
         return false;
      }
      if (0 == (rbm[off >> 5] & (1 << (off & 31)))) {
 //      rbm[off >> 5] |= (1<<(off & 31));
         ords[off] = -1;
         y0 = Math.floor(off / intwidth32);
         x0 = off % intwidth32;
         return true;
      }
   }
}
function doworkfull() {
    var x, y;
    while (1) {
        if (state == 0) {
            for (var lop=1; ; lop++) {
                if (pttype == 1) {
                   if (!findunsetbit()) {
                      return ;
                   }
                   if (firstpts > 1) {
                      x = (x0 - ax) / sc ;
                      y = (y0 - ay) / sc ;
                   } else {
                      x = 1e-6 + 2 * firstpts ;
                      y = 0 ;
                      firstpts++ ;
                   }
                   var off = y0 * intwidth32 + x0;
                   if (Math.hypot(x, y) >= ar && Math.hypot(x-sep, y) >= br) {
                      ords[off] = 0;
                      continue ;
                   }
                } else if (pttype == 2) {
                   x = Math.random() * 2 * 3.14159265358979 ;
                   if (Math.random() < 0.5) {
                      y = (ar - 2 * eps) * Math.sin(x) ;
                      x = (ar - 2 * eps) * Math.cos(x) ;
                   } else {
                      y = (br - 2 * eps) * Math.sin(x) ;
                      x = sep + (br - 2 * eps) * Math.cos(x) ;
                   }
                }
                break ;
            }
            x -= sep / 2 ;
            ord = 0;
            drawit = false;
            incirc = false;
            state = 1;
            ii = 0;
            p[0] = y ;
            p[1] = x ;
            hint[0] = 0 ;
            wps = 0 ;
            ws = 0 ;
            wns = 2 ;
            wne = wns ;
            largeayes = 0 ;
            largebyes = 0 ;
            smallano = 1e10 ;
            smallbno = 1e10 ;
        }
        if (state == 1) {
            var sqrt = Math.sqrt;
            for (ii=1; ; ii++) {
               if (wns == ws) {
                  drawit = true;
                  break ;
               }
               if (wns > 2 * renderlim) {
                  break ;
               }
               for (var i=ws; i<wns; i += 2) {
                  y = p[i] ;
                  x = p[i + 1] ;
                  var h = hint[i>>1] ;
                  var xm = x - sepd2 ;
                  var xp = x + sepd2 ;
                  var pp = xp * xp + y * y ;
                  if (wne + 16 > MAXCOORDS) {
  // if a particular level gets this large, we also blow past renderlim
                     break;
                  }
                  if (pp < arar) {
                     incirc = true ;
                     if (h !== 2) {
                        var nx = xp * can + y * san;
                        var ny = y * can - xp * san;
                        hint[wne>>1] = 1 ;
                        p[wne++] = ny ;
                        p[wne++] = nx - sepd2 ;
                     }
                     if (h !== 1) {
                        var nx = xp * can - y * san;
                        var ny = y * can + xp * san;
                        hint[wne>>1] = 2 ;
                        p[wne++] = ny ;
                        p[wne++] = nx - sepd2 ;
                     }
                     largeayes = Math.max(largeayes, pp) ;
                  } else {
                     smallano = Math.min(smallano, pp) ;
                  }
                  pp = xm * xm + y * y ;
                  if (pp < brbr) {
                     incirc = true ;
                     if (h !== 4) {
                        var nx = xm * cbn + y * sbn;
                        var ny = y * cbn - xm * sbn;
                        hint[wne>>1] = 3 ;
                        p[wne++] = ny ;
                        p[wne++] = nx + sepd2 ;
                     }
                     if (h !== 3) {
                        var nx = xm * cbn - y * sbn;
                        var ny = y * cbn + xm * sbn;
                        hint[wne>>1] = 4 ;
                        p[wne++] = ny ;
                        p[wne++] = nx + sepd2 ;
                     }
                     largebyes = Math.max(largebyes, pp) ;
                  } else {
                     smallbno = Math.min(smallbno, pp) ;
                  }
               }
               coordinateSort(p, wns, wne, hint) ;
               var wpi = wps ;
               var wci = ws ;
               wi = wns ;
               for (var i=wns; i<wne; i += 2) {
                  var skipit = 0 ;
                  for (var j=i+2; skipit === 0 && j<wne && p[i] + eps > p[j]; j += 2) {
                     if (dd(p, i, j) < epseps) {
                        skipit = 1 ;
                     }
                  }
                  while (wpi < ws && p[i] > p[wpi] + eps) {
                     wpi += 2 ;
                  }
                  for (var j=wpi; skipit === 0 && j<ws && p[i] + eps > p[j]; j += 2) {
                     if (dd(p, i, j) < epseps) {
                        skipit = 1 ;
                     }
                  }
                  while (wci < wns && p[i] > p[wci] + eps) {
                     wci += 2 ;
                  }
                  for (var j=wci; skipit === 0 && j<wns && p[i] + eps > p[j]; j += 2) {
                     if (dd(p, i, j) < epseps) {
                        skipit = 1 ;
                     }
                  }
                  if (skipit === 0) {
                     hint[wi>>1] = hint[i>>1] ;
                     p[wi++] = p[i] ;
                     p[wi++] = p[i + 1] ;
                  }
               }
               wps = ws ;
               ws = wns ;
               wns = wi ;
               wne = wi ;
               ord = wi / 2 ;
            }
            if (!drawit) {
               if (renderlim < jumblelev) {
                   deepest = renderlim-1; 
                   noticeOrder(">" + deepest);
                   renderlim = Math.floor(renderlim * 1.2);
                   if (renderlim > jumblelev) {
                      renderlim = jumblelev ;
                   }
               } else { 
                  if (! setmax) {
 console.log("Saw jumbling");
                     deepest = renderlim-1;
                     noticeOrder(">" + deepest);
                     setmax = true;
                  } 
                  useinfcolor = true;
                  drawit = true;
               }
            } else {
               useinfcolor = false;
            }
            if (!incirc || !drawit) {
                state = 0;
            } else {
                state = 2;
                ii = 0;
            }
        }
        if (state == 2) {
            if (ord > jumblelev) {
               ord = jumblelev ;
            }
            mindist = sc * Math.min(
                 Math.min(ar-Math.sqrt(largeayes), Math.sqrt(smallano) - ar),
                 Math.min(br-Math.sqrt(largebyes), Math.sqrt(smallbno) - br)) ;
            if (!useinfcolor && ord > deepest) {
                deepest = ord;
                noticeOrder("" + deepest);
            }
            if (!pallete[ord]) {
               define_color();  
            }
            if (useinfcolor) {
//             ctx.fillStyle = "rgb(128,128,128)";
            } else {
//             ctx.fillStyle = pallete[ord];
            }
            var abx = 0.5 * (ax + bx);
            console.log(ord, mindist);
            if (mindist < mincirc) {
                for (; ii<wi; ii += 2) {
                    y = (p[ii] * sc + ay + 0.5) | 0 ;
                    x = (p[ii+1] * sc + abx + 0.5) | 0 ;
                    if (y >= 0 && y < height && x >= 0 && x < width) {
                       var off = y * intwidth32 + x ;
                       rbm[off>>5] |= 1<<(off&31);
                       ords[off] = ord;
//                     ctx.fillRect(x, y, 1, 1);
                    }
                }
            } else {
                for (; ii<wi; ii += 2) {
                    y = (p[ii] * sc + ay + 0.5) | 0 ;
                    x = (p[ii+1] * sc + abx + 0.5) | 0 ;
                    drawrbmcirc(x, y, mindist) ;
                }
            }
            state = 0;
        }
    }
}
function doworkgen() {
    var x, y;
    while (1) {
        if (state == 0) {
            for (var lop=1; ; lop++) {
                if (pttype == 1) {
                   if (!findunsetbit()) {
                      return ;
                   }
                   if (firstpts > 1) {
                      x = (x0 - ax) / sc ;
                      y = (y0 - ay) / sc ;
                   } else {
                      x = 1e-6 + 2 * firstpts ;
                      y = 0 ;
                      firstpts++ ;
                   }
                   var off = y0 * intwidth32 + x0;
                   if (Math.hypot(x, y) >= ar && Math.hypot(x-sep, y) >= br) {
                      ords[off] = 0;
                      continue ;
                   }
                } else if (pttype == 2) {
                   x = Math.random() * 2 * 3.14159265358979 ;
                   if (Math.random() < 0.5) {
                      y = (ar - 2 * eps) * Math.sin(x) ;
                      x = (ar - 2 * eps) * Math.cos(x) ;
                   } else {
                      y = (br - 2 * eps) * Math.sin(x) ;
                      x = sep + (br - 2 * eps) * Math.cos(x) ;
                   }
                } else {
                   x = sep / 2 ;
                   y = 0 ;
                }
                break ;
            }
            ox = x;
            oy = y;
            ord = 0;
            plen = 0;
            drawit = false;
            mindist = 1000;
            incirc = false;
            state = 1;
            ii = 0;
        }
        if (state == 1) {
            var sqrt = Math.sqrt;
            for (; ii<renderlim; ii++) {
                var t = sqrt(x * x + y * y) - ar;
                if (t <= 0) {
                    t = - t;
                    incirc = true;
                    var nx = x * can + y * san;
                    y = y * can - x * san;
                    x = nx;
                }
                if (t < mindist) {
                    mindist = t;
                }
                x -= sep;
                var t = sqrt(x * x + y * y) - br;
                if (t <= 0) {
                    t = - t;
                    incirc = true;
                    var nx = x * cbn + y * sbn;
                    y = y * cbn - x * sbn;
                    x = nx;
                }
                if (t < mindist) {
                    mindist = t;
                }
                var xx = (x + sep) * sc + ax;
                if (xx + mindist >= 0 && xx - mindist <= width) {
                    var yy = y * sc + ay;
                    if (yy + mindist >= 0 && yy - mindist <= height) {
                        p[plen++] = (x + sep) * sc + ax;
                        p[plen++] = y * sc + ay;
                    }
                }
                ord++;
                var dx = x + sep - ox;
                if (dx * dx < epseps) {
                    var dy = y - oy;
                    if (dx * dx + dy * dy < epseps) {
                        drawit = true;
                        break;
                    }
                }
                x += sep;
            }
            mindist *= sc;
            if (!drawit) {
               if (renderlim < jumblelev) {
                   deepest = renderlim;
                   noticeOrder(">" + deepest);
                   renderlim = Math.floor(renderlim * 1.2);
                   if (renderlim > jumblelev) {
                      renderlim = jumblelev ;
                   }
               } else {
                  if (! setmax) {
 console.log("Saw jumbling");
                     deepest = renderlim;
                     noticeOrder(">" + deepest);
                     setmax = true;
                  }
                  useinfcolor = true;
                  drawit = true;
               }
            } else {
               useinfcolor = false;
            }
            if (!incirc || !drawit) {
                state = 0;
            } else {
                state = 2;
                ii = 0;
            }
        }
        if (state == 2) {
            if (ord > jumblelev) {
               ord = jumblelev ;
            }
            if (!useinfcolor && ord > deepest) {
                deepest = ord;
                noticeOrder("" + deepest);
            }
            if (!pallete[ord]) {
               define_color(); 
            }
            if (useinfcolor) {
  //           ctx.fillStyle = "rgb(128,128,128)";
            } else {
  //           ctx.fillStyle = pallete[ord];
            }
            console.log(ord,mindist);
            if (mindist < mincirc) {
                for (; ii<plen; ii += 2) {
                    y = (p[ii+1] + 0.5) | 0 ;
                    x = (p[ii] + 0.5) | 0 ;
                    if (y >= 0 && y < height && x >= 0 && x < width) {
                       var off = y * intwidth32 + x ;
                       rbm[off>>5] |= 1<<(off&31);
                       ords[off] = ord;
 //                    ctx.fillRect(x, y, 1, 1);
                    }
                }
            } else {
                for (; ii<plen; ii += 2) {
                    y = (p[ii+1] + 0.5) | 0 ;
                    x = (p[ii] + 0.5) | 0 ;
                    drawrbmcirc(x, y, mindist);
                }
            }
            state = 0;
        }
    }
}
function dowork() {
   if (dofull) {
      doworkfull() ;
   } else {
      doworkgen() ;
   }
}
// this function definition looks weird, but it's the way it is
// so we create a closure that stores the "timer".
function getUIParams() {
    width = 4096;
    height = 3072;
    scale = 1;
    if (obeyscale) {
       width *= scale;
       height *= scale;
    } else {
       scale = 1;
    }
    // get the following values:
    // r1 r2 n1 n2 (boundary) (gen) (zoom) (x) (y)
    var i = 2;
    while (i < process.argv.length && process.argv[i][0] === '-') {
       i++ ;
       switch (process.argv[i-1][1]) {
case 'z':
          zoom = +process.argv[i++];
          break;
case 'x':
          newxoff = +process.argv[i++];
          break;
case 'y':
          newyoff = +process.argv[i++];
          break;
case 'b':
          pttype = 2;
          break;
case 'g':
          dofull = false;
          break;
default:
          console.log("Bad argument:", process.argv[i-1]);
          process.exit(10);
       }
    }
    if (process.argv.length - i != 4) {
       console.log("Need four more arguments");
       process.exit(10);
    }
    ar = +process.argv[i++];
    br = +process.argv[i++];
    an = +process.argv[i++];
    bn = +process.argv[i++];
    sep = 2;
    console.log("ar",ar,"br",br,"an",an,"bn",bn,"sep",sep,"zoom",zoom,"scale",scale,"width",width,"height",height);
}
function noticeOrder(s) {
    // document.getElementById("order").innerText = s;
}
// critical radius data
var cr = [[], [], [], [],
   [,,,3.4640934572],
   [,,4.2979145550,2.4114701046,2.0906599246,2.1489558561],
   [,,,,1.9999985579,1.8316984635],
   [,,3.2471482670,1.9676444013,1.8454888403,1.7409736639,1.6582671780,
    1.6235740447],
   [,,3.4228059507,1.9930933664,2.4202914488,1.6667405083,1.5980405486,
    1.5398974328,1.7114054144],
   [,,2.8169637349,1.9425030829,1.6967122888,1.6084147831,1.5512093381,
    1.4839500277,1.4404650815,1.4084818900],
   [,,3.0867048074,1.8808137943,1.6286594127,1.8143239064,1.5032085853,
    1.4443135800,1.4009758071,1.3624008011,1.5433538024],
   [,,2.5811631672,1.8280654175,1.5882766276,1.5474083347,1.4478188622,
    1.4163339837,1.3688628865,1.3324652326,1.3086643040,1.2905815795],
   [,,2.7530930142,2.3842463849,1.9467304198,1.5240035523,1.6733398950,
    1.4010443494,1.4209915185,1.3169691431,1.2883159586,1.2650028341,
    1.3765458610],
   [,,2.4271883432,1.8005338781,1.5711923902,1.4874513465,1.4103829453,
    1.3671470435,1.3134490612,1.2863895144,1.2634960649,1.2428145705,
    1.2234560090,1.2135941609],
   [,,2.3931086658,1.8078526855,1.5656579744,1.4533081054,1.3977617362,
    1.4170040941,1.2940698154,1.2717435366,1.2475095111,1.2274713816,
    1.2093180758,1.1979674945,1.1965543280],
   [,,2.3265516430,1.8078427037,1.5107263356,1.5471053456,1.3993046598,
    1.3181487705,1.2758693526,1.2716794353,1.2414391701,1.2138968069,
    1.1981955279,1.1839434013,1.1724108313,1.1632758148],
   [,,2.2969405947,1.7901035513,1.6241822713,1.4512856371,1.3479094740,
    1.3013294731,1.3505233441,1.2411786676,1.2185635168,1.2013065028,
    1.1871537716,1.1729771130,1.1609933012,1.1533971033,1.1484703008],
   [,,2.2550182480,1.7722846629,1.5067141361,1.4343327483,1.3239256204,
    1.2901240763,1.2563284133,1.2316861446,1.2100832476,1.1907533145,
    1.1762334991,1.1638617556,1.1521716383,1.1442100096,1.1351988746,
    1.1275091246],
   [,,2.2430092781,1.9425030247,1.5138832858,1.4097424348,1.4476880954,
    1.2808116232,1.2432677706,1.2357397691,1.1973278194,1.1811716336,
    1.1845402727,1.1552917179,1.1442139983,1.1359332930,1.1287002524,
    1.1206174440,1.1215046909],
   [,,2.2084922741,1.7640787075,1.4766259010,1.3907420431,1.3124324301,
    1.2852960376,1.2302790636,1.2152931984,1.1909610704,1.1724498003,
    1.1597108576,1.1502103712,1.1378147035,1.1291389089,1.1211220515,
    1.1150827178,1.1085737827,1.1042461410],
   [,,2.2011608879,1.7696141774,1.5564558430,1.4203538704,1.3192328270,
    1.2805497477,1.3031180022,1.2077949699,1.1983676640,1.1640245212,
    1.1696351419,1.1437885854,1.1348125933,1.1233476110,1.1167097050,
    1.1097025662,1.1039143943,1.0981958196,1.1005805699],
   [,,2.1720327428,1.7704010190,1.4752980679,1.3922348306,1.3076010456,
    1.2811431442,1.2110008317,1.2047266981,1.1691352721,1.1568384192,
    1.1467749835,1.1368109558,1.1280172194,1.1187903372,1.1109224192,
    1.1045137775,1.1000757923,1.0939877403,1.0894649056,1.0860163669],
   [,,2.1563230911,1.7631141991,1.4821367182,1.3875532343,1.2904084114,
    1.2423288293,1.1999098635,1.1907110189,1.1640104125,1.1618424456,
    1.1396210080,1.1303837878,1.1224870997,1.1122521975,1.1065748920,
    1.1001985870,1.0947116191,1.0897394802,1.0858313451,1.0817170079,
    1.0781615453],
   [,,2.1440216134,1.7540173599,1.4617078504,1.3684379797,1.2736710191,
    1.2390221191,1.2056971064,1.1795557463,1.1608380745,1.1436258414,
    1.1355735380,1.1249863819,1.1174572981,1.1090553285,1.1014295126,
    1.0959003849,1.0907499410,1.0862151158,1.0819120424,1.0781826278,
    1.0753033036,1.0720108061],
   [,,2.1428084565,1.8557265563,1.5151944308,1.3567085750,1.3707366366,
    1.2318065239,1.2478225672,1.1892178395,1.1610846578,1.1376796713,
    1.1683096798,1.1201028098,1.1124418894,1.1078520750,1.1029464303,
    1.0954850517,1.0911022778,1.0825783743,1.0827570361,1.0752759460,
    1.0718182271,1.0689124601,1.0714042647],
   [,,2.1226424173,1.7507956034,1.4571414635,1.3687192883,1.2685954443,
    1.2346113236,1.1965722532,1.1667151508,1.1622852068,1.1322329353,
    1.1236082168,1.1180356279,1.1083269208,1.1040267705,1.0971081559,
    1.0881760988,1.0848948368,1.0797029916,1.0762450378,1.0721955568,
    1.0690911266,1.0661148543,1.0635689719,1.0613212070],
 ];
function main() {
   getUIParams();
   if (newxoff !== 0 || newyoff !== 0) {
      doanim(3);
   } else {
      doanim(2);
   }
   dowork();
   for (var y=0; y<height; y++) {
      for (var x=0; x<width; ) {
         var startx = x;
         var off = y*intwidth32+x;
         var cnt = 1;
         while (x+cnt<width && ords[off] == ords[off+cnt]) {
            cnt++ ;
         }
         console.log(y,x,cnt,ords[off]);
         x += cnt;
      }
   }
}
main();
