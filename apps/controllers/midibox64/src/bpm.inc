; $Id$
;
; BPM Counter
;
; Derives the BPM from the number of received clock events per time
; The value is stored in BPM_CTR_REG and can be converted to BPM with
; the BPM_Get function
;
; This include file provides following functions:
;    o BPM_Init:     initializes the BPM handler - must be called on every incoming FA
;    o BPM_Tick:     see function header - must be called on every incoming F8
;    o BPM_Handler:  see function header - must be called periodically from an ISR
;    o BPM_Get:      returns the BPM
;
; NOTE: some registers have to be inserted in app_defines.h to get this
;       driver working:
;
;BPM_CTR		EQU	0x050	; running F8 clock counter
;BPM_CTR_REG      	EQU     0x051	; registered counter value
;BPM_TIMER_CTR_L 	EQU     0x052	; used as predivider for a 2-second-timer
;BPM_TIMER_CTR_H	EQU     0x053	; dito, high byte
;
;       they have to be located to free register addresses!
;
; how many times will the BPM_Handler be called after 2 seconds have been passed?
; formula: timer preload value = 2s / period time
; Example: if BPM_Handler is called from USER_SR_Service_Prepare, and this
;          routine is called every mS, the preload value is: 2s / 1mS = (2/1E-3)s = 2000
#define BPM_TIMER_PRELOAD_VALUE		2000
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: BPM_Init
;;  DESCRIPTION: This function initializes the BPM handler. It should be
;;  called from USER_Init and on every incoming FA (MIDI Start) event
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
BPM_Init
	;; initialize the BPM registers
	SET_BSR	BPM_CTR
	clrf	BPM_CTR, BANKED
	clrf	BPM_CTR_REG, BANKED

	movlw	BPM_TIMER_PRELOAD_VALUE & 0xff
	movwf	BPM_TIMER_CTR_L, BANKED
	movlw	BPM_TIMER_PRELOAD_VALUE >> 8
	movwf	BPM_TIMER_CTR_H, BANKED

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: BPM_Tick
;;  DESCRIPTION: This function should be called on every incoming MIDI clock
;;  (F8 event)
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
BPM_Tick
	SET_BSR	BPM_CTR
	incf	BPM_CTR, F
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: BPM_Handler
;;  DESCRIPTION: This function should be called periodically from an ISR
;;  like USER_MPROC_NotifyReceivedByte or USER_Timer, it registers the
;;  running BPM counter BPM_CTR every 2 seconds
;;  The preload value of the predivider is defined in BPM_TIMER_PRELOAD_VALUE,
;;  see also the appr. comments how to calculate the correct value
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
BPM_Handler
	SET_BSR	BPM_CTR

	;; decrement predivider
	decf	BPM_TIMER_CTR_L, F
	skpc
	decf	BPM_TIMER_CTR_H, F

	;; do nothing if counter not zero
	movf	BPM_TIMER_CTR_L, W, BANKED
	iorwf	BPM_TIMER_CTR_H, W, BANKED
	bnz	BPM_Handler_End

	;; else load counter again
	movlw	BPM_TIMER_PRELOAD_VALUE & 0xff
	movwf	BPM_TIMER_CTR_L, BANKED
	movlw	BPM_TIMER_PRELOAD_VALUE >> 8
	movwf	BPM_TIMER_CTR_H, BANKED

	;; copy running BPM counter to BPM_CTR_REG
	movff	BPM_CTR, BPM_CTR_REG

	;; and clear running counter
	clrf	BPM_CTR

BPM_Handler_End
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: BPM_Get
;;  DESCRIPTION: This function returns the converted BPM value
;;  IN:   -
;;  OUT:  result in WREG
;;  USES: BSR
;; --------------------------------------------------------------------------
BPM_Get
	TABLE_ADDR BPM_CONVERT_TABLE
	movff	BPM_CTR_REG, WREG
	TABLE_ADD_W
	tblrd*+
	movf	TABLAT, W
	return

;; --------------------------------------------------------------------------
;;  This table has been created with the tiny perl script under utils/bpm.pl
;; --------------------------------------------------------------------------
BPM_CONVERT_TABLE
	db	0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08
	db	0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12
	db	0x14, 0x15, 0x16, 0x17, 0x19, 0x1A, 0x1B, 0x1C
	db	0x1E, 0x1F, 0x20, 0x21, 0x23, 0x24, 0x25, 0x26
	db	0x28, 0x29, 0x2A, 0x2B, 0x2D, 0x2E, 0x2F, 0x30
	db	0x32, 0x33, 0x34, 0x35, 0x37, 0x38, 0x39, 0x3A
	db	0x3C, 0x3D, 0x3E, 0x3F, 0x41, 0x42, 0x43, 0x44
	db	0x46, 0x47, 0x48, 0x49, 0x4B, 0x4C, 0x4D, 0x4E
	db	0x50, 0x51, 0x52, 0x53, 0x55, 0x56, 0x57, 0x58
	db	0x5A, 0x5B, 0x5C, 0x5D, 0x5F, 0x60, 0x61, 0x62
	db	0x64, 0x65, 0x66, 0x67, 0x69, 0x6A, 0x6B, 0x6C
	db	0x6E, 0x6F, 0x70, 0x71, 0x73, 0x74, 0x75, 0x76
	db	0x78, 0x79, 0x7A, 0x7B, 0x7D, 0x7E, 0x7F, 0x80
	db	0x82, 0x83, 0x84, 0x85, 0x87, 0x88, 0x89, 0x8A
	db	0x8C, 0x8D, 0x8E, 0x8F, 0x91, 0x92, 0x93, 0x94
	db	0x96, 0x97, 0x98, 0x99, 0x9B, 0x9C, 0x9D, 0x9E
	db	0xA0, 0xA1, 0xA2, 0xA3, 0xA5, 0xA6, 0xA7, 0xA8
	db	0xAA, 0xAB, 0xAC, 0xAD, 0xAF, 0xB0, 0xB1, 0xB2
	db	0xB4, 0xB5, 0xB6, 0xB7, 0xB9, 0xBA, 0xBB, 0xBC
	db	0xBE, 0xBF, 0xC0, 0xC1, 0xC3, 0xC4, 0xC5, 0xC6
	db	0xC8, 0xC9, 0xCA, 0xCB, 0xCD, 0xCE, 0xCF, 0xD0
	db	0xD2, 0xD3, 0xD4, 0xD5, 0xD7, 0xD8, 0xD9, 0xDA
	db	0xDC, 0xDD, 0xDE, 0xDF, 0xE1, 0xE2, 0xE3, 0xE4
	db	0xE6, 0xE7, 0xE8, 0xE9, 0xEB, 0xEC, 0xED, 0xEE
	db	0xF0, 0xF1, 0xF2, 0xF3, 0xF5, 0xF6, 0xF7, 0xF8
	db	0xFA, 0xFB, 0xFC, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF
	db	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	db	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	db	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	db	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	db	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	db	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
