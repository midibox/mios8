; $Id$

; SID Control Surface Menu Parameter Copy Functions
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

R2PP2R_CC		EQU	0x01

R2PP2R_VAR		EQU	0x02
R2PP2R_VAR_OSC		EQU	0x03
R2PP2R_VAR_WTPOS	EQU	0x04
R2PP2R_VAR_WTPAR	EQU	0x05
R2PP2R_WTASG		EQU	0x06
R2PP2R_VAR_303SG	EQU	0x07
R2PP2R_VAR_303ACC	EQU	0x08
R2PP2R_VAR_303NOTE	EQU	0x09

R2PP2R_Vx		EQU	0x0a
R2PP2R_Vx_WAV		EQU	0x0b
R2PP2R_Vx_S_R		EQU	0x0c
R2PP2R_Vx_OPS		EQU	0x0d

R2PP2R_FILTER_CHN	EQU	0x0e
R2PP2R_FILTER_MOD	EQU	0x0f
R2PP2R_FILTER_EXT	EQU	0x10
R2PP2R_FILTER_3OF	EQU	0x11

R2PP2R_LFOx		EQU	0x12
R2PP2R_LFOx_WAV		EQU	0x13
R2PP2R_LFOx_SYN		EQU	0x14

R2PP2R_ENVx		EQU	0x15
R2PP2R_ENVx_CUR		EQU	0x16
R2PP2R_ENVx_CAS		EQU	0x17

R2PP2R_MOD_N		EQU	0x18

R2PP2R_303_MOD		EQU	0x19
R2PP2R_303_DEC		EQU	0x1a

R2PP2R_CHN		EQU	0x1b
R2PP2R_DEV		EQU	0x1c
R2PP2R_BNK		EQU	0x1d
R2PP2R_PAT		EQU	0x1e
R2PP2R_NAME		EQU	0x1f
R2PP2R_PLY		EQU	0x20

R2PP2R_SUK		EQU	0x21
R2PP2R_KTR		EQU	0x22

