; $Id$

; MBFM Control Surface Menu Parameter Copy Functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

; ==========================================================================
; ==========================================================================
;  refered by cs_menu_tables.inc
; ==========================================================================
; ==========================================================================

; ==========================================================================
;  All supported R2PP2R_* indices
;; (max allowed number: 0x3f!)
; ==========================================================================
R2PP2R_NOP		EQU	0x00
R2PP2R_VAR		EQU	0x01
R2PP2R_VAR_ENSINIT	EQU	0x02
R2PP2R_VAR_INS		EQU	0x03
R2PP2R_VAR_OP		EQU	0x04
R2PP2R_VAR_WTSTEP	EQU	0x05
R2PP2R_VAR_WTPAR	EQU	0x06
R2PP2R_VAR_BD_OP	EQU	0x07
R2PP2R_VAR_HH		EQU	0x08
R2PP2R_NAME		EQU	0x09

R2PP2R_Px		EQU	0x0a
R2PP2R_Px_VEL		EQU	0x0b
R2PP2R_PLAY		EQU	0x0c
R2PP2R_SUSK		EQU	0x0d
R2PP2R_PxOx		EQU	0x0e
R2PP2R_PxOxTREM		EQU	0x0f
R2PP2R_PxOxVIBR		EQU	0x10
R2PP2R_PxOxEGT		EQU	0x11
R2PP2R_PxOxKSR		EQU	0x12
R2PP2R_PxLFO		EQU	0x13
R2PP2R_PxLFO_WAVE	EQU	0x14
R2PP2R_PxLFO_SYNC	EQU	0x15
R2PP2R_Px_CAS		EQU	0x16

R2PP2R_SAVE_BANK	EQU	0x17
R2PP2R_SAVE_SLOT	EQU	0x18

R2PP2R_PDxBD		EQU	0x19
R2PP2R_PDxHH		EQU	0x1a
R2PP2R_DRUM_CHN		EQU	0x1b

R2PP2R_I1Px		EQU	0x1c
R2PP2R_I2Px		EQU	0x1d
R2PP2R_I3Px		EQU	0x1e
R2PP2R_I4Px		EQU	0x1f

R2PP2R_AOUTxINST	EQU	0x20
R2PP2R_AOUTxASSG	EQU	0x21
R2PP2R_AOUTxOFFS	EQU	0x22


	;; help macro
R2PP2R_TARGET MACRO p2r_func, r2p_func
	bra	p2r_func
	bra	r2p_func
	ENDM

; ==========================================================================
;  The P2R handler
;; IN: R2PP2R_xxx ID in WREG
; ==========================================================================
CS_MENU_P2R_Handler
	clrc
	rlf	WREG, W
	rgoto	CS_MENU_P2R_Handler_Jmp

; ==========================================================================
;  The R2P handler
;; IN: R2PP2R_xxx ID in WREG
; ==========================================================================
CS_MENU_R2P_Handler
	clrc
	rlf	WREG, W
	addlw	1

