; $Id$
;
; SID Control Surface
; Offset Functions
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  returns pointer to Vx register within edit buffer depending on selected OSC
;;  CS_MENU_PARAMETER_IX will be added
;; --------------------------------------------------------------------------
CS_MENU_FSR0_Voice
	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_FSR0_Voice_DM
CS_MENU_FSR0_Voice_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_FSR0_Voice_B

CS_MENU_FSR0_Voice_L	; lead engine
	lfsr	FSR0, SID_EDIT_BUFFER
	movlw	SID_Ix_L_S2V3_BASE
	btfsc	CS_MENU_SELECTED_OSC_FLAGS, 1
	movlw SID_Ix_L_S2V2_BASE
	btfsc	CS_MENU_SELECTED_OSC_FLAGS, 0
	movlw SID_Ix_L_S2V1_BASE
	btfsc	CS_MENU_SELECTED_SID_LR, 0
	addlw -(SID_Ix_L_S2V1_BASE-SID_Ix_L_S1V1_BASE)
	addwf	CS_MENU_PARAMETER_IX, W
	addwf	FSR0L, F
	return

CS_MENU_FSR0_Voice_B	; bassline engine
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_B_S2V1_BASE
	movf	CS_MENU_PARAMETER_IX, W
	btfsc	CS_MENU_SELECTED_SID_LR, 0
	addlw -(SID_Ix_B_S2V1_BASE-SID_Ix_B_S1V1_BASE)
	addwf	FSR0L, F
	return

CS_MENU_FSR0_Voice_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_FSR0_Voice_M

CS_MENU_FSR0_Voice_D	; drum engine
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_D_I1_BASE
	movf	CS_MENU_PARAMETER_IX, W
	addwf	FSR0L, F

	movf	CS_MENU_SELECTED_INS, W
	mullw	SID_Ix_D_I2_BASE-SID_Ix_D_I1_BASE
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F
	return

CS_MENU_FSR0_Voice_M	; multi engine
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_M_I1_BASE
	movf	CS_MENU_PARAMETER_IX, W
	addwf	FSR0L, F

	movf	CS_MENU_SELECTED_INS, W
	mullw	SID_Ix_M_I2_BASE-SID_Ix_M_I1_BASE
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F
	return


;; --------------------------------------------------------------------------
;;  returns pointer to Fx register within edit buffer depending on selected filter
;;  CS_MENU_PARAMETER_IX will be added
;; --------------------------------------------------------------------------
CS_MENU_FSR0_Filter
	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_FSR0_Filter_DM
CS_MENU_FSR0_Filter_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_FSR0_Filter_B

CS_MENU_FSR0_Filter_L
	lfsr	FSR0, SID_EDIT_BUFFER

	movlw	SID_Ix_L_S2F_BASE
	btfsc	CS_MENU_SELECTED_SID_LR, 0
	movlw SID_Ix_L_S1F_BASE
	addwf	CS_MENU_PARAMETER_IX, W
	addwf	FSR0L, F
	return

CS_MENU_FSR0_Filter_B	; bassline engine
	rgoto	CS_MENU_FSR0_Filter_L	; same as for lead engine

CS_MENU_FSR0_Filter_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_FSR0_Filter_M

CS_MENU_FSR0_Filter_D	; drum engine
	rgoto	CS_MENU_FSR0_Filter_L	; same as for lead engine

CS_MENU_FSR0_Filter_M	; multi engine
	rgoto	CS_MENU_FSR0_Filter_L	; same as for lead engine


;; --------------------------------------------------------------------------
;;  returns pointer to LFOx register within edit buffer depending on selected LFO
;;  CS_MENU_PARAMETER_IX will be added
;; --------------------------------------------------------------------------
CS_MENU_FSR0_LFO
	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_FSR0_LFO_DM
CS_MENU_FSR0_LFO_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_FSR0_LFO_B

CS_MENU_FSR0_LFO_L
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_LFO1_BASE

	movf	CS_MENU_SELECTED_LFO, W
	mullw	SID_Ix_L_LFO2_BASE-SID_Ix_L_LFO1_BASE
	movf	PRODL, W
	addwf	CS_MENU_PARAMETER_IX, W
	addwf	FSR0L, F
	return

CS_MENU_FSR0_LFO_B	; bassline engine
	rcall	CS_MENU_FSR0_Voice_B

	movf	CS_MENU_SELECTED_LFO, W
	andlw	0x01
	mullw	SID_Ix_B_Vx_LFO2_MODE-SID_Ix_B_Vx_LFO1_MODE
	movf	PRODL, W
	addwf	FSR0L, F
	return

CS_MENU_FSR0_LFO_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_FSR0_LFO_M

CS_MENU_FSR0_LFO_D	; drum engine
	return		; not relevant