R2PP2R_SEO_303		EQU	0x23
R2PP2R_SEO_FIP		EQU	0x24
R2PP2R_SEO_E2P		EQU	0x25
R2PP2R_SEO_E2V		EQU	0x26
R2PP2R_SEO_GSA		EQU	0x27

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
	JUMPTABLE_2BYTES (2*0x27+2) ; entries
	R2PP2R_TARGET	CS_MENU_P2R_NOP, CS_MENU_R2P_NOP

	R2PP2R_TARGET	CS_MENU_P2R_CC, CS_MENU_R2P_CC

	R2PP2R_TARGET	CS_MENU_P2R_VAR, CS_MENU_R2P_VAR
	R2PP2R_TARGET	CS_MENU_P2R_VAR_OSC, CS_MENU_R2P_VAR_OSC
	R2PP2R_TARGET	CS_MENU_P2R_VAR_WTPOS, CS_MENU_R2P_VAR_WTPOS
	R2PP2R_TARGET	CS_MENU_P2R_VAR_WTPAR, CS_MENU_R2P_VAR_WTPAR
	R2PP2R_TARGET	CS_MENU_P2R_WTASG, CS_MENU_R2P_WTASG
	R2PP2R_TARGET	CS_MENU_P2R_VAR_303SG, CS_MENU_R2P_VAR_303SG
	R2PP2R_TARGET	CS_MENU_P2R_VAR_303ACC, CS_MENU_R2P_VAR_303ACC
	R2PP2R_TARGET	CS_MENU_P2R_VAR_303NOTE, CS_MENU_R2P_VAR_303NOTE

	R2PP2R_TARGET	CS_MENU_P2R_Vx, CS_MENU_R2P_Vx
	R2PP2R_TARGET	CS_MENU_P2R_Vx_WAV, CS_MENU_R2P_Vx_WAV
	R2PP2R_TARGET	CS_MENU_P2R_Vx_S_R, CS_MENU_R2P_Vx_S_R
	R2PP2R_TARGET	CS_MENU_P2R_Vx_OPS, CS_MENU_R2P_Vx_OPS

	R2PP2R_TARGET	CS_MENU_P2R_FILTER_CHN, CS_MENU_R2P_FILTER_CHN
	R2PP2R_TARGET	CS_MENU_P2R_FILTER_MOD, CS_MENU_R2P_FILTER_MOD
	R2PP2R_TARGET	CS_MENU_P2R_FILTER_EXT, CS_MENU_R2P_FILTER_EXT
	R2PP2R_TARGET	CS_MENU_P2R_FILTER_3OF, CS_MENU_R2P_FILTER_3OF

	R2PP2R_TARGET	CS_MENU_P2R_LFOx, CS_MENU_R2P_LFOx
	R2PP2R_TARGET	CS_MENU_P2R_LFOx_WAV, CS_MENU_R2P_LFOx_WAV
	R2PP2R_TARGET	CS_MENU_P2R_LFOx_SYN, CS_MENU_R2P_LFOx_SYN

	R2PP2R_TARGET	CS_MENU_P2R_ENVx, CS_MENU_R2P_ENVx
	R2PP2R_TARGET	CS_MENU_P2R_ENVx_CUR, CS_MENU_R2P_ENVx_CUR
	R2PP2R_TARGET	CS_MENU_P2R_ENVx_CAS, CS_MENU_R2P_ENVx_CAS

	R2PP2R_TARGET	CS_MENU_P2R_MOD_N, CS_MENU_R2P_MOD_N

	R2PP2R_TARGET	CS_MENU_P2R_303_MOD, CS_MENU_R2P_303_MOD
	R2PP2R_TARGET	CS_MENU_P2R_303_DEC, CS_MENU_R2P_303_DEC

	R2PP2R_TARGET	CS_MENU_P2R_CHN, CS_MENU_R2P_CHN
	R2PP2R_TARGET	CS_MENU_P2R_DEV, CS_MENU_R2P_DEV
	R2PP2R_TARGET	CS_MENU_P2R_BNK, CS_MENU_R2P_BNK
	R2PP2R_TARGET	CS_MENU_P2R_PAT, CS_MENU_R2P_PAT
	R2PP2R_TARGET	CS_MENU_P2R_NAME, CS_MENU_R2P_NAME
	R2PP2R_TARGET	CS_MENU_P2R_PLY, CS_MENU_R2P_PLY

	R2PP2R_TARGET	CS_MENU_P2R_SUK, CS_MENU_R2P_SUK
	R2PP2R_TARGET	CS_MENU_P2R_KTR, CS_MENU_R2P_KTR

	R2PP2R_TARGET	CS_MENU_P2R_SEO_303, CS_MENU_R2P_SEO_303
	R2PP2R_TARGET	CS_MENU_P2R_SEO_FIP, CS_MENU_R2P_SEO_FIP
	R2PP2R_TARGET	CS_MENU_P2R_SEO_E2P, CS_MENU_R2P_SEO_E2P
	R2PP2R_TARGET	CS_MENU_P2R_SEO_E2V, CS_MENU_R2P_SEO_E2V
	R2PP2R_TARGET	CS_MENU_P2R_SEO_GSA, CS_MENU_R2P_SEO_GSA


; --------------------------------------------------------------------------
CS_MENU_P2R_NOP
CS_MENU_R2P_NOP
	return

; --------------------------------------------------------------------------
CS_MENU_P2R_CC
	movff	CS_MENU_PARAMETER_L, INDF0
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_CC
	movf	INDF0, W
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR
	rgoto	CS_MENU_P2RR2P_SetVAR

CS_MENU_R2P_VAR
	rcall	CS_MENU_P2RR2P_GetVAR
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_OSC
	movff	CS_MENU_PARAMETER_L, CS_MENU_SELECTED_OSC
	rgoto	CS_MENU_Hlp_UpdateOSCFlags	; update of "osc_flags" register

CS_MENU_R2P_VAR_OSC
	movf	CS_MENU_SELECTED_OSC, W
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_WTPOS
	;; update EEPROM before changing pos
	call	CS_MENU_MS_Update_WT_EEPROM
	;; change pos
	movff	CS_MENU_PARAMETER_L, CS_MENU_WT_POS
	;; update buffer
	goto	CS_MENU_MS_Update_WT_Buffer

