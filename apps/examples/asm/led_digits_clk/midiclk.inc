; $Id$
;
; MIDI Clock Receiver
;
; Decodes the incoming MIDI clock and stores it in
;    MIDICLK_CTR      : counts the incoming clocks from 0 to 23
;    MIDICLK_QN       : counts the quarter notes from 0 to 4
;    MIDICLK_BEAT_LSB : counts the beats (least significant bits)
;    MIDICLK_BEAT_MSB : counts the beats (most significant bits)
;
; This include file provides following functions:
;    o MIDICLK_Init:     initializes all digits
;    o MIDICLK_Handler:  see function header
;
; NOTE: some registers have to be inserted in app_defines.h to get this
;       driver working:
;
;MIDICLK_STATE		EQU	0x130	; state of MIDI clock handler
;MIDICLK_BEAT_MSB	EQU     0x131	; current "beat" (most significant bits))
;MIDICLK_BEAT_LSB	EQU     0x132	; current "beat" (least significant bits))
;MIDICLK_QN		EQU     0x133	; current "quarter note"
;MIDICLK_CTR		EQU     0x134	; current "clock counter"
;MIDICLK_POS_LSB	EQU	0x135	; temporary used to receive the song position
;MIDICLK_POS_MSB	EQU	0x136	; temporary used to receive the song position
;
;       they have to be located to free register addresses!
;
; ==========================================================================
;
;  Copyright 2004 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDICLK_Init
;;  DESCRIPTION: This function initializes the MIDICLK registers
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MIDICLK_Init
	;; clear the MIDI Clock registers
	SET_BSR	MIDICLK_STATE
	clrf	MIDICLK_BEAT_MSB, BANKED
	clrf	MIDICLK_BEAT_LSB, BANKED
	clrf	MIDICLK_QN, BANKED
	clrf	MIDICLK_CTR, BANKED

	;; and the state variable
	clrf	MIDICLK_STATE, BANKED

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MIDICLK_Handler
;;  DESCRIPTION: This function parses the incoming stream for MIDI clock events
;;  This function should be called from USER_MPROC_NotifyReceivedByte
;;  to forward the incoming byte
;; 
;;  The coding is described under http://www.borg.com/~jglatt/tutr/seqtime.htm
;; 
;;  Implemented: decoding of quarter frame messages (F1 xx)
;;               decoding of full frame message (SysEx string)
;;               autoincrement on every 4th quarter frame (F1 0x and F1 4x)
;; 
;;  Not implemented: autodecrement on reverse direction (lack of generator which
;;                   allows me to test this code)
;; 
;;  IN:   incoming MIDI byte in WREG
;;  OUT:  returns 0x01 when the complete MIDI clock has been received, else 0x00
;;  USES: -
;; --------------------------------------------------------------------------

MIDICLK_Handler
	;; store incoming byte in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	;; use banked accesses, select bank where MIDICLK_STATE has been located
	;; (allows to move the MIDICLK_* addresses above 0x7f)
	SET_BSR	MIDICLK_STATE

	;; branch on realtime events (which can be sent in between other events)
	movlw	0xf8
	cpfslt	MIOS_PARAMETER1, ACCESS
	rgoto MIDICLK_Handler_RTEvnt

	;; ------------------------------------------------------------------
MIDICLK_Handler_F2
	;; clear MIDICLK state if status byte (like 0xf2...)
	btfsc	MIOS_PARAMETER1, 7
	clrf	MIDICLK_STATE, BANKED

	;; check sequence (MIDICLK_STATE used as byte counter here)
	movf	MIDICLK_STATE, W, BANKED
	JUMPTABLE_2BYTES 3		; 3 states
	rgoto	MIDICLK_Handler_F2_0	; checks for 0xf2
	rgoto	MIDICLK_Handler_F2_LSB	; get position, low-byte
	rgoto	MIDICLK_Handler_F2_MSB	; get position, high-byte

MIDICLK_Handler_F2_0	; checks for 0xF2
	movlw	0xf2
	;; 	rgoto	MIDICLK_Handler_Syx_Check
MIDICLK_Handler_Syx_Check
	cpfseq	MIOS_PARAMETER1, ACCESS
	rgoto MIDICLK_Handler_Syx_Invalid

	;; if byte matches, increment the state number and exit handler