CS_MENU_FSR0_LFO_M	; multi engine
	rcall	CS_MENU_FSR0_Voice_M

	movf	CS_MENU_SELECTED_LFO, W
	andlw	0x01
	mullw	SID_Ix_M_Vx_LFO2_MODE-SID_Ix_M_Vx_LFO1_MODE
	movf	PRODL, W
	addwf	FSR0L, F
	return


;; --------------------------------------------------------------------------
;;  returns pointer to ENVx register within edit buffer depending on selected ENV
;;  CS_MENU_PARAMETER_IX will be added
;; --------------------------------------------------------------------------
CS_MENU_FSR0_ENV
	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_FSR0_ENV_DM
CS_MENU_FSR0_ENV_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_FSR0_ENV_B

CS_MENU_FSR0_ENV_L
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_ENV1_BASE

	movf	CS_MENU_SELECTED_ENV, W
	mullw	SID_Ix_L_ENV2_BASE-SID_Ix_L_ENV1_BASE
	movf	PRODL, W
	addwf	CS_MENU_PARAMETER_IX, W
	addwf	FSR0L, F
	return

CS_MENU_FSR0_ENV_B	; bassline engine
	rgoto	CS_MENU_FSR0_Voice_B

CS_MENU_FSR0_ENV_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_FSR0_ENV_M

CS_MENU_FSR0_ENV_D	; drum engine
	return		; not relevant

CS_MENU_FSR0_ENV_M	; multi engine
	rgoto	CS_MENU_FSR0_Voice_M


;; --------------------------------------------------------------------------
;;  returns pointer to MODx register within edit buffer depending on selected MOD
;;  CS_MENU_PARAMETER_IX will be added
;; --------------------------------------------------------------------------
CS_MENU_FSR0_MOD
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_MOD1_BASE

	movf	CS_MENU_SELECTED_MOD, W
	mullw	SID_Ix_L_MOD2_BASE-SID_Ix_L_MOD1_BASE
	movf	PRODL, W
	addwf	CS_MENU_PARAMETER_IX, W
	addwf	FSR0L, F
	return


;; --------------------------------------------------------------------------
;;  returns pointer to MODxNx register within edit buffer depending on selected MOD
;;  CS_MENU_PARAMETER_IX will *not*be added, instead SID_Ix_MODx_TARG_L
;; --------------------------------------------------------------------------
CS_MENU_FSR0_MOD_TARG_L
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_MOD1_BASE

	movf	CS_MENU_SELECTED_MOD, W
	mullw	SID_Ix_L_MOD2_BASE-SID_Ix_L_MOD1_BASE
	movf	PRODL, W
	addlw	SID_Ix_MODx_TARG_L
	addwf	FSR0L, F
	return


;; --------------------------------------------------------------------------
;;  returns pointer to TRGx register within edit buffer depending on selected trigger
;;  CS_MENU_PARAMETER_IX will be added
;; --------------------------------------------------------------------------
CS_MENU_FSR0_TRG
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_TRG_BASE

	movf	CS_MENU_SELECTED_TRG, W
	mullw	3
	movf	PRODL, W
	btfsc	CS_MENU_PARAMETER_IX, 3
	addlw 1
	btfsc	CS_MENU_PARAMETER_IX, 4
	addlw 2
	addwf	FSR0L, F
	return


;; --------------------------------------------------------------------------
;;  returns pointer to Kx register within edit buffer depending on selected Knob
;;  CS_MENU_PARAMETER_IX will be added
;; --------------------------------------------------------------------------
CS_MENU_FSR0_Knob
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_P_K1_BASE

	movf	CS_MENU_SELECTED_KNOB, W
	mullw	SID_Ix_P_K2_BASE-SID_Ix_P_K1_BASE
	movf	PRODL, W
	addwf	CS_MENU_PARAMETER_IX, W
	addwf	FSR0L, F
	return


;; --------------------------------------------------------------------------
;;  returns pointer to WTx register within edit buffer depending on selected wavetable
;;  CS_MENU_PARAMETER_IX will be added
;; --------------------------------------------------------------------------
CS_MENU_FSR0_WT
	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_FSR0_WT_DM
CS_MENU_FSR0_WT_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_FSR0_WT_B

CS_MENU_FSR0_WT_L
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_WT1_BASE

	movf	CS_MENU_SELECTED_WT, W
	mullw	SID_Ix_L_WT2_BASE-SID_Ix_L_WT1_BASE
	movf	PRODL, W
	addwf	CS_MENU_PARAMETER_IX, W
	addwf	FSR0L, F
	return

CS_MENU_FSR0_WT_B	; bassline engine
	rgoto	CS_MENU_FSR0_Voice_B

CS_MENU_FSR0_WT_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_FSR0_WT_M

CS_MENU_FSR0_WT_D	; drum engine
	lfsr	FSR0, SID_EDIT_BUFFER
	movff	CS_MENU_PARAMETER_IX, FSR0L
	return