CS_MENU_R2P_VAR_WTPOS
	movf	CS_MENU_WT_POS, W
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_WTPAR
	movf	FSR0L, W	; set modified flag depending on WT parameter
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	iorwf	CS_MENU_WT_MODIFIED, F

	bsf	CS_STAT2, CS_STAT2_EEPROM_WR_WT_REQ	; will be granted appr. one second later by cs_menu_timer.inc
	rgoto	CS_MENU_P2R_CC

CS_MENU_R2P_VAR_WTPAR
	rgoto	CS_MENU_R2P_CC

; --------------------------------------------------------------------------
CS_MENU_P2R_WTASG
	;; stop WT sequencer to avoid data inconsistencies when CC assignments are changed!!!
	;; (works only with master SID!!!)
	movf	CS_MENU_SID, W
	bnz	CS_MENU_P2R_WTASG_NoMaster
CS_MENU_P2R_WTASG_Master
	SET_BSR	SID_BASE
	bsf	SID_WT_STATE, WT_STATE_STOP, BANKED
CS_MENU_P2R_WTASG_NoMaster
	rgoto	CS_MENU_P2R_CC

CS_MENU_R2P_WTASG
	rgoto	CS_MENU_R2P_CC

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_303SG
	bsf	CS_MENU_WT_MODIFIED, 1	; set modified flag
	bsf	CS_STAT2, CS_STAT2_EEPROM_WR_WT_REQ	; will be granted appr. one second later by cs_menu_timer.inc
	movlw	0xcf
	andwf	INDF0, F
	swapf	CS_MENU_PARAMETER_L, W
	andlw	0x30
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_VAR_303SG
	swapf	INDF0, W
	andlw	0x03
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_303ACC
	bsf	CS_MENU_WT_MODIFIED, 1	; set modified flag
	bsf	CS_STAT2, CS_STAT2_EEPROM_WR_WT_REQ	; will be granted appr. one second later by cs_menu_timer.inc
	movlw	0xf0
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x0f
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_VAR_303ACC
	movf	INDF0, W
	andlw	0x0f
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_303NOTE
	bsf	CS_MENU_WT_MODIFIED, 2	; set modified flag
	bsf	CS_STAT2, CS_STAT2_EEPROM_WR_WT_REQ	; will be granted appr. one second later by cs_menu_timer.inc
	movf	CS_MENU_PARAMETER_L, W
	iorlw	0x80
	movwf	INDF0
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_VAR_303NOTE
	movf	INDF0, W
	andlw	0x7f
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx
	;; add offset to voice
	rcall	CS_MENU_Hlp_AddVoiceOffset
	movff	CS_MENU_PARAMETER_L, INDF0
	rgoto	CS_MENU_P2R_Hlp_ParSend_OSC

CS_MENU_R2P_Vx
	rcall	CS_MENU_Hlp_AddVoiceOffset
	rgoto	CS_MENU_R2P_CC

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_WAV
	rcall	CS_MENU_Hlp_AddVoiceOffset
	movlw	0xe0
	andwf	INDF0, F

	movf	CS_MENU_PARAMETER_L, W
	iorwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	skpnz
	bsf	INDF0, 4
	rgoto	CS_MENU_P2R_Hlp_ParSend_OSC

CS_MENU_R2P_Vx_WAV
	rcall	CS_MENU_Hlp_AddVoiceOffset
	movf	INDF0, W
	andlw	0x07
	btfsc	INDF0, 3
	movlw 0x08
	btfsc	INDF0, 4
	movlw 0x00
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_S_R
	rcall	CS_MENU_Hlp_AddVoiceOffset
	movlw	0x1f
	andwf	INDF0, F
	swapf	CS_MENU_PARAMETER_L, W
	rlf	WREG, W
	andlw	0x60
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend_OSC

CS_MENU_R2P_Vx_S_R
	rcall	CS_MENU_Hlp_AddVoiceOffset
	swapf	INDF0, W
	rrf	WREG, W
	andlw	0x03
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_OPS
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x07
	movwf	INDF0
	rgoto	CS_MENU_P2R_CC

CS_MENU_R2P_Vx_OPS
	rgoto	CS_MENU_R2P_CC