CS_MENU_P2R_Handler_Jmp
	JUMPTABLE_2BYTES (2*0x22+2) ; entries
	R2PP2R_TARGET	CS_MENU_P2R_NOP, CS_MENU_R2P_NOP
	R2PP2R_TARGET	CS_MENU_P2R_VAR, CS_MENU_R2P_VAR
	R2PP2R_TARGET	CS_MENU_P2R_VAR_ENSINIT, CS_MENU_R2P_VAR_ENSINIT
	R2PP2R_TARGET	CS_MENU_P2R_VAR_INS, CS_MENU_R2P_VAR_INS
	R2PP2R_TARGET	CS_MENU_P2R_VAR_OP, CS_MENU_R2P_VAR_OP
	R2PP2R_TARGET	CS_MENU_P2R_VAR_WTSTEP, CS_MENU_R2P_VAR_WTSTEP
	R2PP2R_TARGET	CS_MENU_P2R_VAR_WTPAR, CS_MENU_R2P_VAR_WTPAR
	R2PP2R_TARGET	CS_MENU_P2R_VAR_BD_OP, CS_MENU_R2P_VAR_BD_OP
	R2PP2R_TARGET	CS_MENU_P2R_VAR_HH, CS_MENU_R2P_VAR_HH
	R2PP2R_TARGET	CS_MENU_P2R_NAME, CS_MENU_R2P_NAME

	R2PP2R_TARGET	CS_MENU_P2R_Px, CS_MENU_R2P_Px
	R2PP2R_TARGET	CS_MENU_P2R_Px_VEL, CS_MENU_R2P_Px_VEL
	R2PP2R_TARGET	CS_MENU_P2R_PLAY, CS_MENU_R2P_PLAY
	R2PP2R_TARGET	CS_MENU_P2R_SUSK, CS_MENU_R2P_SUSK
	R2PP2R_TARGET	CS_MENU_P2R_PxOx, CS_MENU_R2P_PxOx
	R2PP2R_TARGET	CS_MENU_P2R_PxOxTREM, CS_MENU_R2P_PxOxTREM
	R2PP2R_TARGET	CS_MENU_P2R_PxOxVIBR, CS_MENU_R2P_PxOxVIBR
	R2PP2R_TARGET	CS_MENU_P2R_PxOxEGT, CS_MENU_R2P_PxOxEGT
	R2PP2R_TARGET	CS_MENU_P2R_PxOxKSR, CS_MENU_R2P_PxOxKSR
	R2PP2R_TARGET	CS_MENU_P2R_PxLFO, CS_MENU_R2P_PxLFO
	R2PP2R_TARGET	CS_MENU_P2R_PxLFO_WAVE, CS_MENU_R2P_PxLFO_WAVE
	R2PP2R_TARGET	CS_MENU_P2R_PxLFO_SYNC, CS_MENU_R2P_PxLFO_SYNC
	R2PP2R_TARGET	CS_MENU_P2R_Px_CAS, CS_MENU_R2P_Px_CAS

	R2PP2R_TARGET	CS_MENU_P2R_SAVE_BANK, CS_MENU_R2P_SAVE_BANK
	R2PP2R_TARGET	CS_MENU_P2R_SAVE_SLOT, CS_MENU_R2P_SAVE_SLOT

	R2PP2R_TARGET	CS_MENU_P2R_PDxBD, CS_MENU_R2P_PDxBD
	R2PP2R_TARGET	CS_MENU_P2R_PDxHH, CS_MENU_R2P_PDxHH
	R2PP2R_TARGET	CS_MENU_P2R_DRUM_CHN, CS_MENU_R2P_DRUM_CHN

	R2PP2R_TARGET	CS_MENU_P2R_I1Px, CS_MENU_R2P_I1Px
	R2PP2R_TARGET	CS_MENU_P2R_I2Px, CS_MENU_R2P_I2Px
	R2PP2R_TARGET	CS_MENU_P2R_I3Px, CS_MENU_R2P_I3Px
	R2PP2R_TARGET	CS_MENU_P2R_I4Px, CS_MENU_R2P_I4Px

	R2PP2R_TARGET	CS_MENU_P2R_AOUTxINST, CS_MENU_R2P_AOUTxINST
	R2PP2R_TARGET	CS_MENU_P2R_AOUTxASSG, CS_MENU_R2P_AOUTxASSG
	R2PP2R_TARGET	CS_MENU_P2R_AOUTxOFFS, CS_MENU_R2P_AOUTxOFFS

; --------------------------------------------------------------------------
CS_MENU_P2R_NOP
CS_MENU_R2P_NOP
	return

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR
	rgoto	CS_MENU_P2RR2P_SetVAR

CS_MENU_R2P_VAR
	rcall	CS_MENU_P2RR2P_GetVAR
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_ENSINIT
	rcall	CS_MENU_P2RR2P_SetVAR
	goto	MBFM_ENS_Init

