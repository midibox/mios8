; $Id$
;
; SID "Knobs" (Adjustable Parameters with Min/Max values) Handling
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; global constants for knob assignments
;; must match with patch structure (SID_Ix_P_xx_BASE)
SID_KNOB_1	EQU	0
SID_KNOB_2	EQU	1
SID_KNOB_3	EQU	2
SID_KNOB_4	EQU	3
SID_KNOB_5	EQU	4
SID_KNOB_VEL	EQU	5
SID_KNOB_PB	EQU	6
SID_KNOB_ATH	EQU	7

;; --------------------------------------------------------------------------
;;  This function sets a knob value
;;  IN: knob number in WREG (0-7 for Knob #1-5, Vel, Aft, Pb)
;;      8bit value in MIOS_PARAMETER1
;;  OUT: copies value into patch buffer and shadow buffer
;; --------------------------------------------------------------------------
SID_KNOB_SetValue
	SET_BSR	SID_BASE
	movwf	SID_KNOB_NUM, BANKED	; will be needed again later
	rcall	SID_KNOB_GetPtr

	;; copy value into patch buffer
	movlw	SID_Ix_Px_VALUE
	movff	MIOS_PARAMETER1, PLUSW1

	;; copy value into shadow buffer
	movlw	HIGH(SID_PATCH_BUFFER_SHADOW)-HIGH(SID_PATCH_BUFFER)
	addwf	FSR1H, F
	movlw	SID_Ix_Px_VALUE
	movff	MIOS_PARAMETER1, PLUSW1

	;; if control surface and SID1 selected: copy into edit buffer
	movf	CS_MENU_EDIT_BUFFER_SID, W
	bnz	SID_KNOB_SetValue_NotSID1
SID_KNOB_SetValue_SID1
	movlw	HIGH(SID_EDIT_BUFFER)-HIGH(SID_PATCH_BUFFER_SHADOW)
	addwf	FSR1H, F
	movlw	SID_Ix_Px_VALUE
	movff	MIOS_PARAMETER1, PLUSW1
SID_KNOB_SetValue_NotSID1

	;; forward as signed value into modulation source array
	movf	SID_KNOB_NUM, W, BANKED
	rcall	SID_KNOB_GetScaledValue
	;; store PROD[LH] in SID_KNOB_VALUE_[LH] --- we need this later again
	movff	PRODL, SID_KNOB_VALUE_L
	movff	PRODH, SID_KNOB_VALUE_H

	;; (forward to MOD matrix: lead engine only)
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	bnz	SID_KNOB_SetValue_MOD_NoLead
SID_KNOB_SetValue_MOD_Lead
	lfsr	FSR1, SID_MOD_SRC_L_KNOB1_L
	clrc
	rlf	SID_KNOB_NUM, W, BANKED
	addwf	FSR1L, F

	;; 16bit value in SID_KNOB_VALUE_[LH], convert it to 15bit signed value
	movlw	0x00
	subwf	SID_KNOB_VALUE_L, W, BANKED
	movwf	POSTINC1
	movlw	0x80
	subwfb	SID_KNOB_VALUE_H, W, BANKED
	movwf	POSTDEC1
SID_KNOB_SetValue_MOD_NoLead

	;; forward to parameter handler
	;; - 16bit value in MIOS_PARAMETER[12]
	;; - SID channels which should be modified in MIOS_PARAMETER3
	;; - parameter number in WREG
	movff	SID_KNOB_VALUE_L, MIOS_PARAMETER1
	movff	SID_KNOB_VALUE_H, MIOS_PARAMETER2
	rcall	SID_KNOB_SetValue_Hlp_GetMP3	; loads MIOS_PARAMETER3 depending on engine
	movf	SID_KNOB_NUM, W, BANKED
	rcall	SID_KNOB_GetPtr
	movlw	SID_Ix_Px_ASSIGN1
	movf	PLUSW1, W
	call	SID_PARIN_Set16

	movff	SID_KNOB_VALUE_L, MIOS_PARAMETER1
	movff	SID_KNOB_VALUE_H, MIOS_PARAMETER2
	rcall	SID_KNOB_SetValue_Hlp_GetMP3	; loads MIOS_PARAMETER3 depending on engine
	movf	SID_KNOB_NUM, W, BANKED
	rcall	SID_KNOB_GetPtr
	movlw	SID_Ix_Px_ASSIGN2
	movf	PLUSW1, W
	goto	SID_PARIN_Set16


	;; this help function loads MIOS_PARAMETER3 depending on engine
SID_KNOB_SetValue_Hlp_GetMP3
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SID_KNOB_SetValue_Hlp_GetMP3_DM
SID_KNOB_SetValue_Hlp_GetMP3_LB
	BRA_IFSET WREG, 0, ACCESS, SID_KNOB_SetValue_Hlp_GetMP3_B
SID_KNOB_SetValue_Hlp_GetMP3_L
	movlw	0x03		; always modify both SIDs
	movwf	MIOS_PARAMETER3
	return

SID_KNOB_SetValue_Hlp_GetMP3_B
	movlw	0x03		; always modify both SIDs, only for "Current" use selection in MIOS_PARAMETER3[5:4]
	movwf	MIOS_PARAMETER3
	swapf	CS_MENU_SELECTED_SID_LR, W
	andlw	0x30
	iorwf	MIOS_PARAMETER3, F
	return

SID_KNOB_SetValue_Hlp_GetMP3_DM
	BRA_IFSET WREG, 0, ACCESS, SID_KNOB_SetValue_Hlp_GetMP3_M
SID_KNOB_SetValue_Hlp_GetMP3_D
	;; TODO
	movlw	0x03		; always modify both SIDs
	movwf	MIOS_PARAMETER3
	return

SID_KNOB_SetValue_Hlp_GetMP3_M
	movlw	0x03		; always modify both SIDs, only for "Current" use instrument selection in MIOS_PARAMETER3[5:4]
	movwf	MIOS_PARAMETER3
	swapf	CS_MENU_SELECTED_INS, W
	andlw	0x80
	iorwf	MIOS_PARAMETER3, F
	return


;; --------------------------------------------------------------------------
;;  This function returns a knob value scaled between the min/max boundaries
;;  IN: knob number in WREG (0-7 for Knob #1-5, Vel, Pb, Aft)
;;  OUT: 8bit value (scaled) in WREG, 16bit value (scaled) in PROD[LH]
;; --------------------------------------------------------------------------
SID_KNOB_GetScaledValue
	rcall	SID_KNOB_GetPtr

	;; copy value to PRODH
	movlw	SID_Ix_Px_VALUE
	movff	PLUSW1, PRODH

	;; copy Min/Max to MIOS_PARAMETER[12]
	movlw	SID_Ix_Px_MIN
	movff	PLUSW1, MIOS_PARAMETER1
	movlw	SID_Ix_Px_MAX
	movff	PLUSW1, MIOS_PARAMETER2

	;; swap Max/Min if Min value is greater
	;; invert value (for Max->Min sweep)
	movf	MIOS_PARAMETER2, W
	cpfsgt	MIOS_PARAMETER1, ACCESS
	rgoto SID_KNOB_GetScaledValue_NoSwap
SID_KNOB_GetScaledValue_Swap
	movff	MIOS_PARAMETER1, PRODL
	movff	MIOS_PARAMETER2, MIOS_PARAMETER1
	movff	PRODL, MIOS_PARAMETER2
	comf	PRODH, F
SID_KNOB_GetScaledValue_NoSwap

	;; prepare low byte for range=256 case
	clrf	PRODL

	;; range: max-min+1 = MIOS_PARAMETER2 - MIOS_PARAMETER1 + 1
	movf	MIOS_PARAMETER1, W
	subwf	MIOS_PARAMETER2, W
	addlw	1

	;; skip skaling if zero (-> range = 256)
	bz	SID_KNOB_GetScaledValue_SkipMul
SID_KNOB_GetScaledValue_Mul
	mulwf	PRODH, ACCESS
SID_KNOB_GetScaledValue_SkipMul
	movf	MIOS_PARAMETER1, W
	addwf	PRODH, F
	movf	PRODH, W
	return


;; --------------------------------------------------------------------------
;;  This function returns the unscaled knob value
;;  IN: knob number in WREG (0-7 for Knob #1-5, Vel, Pb, Aft)
;;  OUT: 8bit value
;; --------------------------------------------------------------------------
SID_KNOB_GetValue
	rcall	SID_KNOB_GetPtr

	movlw	SID_Ix_Px_VALUE
	movf	PLUSW1, W
	return


;; --------------------------------------------------------------------------
;;  This function updates a specific knob value
;;  (used when a patch is (re-)initialised, or by the CS when a min/max value is changed)
;;  IN: knob number in WREG (0-7 for Knob #1-5, Vel, Pb, Aft)
;; --------------------------------------------------------------------------
SID_KNOB_UpdateValue
	SET_BSR	SID_BASE
	movwf	SID_KNOB_NUM, BANKED

	rcall	SID_KNOB_GetValue
	movwf	MIOS_PARAMETER1
	movf	SID_KNOB_NUM, W, BANKED
	rgoto	SID_KNOB_SetValue


;; --------------------------------------------------------------------------
;;  This function updates all knob values
;;  (used when a patch is (re-)initialised)
;;  IN: knob number in WREG (0-7 for Knob #1-5, Vel, Pb, Aft)
;; --------------------------------------------------------------------------
SID_KNOB_UpdateAll
	SET_BSR	SID_BASE
	clrf	SID_KNOB_NUM, BANKED
SID_KNOB_UpdateAll_Loop
	movf	SID_KNOB_NUM, W, BANKED
	rcall	SID_KNOB_UpdateValue
	incf	SID_KNOB_NUM, F, BANKED
	BRA_IFCLR SID_KNOB_NUM, 3, BANKED, SID_KNOB_UpdateAll_Loop
	return


;; --------------------------------------------------------------------------
;;  This help function returns the pointer to SID_Ix_P_Kx_BASE within the
;;  patch buffer (SID_PATCH_BUFFER)
;;  IN: knob number in WREG (0-7 for Knob #1-5, Vel, Pb, Aft)
;;  OUT: pointer in FSR1
;; --------------------------------------------------------------------------
SID_KNOB_GetPtr
	andlw	0x07		; avoid crashes on programming errors
	lfsr	FSR1, SID_PATCH_BUFFER + SID_Ix_P_K1_BASE
	mullw	SID_Ix_P_K2_BASE-SID_Ix_P_K1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	return
