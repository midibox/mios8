; $Id$
;
; MTC Receiver
;
; Decodes the incoming MTC and stores it in
;    MTC_HR   ("hours")
;    MTC_MN   ("minutes")
;    MTC_SC   ("seconds")
;    MTC_FR   ("frame")
;
; This include file provides following functions:
;    o MTC_Init:     initializes all digits
;    o MTC_Handler:  see function header
;
; NOTE: some registers have to be inserted in app_defines.h to get this
;       driver working:
;
;MTC_STATE		EQU	0x030	; state of received MTC code
;MTC_HR			EQU     0x031	; current "hours"
;MTC_MN			EQU     0x032	; current "minutes"
;MTC_SC			EQU     0x033	; current "seconds"
;MTC_FR			EQU     0x034	; current "frames"
;MTC_HR_NEW		EQU	0x035	; received "hours"
;MTC_MN_NEW		EQU	0x036	; received "minutes"
;MTC_SC_NEW		EQU	0x037	; received "seconds"
;MTC_FR_NEW		EQU	0x038	; received "frames"
;
;       they have to be located to free register addresses!
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MTC_Init
;;  DESCRIPTION: This function initializes the MTC registers
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MTC_Init
	;; clear the MTC
	SET_BSR	MTC_STATE
	clrf	MTC_HR, BANKED
	clrf	MTC_MN, BANKED
	clrf	MTC_SC, BANKED
	clrf	MTC_FR, BANKED

	;; and the state variable
	clrf	MTC_STATE, BANKED

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MTC_Handler
;;  DESCRIPTION: This function parses the incoming stream for MTC
;;  This function should be called from USER_MPROC_NotifyReceivedByte
;;  to forward the incoming byte
;; 
;;  The coding is described under http://www.borg.com/~jglatt/tech/mtc.htm
;; 
;;  Implemented: decoding of quarter frame messages (F1 xx)
;;               decoding of full frame message (SysEx string)
;;               autoincrement on every 4th quarter frame (F1 0x and F1 4x)
;; 
;;  Not implemented: autodecrement on reverse direction (lack of generator which
;;                   allows me to test this code)
;; 
;;  IN:   incoming MIDI byte in WREG
;;  OUT:  returns 0x01 when the complete MTC has been received, else 0x00
;;  USES: -
;; --------------------------------------------------------------------------

	;; define the used states
#define MTC_STATE__F1	7

MTC_Handler
	;; store incoming byte in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	;; ignore realtime events (which can be sent in between other events)
	movlw	0xf8
	cpfslt	MIOS_PARAMETER1, ACCESS
	rgoto MTC_Handler_End_Ignore

	;; use banked accesses, select bank where MTC_STATE has been located
	;; (allows to move the MTC_* addresses above 0x7f)
	SET_BSR	MTC_STATE

	;; if 0xf1 has been received, switch to MTC_STATE__F1
	movf	MIOS_PARAMETER1, W
	xorlw	0xf1
	bz	MTC_Handler_F1_Start

	;; if in MTC_STATE__F1, branch to second step
	BRA_IFSET MTC_STATE, MTC_STATE__F1, BANKED, MTC_Handler_F1_Second

	;; ------------------------------------------------------------------
MTC_Handler_Syx
	;; clear MTC state if status byte (like 0xf0 or 0xf7...)
	btfsc	MIOS_PARAMETER1, 7
	clrf	MTC_STATE, BANKED

	;; check SysEx sequence (MTC_STATE used as byte counter here)
	movf	MTC_STATE, W, BANKED
	JUMPTABLE_2BYTES 9		; 9 states
	rgoto	MTC_Handler_Syx0	; checks for 0xf0
	rgoto	MTC_Handler_Syx1	; checks for 0x7f
	rgoto	MTC_Handler_Syx2	; checks for 0x7f
	rgoto	MTC_Handler_Syx3	; checks for 0x01
	rgoto	MTC_Handler_Syx4	; checks for 0x01
	rgoto	MTC_Handler_Syx5	; stores "hours"
	rgoto	MTC_Handler_Syx6	; stores "minutes"
	rgoto	MTC_Handler_Syx7	; stores "seconds"
	rgoto	MTC_Handler_Syx8	; stores "frames"

	;; 0xf7 (or any other status message which is < 0xf8) will reset MTC_STATE

MTC_Handler_Syx0	; checks for 0xf0
	movlw	0xf0
	rgoto	MTC_Handler_Syx_Check
MTC_Handler_Syx1	; checks for 0x7f
	movlw	0x7f
	rgoto	MTC_Handler_Syx_Check
MTC_Handler_Syx2	; checks for 0x7f
	movlw	0x7f
	rgoto	MTC_Handler_Syx_Check
MTC_Handler_Syx3	; checks for 0x01
	movlw	0x01
	rgoto	MTC_Handler_Syx_Check
MTC_Handler_Syx4	; checks for 0x01
	movlw	0x01
	;; 	rgoto	MTC_Handler_Syx_Check
MTC_Handler_Syx_Check
	cpfseq	MIOS_PARAMETER1, ACCESS
	rgoto MTC_Handler_Syx_Invalid

	;; if byte matches, increment the state number and exit handler
MTC_Handler_Syx_Matched
	incf	MTC_STATE, F, BANKED
	rgoto	MTC_Handler_End_NotComplete
	
	;; if byte doesn't match, clear state number and exit handler
MTC_Handler_Syx_Invalid
	clrf	MTC_STATE, BANKED
	rgoto	MTC_Handler_End_NotComplete

MTC_Handler_Syx5	; stores "hours"
	movf	MIOS_PARAMETER1, W
	movwf	MTC_HR, BANKED
	movwf	MTC_HR_NEW, BANKED
	rgoto	MTC_Handler_Syx_Matched
