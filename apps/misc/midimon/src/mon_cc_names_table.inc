; $Id$
;
; MIDImon CC controller names
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This table contains names for all 128 CC controller numbers
;;  based on the GM standard
;;  Use MON_PrintCtrlName for printing
;; --------------------------------------------------------------------------
MON_CC_NAMES_TABLE
	;; 0x00-0x0f
	      ;;+01234567+
	db	"Bank MSB"
	db	"ModWheel"
	db	"Breath  "
	db	"Ctrl.  3"
	db	"FootCtrl"
	db	"PortTime"
	db	"Data MSB"
	db	"Volume  "
	db	"Balance "
	db	"Ctrl.  9"
	db	"Panorama"
	db	"Expr.   "
	db	"Fx#1 MSB"
	db	"Fx#2 MSB"
	db	"Ctrl. 14"
	db	"Ctrl. 15"

	;; 0x10-0x0f
	      ;;+01234567+
	db	"Ctrl. 16"
	db	"Ctrl. 17"
	db	"Ctrl. 18"
	db	"Ctrl. 19"
	db	"Ctrl. 20"
	db	"Ctrl. 21"
	db	"Ctrl. 22"
	db	"Ctrl. 23"
	db	"Ctrl. 24"
	db	"Ctrl. 25"
	db	"Ctrl. 26"
	db	"Ctrl. 27"
	db	"Ctrl. 28"
	db	"Ctrl. 29"
	db	"Ctrl. 30"
	db	"Ctrl. 31"

	;; 0x20-0x2f
	      ;;+01234567+
	db	"Bank LSB"
	db	"Mod  LSB"
	db	"BreatLSB"
	db	"Ctrl. 35"
	db	"Foot.LSB"
	db	"PortaLSB"
	db	"Data LSB"
	db	"Vol. LSB"
	db	"Bal. LSB"
	db	"Ctrl. 41"
	db	"Pan. LSB"
	db	"Expr.LSB"
	db	"Fx#1 LSB"
	db	"Fx#2 LSB"
	db	"Ctrl. 46"
	db	"Ctrl. 47"

	;; 0x30-0x3f
	      ;;+01234567+
	db	"Ctrl. 48"
	db	"Ctrl. 49"
	db	"Ctrl. 50"
	db	"Ctrl. 51"
	db	"Ctrl. 52"
	db	"Ctrl. 53"
	db	"Ctrl. 54"
	db	"Ctrl. 55"
	db	"Ctrl. 56"
	db	"Ctrl. 57"
	db	"Ctrl. 58"
	db	"Ctrl. 59"
	db	"Ctrl. 60"
	db	"Ctrl. 61"
	db	"Ctrl. 62"
	db	"Ctrl. 63"

	;; 0x40-0x4f
	      ;;+01234567+
	db	"Sustain "
	db	"Port. ON"
	db	"Sustenu."
	db	"SoftPed."
	db	"LegatoSw"
	db	"Hold2   "
	db	"Ctrl. 70"
	db	"Harmonic"
	db	"Release "
	db	"Attack  "
	db	"CutOff  "
	db	"Ctrl. 75"
	db	"Ctrl. 76"
	db	"Ctrl. 77"
	db	"Ctrl. 78"
	db	"Ctrl. 79"

	;; 0x50-0x5f
	      ;;+01234567+
	db	"Ctrl. 80"
	db	"Ctrl. 81"
	db	"Ctrl. 82"
	db	"Ctrl. 83"
	db	"Ctrl. 84"
	db	"Ctrl. 85"
	db	"Ctrl. 86"
	db	"Ctrl. 87"
	db	"Ctrl. 88"
	db	"Ctrl. 89"
	db	"Ctrl. 90"
	db	"Reverb  "
	db	"Tremolo "
	db	"Chorus  "
	db	"Celeste "
	db	"Phaser  "

	;; 0x60-0x6f
	      ;;+01234567+
	db	"Data Inc"
	db	"Data Dec"
	db	"NRPN LSB"
	db	"NRPN MSB"
	db	"RPN LSB "
	db	"RPN MSB "
	db	"Ctrl.102"
	db	"Ctrl.103"
	db	"Ctrl.104"
	db	"Ctrl.105"
	db	"Ctrl.106"
	db	"Ctrl.107"
	db	"Ctrl.108"
	db	"Ctrl.109"
	db	"Ctrl.110"
	db	"Ctrl.111"

	;; 0x70-0x7f
	      ;;+01234567+
	db	"Ctrl.112"
	db	"Ctrl.113"
	db	"Ctrl.114"
	db	"Ctrl.115"
	db	"Ctrl.116"
	db	"Ctrl.117"
	db	"Ctrl.118"
	db	"Ctrl.119"
	db	"SoundOff"
	db	"ResetAll"	
	db	"Local   "
	db	"NotesOff"
	db	"Omni Off"
	db	"Omni On "
	db	"Mono On "
	db	"Poly On "
