; $Id$
;
; MIDIbox SID Drum Models
;
; Important: this file must be located in upper 64k area!
;            (in most routines it is assumed, that TBLPTRU == 0x01)
; this is checked here
	IF ($ & 0xff0000) != 0x010000
	ERROR	"sid_dmodel.inc must be located in upper 64k area!"
	ENDIF
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; drum model types (defines the data structure)
SID_DMODEL_WT		EQU	0	; wavetable based model.    PAR1: GL offset, PAR2: Speed offset

;; --------------------------------------------------------------------------
;;  Linked list to drum models
;; --------------------------------------------------------------------------
SID_DMODEL_TABLE
	dw	(SID_DMODEL_WT_BD1_TAB) & 0xffff	; model 1: WT Bass Drum #1
	dw	(SID_DMODEL_WT_BD2_TAB) & 0xffff	; model 2: WT Bass Drum #2
	dw	(SID_DMODEL_WT_BD3_TAB) & 0xffff	; model 3: WT Bass Drum #3
	dw	(SID_DMODEL_WT_SD1_TAB) & 0xffff 	; model 4: WT Snare Drum #1
	dw	(SID_DMODEL_WT_SD2_TAB) & 0xffff 	; model 5: WT Snare Drum #2
	dw	(SID_DMODEL_WT_SD3_TAB) & 0xffff 	; model 6: WT Snare Drum #3
	dw	(SID_DMODEL_WT_HH1_TAB) & 0xffff 	; model 7: WT HiHat #1
	dw	(SID_DMODEL_WT_HH2_TAB) & 0xffff 	; model 8: WT HiHat #2
	dw	(SID_DMODEL_WT_TOM1_TAB) & 0xffff 	; model 9: WT Tom #1
	dw	(SID_DMODEL_WT_TOM2_TAB) & 0xffff 	; model 10: WT Tom #2
	dw	(SID_DMODEL_WT_CLAP_TAB) & 0xffff 	; model 11: WT Clap
	dw	(SID_DMODEL_WT_FX1_TAB) & 0xffff 	; model 12: WT FX #1
	dw	(SID_DMODEL_WT_FX2_TAB) & 0xffff 	; model 13: WT FX #2
	dw	(SID_DMODEL_WT_FX3_TAB) & 0xffff 	; model 14: WT FX #3
	dw	(SID_DMODEL_WT_FX4_TAB) & 0xffff 	; model 15: WT FX #4
	dw	(SID_DMODEL_WT_FX5_TAB) & 0xffff 	; model 16: WT FX #5
	dw	(SID_DMODEL_WT_FX6_TAB) & 0xffff 	; model 16: WT FX #6
	dw	(SID_DMODEL_WT_FX7_TAB) & 0xffff 	; model 16: WT FX #7
	dw	(SID_DMODEL_WT_FX8_TAB) & 0xffff 	; model 16: WT FX #8
	dw	(SID_DMODEL_WT_FX9_TAB) & 0xffff 	; model 16: WT FX #9
SID_DMODEL_TABLE_END

	;; calculate number of available models - used at various places
SID_DMODEL_NUM	EQU	(SID_DMODEL_TABLE_END-SID_DMODEL_TABLE) / 2
	

;; --------------------------------------------------------------------------
;;  Wavetable Bass Drum #1
;; --------------------------------------------------------------------------
SID_DMODEL_WT_BD1_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"BD1 ", SID_DMODEL_WT, 0x3c, 0x20, 0x01, 0x80, 0x0a, 0x07, 0xff
	db	0x3c | 0x80, 0x01	; Note/Wave Step #1
	db	0x3a | 0x00, 0x01	; Note/Wave Step #1
	db	0x38 | 0x00, 0x01	; Note/Wave Step #2
	db	0x34 | 0x00, 0x01	; Note/Wave Step #3
	db	0x30 | 0x00, 0x01	; Note/Wave Step #4
	db	0x2c | 0x00, 0x01	; Note/Wave Step #5
	db	0x28 | 0x00, 0x01	; Note/Wave Step #6
	db	0x20 | 0x00, 0x01	; Note/Wave Step #7

