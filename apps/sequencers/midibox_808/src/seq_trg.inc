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
SEQ_TRG_AUX_RANDOM_FLAM EQU	0x07
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
	db	"R.Flm " ; 7
	db	"Dly1  " ; 8
	db	"Dly2  " ; 9
	db	"Dly3  " ; 10
	db	"Dly4  " ; 11
	db	"Spare " ; 12
	db	"Spare " ; 13
	db	"Spare " ; 14
	db	"Spare " ; 15

	db	"Gate "  ; 16
	db	"G/Aux"  ; 17

	
	;; note delay values added by aux triggers (only relevant for delay triggers 8-11)
SEQ_TRG_DELAY_TABLE
	db	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x06, 0x09, 0x0c, 0x00, 0x00, 0x00, 0x00

;; --------------------------------------------------------------------------
;;  This function prints the trigger assignment type assigned to the auxiliary layer
;;  IN: WREG: 0-16: trigger type (0-15: Aux name, 16: prints "Gate ", 17: prints "G/Aux"
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
	;; if "force accent" flag set: overrule accent if track is selected
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_MODE2, SEQ_MODE2_FORCE_AUX, BANKED, SEQ_TRG_Hlp_GetAuxBit_NoForce
SEQ_TRG_Hlp_GetAuxBit_Force
	movf	SEQ_EVNTT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	btfss	SEQ_EVNTT, 3, BANKED
	andwf	SEQ_SELECTED_TRKS_0, W, BANKED	
	btfsc	SEQ_EVNTT, 3, BANKED
	andwf	SEQ_SELECTED_TRKS_1, W, BANKED
	bz	SEQ_TRG_Hlp_GetAuxBit_NoForce
	movlw	0x01
	andlw	0xff		; (fix zero flag)
	return

SEQ_TRG_Hlp_GetAuxBit_NoForce
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
	;; if "force accent" flag set: overrule accent if track is selected
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_MODE2, SEQ_MODE2_FORCE_ACCENT, BANKED, SEQ_TRG_AccentGet_NoForce
SEQ_TRG_AccentGet_Force
	movf	SEQ_EVNTT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	btfss	SEQ_EVNTT, 3, BANKED
	andwf	SEQ_SELECTED_TRKS_0, W, BANKED	
	btfsc	SEQ_EVNTT, 3, BANKED
	andwf	SEQ_SELECTED_TRKS_1, W, BANKED
	bz	SEQ_TRG_AccentGet_NoForce
	movlw	0x01
	andlw	0xff		; (fix zero flag)
	return
	
SEQ_TRG_AccentGet_NoForce
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
SEQ_TRG_RandomFlamGet
	movlw	SEQ_TRG_AUX_RANDOM_FLAM
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
;;  This function is called from SEQ_LAYER_GetEvntL to modify SEQ_EVNTL
;;  depending on aux assignment
;;  IN: step number in SEQ_EVNTS (0-63)
;;      track number in SEQ_EVNTT (0-15)
;;      trigger assignments in TRKASSGN[01]x
;;  OUT: can modify SEQ_EVNTL depending on aux assignment
;; --------------------------------------------------------------------------
SEQ_TRG_ModifyEVNTL
	;; branch depending on aux assignment
	call	SEQ_FSR_TrkAssgn	; get aux assignment
	movf	INDF1, W		; compare with type
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_TRG_ModifyEVNTL_0
	rgoto	SEQ_TRG_ModifyEVNTL_1
	rgoto	SEQ_TRG_ModifyEVNTL_2
	rgoto	SEQ_TRG_ModifyEVNTL_3
	rgoto	SEQ_TRG_ModifyEVNTL_4
	rgoto	SEQ_TRG_ModifyEVNTL_5
	rgoto	SEQ_TRG_ModifyEVNTL_6
	rgoto	SEQ_TRG_ModifyEVNTL_7
	rgoto	SEQ_TRG_ModifyEVNTL_8
	rgoto	SEQ_TRG_ModifyEVNTL_9
	rgoto	SEQ_TRG_ModifyEVNTL_A
	rgoto	SEQ_TRG_ModifyEVNTL_B
	rgoto	SEQ_TRG_ModifyEVNTL_C
	rgoto	SEQ_TRG_ModifyEVNTL_D
	rgoto	SEQ_TRG_ModifyEVNTL_E
	rgoto	SEQ_TRG_ModifyEVNTL_F

SEQ_TRG_ModifyEVNTL_0	; accent: no modification
SEQ_TRG_ModifyEVNTL_1	; skip: no modification
SEQ_TRG_ModifyEVNTL_6	; random gate: no modification
SEQ_TRG_ModifyEVNTL_8	; delay1
SEQ_TRG_ModifyEVNTL_9	; delay2
SEQ_TRG_ModifyEVNTL_A	; delay3
SEQ_TRG_ModifyEVNTL_B	; delay4
SEQ_TRG_ModifyEVNTL_C	; reserved
SEQ_TRG_ModifyEVNTL_D	; reserved
SEQ_TRG_ModifyEVNTL_E	; reserved
SEQ_TRG_ModifyEVNTL_F	; reserved

SEQ_TRG_ModifyEVNTL_2	; flam1
	;; force length to 0x32 (2x18) if flam1 flag is set
	call	SEQ_TRG_Flam1Get
	movlw	0x32
	skpz
	movwf	SEQ_EVNTL, BANKED
	return
	
SEQ_TRG_ModifyEVNTL_3	; flam2
	;; force length to 0x2c (2x12) if flam2 flag is set
	call	SEQ_TRG_Flam2Get
	movlw	0x2c
	skpz
	movwf	SEQ_EVNTL, BANKED
	return

SEQ_TRG_ModifyEVNTL_4	; flam3
	;; force length to 0x46 (3x6) if flam3 flag is set
	call	SEQ_TRG_Flam3Get
	movlw	0x46
	skpz
	movwf	SEQ_EVNTL, BANKED
	return

SEQ_TRG_ModifyEVNTL_5	; flam4
	;; force length to 0x64 (4x4) if flam4 flag is set
	call	SEQ_TRG_Flam4Get
	movlw	0x44
	skpz
	movwf	SEQ_EVNTL, BANKED
	return

SEQ_TRG_ModifyEVNTL_7	; random flam
	;; randomly select flam 1/2/3/4
	call	SEQ_TRG_RandomFlamGet
	bz	SEQ_TRG_ModifyEVNTL_7_End

	call	SEQ_CORE_GenRandomNumber	; generate a new random number
	movlw	0x10	; no flam
	btfsc	SEQ_RANDOM_SEED_L, 0, BANKED
	movlw	0x32	; 2x18
	btfsc	SEQ_RANDOM_SEED_L, 1, BANKED
	movlw	0x2c	; 2x12
	btfsc	SEQ_RANDOM_SEED_L, 2, BANKED
	movlw	0x46	; 3x6
	btfsc	SEQ_RANDOM_SEED_L, 3, BANKED
	movlw	0x44	; 4x4
	movwf	SEQ_EVNTL, BANKED
SEQ_TRG_ModifyEVNTL_7_End
	return


;; --------------------------------------------------------------------------
;;  This function is called from SEQ_GROOVE_GetDelay to return a delay value
;;  depending on aux assignment
;;  IN: step number in SEQ_EVNTS (0-63)
;;      track number in SEQ_EVNTT (0-15)
;;      trigger assignments in TRKASSGN[01]x
;;  OUT: 0x00 if no delay trigger assigned, 1..15 if delay1/2/3/4 assigned
;; --------------------------------------------------------------------------
SEQ_TRG_GetAuxDelay
	;; branch depending on aux assignment
	call	SEQ_FSR_TrkAssgn	; get aux assignment
	movf	INDF1, W		; compare with type
	andlw	0x0f
	TABLE_ADDR_MUL_W SEQ_TRG_DELAY_TABLE, 1
	tblrd*+
	movf	TABLAT, W
	bz	SEQ_TRG_GetAuxDelay_End

	;; check if aux flag set
	rcall	SEQ_TRG_Hlp_GetAuxBit_Ok
	bz	SEQ_TRG_GetAuxDelay_End

	movf	TABLAT, W
SEQ_TRG_GetAuxDelay_End
	return


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
