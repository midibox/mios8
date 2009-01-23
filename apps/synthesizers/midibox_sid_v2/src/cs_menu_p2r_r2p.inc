; $Id$
;
; SID Control Surface Menu Parameter Copy Functions
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
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
; ==========================================================================
R2PP2R_NOP		EQU	0x00

R2PP2R_PAR8		EQU	0x01
R2PP2R_PAR16		EQU	0x02

R2PP2R_VAR		EQU	0x03
R2PP2R_VAR_SID		EQU	0x04
R2PP2R_VAR_OSC		EQU	0x05
R2PP2R_VAR_SIDLR	EQU	0x06
R2PP2R_VAR_CALI		EQU	0x07
R2PP2R_SAVE_NAME	EQU	0x08
R2PP2R_EDIT_SEQ		EQU	0x09

R2PP2R_Vx		EQU	0x0a
R2PP2R_Vx_4U		EQU	0x0b
R2PP2R_Vx_4L		EQU	0x0c
R2PP2R_Vx_PMD		EQU	0x0d
R2PP2R_Vx_PM3		EQU	0x0e
R2PP2R_Vx_GSA		EQU	0x0f
R2PP2R_Vx_12		EQU	0x10
R2PP2R_Vx_WAV		EQU	0x11
R2PP2R_Vx_S_R		EQU	0x12
R2PP2R_Vx_ARP_ON	EQU	0x13
R2PP2R_Vx_ARP_DIR	EQU	0x14
R2PP2R_Vx_ARP_SOR	EQU	0x15
R2PP2R_Vx_ARP_HLD	EQU	0x16
R2PP2R_Vx_ARP_SNC	EQU	0x17
R2PP2R_Vx_ARP_CAC	EQU	0x18
R2PP2R_Vx_ARP_GLN	EQU	0x19
R2PP2R_Vx_ARP_RNG	EQU	0x1a
R2PP2R_Vx_ARP_SPD	EQU	0x1b
R2PP2R_Vx_ARP_ONE	EQU	0x1c

R2PP2R_M_Vx_PHASE	EQU	0x1d
R2PP2R_M_Vx_POL		EQU	0x1e
R2PP2R_BM_Vx_LEG	EQU	0x1f
R2PP2R_M_Vx_WTO		EQU	0x20
R2PP2R_BM_Vx_SUK	EQU	0x21
R2PP2R_M_Vx_VAS		EQU	0x22
R2PP2R_M_D_Vx_ASG	EQU	0x23

R2PP2R_B_Vx_MODE	EQU	0x24
R2PP2R_B_Vx_SEQ_SPD	EQU	0x25
R2PP2R_B_Vx_SEQ_S16	EQU	0x26

R2PP2R_D_Vx_VAS		EQU	0x27

R2PP2R_Fx_CHN		EQU	0x28
R2PP2R_Fx_CUTOFF	EQU	0x29
R2PP2R_Fx_RESON		EQU	0x2a
R2PP2R_Fx_KTR		EQU	0x2b
R2PP2R_Fx_MOD		EQU	0x2c
R2PP2R_Fx_EXT		EQU	0x2d
R2PP2R_Fx_3OF		EQU	0x2e
R2PP2R_Fx_FIP		EQU	0x2f

R2PP2R_LFOx		EQU	0x30
R2PP2R_LFOx_WAV		EQU	0x31
R2PP2R_LFOx_SNC		EQU	0x32
R2PP2R_LFOx_SNC_M	EQU	0x33
R2PP2R_LFOx_ONE		EQU	0x34
R2PP2R_LFOx_CSN		EQU	0x35

R2PP2R_ENVx		EQU	0x36
R2PP2R_ENVx_LPB		EQU	0x37
R2PP2R_ENVx_LPE		EQU	0x38
R2PP2R_ENVx_CSN		EQU	0x39
R2PP2R_ENVx_CAS		EQU	0x3a

R2PP2R_MODx		EQU	0x3b
R2PP2R_MODx_SRC		EQU	0x3c
R2PP2R_MODx_OP		EQU	0x3d
R2PP2R_MODx_INV1	EQU	0x3e
R2PP2R_MODx_INV2	EQU	0x3f
R2PP2R_MODx_Nx		EQU	0x40

R2PP2R_TRGx_Nx		EQU	0x41

R2PP2R_CHN		EQU	0x42
R2PP2R_BNK		EQU	0x43
R2PP2R_PAT		EQU	0x44
R2PP2R_B0		EQU	0x45
R2PP2R_B1		EQU	0x46
R2PP2R_B2		EQU	0x47

R2PP2R_CLK_BPM		EQU	0x48
R2PP2R_CLK_SLAVE	EQU	0x49

R2PP2R_KNB_VALUE	EQU	0x4a
R2PP2R_KNB_P		EQU	0x4b
R2PP2R_KNB_ASG		EQU	0x4c

R2PP2R_EXT_VALUE	EQU	0x4d
R2PP2R_EXT_SWITCH	EQU	0x4e

R2PP2R_WTx_PAR		EQU	0x4f
R2PP2R_WTx_SID		EQU	0x50
R2PP2R_WTx_SPD		EQU	0x51
R2PP2R_WTx_B6		EQU	0x52
R2PP2R_WTx_B7		EQU	0x53
R2PP2R_WTx_POS		EQU	0x54
R2PP2R_WTx_LOP		EQU	0x55

R2PP2R_WTE_PAR		EQU	0x56
R2PP2R_WT_VAL		EQU	0x57

R2PP2R_B_SEQ_VAL	EQU	0x58

R2PP2R_ENSx_VAL		EQU	0x59
R2PP2R_ENSxIx_VAL	EQU	0x5a
R2PP2R_ENSx_X2A		EQU	0x5b
R2PP2R_ENSx_RESERVED	EQU	0x5c
R2PP2R_ENSx_MONO	EQU	0x5d
R2PP2R_ENSx_FILLOG	EQU	0x5e
R2PP2R_ENSx_DOR		EQU	0x5f
R2PP2R_ENSx_SPV		EQU	0x60
R2PP2R_ENSx_EXTINV	EQU	0x61

R2PP2R_303_CUTOFF	EQU	0x62
R2PP2R_303_DEP		EQU	0x63
R2PP2R_303_DEC		EQU	0x64


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
	mullw	4
	rgoto	CS_MENU_P2R_Handler_Jmp

; ==========================================================================
;  The R2P handler
;; IN: R2PP2R_xxx ID in WREG
; ==========================================================================
CS_MENU_R2P_Handler
	mullw	4
	movlw	2
	addwf	PRODL, F

