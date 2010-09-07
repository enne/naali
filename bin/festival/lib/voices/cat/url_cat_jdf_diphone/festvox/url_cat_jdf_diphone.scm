;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                       ;;
;;;                            [SOMEBODY]                                 ;;
;;;                         Copyright (c) 2000                            ;;
;;;                        All Rights Reserved.                           ;;
;;;                                                                       ;;
;;;  Distribution policy                                                  ;;
;;;     [CHOOSE ONE OF]                                                   ;;
;;;     Free for any use                                                  ;;
;;;     Free for non commercial use                                       ;;
;;;     something else                                                    ;;
;;;                                                                       ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  An example diphone voice
;;;
;;;  Authors: [The people who did the work]
;;;

;;; Try to find out where we are
(if (assoc 'url_cat_jdf_diphone voice-locations)
    (defvar url_cat_jdf_dir 
      (cdr (assoc 'url_cat_jdf_diphone voice-locations)))
    ;;; Not installed in Festival yet so assume running in place
    (defvar url_cat_jdf_dir (pwd)))

(if (not (probe_file (path-append url_cat_jdf_dir "festvox/")))
    (begin
     (format stderr "url_cat_jdf: Can't find voice scm files they are not in\n")
     (format stderr "   %s\n" (path-append url_cat_jdf_dir "festvox/"))
     (format stderr "   Either the voice isn't linked into Festival\n")
     (format stderr "   or you are starting festival in the wrong directory\n")
     (error)))

;;;  Add the directory contains general voice stuff to load-path
(set! load-path (cons (path-append url_cat_jdf_dir "festvox/") load-path))

;;; other files we need
(require 'url_cat_phones)
(require 'url_cat_lex)
(require 'url_cat_token)
(require 'url_cat_jdf_int)
(require 'url_cat_jdf_dur)

;;;  Ensure we have a festival with the right diphone support compiled in
(require_module 'UniSyn)

(set! url_cat_jdf_lpc_sep 
      (list
       '(name "url_cat_jdf_lpc_sep")
       (list 'index_file (path-append url_cat_jdf_dir "dic/jdfdiph.est"))
       '(grouped "false")
       (list 'coef_dir (path-append url_cat_jdf_dir "lpc"))
       (list 'sig_dir  (path-append url_cat_jdf_dir "lpc"))
       '(coef_ext ".lpc")
       '(sig_ext ".res")
       (list 'default_diphone 
	     (string-append
	      (car (cadr (car (PhoneSet.description '(silences)))))
	      "-"
	      (car (cadr (car (PhoneSet.description '(silences)))))))))

(set! url_cat_jdf_lpc_group 
      (list
       '(name "jdf_lpc_group")
       (list 'index_file 
	     (path-append url_cat_jdf_dir "group/jdflpc.group"))
       '(grouped "true")
       (list 'default_diphone 
	     (string-append
	      (car (cadr (car (PhoneSet.description '(silences)))))
	      "-"
	      (car (cadr (car (PhoneSet.description '(silences)))))))))

;; Go ahead and set up the diphone db
;;(set! url_cat_jdf_db_name (us_diphone_init url_cat_jdf_lpc_sep))
;; Once you've built the group file you can comment out the above and
;; uncomment the following.
(set! url_cat_jdf_db_name (us_diphone_init url_cat_jdf_lpc_group))

(define (url_cat_jdf_diphone_fix utt)
"(url_cat_jdf_diphone_fix UTT)
Map phones to phonological variants if the diphone database supports
them."
  (mapcar
   (lambda (s)
     (let ((name (item.name s)))
       ;; Check and do something maybe 
       ))
   (utt.relation.items utt 'Segment))
  utt)

(define (url_cat_jdf_voice_reset)
  "(url_cat_jdf_voice_reset)
Reset global variables back to previous voice."
  ;; whatever
)

;;;  Full voice definition 
(define (voice_url_cat_jdf_diphone)
"(voice_url_cat_jdf_diphone)
Set speaker to jdf in cat from url."
  (voice_reset)
  (Parameter.set 'Language 'url_cat)
  ;; Phone set
  (Parameter.set 'PhoneSet 'url_cat)
  (PhoneSet.select 'url_cat)

  ;; token expansion (numbers, symbols, compounds etc)
  (set! token_to_words url_cat_token_to_words)

  ;; No pos prediction (get it from lexicon)
  (set! pos_lex_name nil)
  (set! guess_pos url_cat_guess_pos) 
  ;; Phrase break prediction by punctuation
  (set! pos_supported nil) ;; well not real pos anyhow
  ;; Phrasing
  (set! phrase_cart_tree url_cat_phrase_cart_tree)
  (Parameter.set 'Phrase_Method 'cart_tree)
  ;; Lexicon selection
  (lex.select "url_cat")

  ;; No postlexical rules
  (set! postlex_rules_hooks nil)

  ;; Accent and tone prediction
  (set! int_accent_cart_tree url_cat_accent_cart_tree)

  (Parameter.set 'Int_Target_Method 'Simple)

  (Parameter.set 'Int_Method 'General)
  (set! int_general_params (list (list 'targ_func url_cat_jdf_targ_func1)))

  ;; Duration prediction
  (set! duration_cart_tree url_cat_jdf::zdur_tree)
  (set! duration_ph_info url_cat_jdf::phone_data)
  (Parameter.set 'Duration_Method 'Tree_ZScores)

  ;; Waveform synthesizer: diphones
  (set! UniSyn_module_hooks (list url_cat_jdf_diphone_fix))
  (set! us_abs_offset 0.0)
  (set! window_factor 1.0)
  (set! us_rel_offset 0.0)
  (set! us_gain 0.9)

  (Parameter.set 'Synth_Method 'UniSyn)
  (Parameter.set 'us_sigpr 'lpc)
  (us_db_select url_cat_jdf_db_name)

  ;; set callback to restore some original values changed by this voice
  (set! current_voice_reset url_cat_jdf_voice_reset)

  (set! current-voice 'url_cat_jdf_diphone)
)

(proclaim_voice
 'url_cat_jdf_diphone
 '((language cat)
   (gender COMMENT)
   (dialect COMMENT)
   (description
    "COMMENT"
    )
   (builtwith festvox-1.2)))

(provide 'url_cat_jdf_diphone)