CS_MENU_R2P_VAR_ENSINIT
	rgoto	CS_MENU_R2P_VAR

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_INS
	rcall	CS_MENU_P2RR2P_GetVAR
	movff	CS_MENU_PARAMETER_L, PREINC0
	rgoto	CS_MENU_Hlp_UpdateINSFlags

CS_MENU_R2P_VAR_INS
	incf	CS_MENU_PARAMETER_IX, F	; incr. to *_FLAGS register
	rgoto	CS_MENU_R2P_VAR

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_OP
	rcall	CS_MENU_P2RR2P_GetVAR
	movff	CS_MENU_PARAMETER_L, PREINC0
	rgoto	CS_MENU_Hlp_UpdateOPFlags

CS_MENU_R2P_VAR_OP
	incf	CS_MENU_PARAMETER_IX, F	; incr. to *_FLAGS register
	rgoto	CS_MENU_R2P_VAR

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_WTSTEP
	;; force write to EEPROM before changing pos
	call	CS_MENU_MS_Update_WT_EEPROM
	;; change pos
	rcall	CS_MENU_P2RR2P_SetVAR
	;; update buffer
	rgoto	CS_MENU_MS_Update_WT_Buffer

CS_MENU_R2P_VAR_WTSTEP
	rgoto	CS_MENU_R2P_VAR

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_WTPAR
	movf	CS_MENU_PARAMETER_IX, W	; set modified flag depending on WT parameter
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	iorwf	CS_MENU_WT_MODIFIED, F

	;; request EEPROM update (will be granted appr. one second later by cs_menu_timer.inc)
	bsf	CS_EEPROM_WR, CS_EEPROM_WR_WT_REQ
	rgoto	CS_MENU_P2RR2P_SetWTVAR

CS_MENU_R2P_VAR_WTPAR
	rcall	CS_MENU_P2RR2P_GetWTVAR
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_BD_OP
	rcall	CS_MENU_P2RR2P_GetVAR
	bcf	INDF0, 0
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 0
	return

CS_MENU_R2P_VAR_BD_OP
	rcall	CS_MENU_P2RR2P_GetVAR
	andlw	0x01
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_HH
	rcall	CS_MENU_P2RR2P_GetVAR
	bcf	INDF0, 1
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 1
	return

CS_MENU_R2P_VAR_HH
	rcall	CS_MENU_P2RR2P_GetVAR
	rrf	WREG, W
	andlw	0x01
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_NAME
	call	CS_MENU_MS_GetCurInstrBase
	movf	CS_MENU_NAME_POS, W
	movff	CS_MENU_PARAMETER_L, PLUSW0
	return

CS_MENU_R2P_NAME
	call	CS_MENU_MS_GetCurInstrBase
	movf	CS_MENU_NAME_POS, W
	movf	PLUSW0, W
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Px
	movff	CS_MENU_PARAMETER_L, MIOS_PARAMETER1
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Px
	rcall	CS_MENU_Hlp_GetPxValue
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Px_VEL
	movlw	b'11111100'
	movwf	CS_MENU_P2R_AND_MASK
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x03
	movwf	CS_MENU_P2R_OR_MASK
	rgoto	CS_MENU_P2R_Hlp_ParRMW

CS_MENU_R2P_Px_VEL
	rcall	CS_MENU_Hlp_GetPxValue
	andlw	0x03
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_PLAY
	movlw	b'11111100'
	movwf	CS_MENU_P2R_AND_MASK
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x03
	movwf	CS_MENU_P2R_OR_MASK
	rgoto	CS_MENU_P2R_Hlp_ParRMW

