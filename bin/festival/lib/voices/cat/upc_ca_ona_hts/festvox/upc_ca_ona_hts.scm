;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;;                     Carnegie Mellon University                      ;;;
;;;                  and Alan W Black and Kevin Lenzo                   ;;;
;;;                      Copyright (c) 1998-2005                        ;;;
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
;;;                                                                      ;;
;;;  A generic voice definition file for a hts synthesizer               ;;
;;;  Customized for: upc_ca_ona                                         ;;
;;;                                                                      ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; ------------------------------------------------------------------- ;;;
;;; Extension to Catalan:                                               ;;;
;;; Antonio Bonafonte                                                   ;;;
;;; Universitat Politecnica de Catalunya                                ;;;
;;; 2007, Barcelona, Spain                                              ;;;
;;; ------------------------------------------------------------------- ;;;


;;; Ensure this version of festival has been compiled with clunits module
;(require_module 'clunits)
;(require 'clunits) ;; runtime scheme support

;;; Try to find the directory where the voice is, this may be from
;;; .../festival/lib/voices/ or from the current directory


(if (assoc 'upc_ca_ona_hts voice-locations)
    (defvar upc_ca_ona::hts_dir 
      (cdr (assoc 'upc_ca_ona_hts voice-locations)))
    (defvar upc_ca_ona::hts_dir (string-append (pwd) "/")))

(defvar upc_ca_ona::clunits_dir upc_ca_ona::hts_dir)
(defvar upc_ca_ona::dir upc_ca_ona::hts_dir)

;;; Did we succeed in finding it
(if (not (probe_file (path-append upc_ca_ona::dir "festvox/")))
    (begin
     (format stderr "upc_ca_ona::clunits: Can't find voice scm files they are not in\n")
     (format stderr "   %s\n" (path-append  upc_ca_ona::dir "festvox/"))
     (format stderr "   Either the voice isn't linked in Festival library\n")
     (format stderr "   or you are starting festival in the wrong directory\n")
     (error)))

;;;  Add the directory that contains catalan stuff (normalization, tagger, etc.) to load-path
(set! catalan-path (path-append libdir "upc_catalan/"))
(if (not (member_string catalan-path load-path))
                      (set! load-path (cons catalan-path load-path)))

;;; General issues, as intro-catalan
(require 'upc_catalan)


;;;  Add the directory contains general voice stuff to load-path
(set! load-path (cons (path-append upc_ca_ona::dir "festvox/") 
		      load-path))

(set! hts_data_dir (path-append upc_ca_ona::hts_dir "hts/"))


(set! hts_feats_list
      (load (path-append hts_data_dir "label.feats") t))

(set! upc_ca_ona_hts::hts_feats_list hts_feats_list)

(require 'hts)
(require_module 'hts_engine)

;;; Voice specific parameter are defined in each of the following
;;; files
(require 'upc_ca_ona_phoneset)
(require 'upc_ca_ona_tokenizer)
(require 'upc_ca_ona_tagger)
(require 'upc_ca_ona_lexicon)
(require 'upc_ca_ona_phrasing)
(require 'upc_ca_ona_intonation)
(require 'upc_ca_ona_duration)
(require 'upc_ca_ona_f0model)
(require 'upc_ca_ona_other)
;; ... and others as required

(define (upc_ca_ona::voice_reset)
  "(upc_ca_ona::voice_reset)
Reset global variables back to previous voice."
  (upc_ca_ona::reset_phoneset)
  (upc_ca_ona::reset_tokenizer)
  (upc_ca_ona::reset_tagger)
  (upc_ca_ona::reset_lexicon)
  (upc_ca_ona::reset_phrasing)
  (upc_ca_ona::reset_intonation)
  (upc_ca_ona::reset_duration)
  (upc_ca_ona::reset_f0model)
  (upc_ca_ona::reset_other)

  t
)

(set! upc_ca_ona_hts::hts_engine_params
      (list
       (list "-dm1" (path-append hts_data_dir "mcep_dyn.win"))
       (list "-dm2" (path-append hts_data_dir "mcep_acc.win"))
       (list "-df1" (path-append hts_data_dir "lf0_dyn.win"))
       (list "-df2" (path-append hts_data_dir "lf0_acc.win"))
       (list "-td" (path-append hts_data_dir "tree-dur.inf"))
       (list "-tm" (path-append hts_data_dir "tree-mcp.inf"))
       (list "-tf" (path-append hts_data_dir "tree-lf0.inf"))
       (list "-md" (path-append hts_data_dir "dur.pdf"))
       (list "-mm" (path-append hts_data_dir "mcp.pdf"))
       (list "-mf" (path-append hts_data_dir "lf0.pdf"))
       '("-a"   0.420000)                 
       '("-r"   0.0000000)   ; duration_stretch
       '("-fs"  1.000000)             
       '("-fm"  0.000000)            
       '("-u"   0.500000)                 
       '("-l"   0.000000) 
       '("-b" 0.4)
       ))

(define (voice_upc_ca_ona_hts)
  "(voice_upc_ca_ona_clunits)
Define voice for catalan."
  ;; *always* required
  (voice_reset)

  ;; Select appropriate phone set
  (upc_ca_ona::select_phoneset)

  ;; Select appropriate tokenization
  (upc_ca_ona::select_tokenizer)

  ;; For part of speech tagging
  (upc_ca_ona::select_tagger)

  (upc_ca_ona::select_lexicon)

  (upc_ca_ona::select_phrasing)

  (upc_ca_ona::select_intonation)

  (upc_ca_ona::select_duration)

  (upc_ca_ona::select_f0model)

  ;; Waveform synthesis model: hts
  (set! hts_engine_params upc_ca_ona_hts::hts_engine_params)
  (Parameter.set 'Synth_Method 'HTS)

  ;; This is where you can modify power (and sampling rate) if desired
  (set! after_synth_hooks nil)
;  (set! after_synth_hooks
;      (list
;        (lambda (utt)
;          (utt.wave.rescale utt 2.1))))

  (set! current_voice_reset upc_ca_ona::voice_reset)

  (set! current-voice 'upc_ca_ona_hts)
)

(proclaim_voice
 'upc_ca_ona_hts
 '((language catalan)
   (gender female)
   (dialect central)
   (description
    "Catalan speaker ona from the Festcat project.")))

(provide 'upc_ca_ona_hts)