; --------------------------------------------------------------------------
CS_MENU_P2R_FILTER_CHN
CS_MENU_P2R_FILTER_MOD
	movlw	0xf8
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x07
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_FILTER_CHN
CS_MENU_R2P_FILTER_MOD
	movf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_FILTER_EXT
	bcf	INDF0, 3
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 3
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_FILTER_EXT
	movlw	0x00
	btfsc	INDF0, 3
	movlw 0x01
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_FILTER_3OF
	bsf	INDF0, 3
	btfsc	CS_MENU_PARAMETER_L, 0
	bcf	INDF0, 3
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_FILTER_3OF
	movlw	0x01
	btfsc	INDF0, 3
	movlw 0x00
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_LFOx
	rcall	CS_MENU_Hlp_AddLFOOffset
	rgoto	CS_MENU_P2R_CC

CS_MENU_R2P_LFOx
	rcall	CS_MENU_Hlp_AddLFOOffset
	rgoto	CS_MENU_R2P_CC

; --------------------------------------------------------------------------
CS_MENU_P2R_LFOx_WAV
	rcall	CS_MENU_Hlp_AddLFOOffset
	movlw	0x0e
	andwf	INDF0, F

	decf	CS_MENU_PARAMETER_L, W
	swapf	WREG, W
	andlw	0x70
	iorwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	skpz
	bsf	INDF0, 0
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_LFOx_WAV
	rcall	CS_MENU_Hlp_AddLFOOffset
	swapf	INDF0, W
	andlw	0x07
	addlw	1
	btfss	INDF0, 0
	movlw 0x00
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_LFOx_SYN
	rcall	CS_MENU_Hlp_AddLFOOffset
	movlw	0xf3
	andwf	INDF0, F
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 2
	btfsc	CS_MENU_PARAMETER_L, 1
	bsf	INDF0, 3
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_LFOx_SYN
	rcall	CS_MENU_Hlp_AddLFOOffset
	movlw	0x00
	btfsc	INDF0, 2
	movlw 0x01
	btfsc	INDF0, 3
	movlw 0x02
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_ENVx
	rcall	CS_MENU_Hlp_AddENVOffset
	rgoto	CS_MENU_P2R_CC

CS_MENU_R2P_ENVx
	rcall	CS_MENU_Hlp_AddENVOffset
	rgoto	CS_MENU_R2P_CC

; --------------------------------------------------------------------------
CS_MENU_P2R_ENVx_CUR
	movf	CS_MENU_SELECTED_ENV, W
	addwf	FSR0L, F
	rgoto	CS_MENU_P2R_CC

CS_MENU_R2P_ENVx_CUR
	movf	CS_MENU_SELECTED_ENV, W
	addwf	FSR0L, F
	rgoto	CS_MENU_R2P_CC

; --------------------------------------------------------------------------
CS_MENU_P2R_ENVx_CAS
	movlw	0xf0
	btfsc	CS_MENU_SELECTED_ENV, 0
	movlw	0x0f
	andwf	INDF0, F
	
	movf	CS_MENU_PARAMETER_L, W
	btfsc	CS_MENU_SELECTED_ENV, 0
	swapf	CS_MENU_PARAMETER_L, W
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_ENVx_CAS
	movf	INDF0, W
	btfsc	CS_MENU_SELECTED_ENV, 0
	swapf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_MOD_N
	movf	FSR0L, W
	andlw	0x07
	movwf	TMP1
	movlw	0x80
	andwf	FSR0L, F
	movf	TMP1, W
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_MENU_P2R_MOD_E1
	rgoto	CS_MENU_P2R_MOD_E2
	rgoto	CS_MENU_P2R_MOD_L1
	rgoto	CS_MENU_P2R_MOD_L2
	rgoto	CS_MENU_P2R_MOD_L3
	rgoto	CS_MENU_P2R_MOD_L4
	rgoto	CS_MENU_P2R_MOD_L5
	rgoto	CS_MENU_P2R_MOD_L6

CS_MENU_P2R_MOD_E1
	rcall	CS_MENU_Hlp_AddModENVOffset
	rcall	CS_MENU_Hlp_AddModENVBitOffset
	bnz	CS_MENU_P2R_MOD_E1_4
CS_MENU_P2R_MOD_E1_0
	bcf	INDF0, 0
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 0
	rgoto	CS_MENU_MS_SendParameter
CS_MENU_P2R_MOD_E1_4
	bcf	INDF0, 4
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 4
	rgoto	CS_MENU_MS_SendParameter

