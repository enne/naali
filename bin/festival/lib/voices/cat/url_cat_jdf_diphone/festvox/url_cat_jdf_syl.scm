;;; This release of the Festival TTS system has been modified from the
;;; the original CSTR/University of Edinburgh release 1.1.1
;;;
;;; Festival is being redistributed with permission of CSTR as part of
;;; the CSLU Toolkit.
;;;
;;; The CSLU Toolkit is
;;; Copyright (C)1997 Center for Spoken Language Understanding,
;;; Oregon Graduate Institute of Science & Technology
;;;
;;; span_mx_sykl.scm
;;;
;;; syllabification & stress prediction for Mexican Spanish
;;; ABC (C) 1997
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Phonesets for use as lset in syllabification rules
;;;
(set! A_Z '(a a1 e e1 e2 i i1 o o1 u u1 b tS dZ d f g h j k l L m n N ny p q r rr s t w
x ) )
(set! CONS '( b tS dZ d f g h j k l L m n N ny p q r rr s t w x ) )
(set! VOWELS '(a a1 e e1 i i1 o o1 u u1 w) )
(set! SET1 '( i u o b tS dZ d f g h j k l L m n N ny p q r rr s t w x ) )
(set! SET2 '( i u o b tS dZ d f g h j k l L m n N ny p q r rr s t w x ) )
(set! SET3 '( i u a b tS dZ d f g h j k l L m n N ny p q r rr s t w x ) )
(set! SET4 '(b tS dZ d f g h j k l L m n N ny p q r rr s t x ) )
(define(next_syl word lset)
"(next_syl word lset) apply syllabification rules for Mexican Spanish to word using
lset as right part of the rule (word must be in reverse order)"
(set! tail word )
(cond
(( equal? word nil) nil)
((not (member (car word) lset)) nil) ;;Create a new syllable
((member (car word) '(i u))
(append (cons (car word) nil) (next_syl (car word) A_Z)))

((member (car word) '(a a1))
(append (cons (car word) nil) (next_syl (car word) SET1)))
((member (car word) '(e e1))
(append (cons (car word) nil) (next_syl (car word) SET2)))
((member (car word) '(o o1))
(append (cons (car word) nil) (next_syl (car word) SET3)))
((member (car word) '(i1 u1))
(append (cons (car word) nil) (next_syl (car word) CONS)))
((member (car word) '(w))
(append (cons (car word) nil) (next_syl (car word) SET4)))
((member (car word) '(l) )
(append (cons (car word) nil) (next_syl (car word) '(b d f g k l p t))))
((member (car word) '(m) )
(append (cons (car word) nil) (next_syl (car word) '(m))))
((member (car word) '(n) )
(append (cons (car word) nil) (next_syl (car word) '(n))))
((member (car word) '(r) )
(append (cons (car word) nil) (next_syl (car word) '(b d f g k p r t))))
((and (member (car word) '(s) ) (equal? syl_last 's))
(append (cons (car word) nil) (next_syl (car word) '(b d k n s))))
((member (car word) '(s) )
(append (cons (car word) nil) (next_syl (car word) '(s))))
((member (car word) '(t) )
(append (cons (car word) nil) (next_syl (car word) '(t))))
((member (car word) '(z) )
(append (cons (car word) nil) (next_syl (car word) '(z))))
((member (car word) '(b tS d f g j k J p q x ) )
(append (cons (car word) nil) (next_syl (car word) '(b tS d f g j k J p q x ))))
(t (append (cons (car word) nil) (next_syl (car word) (cons (car word) nil) )))))
(define(syl word )
"(syl word) returns a list with the syllables in word using syllabification rules for
Mexican Spanish. (word must be in reverse order)."
(set! syl_last (car word))
(if
(equal? word nil) nil
(append (cons (next_syl word (cons (car word ))) nil) (syl tail) )))
(define (solve_CV l )
"(solve_CV) asign the isolate consonant in l to the next syllable)"
(cond

(( equal? l nil) nil)
((not (member (caar l) VOWELS ))
;; (cons (reverse(append (car l) (cadr l))) (solve_CV (cddr l)) ))
(cons (append (car l) (cadr l)) (solve_CV (cddr l)) ))
(t
;; (cons (reverse (car l)) (solve_CV (cdr worl)) ))
(cons (car l) (solve_CV (cdr l)) ))))
(define (is_accented_syl s)
"(is_accented_syl s) returns t if the syllable s has an stressed vowel"
(cond
((equal? s nil) nil)
((member (car s) '(a1 e1 i1 o1 u1)) t )
(t (is_accented_syl (cdr s)))))
(define (is_accented l)
"(is_accented l) returns t if there is an stressed vowel in l"
(cond
((equal? l nil) nil)
((is_accented_syl (car l)) t)
(t (is_accented (cdr l)))))
(define (normal s)
"(normal s) returns a list s without stressed vowels"
(cond
((equal? s nil) nil)
((equal? 'a1 (car s)) (append '(a) (normal (cdr s))))
((equal? (car s) 'e1) (cons 'e (normal (cdr s))))
((equal? (car s) 'i1) (cons 'i (normal (cdr s))))
((equal? (car s) 'o1) (cons 'o (normal (cdr s))))
((equal? (car s) 'u1) (cons 'u (normal (cdr s))))
(t (cons (car s) (normal (cdr s))))))
(define (build_accented l)
"(build_accented l) set the stress mark in tilded syllables"
(cond
((equal? l nil) nil)
((is_accented_syl (car l))
(cons (list (reverse (normal (car l))) 1) (build_accented (cdr l))))
(t
(cons (list (reverse (car l)) 0) (build_accented (cdr l))))))
(define (set_stress l pos cp)
"(set_stress l pos cp) set the stress mark in the pos syllable, counting from the
last position"
(cond
((equal? l nil) nil)
((equal? pos cp)
(cons (list (reverse (car l)) 1) (set_stress (cdr l) pos (+ cp 1))))
(t
(cons (list (reverse (car l)) 0) (set_stress (cdr l) pos (+ cp 1))))))
(define (solve_stress l )
"(solve_stress l ) predict the stress syllable in l using Spanish rules"
(cond
((is_accented l)
(build_accented l))
((member (caar l) '(a e i o u n s)) ;"grave"
(VOWELS_stress l 2 1))
((eq (length l) 1 ) ;"monos'ilaba"
(set_stress l 2 1))
(t
(set_stress l 1 1)) ;"aguda"
))
(define (cat.syllabify.phstress word)
"(cat.syllabify.phstress word) syllabification and stress prediction function
for Mexican Spanish"
(set! drow (reverse word))
(reverse(solve_stress (solve_CV (syl drow )))))
(provide 'url_cat_jdf_syl)
