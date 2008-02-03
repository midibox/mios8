; $Id$
;
; MIDIboxSEQ
; Scale functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

SCALE_ENTRY MACRO c, cd, d, dd, e, f, fd, g, gd, a, ad, b, str
	db	c | (cd << 4), d | (dd << 4), e | (f << 4), fd | (g << 4), gd | (a << 4), ad | (b << 4), str
	ENDM


	;; Please contact stryd_one with any corrections or requested scales. I have an algorithm and can generate your scale in minutes - but it may not be perfect, so feedback is welcomed.
	
	;; Big thanks to TK for making this awesome instrument, and being such a generous guy as to share with us all the benefits of his skill and hard work.


SEQ_SCALE_TABLE
	;;	        C 	C#	D 	D#	E 	F 	F#	G 	G#	A 	A#	B
	;;	        0 	1 	2 	3 	4 	5 	6 	7 	8 	9 	10	11 	Semitone
	;;              1 	b2	2 	b3	3 	4 	b5	5 	b6	6 	b7	7	Minor Tone
	;;              1 	#1	2 	#2	3 	4 	#4	5 	#5	6 	#6	7 	Augmented Tone
	;; 1..10                                                                                                         <------------------>
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	9,	9,	11,	11,	"Major               "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	8,	8,	11,	11,	"Harmonic Minor      "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	9,	9,	11,	11,	"Melodic Minor       "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	8,	8,	10,	10,	"Natural Minor       "
        SCALE_ENTRY     0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	"Chromatic           "
        SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	8,	8,	10,	10,	10,	"Whole Tone          "
        SCALE_ENTRY     0,	2,	2,	4,	4,	7,	7,	7,	9,	9,	9,	9,	"Pentatonic Major    "
        SCALE_ENTRY     0,	0,	3,	3,	3,	5,	7,	7,	7,	10,	10,	10,	"Pentatonic Minor    "
        SCALE_ENTRY     0,	0,	3,	3,	3,	5,	6,	7,	7,	10,	10,	10,	"Pentatonic Blues    "
        SCALE_ENTRY     0,	2,	2,	2,	5,	5,	7,	7,	7,	10,	10,	10,	"Pentatonic Neutral  "
	;; 11..20                                                                                                        <------------------>
        SCALE_ENTRY     0,	1,	1,	3,	4,	6,	6,	7,	9,	9,	10,	10,	"Octatonic (H-W)     "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	6,	6,	8,	9,	11,	11,	"Octatonic (W-H)     "
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	9,	9,	11,	11,	"Ionian              "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	9,	9,	10,	10,	"Dorian              "
        SCALE_ENTRY     0,	1,	1,	3,	3,	5,	7,	7,	8,	8,	10,	10,	"Phrygian            "
        SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	7,	9,	9,	11,	11,	"Lydian              "
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	9,	9,	10,	10,	"Mixolydian          "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	8,	8,	10,	10,	"Aeolian             "
        SCALE_ENTRY     0,	1,	1,	3,	3,	5,	6,	6,	8,	8,	10,	10,	"Locrian             "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	6,	7,	8,	8,	11,	11,	"Algerian            "
 	;; 21..30                                                                                                        <------------------>
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	6,	8,	8,	9,	11,	11,	"Arabian (A)         "
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	6,	8,	8,	10,	10,	10,	"Arabian (B)         "
        SCALE_ENTRY     0,	3,	3,	3,	4,	6,	6,	8,	8,	11,	11,	11,	"Augmented           "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	6,	8,	8,	9,	11,	11,	"Auxiliary Diminished"
        SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	8,	8,	10,	10,	10,	"Auxiliary Augmented "
        SCALE_ENTRY     0,	1,	1,	3,	4,	6,	6,	7,	9,	9,	10,	10,	"Auxiliary Diminished"
        SCALE_ENTRY     0,	1,	1,	3,	3,	7,	7,	7,	8,	8,	8,	8,	"Balinese            "
        SCALE_ENTRY     0,	0,	3,	3,	3,	5,	6,	7,	7,	10,	10,	10,	"Blues               "
        SCALE_ENTRY     0,	1,	1,	4,	4,	5,	7,	7,	8,	8,	11,	11,	"Byzantine           "
        SCALE_ENTRY     0,	0,	4,	4,	4,	6,	6,	7,	7,	11,	11,	11,	"Chinese             "
	;; 31..40                                                                                                        <------------------>
        SCALE_ENTRY     0,	2,	2,	4,	4,	7,	7,	7,	9,	9,	9,	9,	"Chinese Mongolian   "
        SCALE_ENTRY     0,	2,	2,	4,	4,	7,	7,	7,	9,	9,	9,	9,	"Diatonic            "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	6,	6,	8,	9,	11,	11,	"Diminished          "
        SCALE_ENTRY     0,	1,	1,	3,	4,	6,	6,	7,	9,	9,	10,	10,	"Diminished, Half    "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	6,	6,	8,	9,	11,	11,	"Diminished, Whole   "
        SCALE_ENTRY     0,	1,	1,	3,	4,	6,	6,	6,	8,	8,	10,	10,	"Diminished WholeTone"
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	9,	9,	10,	10,	"Dominant 7th        "
        SCALE_ENTRY     0,	1,	1,	4,	4,	5,	7,	7,	8,	8,	11,	11,	"Double Harmonic     "
        SCALE_ENTRY     0,	2,	2,	2,	5,	5,	7,	7,	7,	10,	10,	10,	"Egyptian            "
        SCALE_ENTRY     0,	1,	1,	3,	4,	5,	6,	6,	8,	8,	10,	10,	"Eight Tone Spanish  "
	;; 41..50                                                                                                        <------------------>
        SCALE_ENTRY     0,	1,	1,	4,	4,	6,	6,	8,	8,	10,	10,	11,	"Enigmatic           "
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	9,	9,	11,	11,	"Ethiopian (A raray) "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	8,	8,	10,	10,	"Ethiopian Geez Ezel "
        SCALE_ENTRY     0,	1,	1,	3,	3,	5,	6,	6,	8,	8,	10,	10,	"Half Dim (Locrian)  "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	6,	6,	8,	8,	10,	10,	"Half Dim 2 (Locrian)"
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	9,	9,	11,	11,	"Hawaiian            "
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	8,	8,	10,	10,	"Hindu               "
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	8,	8,	10,	10,	"Hindustan           "
        SCALE_ENTRY     0,	2,	2,	3,	3,	7,	7,	7,	8,	8,	8,	8,	"Hirajoshi           "
        SCALE_ENTRY     0,	3,	3,	3,	4,	6,	6,	7,	9,	9,	10,	10,	"Hungarian Major     "
	;; 51..60                                                                                                        <------------------>
        SCALE_ENTRY     0,	2,	2,	3,	3,	6,	6,	7,	8,	8,	11,	11,	"Hungarian Gypsy     "
        SCALE_ENTRY     0,	1,	1,	4,	4,	5,	7,	7,	8,	8,	11,	11,	"Hungarian G. Persian"
        SCALE_ENTRY     0,	2,	2,	3,	3,	6,	6,	7,	8,	8,	11,	11,	"Hungarian Minor     "
        SCALE_ENTRY     0,	1,	1,	1,	5,	5,	7,	7,	8,	8,	8,	8,	"Japanese (A)        "
        SCALE_ENTRY     0,	2,	2,	2,	5,	5,	7,	7,	8,	8,	8,	8,	"Japanese (B)        "
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	6,	7,	9,	9,	11,	11,	"Japan. (Ichikosucho)"
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	6,	7,	9,	9,	10,	11,	"Japan. (Taishikicho)"
        SCALE_ENTRY     0,	1,	1,	3,	3,	5,	7,	7,	9,	9,	10,	10,	"Javanese            "
        SCALE_ENTRY     0,	1,	2,	3,	3,	5,	7,	7,	9,	9,	10,	10,	"Jewish(AdonaiMalakh)"
        SCALE_ENTRY     0,	1,	1,	4,	4,	5,	7,	7,	8,	8,	10,	10,	"Jewish (Ahaba Rabba)"
 	;; 61..70                                                                                                        <------------------>
        SCALE_ENTRY     0,	1,	1,	3,	4,	6,	6,	8,	8,	10,	10,	11,	"Jewish (Magen Abot) "
        SCALE_ENTRY     0,	2,	2,	3,	3,	7,	7,	7,	9,	9,	9,	9,	"Kumoi               "
        SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	8,	8,	10,	10,	11,	"Leading Whole Tone  "
        SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	8,	8,	9,	11,	11,	"Lydian Augmented    "
        SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	7,	8,	8,	10,	10,	"Lydian Minor        "
        SCALE_ENTRY     0,	2,	2,	3,	3,	6,	6,	7,	9,	9,	11,	11,	"Lydian Diminished   "
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	6,	6,	8,	8,	10,	10,	"Major Locrian       "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	8,	8,	11,	11,	"Mohammedan          "
        SCALE_ENTRY     0,	1,	1,	3,	3,	5,	7,	7,	8,	8,	11,	11,	"Neopolitan          "
        SCALE_ENTRY     0,	1,	1,	3,	3,	5,	7,	7,	9,	9,	11,	11,	"Neoploitan Major    "
	;; 71..80                                                                                                        <------------------>
        SCALE_ENTRY     0,	1,	1,	3,	3,	5,	7,	7,	8,	8,	10,	10,	"Neopolitan Minor    "
        SCALE_ENTRY     0,	2,	2,	3,	4,	6,	6,	7,	8,	9,	11,	11,	"Nine Tone Scale     "
        SCALE_ENTRY     0,	1,	1,	4,	4,	5,	6,	6,	8,	8,	10,	10,	"Oriental (A)        "
        SCALE_ENTRY     0,	1,	1,	4,	4,	5,	6,	6,	9,	9,	10,	10,	"Oriental (B)        "
        SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	7,	9,	9,	10,	10,	"Overtone            "
        SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	7,	9,	9,	10,	10,	"Overtone Dominant   "
        SCALE_ENTRY     0,	1,	1,	3,	3,	7,	7,	7,	8,	8,	8,	8,	"Pelog               "
        SCALE_ENTRY     0,	1,	1,	4,	4,	5,	6,	6,	8,	8,	11,	11,	"Persian             "
        SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	6,	9,	9,	10,	10,	"Prometheus          "
        SCALE_ENTRY     0,	1,	1,	4,	4,	6,	6,	6,	9,	9,	10,	10,	"PrometheusNeopolitan"
	;; 81..90                                                                                                        <------------------>
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	8,	8,	10,	10,	"Pure Minor          "
        SCALE_ENTRY     0,	1,	1,	4,	4,	6,	6,	7,	8,	8,	11,	11,	"Purvi Theta         "
        SCALE_ENTRY     0,	2,	2,	3,	3,	6,	6,	7,	9,	9,	10,	10,	"Roumanian Minor     "
        SCALE_ENTRY     0,	1,	1,	4,	4,	5,	8,	8,	8,	9,	9,	9,	"Six Tone Symmetrical"
        SCALE_ENTRY     0,	1,	1,	4,	4,	5,	7,	7,	8,	8,	10,	10,	"Spanish Gypsy       "
        SCALE_ENTRY     0,	1,	1,	3,	4,	6,	6,	8,	8,	10,	10,	10,	"Super Locrian       "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	8,	8,	10,	10,	"Theta, Asavari      "
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	9,	9,	11,	11,	"Theta, Bilaval      "
        SCALE_ENTRY     0,	1,	1,	4,	4,	5,	7,	7,	8,	8,	11,	11,	"Theta, Bhairav      "
        SCALE_ENTRY     0,	1,	1,	3,	3,	5,	7,	7,	8,	8,	10,	10,	"Theta, Bhairavi     "
	;; 91..100                                                                                                        <------------------>
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	9,	9,	10,	10,	"Theta, Kafi         "
        SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	7,	9,	9,	11,	11,	"Theta, Kalyan       "
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	9,	9,	10,	10,	"Theta, Khamaj       "
        SCALE_ENTRY     0,	1,	1,	4,	4,	6,	6,	7,	9,	9,	11,	11,	"Theta, Marva        "
        SCALE_ENTRY     0,	1,	1,	3,	3,	6,	6,	7,	8,	8,	11,	11,	"Todi Theta          "
        SCALE_ENTRY     0,	1,	2,	2,	5,	5,	7,	7,	8,	9,	9,	9,	"M. Bhavapriya 44    "
        SCALE_ENTRY     0,	1,	1,	4,	4,	5,	7,	7,	9,	9,	10,	10,	"M. Chakravakam 16   "
        SCALE_ENTRY     0,	3,	3,	3,	4,	5,	7,	7,	10,	10,	10,	11,	"M. Chalanata 36     "
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	8,	8,	10,	10,	"M. Charukesi 26     "
        SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	7,	10,	10,	10,	11,	"M. Chitrambari 66   "
 	;; 101..110                                                                                                        <------------------>
        SCALE_ENTRY     0,	2,	2,	3,	3,	6,	6,	7,	9,	9,	11,	11,	"M. Dharmavati 59    "
        SCALE_ENTRY     0,	3,	3,	3,	4,	6,	6,	7,	8,	8,	11,	11,	"M. Dhatuvardhani 69 "
        SCALE_ENTRY     0,	1,	1,	4,	4,	6,	6,	7,	8,	9,	9,	9,	"M. Dhavalambari 49  "
        SCALE_ENTRY     0,	1,	1,	3,	3,	5,	7,	7,	8,	8,	11,	11,	"M. Dhenuka 9        "
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	9,	9,	11,	11,	"M. Dhirasankarabhara"
        SCALE_ENTRY     0,	1,	1,	3,	3,	6,	6,	7,	10,	10,	10,	11,	"M. Divyamani 48     "
        SCALE_ENTRY     0,	1,	1,	4,	4,	6,	6,	7,	9,	9,	11,	11,	"M. Gamanasrama 53   "
        SCALE_ENTRY     0,	1,	2,	2,	5,	5,	7,	7,	8,	8,	11,	11,	"M. Ganamurti 3      "
        SCALE_ENTRY     0,	3,	3,	3,	4,	5,	7,	7,	8,	8,	11,	11,	"M. Gangeyabhusani 33"
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	9,	9,	11,	11,	"M. Gaurimanohari 23 "
 	;; 111..120                                                                                                        <------------------>
        SCALE_ENTRY     0,	1,	1,	3,	3,	6,	6,	7,	8,	9,	9,	9,	"M. Gavambodhi 43    "
        SCALE_ENTRY     0,	1,	1,	4,	4,	5,	7,	7,	8,	9,	9,	9,	"M. Gayakapriya 13   "
        SCALE_ENTRY     0,	1,	1,	3,	3,	5,	7,	7,	8,	8,	10,	10,	"M. Hanumattodi 8    "
        SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	9,	9,	10,	10,	"M. Harikambhoji 28  "
        SCALE_ENTRY     0,	1,	1,	4,	4,	5,	7,	7,	10,	10,	10,	11,	"M. Hatakambari 18   "
        SCALE_ENTRY     0,	2,	2,	3,	3,	6,	6,	7,	9,	9,	10,	10,	"M. Hemavati 58      "
        SCALE_ENTRY     0,	1,	2,	2,	6,	6,	6,	7,	8,	8,	10,	10,	"M. Jalarnavam 38    "
        SCALE_ENTRY     0,	1,	2,	2,	6,	6,	6,	7,	9,	9,	10,	10,	"M. Jhalavarali 39   "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	9,	9,	10,	10,	"M. Jhankaradhvani 19"
        SCALE_ENTRY     0,	3,	3,	3,	4,	6,	6,	7,	8,	8,	10,	10,	"M. Jyotisvarupini 68"
	;; 121..127                                                                                                        <------------------>
        SCALE_ENTRY     0,	1,	1,	4,	4,	6,	6,	7,	8,	8,	11,	11,	"M. Kamavardhani 51  "
        SCALE_ENTRY     0,	1,	2,	2,	5,	5,	7,	7,	8,	9,	9,	9,	"M. Kanakangi 1      "
        SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	7,	8,	9,	9,	9,	"M. Kantamani 61     "
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	9,	9,	10,	10,	"M. Kharaharapriya 22"
        SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	8,	8,	11,	11,	"M. Kiravani 21      "
        SCALE_ENTRY     0,	1,	1,	3,	3,	5,	7,	7,	9,	9,	11,	11,	"M. Kokilapriya 11   "
        SCALE_ENTRY     0,	3,	3,	3,	4,	6,	6,	7,	9,	9,	11,	11,	"M. Kosalam 71       "
	SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	7,	8,	8,	11,	11,	"M. Latangi 63       "

	;; 129....                                                                                                         <------------------>
	;; IF YOU WISH TO USE THESE SCALES, YOU WILL NEED TO SWAP ONE OF THESE ENTRIES WITH ONE ABOVE - SEQv3 SUPPORTS MAX 128 SCALES
	;;
  ;;    SCALE_ENTRY     0,	1,	2,	2,	5,	5,	7,	7,	9,	9,	11,	11,	"M. Manavati 5       "
  ;;    SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	8,	9,	9,	9,	"M. Mararanjani 25   "
  ;;    SCALE_ENTRY     0,	1,	1,	4,	4,	5,	7,	7,	8,	9,	9,	9,	"M. Mayamalavagaula 1"
  ;;    SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	7,	9,	9,	11,	11,	"M. Mechakalyani 65  "
  ;;    SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	10,	10,	10,	11,	"M. Naganandini 30   "
  ;;    SCALE_ENTRY     0,	1,	1,	4,	4,	6,	6,	7,	8,	8,	10,	10,	"M. Namanarayani 50  "
  ;;    SCALE_ENTRY     0,	3,	3,	3,	4,	6,	6,	7,	9,	9,	10,	10,	"M. Nasikabhusani 70 "
  ;;    SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	8,	8,	10,	10,	"M. Natabhairavi 20  "
  ;;    SCALE_ENTRY     0,	1,	1,	3,	3,	5,	7,	7,	9,	9,	10,	10,	"M. Natakapriya 10   "
  ;;    SCALE_ENTRY     0,	1,	2,	2,	6,	6,	6,	7,	9,	9,	10,	10,	"M. Navanitam 40     "
  ;;    SCALE_ENTRY     0,	2,	2,	3,	3,	6,	6,	7,	10,	10,	10,	11,	"M. Nitimati 60      "
  ;;    SCALE_ENTRY     0,	1,	2,	2,	6,	6,	6,	7,	9,	9,	11,	11,	"M. Pavani 41        "
  ;;    SCALE_ENTRY     0,	3,	3,	3,	4,	5,	7,	7,	8,	8,	10,	10,	"M. Ragavardhani 32  "
  ;;    SCALE_ENTRY     0,	1,	2,	2,	6,	6,	6,	7,	10,	10,	10,	11,	"M. Raghupriya 42    "
  ;;    SCALE_ENTRY     0,	1,	1,	4,	4,	6,	6,	7,	9,	9,	10,	10,	"M. Ramapriya 52     "
  ;;    SCALE_ENTRY     0,	3,	3,	3,	4,	6,	6,	7,	10,	10,	10,	11,	"M. Rasikapriya 72   "
  ;;    SCALE_ENTRY     0,	1,	2,	2,	5,	5,	7,	7,	8,	8,	10,	10,	"M. Ratnangi 2       "
  ;;    SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	7,	8,	8,	10,	10,	"M. Risabhapriya 62  "
  ;;    SCALE_ENTRY     0,	1,	1,	3,	3,	5,	7,	7,	10,	10,	10,	11,	"M. Rupavati 12      "
  ;;    SCALE_ENTRY     0,	1,	1,	3,	3,	6,	6,	7,	9,	9,	10,	10,	"M. Sadvidhamargini 4"
  ;;    SCALE_ENTRY     0,	1,	2,	2,	6,	6,	6,	7,	8,	9,	9,	9,	"M. Salagam 37       "
  ;;    SCALE_ENTRY     0,	2,	2,	3,	3,	6,	6,	7,	8,	8,	10,	10,	"M. Sanmukhapriya 56 "
  ;;    SCALE_ENTRY     0,	2,	2,	4,	4,	5,	7,	7,	8,	8,	11,	11,	"M. Sarasangi 27     "
  ;;    SCALE_ENTRY     0,	1,	1,	3,	3,	5,	7,	7,	8,	9,	9,	9,	"M. Senavati 7       "
  ;;    SCALE_ENTRY     0,	2,	2,	3,	3,	6,	6,	7,	8,	8,	11,	11,	"M. Simhendramadhyama"
  ;;    SCALE_ENTRY     0,	1,	1,	3,	3,	6,	6,	7,	8,	8,	11,	11,	"M. Subhapantuvarali "
  ;;    SCALE_ENTRY     0,	3,	3,	3,	4,	6,	6,	7,	8,	9,	9,	9,	"M. Sucharitra 67    "
  ;;    SCALE_ENTRY     0,	3,	3,	3,	4,	5,	7,	7,	9,	9,	11,	11,	"M. Sulini 35        "
  ;;    SCALE_ENTRY     0,	1,	1,	4,	4,	5,	7,	7,	9,	9,	11,	11,	"M. Suryakantam 17   "
  ;;    SCALE_ENTRY     0,	1,	2,	2,	6,	6,	6,	7,	9,	9,	11,	11,	"M. Suvarnangi 47    "
  ;;    SCALE_ENTRY     0,	2,	2,	3,	3,	6,	6,	7,	8,	9,	9,	9,	"M. Syamalangi 55    "
  ;;    SCALE_ENTRY     0,	1,	2,	2,	5,	5,	7,	7,	10,	10,	10,	11,	"M. Tanarupi 6       "
  ;;    SCALE_ENTRY     0,	2,	2,	4,	4,	6,	6,	7,	9,	9,	10,	10,	"M. Vaschaspati 64   "
  ;;    SCALE_ENTRY     0,	3,	3,	3,	4,	5,	7,	7,	9,	9,	10,	10,	"M. Vagadhisvari 34  "
  ;;    SCALE_ENTRY     0,	3,	3,	3,	4,	5,	7,	7,	8,	8,	10,	10,	"M. Vakulabharanam 14"
  ;;    SCALE_ENTRY     0,	1,	2,	2,	5,	5,	7,	7,	9,	9,	10,	10,	"M. Vanaspati 4      "
  ;;    SCALE_ENTRY     0,	2,	2,	3,	3,	5,	7,	7,	10,	10,	10,	11,	"M. Varunapriya 24   "
  ;;    SCALE_ENTRY     0,	3,	3,	3,	4,	6,	6,	7,	10,	10,	10,	11,	"M. Visvambari 54    "
  ;;    SCALE_ENTRY     0,	3,	3,	3,	4,	5,	7,	7,	8,	9,	9,	9,	"M. Yagapriya 31     "