CS_MENU_P2R_MOD_E2
	rcall	CS_MENU_Hlp_AddModENVOffset
	rcall	CS_MENU_Hlp_AddModENVBitOffset
	bnz	CS_MENU_P2R_MOD_E2_5
CS_MENU_P2R_MOD_E2_0
	bcf	INDF0, 1
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 1
	rgoto	CS_MENU_MS_SendParameter
CS_MENU_P2R_MOD_E2_5
	bcf	INDF0, 5
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 5
	rgoto	CS_MENU_MS_SendParameter

CS_MENU_P2R_MOD_L1
	rcall	CS_MENU_Hlp_AddModLFOOffset
	bcf	INDF0, 0
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 0
	rgoto	CS_MENU_MS_SendParameter

CS_MENU_P2R_MOD_L2
	rcall	CS_MENU_Hlp_AddModLFOOffset
	bcf	INDF0, 1
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 1
	rgoto	CS_MENU_MS_SendParameter

CS_MENU_P2R_MOD_L3
	rcall	CS_MENU_Hlp_AddModLFOOffset
	bcf	INDF0, 2
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 2
	rgoto	CS_MENU_MS_SendParameter

CS_MENU_P2R_MOD_L4
	rcall	CS_MENU_Hlp_AddModLFOOffset
	bcf	INDF0, 3
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 3
	rgoto	CS_MENU_MS_SendParameter

CS_MENU_P2R_MOD_L5
	rcall	CS_MENU_Hlp_AddModLFOOffset
	bcf	INDF0, 4
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 4
	rgoto	CS_MENU_MS_SendParameter

CS_MENU_P2R_MOD_L6
	rcall	CS_MENU_Hlp_AddModLFOOffset
	bcf	INDF0, 5
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 5
	rgoto	CS_MENU_MS_SendParameter




CS_MENU_R2P_MOD_N
	movf	FSR0L, W
	andlw	0x07
	movwf	TMP1
	movlw	0x80
	andwf	FSR0L, F
	movf	TMP1, W
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_MENU_R2P_MOD_E1
	rgoto	CS_MENU_R2P_MOD_E2
	rgoto	CS_MENU_R2P_MOD_L1
	rgoto	CS_MENU_R2P_MOD_L2
	rgoto	CS_MENU_R2P_MOD_L3
	rgoto	CS_MENU_R2P_MOD_L4
	rgoto	CS_MENU_R2P_MOD_L5
	rgoto	CS_MENU_R2P_MOD_L6

CS_MENU_R2P_MOD_E1
	rcall	CS_MENU_Hlp_AddModENVOffset
	rcall	CS_MENU_Hlp_AddModENVBitOffset
	bnz	CS_MENU_R2P_MOD_E1_4
CS_MENU_R2P_MOD_E1_0
	movlw	0x00
	btfsc	INDF0, 0
	movlw 0x01
	rgoto	CS_MENU_R2P_MOD_Cont
CS_MENU_R2P_MOD_E1_4
	movlw	0x00
	btfsc	INDF0, 4
	movlw 0x01
	rgoto	CS_MENU_R2P_MOD_Cont

CS_MENU_R2P_MOD_E2
	rcall	CS_MENU_Hlp_AddModENVOffset
	rcall	CS_MENU_Hlp_AddModENVBitOffset
	bnz	CS_MENU_R2P_MOD_E2_5
CS_MENU_R2P_MOD_E2_0
	movlw	0x00
	btfsc	INDF0, 1
	movlw 0x01
	rgoto	CS_MENU_R2P_MOD_Cont
CS_MENU_R2P_MOD_E2_5
	movlw	0x00
	btfsc	INDF0, 5
	movlw 0x01
	rgoto	CS_MENU_R2P_MOD_Cont

CS_MENU_R2P_MOD_L1
	rcall	CS_MENU_Hlp_AddModLFOOffset
	movlw	0x00
	btfsc	INDF0, 0
	movlw 0x01
	rgoto	CS_MENU_R2P_MOD_Cont

CS_MENU_R2P_MOD_L2
	rcall	CS_MENU_Hlp_AddModLFOOffset
	movlw	0x00
	btfsc	INDF0, 1
	movlw 0x01
	rgoto	CS_MENU_R2P_MOD_Cont