CS_MENU_P2R_Handler_Jmp
	call	CS_MENU_P2RR2P_GetIndex_2bytes	; special variant of MIOS_HLP_GetIndex_2bytes which allows to add 16bit values (PROD[LH])
	R2PP2R_TARGET	CS_MENU_P2R_NOP, CS_MENU_R2P_NOP

	R2PP2R_TARGET	CS_MENU_P2R_PAR8, CS_MENU_R2P_PAR8
	R2PP2R_TARGET	CS_MENU_P2R_PAR16, CS_MENU_R2P_PAR16

	R2PP2R_TARGET	CS_MENU_P2R_VAR, CS_MENU_R2P_VAR
	R2PP2R_TARGET	CS_MENU_P2R_VAR_SID, CS_MENU_R2P_VAR_SID
	R2PP2R_TARGET	CS_MENU_P2R_VAR_OSC, CS_MENU_R2P_VAR_OSC
	R2PP2R_TARGET	CS_MENU_P2R_VAR_SIDLR, CS_MENU_R2P_VAR_SIDLR
	R2PP2R_TARGET	CS_MENU_P2R_VAR_CALI, CS_MENU_R2P_VAR_CALI
	R2PP2R_TARGET	CS_MENU_P2R_SAVE_NAME, CS_MENU_R2P_SAVE_NAME
	R2PP2R_TARGET	CS_MENU_P2R_EDIT_SEQ, CS_MENU_R2P_EDIT_SEQ

	R2PP2R_TARGET	CS_MENU_P2R_Vx, CS_MENU_R2P_Vx
	R2PP2R_TARGET	CS_MENU_P2R_Vx_4U, CS_MENU_R2P_Vx_4U
	R2PP2R_TARGET	CS_MENU_P2R_Vx_4L, CS_MENU_R2P_Vx_4L
	R2PP2R_TARGET	CS_MENU_P2R_Vx_PMD,CS_MENU_R2P_Vx_PMD
	R2PP2R_TARGET	CS_MENU_P2R_Vx_PM3,CS_MENU_R2P_Vx_PM3
	R2PP2R_TARGET	CS_MENU_P2R_Vx_GSA,CS_MENU_R2P_Vx_GSA
	R2PP2R_TARGET	CS_MENU_P2R_Vx_12, CS_MENU_R2P_Vx_12
	R2PP2R_TARGET	CS_MENU_P2R_Vx_WAV, CS_MENU_R2P_Vx_WAV
	R2PP2R_TARGET	CS_MENU_P2R_Vx_S_R, CS_MENU_R2P_Vx_S_R
	R2PP2R_TARGET	CS_MENU_P2R_Vx_ARP_ON, CS_MENU_R2P_Vx_ARP_ON
	R2PP2R_TARGET	CS_MENU_P2R_Vx_ARP_DIR, CS_MENU_R2P_Vx_ARP_DIR
	R2PP2R_TARGET	CS_MENU_P2R_Vx_ARP_SOR, CS_MENU_R2P_Vx_ARP_SOR
	R2PP2R_TARGET	CS_MENU_P2R_Vx_ARP_HLD, CS_MENU_R2P_Vx_ARP_HLD
	R2PP2R_TARGET	CS_MENU_P2R_Vx_ARP_SNC, CS_MENU_R2P_Vx_ARP_SNC
	R2PP2R_TARGET	CS_MENU_P2R_Vx_ARP_CAC, CS_MENU_R2P_Vx_ARP_CAC
	R2PP2R_TARGET	CS_MENU_P2R_Vx_ARP_GLN, CS_MENU_R2P_Vx_ARP_GLN
	R2PP2R_TARGET	CS_MENU_P2R_Vx_ARP_RNG, CS_MENU_R2P_Vx_ARP_RNG
	R2PP2R_TARGET	CS_MENU_P2R_Vx_ARP_SPD, CS_MENU_R2P_Vx_ARP_SPD
	R2PP2R_TARGET	CS_MENU_P2R_Vx_ARP_ONE, CS_MENU_R2P_Vx_ARP_ONE

	R2PP2R_TARGET	CS_MENU_P2R_M_Vx_PHASE, CS_MENU_R2P_M_Vx_PHASE
	R2PP2R_TARGET	CS_MENU_P2R_M_Vx_POL, CS_MENU_R2P_M_Vx_POL
	R2PP2R_TARGET	CS_MENU_P2R_BM_Vx_LEG, CS_MENU_R2P_BM_Vx_LEG
	R2PP2R_TARGET	CS_MENU_P2R_M_Vx_WTO, CS_MENU_R2P_M_Vx_WTO
	R2PP2R_TARGET	CS_MENU_P2R_BM_Vx_SUK, CS_MENU_R2P_BM_Vx_SUK
	R2PP2R_TARGET	CS_MENU_P2R_M_Vx_VAS, CS_MENU_R2P_M_Vx_VAS
	R2PP2R_TARGET	CS_MENU_P2R_M_D_Vx_ASG, CS_MENU_R2P_M_D_Vx_ASG

	R2PP2R_TARGET	CS_MENU_P2R_B_Vx_MODE, CS_MENU_R2P_B_Vx_MODE
	R2PP2R_TARGET	CS_MENU_P2R_B_Vx_SEQ_SPD, CS_MENU_R2P_B_Vx_SEQ_SPD
	R2PP2R_TARGET	CS_MENU_P2R_B_Vx_SEQ_S16, CS_MENU_R2P_B_Vx_SEQ_S16

	R2PP2R_TARGET	CS_MENU_P2R_D_Vx_VAS, CS_MENU_R2P_D_Vx_VAS

	R2PP2R_TARGET	CS_MENU_P2R_Fx_CHN, CS_MENU_R2P_Fx_CHN
	R2PP2R_TARGET	CS_MENU_P2R_Fx_CUTOFF, CS_MENU_R2P_Fx_CUTOFF
	R2PP2R_TARGET	CS_MENU_P2R_Fx_RESON, CS_MENU_R2P_Fx_RESON
	R2PP2R_TARGET	CS_MENU_P2R_Fx_KTR, CS_MENU_R2P_Fx_KTR
	R2PP2R_TARGET	CS_MENU_P2R_Fx_MOD, CS_MENU_R2P_Fx_MOD
	R2PP2R_TARGET	CS_MENU_P2R_Fx_EXT, CS_MENU_R2P_Fx_EXT
	R2PP2R_TARGET	CS_MENU_P2R_Fx_3OF, CS_MENU_R2P_Fx_3OF
	R2PP2R_TARGET	CS_MENU_P2R_Fx_FIP, CS_MENU_R2P_Fx_FIP

	R2PP2R_TARGET	CS_MENU_P2R_LFOx, CS_MENU_R2P_LFOx
	R2PP2R_TARGET	CS_MENU_P2R_LFOx_WAV, CS_MENU_R2P_LFOx_WAV
	R2PP2R_TARGET	CS_MENU_P2R_LFOx_SNC, CS_MENU_R2P_LFOx_SNC
	R2PP2R_TARGET	CS_MENU_P2R_LFOx_SNC_M, CS_MENU_R2P_LFOx_SNC_M
	R2PP2R_TARGET	CS_MENU_P2R_LFOx_ONE, CS_MENU_R2P_LFOx_ONE
	R2PP2R_TARGET	CS_MENU_P2R_LFOx_CSN, CS_MENU_R2P_LFOx_CSN

	R2PP2R_TARGET	CS_MENU_P2R_ENVx, CS_MENU_R2P_ENVx
	R2PP2R_TARGET	CS_MENU_P2R_ENVx_LPB, CS_MENU_R2P_ENVx_LPB
	R2PP2R_TARGET	CS_MENU_P2R_ENVx_LPE, CS_MENU_R2P_ENVx_LPE
	R2PP2R_TARGET	CS_MENU_P2R_ENVx_CSN, CS_MENU_R2P_ENVx_CSN
	R2PP2R_TARGET	CS_MENU_P2R_ENVx_CAS, CS_MENU_R2P_ENVx_CAS

	R2PP2R_TARGET	CS_MENU_P2R_MODx, CS_MENU_R2P_MODx
	R2PP2R_TARGET	CS_MENU_P2R_MODx_SRC, CS_MENU_R2P_MODx_SRC
	R2PP2R_TARGET	CS_MENU_P2R_MODx_OP, CS_MENU_R2P_MODx_OP
	R2PP2R_TARGET	CS_MENU_P2R_MODx_INV1, CS_MENU_R2P_MODx_INV1
	R2PP2R_TARGET	CS_MENU_P2R_MODx_INV2, CS_MENU_R2P_MODx_INV2
	R2PP2R_TARGET	_CS_MENU_P2R_MODx_Nx, _CS_MENU_R2P_MODx_Nx

	R2PP2R_TARGET	_CS_MENU_P2R_TRGx_Nx, _CS_MENU_R2P_TRGx_Nx

	R2PP2R_TARGET	CS_MENU_P2R_CHN, CS_MENU_R2P_CHN
	R2PP2R_TARGET	CS_MENU_P2R_BNK, CS_MENU_R2P_BNK
	R2PP2R_TARGET	CS_MENU_P2R_PAT, CS_MENU_R2P_PAT
	R2PP2R_TARGET	CS_MENU_P2R_B0, CS_MENU_R2P_B0
	R2PP2R_TARGET	CS_MENU_P2R_B1, CS_MENU_R2P_B1
	R2PP2R_TARGET	CS_MENU_P2R_B2, CS_MENU_R2P_B2

	R2PP2R_TARGET	CS_MENU_P2R_CLK_BPM, CS_MENU_R2P_CLK_BPM
	R2PP2R_TARGET	CS_MENU_P2R_CLK_SLAVE, CS_MENU_R2P_CLK_SLAVE

	R2PP2R_TARGET	CS_MENU_P2R_KNB_VALUE, CS_MENU_R2P_KNB_VALUE
	R2PP2R_TARGET	CS_MENU_P2R_KNB_P, CS_MENU_R2P_KNB_P
	R2PP2R_TARGET	CS_MENU_P2R_KNB_ASG, CS_MENU_R2P_KNB_ASG

	R2PP2R_TARGET	CS_MENU_P2R_EXT_VALUE, CS_MENU_R2P_EXT_VALUE
	R2PP2R_TARGET	CS_MENU_P2R_EXT_SWITCH, CS_MENU_R2P_EXT_SWITCH

	R2PP2R_TARGET	CS_MENU_P2R_WTx_PAR, CS_MENU_R2P_WTx_PAR
	R2PP2R_TARGET	CS_MENU_P2R_WTx_SID, CS_MENU_R2P_WTx_SID
	R2PP2R_TARGET	CS_MENU_P2R_WTx_SPD, CS_MENU_R2P_WTx_SPD
	R2PP2R_TARGET	CS_MENU_P2R_WTx_B6, CS_MENU_R2P_WTx_B6
	R2PP2R_TARGET	CS_MENU_P2R_WTx_B7, CS_MENU_R2P_WTx_B7
	R2PP2R_TARGET	CS_MENU_P2R_WTx_POS, CS_MENU_R2P_WTx_POS
	R2PP2R_TARGET	CS_MENU_P2R_WTx_LOP, CS_MENU_R2P_WTx_LOP

	R2PP2R_TARGET	CS_MENU_P2R_WTE_PAR, CS_MENU_R2P_WTE_PAR
	R2PP2R_TARGET	CS_MENU_P2R_WT_VAL, CS_MENU_R2P_WT_VAL

	R2PP2R_TARGET	CS_MENU_P2R_B_SEQ_VAL, CS_MENU_R2P_B_SEQ_VAL

	R2PP2R_TARGET	CS_MENU_P2R_ENSx_VAL, CS_MENU_R2P_ENSx_VAL
	R2PP2R_TARGET	CS_MENU_P2R_ENSxIx_VAL, CS_MENU_R2P_ENSxIx_VAL
	R2PP2R_TARGET	CS_MENU_P2R_ENSx_X2A, CS_MENU_R2P_ENSx_X2A
	R2PP2R_TARGET	CS_MENU_P2R_ENSx_RESERVED, CS_MENU_R2P_ENSx_RESERVED
	R2PP2R_TARGET	CS_MENU_P2R_ENSx_MONO, CS_MENU_R2P_ENSx_MONO
	R2PP2R_TARGET	CS_MENU_P2R_ENSx_FILLOG, CS_MENU_R2P_ENSx_FILLOG
	R2PP2R_TARGET	CS_MENU_P2R_ENSx_DOR, CS_MENU_R2P_ENSx_DOR
	R2PP2R_TARGET	CS_MENU_P2R_ENSx_SPV, CS_MENU_R2P_ENSx_SPV
	R2PP2R_TARGET	CS_MENU_P2R_ENSx_EXTINV, CS_MENU_R2P_ENSx_EXTINV

	R2PP2R_TARGET	CS_MENU_P2R_303_CUTOFF, CS_MENU_R2P_303_CUTOFF
	R2PP2R_TARGET	CS_MENU_P2R_303_DEP, CS_MENU_R2P_303_DEP
	R2PP2R_TARGET	CS_MENU_P2R_303_DEC, CS_MENU_R2P_303_DEC


; --------------------------------------------------------------------------
; Macros used for passing parameter values to the selected SIDs
; --------------------------------------------------------------------------
CS_MENU_P2R_VX_SEND_MACRO MACRO
	;; dirty trick:	this call puts the address behind the instruction into the caller stack
	;; the address is visible in TOS[LHU] and will be taken by CS_MENU_P2R_Vx_Send to determine
	;; the function which should be called multiple times
	;; at the end a "pop" instruction will be executed, which turns the "call" into a "goto"!
	rcall	CS_MENU_P2R_Vx_Send
	ENDM

CS_MENU_P2R_FX_SEND_MACRO MACRO
	;; see comments for CS_MENU_P2R_VX_SEND_MACRO
	rcall	CS_MENU_P2R_Fx_Send
	ENDM

CS_MENU_P2R_LFOX_SEND_MACRO MACRO
	;; see comments for CS_MENU_P2R_VX_SEND_MACRO
	rcall	CS_MENU_P2R_LFOx_Send
	ENDM

CS_MENU_P2R_ENVX_SEND_MACRO MACRO
	;; see comments for CS_MENU_P2R_VX_SEND_MACRO
	rcall	CS_MENU_P2R_ENVx_Send
	ENDM


; --------------------------------------------------------------------------
CS_MENU_P2R_NOP
CS_MENU_R2P_NOP
	return

; --------------------------------------------------------------------------
CS_MENU_P2R_PAR8
	movff	CS_MENU_PARAMETER_L, INDF0
	goto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_PAR8
	movf	INDF0, W
	goto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_PAR16
	movff	CS_MENU_PARAMETER_L, POSTINC0
	movff	CS_MENU_PARAMETER_H, POSTDEC0
	goto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_PAR16
	movff	POSTINC0, CS_MENU_PARAMETER_L
	movff	POSTDEC0, CS_MENU_PARAMETER_H
	return

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR
	goto	CS_MENU_P2RR2P_SetVAR