SEQ_SCALE_TABLE_End

	;; number of scales: used in cs_m_trkmode.inc and seq_midi.inc
SEQ_SCALE_NUMBER	EQU	(SEQ_SCALE_TABLE_End-SEQ_SCALE_TABLE) / (6+20)

;; --------------------------------------------------------------------------
;;  this function is called from SEQ_CORE if a note value should be forced 
;;  to the globally selected scale
;;  IN: SEQ_GLOBAL_SCALE_CTRL[3:0]: the scale mode
;;      SEQ_EVNT1: the note value
;;      FSR0: pointer to track record
;;      FSR2: pointer to track variables
;;  OUT: modified note value in SEQ_EVNT1
;;  USES: FSR1, TMP[123]
;; --------------------------------------------------------------------------
SEQ_SCALE_Note
	SET_BSR	SEQ_BASE

	;; exit if EVNT0 is zero
	movf	SEQ_EVNT0, W, BANKED
	bz	SEQ_SCALE_Note_End

	;; exit if force scale flag not selected
	movlw	SEQ_TRKMODEx
	BRA_IFCLR PLUSW0, SEQ_TRKMODE_FLAG_FORCE_SCALE, ACCESS, SEQ_SCALE_Note_End

	;; store root note in TMP3
	;; if GetRoot returns 0, take keyboard controlled root note
	rcall	SEQ_SCALE_GetRoot
	skpnz
	incf	SEQ_GLOBAL_SCALE_KEYB_ROOT, W, BANKED
	addlw	-1
	movwf	TMP3

	;; normalize and count the octaves
	subwf	SEQ_EVNT1, W, BANKED	; note - root note -> TMP1
	btfsc	WREG, 7; ensure that note >= 0
	addlw 12	
	movwf	TMP1	; normalized note value will be in TMP1
	clrf	TMP2	; octave will be in TMP2