CS_MENU_R2P_MOD_L3
	rcall	CS_MENU_Hlp_AddModLFOOffset
	movlw	0x00
	btfsc	INDF0, 2
	movlw 0x01
	rgoto	CS_MENU_R2P_MOD_Cont

CS_MENU_R2P_MOD_L4
	rcall	CS_MENU_Hlp_AddModLFOOffset
	movlw	0x00
	btfsc	INDF0, 3
	movlw 0x01
	rgoto	CS_MENU_R2P_MOD_Cont

CS_MENU_R2P_MOD_L5
	rcall	CS_MENU_Hlp_AddModLFOOffset
	movlw	0x00
	btfsc	INDF0, 4
	movlw 0x01
	rgoto	CS_MENU_R2P_MOD_Cont

CS_MENU_R2P_MOD_L6
	rcall	CS_MENU_Hlp_AddModLFOOffset
	movlw	0x00
	btfsc	INDF0, 5
	movlw 0x01
	rgoto	CS_MENU_R2P_MOD_Cont


CS_MENU_R2P_MOD_Cont
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H
	return

; --------------------------------------------------------------------------
CS_MENU_P2R_303_MOD
CS_MENU_P2R_303_DEC
	rcall	CS_MENU_Hlp_AddENV1Offset
	rgoto	CS_MENU_P2R_CC

CS_MENU_R2P_303_MOD
CS_MENU_R2P_303_DEC
	rcall	CS_MENU_Hlp_AddENV1Offset
	rgoto	CS_MENU_R2P_CC

; --------------------------------------------------------------------------
CS_MENU_P2R_CHN
	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	andlw	0x03
	addwf	FSR1L, F
	movff	CS_MENU_PARAMETER_L, INDF1
	movf	CS_MENU_SID, W
	goto	CS_MENU_MS_Send_SysExDump

CS_MENU_R2P_CHN
	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	andlw	0x03
	addwf	FSR1L, F
	movf	INDF1, W
	andlw	0x0f
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_DEV
#if AUTO_DEVICE_ID == 0
	movf	CS_MENU_SID, W
	andlw	0x03
	skpnz
	movff	CS_MENU_PARAMETER_L, SID_MIDI_DEVICE
#endif
	return

CS_MENU_R2P_DEV
	movf	SID_MIDI_DEVICE, W
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_BNK
	call	CS_MENU_MS_GetSIDBank
	movff	CS_MENU_PARAMETER_L, INDF1
	rcall	CS_MENU_P2R_CFG_Hlp_PatchUpdate
	;; 	rgoto	CS_MENU_R2P_BNK

CS_MENU_R2P_BNK
	call	CS_MENU_MS_GetSIDBank
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_PAT
	call	CS_MENU_MS_GetSIDPatch
	movff	CS_MENU_PARAMETER_L, INDF1
	rcall	CS_MENU_P2R_CFG_Hlp_PatchUpdate
	;; 	rgoto	CS_MENU_R2P_PAT

CS_MENU_R2P_PAT
	call	CS_MENU_MS_GetSIDPatch
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_NAME
	movf	CS_MENU_NAME_POS, W
	movff	CS_MENU_PARAMETER_L, PLUSW0
	return

CS_MENU_R2P_NAME
	movf	CS_MENU_NAME_POS, W
	movf	PLUSW0, W
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_PLY
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x07
	movwf	INDF0
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_PLY
	movf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_SUK
	bcf	INDF0, 0
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 0
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_SUK
	movf	INDF0, W
	andlw	0x01
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_KTR
	movlw	0x01
	andwf	INDF0, F
	rlf	CS_MENU_PARAMETER_L, W
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_KTR
	rrf	INDF0, W
	andlw	0x3f
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_SEO_303
	bcf	INDF0, SE_OPTION_TB303
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, SE_OPTION_TB303
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_SEO_303
	movlw	0x00
	btfsc	INDF0, SE_OPTION_TB303
	movlw	0x01
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_SEO_FIP
	bcf	INDF0, SE_OPTION_FIP
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, SE_OPTION_FIP
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_SEO_FIP
	movlw	0x00
	btfsc	INDF0, SE_OPTION_FIP
	movlw	0x01
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_SEO_E2P
	bcf	INDF0, SE_OPTION_ENV2PORTA
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, SE_OPTION_ENV2PORTA
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_SEO_E2P
	movlw	0x00
	btfsc	INDF0, SE_OPTION_ENV2PORTA
	movlw	0x01
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_SEO_E2V
	bcf	INDF0, SE_OPTION_ENV2VOL
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, SE_OPTION_ENV2VOL
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_SEO_E2V
	movlw	0x00
	btfsc	INDF0, SE_OPTION_ENV2VOL
	movlw	0x01
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_SEO_GSA
	bcf	INDF0, SE_OPTION_GSA
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, SE_OPTION_GSA
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_SEO_GSA
	movlw	0x00
	btfsc	INDF0, SE_OPTION_GSA
	movlw	0x01
	rgoto	CS_MENU_R2P_SetPar

