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
;;; lexical look up, from words to pronunciations
;;;
;;;
;;;
;;; La majoria de les regles aplicades son basades en les ja existents
;;; al sistema TTS de la Salle.
;;; La resta son modificacions practiques per tal de que el sistema funcioni
;;; Codi i modificacions: (c) Jordi Domínguez Fuster 2003
;;;
;;;




;;;  Function called when word not found in lexicon
(define (url_cat_lts_function word features)
  "(url_cat_lts_function WORD FEATURES)
Return pronunciation of word not in lexicon."
  (format stderr "failed to find pronunciation for %s\n" word)
  (let ((dword (downcase word)))
    ;; Not you may need to use a letter to sound rule set to do
    ;; casing if the language has non-ascii characters in it.
    (if (lts.in.alphabet word 'url_cat)
	(list
	 word
	 features
	 ;; This syllabification is almocst certainly wrong for
	 ;; this language (its not even very good for English)
	 ;; but it will give you something to start off with
	 (lex.syllabify.phstress
	   (lts.apply word 'url_cat)))
	(begin
	  (format stderr "unpronounceable word %s\n" word)
	  ;; Put in a word that means "unknown" with its pronunciation
	  '("nepoznat" nil (((N EH P) 0) ((AO Z) 0) ((N AA T) 0))))))
)

(define (url_cat_addenda)
  "(cat_addenda)
Basic lexicon should (must ?) basic letters and punctuation."

;;; Pronunciation of letters in the alphabet
(lex.add.entry '("a" nn (((a) 0))))
(lex.add.entry '("b" nn (((b e) 0))))
(lex.add.entry '("c" nn (((s e) 0))))
(lex.add.entry '("d" nn (((d e) 0))))
(lex.add.entry '("e" nn (((e) 0))))
(lex.add.entry '("f" nn (((e f e) 0))))
(lex.add.entry '("g" nn (((j e) 0))))
(lex.add.entry '("h" nn (((a k) 0))))
(lex.add.entry '("i" nn (((i) 0))))
(lex.add.entry '("j" nn (((j o t a) 0))))
(lex.add.entry '("k" nn (((k a) 0))))
(lex.add.entry '("l" nn (((e l e) 0))))
(lex.add.entry '("m" nn (((e m e) 0))))
(lex.add.entry '("n" nn (((e n e) 0))))
(lex.add.entry '("o" nn (((o) 0))))
(lex.add.entry '("p" nn (((p e) 0))))
(lex.add.entry '("q" nn (((k u) 0))))
(lex.add.entry '("r" nn (((e R e) 0))))
(lex.add.entry '("s" nn (((e s e) 0))))
(lex.add.entry '("t" nn (((t e) 0))))
(lex.add.entry '("u" nn (((u) 0))))
(lex.add.entry '("v" nn (((b e b a i x a) 0))))
(lex.add.entry '("w" nn (((b e d o b l e) 0))))
(lex.add.entry '("x" nn (((i k s) 0))))
(lex.add.entry '("y" nn (((i g r e g a) 0))))
(lex.add.entry '("z" nn (((T e t a) 0))))

; ...
; Symbols ...
(lex.add.entry
 '("*" n (((a s) 0) ((t e) 0) ((r i1 s) 1)  ((k) 0))))
(lex.add.entry
 '("%" n (((p e r) 0) ((c e n) 1) ((t) 0))))

;; Basic punctuation must be in with nil pronunciation
(lex.add.entry '("." punc nil))
;(lex.add.entry '("." nn (((p u n) 1) ((t) 0)) nil))
(lex.add.entry '("'" punc nil))
(lex.add.entry '(":" punc nil))
(lex.add.entry '(";" punc nil))
(lex.add.entry '("," punc nil))
;(lex.add.entry '("," nn (((k o) 1) ((m a) 0)) nil))
(lex.add.entry '("-" punc nil))
(lex.add.entry '("\"" punc nil))
(lex.add.entry '("`" punc nil))
(lex.add.entry '("?" punc nil))
(lex.add.entry '("!" punc nil))

;excepcions de pronunciació

(lex.add.entry '("amb" nn (((am) 0))))
)


;; You may or may not be able to write a letter to sound rule set for
;; your language.  If its largely lexicon based learning a rule
;; set will be better and easier that writing one (probably).


(lts.ruleset url_cat

; OJO!!!! LA ô equival a ó !!!!!!!
;  Sets used in the rules
(
  (LNS l n s )
  (DNSR d n s r )
  (EI e i é e1 è e2 ê í)  ; note that accented vowels are included in this set
  (AEIOUt  à é e1 è e2 ê í ó ò ô ú )
  (AE a e )
  (AOU a à o ó ò u ú ü )
  (CÇ c ç )
  (V a e i o u )
  (C b c ç d f g h j k l ll m n p q r s t v w x y z )
)

; per a introduïr regles noves, recordar que el sistema llegeix de dalt
; a baix, per tant no afegir les regles a la part de baix, ja que el sistema
; les ignora.
; afegir les excepcions abans de la regla generica!!!!

 (



 ; alguns trifonemes es millor colocar-los aci
 ( V [ r p ] = Rp )
 ( V [ r b ] = Rb )
 ( V [ r t ] = Rt )
 ( V [ r d ] = Rd )
 ( V [ r k ] = Rk )
 ( V [ r q u ] = Rk)
 ( V [ r g ] = Rg )
 ( V [ r m ] = Rm )
 ( V [ r n ] = Rn )
 ( V [ r n y ] = RJ )
 ( V [ r f ] = Rf )
 ( V [ r z ] = RT )
 ( V [ r x ] = RS )
 ( V [ r l ] = Rl )
 ( V [ r l l ] = RL )
 ( V [ r r ] V = R )
 ( V [ r r  ] = RR )
 ( V [ r c ] EI = Rs )
 ( V [ r s ] # = Rs )


 ; excepcions generals d' accentuació
 ; "-ea , -ees "
 ( # d [ e ] a # = e )
 ( # d [ e ] e s # = e )
 ; "-ebre, -ebra"
 ( # c o n c [ e ] b r e # = E )
 ( # o r f [ e ] b r e # = E )
 ( # p [ e ] b r e # = E )
 ( # p [ e ] b r e s # = E )
 ( # r [ e ] b r e # = E)
 ; " -ec, -ecs"
 ( # a i x [ e ] c # = e )
 ( # a i x [ e ] c s # = e )
 ( # b [ e ] c # = e )
 ( # b [ e ] c s # = e )
 ( # c [ e ] c # = e )
 ( # c [ e ] c s # = e )
 ( # g e m [ e ] c # = e )
 ( # g e m [ e ] c s # = e )
 ( # g r [ e c ] # = e k )
 ( # g r [ e c s ] # = e k s )
 ( t [ e ] c e d e n t = @ )
 ( # l l [ e ] c # = e )
 ( # l l [ e ] c s # = e )
 ( # p [ e ] c # = e )
 ( # p [ e ] c s # = e )
 ( # p r [ e ] c # = e )
 ( # p r [ e ] c s # = e )
 ( # r [ e ] c # = e )
 ( # r [ e ] c s # = e )
 ( # r e n [ e ] c # = e )
 ( # r e n [ e ] c s # = e )


 ;cosetes

 ( [ "'" ] =  )
 ( # d [ a ] v i d # = @ )
 ;" bruixa, angoixa"
 ( q u [ i ] x = i )
 ( g u [ i ] x = i )
 ( V [ i ] x =  )
 ;"desbrossar"
 ( # d [ e ] s = @ )
 ; " gelosia"
 ( [ o ] s i a # = u )
 ;" cobrar, robar, però no coure o roure"
 ( # C [ o ] C V C = u )

 ; regles de la "a":
 ; "a" a final de paraula sone "@ "
 ( [ a ] # = @ )
 ; "a" a meitat de paraula sone "a"
 ( [ a ] = a )
 ; "à" a final de paraula sone "a" "Cassà de la Selva"
 ( [ à ] # = a )
 ; "à" a principi de paraula sone "a"
 ( # [ a ] = a )

 ; regles de la "e"
 ;"el, els"
 ( # [ e ] l # = @ )
 ( # [ e ] l s # = @ )
 ;"estudiós, escolapi""
 ( # [ e ] = @ )
 ;"arquitecte"
 ( [ e ] # = @ )
 ; "les nenes"
 ( [ e ] s # = @ )
 ; "del, mel, pel"
 ( # C [ e ] C # = @ )
 ; "aldea"
 ( [ e ] a # = E )
 ; "real, crear"
 ( [ e ] a = e )
 ; "aeròdrom, israelià"
 ( a [ e ] = e )
 ; "aldees"
 ( [ e e ] s # = E )
 ; "grecs, renecs"
 ( e [ c s ] # = k s )
 ; "aplec"
 ( [ e ] c = E )
 ; "plet"
 ( [ e ] t = E )
 ; "aguilenc, rogenc"
 ( [ e ] n c # = E )
  ; "aparença"
 ( [ e ] n CÇ AE # = E )
 ; "biblioteca, fageda, bodega"
 ( C [ e ] C a # = E )
 ; "bateig"
 ( [ e ] i g # = E )
 ; "prendre, vendre"
 ( [ e ] n d r e # = E )
 ; "ascens"
 ( C [ e ] n s = E )
 ; "ensenya, areny, arenys"
 ( C [ e ] n y = E )
 ; "hivern, taverna, hiverns"
 ( C [ e ] r n = E )
 ; "cert"
 ( C [ e ] r t = E )
 ; "francesc, cesca, cescos"
 ( c [ e ] s C = E )
 ; "abadessa, abadesses"
 ( [ e ] s s a # = E )
 ( [ e ] s s e s # = E )
 ; "cremar, sucre"
 ( c r [ e ] = @ )
 ; "deu, beu"
 ( [ e ] u = E )
 ; "es pot, especial"
 ( # [ e ] = @ )
 ( # C [ e ] = @ )
 ; "dent"
 ( d [ e ] n t = e )
 ; per defecte
 ( [ e ] = e )

 ; regles de la "é"
 ; "bé"
 ( [ é ] = e )
 ;per solucionar problemes de teclat
 ( [ e1 ] = e )

 ; regles de la è
 ; cafè
 ( [ è ] = E )
 ( [ ê ] = E )
 ( [ e2 ] = E )

 ; excepció "lleig, maig"
 ( V [ i g ] # = t S )

 ; regles de la i
 ;"quilo, arquitecte"
 ( q u [ i ] = i )
 ; semivocal
 ( V [ i ] = j )
 ; genèrica
 ( [ i ] = i )
 ; regles de la í
 ( [ í ] = i )
 ; regles de la ï
 ( [ ï ] = i )

 ; regles de la o
 ; "bloc, pot"
 ( [ o ] C # = O )
 ; "blocs, pots"
 ( [ o ] C s # = O )
 ; "pota"
 ( [ o ] t a # = O )
 ; "potes"
 ( [ o ] t e s # = O )
 ; "coques"
 ( [ o ] q u e s # = O )
 ; "fort"
 ( [ o ] r t # = O )
 ; "forts"
 ( [ o ] r t s # = O )
 ; " forta"
 ( [ o ] r t a # = O )
 ; "fortes"
 ( [ o ] r t e s # = O )
 ( [ o ] ss = O )
;; "tongo, penjo"
 ( [ o ] # = u )
 ; per defecte
 ( [ o ] = o )
 ; regles de la ó
 ( [ ó ] = o )
 ( [ ô ] = o )
 ; regles de la ò
 ( [ ò ] = O )

 ; regles de la u
 ; "pau gasol"
 ( V [ u ] = w )
 ;; "couen"
 ( V [ u ] V = w )
 ; genèric "punta"
 ( [ u ] = u )
 ; la regla del pingüí
 ( [ ü ] EI = w )
 ( V [ ü ] = u )
 ; la ú
 ( [ ú ] = u )

 ; regles de la "b" i la "v":
 ; "obra, cabrit"
 ( V [ b r ] V = Br )
 ; "poble, noble"
 ( V [ b l ] = bl )
 ;; "blanc"
 ( # [ b l ] = Bl )
 ( # [ b r ] = Br )
 ( [ b r ] = br )
 ( [ b l ] = bl )
 ;; "tub"
 ( V [ b ] # = b )
 ; "alba"
 ( l [ b ] = B )
 ; "vint"
 ( # [ v ] V = B )
 ; "cava"
 ( V [ v ] V = B )
 ; else
 ( [ v ] = b )
 ; "ambdos"
 ( m [ b ] C = # )
 ; "amb ells"
 ( m [ b ] # = b )
 ;; "sobar"
 ( V [ b ] V = B )
 ;( # [ b ] = b )
 ; regla per defecte
 ( [ b ] = b )

 ; regles de la "c"
 ; "client"
 ( [ c l ] = kl )
 ;"criatura"
 ( [ c r ] = kr )
 ; "excitar, excedir"
 ( x [ c ] EI = s )
 ;; "cirera, encís"
 ( [ c ] EI C = s )
 ( [ c ] EI = s )
 ;; "blanca, clement"
 ( n [ c ] V = k )
 ;; "blanc, blancs"
 ( [ n c ] # = N )
 ;; "cacofonia": regla per defecte
 ( [ c ] = k );

 ; regles de la "ç"
 ; "feliçment"
 ( [ ç ] C = z )
 ; "feliç"
 ( V [ ç ] # = z )
 ; "caça":regla per defecte
 ( [ ç ] = s )

 ; regles de la "d"
 ; "David"
 ( [ d ] # = t )
 ( # [ d r ] = Dr )
 ( [ d r ] = dr )
 ; "Armand, Elrond"
 ( l [ d ] # = # )
 ( n [ d ] # = # )
 ; "quedar "
 ( V [ d ] V = D )
 ; "verds, perds"
 ( r [ d ] s = R )
 ; "adquirir, adscriure"
 ( [ d ] q = t )
 ( [ d ] s = t )
 ( [ d ] c = t )
 ( [ d ] ç = t )
 ( [ d ] f = t )
 ( [ d ] p = t )
 ( [ d ] t = t )
 ( [ d ] x = t )
; ; regla per defecte
 ( [ d ] = d )

 ; regles de la "f"
 ; "frú-frú"
 ( [ f r ] = fr )
 ( [ f l ] = fl )
 ; "fotògraf"
 ( [ f ] # = v )
 ( [ f ] C = v )
 ; per defecte
 ( [ f ] = f )

 ; regles de la "g"
 ( # [ g r ] = Gr )
 ( [ g r ] = gr )
 ( # [ g l ] = Gl )
 ( [ g l ] = gl )
 ( # [ g w ] = Gw )
 ( [ g w ] = gw )


 ; "mig, desig"
 ( C i [ g ] # = t S )
 ( C i [ g ] s # = t S )
 ;; "agustí, agosarat, algarabia"
 ( V [ g ] AOU = G )
 ( l [ g ] AOU = G )
 ;;; "aglomerat"
 ;( V [ g ] l = G )
 ;; "agradar"
 ( V [ g ] r = G )
 ( l [ g ] l = G )
 ( l [ g ] r = G )
 ;; "suggeriment" (amb d, sí)
 ( [ g ] g = d )
 ; "migjorn"
 ( [ g ] j = d )
;;; ; "sang"
 ( [ n g ] # = N )
; ; "càstig, llarg"
 ( [ g ] # = k )
; ; "gepa, gimnàs"
 ( [ g ] EI = Z )
 ( [ g u ] EI = g )
 ; "gnom, Ignasi"
 ( [ g n ] = N n )
 ; per defecte
; ; "gat, gos"
 ( [ g ] = g )

 ; regles de la "h"
 ; "hola, alcohol"
 ( [ h ] =  # )

 ; regles de la j
 ;"joglar"
 ( [ j ] = Z )

 ; regles de la k
 ; "kilo, kàiser"
 ( [ k ] = k )

 ;; regles de la l
 ;; l geminada
 ;;;( [ l . l ] = l );;;;OJO!!! AQUESTA REGLA PENJA EL SISTEMA!!!!
 ;;;; FESTIVAL LA ENTEN COM REDEFINICIO DEL PUNT
 ;;;; CUIDADIN!!!!!
 ( [ l · l ] = l )
 ( [ l _ l ] = l )
 ; "àlgebra"
 ( [ l ] g EI = L )
 ( [ l l ] = L )
 ; per defecte "pala"
 ( [ l ] = l )
;
 ; regles de la m
 ; "àmfora"
 ( [ m ] f = M )
 ; per defecte "mare"
 ( [ m ] = m )

 ; regles de la n
 ; "envà"
 ( [ n ] v = m )
 ( [ n ] m = m )
 ( [ n ] b = m )
 ( [ n ] p = m )
 ;; "aniràs a l'infern"
 ( [ n ] f = m )
 ;; "monja"
 ( [ n j ] = J )
 ;; "punxa"
 ;( [ n x ] = J )
 ; "canya"
 ( [ n y ] = J )
 ; per despistats: "ñ= ny"
 ( [ ñ ] = n y )
 ; "singlot"
 ( V [ n g ] = N )
 ; "Blanca"
 ( V [ n ] c = N )
 ( V [ n ] k = N )
 ; "blanques"
 ( V [ n q u ] EI = N )
 ;;;; per defecte
 ;;;"cana, canela, nap"
 ( [ n ] = n )

 ; regles de la p
 ; "psicologia, psiquiatra"
 ( # [ p ] s = # )
 ; "hipnotisme, capmà"
 ( [ p ] m = m )
 ( [ p ] n = m )
 ; "capbussar"
 ( [ p ] b = b )
 ( [ p ] v = b )
 ( [ p ] d = b )
 ( [ p ] j = b )
 ( [ p ] g = b )
 ( [ p ] l = b )
 ( [ p ] m = b )
 ( [ p ] n = b )
 ( [ p ] z = b )
 ;;"temps era temps"
 ( m [ p ] s = # )
 ( m [ p ] c EI = # )
 ;; "camp"
 ( m [ p ] # = # )
 ; prova
 ( [ p r ] V = pr )
 ;ploure
 ( [ p l ] = pl )
 ;; per defecte pare
 ( [ p ] = p )


 ; regles de la q
 ; "quilo, queco"
 ( [ q u ] EI = k )
 ( [ q u i ] = k i )
 ;;; per defecte "qualsevol"
 ( [ q ] = k )

 ; la "r"
 ; final de paraula
 ( [ r ] # = # )
 ; "carro"
 ( [ r r ] = R )
 ; "carnívor, carn"
 ( [ r ] n = R )
 ;"rocambolesc, Roger, rata"
 ( # [ r ] = R )
 ; "Elrond, Elrohir"
 ( l [ r ] V = R )
 ; "Enric"
 ( n [ r ] V = R )
 ; "carro"
 ( [ r r ] = R )
 ; "cabra, cabrit"
 ( [ r ] V = r )
 ; per defecte "germanor"
 ( [ r ] = R )

 ; regles de la s
 ; "aquest, aquests"
 ( a q u e [ s ] t # = # )
 ( a q u e [ s ] t s # = # )
 ; "dins, endinsar"
 ( d i n [ s ] = z )
 ; "transport, transeunt"
 ( t r a n [ s ] = z )
 ; "enfonsar"
 ( f o n [ s ] = z )
 ;"esfinxs"
 ( n x [ s ] = # )
 ; "larixs"
 ( i x [ s ] = # )
 ;"soca, sabata"
 ( # [ s ] = s )
 ; "rosa" intervocàlica
 ( V [ s ] V = z )
 ; "mas, ros"
 ( # C V [ s ] # = s )
 ;"fals"
 ( l [ s ] # = z )
 ; "passar de tot"
 ( [ s s ] = s )
 ;"esbull"
 ( [ s ] b = z )
 ; "esglaó, esma, esverat, esnovar, eslavó"
 ( [ s ] g = z )
 ( [ s ] l = z )
 ( [ s ] m = z )
 ( [ s ] n = z )
 ( [ s ] v = z )
 ;; per defecte
 ( [ s ] = s )

 ; regles de la t
 ; trifonema
 ( [ t r ] = tr )
 ; "tant, alt"
 ( n [ t ] # = # )
 ( l [ t ] # = # )
 ; "dents, alts"
 ( n [ t ] s # = # )
 ( l [ t ] s # = # )
 ; "gots"
 ( V [ t ] s = t )
 ;"gatllardó"
 ( [ t ] C = d )
 ; per defecte "taca"
 ( [ t ] = t )

 ; w
 ;"wagner"
 ( [ w ] = b )

 ; regles de la "iquis"
 ; inci paraula "xarel.lo"
 ( # [ x ] = S )
 ; "fitxer"
 ( t [ x ] = S )
 ; final
 ( [ x ] # = S )
 ;; "excedir"
 ( [ x c ] = k s )
 ; "examen"
 ( e [ x ] V = z )
 ; "exhaurit"
 ( [ x h ] = g s )
 ; "auxili"
 ( u [ x ] i = s )
 ;per defecte
 ( [ x ] = S )

 ;per defecte "cayo viscaíno key biskein"
 ( [ y ] = i )

 ; regles de la z
 ( [ z ] = T )

  ;; LTS rulez

  ( [ s Gr ]  = s # Gr )
  ))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  Catalan syllabification by rewrite rules
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(lts.ruleset
   url_cat_syl
   (  (V a1 i1 u1 e1 e2 ê o1 a @ i j u e o O )
      (IUT i1 u1 )
      (C b ç d f g x k l L m n p q r R s S t T v z Z )
   )
   ;; Rules will add - at syllable boundary
   (
      ;; grups vàlids
      ( V C * [ b l ] V = - b l )
      ( V C * [ b r ] V = - b r )
      ( V C * [ k l ] V = - k l )
      ( V C * [ k r ] V = - k r )
      ( V C * [ k s ] V = - k s ) ; paraules amb "iquis" ó "xeix"
      ( V C * [ d r ] V = - d r )
      ( V C * [ f l ] V = - f l )
      ( V C * [ f r ] V = - f r )
      ( V C * [ g l ] V = - g l )
      ( V C * [ g r ] V = - g r )
      ( V C * [ p l ] V = - p l )
      ( V C * [ p r ] V = - p r )
      ( V C * [ t l ] V = - t l )
      ( V C * [ t r ] V = - t r )

      ;; triptongs
      ( [ i a i ] = i a i )
      ( [ i a u ] = i a u )
      ( [ u a i ] = u a i )
      ( [ u a u ] = u a u )
      ( [ i e i ] = i e i )
      ( [ i e u ] = i e u )
      ( [ u e i ] = u e i )
      ( [ u e u ] = u e u )
      ( [ i o i ] = i o i )
      ( [ i o u ] = i o u )
      ( [ u o i ] = u o i )
      ( [ u o u ] = u o u )
      ( [ i a1 i ] = i a1 i )
      ( [ i a1 u ] = i a1 u )
      ( [ u a1 i ] = u a1 i )
      ( [ u a1 u ] = u a1 u )
      ( [ i e1 i ] = i e1 i )
      ( [ i e1 u ] = i e1 u )
      ( [ u e1 i ] = u e1 i )
      ( [ u e1 u ] = u e1 u )
      ( [ i o1 i ] = i o1 i )
      ( [ i o1 u ] = i o1 u )
      ( [ u o1 i ] = u o1 i )
      ( [ u o1 u ] = u o1 u )

      ;; break invalid triptongs
      ( IUT [ i a ]  = - i a )
      ( IUT [ i e ]  = - i e )
      ( IUT [ i o ]  = - i o )
      ( IUT [ u a ]  = - u a )
      ( IUT [ u e ]  = - u e )
      ( IUT [ u o ]  = - u o )
      ( IUT [ a i ]  = - a i )
      ( IUT [ e i ]  = - e i )
      ( IUT [ o i ]  = - o i )
      ( IUT [ a u ]  = - a u )
      ( IUT [ e u ]  = - e u )
      ( IUT [ o u ]  = - o u )
      ( IUT [ i u ]  = - i u )
      ( IUT [ u i ]  = - u i )
      ( IUT [ i a1 ]  = - i a1 )
      ( IUT [ i e1 ]  = - i e1 )
      ( IUT [ i o1 ]  = - i o1 )
      ( IUT [ u a1 ]  = - u a1 )
      ( IUT [ u e1 ]  = - u e1 )
      ( IUT [ u o1 ]  = - u o1 )
      ( IUT [ a1 i ]  = - a1 i )
      ( IUT [ e1 i ]  = - e1 i )
      ( IUT [ o1 i ]  = - o1 i )
      ( IUT [ a1 u ]  = - a1 u )
      ( IUT [ e1 u ]  = - e1 u )
      ( IUT [ o1 u ]  = - o1 u )
      ( IUT [ i u1 ]  = - i u1 )
      ( IUT [ u i1 ]  = - u i1 )

      ;; diptongs
      ( [ i a ]  = i a )
      ( [ i e ]  = i e )
      ( [ i o ]  = i o )
      ( [ u a ]  = u a )
      ( [ u e ]  = u e )
      ( [ u o ]  = u o )
      ( [ a i ]  = a i )
      ( [ e i ]  = e i )
      ( [ o i ]  = o i )
      ( [ a u ]  = a u )
      ( [ e u ]  = e u )
      ( [ o u ]  = o u )
      ( [ i u ]  = i u )
      ( [ u i ]  = u i )
      ( [ i a1 ]  = i a1 )
      ( [ i e1 ]  = i e1 )
      ( [ i o1 ]  = i o1 )
      ( [ u a1 ]  = u a1 )
      ( [ u e1 ]  = u e1 )
      ( [ u o1 ]  = u o1 )
      ( [ a1 i ]  = a1 i )
      ( [ e1 i ]  = e1 i )
      ( [ o1 i ]  = o1 i )
      ( [ a1 u ]  = a1 u )
      ( [ e1 u ]  = e1 u )
      ( [ o1 u ]  = o1 u )
      ( [ u1 i ]  = u1 i )
      ( [ i1 u ]  = i1 u )

      ;; Vowels preceeded by vowels are syllable breaks
      ;; triptongs and diptongs are dealt with above
      ( V [ a ]  = - a )
      ( V [ i ]  = - i )
      ( V [ u ]  = - u )
      ( V [ e ]  = - e )
      ( V [ o ]  = - o )
      ( V [ a1 ]  = - a1 )
      ( V [ e1 ]  = - e1 )
      ( V [ i1 ]  = - i1 )
      ( V [ o1 ]  = - o1 )
      ( V [ u1 ]  = - u1 )

      ;; If any consonant is followed by a vowel and there is a vowel
      ;; before it, its a syl break
      ;; the consonant cluster are dealt with above
      ( V C * [ b ] V = - b )
      ( V C * [ ch ] V = - ch )
      ( V C * [ d ] V = - d )
      ( V C * [ f ] V = - f )
      ( V C * [ g ] V = - g )
      ( V C * [ x ] V = - S )
      ( V C * [ k ] V = - k )
      ( V C * [ l ] V = - l )
      ( V C * [ L ] V = - L )
      ( V C * [ m ] V = - m )
      ( V C * [ n ] V = - n )
      ( V C * [ p ] V = - p )
      ( V C * [ q ] V = - q )
      ( V C * [ r ] V = - r )
      ( V C * [ R ] V = - R )
      ( V C * [ s ] V = - s )
      ( V C * [ t ] V = - t )
      ( V C * [ v ] V = - b )
      ( V C * [ z ] V = - T )

      ;; Catch all consonants on their own (at end of word)
      ;; and vowels not preceded by vowels are just written as it
      ( [ b ] = b )
      ( [ c ] = k )
      ( [ d ] = d )
      ( [ f ] = f )
      ( [ g ] = g )
      ( [ x ] = x )
      ( [ k ] = k )
      ( [ l ] = l )
      ( [ L ] = L )
      ( [ m ] = m )
      ( [ n ] = n )
      ( [ p ] = p )
      ( [ q ] = q )
      ( [ r ] = r )
      ( [ R ] = R )
      ( [ s ] = s )
      ( [ x ] = S )
      ( [ t ] = t )
      ( [ v ] = f )
      ( [ z ] = T )
      ( [ a ]  =  a )
      ( [ i ]  =  i )
      ( [ u ]  =  u )
      ( [ e ]  =  e )
      ( [ o ]  =  o )
      ( [ a1 ]  =  a1 )
      ( [ i1 ]  =  i1 )
      ( [ u1 ]  =  u1 )
      ( [ e1 ]  =  e1 )
      ( [ o1 ]  =  o1 )
   )
)


;;; Lexicon definition

(lex.create "url_cat")
(lex.set.phoneset "url_cat")
(lex.set.lts.method 'url_cat_lts_function)
;(lex.set.compile.file (path-append url_cat_jdf_dir "url_cat_lex.out"))
(url_cat_addenda)

(provide 'url_cat_lex)