CS_MENU_R2P_PLAY
	rcall	CS_MENU_Hlp_GetPxValue
	andlw	0x03
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_SUSK
	movlw	b'11111011'
	movwf	CS_MENU_P2R_AND_MASK
	movlw	0x00
	btfsc	CS_MENU_PARAMETER_L, 0
	movlw b'00000100'
	movwf	CS_MENU_P2R_OR_MASK
	rgoto	CS_MENU_P2R_Hlp_ParRMW

CS_MENU_R2P_SUSK
	rcall	CS_MENU_Hlp_GetPxValue
	movlw	0x00
	btfsc	INDF0, 2
	movlw 0x01
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_PxOx
	clrf	CS_MENU_P2R_AND_MASK
	movff	CS_MENU_PARAMETER_L, CS_MENU_P2R_OR_MASK

CS_MENU_P2R_PxOx_Set
	;; send selected parameter of OP1
	BRA_IFCLR CS_MENU_SELECTED_OP_FLAGS, 0, ACCESS, CS_MENU_P2R_PxOx_NoOP1
	rcall	CS_MENU_P2R_Hlp_ParRMW
CS_MENU_P2R_PxOx_NoOP1

	;; switch to next OP
	incf	CS_MENU_PARAMETER_IX, F

	;; send selected parameter of OP2
	BRA_IFCLR CS_MENU_SELECTED_OP_FLAGS, 1, ACCESS, CS_MENU_P2R_PxOx_NoOP2
	rcall	CS_MENU_P2R_Hlp_ParRMW
CS_MENU_P2R_PxOx_NoOP2

	;; switch to next OP
	incf	CS_MENU_PARAMETER_IX, F

	;; send selected parameter of OP3
	BRA_IFCLR CS_MENU_SELECTED_OP_FLAGS, 2, ACCESS, CS_MENU_P2R_PxOx_NoOP3
	rcall	CS_MENU_P2R_Hlp_ParRMW
CS_MENU_P2R_PxOx_NoOP3

	;; switch to next OP
	incf	CS_MENU_PARAMETER_IX, F

	;; send selected parameter of OP4
	BRA_IFCLR CS_MENU_SELECTED_OP_FLAGS, 3, ACCESS, CS_MENU_P2R_PxOx_NoOP4
	rcall	CS_MENU_P2R_Hlp_ParRMW
CS_MENU_P2R_PxOx_NoOP4
	return


CS_MENU_R2P_PxOx
	rcall	CS_MENU_Hlp_GetPxOxValue
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_PxOxTREM
	movlw	3
	rgoto	CS_MENU_P2R_PxOxFlags_Cont
CS_MENU_P2R_PxOxVIBR
	movlw	2
	rgoto	CS_MENU_P2R_PxOxFlags_Cont
CS_MENU_P2R_PxOxEGT
	movlw	1
	rgoto	CS_MENU_P2R_PxOxFlags_Cont
CS_MENU_P2R_PxOxKSR
	movlw	0
	;; 	rgoto	CS_MENU_P2R_PxOxFlags_Cont
CS_MENU_P2R_PxOxFlags_Cont
	movwf	TMP1
	call	MIOS_HLP_GetBitANDMask
	movwf	CS_MENU_P2R_AND_MASK
	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask
	btfss	CS_MENU_PARAMETER_L, 0
	movlw 0x00
	movwf	CS_MENU_P2R_OR_MASK
	rgoto	CS_MENU_P2R_PxOx_Set



CS_MENU_R2P_PxOxTREM
	movlw	3
	rgoto	CS_MENU_R2P_PxOxFlags_Cont
CS_MENU_R2P_PxOxVIBR
	movlw	2
	rgoto	CS_MENU_R2P_PxOxFlags_Cont
CS_MENU_R2P_PxOxEGT
	movlw	1
	rgoto	CS_MENU_R2P_PxOxFlags_Cont
CS_MENU_R2P_PxOxKSR
	movlw	0
	rgoto	CS_MENU_R2P_PxOxFlags_Cont