;; --------------------------------------------------------------------------
;;  Wavetable Bass Drum #2
;; --------------------------------------------------------------------------
SID_DMODEL_WT_BD2_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"BD2 ", SID_DMODEL_WT, 0x6b, 0x20, 0x08, 0x80, 0x0a, 0x04, 0xff
	db	0x6b | 0x80, 0x08	; Note/Wave Step #0
	db	0x3c | 0x00, 0x04	; Note/Wave Step #1
	db	0x37 | 0x00, 0x04	; Note/Wave Step #2
	db	0x32 | 0x00, 0x04	; Note/Wave Step #3
	db	0x2a | 0x00, 0x04	; Note/Wave Step #4

;; --------------------------------------------------------------------------
;;  Wavetable Bass Drum #3
;; --------------------------------------------------------------------------
SID_DMODEL_WT_BD3_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"BD3 ", SID_DMODEL_WT, 0x48, 0x20, 0x01, 0x80, 0x0a, 0x07, 0xff
	db	0x48 | 0x80, 0x01	; Note/Wave Step #0
	db	0x4e | 0x00, 0x08	; Note/Wave Step #1
	db	0x45 | 0x80, 0x01	; Note/Wave Step #2
	db	0x35 | 0x80, 0x04	; Note/Wave Step #3
	db	0x4e | 0x00, 0x08	; Note/Wave Step #4
	db	0x40 | 0x00, 0x08	; Note/Wave Step #5
	db	0x47 | 0x00, 0x08	; Note/Wave Step #6
	db	0x3b | 0x00, 0x08	; Note/Wave Step #7

;; --------------------------------------------------------------------------
;;  Wavetable Snare Drum #1
;; --------------------------------------------------------------------------
SID_DMODEL_WT_SD1_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"SD1 ", SID_DMODEL_WT, 0x6b, 0x20, 0x08, 0x80, 0x0a, 0x04, 0xff
	db	0x6b | 0x00, 0x08	; Note/Wave Step #0
	db	0x3c | 0x80, 0x04	; Note/Wave Step #1
	db	0x6b | 0x00, 0x08	; Note/Wave Step #2
	db	0x3b | 0x80, 0x04	; Note/Wave Step #3
	db	0x6b | 0x00, 0x08	; Note/Wave Step #4

;; --------------------------------------------------------------------------
;;  Wavetable Snare Drum #2
;; --------------------------------------------------------------------------
SID_DMODEL_WT_SD2_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"SD2 ", SID_DMODEL_WT, 0x50, 0x20, 0x08, 0x80, 0x0a, 0x0a, 0xff
	db	0x50 | 0x00, 0x08	; Note/Wave Step #0
	db	0x3c | 0x80, 0x04	; Note/Wave Step #1
	db	0x3a | 0x80, 0x04	; Note/Wave Step #2
	db	0x50 | 0x00, 0x08	; Note/Wave Step #3
	db	0x51 | 0x00, 0x08	; Note/Wave Step #4
	db	0x52 | 0x00, 0x08	; Note/Wave Step #5
	db	0x53 | 0x00, 0x08	; Note/Wave Step #6
	db	0x54 | 0x00, 0x08	; Note/Wave Step #7
	db	0x55 | 0x00, 0x08	; Note/Wave Step #8
	db	0x56 | 0x00, 0x08	; Note/Wave Step #9
	db	0x57 | 0x00, 0x08	; Note/Wave Step #10

;; --------------------------------------------------------------------------
;;  Wavetable Snare Drum #3
;; --------------------------------------------------------------------------
SID_DMODEL_WT_SD3_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"SD3 ", SID_DMODEL_WT, 0x4b, 0x20, 0x08, 0x80, 0x0a, 0x01, 0x00
	db	0x4b | 0x00, 0x08	; Note/Wave Step #0
	db	0x3c | 0x80, 0x01	; Note/Wave Step #1

;; --------------------------------------------------------------------------
;;  Wavetable HiHat #1
;; --------------------------------------------------------------------------
SID_DMODEL_WT_HH1_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"HH1 ", SID_DMODEL_WT, 0x6c, 0x20, 0x08, 0x80, 0x0a, 0x00, 0xff
	db	0x6c | 0x80, 0x08	; Note/Wave Step #0

