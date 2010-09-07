;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;;                     Carnegie Mellon University                      ;;;
;;;                  and Alan W Black and Kevin Lenzo                   ;;;
;;;                      Copyright (c) 1998-2000                        ;;;
;;;                        All Rights Reserved.                         ;;;
;;;                                                                     ;;;
;;; Permission is hereby granted, free of charge, to use and distribute ;;;
;;; this software and its documentation without restriction, including  ;;;
;;; without limitation the rights to use, copy, modify, merge, publish, ;;;
;;; distribute, sublicense, and/or sell copies of this work, and to     ;;;
;;; permit persons to whom this work is furnished to do so, subject to  ;;;
;;; the following conditions:                                           ;;;
;;;  1. The code must retain the above copyright notice, this list of   ;;;
;;;     conditions and the following disclaimer.                        ;;;
;;;  2. Any modifications must be clearly marked as such.               ;;;
;;;  3. Original authors' names are not deleted.                        ;;;
;;;  4. The authors' names are not used to endorse or promote products  ;;;
;;;     derived from this software without specific prior written       ;;;
;;;     permission.                                                     ;;;
;;;                                                                     ;;;
;;; CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK        ;;;
;;; DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING     ;;;
;;; ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT  ;;;
;;; SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE     ;;;
;;; FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   ;;;
;;; WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN  ;;;
;;; AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,         ;;;
;;; ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF      ;;;
;;; THIS SOFTWARE.                                                      ;;;
;;;                                                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  Language and voice specific durations
;;;

(set! url_cat_jdf::zdur_tree 
 '
   ((R:SylStructure.parent.R:Syllable.p.syl_break > 1 ) ;; clause initial
    ((1.5))
    ((R:SylStructure.parent.syl_break > 1)   ;; clause final
     ((1.5))
     ((1.0)))))

(set! url_cat_jdf::phone_data
'(
  ;;; PHONE DATA  
  ;; name zero mean in seconds e.g.
 (# 0.0 0.500)
 (a 0.0 0.090)
 (@ 0.0 0.090)
 (e 0.0 0.080)
 (E 0.0 0.090)
 (i 0.0 0.080)
 (j 0.0 0.060)
 (o 0.0 0.088)
 (O 0.0 0.085)
 (u 0.0 0.064)
 (w 0.0 0.063)
 (b 0.0 0.076)
 (bl 0.0 0.135)
 (br 0.0 0.153)
 (Bl 0.0 0.152)
 (Br 0.0 0.175)
 (B 0.0 0.095)
 (d 0.0 0.069)
 (dr 0.0 0.141)
 (D 0.0 0.093)
 (Dr 0.0 0.171)
 (f 0.0 0.105)
 (fl 0.0 0.163)
 (fr 0.0 0.183)
 (g 0.0 0.061)
 (gl 0.0 0.120)
 (gr 0.0 0.140)
 (gw 0.0 0.124)
 (G 0.0 0.079)
 (Gl 0.0 0.138)
 (Gr 0.0 0.158)
 (Gw 0.0 0.143)
 (k 0.0 0.100)
 (kl 0.0 0.158)
 (kr 0.0 0.178)
 (kw 0.0 0.163)
 (l 0.0 0.058)
 (L 0.0 0.120)
 (m 0.0 0.073)
 (M 0.0 0.085)
 (n 0.0 0.065)
 (N 0.0 0.086)
 (J 0.0 0.110)
 (p 0.0 0.092)
 (pr 0.0 0.160)
 (pl 0.0 0.150)
 (r 0.0 0.078)
 (R 0.0 0.151)
 (Rp 0.0 0.243)
 (Rb 0.0 0.226)
 (Rd 0.0 0.220)
 (Rt 0.0 0.250)
 (Rk 0.0 0.251)
 (Rg 0.0 0.216)
 (Rm 0.0 0.214)
 (Rn 0.0 0.216)
 (RJ 0.0 0.261)
 (Rf 0.0 0.156)
 (RT 0.0 0.261)
 (Rs 0.0 0.241)
 (Rz 0.0 0.226)
 (RS 0.0 0.256)
 (RZ 0.0 0.266)
 (Rx 0.0 0.250)
 (Rl 0.0 0.208)
 (RL 0.0 0.270)
 (RR 0.0 0.270)
 (s 0.0 0.095)
 (S 0.0 0.105)
 (t 0.0 0.098)
 (tr 0.0 0.175)
 (T 0.0 0.110)
 (z 0.0 0.076)
 (Z 0.0 0.115)
 (x 0.0 0.100)
  ; all other phones on url_cat phoneset
))

(provide 'url_cat_jdf_dur)
