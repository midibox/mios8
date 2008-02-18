; $Id$
;
; MIDIboxSEQ
; Step Trigger functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; aux assignment numbers
SEQ_TRG_AUX_ACCENT	EQU	0x00
SEQ_TRG_AUX_SKIP	EQU	0x01
SEQ_TRG_AUX_FLAM1	EQU	0x02
SEQ_TRG_AUX_FLAM2	EQU	0x03
SEQ_TRG_AUX_FLAM3	EQU	0x04
SEQ_TRG_AUX_FLAM4	EQU	0x05
SEQ_TRG_AUX_RANDOM_GATE EQU	0x06
SEQ_TRG_AUX_RANDOM_JUMP EQU	0x07
SEQ_TRG_AUX_DELAY1	EQU	0x08
SEQ_TRG_AUX_DELAY2	EQU	0x09
SEQ_TRG_AUX_DELAY3	EQU	0x0a
SEQ_TRG_AUX_DELAY4	EQU	0x0b

	;; names of auxiliary triggers
SEQ_TRG_NAMES
	db	"Acc.  " ; 0
	db	"Skip  " ; 1
	db	"Flam1 " ; 2
	db	"Flam2 " ; 3
	db	"Flam3 " ; 4
	db	"Flam4 " ; 5
	db	"R.Gte " ; 6
	db	"R.Jmp " ; 7
	db	"Dly1  " ; 8
	db	"Dly2  " ; 9
	db	"Dly3  " ; 10
	db	"Dly4  " ; 11
	db	"Spare " ; 12
	db	"Spare " ; 13
	db	"Spare " ; 14
	db	"Spare " ; 15

	db	"Gate "  ; 16