;; --------------------------------------------------------------------------
;;  Wavetable HiHat #2
;; --------------------------------------------------------------------------
SID_DMODEL_WT_HH2_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"HH2 ", SID_DMODEL_WT, 0x6c, 0x20, 0x08, 0x80, 0x0a, 0x02, 0xff
	db	0x6c | 0x80, 0x08	; Note/Wave Step #0
	db	0x33 | 0x00, 0x04	; Note/Wave Step #1
	db	0x6c | 0x80, 0x08	; Note/Wave Step #2

;; --------------------------------------------------------------------------
;;  Wavetable Tom #1
;; --------------------------------------------------------------------------
SID_DMODEL_WT_TOM1_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"TO1 ", SID_DMODEL_WT, 0x3c, 0x20, 0x04, 0x80, 0x0a, 0x0d, 0xff
	db	0x3c | 0x80, 0x04	; Note/Wave Step #0
	db	0x3b | 0x00, 0x04	; Note/Wave Step #1
	db	0x3a | 0x80, 0x04	; Note/Wave Step #2
	db	0x38 | 0x00, 0x04	; Note/Wave Step #3
	db	0x37 | 0x80, 0x04	; Note/Wave Step #4
	db	0x34 | 0x00, 0x04	; Note/Wave Step #5
	db	0x33 | 0x80, 0x04	; Note/Wave Step #6
	db	0x31 | 0x00, 0x04	; Note/Wave Step #7
	db	0x2f | 0x80, 0x04	; Note/Wave Step #8
	db	0x2d | 0x00, 0x04	; Note/Wave Step #9
	db	0x2a | 0x80, 0x04	; Note/Wave Step #10
	db	0x26 | 0x00, 0x04	; Note/Wave Step #11
	db	0x22 | 0x80, 0x04	; Note/Wave Step #12
	db	0x1c | 0x00, 0x04	; Note/Wave Step #13
	db	0x13 | 0x80, 0x04	; Note/Wave Step #14

;; --------------------------------------------------------------------------
;;  Wavetable Tom #2 (MEMO: try high tune and fast speed - sounds cool!)
;; --------------------------------------------------------------------------
SID_DMODEL_WT_TOM2_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"TO2 ", SID_DMODEL_WT, 0x3c, 0x20, 0x04, 0x80, 0x0a, 0x0d, 0x0c
	db	0x3c | 0x80, 0x04	; Note/Wave Step #0
	db	0x3b | 0x00, 0x01	; Note/Wave Step #1
	db	0x3a | 0x80, 0x04	; Note/Wave Step #2
	db	0x38 | 0x00, 0x01	; Note/Wave Step #3
	db	0x37 | 0x80, 0x04	; Note/Wave Step #4
	db	0x34 | 0x00, 0x01	; Note/Wave Step #5
	db	0x33 | 0x80, 0x04	; Note/Wave Step #6
	db	0x31 | 0x00, 0x01	; Note/Wave Step #7
	db	0x2f | 0x80, 0x04	; Note/Wave Step #8
	db	0x2d | 0x00, 0x01	; Note/Wave Step #9
	db	0x2a | 0x80, 0x04	; Note/Wave Step #10
	db	0x26 | 0x00, 0x01	; Note/Wave Step #11
	db	0x22 | 0x80, 0x04	; Note/Wave Step #12
	db	0x1c | 0x00, 0x01	; Note/Wave Step #13
	db	0x13 | 0x80, 0x04	; Note/Wave Step #14

;; --------------------------------------------------------------------------
;;  Wavetable Clap
;; --------------------------------------------------------------------------
SID_DMODEL_WT_CLAP_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"CLP ", SID_DMODEL_WT, 0x3c, 0x20, 0x08, 0x80, 0x0a, 0x04, 0x01
	db	0x3c | 0x00, 0x08	; Note/Wave Step #0
	db	0x70 | 0x80, 0x08	; Note/Wave Step #1
	db	0x30 | 0x00, 0x08	; Note/Wave Step #2
	db	0x3c | 0x80, 0x08	; Note/Wave Step #3

;; --------------------------------------------------------------------------
;;  Wavetable FX #1 (just a triangle wave - as basis for ring modulation)
;; --------------------------------------------------------------------------
SID_DMODEL_WT_FX1_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"FX1 ", SID_DMODEL_WT, 0x3c, 0x20, 0x01, 0x80, 0x0a, 0x00, 0xff
	db	0x3c | 0x80, 0x01	; Note/Wave Step #0