SEQ_SCALE_Note_Loop
	movlw	12-1
	cpfsgt	TMP1, ACCESS
	rgoto SEQ_SCALE_Note_Loop_Break
	incf	TMP2, F
	movlw	-12
	addwf	TMP1, F
	rgoto	SEQ_SCALE_Note_Loop
SEQ_SCALE_Note_Loop_Break

	;; get scaled value from table
	rcall	SEQ_SCALE_GetScale
	mullw	12/2+20
	clrc
	rrf	TMP1, W
	addwf	PRODL, F

	movlw	LOW(SEQ_SCALE_TABLE)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(SEQ_SCALE_TABLE)
	addwfc	PRODH, W
	movwf	TBLPTRH

	tblrd*+
	movf	TABLAT, W
	btfsc	TMP1, 0
	swapf	TABLAT, W
	andlw	0x0f
	movwf	TMP1

	;; multiply octave
	movf	TMP2, W
	mullw	12
	movf	PRODL, W
	addwf	TMP1, W

	;; add root note
	addwf	TMP3, W

	;; decrement octave so long note >= 0x80
SEQ_SCALE_Note_Loop2
	BRA_IFCLR WREG, 7, ACCESS, SEQ_SCALE_Note_Loop2_Break
	addlw	-12
	rgoto	SEQ_SCALE_Note_Loop2