; ==========================================================================
; ==========================================================================
;  Help Functions
; ==========================================================================
; ==========================================================================

; --------------------------------------------------------------------------
;; get variable (value in WREG, pointer in FSR0)
CS_MENU_P2RR2P_GetVAR
	clrf	FSR0H
	bcf	FSR0L, 7
	movf	INDF0, W
	return
	
; --------------------------------------------------------------------------
;; set variable
CS_MENU_P2RR2P_SetVAR
	rcall	CS_MENU_P2RR2P_GetVAR
	movff	CS_MENU_PARAMETER_L, INDF0
	return
	
; --------------------------------------------------------------------------
;; set common value
CS_MENU_R2P_SetPar
	movwf	CS_MENU_PARAMETER_L
	return
	
CS_MENU_P2R_CFG_Hlp_PatchUpdate
	call	CS_MENU_MS_GetSIDPatch
	bcf	INDF1, 7	; force save of patch/bank
	movff	INDF1, CS_MENU_PARAMETER_L
	goto	CS_MENU_PatchUpdateSingle

; ==========================================================================
;  Function to send a parameter to all selected SIDs
;  expecting value in MIOS_PARAMETER1
; ==========================================================================
CS_MENU_P2R_Hlp_ParSend
	goto	CS_MENU_MS_SendParameter

; ==========================================================================
;  Function to send a parameter to all selected SIDs and OSCs
;  expecting value in MIOS_PARAMETER1
; ==========================================================================
CS_MENU_P2R_Hlp_ParSend_OSC
	;; save value
	movff	INDF0, CS_MENU_P2R_R2P_TMP

	;; send to all selected voices
	movlw	0x8f
	andwf	FSR0L, F
	movlw	CS_SID_VOICE1_OFFSET
	addwf	FSR0L, F

	;; send selected parameter of OSC1
	BRA_IFCLR CS_MENU_SELECTED_OSC_FLAGS, 0, ACCESS, CS_MENU_P2R_Hlp_ParSend_NoOSC1
CS_MENU_P2R_Hlp_ParSend_OSC1
	movff	CS_MENU_P2R_R2P_TMP, INDF0
	rcall	CS_MENU_P2R_Hlp_ParSend
CS_MENU_P2R_Hlp_ParSend_NoOSC1

	;; switch to next OSC
	movlw	0x10
	addwf	FSR0L, F

	;; send selected parameter of OSC2
	BRA_IFCLR CS_MENU_SELECTED_OSC_FLAGS, 1, ACCESS, CS_MENU_P2R_Hlp_ParSend_NoOSC2
CS_MENU_P2R_Hlp_ParSend_OSC2
	movff	CS_MENU_P2R_R2P_TMP, INDF0
	rcall	CS_MENU_P2R_Hlp_ParSend
CS_MENU_P2R_Hlp_ParSend_NoOSC2

	;; switch to next OSC
	movlw	0x10
	addwf	FSR0L, F

	;; send selected parameter of OSC3
	BRA_IFCLR CS_MENU_SELECTED_OSC_FLAGS, 2, ACCESS, CS_MENU_P2R_Hlp_ParSend_NoOSC3
CS_MENU_P2R_Hlp_ParSend_OSC3
	movff	CS_MENU_P2R_R2P_TMP, INDF0
	rcall	CS_MENU_P2R_Hlp_ParSend
CS_MENU_P2R_Hlp_ParSend_NoOSC3
	return