CS_MENU_R2P_VAR
	call	CS_MENU_P2RR2P_GetVAR
	goto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_SID
	;; set new SID number
	;; check if new SID available - if not, increment/decrement until an available one has been found

	;; store available flags in PRODL
	movff	MBNET_NODE_AVAIL, PRODL

	;; branch depending on current SID
	movf	CS_MENU_SID, W
	cpfsgt	CS_MENU_PARAMETER_L, ACCESS
	rgoto CS_MENU_P2R_VAR_SID_Dec
CS_MENU_P2R_VAR_SID_Inc
	movf	CS_MENU_PARAMETER_L, W
	call	MIOS_HLP_GetBitORMask
	andwf	PRODL, W
	bnz	CS_MENU_P2R_VAR_SID_Ok
	incf	CS_MENU_PARAMETER_L, W
	andlw	0x03
	movwf	CS_MENU_PARAMETER_L
	bnz	CS_MENU_P2R_VAR_SID

CS_MENU_P2R_VAR_SID_Old
	movf	CS_MENU_SID, W
	movwf	CS_MENU_PARAMETER_L
 	rgoto	CS_MENU_P2R_VAR_SID_Ok

CS_MENU_P2R_VAR_SID_Dec
	movf	CS_MENU_PARAMETER_L, W
	call	MIOS_HLP_GetBitORMask
	andwf	PRODL, W
	bnz	CS_MENU_P2R_VAR_SID_Ok
	decf	CS_MENU_PARAMETER_L, W
	andlw	0x03
	movwf	CS_MENU_PARAMETER_L
	xorlw	0x03
	bnz	CS_MENU_P2R_VAR_SID
	rgoto	CS_MENU_P2R_VAR_SID_Old


CS_MENU_P2R_VAR_SID_Ok	
	movf	CS_MENU_PARAMETER_L, W
	call	MIOS_HLP_GetBitORMask
	iorlw	0xf0		; (for button pressed detection)
	movwf	CS_MENU_SELECTED_SID_FLAGS
	;; request edit buffer update
	bsf	CS_STAT2, CS_STAT2_EDIT_UPDATE_REQ
	return

CS_MENU_R2P_VAR_SID
	movf	CS_MENU_SID, W		; (already derived from CS_MENU_SELECTED_SID_FLAGS)
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_OSC
	movff	CS_MENU_PARAMETER_L, CS_MENU_SELECTED_OSC
	goto	CS_MENU_Hlp_UpdateOSCFlags	; update of "osc_flags" register

CS_MENU_R2P_VAR_OSC
	movf	CS_MENU_SELECTED_OSC, W
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_SIDLR
	;; 0: L, 1: R, 2: LR
	incf	CS_MENU_PARAMETER_L, W
	movwf	CS_MENU_SELECTED_SID_LR
	return

CS_MENU_R2P_VAR_SIDLR
	;; L: 0, R: 1, LR: 2
	decf	CS_MENU_SELECTED_SID_LR, W
	andlw	0x03
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_VAR_CALI
	call	CS_MENU_FSR0_ENS
	RCALL_IFSET CS_MENU_SELECTED_SID_LR, 0, ACCESS, CS_MENU_P2R_VAR_CALI_Sub
	movlw	4
	addwf	CS_MENU_PARAMETER_IX, F
	call	CS_MENU_FSR0_ENS
	RCALL_IFSET CS_MENU_SELECTED_SID_LR, 1, ACCESS, CS_MENU_P2R_VAR_CALI_Sub
	return

CS_MENU_P2R_VAR_CALI_Sub
	swapf	CS_MENU_PARAMETER_L, W	; 12bit -> 16bit
	andlw	0xf0
	movwf	POSTINC0
	swapf	CS_MENU_PARAMETER_L, W
	andlw	0x0f
	movwf	INDF0
	swapf	CS_MENU_PARAMETER_H, W
	andlw	0xf0
	iorwf	POSTDEC0, F

	;; forward 16bit parameter to master/slave
	call	CS_MENU_MS_SendEnsParameter
	incf	FSR0L, F
	goto	CS_MENU_MS_SendEnsParameter


CS_MENU_R2P_VAR_CALI
	call	CS_MENU_FSR0_ENS
	;; add offset 4 if left SID is not selected
	movlw	0x00
	btfss	CS_MENU_SELECTED_SID_LR, 0
	addlw 4
	addwf	FSR0L, F
	swapf	POSTINC0, W	; 16bit -> 12bit
	andlw	0x0f
	movwf	CS_MENU_PARAMETER_L
	swapf	INDF0, W
	andlw	0xf0
	iorwf	CS_MENU_PARAMETER_L, F
	swapf	INDF0, W
	andlw	0x0f
	movwf	CS_MENU_PARAMETER_H
	return

; --------------------------------------------------------------------------
CS_MENU_P2R_SAVE_NAME
	movf	CS_MENU_NAME_POS, W
	movff	CS_MENU_PARAMETER_L, PLUSW0
	return

CS_MENU_R2P_SAVE_NAME
	movf	CS_MENU_NAME_POS, W
	movf	PLUSW0, W
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_EDIT_SEQ
	;; if rotate function active: branch to special routine which is doing the rotation
	BRA_IFSET CS_STAT3, CS_STAT3_MODIFY_SEQ_ROTATE, ACCESS, CS_MENU_P2R_RotateSeqTrack
	rgoto	CS_MENU_P2R_VAR

CS_MENU_R2P_EDIT_SEQ
	;; nothing to do here if rotate function active
	btfsc	CS_STAT3, CS_STAT3_MODIFY_SEQ_ROTATE
	return
	rgoto	CS_MENU_R2P_VAR

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx
	CS_MENU_P2R_VX_SEND_MACRO
	movff	CS_MENU_PARAMETER_L, INDF0
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Vx
	rcall	CS_MENU_FSR0_Voice
	rgoto	CS_MENU_R2P_PAR8

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_4U
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	0x0f
	andwf	INDF0, F
	swapf	CS_MENU_PARAMETER_L, W
	andlw	0xf0
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Vx_4U
	rcall	CS_MENU_FSR0_Voice
	swapf	INDF0, W
	andlw	0x0f
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_4L
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	0xf0
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x0f
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Vx_4L
	rcall	CS_MENU_FSR0_Voice
	movf	INDF0, W
	andlw	0x0f
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_PMD
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	0xfc
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x03
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Vx_PMD
	rcall	CS_MENU_FSR0_Voice
	movf	INDF0, W
	andlw	0x03
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_PM3
	;; TODO: proper +/- handling
	rgoto	CS_MENU_P2R_Vx

CS_MENU_R2P_Vx_PM3
	;; TODO: proper +/- handling
	rgoto	CS_MENU_R2P_Vx

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_GSA
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	SID_I_V_FLAGS1_GSA
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_Vx_GSA
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_V_FLAGS1_GSA
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_12
	CS_MENU_P2R_VX_SEND_MACRO
	movff	CS_MENU_PARAMETER_L, INDF0	; send low byte
	rcall	CS_MENU_P2R_Hlp_ParSend
	movf	POSTINC0, W
	movff	CS_MENU_PARAMETER_H, INDF0	; send high byte
	rcall	CS_MENU_P2R_Hlp_ParSend
	movf	POSTDEC0, W
	return	

CS_MENU_R2P_Vx_12
	rcall	CS_MENU_FSR0_Voice
	movff	POSTINC0, CS_MENU_PARAMETER_L
	movff	POSTDEC0, CS_MENU_PARAMETER_H
	return

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_WAV
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	0xe0
	andwf	INDF0, F

	movf	CS_MENU_PARAMETER_L, W
	iorwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	skpnz
	bsf	INDF0, 4
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Vx_WAV
	rcall	CS_MENU_FSR0_Voice
	movf	INDF0, W
	andlw	0x07
	btfsc	INDF0, 3
	movlw 0x08
	btfsc	INDF0, 4
	movlw 0x00
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_S_R
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	0x1f
	andwf	INDF0, F
	swapf	CS_MENU_PARAMETER_L, W
	rlf	WREG, W
	andlw	0x60
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Vx_S_R
	rcall	CS_MENU_FSR0_Voice
	swapf	INDF0, W
	rrf	WREG, W
	andlw	0x03
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_ARP_ON
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	SID_I_V_ARP_MODE_ENABLE
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_Vx_ARP_ON
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_V_ARP_MODE_ENABLE
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_ARP_DIR
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	0xf1
	andwf	INDF0, F
	rlf	CS_MENU_PARAMETER_L, W
	andlw	0x0e
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Vx_ARP_DIR
	rcall	CS_MENU_FSR0_Voice
	rrf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_ARP_SOR
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	SID_I_V_ARP_MODE_SORTED
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_Vx_ARP_SOR
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_V_ARP_MODE_SORTED
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_ARP_HLD
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	SID_I_V_ARP_MODE_HOLD
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_Vx_ARP_HLD
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_V_ARP_MODE_HOLD
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_ARP_SNC
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	SID_I_V_ARP_MODE_DIV_SYNC
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_Vx_ARP_SNC
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_V_ARP_MODE_DIV_SYNC
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_ARP_CAC
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	SID_I_V_ARP_MODE_CAC
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_Vx_ARP_CAC
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_V_ARP_MODE_CAC
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_ARP_GLN
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	0xe0
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Vx_ARP_GLN
	rcall	CS_MENU_FSR0_Voice
	movf	INDF0, W
	andlw	0x1f
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_ARP_RNG
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	0x1f
	andwf	INDF0, F
	swapf	CS_MENU_PARAMETER_L, W
	rlf	WREG, W
	andlw	0xe0
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Vx_ARP_RNG
	rcall	CS_MENU_FSR0_Voice
	swapf	INDF0, W
	rrf	WREG, W
	andlw	0x07
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_ARP_SPD
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	0xc0
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x3f
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Vx_ARP_SPD
	rcall	CS_MENU_FSR0_Voice
	movf	INDF0, W
	andlw	0x3f
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Vx_ARP_ONE
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	SID_I_V_ARP_SPEED_DIV_ONESHOT
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_Vx_ARP_ONE
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_V_ARP_SPEED_DIV_ONESHOT
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_M_Vx_PHASE
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	SID_I_M_V_FLAGS2_PHASE
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_M_Vx_PHASE
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_M_V_FLAGS2_PHASE
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_M_Vx_POL
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	SID_I_M_V_FLAGS2_POLY
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_M_Vx_POL
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_M_V_FLAGS2_POLY
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_BM_Vx_LEG
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	SID_I_M_V_FLAGS2_LEGATO
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_BM_Vx_LEG
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_M_V_FLAGS2_LEGATO
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_M_Vx_WTO
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	SID_I_M_V_FLAGS2_WT_ONLY
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_M_Vx_WTO
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_M_V_FLAGS2_WT_ONLY
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_BM_Vx_SUK
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	SID_I_M_V_FLAGS2_SUS_KEY
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_BM_Vx_SUK
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_M_V_FLAGS2_SUS_KEY
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_M_Vx_VAS
	rcall	CS_MENU_P2R_Vx
	;; re-init exclusive voice assignments
	goto	CS_MENU_MS_VoiceReInit