CS_MENU_R2P_PxOxFlags_Cont
	call	MIOS_HLP_GetBitANDMask
	movwf	TMP1
	rcall	CS_MENU_Hlp_GetPxOxValue
	andwf	TMP1, W
	skpz
	movlw	0x01
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_PxLFO
	rcall	CS_MENU_Hlp_AddLFOOffset
	clrf	CS_MENU_P2R_AND_MASK
	movf	CS_MENU_PARAMETER_L, W
	movwf	CS_MENU_P2R_OR_MASK
	rgoto	CS_MENU_P2R_Hlp_ParRMW

CS_MENU_R2P_PxLFO
	rcall	CS_MENU_Hlp_GetPxLFOxValue
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_PxLFO_WAVE
	rcall	CS_MENU_Hlp_AddLFOOffset
	movlw	b'10001110'
	movwf	CS_MENU_P2R_AND_MASK
	decf	CS_MENU_PARAMETER_L, W
	swapf	WREG, W
	andlw	0x70
	movwf	CS_MENU_P2R_OR_MASK
	movf	CS_MENU_PARAMETER_L, W
	skpz
	bsf	CS_MENU_P2R_OR_MASK, 0
	rgoto	CS_MENU_P2R_Hlp_ParRMW

CS_MENU_R2P_PxLFO_WAVE
	rcall	CS_MENU_Hlp_GetPxLFOxValue
	movwf	TMP1
	
	swapf	TMP1, W
	andlw	0x07
	addlw	1
	btfss	TMP1, 0
	movlw 0x00
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_PxLFO_SYNC
	rcall	CS_MENU_Hlp_AddLFOOffset
	movlw	b'11111011'
	movwf	CS_MENU_P2R_AND_MASK
	movlw	0x00
	btfsc	CS_MENU_PARAMETER_L, 0
	movlw b'00000100'
	movwf	CS_MENU_P2R_OR_MASK
	rgoto	CS_MENU_P2R_Hlp_ParRMW

CS_MENU_R2P_PxLFO_SYNC
	rcall	CS_MENU_Hlp_GetPxLFOxValue
	movlw	0x00
	btfsc	INDF0, 2
	movlw 0x01
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Px_CAS
	rcall	CS_MENU_Hlp_AddEGOffset
	movlw	b'11110000'
	movwf	CS_MENU_P2R_AND_MASK
	movf	CS_MENU_PARAMETER_L, W
	movwf	CS_MENU_P2R_OR_MASK
	rgoto	CS_MENU_P2R_Hlp_ParRMW

CS_MENU_R2P_Px_CAS
	rcall	CS_MENU_Hlp_GetPxEGxValue
	andlw	0x0f
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_SAVE_BANK
	movf	CS_MENU_PARAMETER_L, W
	BRA_IFSET CS_MENU_SAVE_TYPE, 0, ACCESS, CS_MENU_P2R_SAVE_BANK_D
	BRA_IFSET CS_MENU_SAVE_TYPE, 1, ACCESS, CS_MENU_P2R_SAVE_BANK_E
CS_MENU_P2R_SAVE_BANK_P
	movwf	CS_MENU_SAVE_BANK
	return
CS_MENU_P2R_SAVE_BANK_D
	movwf	CS_MENU_SAVE_DBANK
	return
CS_MENU_P2R_SAVE_BANK_E
	movwf	CS_MENU_SAVE_EBANK
	return


CS_MENU_R2P_SAVE_BANK
	movf	CS_MENU_SAVE_BANK, W
	btfsc	CS_MENU_SAVE_TYPE, 0
	movf	CS_MENU_SAVE_DBANK, W
	btfsc	CS_MENU_SAVE_TYPE, 1
	movf	CS_MENU_SAVE_EBANK, W
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_SAVE_SLOT
	movf	CS_MENU_PARAMETER_L, W
	BRA_IFSET CS_MENU_SAVE_TYPE, 0, ACCESS, CS_MENU_P2R_SAVE_SLOT_D
	BRA_IFSET CS_MENU_SAVE_TYPE, 1, ACCESS, CS_MENU_P2R_SAVE_SLOT_E