SEQ_SCALE_Note_Loop2_Break

	;; store value in SEQ_EVNT1
	movwf	SEQ_EVNT1, BANKED
	
SEQ_SCALE_Note_End
	return
	

;; --------------------------------------------------------------------------
;;  this function prints the name of a scale
;;  IN: scale number in WREG
;;  OUT: 20 characters
;;  USES: FSR1
;; --------------------------------------------------------------------------
SEQ_SCALE_Prn
	TABLE_ADDR_MUL_W SEQ_SCALE_TABLE, 12/2 + 20
	movlw	12/2
	addwf	TBLPTRL, F
	movlw	0
	addwfc	TBLPTRH, F

	movlw	20
	goto	MIOS_LCD_PrintPreconfString


;; --------------------------------------------------------------------------
;;  This function returns a pointer to the current active scale
;;  IN: scale control mode (0=global, 1-4: group 1-4) in SEQ_GLOBAL_SCALE_CTRL[3:0]
;;  OUT: pointer to scale in FSR1
;;  USES: FSR1
;; --------------------------------------------------------------------------
SEQ_SCALE_GetScalePtr
	SET_BSR	SEQ_BASE
	movf	SEQ_GLOBAL_SCALE_CTRL, W, BANKED
	andlw	0x0f
	bnz	SEQ_SCALE_GetScalePtr_Group