CS_MENU_R2P_M_Vx_VAS
	rgoto	CS_MENU_R2P_Vx

; --------------------------------------------------------------------------
CS_MENU_P2R_M_D_Vx_ASG
	rcall	CS_MENU_P2R_Vx
	;; update shadow buffer (so that the old assignment has no effect anymore)
	goto	CS_MENU_MS_UpdateShadow

CS_MENU_R2P_M_D_Vx_ASG
	rgoto	CS_MENU_R2P_Vx

; --------------------------------------------------------------------------
CS_MENU_P2R_B_Vx_MODE
	CS_MENU_P2R_VX_SEND_MACRO
	;; ensure that Legato and SusKey always activated
	movlw	(1 << SID_I_B_V_FLAGS2_LEGATO) | (1 << SID_I_B_V_FLAGS2_SUS_KEY)
	iorwf	INDF0, F
	;; switch WTO flag
	movlw	SID_I_B_V_FLAGS2_WT_ONLY
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_B_Vx_MODE
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_B_V_FLAGS2_WT_ONLY
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_B_Vx_SEQ_SPD
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	0xc0
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x3f
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_B_Vx_SEQ_SPD
	rcall	CS_MENU_FSR0_Voice
	movf	INDF0, W
	andlw	0x3f
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_B_Vx_SEQ_S16
	CS_MENU_P2R_VX_SEND_MACRO
	movlw	SID_I_V_SEQ_SYNC16
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_B_Vx_SEQ_S16
	rcall	CS_MENU_FSR0_Voice
	movlw	SID_I_V_SEQ_SYNC16
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_D_Vx_VAS
	rcall	CS_MENU_P2R_Vx_4U
	;; re-init voice assignments
	goto	CS_MENU_MS_VoiceReInit

CS_MENU_R2P_D_Vx_VAS
	rgoto	CS_MENU_R2P_Vx_4U

; --------------------------------------------------------------------------
CS_MENU_P2R_Fx_CHN
	CS_MENU_P2R_FX_SEND_MACRO
	movlw	0xf8
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x07
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Fx_CHN
	rcall	CS_MENU_FSR0_Filter
	movf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Fx_MOD
	CS_MENU_P2R_FX_SEND_MACRO
	movlw	0x8f
	andwf	INDF0, F
	swapf	CS_MENU_PARAMETER_L, W
	andlw	0x70
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Fx_MOD
	rcall	CS_MENU_FSR0_Filter
	swapf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_Fx_CUTOFF
	CS_MENU_P2R_FX_SEND_MACRO
	movff	CS_MENU_PARAMETER_L, INDF0	; send low byte
	rcall	CS_MENU_P2R_Hlp_ParSend
	movf	POSTINC0, W
	movlw	0xf0				; send high byte
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_H, W
	iorwf	INDF0, F
	rcall	CS_MENU_P2R_Hlp_ParSend
	movf	POSTDEC0, W
	return

CS_MENU_R2P_Fx_CUTOFF
	rcall	CS_MENU_FSR0_Filter
	movff	POSTINC0, CS_MENU_PARAMETER_L
	movf	POSTDEC0, W
	andlw	0x0f
	movwf	CS_MENU_PARAMETER_H
	return

; --------------------------------------------------------------------------
CS_MENU_P2R_Fx_RESON
CS_MENU_P2R_Fx_KTR
	CS_MENU_P2R_FX_SEND_MACRO
	movf	CS_MENU_PARAMETER_L, W
	movwf	INDF0
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_Fx_RESON
CS_MENU_R2P_Fx_KTR
	rcall	CS_MENU_FSR0_Filter
	movff	INDF0, CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H
	return

; --------------------------------------------------------------------------
CS_MENU_P2R_Fx_EXT
	CS_MENU_P2R_FX_SEND_MACRO
	movlw	3
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_Fx_EXT
	rcall	CS_MENU_FSR0_Filter
	movlw	3
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_Fx_3OF
CS_MENU_P2R_Fx_FIP
	CS_MENU_P2R_FX_SEND_MACRO
	movlw	7
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_Fx_3OF
CS_MENU_R2P_Fx_FIP
	rcall	CS_MENU_FSR0_Filter
	movlw	7
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_LFOx
	CS_MENU_P2R_LFOX_SEND_MACRO
	rgoto	CS_MENU_P2R_PAR8

CS_MENU_R2P_LFOx
	rcall	CS_MENU_FSR0_LFO
	rgoto	CS_MENU_R2P_PAR8

; --------------------------------------------------------------------------
CS_MENU_P2R_LFOx_WAV
	CS_MENU_P2R_LFOX_SEND_MACRO
	movlw	0x0e
	andwf	INDF0, F

	decf	CS_MENU_PARAMETER_L, W
	swapf	WREG, W
	andlw	0xf0
	iorwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	skpz
	bsf	INDF0, 0
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_LFOx_WAV
	rcall	CS_MENU_FSR0_LFO
	swapf	INDF0, W
	andlw	0x0f
	addlw	1
	btfss	INDF0, 0
	movlw 0x00
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_LFOx_SNC
	CS_MENU_P2R_LFOX_SEND_MACRO
	;; controlled from trigger matrix (Note On Event)
	;; index already in CS_MENU_PARAMETER_IX, add LFO offset
	movf	CS_MENU_SELECTED_LFO, W
	addwf	CS_MENU_PARAMETER_IX, F
	movff	CS_MENU_SELECTED_TRG, TMP5	; store current trigger number
	clrf	CS_MENU_SELECTED_TRG; (Note On)
	rcall	CS_MENU_P2R_TRGx_Nx
	movff	TMP5, CS_MENU_SELECTED_TRG	; restore old trigger number
	return

CS_MENU_R2P_LFOx_SNC
	;; controlled from trigger matrix (Note On Event)
	;; index already in CS_MENU_PARAMETER_IX, add LFO offset
	movf	CS_MENU_SELECTED_LFO, W
	addwf	CS_MENU_PARAMETER_IX, F
	movff	CS_MENU_SELECTED_TRG, TMP5	; store current trigger number
	clrf	CS_MENU_SELECTED_TRG; (Note On)
	call	CS_MENU_R2P_TRGx_Nx
	movff	TMP5, CS_MENU_SELECTED_TRG	; restore old trigger number
	return

; --------------------------------------------------------------------------
CS_MENU_P2R_LFOx_SNC_M
	CS_MENU_P2R_LFOX_SEND_MACRO
	movlw	SID_I_LFO_MODE_SYNC_M
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_LFOx_SNC_M
	call	CS_MENU_FSR0_LFO
	movlw	SID_I_LFO_MODE_SYNC_M
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_LFOx_ONE
	CS_MENU_P2R_LFOX_SEND_MACRO
	movlw	SID_I_LFO_MODE_ONESHOT
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_LFOx_ONE
	call	CS_MENU_FSR0_LFO
	movlw	SID_I_LFO_MODE_ONESHOT
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_LFOx_CSN
	CS_MENU_P2R_LFOX_SEND_MACRO
	movlw	SID_I_LFO_MODE_CLKSYNC
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_LFOx_CSN
	call	CS_MENU_FSR0_LFO
	movlw	SID_I_LFO_MODE_CLKSYNC
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_ENVx
	CS_MENU_P2R_ENVX_SEND_MACRO
	rgoto	CS_MENU_P2R_PAR8

CS_MENU_R2P_ENVx
	call	CS_MENU_FSR0_ENV
	rgoto	CS_MENU_R2P_PAR8

; --------------------------------------------------------------------------
CS_MENU_P2R_ENVx_LPB
	CS_MENU_P2R_ENVX_SEND_MACRO
	movlw	0xf8
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x07
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_ENVx_LPB
	call	CS_MENU_FSR0_ENV
	movf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_ENVx_LPE
CS_MENU_P2R_ENVx_CAS
	CS_MENU_P2R_ENVX_SEND_MACRO
	movlw	0x8f
	andwf	INDF0, F
	swapf	CS_MENU_PARAMETER_L, W
	andlw	0x70
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_ENVx_LPE
CS_MENU_R2P_ENVx_CAS
	call	CS_MENU_FSR0_ENV
	swapf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_ENVx_CSN
	CS_MENU_P2R_ENVX_SEND_MACRO
	movlw	SID_I_ENV_MODE_CLKSYNC
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_ENVx_CSN
	call	CS_MENU_FSR0_ENV
	movlw	SID_I_ENV_MODE_CLKSYNC
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_MODx
	call	CS_MENU_FSR0_MOD
	rgoto	CS_MENU_P2R_PAR8

CS_MENU_R2P_MODx
	call	CS_MENU_FSR0_MOD
	rgoto	CS_MENU_R2P_PAR8

; --------------------------------------------------------------------------
CS_MENU_P2R_MODx_SRC
	call	CS_MENU_FSR0_MOD
	movf	CS_MENU_PARAMETER_L, W
	addlw	-SID_MOD_SRC_NUM-1
	BRA_IFSET WREG, 7, ACCESS, CS_MENU_P2R_MODx_SRC_Norm
	addlw	0x80
	movwf	INDF0
	rgoto	CS_MENU_P2R_Hlp_ParSend
CS_MENU_P2R_MODx_SRC_Norm
	rgoto	CS_MENU_P2R_PAR8

CS_MENU_R2P_MODx_SRC
	call	CS_MENU_FSR0_MOD
	movf	INDF0, W
	btfsc	INDF0, 7
	addlw -0x80+SID_MOD_SRC_NUM
	
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_MODx_OP
	call	CS_MENU_FSR0_MOD
	movlw	0xf0
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_MODx_OP
	call	CS_MENU_FSR0_MOD
	movf	INDF0, W
	andlw	0x0f
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_MODx_INV1
	call	CS_MENU_FSR0_MOD
	movlw	6
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_MODx_INV1
	call	CS_MENU_FSR0_MOD
	movlw	6
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_MODx_INV2
	call	CS_MENU_FSR0_MOD
	movlw	7
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_MODx_INV2
	call	CS_MENU_FSR0_MOD
	movlw	7
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
_CS_MENU_P2R_MODx_Nx
	rgoto	CS_MENU_P2R_MODx_Nx	; link to final code (ensures that jumptable still works with rgoto branches)
_CS_MENU_R2P_MODx_Nx
	rgoto	CS_MENU_R2P_MODx_Nx	; link to final code (ensures that jumptable still works with rgoto branches)

; --------------------------------------------------------------------------
_CS_MENU_P2R_TRGx_Nx
	rgoto	CS_MENU_P2R_TRGx_Nx	; link to final code (ensures that jumptable still works with rgoto branches)