MTC_Handler_Syx6	; stores "minutes"
	movf	MIOS_PARAMETER1, W
	movwf	MTC_MN, BANKED
	movwf	MTC_MN_NEW, BANKED
	rgoto	MTC_Handler_Syx_Matched
MTC_Handler_Syx7	; stores "seconds"
	movf	MIOS_PARAMETER1, W
	movwf	MTC_SC, BANKED
	movwf	MTC_SC_NEW, BANKED
	rgoto	MTC_Handler_Syx_Matched
MTC_Handler_Syx8	; stores "frames", clear state number --- transfer completed
	movf	MIOS_PARAMETER1, W
	movwf	MTC_FR, BANKED
	movwf	MTC_FR_NEW, BANKED
	rgoto	MTC_Handler_End_Complete
	
	;; ------------------------------------------------------------------
MTC_Handler_End_NotComplete
MTC_Handler_End_Ignore
	andlw	0x00		; fix zero flag
	return

MTC_Handler_End_Complete
	movlw	0x01
	andlw	0xff		; fix zero flag
	return

	;; ------------------------------------------------------------------
MTC_Handler_F1_Start
	;; switch to MTC_STATE__F1
	clrf	MTC_STATE, BANKED
	bsf	MTC_STATE, MTC_STATE__F1, BANKED
	rgoto	MTC_Handler_End_NotComplete

	;; ------------------------------------------------------------------
MTC_Handler_F1_Second
	;; second byte received, clear state
	clrf	MTC_STATE, BANKED

	;; and decode the quarter frame message
	swapf	MIOS_PARAMETER1, W
	andlw	0x07		; 7 entries
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MTC_Handler_F1_0x
	rgoto	MTC_Handler_F1_1x
	rgoto	MTC_Handler_F1_2x
	rgoto	MTC_Handler_F1_3x
	rgoto	MTC_Handler_F1_4x
	rgoto	MTC_Handler_F1_5x
	rgoto	MTC_Handler_F1_6x
	rgoto	MTC_Handler_F1_7x

MTC_Handler_F1_0x
	rcall	MTC_Handler_Inc	   ; increment current MTC (every 4th quarter frame increments the frame number)
	movlw	MTC_FR_NEW & 0xff
	rgoto	MTC_Handler_F1_Low
MTC_Handler_F1_1x
	movlw	MTC_FR_NEW & 0xff
	rgoto	MTC_Handler_F1_High
MTC_Handler_F1_2x
	movlw	MTC_SC_NEW & 0xff
	rgoto	MTC_Handler_F1_Low
MTC_Handler_F1_3x
	movlw	MTC_SC_NEW & 0xff
	rgoto	MTC_Handler_F1_High
MTC_Handler_F1_4x
	rcall	MTC_Handler_Inc   ; increment current MTC (every 4th quarter frame increments the frame number)
	movlw	MTC_MN_NEW & 0xff
	rgoto	MTC_Handler_F1_Low
MTC_Handler_F1_5x
	movlw	MTC_MN_NEW & 0xff
	rgoto	MTC_Handler_F1_High
MTC_Handler_F1_6x
	movlw	MTC_HR_NEW & 0xff
	rgoto	MTC_Handler_F1_Low
MTC_Handler_F1_7x
	movlw	MTC_HR_NEW & 0xff
	rgoto	MTC_Handler_F1_High

MTC_Handler_F1_Low
	;; received low-nibble
	lfsr	FSR1, MTC_HR		; use FSR1 pointer to reference the register
	movwf	FSR1L
	movlw	0xf0			; mask out high-nibble
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W	; insert new low-nibble
	andlw	0x0f
	iorwf	INDF1, F

	;; display update when 0x or 4x has been received
	movf	MIOS_PARAMETER1, W
	andlw	0xb0
	xorlw	0x00
	bnz	MTC_Handler_End_Complete

	rgoto	MTC_Handler_End_NotComplete
	
MTC_Handler_F1_High
	;; received high-nibble
	lfsr	FSR1, MTC_HR		; use FSR1 pointer to reference the register
	movwf	FSR1L
	movlw	0x0f			; mask out low-nibble
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W	; insert new high-nibble
	andlw	0xf0
	iorwf	INDF1, F

	;; code is complete when HR has been received - copy _NEW registers
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	xorlw	0x70
	bnz	MTC_Handler_End_NotComplete

MTC_Handler_Copy
	movff	MTC_HR_NEW, MTC_HR
	movff	MTC_MN_NEW, MTC_MN
	movff	MTC_SC_NEW, MTC_SC
	movff	MTC_FR_NEW, MTC_FR

	rgoto	MTC_Handler_End_Complete

MTC_Handler_Inc
	;; increment MTC with every 4th event
	incf	MTC_FR, F, BANKED
	movlw	24-1				; 24 frames per second
	btfsc	MTC_HR, 5, BANKED; 25 frames per second
	movlw 25-1	
	btfsc	MTC_HR, 6, BANKED; 30 frames per second
	movlw 30-1	
	cpfsgt	MTC_FR, BANKED
	return
	clrf	MTC_FR, BANKED
	incf	MTC_SC, F, BANKED
	movlw	60-1
	cpfsgt	MTC_SC, BANKED
	return
	clrf	MTC_SC, BANKED
	incf	MTC_MN, F, BANKED
	movlw	60-1
	cpfsgt	MTC_MN, BANKED
	return
	clrf	MTC_MN, BANKED
	incf	MTC_HR, W, BANKED
	andlw	0x1f
	movwf	MTC_HR, BANKED
	return
