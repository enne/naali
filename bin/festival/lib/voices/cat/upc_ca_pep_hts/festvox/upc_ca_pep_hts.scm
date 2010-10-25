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
;;;  Customized for: upc_ca_pep                                         ;;
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


(if (assoc 'upc_ca_pep_hts voice-locations)
    (defvar upc_ca_pep::hts_dir 
      (cdr (assoc 'upc_ca_pep_hts voice-locations)))
    (defvar upc_ca_pep::hts_dir (string-append (pwd) "/")))

(defvar upc_ca_pep::clunits_dir upc_ca_pep::hts_dir)
(defvar upc_ca_pep::dir upc_ca_pep::hts_dir)

;;; Did we succeed in finding it
(if (not (probe_file (path-append upc_ca_pep::dir "festvox/")))
    (begin
     (format stderr "upc_ca_pep::clunits: Can't find voice scm files they are not in\n")
     (format stderr "   %s\n" (path-append  upc_ca_pep::dir "festvox/"))
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
(set! load-path (cons (path-append upc_ca_pep::dir "festvox/") 
		      load-path))

(set! hts_data_dir (path-append upc_ca_pep::hts_dir "hts/"))


(require 'hts)
(require_module 'hts_engine)



;;; Voice specific parameter are defined in each of the following
;;; files
(require 'upc_ca_pep_phoneset)
(require 'upc_ca_pep_tokenizer)
(require 'upc_ca_pep_tagger)
(require 'upc_ca_pep_lexicon)
(require 'upc_ca_pep_phrasing)
(require 'upc_ca_pep_intonation)
(require 'upc_ca_pep_duration)
(require 'upc_ca_pep_f0model)
(require 'upc_ca_pep_other)
;; ... and others as required

;;;
;;;  Code specific to the hts waveform synthesis method
;;;

(set! upc_ca_pep_hts::hts_feats_list
      (load (path-append hts_data_dir "label.feats") t))

(set! upc_ca_pep_hts::hts_engine_params
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
       '("-a" 0.420000)
       '("-r" 0.0000000)   ; duration_stretch
       '("-fs" 1.000000)
       '("-fm" 0.000000)
       '("-u" 0.500000)
       '("-l" 0.000000)
       '("-b" 0.4)
       ))




;;; Flag to save multiple loading of db
(defvar upc_ca_pep::clunits_loaded nil)
;;; When set to non-nil clunits voices *always* use their closest voice
;;; this is used when generating the prompts
(defvar upc_ca_pep::clunits_prompting_stage nil)
;;; Flag to allow new lexical items to be added only once
(defvar upc_ca_pep::clunits_added_extra_lex_items nil)

;;; You may wish to change this (only used in building the voice)
(set! upc_ca_pep::closest_voice 'voice_kal_diphone_ca)

(set! ca_phone_maps
      '(
;        (M_t t)
;        (M_dH d)
        ))

(define (voice_kal_diphone_ca_phone_maps utt)
  (mapcar
   (lambda (s) 
     (let ((m (assoc_string (item.name s) ca_phone_maps)))
       (if m
           (item.set_feat s "us_diphone" (cadr m))
           (item.set_feat s "us_diphone"))))
   (utt.relation.items utt 'Segment))
  utt)

(define (voice_kal_diphone_ca)
  (voice_kal_diphone)
  (set! UniSyn_module_hooks (list voice_kal_diphone_ca_phone_maps ))

  'kal_diphone_ca
)

;;;  These are the parameters which are needed at run time
;;;  build time parameters are added to his list in upc_ca_pep_build.scm
(set! upc_ca_pep::dt_params
      (list
       (list 'db_dir upc_ca_pep::dir)
       '(name upc_ca_pep)
       '(index_name upc_ca_pep)
       '(f0_join_weight 0.0)
       '(join_weights
         (0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 ))
       '(trees_dir "festival/trees/")
       '(catalogue_dir "festival/clunits/")
       '(coeffs_dir "mcep/")
       '(coeffs_ext ".mcep")
       '(clunit_name_feat lisp_upc_ca_pep::clunit_name)
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

(define (upc_ca_pep::nextvoicing i)
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

(define (upc_ca_pep::clunit_name i)
  "(upc_ca_pep::clunit_name i)
Defines the unit name for unit selection for ca.  The can be modified
changes the basic classification of unit for the clustering.  By default
this we just use the phone name, but you may want to make this, phone
plus previous phone (or something else)."
  (let ((name (item.name i)))
    (cond
     ((and (not upc_ca_pep::clunits_loaded)
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
       (upc_ca_pep::nextvoicing i)))
     ((string-equal name "pau")
      (string-append
       name
       "_"
       (upc_ca_pep::nextvoicing i)))
     (t
      (string-append
       name
       "_"
;       (item.feat i "seg_onsetcoda")
;       "_"
       (upc_ca_pep::nextvoicing i))))))

(define (upc_ca_pep::clunits_load)
  "(upc_ca_pep::clunits_load)
Function that actual loads in the databases and selection trees.
SHould only be called once per session."
  (set! dt_params upc_ca_pep::dt_params)
  (set! clunits_params upc_ca_pep::dt_params)
  (clunits:load_db clunits_params)
  (load (string-append
	 (string-append 
	  upc_ca_pep::dir "/"
	  (get_param 'trees_dir dt_params "trees/")
	  (get_param 'index_name dt_params "all")
	  ".tree")))
  (set! upc_ca_pep::clunits_clunit_selection_trees clunits_selection_trees)
  (set! upc_ca_pep::clunits_loaded t))

(define (upc_ca_pep::voice_reset)
  "(upc_ca_pep::voice_reset)
Reset global variables back to previous voice."
  (upc_ca_pep::reset_phoneset)
  (upc_ca_pep::reset_tokenizer)
  (upc_ca_pep::reset_tagger)
  (upc_ca_pep::reset_lexicon)
  (upc_ca_pep::reset_phrasing)
  (upc_ca_pep::reset_intonation)
  (upc_ca_pep::reset_duration)
  (upc_ca_pep::reset_f0model)
  (upc_ca_pep::reset_other)

  t
)

;; This function is called to setup a voice.  It will typically
;; simply call functions that are defined in other files in this directory
;; Sometime these simply set up standard Festival modules othertimes
;; these will be specific to this voice.
;; Feel free to add to this list if your language requires it







(define (voice_upc_ca_pep_hts)
  "(voice_upc_ca_pep_clunits)
Define voice for ca."
  ;; *always* required
  (voice_reset)

  ;; Select appropriate phone set
  (upc_ca_pep::select_phoneset)

  ;; Select appropriate tokenization
  (upc_ca_pep::select_tokenizer)

  ;; For part of speech tagging
  (upc_ca_pep::select_tagger)

  (upc_ca_pep::select_lexicon)
  ;; For clunits selection you probably don't want vowel reduction
  ;; the unit selection will do that
  (if (string-equal "americanenglish" (Param.get 'Language))
      (set! postlex_vowel_reduce_cart_tree nil))

  (upc_ca_pep::select_phrasing)

  (upc_ca_pep::select_intonation)

  (upc_ca_pep::select_duration)

  (upc_ca_pep::select_f0model)

  ;; Waveform synthesis model: hts
  (set! hts_engine_params upc_ca_pep_hts::hts_engine_params)
  (set! hts_feats_list upc_ca_pep_hts::hts_feats_list)
  (Parameter.set 'Synth_Method 'HTS)

  ;; This is where you can modify power (and sampling rate) if desired
  (set! after_synth_hooks nil)
;  (set! after_synth_hooks
;      (list
;        (lambda (utt)
;          (utt.wave.rescale utt 2.1))))

  (set! current_voice_reset upc_ca_pep::voice_reset)

  (set! current-voice 'upc_ca_pep_clunits)
)

(define (is_pau i)
  (if (phone_is_silence (item.name i))
      "1"
      "0"))

(provide 'upc_ca_pep_hts)