_CS_MENU_R2P_TRGx_Nx
	rgoto	CS_MENU_R2P_TRGx_Nx	; link to final code (ensures that jumptable still works with rgoto branches)

; --------------------------------------------------------------------------
CS_MENU_P2R_CHN
	;; update channel stored in ENS structure
	rcall	CS_MENU_P2R_ENSxIx_VAL

	;; update channel stored in CS structure (only for first instrument)
	movf	CS_MENU_SELECTED_INS, W
	bnz	CS_MENU_P2R_CHN_End
	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	andlw	0x03
	addwf	FSR1L, F
	movff	CS_MENU_PARAMETER_L, INDF1
CS_MENU_P2R_CHN_End
	return

CS_MENU_R2P_CHN
	rgoto	CS_MENU_R2P_ENSxIx_VAL

; --------------------------------------------------------------------------
CS_MENU_P2R_BNK
	;; update bank stored in ENS structure
	rcall	CS_MENU_P2R_ENSx_VAL

	;; update bank stored in CS structure
	call	CS_MENU_MS_GetSIDBank
	movff	CS_MENU_PARAMETER_L, INDF1
	rcall	CS_MENU_P2R_CFG_Hlp_PatchUpdate

	;; 	rgoto	CS_MENU_R2P_BNK

CS_MENU_R2P_BNK
	rgoto	CS_MENU_R2P_ENSx_VAL

; --------------------------------------------------------------------------
CS_MENU_P2R_PAT
	;; update bank stored in ENS structure
	rcall	CS_MENU_P2R_ENSx_VAL

	;; update bank stored in CS structure
	call	CS_MENU_MS_GetSIDPatch
	movff	CS_MENU_PARAMETER_L, INDF1
	rcall	CS_MENU_P2R_CFG_Hlp_PatchUpdate
	;; 	rgoto	CS_MENU_R2P_PAT

CS_MENU_R2P_PAT
	rgoto	CS_MENU_R2P_ENSx_VAL

; --------------------------------------------------------------------------
CS_MENU_P2R_B0
	movlw	0
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_B0
	movlw	0
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_B1
	movlw	1
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_B1
	movlw	1
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_B2
	movlw	2
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_B2
	movlw	2
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_CLK_BPM
	call	CS_MENU_FSR0_ENS
	movf	CS_MENU_PARAMETER_L, W
	addlw	50
	skpnc
	movlw	255
	movwf	INDF0
	goto	CS_MENU_MS_SendEnsParameter

CS_MENU_R2P_CLK_BPM
	call	CS_MENU_FSR0_ENS
	movf	INDF0, W
	addlw	-50
	skpc
	movlw	0
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_CLK_SLAVE
	call	CS_MENU_FSR0_ENS
	movlw	~((1 << SID_ENS_CTRL1_CLK_SLAVE) | (1 << SID_ENS_CTRL1_CLK_AUTO))
	andwf	INDF0, F
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, SID_ENS_CTRL1_CLK_SLAVE
	btfsc	CS_MENU_PARAMETER_L, 1
	bsf	INDF0, SID_ENS_CTRL1_CLK_AUTO
	;; forward to master/slave
	goto	CS_MENU_MS_SendEnsParameter

CS_MENU_R2P_CLK_SLAVE
	call	CS_MENU_FSR0_ENS
	movlw	0
	btfsc	INDF0, SID_ENS_CTRL1_CLK_SLAVE
	iorlw 0x01
	btfsc	INDF0, SID_ENS_CTRL1_CLK_AUTO
	iorlw 0x02
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_KNB_VALUE
	movf	CS_MENU_PARAMETER_IX, W
	call	SID_KNOB_GetPtr
	movf	FSR1L, W
	addlw	SID_Ix_Px_VALUE
	movwf	FSR0L
	movf	FSR1H, W
	addlw	HIGH(SID_EDIT_BUFFER)-HIGH(SID_PATCH_BUFFER)
	movwf	FSR0H
	movff	CS_MENU_PARAMETER_L, INDF0

	;; special sending routine knob values
	goto	CS_MENU_MS_SendKnob

CS_MENU_R2P_KNB_VALUE
	movf	CS_MENU_PARAMETER_IX, W
	call	SID_KNOB_GetPtr
	movlw	HIGH(SID_EDIT_BUFFER)-HIGH(SID_PATCH_BUFFER)
	addwf	FSR1H, F
	movlw	SID_Ix_Px_VALUE
	movf	PLUSW1, W
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_KNB_P
	call	CS_MENU_FSR0_Knob
	rcall	CS_MENU_P2R_PAR8
	;; update knob depending on new Min/Max boundary
	goto	CS_MENU_MS_UpdateKnob

CS_MENU_R2P_KNB_P
	call	CS_MENU_FSR0_Knob
	rgoto	CS_MENU_R2P_PAR8

; --------------------------------------------------------------------------
CS_MENU_P2R_KNB_ASG
	call	CS_MENU_FSR0_Knob
	rcall	CS_MENU_P2R_PAR8
	;; update shadow buffer (so that the old assignment has no effect anymore)
	goto	CS_MENU_MS_UpdateShadow

CS_MENU_R2P_KNB_ASG
	call	CS_MENU_FSR0_Knob
	rgoto	CS_MENU_R2P_PAR8

; --------------------------------------------------------------------------
CS_MENU_P2R_EXT_VALUE
	;; convert 12bit -> 16bit
	swapf	CS_MENU_PARAMETER_H, W
	andlw	0xf0
	movwf	PRODH
	swapf	CS_MENU_PARAMETER_L, W
	andlw	0x0f
	iorwf	PRODH, F
	swapf	CS_MENU_PARAMETER_L, W
	andlw	0xf0
	movwf	PRODL

	movff	PRODL, INDF0		; send low byte
	rcall	CS_MENU_P2R_Hlp_ParSend
	movf	POSTINC0, W
	movff	PRODH, INDF0		; send high byte
	rcall	CS_MENU_P2R_Hlp_ParSend
	movf	POSTDEC0, W
	return

CS_MENU_R2P_EXT_VALUE
	;; convert 16bit -> 12bit
	movff	POSTINC0, PRODL
	movff	POSTDEC0, PRODH

	swapf	PRODH, W
	andlw	0x0f
	movwf	CS_MENU_PARAMETER_H
	swapf	PRODH, W
	andlw	0xf0
	movwf	CS_MENU_PARAMETER_L
	swapf	PRODL, W
	andlw	0x0f
	iorwf	CS_MENU_PARAMETER_L, F
	return

; --------------------------------------------------------------------------
CS_MENU_P2R_EXT_SWITCH
	;; change address to custom switch register
	movlw	SID_Ix_CUSTOM_SW
	movwf	FSR0L
	;; turn on/off switch flag which is stored in CS_MENU_PARAMETER_IX
	movf	CS_MENU_PARAMETER_IX, W
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_EXT_SWITCH
	;; change address to custom switch register
	movlw	SID_Ix_CUSTOM_SW
	movwf	FSR0L
	;; flag number in CS_MENU_PARAMETER_IX
	movf	CS_MENU_PARAMETER_IX, W
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_WTx_PAR
	call	CS_MENU_FSR0_WT
	rcall	CS_MENU_P2R_PAR8
	;; update shadow buffer (so that the old assignment has no effect anymore)
	goto	CS_MENU_MS_UpdateShadow

CS_MENU_R2P_WTx_PAR
	call	CS_MENU_FSR0_WT
	rgoto	CS_MENU_R2P_PAR8

; --------------------------------------------------------------------------
CS_MENU_P2R_WTx_SID
	call	CS_MENU_FSR0_WT
	bcf	INDF0, 6
	bcf	INDF0, 7
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 6
	btfsc	CS_MENU_PARAMETER_L, 1
	bsf	INDF0, 7
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_WTx_SID
	call	CS_MENU_FSR0_WT
	movlw	0x00
	btfsc	INDF0, 6
	movlw 0x01
	btfsc	INDF0, 7
	iorlw 0x02
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_WTx_SPD
	call	CS_MENU_FSR0_WT
	movlw	0xc0
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x3f
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_WTx_SPD
	call	CS_MENU_FSR0_WT
	movf	INDF0, W
	andlw	0x3f
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_WTx_B6
	call	CS_MENU_FSR0_WT
	movlw	6
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_WTx_B6
	call	CS_MENU_FSR0_WT
	movlw	6
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_WTx_B7
	call	CS_MENU_FSR0_WT
	movlw	7
	rgoto	CS_MENU_P2R_Hlp_ParSendFlag

CS_MENU_R2P_WTx_B7
	call	CS_MENU_FSR0_WT
	movlw	7
	rgoto	CS_MENU_R2P_SetParFlag

; --------------------------------------------------------------------------
CS_MENU_P2R_WTx_POS
	call	CS_MENU_FSR0_WT
	movlw	0x80
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x7f
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_WTx_POS
	call	CS_MENU_FSR0_WT
	movf	INDF0, W
	andlw	0x7f
	rgoto	CS_MENU_R2P_SetPar


; --------------------------------------------------------------------------
CS_MENU_P2R_WTx_LOP
	call	CS_MENU_FSR0_WT
	decf	CS_MENU_PARAMETER_L, W
	andlw	0x7f
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	INDF0
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_WTx_LOP
	call	CS_MENU_FSR0_WT
	incf	INDF0, W
	btfsc	INDF0, 7
	movlw 0x00
	rgoto	CS_MENU_R2P_SetPar


; --------------------------------------------------------------------------
CS_MENU_P2R_WTE_PAR
	rgoto	CS_MENU_P2R_Hlp_WTP
	
CS_MENU_R2P_WTE_PAR
	call	CS_MENU_FSR0_WTPos
	skpz
	goto	CS_MENU_R2P_PAR8
	rgoto	CS_MENU_R2P_SetPar


; --------------------------------------------------------------------------
CS_MENU_P2R_WT_VAL
	rcall	CS_MENU_P2RR2P_GetVAR
	bcf	INDF0, CS_STAT2_WT_DEC_VIEW
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, CS_STAT2_WT_DEC_VIEW
	return

CS_MENU_R2P_WT_VAL
	rcall	CS_MENU_P2RR2P_GetVAR
	movlw	0x00
	btfsc	INDF0, CS_STAT2_WT_DEC_VIEW
	movlw 0x01
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_B_SEQ_VAL
	rgoto	CS_MENU_P2R_B_SEQ_VAL_Sub
	
	
CS_MENU_R2P_B_SEQ_VAL
	rgoto	CS_MENU_R2P_B_SEQ_VAL_Sub

; --------------------------------------------------------------------------
CS_MENU_P2R_ENSx_VAL
	call	CS_MENU_FSR0_ENS
	movf	CS_MENU_PARAMETER_L, W
	movwf	INDF0
	goto	CS_MENU_MS_SendEnsParameter