;; --------------------------------------------------------------------------
;;  This function prints the trigger assignment type assigned to the auxiliary layer
;;  IN: WREG: 0-16: trigger type (0-15: Aux name, 16: prints "Gate "
;;  OUT: 5 characters
;;  USES: BSR, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_TRG_PrnType
	;; print string from table
	mullw	6
	movlw	LOW(SEQ_TRG_NAMES)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(SEQ_TRG_NAMES)
	addwfc	PRODH, W
	movwf	TBLPTRH

	;; 5 characters
	movlw	5
	goto	MIOS_LCD_PrintPreconfString


;; --------------------------------------------------------------------------
;;  returns the bit of a trigger layer depending on step
;;  IN: pointer to trigger layer in FSR1
;;      step number in SEQ_EVNTS (0-63)
;;  OUT: 0 or 1
;; --------------------------------------------------------------------------
SEQ_TRG_GetStepBit
	SET_BSR	SEQ_BASE

	;; get offset within trigger layer (0..7)
	rlf	SEQ_EVNTS, W, BANKED
	swapf	WREG, W
	andlw	0x07
	addwf	FSR1L, F

	;; determine bit mask
	movf	SEQ_EVNTS, W, BANKED
	call	MIOS_HLP_GetBitORMask

	;; return 0 if bit cleared, and 1 if bit set (independent from position)
	andwf	INDF1, W
	skpz
	iorlw	0x01
	andlw	0x01		; ensure that only first bit is set
	return


;; --------------------------------------------------------------------------
;;  check if aux layer assigned to trigger type in WREG
;;  IN: trigger type in WREG
;;      step number in SEQ_EVNTS (0-63)
;;  OUT: 0 of type not assigned, otherwise 0 or 1
;; --------------------------------------------------------------------------
SEQ_TRG_Hlp_GetAuxBit
	movwf	TABLAT			; temporary store trigger type in TABLAT
	call	SEQ_FSR_TrkAssgn	; get aux assignment
	movf	INDF1, W		; compare with type
	xorwf	TABLAT, W
	bnz	SEQ_TRG_Hlp_GetAuxBit_0	; ...if not assigned return 0

SEQ_TRG_Hlp_GetAuxBit_Ok
	call	SEQ_FSR_TrgAux		; get pointer to aux trigger layer -> FSR1
	rgoto	SEQ_TRG_GetStepBit	; return step bit

SEQ_TRG_Hlp_GetAuxBit_0
	andlw	0x00			; (to ensure that ZERO flag is set correctly)
	return


;; --------------------------------------------------------------------------
;;  sets a bit within trigger layer depending on step
;;  IN: pointer to trigger layer in FSR1
;;      step number in SEQ_EVNTS (0-63)
;;      value (0 or 1) in WREG
;;  OUT: 0 or 1
;; --------------------------------------------------------------------------
SEQ_TRG_SetStepBit
	SET_BSR	SEQ_BASE

	;; temporary store value in TABLAT
	movwf	TABLAT

	;; get offset within trigger layer (0..7)
	rlf	SEQ_EVNTS, W, BANKED
	swapf	WREG, W
	andlw	0x07
	addwf	FSR1L, F

	;; branch depending on value
	movf	TABLAT, W
	bnz	SEQ_TRG_SetStepBit_1
SEQ_TRG_SetStepBit_0
	;; determine bit mask
	movf	SEQ_EVNTS, W, BANKED
	call	MIOS_HLP_GetBitANDMask

	;; clear bit
	andwf	INDF1, F
	return

SEQ_TRG_SetStepBit_1
	;; determine bit mask
	movf	SEQ_EVNTS, W, BANKED
	call	MIOS_HLP_GetBitORMask

	;; set bit
	iorwf	INDF1, F
	return

	
;; --------------------------------------------------------------------------
;;  These "Get" functions return the assigned value (0 or 1) of a given
;;  function for the given step
;;  IN: step number in SEQ_EVNTS (0-63)
;;      track number in SEQ_EVNTT (0-15)
;;      only for functions != GateGet (aux layer): trigger assignments in TRKASSGNx
;;  OUT: 0 or 1
;; --------------------------------------------------------------------------
SEQ_TRG_GateGet
	;; always assigned to gate layer
	call	SEQ_FSR_TrgGate		; get pointer to gate trigger layer -> FSR1
	rgoto	SEQ_TRG_GetStepBit	; get step bit depending on SEQ_EVNTS

SEQ_TRG_AuxGet
	;; return aux bit regardless of assigned function
	call	SEQ_FSR_TrgAux		; get pointer to aux trigger layer -> FSR1
	rgoto	SEQ_TRG_GetStepBit	; get step bit depending on SEQ_EVNTS


	;; dedicated aux layer return functions
SEQ_TRG_AccentGet
	movlw	SEQ_TRG_AUX_ACCENT
	rgoto	SEQ_TRG_Hlp_GetAuxBit
SEQ_TRG_SkipGet
	movlw	SEQ_TRG_AUX_SKIP
	rgoto	SEQ_TRG_Hlp_GetAuxBit
SEQ_TRG_Flam1Get
	movlw	SEQ_TRG_AUX_FLAM1
	rgoto	SEQ_TRG_Hlp_GetAuxBit
SEQ_TRG_Flam2Get
	movlw	SEQ_TRG_AUX_FLAM2
	rgoto	SEQ_TRG_Hlp_GetAuxBit
SEQ_TRG_Flam3Get
	movlw	SEQ_TRG_AUX_FLAM3
	rgoto	SEQ_TRG_Hlp_GetAuxBit
SEQ_TRG_Flam4Get
	movlw	SEQ_TRG_AUX_FLAM4
	rgoto	SEQ_TRG_Hlp_GetAuxBit
SEQ_TRG_RandomGateGet
	movlw	SEQ_TRG_AUX_RANDOM_GATE
	rgoto	SEQ_TRG_Hlp_GetAuxBit
SEQ_TRG_RandomJumpGet
	movlw	SEQ_TRG_AUX_RANDOM_JUMP
	rgoto	SEQ_TRG_Hlp_GetAuxBit
SEQ_TRG_Delay1Get
	movlw	SEQ_TRG_AUX_DELAY1
	rgoto	SEQ_TRG_Hlp_GetAuxBit
SEQ_TRG_Delay2Get
	movlw	SEQ_TRG_AUX_DELAY2
	rgoto	SEQ_TRG_Hlp_GetAuxBit
SEQ_TRG_Delay3Get
	movlw	SEQ_TRG_AUX_DELAY3
	rgoto	SEQ_TRG_Hlp_GetAuxBit
SEQ_TRG_Delay4Get
	movlw	SEQ_TRG_AUX_DELAY4
	rgoto	SEQ_TRG_Hlp_GetAuxBit


;; --------------------------------------------------------------------------
;;  These "Set" functions change the assigned value (0 or 1) to a given
;;  function for the given step
;;  IN: value in WREG (0 or 1)
;;      step number in SEQ_EVNTS (0-63)
;;      track number in SEQ_EVNTT (0-15)
;;      trigger assignments in TRKASSGN[01]x
;;  OUT: -
;; --------------------------------------------------------------------------
SEQ_TRG_GateSet
	;; always assigned to gate layer
	movwf	TABLAT			; temporary store value in TABLAT
	call	SEQ_FSR_TrgGate		; get pointer to gate trigger layer -> FSR1
	movf	TABLAT, W		; restore value
	rgoto	SEQ_TRG_SetStepBit	; set bit

SEQ_TRG_AuxSet
	movwf	TABLAT			; temporary store value in TABLAT
	call	SEQ_FSR_TrgAux		; get pointer to aux trigger layer -> FSR1
	movf	TABLAT, W		; restore value
	rgoto	SEQ_TRG_SetStepBit	; set bit