CS_MENU_P2R_SAVE_SLOT_P
	movwf	CS_MENU_SAVE_PATCH
	return
CS_MENU_P2R_SAVE_SLOT_D
	movwf	CS_MENU_SAVE_DRUMSET
	return
CS_MENU_P2R_SAVE_SLOT_E
	movwf	CS_MENU_SAVE_ENSEMBLE
	return


CS_MENU_R2P_SAVE_SLOT
	;; special: the max value is different depending on type
	movlw	0x7f
	btfsc	CS_MENU_SAVE_TYPE, 0
	movlw	0x0f
	btfsc	CS_MENU_SAVE_TYPE, 1
	movlw	0x1f
	movwf	CS_MENU_PARAMETER_MAX_L

	;; get parameter value depending on type
	movf	CS_MENU_SAVE_PATCH, W
	btfsc	CS_MENU_SAVE_TYPE, 0
	movf	CS_MENU_SAVE_DRUMSET, W
	btfsc	CS_MENU_SAVE_TYPE, 1
	movf	CS_MENU_SAVE_ENSEMBLE, W
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_PDxBD
	rcall	CS_MENU_Hlp_AddBDOPOffset
	rgoto	CS_MENU_P2R_Px

CS_MENU_R2P_PDxBD
	rcall	CS_MENU_Hlp_AddBDOPOffset
	rgoto	CS_MENU_R2P_Px

; --------------------------------------------------------------------------
CS_MENU_P2R_PDxHH
	rcall	CS_MENU_Hlp_AddHHOCOffset
	rgoto	CS_MENU_P2R_Px

CS_MENU_R2P_PDxHH
	rcall	CS_MENU_Hlp_AddHHOCOffset
	rgoto	CS_MENU_R2P_Px

; --------------------------------------------------------------------------
CS_MENU_P2R_DRUM_CHN
	movff	CS_MENU_PARAMETER_L, MBFM_DRUM_MIDI_CHANNEL
	return

CS_MENU_R2P_DRUM_CHN
	movff	MBFM_DRUM_MIDI_CHANNEL, WREG
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_I1Px
	movlw	0x00
	rgoto	CS_MENU_P2R_Hlp_ParSendSI

CS_MENU_P2R_I2Px
	movlw	0x01
	rgoto	CS_MENU_P2R_Hlp_ParSendSI

CS_MENU_P2R_I3Px
	movlw	0x02
	rgoto	CS_MENU_P2R_Hlp_ParSendSI

CS_MENU_P2R_I4Px
	movlw	0x03
	rgoto	CS_MENU_P2R_Hlp_ParSendSI



CS_MENU_R2P_I1Px
	movlw	0x00
	movff	WREG, MBFM_CURRENT_INSTRUMENT
	rgoto	CS_MENU_R2P_Px

CS_MENU_R2P_I2Px
	movlw	0x01
	movff	WREG, MBFM_CURRENT_INSTRUMENT
	rgoto	CS_MENU_R2P_Px

CS_MENU_R2P_I3Px
	movlw	0x02
	movff	WREG, MBFM_CURRENT_INSTRUMENT
	rgoto	CS_MENU_R2P_Px

CS_MENU_R2P_I4Px
	movlw	0x03
	movff	WREG, MBFM_CURRENT_INSTRUMENT
	rgoto	CS_MENU_R2P_Px

; --------------------------------------------------------------------------
CS_MENU_P2R_AOUTxINST
	movlw	b'11001111'
	movwf	CS_MENU_P2R_AND_MASK
	swapf	CS_MENU_PARAMETER_L, W
	andlw	0x30
	movwf	CS_MENU_P2R_OR_MASK
	rgoto	CS_MENU_P2R_Hlp_AOUTParRMW