CS_MENU_FSR0_WT_M	; multi engine
	rgoto	CS_MENU_FSR0_Voice_M


;; --------------------------------------------------------------------------
;;  returns pointer to WT table entry
;;  WT number in CS_MENU_PARAMETER_IX
;;  it sets the ZERO flag if WT entry outside begin/end range
;; --------------------------------------------------------------------------
CS_MENU_FSR0_WTPos
	;; determine starting position
	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_FSR0_WTPos_DM
CS_MENU_FSR0_WTPos_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_FSR0_WTPos_B

CS_MENU_FSR0_WTPos_L
	movf	CS_MENU_PARAMETER_IX, W	; WT number 0..3 in FSR0L
	andlw	0x03
	mullw	SID_Ix_L_WT2_BASE-SID_Ix_L_WT1_BASE
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_WT1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	rgoto	CS_MENU_FSR0_WTPos_Cont

CS_MENU_FSR0_WTPos_B
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES
	swapf	CS_MENU_WT_SEQ, W
	andlw	0x70
	addwf	FSR0L, F
	movf	CS_MENU_WT_POS, W
	andlw	0x0f
	addwf	FSR0L, F
	return

CS_MENU_FSR0_WTPos_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_FSR0_WTPos_M

CS_MENU_FSR0_WTPos_D
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_D_SEQUENCES
	movf	CS_MENU_WT_SEQ, W
	mullw	2*2*8
	movf	PRODL, W
	addwf	FSR0L, F

	movf	CS_MENU_SELECTED_TRK, W
	andlw	0x07
	mullw	4
	movf	PRODL, W
	addwf	FSR0L, F
	return

CS_MENU_FSR0_WTPos_M
	movff	CS_MENU_PARAMETER_IX, TABLAT	; (temporary location)
	movff	CS_MENU_SELECTED_INS, FSR1L	; (temporary location)
	movff	CS_MENU_PARAMETER_IX, CS_MENU_SELECTED_INS
	movlw	SID_Ix_M_Vx_WT_SPEED	; (calculate pointer to first WT parameter entry. offsets compatible with SID_Ix_WTx_*)
	movwf	CS_MENU_PARAMETER_IX
	rcall	CS_MENU_FSR0_Voice_M
	movff	TABLAT, CS_MENU_PARAMETER_IX
	movff	FSR1L, CS_MENU_SELECTED_INS
	movff	FSR0L, FSR1L
	movff	FSR0H, FSR1H
	;; 	rgoto	CS_MENU_FSR0_WTPos_Cont	
	
CS_MENU_FSR0_WTPos_Cont

	movlw	SID_Ix_WTx_BEGIN
	movf	PLUSW1, W
	andlw	0x7f
	movwf	PRODL
	movlw	SID_Ix_WTx_END
	movf	PLUSW1, W
	andlw	0x7f
	movwf	PRODH

	;; now determine WT entry
	;; if outside begin-end range, set FSR0L to 0!

	movf	CS_MENU_WT_POS, W
	andlw	0x7f
	addwf	PRODL, F
	incf	PRODH, W
	cpfslt	PRODL, ACCESS
	rgoto CS_MENU_FSR0_WTPos_Inv
CS_MENU_FSR0_WTPos_Ok
	lfsr	FSR0, SID_EDIT_BUFFER
	movf	PRODL, W
	addlw	LOW(SID_Ix_WAVETABLE)
	movwf	FSR0L
	movlw	HIGH(SID_Ix_WAVETABLE)
	addwfc	FSR0H, F
	iorlw	0xff		; clear zero flag
	return

CS_MENU_FSR0_WTPos_Inv
	andlw	0x00		; set zero flag
	return

;; --------------------------------------------------------------------------
;;  returns the pointer to an ensemble parameter
;;  CS_MENU_PARAMETER_IX will be added
;; --------------------------------------------------------------------------
CS_MENU_FSR0_ENS
	lfsr	FSR0, SID_ENS_BUFFER
	movf	CS_MENU_PARAMETER_IX, W
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  returns the pointer to an ensemble parameter
;;  SID_ENSx_CTRL2 will be added
;; --------------------------------------------------------------------------
CS_MENU_FSR0_ENS_CTRL2
	lfsr	FSR0, SID_ENS_BUFFER
	movlw	SID_ENSx_CTRL2
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  returns the pointer to instrument section of ensemble parameter dependin on CS_MENU_SELECTED_INS
;;  CS_MENU_PARAMETER_IX will be added
;; --------------------------------------------------------------------------
CS_MENU_FSR0_ENS_Ix
	rcall	CS_MENU_FSR0_ENS
	movf	CS_MENU_SELECTED_INS, W
	mullw	SID_ENSx_Ix_RECORD_LEN
	movf	PRODL, W
	addlw	SID_ENSx_I1_BASE
	addwf	FSR0L, F
	return

