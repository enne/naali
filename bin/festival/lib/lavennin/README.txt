Suopuhe-projektin tekstin lavennin
==================================

Copyright (c) 2000-2003
Helsingin yliopisto, 2000-2003 tai Suopuhe-projekti 

Tekij�: Nicholas Volk, <nvolk@ling.helsinki.fi>

Lisenssi: LGPL, katso tiedosto LICENCE.

Versiohistoria:

----    Fri Oct 22 2004

	Lis�tty lisenssitekstiehdotelma. Kiert�nee viel� hyv�ksytt�v�n�.

1.0d    Wed Oct 8 2003
	Korjattu kongruoiviin lyhenteisiin raakatekstimoodissa 
	liittynyt bugi, joka aiheutti ohjelman suorituksen
	keskeytyksen.
	Parannettu tuntemattomien merkkien lukemisessa
	graavi- tms. merkkien k�sittely� ja
	tuntematon merkki ignoroidaan kokonaan.

1.0c    Sun Sep 14 2003
	Erin�isi� manuaalin p�ivityksi� johtoryhm�n iloksi.
	Satunnaisia koodiin ja s��n�tihin kajoamisia siin� ohessa.
	Roivottavasti en rikkonut mit��n, ei aikaa asianmukaiseen
	testaukseen...
	Lis�tty taika, joka muuttaa sy�terivit, joilla on vain isoja kirjaimia
	riveiksi joilla on vain pieni� kirjaimiksi. N�in otsikkoja
	tms. ei luulla lyhenteiksi ja lueta kirjain kerrallaan...

1.0b    Tue Jun 02 2003
	Bugikorjauksia evaluaation pohjalta.
	Korjattu mm. isojen kirjainten erittely�,
        kilo-tyyppisten etuliitteiden k�sittely�, tokenisointia sek�
        rahayksikk�jen lukua ja desimaalinollien k�sittely�.
        Leksikkoa kasvatettu hieman.

1.0a	Tue May 13 08:42:40 EEST 2003
	Muutama bugifiksi. L�hinn� evaluaatiota varten "j��dytetty" versio.

1.0	Mon Mar 24 15:25:54 EET 2003
	Ensimm�inen virallinen versio


Tiedostot
---------

lavennin/README.txt
	T�m� tiedosto
lavennin/bin/lavennin
	Itse lavenninohjelma
lavennin/data/fin_kongruoivat_lyhenteet.txt
lavennin/data/fin_kongruoivat_sanat.txt
lavennin/data/fin_lavennussaannot.txt
lavennin/data/fin_luvut.txt
lavennin/data/fin_merkit.txt
lavennin/data/fin_yleissanasto.txt
	Ohjelman s��nt�tiedostot
lavennin/man/*.shtml \
	Ohjelman k�ytt�ohjeet ja dokumentaatio HTML-muodossa.
lavennin/tmp
	Ohjelman v�liaikaistiedostot

K�ytt� itsen�isesti:

$ lavennin < foo.txt

K�ytt� suopuheen syntetisaattorin kanssa:

$ lavennin --output=xml < foo.txt > foo.suo; festival --tts foo.suo

Tarkemmat ohjeet man-hakemistossa.