CS_MENU_R2P_AOUTxINST
	rcall	CS_MENU_Hlp_GetPxAOUTxValue
	swapf	WREG, W
	andlw	0x03
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_AOUTxASSG
	movlw	b'11110000'
	movwf	CS_MENU_P2R_AND_MASK
	movf 	CS_MENU_PARAMETER_L, W
	movwf	CS_MENU_P2R_OR_MASK
	rgoto	CS_MENU_P2R_Hlp_AOUTParRMW

CS_MENU_R2P_AOUTxASSG
	rcall	CS_MENU_Hlp_GetPxAOUTxValue
	andlw	0x0f
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_AOUTxOFFS
	clrf	CS_MENU_P2R_AND_MASK
	movf 	CS_MENU_PARAMETER_L, W
	movwf	CS_MENU_P2R_OR_MASK
	rgoto	CS_MENU_P2R_Hlp_AOUTParRMW

CS_MENU_R2P_AOUTxOFFS
	rcall	CS_MENU_Hlp_GetPxAOUTxValue
	rgoto	CS_MENU_R2P_SetPar


	
; ==========================================================================
; ==========================================================================
;  Help Functions
; ==========================================================================
; ==========================================================================

; --------------------------------------------------------------------------
;; get variable (value in WREG, pointer in FSR0)
CS_MENU_P2RR2P_GetVAR
	movff	CS_MENU_PARAMETER_IX, FSR0L
	clrf	FSR0H
	movf	INDF0, W
	return
	
; --------------------------------------------------------------------------
;; set variable
CS_MENU_P2RR2P_SetVAR
	rcall	CS_MENU_P2RR2P_GetVAR
	movff	CS_MENU_PARAMETER_L, INDF0
	return
	
; --------------------------------------------------------------------------
;; get WT variable (value in WREG, pointer in FSR0)
CS_MENU_P2RR2P_GetWTVAR
	lfsr	FSR0, CS_MENU_WT_MODE
	movf	CS_MENU_PARAMETER_IX, W
	addwf	FSR0L, F
	movf	INDF0, W
	return
	
; --------------------------------------------------------------------------
;; set WT variable
CS_MENU_P2RR2P_SetWTVAR
	rcall	CS_MENU_P2RR2P_GetWTVAR
	movff	CS_MENU_PARAMETER_L, INDF0
	return

	
; ==========================================================================
;  P2R help function which is doing a read-modify-write via AND/OR mask
;  on a parameter
; ==========================================================================
CS_MENU_P2R_Hlp_ParRMW
	movf	CS_MENU_PARAMETER_IX, W
	call	MBFM_PAROUT_Get
	movwf	MIOS_PARAMETER1
	movf	CS_MENU_P2R_AND_MASK, W
	andwf	MIOS_PARAMETER1, F
	movf	CS_MENU_P2R_OR_MASK, W
	iorwf	MIOS_PARAMETER1, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

; ==========================================================================
;  P2R help function which is doing a read-modify-write via AND/OR mask
;  on an AOUT parameter
; ==========================================================================
CS_MENU_P2R_Hlp_AOUTParRMW
	rcall	CS_MENU_Hlp_AddAOUTOffset
	movf	CS_MENU_PARAMETER_IX, W
	call	MBFM_AOUT_ParGet
	movwf	MIOS_PARAMETER1
	movf	CS_MENU_P2R_AND_MASK, W
	andwf	MIOS_PARAMETER1, F
	movf	CS_MENU_P2R_OR_MASK, W
	iorwf	MIOS_PARAMETER1, F
	movf	CS_MENU_PARAMETER_IX, W
	goto	MBFM_AOUT_ParSet

; ==========================================================================
;  Misc help functions (save code memory usage)
; ==========================================================================
CS_MENU_R2P_SetPar
	movwf	CS_MENU_PARAMETER_L
	return

