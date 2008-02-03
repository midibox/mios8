; $Id$
;
; MIDIbox LC Misc functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  get relative positions of MTC digits
;; --------------------------------------------------------------------------
LC_HLP_MTCPositionGet
	BRA_IFCLR LC_STAT, LC_STAT_SMPTE, ACCESS, LC_HLP_MTCPositionGet_BEATS
LC_HLP_MTCPositionGet_SMPTE
	JUMPTABLE_2BYTES 14
	;; mtc digits
	retlw	12	; rightmost digit
	retlw	11
	retlw	0xff		; this digit will not be print in SMPTE mode
	
	retlw	9
	retlw	8

	retlw	6
	retlw	5

	retlw	3
	retlw	2
	retlw	1	; leftmost digit

	;; colons
	retlw	4
	retlw	7
	retlw	10

	;; space
	retlw	0		; to clear unusued position


LC_HLP_MTCPositionGet_BEATS
	JUMPTABLE_2BYTES 14
	;; mtc digits
	retlw	12	; rightmost digit
	retlw	11
	retlw	10
	
	retlw	8
	retlw	7

	retlw	5
	retlw	4

	retlw	2
	retlw	1
	retlw	0	; first digit

	;; colons
	retlw	3
	retlw	6
	retlw	9

	;; space
	retlw	0		; not used here


;;_____________________________________________________
;; Charcter LCD Position Definitions
;; provided by Axel
;;_____________________________________________________

LC_HLP_MsgCursorPos
	JUMPTABLE_2BYTES 55 ;; (55 table entries)
	retlw	0x00 + 7
	retlw	0x00 + 8
	retlw	0x00 + 9
	retlw	0x00 + 10
	retlw	0x00 + 11
	retlw	0x00 + 12
	retlw	0x00 + 13

	retlw	0x00 + 16
	retlw	0x00 + 17
	retlw	0x00 + 18
	retlw	0x00 + 19
	retlw	0x00 + 20
	retlw	0x00 + 21
	retlw	0x00 + 22

	retlw	0x00 + 25
	retlw	0x00 + 26
	retlw	0x00 + 27
	retlw	0x00 + 28
	retlw	0x00 + 29
	retlw	0x00 + 30
	retlw	0x00 + 31

	retlw	0x00 + 34
	retlw	0x00 + 35
	retlw	0x00 + 36
	retlw	0x00 + 37
	retlw	0x00 + 38
	retlw	0x00 + 39

	retlw	0x80 + 0
	retlw	0x80 + 1
	retlw	0x80 + 2
	retlw	0x80 + 3
	retlw	0x80 + 4
	retlw	0x80 + 5
	retlw	0x80 + 6

	retlw	0x80 + 9
	retlw	0x80 + 10
	retlw	0x80 + 11
	retlw	0x80 + 12
	retlw	0x80 + 13
	retlw	0x80 + 14
	retlw	0x80 + 15

	retlw	0x80 + 18
	retlw	0x80 + 19
	retlw	0x80 + 20
	retlw	0x80 + 21
	retlw	0x80 + 22
	retlw	0x80 + 23
	retlw	0x80 + 24

	retlw	0x80 + 27
	retlw	0x80 + 28
	retlw	0x80 + 29
	retlw	0x80 + 30
	retlw	0x80 + 31
	retlw	0x80 + 32
	retlw	0x80 + 33