CS_MENU_R2P_ENSx_VAL
	call	CS_MENU_FSR0_ENS
	movf	INDF0, W
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_ENSxIx_VAL
	call	CS_MENU_FSR0_ENS_Ix
	movf	CS_MENU_PARAMETER_L, W
	movwf	INDF0
	goto	CS_MENU_MS_SendEnsParameter

CS_MENU_R2P_ENSxIx_VAL
	call	CS_MENU_FSR0_ENS_Ix
	movf	INDF0, W
	rgoto	CS_MENU_R2P_SetPar

; --------------------------------------------------------------------------
CS_MENU_P2R_ENSx_X2A
	call	CS_MENU_FSR0_ENS_CTRL2
	movf	CS_MENU_PARAMETER_IX, W		; contains x2A flag
	call	MIOS_HLP_GetBitORMask
	BRA_IFSET CS_MENU_PARAMETER_L, 0, ACCESS, CS_MENU_P2R_ENSx_X2A_1
CS_MENU_P2R_ENSx_X2A_0
	xorlw	0xff
	andwf	INDF0, F
	rgoto	CS_MENU_P2R_ENSx_X2A_Cont
CS_MENU_P2R_ENSx_X2A_1
	iorwf	INDF0, F
CS_MENU_P2R_ENSx_X2A_Cont
	;; forward to master/slave
	goto	CS_MENU_MS_SendEnsParameter

CS_MENU_R2P_ENSx_X2A
	call	CS_MENU_FSR0_ENS_CTRL2
	movf	CS_MENU_PARAMETER_IX, W		; contains x2A flag
	call	MIOS_HLP_GetBitORMask
	andwf	INDF0, W
	skpz
	movlw	0x01
	rgoto	CS_MENU_R2P_SetPar


; --------------------------------------------------------------------------
CS_MENU_P2R_ENSx_RESERVED
	return

CS_MENU_R2P_ENSx_RESERVED
	return

; --------------------------------------------------------------------------
CS_MENU_P2R_ENSx_MONO
	call	CS_MENU_FSR0_ENS
	bcf	INDF0, SID_ENS_CTRL1_MONO
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, SID_ENS_CTRL1_MONO
	;; forward to master/slave
	goto	CS_MENU_MS_SendEnsParameter

CS_MENU_R2P_ENSx_MONO
	call	CS_MENU_FSR0_ENS
	movlw	0
	btfsc	INDF0, SID_ENS_CTRL1_MONO
	movlw 1
	rgoto	CS_MENU_R2P_SetPar


; --------------------------------------------------------------------------
CS_MENU_P2R_ENSx_FILLOG
	call	CS_MENU_FSR0_ENS
	bcf	INDF0, SID_ENS_CTRL1_FIL_LOG
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, SID_ENS_CTRL1_FIL_LOG
	;; forward to master/slave
	goto	CS_MENU_MS_SendEnsParameter

CS_MENU_R2P_ENSx_FILLOG
	call	CS_MENU_FSR0_ENS
	movlw	0
	btfsc	INDF0, SID_ENS_CTRL1_FIL_LOG
	movlw 1
	rgoto	CS_MENU_R2P_SetPar


; --------------------------------------------------------------------------
CS_MENU_P2R_ENSx_DOR
	call	CS_MENU_FSR0_ENS
	bcf	INDF0, SID_ENS_CTRL1_DOR
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, SID_ENS_CTRL1_DOR
	;; forward to master/slave
	goto	CS_MENU_MS_SendEnsParameter

CS_MENU_R2P_ENSx_DOR
	call	CS_MENU_FSR0_ENS
	movlw	0
	btfsc	INDF0, SID_ENS_CTRL1_DOR
	movlw 1
	rgoto	CS_MENU_R2P_SetPar


; --------------------------------------------------------------------------
CS_MENU_P2R_ENSx_SPV
	;; don't touch value if master SID not selected
	movf	CS_MENU_SID, W
	bnz	CS_MENU_P2R_ENSx_SPV_NotM
CS_MENU_P2R_ENSx_SPV_M
	call	CS_MENU_FSR0_ENS
	movlw	0xf8
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	iorwf	INDF0, F
	;; forward to all master and slaves (will only take effect if CS_MENU_SID == 0)
	call	CS_MENU_MS_SendEnsParameterToAll
CS_MENU_P2R_ENSx_SPV_NotM
	;; re-init voice assignments
	goto	CS_MENU_MS_VoiceReInit

CS_MENU_R2P_ENSx_SPV
	call	CS_MENU_FSR0_ENS
	movf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_R2P_SetPar


; --------------------------------------------------------------------------
CS_MENU_P2R_ENSx_EXTINV
	rcall	CS_MENU_R2P_ENSx_EXTINV_GetAddr
	movf	CS_MENU_PARAMETER_IX, W
	call	MIOS_HLP_GetBitANDMask
	andwf	INDF0, F
	xorlw	0xff
	movf	CS_MENU_PARAMETER_L, F
	skpz
	iorwf	INDF0, F
	;; forward to master/slave
	goto	CS_MENU_MS_SendEnsParameter

CS_MENU_R2P_ENSx_EXTINV
	rcall	CS_MENU_R2P_ENSx_EXTINV_GetAddr
	movf	CS_MENU_PARAMETER_IX, W
	call	MIOS_HLP_GetBitORMask
	andwf	INDF0, W
	skpz
	movlw	0x01
	rgoto	CS_MENU_R2P_SetPar

CS_MENU_R2P_ENSx_EXTINV_GetAddr
	call	CS_MENU_FSR0_ENS
	movf	FSR0L, W	; (CS_MENU_PARAMETER_IX and FSR0L contain offset to bit instead of address)
	andlw	0xf0
	addlw	SID_ENSx_AOUT_INVERTED
	movwf	FSR0L
	return

; --------------------------------------------------------------------------
CS_MENU_P2R_303_CUTOFF
	CS_MENU_P2R_FX_SEND_MACRO
	;; scaled by factor 32 (2^5)
	movf	CS_MENU_PARAMETER_L, W
	mullw	32
	movff	PRODL, INDF0
	rcall	CS_MENU_P2R_Hlp_ParSend		; send low byte
	movf	POSTINC0, W

	movf	CS_MENU_PARAMETER_L, W
	mullw	32
	movlw	0xf0
	andwf	INDF0, F
	movf	PRODH, W
	iorwf	INDF0, F
	rcall	CS_MENU_P2R_Hlp_ParSend		; send high byte
	movf	POSTDEC0, W
	return

CS_MENU_R2P_303_CUTOFF
	call	CS_MENU_FSR0_Filter
	swapf	POSTINC0, W
	rrf	WREG, W
	andlw	0x07
	movwf	CS_MENU_PARAMETER_L

	swapf	POSTDEC0, W
	rrf	WREG, W
	andlw	0x78
	iorwf	CS_MENU_PARAMETER_L, F
	clrf	CS_MENU_PARAMETER_H
	return


; --------------------------------------------------------------------------
CS_MENU_P2R_303_DEP
	;; 7bit -> 8bit (+0x80, positive range only)
	call	CS_MENU_FSR0_ENV
	movf	CS_MENU_PARAMETER_L, W
	addlw	0x80
	btfss	WREG, 7
	movlw 0x80
	movwf	INDF0
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_303_DEP
	;; 8bit -> 7bit (-0x80, positive range only)
	call	CS_MENU_FSR0_ENV
	movf	INDF0, W
	addlw	-0x80
	btfsc	WREG, 7
	movlw 0x00
	rgoto	CS_MENU_R2P_SetPar


; --------------------------------------------------------------------------
CS_MENU_P2R_303_DEC
	;; scaled by two
	call	CS_MENU_FSR0_ENV
	clrc
	rlf	CS_MENU_PARAMETER_L, W
	movwf	INDF0
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_R2P_303_DEC
	call	CS_MENU_FSR0_ENV
	clrc
	rrf	INDF0, W
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
	clrf	CS_MENU_PARAMETER_H
	return
	
; --------------------------------------------------------------------------
;; update patch (used on BNK and PAT changes)
CS_MENU_P2R_CFG_Hlp_PatchUpdate
	call	CS_MENU_MS_GetSIDPatch
	bcf	INDF1, 7	; force save of patch/bank
	movff	INDF1, CS_MENU_PARAMETER_L
	goto	CS_MENU_PatchUpdateSingle


; --------------------------------------------------------------------------
;; set flag (address in FSR0)
CS_MENU_R2P_SetParFlag
	call	MIOS_HLP_GetBitORMask
	andwf	INDF0, W
	movlw	0
	skpz
	movlw	1
	rgoto	CS_MENU_R2P_SetPar
	
; --------------------------------------------------------------------------
;; rotate the selected sequencer track
CS_MENU_P2R_RotateSeqTrack
	;; pointer to 16bit entry -> FSR0
	call	CS_MENU_FSR0_WTPos

	;; re-arrange order of 4 bytes for better handling
	movff	POSTINC0, TMP1	; gate shift register (Step 1-8)
	movff	POSTINC0, TMP3	; accent shift register (Step 1-8)
	movff	POSTINC0, TMP2	; gate shift register (Step 9-16)
	movff	INDF0, TMP4	; accent shift register (Step 9-16)
	movlw	-3		; (switch back to initial offset)
	addwf	FSR0L, F

	;; direction depends on incrementer
	BRA_IFCLR CS_MENU_ENC_INCREMENTER, 7, ACCESS, CS_MENU_P2R_RotateSeqTrack_R
CS_MENU_P2R_RotateSeqTrack_L
	;; rotate gates and accent to the left (by using rrf - no programming error, bits are mirrored)
	clrc
	rrf	TMP2, F
	rrf	TMP1, F
	skpnc
	bsf	TMP2, 7

	clrc
	rrf	TMP4, F
	rrf	TMP3, F
	skpnc
	bsf	TMP4, 7
	rgoto	CS_MENU_P2R_RotateSeqTrack_End

CS_MENU_P2R_RotateSeqTrack_R
	;; rotate gates and accent to the right (by using rlf - no programming error, bits are mirrored)
	clrc
	rlf	TMP1, F
	rlf	TMP2, F
	skpnc
	bsf	TMP1, 0

	clrc
	rlf	TMP3, F
	rlf	TMP4, F
	skpnc
	bsf	TMP3, 0
	;; 	rgoto	CS_MENU_P2R_RotateSeqTrack_End
CS_MENU_P2R_RotateSeqTrack_End

	;; copy-back results
	movff	TMP1, INDF0	; gate shift register (Step 1-8)
	rcall	CS_MENU_P2R_Hlp_ParSend
	movff	TMP3, PREINC0	; accent shift register (Step 1-8)
	rcall	CS_MENU_P2R_Hlp_ParSend
	movff	TMP2, PREINC0	; gate shift register (Step 9-16)
	rcall	CS_MENU_P2R_Hlp_ParSend
	movff	TMP4, PREINC0	; accent shift register (Step 9-16)
	rgoto	CS_MENU_P2R_Hlp_ParSend


