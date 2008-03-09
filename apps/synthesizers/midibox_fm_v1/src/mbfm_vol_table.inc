; $Id$
;
; MIDIbox FM
; Volume/Velocity/Expression
; taken from opl3_midi.inc of the alsa package (http://www.alsa-project.org)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;; In: volume index (0x00-0x7f) in WREG
;; Out: 8*log(WREG/64)/log(2) in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MBFM_VOL_TABLE_Get
	addlw	MBFM_VOL_TABLE & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	MBFM_VOL_TABLE >> 8
	addwfc	TBLPTRH, F

	tblrd*+
	movf	TABLAT, W
	movwf	MIOS_PARAMETER1
	return

; ==========================================================================

MBFM_VOL_TABLE
	;; from the original source code (opl3_midi.c)
	;; /*
	;;  * The next table looks magical, but it certainly is not. Its values have
	;;  * been calculated as table[i]=8*log(i/64)/log(2) with an obvious exception
	;;  * for i=0. This log-table converts a linear volume-scaling (0..127) to a
	;;  * logarithmic scaling as present in the FM-synthesizer chips. so :    Volume
	;;  * 64 =  0 db = relative volume  0 and:    Volume 32 = -6 db = relative
	;;  * volume -8 it was implemented as a table because it is only 128 bytes and
	;;  * it saves a lot of log() calculations. (Rob Hooft <hooft@chem.ruu.nl>)
	;;  */

MBFM_VOL_TABLE_LINE MACRO a, b, c, d, e, f, g, h
	db	a&0xff, b&0xff, c&0xff, d&0xff, e&0xff, f&0xff, g&0xff, h&0xff
	ENDM
	
	MBFM_VOL_TABLE_LINE	-63, -48, -40, -35, -32, -29, -27, -26
	MBFM_VOL_TABLE_LINE	-24, -23, -21, -20, -19, -18, -18, -17
	MBFM_VOL_TABLE_LINE	-16, -15, -15, -14, -13, -13, -12, -12
	MBFM_VOL_TABLE_LINE	-11, -11, -10, -10, -10, -9, -9, -8
	MBFM_VOL_TABLE_LINE	-8, -8, -7, -7, -7, -6, -6, -6
	MBFM_VOL_TABLE_LINE	-5, -5, -5, -5, -4, -4, -4, -4
	MBFM_VOL_TABLE_LINE	-3, -3, -3, -3, -2, -2, -2, -2
	MBFM_VOL_TABLE_LINE	-2, -1, -1, -1, -1, 0, 0, 0
	MBFM_VOL_TABLE_LINE	0, 0, 0, 1, 1, 1, 1, 1
	MBFM_VOL_TABLE_LINE	1, 2, 2, 2, 2, 2, 2, 2
	MBFM_VOL_TABLE_LINE	3, 3, 3, 3, 3, 3, 3, 4
	MBFM_VOL_TABLE_LINE	4, 4, 4, 4, 4, 4, 4, 5
	MBFM_VOL_TABLE_LINE	5, 5, 5, 5, 5, 5, 5, 5
	MBFM_VOL_TABLE_LINE	6, 6, 6, 6, 6, 6, 6, 6
	MBFM_VOL_TABLE_LINE	6, 7, 7, 7, 7, 7, 7, 7
	MBFM_VOL_TABLE_LINE	7, 7, 7, 8, 8, 8, 8, 8