SEQ_SCALE_GetScalePtr_Global			; use global scale
	lfsr	FSR1, SEQ_GLOBAL_SCALE
	return

SEQ_SCALE_GetScalePtr_Group			; use group based scale
	lfsr	FSR1, SEQ_TRK2 + SEQ_TRKSCALEx	; stored in third track, first byte
	addlw	-1
	mullw	4*SEQ_TRKRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	return

;; --------------------------------------------------------------------------
;;  This function returns the scale
;;  IN: -
;;  OUT: scale in WREG
;;  USES: FSR1
;; --------------------------------------------------------------------------
SEQ_SCALE_GetScale
	rcall	SEQ_SCALE_GetScalePtr
	movf	INDF1, W
	return

;; --------------------------------------------------------------------------
;;  This function returns a pointer to the current active root note
;;  IN: scale control mode (0=global, 1-4: group 1-4) in SEQ_GLOBAL_SCALE_CTRL[3:0]
;;  OUT: pointer to root note in FSR1
;;       the note is located in bitfield [7:4] !!!
;;  USES: FSR1
;; --------------------------------------------------------------------------
SEQ_SCALE_GetRootPtr
	SET_BSR	SEQ_BASE
	movf	SEQ_GLOBAL_SCALE_CTRL, W, BANKED
	andlw	0x0f
	bnz	SEQ_SCALE_GetRootPtr_Group

SEQ_SCALE_GetRootPtr_Global			; use global scale
	lfsr	FSR1, SEQ_GLOBAL_SCALE_CTRL
	return

SEQ_SCALE_GetRootPtr_Group			; use group based scale
	lfsr	FSR1, SEQ_TRK3 + SEQ_TRKSCALEROOTx	; stored in fourth track, first byte
	addlw	-1
	mullw	4*SEQ_TRKRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	return

;; --------------------------------------------------------------------------
;;  This function returns the scale
;;  IN: -
;;  OUT: scale in WREG
;;  USES: FSR1
;; --------------------------------------------------------------------------
SEQ_SCALE_GetRoot
	rcall	SEQ_SCALE_GetRootPtr
	swapf	INDF1, W
	andlw	0x0f
	return