; ==========================================================================
;  Function to send a parameter to all selected SIDs
;  expecting value in INDF0
; ==========================================================================
CS_MENU_P2R_Hlp_ParSend
	goto	CS_MENU_MS_SendParameter


; ==========================================================================
;  Function to copy CS_MENU_PARAMETER_L[0] to a given flag
;; modified parameter is sent to all selected SIDs
;  expecting address in of register which should be modified in FSR0
; ==========================================================================
CS_MENU_P2R_Hlp_ParSendFlag
	BRA_IFSET CS_MENU_PARAMETER_L, 0, ACCESS, CS_MENU_P2R_Hlp_ParSendFlag_1

CS_MENU_P2R_Hlp_ParSendFlag_0
	call	MIOS_HLP_GetBitANDMask
	andwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_P2R_Hlp_ParSendFlag_1
	call	MIOS_HLP_GetBitORMask
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend


; ==========================================================================
;  Function to execute P2R filter handlers for all selected SIDs
;  expecting pointer to handler function in TOS[LHU] (!)
; ==========================================================================
CS_MENU_P2R_Fx_Send
	;; save TOS[LHU] (the return pointer) in TBLPTR[LHU]
	movff	TOSL, TBLPTRL
	movff	TOSH, TBLPTRH
	movff	TOSU, TBLPTRU

	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_P2R_Fx_Send_DM
CS_MENU_P2R_Fx_Send_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_P2R_Fx_Send_B

CS_MENU_P2R_Fx_Send_L
	movlw	SID_Ix_L_S1F_BASE
	addwf	FSR0L, F
	RCALL_IFSET CS_MENU_SELECTED_SID_LR, 0, ACCESS, CS_MENU_P2R_CallSub
	movlw	SID_Ix_L_S2F_BASE-SID_Ix_L_S1F_BASE
	addwf	FSR0L, F
	RCALL_IFSET CS_MENU_SELECTED_SID_LR, 1, ACCESS, CS_MENU_P2R_CallSub

	;; jump back to the previous caller!
	pop
	return


CS_MENU_P2R_Fx_Send_B		; bassline engine
	rgoto	CS_MENU_P2R_Fx_Send_L	; same as for lead engine


CS_MENU_P2R_Fx_Send_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_P2R_Fx_Send_M

CS_MENU_P2R_Fx_Send_D		; drum engine
	;; (same as for multi engine)
CS_MENU_P2R_Fx_Send_M		; multi engine
	rgoto	CS_MENU_P2R_Fx_Send_L	; same as for lead engine


; ==========================================================================
;  Function to execute P2R voice handlers for all selected voices and SIDs
;  expecting pointer to handler function in TOS[LHU] (!)
; ==========================================================================
CS_MENU_P2R_Vx_Send
	;; save TOS[LHU] (the return pointer) in TBLPTR[LHU]
	movff	TOSL, TBLPTRL
	movff	TOSH, TBLPTRH
	movff	TOSU, TBLPTRU

	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_P2R_Vx_Send_DM
CS_MENU_P2R_Vx_Send_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_P2R_Vx_Send_B

CS_MENU_P2R_Vx_Send_L		; lead engine
	BRA_IFCLR CS_MENU_SELECTED_SID_LR, 0, ACCESS, CS_MENU_P2R_Vx_Send_L_SR
CS_MENU_P2R_Vx_Send_L_SL
	movlw	SID_Ix_L_S1V1_BASE
	addwf	CS_MENU_PARAMETER_IX, W
	movwf	FSR0L
	RCALL_IFSET CS_MENU_SELECTED_OSC_FLAGS, 0, ACCESS, CS_MENU_P2R_CallSub
	movlw	SID_Ix_L_S1V2_BASE-SID_Ix_L_S1V1_BASE
	addwf	FSR0L, F
	RCALL_IFSET CS_MENU_SELECTED_OSC_FLAGS, 1, ACCESS, CS_MENU_P2R_CallSub
	movlw	SID_Ix_L_S1V3_BASE-SID_Ix_L_S1V2_BASE
	addwf	FSR0L, F
	RCALL_IFSET CS_MENU_SELECTED_OSC_FLAGS, 2, ACCESS, CS_MENU_P2R_CallSub
CS_MENU_P2R_Vx_Send_L_SR
	BRA_IFCLR CS_MENU_SELECTED_SID_LR, 1, ACCESS, CS_MENU_P2R_Vx_Send_L_End
	movlw	SID_Ix_L_S2V1_BASE
	addwf	CS_MENU_PARAMETER_IX, W
	movwf	FSR0L
	RCALL_IFSET CS_MENU_SELECTED_OSC_FLAGS, 0, ACCESS, CS_MENU_P2R_CallSub
	movlw	SID_Ix_L_S2V2_BASE-SID_Ix_L_S2V1_BASE
	addwf	FSR0L, F
	RCALL_IFSET CS_MENU_SELECTED_OSC_FLAGS, 1, ACCESS, CS_MENU_P2R_CallSub
	movlw	SID_Ix_L_S2V3_BASE-SID_Ix_L_S2V2_BASE
	addwf	FSR0L, F
	RCALL_IFSET CS_MENU_SELECTED_OSC_FLAGS, 2, ACCESS, CS_MENU_P2R_CallSub
CS_MENU_P2R_Vx_Send_L_End
	;; jump back to the previous caller!
	pop
	return


CS_MENU_P2R_Vx_Send_B		; bassline engine
	BRA_IFCLR CS_MENU_SELECTED_SID_LR, 0, ACCESS, CS_MENU_P2R_Vx_Send_B_SR
CS_MENU_P2R_Vx_Send_B_SL
	movlw	SID_Ix_B_S1V1_BASE
	addwf	CS_MENU_PARAMETER_IX, W
	movwf	FSR0L
	rcall	CS_MENU_P2R_CallSub
CS_MENU_P2R_Vx_Send_B_SR
	BRA_IFCLR CS_MENU_SELECTED_SID_LR, 1, ACCESS, CS_MENU_P2R_Vx_Send_B_End
	movlw	SID_Ix_B_S2V1_BASE
	addwf	CS_MENU_PARAMETER_IX, W
	movwf	FSR0L
	rcall	CS_MENU_P2R_CallSub
CS_MENU_P2R_Vx_Send_B_End
	;; jump back to the previous caller!
	pop
	return


CS_MENU_P2R_Vx_Send_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_P2R_Vx_Send_M

CS_MENU_P2R_Vx_Send_D		; drum engine
	;; (same as for multi engine)
CS_MENU_P2R_Vx_Send_M		; multi engine
	call	CS_MENU_FSR0_Voice
	rcall	CS_MENU_P2R_CallSub

	;; jump back to the previous caller!
	pop
	return



CS_MENU_P2R_CallSub
	;; copy TBLPTR[LHU] to TOS[LHU] and return
	IRQ_DISABLE
	push
	movf	TBLPTRL, W
	movwf	TOSL
	movf	TBLPTRH, W
	movwf	TOSH
	movf	TBLPTRU, W
	movwf	TOSU
	IRQ_ENABLE
	return


; ==========================================================================
;  Function to execute P2R LFO handlers for all selected SIDs
;  expecting pointer to handler function in TOS[LHU] (!)
; ==========================================================================
CS_MENU_P2R_LFOx_Send
	;; save TOS[LHU] (the return pointer) in TBLPTR[LHU]
	movff	TOSL, TBLPTRL
	movff	TOSH, TBLPTRH
	movff	TOSU, TBLPTRU

	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_P2R_LFOx_Send_DM
CS_MENU_P2R_LFOx_Send_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_P2R_LFOx_Send_B

CS_MENU_P2R_LFOx_Send_L
	call	CS_MENU_FSR0_LFO
	rcall	CS_MENU_P2R_CallSub

	;; jump back to the previous caller!
	pop
	return


CS_MENU_P2R_LFOx_Send_B		; bassline engine
	BRA_IFCLR CS_MENU_SELECTED_SID_LR, 0, ACCESS, CS_MENU_P2R_LFOx_Send_B_SR
CS_MENU_P2R_LFOx_Send_B_SL
	movlw	SID_Ix_B_S1V1_BASE
	addwf	CS_MENU_PARAMETER_IX, W
	movwf	FSR0L
	movf	CS_MENU_SELECTED_LFO, W
	andlw	0x01
	mullw	SID_Ix_B_Vx_LFO2_MODE-SID_Ix_B_Vx_LFO1_MODE
	movf	PRODL, W
	addwf	FSR0L, F
	rcall	CS_MENU_P2R_CallSub
CS_MENU_P2R_LFOx_Send_B_SR
	BRA_IFCLR CS_MENU_SELECTED_SID_LR, 1, ACCESS, CS_MENU_P2R_LFOx_Send_B_End
	movlw	SID_Ix_B_S2V1_BASE
	addwf	CS_MENU_PARAMETER_IX, W
	movwf	FSR0L
	movf	CS_MENU_SELECTED_LFO, W
	andlw	0x01
	mullw	SID_Ix_B_Vx_LFO2_MODE-SID_Ix_B_Vx_LFO1_MODE
	movf	PRODL, W
	addwf	FSR0L, F
	rcall	CS_MENU_P2R_CallSub
CS_MENU_P2R_LFOx_Send_B_End
	;; jump back to the previous caller!
	pop
	return


CS_MENU_P2R_LFOx_Send_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_P2R_LFOx_Send_M

CS_MENU_P2R_LFOx_Send_D		; drum engine
	;; (same as for multi engine)
CS_MENU_P2R_LFOx_Send_M		; multi engine
	rgoto	CS_MENU_P2R_LFOx_Send_L	; same as for lead engine


; ==========================================================================
;  Function to execute P2R ENV handlers for all selected SIDs
;  expecting pointer to handler function in TOS[LHU] (!)
; ==========================================================================
CS_MENU_P2R_ENVx_Send
	;; save TOS[LHU] (the return pointer) in TBLPTR[LHU]
	movff	TOSL, TBLPTRL
	movff	TOSH, TBLPTRH
	movff	TOSU, TBLPTRU

	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_P2R_ENVx_Send_DM
CS_MENU_P2R_ENVx_Send_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_P2R_ENVx_Send_B

CS_MENU_P2R_ENVx_Send_L
	call	CS_MENU_FSR0_ENV
	rcall	CS_MENU_P2R_CallSub

	;; jump back to the previous caller!
	pop
	return


CS_MENU_P2R_ENVx_Send_B		; bassline engine
	BRA_IFCLR CS_MENU_SELECTED_SID_LR, 0, ACCESS, CS_MENU_P2R_ENVx_Send_B_SR
CS_MENU_P2R_ENVx_Send_B_SL
	movlw	SID_Ix_B_S1V1_BASE
	addwf	CS_MENU_PARAMETER_IX, W
	movwf	FSR0L
	rcall	CS_MENU_P2R_CallSub
