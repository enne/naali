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
;;;  A generic voice definition file for a clunits synthesizer           ;;
;;;  Customized for: upc_LANG_ona_clunits.scm                                       ;;
;;;                                                                      ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; ------------------------------------------------------------------- ;;;
;;; Extension to Catalan:                                               ;;;
;;; Antonio Bonafonte                                                   ;;;
;;; Universitat Politecnica de Catalunya                                ;;;
;;; 2007, Barcelona, Spain                                              ;;;
;;; ------------------------------------------------------------------- ;;;

;;; Ensure this version of festival has been compiled with clunits module
(require_module 'clunits)
(require 'clunits) ;; runtime scheme support

;;; Try to find the directory where the voice is, this may be from
;;; .../festival/lib/voices/ or from the current directory
(if (assoc 'upc_LANG_ona_clunits.scm_clunits voice-locations)
    (defvar upc_LANG_ona_clunits.scm::dir 
      (cdr (assoc 'upc_LANG_ona_clunits.scm_clunits voice-locations)))
    (defvar upc_LANG_ona_clunits.scm::dir (string-append (pwd) "/")))

;;; Did we succeed in finding it
(if (not (probe_file (path-append upc_LANG_ona_clunits.scm::dir "festvox/")))
    (begin
     (format stderr "upc_LANG_ona_clunits.scm::clunits: Can't find voice scm files they are not in\n")
     (format stderr "   %s\n" (path-append  upc_LANG_ona_clunits.scm::dir "festvox/"))
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
(set! load-path (cons (path-append upc_LANG_ona_clunits.scm::dir "festvox/") 
		      load-path))

;;; Voice specific parameter are defined in each of the following
;;; files
(require 'upc_LANG_ona_clunits.scm_phoneset)
(require 'upc_LANG_ona_clunits.scm_tokenizer)
(require 'upc_LANG_ona_clunits.scm_tagger)
(require 'upc_LANG_ona_clunits.scm_lexicon)
(require 'upc_LANG_ona_clunits.scm_phrasing)
(require 'upc_LANG_ona_clunits.scm_intonation)
(require 'upc_LANG_ona_clunits.scm_duration)
(require 'upc_LANG_ona_clunits.scm_f0model)
(require 'upc_LANG_ona_clunits.scm_other)
;; ... and others as required

;;;
;;;  Code specific to the clunits waveform synthesis method
;;;

;;; Flag to save multiple loading of db
(defvar upc_LANG_ona_clunits.scm::clunits_loaded nil)
;;; When set to non-nil clunits voices *always* use their closest voice
;;; this is used when generating the prompts
(defvar upc_LANG_ona_clunits.scm::clunits_prompting_stage nil)
;;; Flag to allow new lexical items to be added only once
(defvar upc_LANG_ona_clunits.scm::clunits_added_extra_lex_items nil)

;;;  These are the parameters which are needed at run time
;;;  build time parameters are added to his list in upc_LANG_ona_clunits.scm_build.scm
(set! upc_LANG_ona_clunits.scm::dt_params
      (list
       (list 'db_dir upc_LANG_ona_clunits.scm::dir)
       '(name upc_LANG_ona_clunits.scm)
       '(index_name upc_LANG_ona_clunits.scm)
       '(f0_join_weight 0.0)
       '(join_weights
         (0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 ))
       '(trees_dir "festival/trees/")
       '(catalogue_dir "festival/clunits/")
       '(coeffs_dir "mcep/")
       '(coeffs_ext ".mcep")
       '(clunit_name_feat lisp_upc_LANG_ona_clunits.scm::clunit_name)
       ;;  Run time parameters 
       '(join_method windowed)
       ;; if pitch mark extraction is bad this is better than the above
;       '(join_method smoothedjoin)
;       '(join_method modified_lpc)
       '(continuity_weight 5)
;       '(log_scores 1)  ;; good for high variance joins (not so good for ldom)
       '(optimal_coupling 1)
       '(extend_selections 2)
       '(pm_coeffs_dir "mcep/")
       '(pm_coeffs_ext ".mcep")
       '(sig_dir "wav/")
       '(sig_ext ".wav")
;       '(pm_coeffs_dir "lpc/")
;       '(pm_coeffs_ext ".lpc")
;       '(sig_dir "lpc/")
;       '(sig_ext ".res")
;       '(clunits_debug 1)
))

(define (upc_LANG_ona_clunits.scm::nextvoicing i)
  (let ((nname (item.feat i "n.name")))
    (cond
;     ((string-equal nname "pau")
;      "PAU")
     ((string-equal "+" (item.feat i "n.ph_vc"))
      "V")
     ((string-equal (item.feat i "n.ph_cvox") "+")
      "CVox")
     (t
      "UV"))))

(define (upc_LANG_ona_clunits.scm::clunit_name i)
  "(upc_LANG_ona_clunits.scm::clunit_name i)
Defines the unit name for unit selection for LANG.  The can be modified
changes the basic classification of unit for the clustering.  By default
this we just use the phone name, but you may want to make this, phone
plus previous phone (or something else)."
  (let ((name (item.name i)))
    (cond
     ((and (not upc_LANG_ona_clunits.scm::clunits_loaded)
	   (or (string-equal "h#" name) 
	       (string-equal "1" (item.feat i "ignore"))
	       (and (string-equal "pau" name)
		    (or (string-equal "pau" (item.feat i "p.name"))
			(string-equal "h#" (item.feat i "p.name")))
		    (string-equal "pau" (item.feat i "n.name")))))
      "ignore")
     ;; Comment out this if you want a more interesting unit name
     ((null nil)
      name)

     ;; Comment out the above if you want to use these rules
     ((string-equal "+" (item.feat i "ph_vc"))
      (string-append
       name
       "_"
       (item.feat i "R:SylStructure.parent.stress")
       "_"
       (upc_LANG_ona_clunits.scm::nextvoicing i)))
     ((string-equal name "pau")
      (string-append
       name
       "_"
       (upc_LANG_ona_clunits.scm::nextvoicing i)))
     (t
      (string-append
       name
       "_"
;       (item.feat i "seg_onsetcoda")
;       "_"
       (upc_LANG_ona_clunits.scm::nextvoicing i))))))

(define (upc_LANG_ona_clunits.scm::clunits_load)
  "(upc_LANG_ona_clunits.scm::clunits_load)
Function that actual loads in the databases and selection trees.
SHould only be called once per session."
  (set! dt_params upc_LANG_ona_clunits.scm::dt_params)
  (set! clunits_params upc_LANG_ona_clunits.scm::dt_params)
  (clunits:load_db clunits_params)
  (load (string-append
	 (string-append 
	  upc_LANG_ona_clunits.scm::dir "/"
	  (get_param 'trees_dir dt_params "trees/")
	  (get_param 'index_name dt_params "all")
	  ".tree")))
  (set! upc_LANG_ona_clunits.scm::clunits_clunit_selection_trees clunits_selection_trees)
  (set! upc_LANG_ona_clunits.scm::clunits_loaded t))

(define (upc_LANG_ona_clunits.scm::voice_reset)
  "(upc_LANG_ona_clunits.scm::voice_reset)
Reset global variables back to previous voice."
  (upc_LANG_ona_clunits.scm::reset_phoneset)
  (upc_LANG_ona_clunits.scm::reset_tokenizer)
  (upc_LANG_ona_clunits.scm::reset_tagger)
  (upc_LANG_ona_clunits.scm::reset_lexicon)
  (upc_LANG_ona_clunits.scm::reset_phrasing)
  (upc_LANG_ona_clunits.scm::reset_intonation)
  (upc_LANG_ona_clunits.scm::reset_duration)
  (upc_LANG_ona_clunits.scm::reset_f0model)
  (upc_LANG_ona_clunits.scm::reset_other)

  t
)

;; This function is called to setup a voice.  It will typically
;; simply call functions that are defined in other files in this directory
;; Sometime these simply set up standard Festival modules othertimes
;; these will be specific to this voice.
;; Feel free to add to this list if your language requires it

(define (voice_upc_LANG_ona_clunits.scm_clunits)
  "(voice_upc_LANG_ona_clunits.scm_clunits)
Define voice for LANG."
  ;; *always* required
  (voice_reset)

  ;; Select appropriate phone set
  (upc_LANG_ona_clunits.scm::select_phoneset)

  ;; Select appropriate tokenization
  (upc_LANG_ona_clunits.scm::select_tokenizer)

  ;; For part of speech tagging
  (upc_LANG_ona_clunits.scm::select_tagger)

  (upc_LANG_ona_clunits.scm::select_lexicon)
  ;; For clunits selection you probably don't want vowel reduction
  ;; the unit selection will do that
  (if (string-equal "americanenglish" (Param.get 'Language))
      (set! postlex_vowel_reduce_cart_tree nil))

  (upc_LANG_ona_clunits.scm::select_phrasing)

  (upc_LANG_ona_clunits.scm::select_intonation)

  (upc_LANG_ona_clunits.scm::select_duration)

  (upc_LANG_ona_clunits.scm::select_f0model)

  ;; Waveform synthesis model: clunits

  ;; Load in the clunits databases (or select it if its already loaded)
  (if (not upc_LANG_ona_clunits.scm::clunits_prompting_stage)
      (begin
	(if (not upc_LANG_ona_clunits.scm::clunits_loaded)
	    (upc_LANG_ona_clunits.scm::clunits_load)
	    (clunits:select 'upc_LANG_ona_clunits.scm))
	(set! clunits_selection_trees 
	      upc_LANG_ona_clunits.scm::clunits_clunit_selection_trees)
	(Parameter.set 'Synth_Method 'Cluster)))

  ;; This is where you can modify power (and sampling rate) if desired
  (set! after_synth_hooks nil)
;  (set! after_synth_hooks
;      (list
;        (lambda (utt)
;          (utt.wave.rescale utt 2.1))))

  (set! current_voice_reset upc_LANG_ona_clunits.scm::voice_reset)
  (set! current-voice 'upc_LANG_ona_clunits.scm_clunits)
)

(define (is_pau i)
  (if (phone_is_silence (item.name i))
      "1"
      "0"))


(proclaim_voice
 'upc_ca_ona_clunits.scm_clunits
 '((language catalan)
   (gender female)
   (dialect central)
   (description
    "Catalan speaker ona_clunits.scm from the Festcat project.")))


(provide 'upc_LANG_ona_clunits.scm_clunits)

