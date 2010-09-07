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
;;; Phoneset definition 
;;;

(defPhoneSet
  url_cat
  ;;;  Phone Features
  (;; vowel or consonant
   (vc + -)  
   ;; vowel length: short long dipthong schwa
   (vlng s l d a 0)
   ;; vowel height: high mid low
   (vheight 1 2 3 -)
   ;; vowel frontness: front mid back
   (vfront 1 2 3 -)
   ;; lip rounding
   (vrnd + - 0)
   ;; consonant type: stop fricative affricative nasal liquid
   (ctype s f a n l 0)
   ;; place of articulation: labial alveolar palatal labio-dental
   ;;                         dental velar
   (cplace l a p b d v 0)
   ;; consonant voicing
   (cvox + - 0)
   )
  (
   (#  - 0 - - - 0 0 -)  ;; silence ... (if you change also change # below too)

   ;; insert the phones here, see examples in 
   ;; festival/lib/*_phones.scm
   ;;A partir d'aquí venen les modificacions per al català
   ;;Versió 1.0
   (a  + s 2 2 - 0 0 +)
   (@  + s 2 2 - 0 0 +)
   (e  + s 2 2 - 0 0 +)
   (E  + s 2 2 - 0 0 +)
   (i  + s 1 1 - 0 0 +)
   (j  + s 1 1 - 0 0 +)
   (o  + s 2 2 - 0 0 +)
   (O  + s 2 2 - 0 0 +)
   (u  + s 1 1 - 0 0 +)
   (w  + s 1 1 - 0 0 +)
   (b  - 0 - - + s l +)
   (bl - 0 - - + l d +)
   (br - 0 - - + l p +)
   (B  - 0 - - + s l +)
   (Bl - 0 - - + l d +)
   (Br - 0 - - + l l +)
   (S  - 0 - - + s l -)
   (Z  - 0 - - + s p -)
   (d  - 0 - - + s p -)
   (dr - 0 - - + l l +)
   (D  - 0 - - + s p -)
   (Dr - 0 - - + l l +)
   (f  - 0 - - + f b -)
   (fl - 0 - - + l d +)
   (fr - 0 - - + l p +)
   (g  - 0 - - + f p -)
   (gl - 0 - - + l d +)
   (gr - 0 - - + l p +)
   (gw - 0 - - + l p +)
   (G  - 0 - - + f p -)
   (Gl - 0 - - + l d +)
   (Gr - 0 - - + l p +)
   (Gw - 0 - - + l p +)
   (k  - 0 - - + f p -)
   (kl - 0 - - + l d +)
   (kr - 0 - - + l p +)
   (kw - 0 - - + l p +)
   (l  - 0 - - + l d +)
   (L  - 0 - - + l d +)
   (m  - 0 - - + n l +)
   (M  - 0 - - + n l +)
   (n  - 0 - - + n l +)
   (N  - 0 - - + n l +)
   (J  - 0 - - + n l +)
   (p  - 0 - - + s l -)
   (pr - 0 - - + l p -)
   (pl - 0 - - + l d +)
   (r  - 0 - - + l p +)
   (R  - 0 - - + l p +)
   (Rp - 0 - - + l p -)
   (Rb - 0 - - + s l +)
   (Rd - 0 - - + s p +)
   (Rt - 0 - - + s a -)
   (Rk - 0 - - + f p -)
   (Rg - 0 - - + f p -)
   (Rm - 0 - - + n l +)
   (Rn - 0 - - + n l +)
   (RJ - 0 - - + n l -)
   (Rf - 0 - - + f b -)
   (RT - 0 - - + s a -)
   (Rs - 0 - - + f a +)
   (Rz - 0 - - + f a +)
   (RS - 0 - - + s l -)
   (RZ - 0 - - + s p -)
   (Rx - 0 - - + f a -)
   (Rl - 0 - - + l d +)
   (RL - 0 - - + l d +)
   (RR - 0 - - + l p +)
   (s  - 0 - - + f a +)
   (z  - 0 - - + f a +)
   (t  - 0 - - + s a -)
   (tr - 0 - - + l p -)
   (T  - 0 - - + s a -)
   (x  - 0 - - + f a -)
  )
)
(PhoneSet.silences '(#))

(provide 'url_cat_phones)
