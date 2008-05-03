; $Id$
;
; Register Access Routines for MIDIbox FM
;
; define the pins to which the MBHP_OPL3 module is connected
;
MBFM_LAT_D	EQU	LATB		; Port B
MBFM_LAT_A1	EQU	LATD		; Pin D.6
MBFM_PIN_A1	EQU	6
MBFM_LAT_A0	EQU	LATD		; Pin D.5
MBFM_PIN_A0	EQU	5
MBFM_LAT_CS	EQU	LATC		; Pin C.5
MBFM_PIN_CS	EQU	5
MBFM_LAT_IC	EQU	LATC		; Pin C.4
MBFM_PIN_IC	EQU	4
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  Initialize the MBHP_OPL3 module
;; --------------------------------------------------------------------------
MBFM_REG_Init
	;; activate reset of OPL3 module (low-active)
	bcf	MBFM_LAT_IC, MBFM_PIN_IC

	;; release chip select (low-active)
	bsf	MBFM_LAT_CS, MBFM_PIN_CS

	;; wait for at least 400/fM cycles
	rcall	MBFM_REG_Init_Hlp_Wait
	
	;; release reset
	bsf	MBFM_LAT_IC, MBFM_PIN_IC

	;; wait again for some cycles
	rcall	MBFM_REG_Init_Hlp_Wait

	;; enable OPL3 extension and switch to 4OP voices
	movlw	0x05
	movwf	MIOS_PARAMETER1
	setf	MIOS_PARAMETER2
	movlw	0x01
	rcall	MBFM_REG_Write

	rcall	MBFM_REG_WaitOPL

	movlw	0x04
	movwf	MIOS_PARAMETER1
	setf	MIOS_PARAMETER2
	movlw	0x3f
	rgoto	MBFM_REG_Write


;; --- help routine which waits for at least 400/fM cycles
MBFM_REG_Init_Hlp_Wait
	clrf	TMP1
	clrf	TMP2
MBFM_REG_Init_Hlp_WaitLoop
	clrwdt
	decfsz	TMP1, F
	rgoto	MBFM_REG_Init_Hlp_WaitLoop
	decfsz	TMP2, F
	rgoto	MBFM_REG_Init_Hlp_WaitLoop
	return


;; --------------------------------------------------------------------------
;;  Register Write: write to OPL3 register
;;  IN: Address in MIOS_PARAMETER1 and MIOS_PARAMETER2[0]
;;      Data in WREG (will be saved in MIOS_PARAMETER3)
;; --------------------------------------------------------------------------
MBFM_REG_Write
	movwf	MIOS_PARAMETER3			; save data word

	;; transfer address to OPL3
	bcf	MBFM_LAT_A0, MBFM_PIN_A0	; A0=0: address write mode
	bcf	MBFM_LAT_A1, MBFM_PIN_A1	; A1=selected array
	btfsc	MIOS_PARAMETER2, 0
	bsf	MBFM_LAT_A1, MBFM_PIN_A1
	movff	MIOS_PARAMETER1, MBFM_LAT_D	; D0-D7: selected address
	bcf	MBFM_LAT_CS, MBFM_PIN_CS	; activate chip select line
	nop
	bsf	MBFM_LAT_CS, MBFM_PIN_CS	; release chip select line

	movlw	5				; wait for 32 OPL3 clock cycles (ca. 2.2 uS)
	movwf	TMP2				; (we are calculating from the movlw to the next CS)
MBFM_REG_Write_WaitLoop1
	decfsz	TMP2, F
	rgoto	MBFM_REG_Write_WaitLoop1

	;; transfer data to OPL3
	bsf	MBFM_LAT_A0, MBFM_PIN_A0	; A0=1: data write mode, don't change A0
	movff	MIOS_PARAMETER3, MBFM_LAT_D	; D0-D7: data word
	bcf	MBFM_LAT_CS, MBFM_PIN_CS	; activate chip select line
	nop
	bsf	MBFM_LAT_CS, MBFM_PIN_CS	; release chip select line

	;; the additional 32 waitcycles will be inserted by return/next call

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_REG_WaitOPL
;;  DESCRIPTION: this delay function can be used to wait for ca. 32 OPL3 cycles
;;  OUT:  -
;;  USES: PRODL
;; --------------------------------------------------------------------------
MBFM_REG_WaitOPL
	movlw	5				; wait for 32 OPL3 clock cycles (ca. 2.2 uS)
	movwf	PRODL				; (we are calculating from the movlw to the next CS)
MBFM_REG_WaitOPLLoop
	decfsz	PRODL, F
	rgoto	MBFM_REG_WaitOPLLoop
	return