; ==========================================================================
;  Function to send a parameter to MBFM_PAR and (optionally) via CC
;  expecting value in MIOS_PARAMETER1
; ==========================================================================
CS_MENU_P2R_Hlp_ParSendSI
	;; send a specific instrument
	movff	WREG, MBFM_CURRENT_INSTRUMENT
	movff	CS_MENU_PARAMETER_L, MIOS_PARAMETER1
	rgoto	CS_MENU_P2R_Hlp_ParSend_Single

CS_MENU_P2R_Hlp_ParSend
	;; if no drum parameter: send changes to all instruments
	BRA_IFCLR CS_MENU_PARAMETER_IX, 7, ACCESS, CS_MENU_P2R_Hlp_ParSend_I
	movlw	0xf0 - 1
	cpfsgt	CS_MENU_PARAMETER_IX, ACCESS
	rgoto CS_MENU_P2R_Hlp_ParSend_Single

CS_MENU_P2R_Hlp_ParSend_I
	;; send to all selected instruments
	SET_BSR	MBFM_BASE
	clrf	MBFM_CURRENT_INSTRUMENT, BANKED
	movff	MIOS_PARAMETER1, CS_MENU_P2R_R2P_TMP
	RCALL_IFSET CS_MENU_SELECTED_INS_FLAGS, 0, ACCESS, CS_MENU_P2R_Hlp_ParSend_Single
	movff	CS_MENU_P2R_R2P_TMP, MIOS_PARAMETER1
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	RCALL_IFSET CS_MENU_SELECTED_INS_FLAGS, 1, ACCESS, CS_MENU_P2R_Hlp_ParSend_Single
	movff	CS_MENU_P2R_R2P_TMP, MIOS_PARAMETER1
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	RCALL_IFSET CS_MENU_SELECTED_INS_FLAGS, 2, ACCESS, CS_MENU_P2R_Hlp_ParSend_Single
	movff	CS_MENU_P2R_R2P_TMP, MIOS_PARAMETER1
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	RCALL_IFSET CS_MENU_SELECTED_INS_FLAGS, 3, ACCESS, CS_MENU_P2R_Hlp_ParSend_Single
	return	

	;;
CS_MENU_P2R_Hlp_ParSend_Single
	BRA_IFCLR CS_MENU_MODE, CS_MENU_MODE_CC, ACCESS, CS_MENU_P2R_Hlp_ParSend_NoCC
CS_MENU_P2R_Hlp_ParSend_CC
	movlw	0x10 - 1
	cpfsgt	CS_MENU_PARAMETER_IX, ACCESS
	rgoto CS_MENU_P2R_Hlp_ParSend_NoCC
	movlw	0xf0
	cpfslt	CS_MENU_PARAMETER_IX, ACCESS
	rgoto CS_MENU_P2R_Hlp_ParSend_NoCC
	BRA_IFSET CS_MENU_PARAMETER_IX, 7, ACCESS, CS_MENU_P2R_Hlp_ParSend_CC_D
CS_MENU_P2R_Hlp_ParSend_CC_I	; send instrument CC
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_MIDI_CHANNEL
	movf	PLUSW1, W
	rgoto	CS_MENU_P2R_Hlp_ParSend_CC_Cont

CS_MENU_P2R_Hlp_ParSend_CC_D	; send drum CC
	movff	MBFM_DRUM_MIDI_CHANNEL, WREG
	;; 	rgoto	CS_MENU_P2R_Hlp_ParSend_CC_Cont

CS_MENU_P2R_Hlp_ParSend_CC_Cont
	andlw	0x0f
	iorlw	0xb0
	call	MIOS_MIDI_TxBufferPut
	movf	CS_MENU_PARAMETER_IX, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movf	MIOS_PARAMETER1, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut

CS_MENU_P2R_Hlp_ParSend_NoCC
	movf	CS_MENU_PARAMETER_IX, W
	goto	MBFM_PARIN_Set