CS_MENU_P2R_ENVx_Send_B_SR
	BRA_IFCLR CS_MENU_SELECTED_SID_LR, 1, ACCESS, CS_MENU_P2R_ENVx_Send_B_End
	movlw	SID_Ix_B_S2V1_BASE
	addwf	CS_MENU_PARAMETER_IX, W
	movwf	FSR0L
	rcall	CS_MENU_P2R_CallSub
CS_MENU_P2R_ENVx_Send_B_End
	;; jump back to the previous caller!
	pop
	return


CS_MENU_P2R_ENVx_Send_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_P2R_ENVx_Send_M

CS_MENU_P2R_ENVx_Send_D		; drum engine
	;; (same as for multi engine)
CS_MENU_P2R_ENVx_Send_M		; multi engine
	rgoto	CS_MENU_P2R_ENVx_Send_L	; same as for lead engine



; ==========================================================================
;  special variant of MIOS_HLP_GetIndex_2bytes which allows to add 16bit values (PROD[LH])
; ==========================================================================
CS_MENU_P2RR2P_GetIndex_2bytes
	movf	PRODL, W
        addwf   TOSL, F
	movf	PRODH, W
	addwfc	TOSH, F
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
	movlw	0x00
	addwfc	TOSU, F
#endif
        return


; ==========================================================================
;  memory intensive P2R/R2P routines for MOD and TRG nodes located here to
;  ensure that the rgoto based jumptable still accesses code within 1024 byte range
; ==========================================================================
CS_MENU_P2R_MODx_Nx
	;; NOTE: also used by CS_MENU_BUTTON_M_* routines to toggle modulation matrix node
	;; bit number stored in CS_MENU_PARAMETER_IX

	;; get pointer to modulation nodes
	call	CS_MENU_FSR0_MOD_TARG_L

	;; determine masks
	movf	CS_MENU_PARAMETER_IX, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1
	movf	CS_MENU_PARAMETER_IX, W
	call	MIOS_HLP_GetBitANDMask
	movwf	TMP2

	;; copy bits to SID_Ix_MODx_TARG_[LR]
	;; 	movf	TMP2, W
	andwf	INDF0, F
	movf	TMP1, W
	btfsc	CS_MENU_PARAMETER_L, 0
	iorwf	INDF0, F
	rcall	CS_MENU_P2R_Hlp_ParSend

	movf	TMP2, W
	andwf	PREINC0, F
	movf	TMP1, W
	btfsc	CS_MENU_PARAMETER_L, 1
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend


CS_MENU_R2P_MODx_Nx
	;; NOTE: also used by CS_MENU_BUTTON_M_* routines to toggle modulation matrix node
	;; bit number stored in CS_MENU_PARAMETER_IX

	;; get pointer to modulation nodes
	call	CS_MENU_FSR0_MOD_TARG_L

	;; determine mask
	movf	CS_MENU_PARAMETER_IX, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1

	;; build up CS_MENU_PARAMETER_[LH]
	clrf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	movf	POSTINC0, W
	andwf	TMP1, W
	skpz
	bsf	CS_MENU_PARAMETER_L, 0

	movf	POSTDEC0, W
	andwf	TMP1, W
	skpz
	bsf	CS_MENU_PARAMETER_L, 1

	return

CS_MENU_P2R_TRGx_Nx
	;; also used by CS_MENU_P2R_LFOx_SNC
	;; don't overwrite TMP5 !!!

	;; get pointer to trigger target
	call	CS_MENU_FSR0_TRG

	;; determine masks
	movf	CS_MENU_PARAMETER_IX, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1
	movf	CS_MENU_PARAMETER_IX, W
	call	MIOS_HLP_GetBitANDMask
	movwf	TMP2

	;; copy bit to SID_Ix_TRG_xxx_BASE
	;; 	movf	TMP2, W
	andwf	INDF0, F
	movf	TMP1, W
	btfsc	CS_MENU_PARAMETER_L, 0
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend


CS_MENU_R2P_TRGx_Nx
	;; also used by CS_MENU_R2P_LFOx_SNC
	;; don't overwrite TMP5 !!!

	;; get pointer to trigger target
	call	CS_MENU_FSR0_TRG

	;; determine mask
	movf	CS_MENU_PARAMETER_IX, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1

	;; build up CS_MENU_PARAMETER_[LH]
	clrf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	movf	INDF0, W
	andwf	TMP1, W
	skpz
	bsf	CS_MENU_PARAMETER_L, 0

	return

; ==========================================================================
;  Function to set a WT parameter
; ==========================================================================
CS_MENU_P2R_Hlp_WTP
	;; store parameter
	call	CS_MENU_FSR0_WTPos		; sets ZERO flag if value outside range
	skpnz
	return

	movf	CS_MENU_WT_POS, W
	xorlw	0x80
	bz	CS_MENU_P2R_Hlp_WTP_All
CS_MENU_P2R_Hlp_WTP_Single
	movff	CS_MENU_PARAMETER_L, INDF0
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_P2R_Hlp_WTP_All
	;; modify all entries from begin to end position
	clrf	CS_MENU_WT_POS
CS_MENU_P2R_Hlp_WTP_AllLoop
	call	CS_MENU_FSR0_WTPos		; sets ZERO flag if value outside range
	bz	CS_MENU_P2R_Hlp_WTP_AllLoopBreak
	movff	CS_MENU_PARAMETER_L, INDF0
	rcall	CS_MENU_P2R_Hlp_ParSend
	incf	CS_MENU_WT_POS, F
	BRA_IFCLR CS_MENU_WT_POS, 7, ACCESS, CS_MENU_P2R_Hlp_WTP_AllLoop
CS_MENU_P2R_Hlp_WTP_AllLoopBreak
	;; set back CS_MENU_WT_POS to original value (0x80)
	movlw	0x80
	movwf	CS_MENU_WT_POS
	return		

; ==========================================================================
;  Function to set a SEQ parameter
; ==========================================================================
CS_MENU_P2R_B_SEQ_VAL_Sub
	;; if WTPOS >= 0x10, modify all entries
	movf	CS_MENU_WT_POS, W
	andlw	0xf0
	bz	CS_MENU_P2R_B_SEQ_VAL_Sub_Single

CS_MENU_P2R_B_SEQ_VAL_Sub_All
	clrf	CS_MENU_WT_POS		; used as loop counter - 16 steps
CS_MENU_P2R_B_SEQ_VAL_Sub_All_Lp
	rcall	CS_MENU_P2R_B_SEQ_VAL_Sub_Single
	incf	CS_MENU_WT_POS, F
	BRA_IFCLR CS_MENU_WT_POS, 4, ACCESS, CS_MENU_P2R_B_SEQ_VAL_Sub_All_Lp

	;; CS_MENU_WT_POS is 0x10 again - thats fine!
	return


CS_MENU_P2R_B_SEQ_VAL_Sub_Single
	;; determine pointer to parameter depending on CS_MENU_WT_SEQ/CS_MENU_WT_POS
	call	CS_MENU_FSR0_WTPos

	;; branch depending on parameter part:
	;; 0 = note, 1 = Octave, 2 = Slide/Gate, 3 = Accent, 4 = Parameter
	movf	CS_MENU_PARAMETER_IX, W
	bz	CS_MENU_P2R_B_SEQ_VAL_Note
	addlw	-1
	bz	CS_MENU_P2R_B_SEQ_VAL_Oct
	addlw	-1
	bz	CS_MENU_P2R_B_SEQ_VAL_S_G
	addlw	-1
	bz	CS_MENU_P2R_B_SEQ_VAL_Acc
	rgoto	CS_MENU_P2R_B_SEQ_VAL_Par
CS_MENU_P2R_B_SEQ_VAL_Note
	movlw	0xf0
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x0f
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_P2R_B_SEQ_VAL_Oct
	movlw	0xcf
	andwf	INDF0, F
	swapf	CS_MENU_PARAMETER_L, W
	andlw	0x30
	btfsc	WREG, 5	; for mapping: 0=0, 1=1, 2=3, 3=2
	btg	WREG, 4
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend

CS_MENU_P2R_B_SEQ_VAL_S_G
	movlw	0x3f
	andwf	INDF0, F
	swapf	CS_MENU_PARAMETER_L, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xc0
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend
	
CS_MENU_P2R_B_SEQ_VAL_Acc
	;; accent is located in upper bank
	movlw	0x80
	addwf	FSR0L, F

	bcf	INDF0, 7
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF0, 7
	rgoto	CS_MENU_P2R_Hlp_ParSend
	
CS_MENU_P2R_B_SEQ_VAL_Par
	;; 7bit parameter is located in upper bank
	movlw	0x80
	addwf	FSR0L, F

	movlw	0x80
	andwf	INDF0, F
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x7f
	iorwf	INDF0, F
	rgoto	CS_MENU_P2R_Hlp_ParSend



CS_MENU_R2P_B_SEQ_VAL_Sub
	;; determine pointer to parameter depending on CS_MENU_WT_SEQ/CS_MENU_WT_POS
	call	CS_MENU_FSR0_WTPos

	;; branch depending on parameter part:
	;; 0 = note, 1 = Octave, 2 = Slide/Gate, 3 = Accent, 4 = Parameter
	movf	CS_MENU_PARAMETER_IX, W
	bz	CS_MENU_R2P_B_SEQ_VAL_Note
	addlw	-1
	bz	CS_MENU_R2P_B_SEQ_VAL_Oct
	addlw	-1
	bz	CS_MENU_R2P_B_SEQ_VAL_S_G
	addlw	-1
	bz	CS_MENU_R2P_B_SEQ_VAL_Acc
	rgoto	CS_MENU_R2P_B_SEQ_VAL_Par
CS_MENU_R2P_B_SEQ_VAL_Note
	movf	INDF0, W
	andlw	0x0f
	rgoto	CS_MENU_R2P_SetPar
	
CS_MENU_R2P_B_SEQ_VAL_Oct
	swapf	INDF0, W
	andlw	0x03
	btfsc	WREG, 1	; for mapping: 0=0, 1=1, 2=3, 3=2
	btg	WREG, 0
	rgoto	CS_MENU_R2P_SetPar
	
CS_MENU_R2P_B_SEQ_VAL_S_G
	swapf	INDF0, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x03
	rgoto	CS_MENU_R2P_SetPar	

CS_MENU_R2P_B_SEQ_VAL_Acc
	;; accent is located in upper bank
	movlw	0x80
	addwf	FSR0L, F
	movlw	0x00
	btfsc	INDF0, 7
	movlw 0x01
	rgoto	CS_MENU_R2P_SetPar	

CS_MENU_R2P_B_SEQ_VAL_Par
	;; 7bit parameter is located in upper bank
	movlw	0x80
	addwf	FSR0L, F
	movf	INDF0, W
	andlw	0x7f
	rgoto	CS_MENU_R2P_SetPar	