;; --------------------------------------------------------------------------
;;  Wavetable FX #2 (trilling triangle wave - also as basis for ring modulation)
;; --------------------------------------------------------------------------
SID_DMODEL_WT_FX2_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"FX2 ", SID_DMODEL_WT, 0x3c, 0x20, 0x01, 0x80, 0x0a, 0x03, 0x00
	db	0x3c | 0x80, 0x01	; Note/Wave Step #0
	db	0x30 | 0x00, 0x01	; Note/Wave Step #1
	db	0x3c | 0x80, 0x01	; Note/Wave Step #2
	db	0x48 | 0x00, 0x01	; Note/Wave Step #3

;; --------------------------------------------------------------------------
;;  Wavetable FX #3 (Triangle with Ringmodulation enabled)
;; --------------------------------------------------------------------------
SID_DMODEL_WT_FX3_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"FX3 ", SID_DMODEL_WT, 0x3c, 0x20, 0x41, 0x80, 0x0a, 0x00, 0xff
	db	0x3c | 0x80, 0x41	; Note/Wave Step #0

;; --------------------------------------------------------------------------
;;  Wavetable FX #4 (Triangle with Ringmodulation, frequency sweeped down)
;; --------------------------------------------------------------------------
SID_DMODEL_WT_FX4_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"FX4 ", SID_DMODEL_WT, 0x3c, 0x20, 0x41, 0x80, 0x0a, 0x0d, 0xff
	db	0x3c | 0x80, 0x41	; Note/Wave Step #0
	db	0x3b | 0x00, 0x41	; Note/Wave Step #1
	db	0x3a | 0x80, 0x41	; Note/Wave Step #2
	db	0x38 | 0x00, 0x41	; Note/Wave Step #3
	db	0x37 | 0x80, 0x41	; Note/Wave Step #4
	db	0x34 | 0x00, 0x41	; Note/Wave Step #5
	db	0x33 | 0x80, 0x41	; Note/Wave Step #6
	db	0x31 | 0x00, 0x41	; Note/Wave Step #7
	db	0x2f | 0x80, 0x41	; Note/Wave Step #8
	db	0x2d | 0x00, 0x41	; Note/Wave Step #9
	db	0x2a | 0x80, 0x41	; Note/Wave Step #10
	db	0x26 | 0x00, 0x41	; Note/Wave Step #11
	db	0x22 | 0x80, 0x41	; Note/Wave Step #12
	db	0x1c | 0x00, 0x41	; Note/Wave Step #13
	db	0x13 | 0x80, 0x41	; Note/Wave Step #14

;; --------------------------------------------------------------------------
;;  Wavetable FX #5 (Computer Effect, with high speed it sounds "Kraftwerkisch")
;; --------------------------------------------------------------------------
SID_DMODEL_WT_FX5_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"FX5 ", SID_DMODEL_WT, 0x54, 0x20, 0x04, 0x80, 0x0a, 0x0d, 0x0a
	db	0x54 | 0x80, 0x04	; Note/Wave Step #0
	db	0x3b | 0x00, 0x01	; Note/Wave Step #1
	db	0x60 | 0x80, 0x04	; Note/Wave Step #2
	db	0x38 | 0x00, 0x01	; Note/Wave Step #3
	db	0x54 | 0x80, 0x04	; Note/Wave Step #4
	db	0x34 | 0x00, 0x01	; Note/Wave Step #5
	db	0x60 | 0x80, 0x04	; Note/Wave Step #6
	db	0x31 | 0x00, 0x01	; Note/Wave Step #7
	db	0x54 | 0x80, 0x04	; Note/Wave Step #8
	db	0x2d | 0x00, 0x01	; Note/Wave Step #9
	db	0x60 | 0x80, 0x04	; Note/Wave Step #10
	db	0x26 | 0x00, 0x01	; Note/Wave Step #11
	db	0x54 | 0x80, 0x04	; Note/Wave Step #12
	db	0x1c | 0x00, 0x01	; Note/Wave Step #13
	db	0x60 | 0x80, 0x04	; Note/Wave Step #14