MIDICLK_Handler_Syx_Matched
	incf	MIDICLK_STATE, F, BANKED
	rgoto	MIDICLK_Handler_End_NotComplete
	
	;; if byte doesn't match, clear state number and exit handler
MIDICLK_Handler_Syx_Invalid
	clrf	MIDICLK_STATE, BANKED
	rgoto	MIDICLK_Handler_End_NotComplete


MIDICLK_Handler_F2_LSB	; temp. store low-byte of position
	movff	MIOS_PARAMETER1, MIDICLK_POS_LSB
	rgoto	MIDICLK_Handler_Syx_Matched
	
MIDICLK_Handler_F2_MSB	; get high-byte and refresh clock position
	;; copy first bit of MSB to bit #7 of LSB
	btfsc	MIOS_PARAMETER1, 0
	bsf	MIDICLK_POS_LSB, 7, BANKED
	;; and shift MSB to the right
	clrc
	rrf	MIOS_PARAMETER1, W
	movwf	MIDICLK_POS_MSB, BANKED

	;; clock counter: LSB[1..0] * 6
	movf	MIDICLK_POS_LSB, W, BANKED
	andlw	0x03
	mullw	6
	movff	PRODL, MIDICLK_CTR

	;; quarter note: LSB[3..2]
	rrf	MIDICLK_POS_LSB, W, BANKED
	rrf	WREG, W
	andlw	0x03
	movwf	MIDICLK_QN, BANKED

	;; beat: (MSB << 4) | LSB[7..4]
	swapf	MIDICLK_POS_LSB, W, BANKED
	andlw	0x0f
	movwf	MIDICLK_POS_LSB, BANKED
	swapf	MIDICLK_POS_MSB, W, BANKED
	andlw	0xf0
	iorwf	MIDICLK_POS_LSB, F, BANKED
	swapf	MIDICLK_POS_MSB, W, BANKED
	andlw	0x0f
	movwf	MIDICLK_POS_MSB, BANKED

	;; copy result to MIDICLK_BEAT_*
	movff	MIDICLK_POS_LSB, MIDICLK_BEAT_LSB
	movff	MIDICLK_POS_MSB, MIDICLK_BEAT_MSB

	rgoto	MIDICLK_Handler_End_Complete
	
	;; ------------------------------------------------------------------
MIDICLK_Handler_End_NotComplete
	andlw	0x00		; fix zero flag
	return

MIDICLK_Handler_End_Complete
	movlw	0x01
	andlw	0xff		; fix zero flag
	return

	;; ------------------------------------------------------------------
MIDICLK_Handler_RTEvnt
	;; increment clock if F8 has been received
	movf	MIOS_PARAMETER1, W
	xorlw	0xf8
	bz	MIDICLK_Handler_RTEvnt_F8

	;; reset clock if FA (start) has been received
	movf	MIOS_PARAMETER1, W
	xorlw	0xfa
	bz	MIDICLK_Handler_RTEvnt_FA

	;; else do nothing
	rgoto	MIDICLK_Handler_End_NotComplete

	;; ------------------------------------------------------------------
MIDICLK_Handler_RTEvnt_F8
	;; increment clock
	incf	MIDICLK_CTR, F, BANKED
	movlw	24-1
	cpfsgt	MIDICLK_CTR, BANKED
	rgoto MIDICLK_Handler_End_Complete
	clrf	MIDICLK_CTR, BANKED
	incf	MIDICLK_QN, F, BANKED
	movlw	4-1
	cpfsgt	MIDICLK_QN, BANKED
	rgoto MIDICLK_Handler_End_Complete
	clrf	MIDICLK_QN, BANKED
	incf	MIDICLK_BEAT_LSB, F, BANKED
	skpnc
	incf	MIDICLK_BEAT_MSB, F, BANKED
	rgoto	MIDICLK_Handler_End_Complete

	;; ------------------------------------------------------------------
MIDICLK_Handler_RTEvnt_FA
	;; clear clock registers
	clrf	MIDICLK_CTR, BANKED
	clrf	MIDICLK_QN, BANKED
	clrf	MIDICLK_BEAT_LSB, BANKED
	clrf	MIDICLK_BEAT_MSB, BANKED
	rgoto	MIDICLK_Handler_End_Complete