;; --------------------------------------------------------------------------
;;  Wavetable FX #6 (just another effect)
;; --------------------------------------------------------------------------
SID_DMODEL_WT_FX6_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"FX6 ", SID_DMODEL_WT, 0x18, 0x20, 0x01, 0x80, 0x0a, 0x05, 0x00
	db	0x18 | 0x80, 0x01	; Note/Wave Step #0
	db	0x24 | 0x00, 0x01	; Note/Wave Step #1
	db	0x30 | 0x80, 0x01	; Note/Wave Step #2
	db	0x3c | 0x00, 0x01	; Note/Wave Step #3
	db	0x48 | 0x80, 0x01	; Note/Wave Step #4
	db	0x54 | 0x00, 0x01	; Note/Wave Step #5

;; --------------------------------------------------------------------------
;;  Wavetable FX #7 (just another effect)
;; --------------------------------------------------------------------------
SID_DMODEL_WT_FX7_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"FX7 ", SID_DMODEL_WT, 0x18, 0x20, 0x02, 0x80, 0x0a, 0x04, 0x00
	db	0x18 | 0x80, 0x02	; Note/Wave Step #0
	db	0x3c | 0x00, 0x02	; Note/Wave Step #1
	db	0x24 | 0x00, 0x02	; Note/Wave Step #2
	db	0x3c | 0x00, 0x02	; Note/Wave Step #3
	db	0x30 | 0x80, 0x02	; Note/Wave Step #4

;; --------------------------------------------------------------------------
;;  Wavetable FX #8 (just another effect)
;; --------------------------------------------------------------------------
SID_DMODEL_WT_FX8_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"FX8 ", SID_DMODEL_WT, 0x70, 0x20, 0x08, 0x80, 0x0a, 0x08, 0x00
	db	0x70 | 0x80, 0x08	; Note/Wave Step #0
	db	0x68 | 0x00, 0x08	; Note/Wave Step #1
	db	0x60 | 0x00, 0x04	; Note/Wave Step #2
	db	0x58 | 0x80, 0x04	; Note/Wave Step #3
	db	0x50 | 0x00, 0x08	; Note/Wave Step #4
	db	0x48 | 0x00, 0x08	; Note/Wave Step #5
	db	0x40 | 0x80, 0x04	; Note/Wave Step #6
	db	0x38 | 0x00, 0x08	; Note/Wave Step #7
	db	0x30 | 0x00, 0x04	; Note/Wave Step #8

;; --------------------------------------------------------------------------
;;  Wavetable FX #9 (just another effect)
;; --------------------------------------------------------------------------
SID_DMODEL_WT_FX9_TAB
	;;	name    type           note  GLng  Wave   PW   Speed End   Loop
	db	"FX9 ", SID_DMODEL_WT, 0x18, 0x20, 0x04, 0x80, 0x0a, 0x11, 0x00
	db	0x18 | 0x80, 0x04	; Note/Wave Step #0
	db	0x54 | 0x00, 0x01	; Note/Wave Step #1
	db	0x24 | 0x00, 0x04	; Note/Wave Step #2
	db	0x48 | 0x80, 0x01	; Note/Wave Step #3
	db	0x30 | 0x00, 0x04	; Note/Wave Step #4
	db	0x3c | 0x00, 0x01	; Note/Wave Step #5
	db	0x3c | 0x80, 0x04	; Note/Wave Step #6
	db	0x30 | 0x00, 0x01	; Note/Wave Step #7
	db	0x48 | 0x00, 0x04	; Note/Wave Step #8
	db	0x18 | 0x80, 0x01	; Note/Wave Step #9
	db	0x54 | 0x00, 0x04	; Note/Wave Step #10
	db	0x24 | 0x00, 0x01	; Note/Wave Step #11
	db	0x48 | 0x80, 0x04	; Note/Wave Step #12
	db	0x30 | 0x00, 0x01	; Note/Wave Step #13
	db	0x3c | 0x00, 0x04	; Note/Wave Step #14
	db	0x3c | 0x80, 0x01	; Note/Wave Step #15
	db	0x30 | 0x00, 0x04	; Note/Wave Step #16
	db	0x48 | 0x00, 0x01	; Note/Wave Step #17
