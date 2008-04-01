; $Id$
;
; SID Control Surface Menu Parameter Print Functions
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
;  All supported PRINT_* indices
; ==========================================================================
PRINT_NOP		EQU	0x00

PRINT_DEC8		EQU	0x01
PRINT_DECP1		EQU	0x02
PRINT_PMDEC7		EQU	0x03
PRINT_PMDEC8		EQU	0x04
PRINT_HEX12		EQU	0x05
PRINT_CHN		EQU	0x06
PRINT_BNK		EQU	0x07
PRINT_PAT		EQU	0x08
PRINT_ENS		EQU	0x09
PRINT_B0		EQU	0x0a
PRINT_B1		EQU	0x0b
PRINT_B2		EQU	0x0c

PRINT_VAR_123		EQU	0x0d
PRINT_VAR_SID1234	EQU	0x0e
PRINT_VAR_DEC		EQU	0x0f
PRINT_VAR_DECP1		EQU	0x10
PRINT_VAR_CALI		EQU	0x11
PRINT_VAR_LR		EQU	0x12
	
PRINT_Vx_DEC		EQU	0x13
PRINT_Vx_DEC4U		EQU	0x14
PRINT_Vx_DEC4L		EQU	0x15
PRINT_Vx_DECP1		EQU	0x16
PRINT_Vx_PMD		EQU	0x17
PRINT_Vx_GSA		EQU	0x18
PRINT_Vx_RESERVED	EQU	0x19	; (bit #3 of flags register)
PRINT_Vx_HEX8		EQU	0x1a
PRINT_Vx_HEX12		EQU	0x1b
PRINT_Vx_PMDEC7		EQU	0x1c
PRINT_Vx_PMDEC8		EQU	0x1d
PRINT_Vx_WAV		EQU	0x1e
PRINT_Vx_S_R		EQU	0x1f
PRINT_Vx_ARP_ON		EQU	0x20
PRINT_Vx_ARP_DIR	EQU	0x21
PRINT_Vx_ARP_SOR	EQU	0x22
PRINT_Vx_ARP_HLD	EQU	0x23
PRINT_Vx_ARP_SNC	EQU	0x24
PRINT_Vx_ARP_CAC	EQU	0x25
PRINT_Vx_ARP_GLN	EQU	0x26
PRINT_Vx_ARP_RNG	EQU	0x27
PRINT_Vx_ARP_SPD	EQU	0x28
PRINT_Vx_ARP_ONE	EQU	0x29

PRINT_M_Vx_VAS		EQU	0x2a
PRINT_M_Vx_PHASE	EQU	0x2b
PRINT_M_Vx_POL		EQU	0x2c
PRINT_BM_Vx_LEG		EQU	0x2d
PRINT_M_Vx_WTO		EQU	0x2e
PRINT_BM_Vx_SUK		EQU	0x2f
PRINT_M_D_Vx_ASG	EQU	0x30

PRINT_D_Vx_VAS		EQU	0x31

PRINT_Fx_CHN		EQU	0x32
PRINT_Fx_CUTOFF		EQU	0x33
PRINT_Fx_RESON		EQU	0x34
PRINT_Fx_KTR		EQU	0x35
PRINT_Fx_MOD		EQU	0x36
PRINT_Fx_EXT		EQU	0x37
PRINT_Fx_3OF		EQU	0x38
PRINT_Fx_FIP		EQU	0x39

PRINT_LFOx_WAV		EQU	0x3a
PRINT_LFOx_RATE		EQU	0x3b
PRINT_LFOx_DEC		EQU	0x3c
PRINT_LFOx_PMDEC8	EQU	0x3d
PRINT_LFOx_SNC		EQU	0x3e
PRINT_LFOx_SNC_M	EQU	0x3f
PRINT_LFOx_ONE		EQU	0x40
PRINT_LFOx_CSN		EQU	0x41

PRINT_ENVx_DEC		EQU	0x42
PRINT_ENVx_PMDEC8	EQU	0x43
PRINT_ENVx_LPB		EQU	0x44
PRINT_ENVx_LPE		EQU	0x45
PRINT_ENVx_CSN		EQU	0x46
PRINT_ENVx_CAS		EQU	0x47

PRINT_MODx_SRC		EQU	0x48
PRINT_MODx_OP		EQU	0x49
PRINT_MODx_TAR		EQU	0x4a
PRINT_MODx_PMDEC8	EQU	0x4b
PRINT_MODx_INV1		EQU	0x4c
PRINT_MODx_INV2		EQU	0x4d
PRINT_MODx_Nx		EQU	0x4e

PRINT_TRGx_SRC		EQU	0x4f
PRINT_TRGx_Nx		EQU	0x50

PRINT_CLK_BPM		EQU	0x51
PRINT_CLK_SLAVE		EQU	0x52

PRINT_KNB		EQU	0x53
PRINT_KNB_VALUE		EQU	0x54
PRINT_KNB_PAR		EQU	0x55
PRINT_KNB_MINMAX	EQU	0x56

PRINT_EXT_VALUE		EQU	0x57
PRINT_EXT_SWITCH	EQU	0x58

PRINT_WTx_PAR		EQU	0x59
PRINT_WTx_PAR_P1	EQU	0x5a
PRINT_WTx_SID		EQU	0x5b
PRINT_WTx_SPD		EQU	0x5c
PRINT_WTx_B7		EQU	0x5d
PRINT_WTx_POS		EQU	0x5e
PRINT_WTx_LOP		EQU	0x5f

PRINT_WTE_POS		EQU	0x60
PRINT_WTE_PAR		EQU	0x61
PRINT_WT_VAL		EQU	0x62
PRINT_WT_GO		EQU	0x63

PRINT_SEQ_POS		EQU	0x64
PRINT_SEQ_NUM		EQU	0x65
PRINT_SEQ_S16		EQU	0x66
PRINT_D_SEQ_ON		EQU	0x67
PRINT_B_SEQ_VAL		EQU	0x68

PRINT_D_MODEL		EQU	0x69

PRINT_RND_GEN		EQU	0x6a

PRINT_SAVE_BNK		EQU	0x6b
PRINT_SAVE_PAT		EQU	0x6c
PRINT_SAVE_NAME		EQU	0x6d
PRINT_SAVE_ENS		EQU	0x6e

PRINT_ENSxIx_DECP1	EQU	0x6f
PRINT_ENSxIx_CHN	EQU	0x70
PRINT_ENSxIx_NOTE	EQU	0x71
PRINT_ENSxIx_PMDEC7	EQU	0x72
PRINT_ENSx_X2A		EQU	0x73
PRINT_ENSx_RESERVED	EQU	0x74
PRINT_ENSx_MONO		EQU	0x75
PRINT_ENSx_SPV		EQU	0x76
PRINT_ENSx_EXTINV	EQU	0x77


; ==========================================================================
;  The PRINT handler
;; IN: PRINT_xxx ID in WREG
; ==========================================================================
CS_MENU_PRINT_Handler
	JUMPTABLE_2BYTES (0x77+1) ; entries
	rgoto	CS_MENU_PRINT_NOP

	rgoto	CS_MENU_PRINT_DEC8
	rgoto	CS_MENU_PRINT_DECP1
	rgoto	CS_MENU_PRINT_PMDEC7
	rgoto	CS_MENU_PRINT_PMDEC8
	rgoto	CS_MENU_PRINT_HEX12
	rgoto	CS_MENU_PRINT_CHN
	rgoto	CS_MENU_PRINT_BNK
	rgoto	CS_MENU_PRINT_PAT
	rgoto	CS_MENU_PRINT_ENS
	rgoto	CS_MENU_PRINT_B0
	rgoto	CS_MENU_PRINT_B1
	rgoto	CS_MENU_PRINT_B2

	rgoto	CS_MENU_PRINT_VAR_123
	rgoto	CS_MENU_PRINT_VAR_SID1234
	rgoto	CS_MENU_PRINT_VAR_DEC
	rgoto	CS_MENU_PRINT_VAR_DECP1
	rgoto	CS_MENU_PRINT_VAR_CALI
	rgoto	CS_MENU_PRINT_VAR_LR
	
	rgoto	CS_MENU_PRINT_Vx_DEC
	rgoto	CS_MENU_PRINT_Vx_DEC4U
	rgoto	CS_MENU_PRINT_Vx_DEC4L
	rgoto	CS_MENU_PRINT_Vx_DECP1
	rgoto	CS_MENU_PRINT_Vx_PMD
	rgoto	CS_MENU_PRINT_Vx_GSA
	rgoto	CS_MENU_PRINT_Vx_RESERVED
	rgoto	CS_MENU_PRINT_Vx_HEX8
	rgoto	CS_MENU_PRINT_Vx_HEX12
	rgoto	CS_MENU_PRINT_Vx_PMDEC7
	rgoto	CS_MENU_PRINT_Vx_PMDEC8
	rgoto	CS_MENU_PRINT_Vx_WAV
	rgoto	CS_MENU_PRINT_Vx_S_R
	rgoto	CS_MENU_PRINT_Vx_ARP_ON
	rgoto	CS_MENU_PRINT_Vx_ARP_DIR
	rgoto	CS_MENU_PRINT_Vx_ARP_SOR
	rgoto	CS_MENU_PRINT_Vx_ARP_HLD
	rgoto	CS_MENU_PRINT_Vx_ARP_SNC
	rgoto	CS_MENU_PRINT_Vx_ARP_CAC
	rgoto	CS_MENU_PRINT_Vx_ARP_GLN
	rgoto	CS_MENU_PRINT_Vx_ARP_RNG
	rgoto	CS_MENU_PRINT_Vx_ARP_SPD
	rgoto	CS_MENU_PRINT_Vx_ARP_ONE

	rgoto	CS_MENU_PRINT_M_Vx_VAS
	rgoto	CS_MENU_PRINT_M_Vx_PHASE
	rgoto	CS_MENU_PRINT_M_Vx_POL
	rgoto	CS_MENU_PRINT_BM_Vx_LEG
	rgoto	CS_MENU_PRINT_M_Vx_WTO
	rgoto	CS_MENU_PRINT_BM_Vx_SUK
	rgoto	CS_MENU_PRINT_M_D_Vx_ASG

	rgoto	CS_MENU_PRINT_D_Vx_VAS

	rgoto	CS_MENU_PRINT_Fx_CHN
	rgoto	CS_MENU_PRINT_Fx_CUTOFF
	rgoto	CS_MENU_PRINT_Fx_RESON
	rgoto	CS_MENU_PRINT_Fx_KTR
	rgoto	CS_MENU_PRINT_Fx_MOD
	rgoto	CS_MENU_PRINT_Fx_EXT
	rgoto	CS_MENU_PRINT_Fx_3OF
	rgoto	CS_MENU_PRINT_Fx_FIP

	rgoto	CS_MENU_PRINT_LFOx_WAV
	rgoto	CS_MENU_PRINT_LFOx_RATE
	rgoto	CS_MENU_PRINT_LFOx_DEC
	rgoto	CS_MENU_PRINT_LFOx_PMDEC8
	rgoto	CS_MENU_PRINT_LFOx_SNC
	rgoto	CS_MENU_PRINT_LFOx_SNC_M
	rgoto	CS_MENU_PRINT_LFOx_ONE
	rgoto	CS_MENU_PRINT_LFOx_CSN

	rgoto	CS_MENU_PRINT_ENVx_DEC
	rgoto	CS_MENU_PRINT_ENVx_PMDEC8
	rgoto	CS_MENU_PRINT_ENVx_LPB
	rgoto	CS_MENU_PRINT_ENVx_LPE
	rgoto	CS_MENU_PRINT_ENVx_CSN
	rgoto	CS_MENU_PRINT_ENVx_CAS

	rgoto	CS_MENU_PRINT_MODx_SRC
	rgoto	CS_MENU_PRINT_MODx_OP
	rgoto	CS_MENU_PRINT_MODx_TAR
	rgoto	CS_MENU_PRINT_MODx_PMDEC8
	rgoto	CS_MENU_PRINT_MODx_INV1
	rgoto	CS_MENU_PRINT_MODx_INV2
	rgoto	CS_MENU_PRINT_MODx_Nx

	rgoto	CS_MENU_PRINT_TRGx_SRC
	rgoto	CS_MENU_PRINT_TRGx_Nx

	rgoto	CS_MENU_PRINT_CLK_BPM
	rgoto	CS_MENU_PRINT_CLK_SLAVE

	rgoto	CS_MENU_PRINT_KNB
	rgoto	CS_MENU_PRINT_KNB_VALUE
	rgoto	CS_MENU_PRINT_KNB_PAR
	rgoto	CS_MENU_PRINT_KNB_MINMAX

	rgoto	CS_MENU_PRINT_EXT_VALUE
	rgoto	CS_MENU_PRINT_EXT_SWITCH

	rgoto	CS_MENU_PRINT_WTx_PAR
	rgoto	CS_MENU_PRINT_WTx_PAR_P1
	rgoto	CS_MENU_PRINT_WTx_SID
	rgoto	CS_MENU_PRINT_WTx_SPD
	rgoto	CS_MENU_PRINT_WTx_B7
	rgoto	CS_MENU_PRINT_WTx_POS
	rgoto	CS_MENU_PRINT_WTx_LOP

	rgoto	CS_MENU_PRINT_WTE_POS
	rgoto	CS_MENU_PRINT_WTE_PAR
	rgoto	CS_MENU_PRINT_WT_VAL
	rgoto	CS_MENU_PRINT_WT_GO

	rgoto	CS_MENU_PRINT_SEQ_POS
	rgoto	CS_MENU_PRINT_SEQ_NUM
	rgoto	CS_MENU_PRINT_SEQ_S16
	rgoto	CS_MENU_PRINT_D_SEQ_ON
	rgoto	CS_MENU_PRINT_B_SEQ_VAL

	rgoto	CS_MENU_PRINT_D_MODEL

	rgoto	CS_MENU_PRINT_RND_GEN

	rgoto	CS_MENU_PRINT_SAVE_BNK
	rgoto	CS_MENU_PRINT_SAVE_PAT
	rgoto	CS_MENU_PRINT_SAVE_NAME
	rgoto	CS_MENU_PRINT_SAVE_ENS

	rgoto	CS_MENU_PRINT_ENSxIx_DECP1
	rgoto	CS_MENU_PRINT_ENSxIx_CHN
	rgoto	CS_MENU_PRINT_ENSxIx_NOTE
	rgoto	CS_MENU_PRINT_ENSxIx_PMDEC7
	rgoto	CS_MENU_PRINT_ENSx_X2A
	rgoto	CS_MENU_PRINT_ENSx_RESERVED
	rgoto	CS_MENU_PRINT_ENSx_MONO
	rgoto	CS_MENU_PRINT_ENSx_SPV
	rgoto	CS_MENU_PRINT_ENSx_EXTINV


; ==========================================================================
; ==========================================================================
; ==========================================================================

CS_MENU_PRINT_NOP
	movlw	3
	goto	SID_LCD_PrintSpaces

; --------------------------------------------------------------------------
CS_MENU_PRINT_DEC8
	movf	INDF0, W
	goto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_DECP1
	incf	INDF0, W
	goto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_PMDEC7
	movf	INDF0, W
	goto	CS_MENU_PRINT_PMDec7

; --------------------------------------------------------------------------
CS_MENU_PRINT_PMDEC8
	movf	INDF0, W
	goto	CS_MENU_PRINT_PMDec8

; --------------------------------------------------------------------------
CS_MENU_PRINT_HEX12
	movf	PREINC0, W
	call	MIOS_LCD_PrintHex1
	movf	POSTDEC0, W
	movf	INDF0, W
	goto	MIOS_LCD_PrintHex2

; --------------------------------------------------------------------------
CS_MENU_PRINT_CHN
	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	andlw	0x03
	addwf	FSR1L, F
	incf	INDF1, W
	andlw	0x7f		; mask out "sent" flag
	goto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_BNK
	movlw	' '
	call	CS_MENU_PRINT_Char

	call	CS_MENU_MS_GetSIDPatch
	movwf	SID_PATCH
	call	CS_MENU_MS_GetSIDBank
	movwf	SID_BANK
	addlw	'A'
	rcall	CS_MENU_PRINT_Char

	;; add '*' if BankStick or Patch not available
	call	SID_PBANK_CheckPatchValid
	movlw	' '
	skpz
	movlw	'*'
	rgoto	CS_MENU_PRINT_Char
	
; --------------------------------------------------------------------------
CS_MENU_PRINT_PAT
	call	CS_MENU_MS_GetSIDPatch
	addlw	1
	goto	CS_MENU_PRINT_Dec000

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENS
	call	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	addlw	1
	goto	CS_MENU_PRINT_Dec000

; --------------------------------------------------------------------------
CS_MENU_PRINT_B0
	movlw	0
	goto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_B1
	movlw	1
	goto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_B2
	movlw	2
	goto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_SID1234
	call	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	movwf	TMP1
	movff	MBNET_NODE_AVAIL, TMP2

	movlw	'-'
	btfsc	TMP1, 0
	movlw '1'
	btfss	TMP2, 0
	movlw '*'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	TMP1, 1
	movlw '2'
	btfss	TMP2, 1
	movlw '*'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	TMP1, 2
	movlw '3'
	btfss	TMP2, 2
	movlw '*'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	TMP1, 3
	movlw '4'
	btfss	TMP2, 3
	movlw '*'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_123
	call	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	goto	CS_MENU_PRINT_123

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_DEC
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	goto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_DECP1
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_LR
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	INDF0, 0
	movlw 'L'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	INDF0, 1
	movlw 'R'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_CALI
	call	CS_MENU_FSR0_ENS

	;; add offset 4 if left SID is not selected
	movlw	0x00
	btfss	CS_MENU_SELECTED_SID_LR, 0
	addlw 4
	addwf	FSR0L, F
	movf	INDF0, W
	swapf	PREINC0, W
	call	MIOS_LCD_PrintHex1
	swapf	POSTDEC0, W
	andlw	0xf0
	movwf	PRODL
	swapf	INDF0, W
	andlw	0x0f
	iorwf	PRODL, W
	goto	MIOS_LCD_PrintHex2

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_DEC
	rcall	CS_MENU_PRINT_GetVx
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_DEC4U
	rcall	CS_MENU_PRINT_GetVx
	swapf	WREG, W
	andlw	0x0f
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_DEC4L
	rcall	CS_MENU_PRINT_GetVx
	andlw	0x0f
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_DECP1
	rcall	CS_MENU_PRINT_GetVx
	andlw	0x7f
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_PMD
	rcall	CS_MENU_PRINT_GetVx
	TABLE_ADDR CS_MENU_PRINT_PMD_Tab
	movlw	0
	btfsc	INDF0, SID_I_V_FLAGS1_PORTA_CTG
	movlw 1
	btfsc	INDF0, SID_I_V_FLAGS1_PORTA_GLISSANDO
	movlw 2
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_PMD_Tab
	dw	"Nor "
	dw	"CTG "
	dw	"Gls "
	dw	"??? "

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_GSA
	rcall	CS_MENU_PRINT_GetVx
 	movlw	SID_I_V_FLAGS1_GSA
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_RESERVED
	return

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_HEX8
	rcall	CS_MENU_PRINT_GetVx
	rgoto	CS_MENU_PRINT_Hex

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_HEX12
	rcall	CS_MENU_PRINT_GetVx
	movf	PREINC0, W
	call	MIOS_LCD_PrintHex1
	movf	POSTDEC0, W
	movf	INDF0, W
	goto	MIOS_LCD_PrintHex2

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_PMDEC7
	rcall	CS_MENU_PRINT_GetVx
	rgoto	CS_MENU_PRINT_PMDec7

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_PMDEC8
	rcall	CS_MENU_PRINT_GetVx
	rgoto	CS_MENU_PRINT_PMDec8

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_WAV
	rcall	CS_MENU_PRINT_GetVx
	TABLE_ADDR CS_MENU_PRINT_Vx_WAV_Tab
	;; if INDF0[4], then oscillator has been switched off
	;; -> index 0
	;; else index = INDF0[2..0] (if INDF0[3] then noise, index=8)
	movf	INDF0, W
	andlw	0x07
	btfsc	INDF0, 3
	movlw 0x08
	btfsc	INDF0, 4
	movlw 0x00
	rgoto	CS_MENU_PRINT_StringFromTable

CS_MENU_PRINT_Vx_WAV_Tab
	dw	"off "
	dw	"Tri "
	dw	"Saw "
	dw	"T+S "
	dw	"Pul "
	dw	"P+T "
	dw	"P+S "
	dw	"PST "
	dw	"Noi "

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_S_R
	rcall	CS_MENU_PRINT_GetVx
	TABLE_ADDR CS_MENU_PRINT_Vx_S_R_Tab
	;; status in INDF0[6..5]
	;; table allocates 4 bytes per entry
	;; therefore: offset = INDF0[6..5] >> 3
	rrf	INDF0, W
	swapf	WREG, W
	andlw	0x03
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_Vx_S_R_Tab
	dw	"--- "
	dw	"Syn "
	dw	"Rin "
	dw	"R&S "

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_ARP_ON
	rcall	CS_MENU_PRINT_GetVx
	movlw	SID_I_V_ARP_MODE_ENABLE
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_ARP_DIR
	rcall	CS_MENU_PRINT_GetVx
	TABLE_ADDR CS_MENU_PRINT_Vx_ARP_DIR_Tab
	rrf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_PRINT_StringFromTable

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_ARP_SOR
	rcall	CS_MENU_PRINT_GetVx
	movlw	SID_I_V_ARP_MODE_SORTED
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_ARP_HLD
	rcall	CS_MENU_PRINT_GetVx
	movlw	SID_I_V_ARP_MODE_HOLD
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_ARP_SNC
	rcall	CS_MENU_PRINT_GetVx
	movlw	SID_I_V_ARP_MODE_DIV_SYNC
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_ARP_CAC
	rcall	CS_MENU_PRINT_GetVx
	movlw	SID_I_V_ARP_MODE_CAC
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_ARP_GLN
	rcall	CS_MENU_PRINT_GetVx
	movf	INDF0, W
	andlw	0x1f
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_ARP_RNG
	rcall	CS_MENU_PRINT_GetVx
	swapf	INDF0, W
	rrf	WREG, W
	andlw	0x07
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_ARP_SPD
	rcall	CS_MENU_PRINT_GetVx
	movf	INDF0, W
	andlw	0x3f
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_ARP_ONE
	rcall	CS_MENU_PRINT_GetVx
	movlw	SID_I_V_ARP_SPEED_DIV_ONESHOT
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_M_Vx_VAS
	rcall	CS_MENU_PRINT_GetVx
	TABLE_ADDR CS_MENU_PRINT_Vx_VAS_Tab
	movf	INDF0, W
	andlw	0x0f
	rgoto	CS_MENU_PRINT_StringFromTable

; --------------------------------------------------------------------------
CS_MENU_PRINT_M_Vx_PHASE
	rcall	CS_MENU_PRINT_GetVx
	movlw	SID_I_M_V_FLAGS2_PHASE
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_M_Vx_POL
	rcall	CS_MENU_PRINT_GetVx
	movlw	SID_I_M_V_FLAGS2_POLY
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_BM_Vx_LEG
	rcall	CS_MENU_PRINT_GetVx
	movlw	SID_I_M_V_FLAGS2_LEGATO
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_M_Vx_WTO
	rcall	CS_MENU_PRINT_GetVx
	movlw	SID_I_M_V_FLAGS2_WT_ONLY
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_BM_Vx_SUK
	rcall	CS_MENU_PRINT_GetVx
	movlw	SID_I_M_V_FLAGS2_SUS_KEY
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_M_D_Vx_ASG
	;; special code only as notifier for assignment message - checked in cs_menu.inc
	rgoto	CS_MENU_PRINT_Vx_DEC

; --------------------------------------------------------------------------
CS_MENU_PRINT_D_Vx_VAS
	rcall	CS_MENU_PRINT_GetVx
	TABLE_ADDR CS_MENU_PRINT_Vx_VAS_Tab
	swapf	INDF0, W
	andlw	0x0f
	rgoto	CS_MENU_PRINT_StringFromTable

; --------------------------------------------------------------------------
CS_MENU_PRINT_Fx_CHN
	;; print selected channels
	TABLE_ADDR CS_MENU_PRINT_Fx_CHN_Tab
	call	CS_MENU_FSR0_Filter
	movf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_Fx_CHN_Tab
	dw	"--- "
	dw	"1-- "
	dw	"-2- "
	dw	"12- "
	dw	"--3 "
	dw	"1-3 "
	dw	"-23 "
	dw	"123 "

; --------------------------------------------------------------------------
CS_MENU_PRINT_Fx_CUTOFF
	call	CS_MENU_FSR0_Filter
	movf	PREINC0, W
	call	MIOS_LCD_PrintHex1
	movf	POSTDEC0, W
	movf	INDF0, W
	goto	MIOS_LCD_PrintHex2

; --------------------------------------------------------------------------
CS_MENU_PRINT_Fx_RESON
	call	CS_MENU_FSR0_Filter
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_Fx_KTR
	call	CS_MENU_FSR0_Filter
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_Fx_MOD
	TABLE_ADDR CS_MENU_PRINT_Fx_MOD_Tab
	call	CS_MENU_FSR0_Filter
	swapf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_Fx_MOD_Tab
	dw	"--- "
	dw	"L-- "
	dw	"-B- "
	dw	"LB- "
	dw	"--H "
	dw	"L-H "
	dw	"-BH "
	dw	"LBH "

; --------------------------------------------------------------------------
CS_MENU_PRINT_Fx_EXT
	call	CS_MENU_FSR0_Filter
	movff	INDF0, MIOS_PARAMETER1
	movlw	3
	rgoto	CS_MENU_PRINT_OnOff

; --------------------------------------------------------------------------
CS_MENU_PRINT_Fx_3OF
CS_MENU_PRINT_Fx_FIP
	call	CS_MENU_FSR0_Filter
	movff	INDF0, MIOS_PARAMETER1
	movlw	7
	rgoto	CS_MENU_PRINT_OnOff

; --------------------------------------------------------------------------
CS_MENU_PRINT_LFOx_WAV
	TABLE_ADDR CS_MENU_PRINT_LFOx_WAV_Tab
	;; waveform stored in INDF0[6..4]
	;; 8 waveforms provided
	;; add 1, index 0 notifies "LFO Off"
	rcall	CS_MENU_PRINT_GetLFOx
	swapf	WREG, W
	andlw	0x0f
	addlw	1
	btfss	INDF0, 0
	movlw 0x00
	rgoto	CS_MENU_PRINT_StringFromTable

; --------------------------------------------------------------------------
CS_MENU_PRINT_LFOx_RATE
	rcall	CS_MENU_PRINT_GetLFOx	; FSR0 will point to SID_Ix_LFOx_RATE

	;; prepare TBLPTR[LH]
	TABLE_ADDR CS_MENU_PRINT_LFOx_RATE_Tab
	;; if clock sync activated, and rate >= 245, print period length
	movlw	(SID_Ix_LFOx_MODE-SID_Ix_LFOx_RATE)
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_CLKSYNC, ACCESS, CS_MENU_PRINT_LFOx_RATE_Dec
	movf	INDF0, W
	addlw	-245
	bnc	CS_MENU_PRINT_LFOx_RATE_Dec
CS_MENU_PRINT_LFOx_RATE_Str
	rgoto	CS_MENU_PRINT_StringFromTable
	
CS_MENU_PRINT_LFOx_RATE_Dec
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_LFOx_DEC
	call	CS_MENU_PRINT_GetLFOx
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_LFOx_PMDEC8
	call	CS_MENU_PRINT_GetLFOx
	rgoto	CS_MENU_PRINT_PMDec8

; --------------------------------------------------------------------------
CS_MENU_PRINT_LFOx_SNC
	;; controlled from trigger matrix (Note On Event)
	;; index already in CS_MENU_PARAMETER_IX, add LFO offset
	movf	CS_MENU_SELECTED_LFO, W
	addwf	CS_MENU_PARAMETER_IX, F
	movff	CS_MENU_SELECTED_TRG, TMP5	; store current trigger number
	clrf	CS_MENU_SELECTED_TRG; (Note On)
	rcall	CS_MENU_PRINT_TRGx_Nx
	movff	TMP5, CS_MENU_SELECTED_TRG	; restore old trigger number
	return

; --------------------------------------------------------------------------
CS_MENU_PRINT_LFOx_SNC_M
	rcall	CS_MENU_PRINT_GetLFOx
	movlw	SID_I_LFO_MODE_SYNC_M
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_LFOx_ONE
	rcall	CS_MENU_PRINT_GetLFOx
	movlw	SID_I_LFO_MODE_ONESHOT
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_LFOx_CSN
	rcall	CS_MENU_PRINT_GetLFOx
	movlw	SID_I_LFO_MODE_CLKSYNC
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENVx_DEC
	rcall	CS_MENU_PRINT_GetENVx
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENVx_PMDEC8
	rcall	CS_MENU_PRINT_GetENVx
	rgoto	CS_MENU_PRINT_PMDec8

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENVx_LPB
	rcall	CS_MENU_PRINT_GetENVx
	andlw	0x07
	;; 	rgoto	CS_MENU_PRINT_ENVx_PrnLoop

CS_MENU_PRINT_ENVx_PrnLoop
	movwf	PRODL
	TABLE_ADDR CS_MENU_PRINT_ENVx_Loop_Tab
	movf	PRODL, W
	rgoto	CS_MENU_PRINT_StringFromTable

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENVx_LPE
	rcall	CS_MENU_PRINT_GetENVx
	swapf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_PRINT_ENVx_PrnLoop

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENVx_CSN
	rcall	CS_MENU_PRINT_GetENVx
	movlw	SID_I_ENV_MODE_CLKSYNC
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENVx_CAS
	TABLE_ADDR CS_MENU_PRINT_ENVx_CAS_Tab
	rcall	CS_MENU_PRINT_GetENVx
	swapf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_PRINT_StringFromTable

; --------------------------------------------------------------------------
CS_MENU_PRINT_MODx_SRC
	TABLE_ADDR CS_MENU_PRINT_MODx_SRC_Tab
	rcall	CS_MENU_PRINT_GetMODx
	BRA_IFCLR WREG, 7, ACCESS, CS_MENU_PRINT_MODx_SRC_Norm
CS_MENU_PRINT_MODx_SRC_Const
	andlw	0x7f
	rgoto	CS_MENU_PRINT_Dec

CS_MENU_PRINT_MODx_SRC_Norm
	rgoto	CS_MENU_PRINT_StringFromTable

; --------------------------------------------------------------------------
CS_MENU_PRINT_MODx_OP
	TABLE_ADDR CS_MENU_PRINT_MODx_OP_Tab
	rcall	CS_MENU_PRINT_GetMODx
	andlw	0x0f
	rgoto	CS_MENU_PRINT_StringFromTable

; --------------------------------------------------------------------------
CS_MENU_PRINT_MODx_TAR
	TABLE_ADDR CS_MENU_PRINT_MODx_TAR_Tab
	rcall	CS_MENU_PRINT_GetMODx
	rgoto	CS_MENU_PRINT_StringFromTable

; --------------------------------------------------------------------------
CS_MENU_PRINT_MODx_INV1
	rcall	CS_MENU_PRINT_GetMODx
	movlw	6
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_MODx_INV2
	rcall	CS_MENU_PRINT_GetMODx
	movlw	7
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_MODx_Nx
	;; bit number stored in CS_MENU_PARAMETER_IX

	;; get pointer to modulation nodes
	call	CS_MENU_FSR0_MOD_TARG_L

	;; determine mask
	movf	CS_MENU_PARAMETER_IX, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1

	;; print L/R depending on activated flags
	movf	POSTINC0, W
	andwf	TMP1, W
	movlw	'L'
	skpnz
	movlw	'-'
	rcall	CS_MENU_PRINT_Char

	movlw	'/'
	rcall	CS_MENU_PRINT_Char

	movf	POSTDEC0, W
	andwf	TMP1, W
	movlw	'R'
	skpnz
	movlw	'-'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_MODx_PMDEC8
	rcall	CS_MENU_PRINT_GetMODx
	rgoto	CS_MENU_PRINT_PMDec8

; --------------------------------------------------------------------------
CS_MENU_PRINT_TRGx_SRC
	TABLE_ADDR CS_MENU_PRINT_TRGx_SRC_Tab
#if 0
	movf	CS_MENU_SELECTED_TRG, W
	rgoto	CS_MENU_PRINT_StringFromTable
#else
	;; special variation required for Page Up/Down print function (only 3 characters print)
	rlf	CS_MENU_SELECTED_TRG, W
	rlf	WREG, W
	andlw	0xfc
	TABLE_ADD_W
	movlw	3
	goto	MIOS_LCD_PrintPreconfString
#endif

; --------------------------------------------------------------------------
CS_MENU_PRINT_TRGx_Nx
	;; also used by CS_MENU_PRINT_LFOx_SNC
	;; don't overwrite TMP5 !!!

	;; bit number stored in CS_MENU_PARAMETER_IX

	;; get pointer to trigger target
	call	CS_MENU_FSR0_TRG

	;; print On/Off
	movf	CS_MENU_PARAMETER_IX, W
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_CLK_BPM
	call	CS_MENU_FSR0_ENS
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_CLK_SLAVE
	TABLE_ADDR CS_MENU_PRINT_CLK_SLAVE_Tab
	call	CS_MENU_FSR0_ENS
	movlw	0
	btfsc	INDF0, SID_ENS_CTRL1_CLK_SLAVE
	movlw 1
	btfsc	INDF0, SID_ENS_CTRL1_CLK_AUTO
	movlw 2
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_CLK_SLAVE_Tab
	dw	"Mst."
	dw	"Slv."
	dw	"Auto"

; --------------------------------------------------------------------------
CS_MENU_PRINT_KNB
	TABLE_ADDR CS_MENU_PRINT_KNB_Tab
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	rgoto	CS_MENU_PRINT_StringFromTable

; --------------------------------------------------------------------------
CS_MENU_PRINT_KNB_VALUE
	movf	CS_MENU_PARAMETER_IX, W
	call	SID_KNOB_GetPtr
	movlw	HIGH(SID_EDIT_BUFFER)-HIGH(SID_PATCH_BUFFER)
	addwf	FSR1H, F
	movlw	SID_Ix_Px_VALUE
	movf	PLUSW1, W
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_KNB_PAR
	call	CS_MENU_FSR0_Knob
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_KNB_MINMAX
	call	CS_MENU_FSR0_Knob
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_EXT_VALUE
	;; convert 16bit -> 12bit
	movff	POSTINC0, PRODL
	movff	POSTDEC0, PRODH

	swapf	PRODH, W
	andlw	0x0f
	call	MIOS_LCD_PrintHex1
	swapf	PRODH, W
	andlw	0xf0
	movwf	PRODH
	swapf	PRODL, W
	andlw	0x0f
	iorwf	PRODH, W
	goto	MIOS_LCD_PrintHex2

; --------------------------------------------------------------------------
CS_MENU_PRINT_EXT_SWITCH
	;; change address to custom switch register
	movlw	SID_Ix_CUSTOM_SW
	movwf	FSR0L
	;; flag number in CS_MENU_PARAMETER_IX
	movf	CS_MENU_PARAMETER_IX, W
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_WTx_PAR
	call	CS_MENU_FSR0_WT
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_WTx_PAR_P1
	call	CS_MENU_FSR0_WT
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_WTx_SID
	call	CS_MENU_FSR0_WT
	;; print L/R depending on activated flags
	movlw	'-'
	btfsc	INDF0, 6
	movlw 'L'
	rcall	CS_MENU_PRINT_Char

	movlw	'/'
	rcall	CS_MENU_PRINT_Char

	movlw	'-'
	btfsc	INDF0, 7
	movlw 'R'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_WTx_SPD
	call	CS_MENU_FSR0_WT
	movf	INDF0, W
	andlw	0x3f
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_WTx_B7
	call	CS_MENU_FSR0_WT
	movlw	7
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_WTx_POS
	call	CS_MENU_FSR0_WT
	movf	INDF0, W
	andlw	0x7f
	rgoto	CS_MENU_PRINT_WTVal

; --------------------------------------------------------------------------
CS_MENU_PRINT_WTx_LOP
	call	CS_MENU_FSR0_WT
	BRA_IFSET INDF0, 7, ACCESS, CS_MENU_PRINT_WTx_LOP_One
	movf	INDF0, W
	andlw	0x7f
	rgoto	CS_MENU_PRINT_WTVal
CS_MENU_PRINT_WTx_LOP_One
	movlw	'-'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_WTE_POS
	;; value 0x00-0x1f: print decimal number
	;; value >= 0x80: print "All"
	rcall	CS_MENU_PRINT_GetVAR
	movlw	0x7f
	cpfsgt	CS_MENU_WT_POS, ACCESS
	rgoto CS_MENU_PRINT_WTE_POS_Val
CS_MENU_PRINT_WTE_POS_All
	movlw	'A'
	rcall	CS_MENU_PRINT_Char
	movlw	'l'
	rcall	CS_MENU_PRINT_Char
	movlw	'l'
	rgoto	CS_MENU_PRINT_Char

CS_MENU_PRINT_WTE_POS_Val
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_WTVal

; --------------------------------------------------------------------------
CS_MENU_PRINT_WTE_PAR
	call	CS_MENU_FSR0_WTPos		; sets ZERO flag if value outside range
	bnz	CS_MENU_PRINT_WTE_PAR_Ok
CS_MENU_PRINT_WTE_PAR_Inv
	movlw	'.'
	rcall	CS_MENU_PRINT_Char
	movlw	'.'
	rcall	CS_MENU_PRINT_Char
	movlw	'.'
	rcall	CS_MENU_PRINT_Char
	rgoto	CS_MENU_PRINT_WTE_PAR_End

CS_MENU_PRINT_WTE_PAR_Ok
	BRA_IFCLR INDF0, 7, ACCESS, CS_MENU_PRINT_WTE_PAR_PM
CS_MENU_PRINT_WTE_PAR_Px
	;; special case: if Assignment in between 0xfc..0xff: print note instead of hex/dec number

	;; get WT assignment which corresponds to displayed entry
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_PRINT_WTE_PAR_Px_DM
CS_MENU_PRINT_WTE_PAR_Px_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_PRINT_WTE_PAR_Px_B
CS_MENU_PRINT_WTE_PAR_Px_L
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_WT1_BASE + SID_Ix_WTx_ASSGN
	movf	CS_MENU_PARAMETER_IX, W
	andlw	0x03
	mullw	SID_Ix_L_WT2_BASE-SID_Ix_L_WT1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	INDF1, W
	andlw	0xfc
	xorlw	0xfc
	rgoto	CS_MENU_PRINT_WTE_PAR_Px_FOk
CS_MENU_PRINT_WTE_PAR_Px_B
	rgoto	CS_MENU_PRINT_WTE_PAR_Px_L	; not implemented yet
CS_MENU_PRINT_WTE_PAR_Px_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_PRINT_WTE_PAR_Px_M
CS_MENU_PRINT_WTE_PAR_Px_D
	rgoto	CS_MENU_PRINT_WTE_PAR_Px_L	; not implemented yet
CS_MENU_PRINT_WTE_PAR_Px_M
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_M_I1_BASE + SID_Ix_M_Vx_WT_ASSGN
	movf	CS_MENU_PARAMETER_IX, W
	andlw	0x07
	mullw	SID_Ix_M_I2_BASE - SID_Ix_M_I1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	movf	INDF1, W
	andlw	0xf8
	xorlw	0xf8
	;; 	rgoto	CS_MENU_PRINT_WTE_PAR_Px_FOk

CS_MENU_PRINT_WTE_PAR_Px_FOk

	bnz	CS_MENU_PRINT_WTE_PAR_Px_Hex
CS_MENU_PRINT_WTE_PAR_Px_Note
	;; if INDF0 >= 0xfc, print Ky1...Ky4 (for arpeggiator)
	movf	INDF0, W
	andlw	0xfc
	xorlw	0xfc
	bnz	CS_MENU_PRINT_WTE_PAR_Px_NoArp
CS_MENU_PRINT_WTE_PAR_Px_Arp
	movlw	'K'
	rcall	CS_MENU_PRINT_Char
	movlw	'y'
	rcall	CS_MENU_PRINT_Char
	movf	INDF0, W
	andlw	0x03
	addlw	1
	call	MIOS_LCD_PrintBCD1
	rgoto	CS_MENU_PRINT_WTE_PAR_End

CS_MENU_PRINT_WTE_PAR_Px_NoArp
	;; if INDF0 == 0, print "---" (for no note)
	movf	INDF0, W
	andlw	0x7f
	bz	CS_MENU_PRINT_WTE_PAR_Px_NoN
	;; if INDF0 == 1, print "+++" (for hold note)
	addlw	-1
	bz	CS_MENU_PRINT_WTE_PAR_Px_Hold
	;; else print note name
CS_MENU_PRINT_WTE_PAR_Px_NoHld
	addlw	+1
	call	SID_NOTE_Print
	rgoto	CS_MENU_PRINT_WTE_PAR_End

CS_MENU_PRINT_WTE_PAR_Px_NoN
	movlw	'-'
	rgoto	CS_MENU_PRINT_WTE_PAR_Px_NoNC
CS_MENU_PRINT_WTE_PAR_Px_Hold
	movlw	'+'
CS_MENU_PRINT_WTE_PAR_Px_NoNC
	movwf	TABLAT
	rcall	CS_MENU_PRINT_Char
	movf	TABLAT, W
	rcall	CS_MENU_PRINT_Char
	movf	TABLAT, W
	rcall	CS_MENU_PRINT_Char
	rgoto	CS_MENU_PRINT_WTE_PAR_End

CS_MENU_PRINT_WTE_PAR_Px_Hex
	movf	INDF0, W
	andlw	0x7f
	rcall	CS_MENU_PRINT_WTVal
	rgoto	CS_MENU_PRINT_WTE_PAR_End

CS_MENU_PRINT_WTE_PAR_PM
	movf	INDF0, W
	rcall	CS_MENU_PRINT_PMWTVal
	;; 	rgoto	CS_MENU_PRINT_WTE_PAR_End

CS_MENU_PRINT_WTE_PAR_End
	;; if position matches with loop point, add '<' marker
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_PRINT_WTE_PAR_LC_DM
CS_MENU_PRINT_WTE_PAR_LC_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_PRINT_WTE_PAR_LC_B
CS_MENU_PRINT_WTE_PAR_LC_L
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_WT1_BASE + SID_Ix_WTx_LOOP
	movf	CS_MENU_PARAMETER_IX, W
	andlw	0x03
	mullw	SID_Ix_L_WT2_BASE-SID_Ix_L_WT1_BASE
	rgoto	CS_MENU_PRINT_WTE_PAR_LC_FOk
CS_MENU_PRINT_WTE_PAR_LC_B
	rgoto	CS_MENU_PRINT_WTE_PAR_LC_L	; not implemented yet
CS_MENU_PRINT_WTE_PAR_LC_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_PRINT_WTE_PAR_LC_M
CS_MENU_PRINT_WTE_PAR_LC_D
	rgoto	CS_MENU_PRINT_WTE_PAR_LC_L	; not implemented yet
CS_MENU_PRINT_WTE_PAR_LC_M
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_M_I1_BASE + SID_Ix_WTx_LOOP
	movf	CS_MENU_PARAMETER_IX, W
	andlw	0x07
	mullw	SID_Ix_M_I2_BASE - SID_Ix_M_I1_BASE
	;; 	rgoto	CS_MENU_PRINT_WTE_PAR_LC_FOk

CS_MENU_PRINT_WTE_PAR_LC_FOk
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F

	;; skip if oneshot flag set
	btfsc	INDF1, 7
	return
	movf	INDF1, W
	andlw	0x7f
	movwf	PRODL

	movf	FSR0L, W
	addlw	0-LOW(SID_Ix_WAVETABLE)
	xorwf	PRODL, W
	movlw	'<'
	skpnz
	rcall	CS_MENU_PRINT_Char
	return


; --------------------------------------------------------------------------
CS_MENU_PRINT_WT_VAL
	movlw	'H'
	btfsc	CS_STAT2, CS_STAT2_WT_DEC_VIEW
	movlw 'D'
	rcall	CS_MENU_PRINT_Char
	movlw	'e'
	rcall	CS_MENU_PRINT_Char
	movlw	'x'
	btfsc	CS_STAT2, CS_STAT2_WT_DEC_VIEW
	movlw 'c'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_WT_GO
	movlw	'G'
	rcall	CS_MENU_PRINT_Char
	movlw	'o'
	rcall	CS_MENU_PRINT_Char
	movlw	'!'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_SEQ_POS
	;; value 0x00-0x0f: print decimal number+1
	;; value >= 0x10: print "All"
	rcall	CS_MENU_PRINT_GetVAR
	movlw	0x0f
	cpfsgt	CS_MENU_WT_POS, ACCESS
	rgoto CS_MENU_PRINT_SEQ_POS_Val
CS_MENU_PRINT_SEQ_POS_All
	rgoto	CS_MENU_PRINT_WTE_POS_All

CS_MENU_PRINT_SEQ_POS_Val
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_SEQ_NUM
	call	CS_MENU_FSR0_WT
	movf	INDF0, W
	andlw	0xf8
	bnz	CS_MENU_PRINT_SEQ_NUM_Dis
CS_MENU_PRINT_SEQ_NUM_Valid
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_DecPlus1
CS_MENU_PRINT_SEQ_NUM_Dis
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_SEQ_S16
	call	CS_MENU_FSR0_WT
	movlw	SID_I_V_SEQ_SYNC16
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_D_SEQ_ON
	call	CS_MENU_FSR0_WT
	movlw	SID_I_V_SEQ_ON
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_B_SEQ_VAL
	;; determine pointer to parameter depending on CS_MENU_WT_SEQ/CS_MENU_WT_POS
	call	CS_MENU_FSR0_WTPos

	;; branch depending on parameter part:
	;; 0 = note, 1 = Octave, 2 = Slide/Gate, 3 = Accent, 4 = Parameter
	movf	CS_MENU_PARAMETER_IX, W
	bz	CS_MENU_PRINT_B_SEQ_VAL_Note
	addlw	-1
	bz	CS_MENU_PRINT_B_SEQ_VAL_Oct
	addlw	-1
	bz	CS_MENU_PRINT_B_SEQ_VAL_S_G
	addlw	-1
	bz	CS_MENU_PRINT_B_SEQ_VAL_Acc
	rgoto	CS_MENU_PRINT_B_SEQ_VAL_Par

CS_MENU_PRINT_B_SEQ_VAL_Note
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	movf	INDF0, W
	andlw	0x0f
	TABLE_ADDR_MUL_W CS_MENU_PRINT_SEQ_NOTE_Tab, 2
	movlw	2
	goto	MIOS_LCD_PrintPreconfString
	
CS_MENU_PRINT_B_SEQ_VAL_Oct
	TABLE_ADDR CS_MENU_PRINT_B_SEQ_VAL_Oct_Tab
	swapf	INDF0, W
	andlw	0x03
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_B_SEQ_VAL_Oct_Tab
	dw	" -  "	; mapped to 0
	dw	" Up "	; mapped to 1
	dw	"Dwn "	; mapped to 3
	dw	"Up2 "	; mapped to 2

CS_MENU_PRINT_B_SEQ_VAL_S_G
	movlw	'-'
	btfsc	INDF0, 6
	movlw '*'
	rcall	CS_MENU_PRINT_Char

	movlw	' '
	rcall	CS_MENU_PRINT_Char

	movlw	'-'
	btfsc	INDF0, 7
	movlw '*'
	rgoto	CS_MENU_PRINT_Char

CS_MENU_PRINT_B_SEQ_VAL_Acc
	movlw	0x80		; accent flag is located in upper bank
	addwf	FSR0L, F
	movlw	7
	rgoto	CS_MENU_PRINT_OnOff_INDF0

CS_MENU_PRINT_B_SEQ_VAL_Par
	movlw	0x80		; 7bit parameter is located in upper bank
	addwf	FSR0L, F
	movf	INDF0, W
	andlw	0x7f
	rgoto	CS_MENU_PRINT_Dec
	
; --------------------------------------------------------------------------
CS_MENU_PRINT_D_MODEL
	rcall	CS_MENU_PRINT_GetVx
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_DModel

; --------------------------------------------------------------------------
CS_MENU_PRINT_RND_GEN
	movlw	'G'
	rcall	CS_MENU_PRINT_Char
	movlw	'e'
	rcall	CS_MENU_PRINT_Char
	movlw	'n'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_SAVE_BNK
	movff	CS_MENU_SAVE_BANK, SID_BANK
	movff	CS_MENU_SAVE_PATCH, SID_PATCH
	call	SID_PBANK_CheckPatchValid
	bz	CS_MENU_PRINT_SAVE_BAK_Ok
	movf	SID_BANK, W	; always allow to store in bank #0
	bz	CS_MENU_PRINT_SAVE_BAK_Ok
	
CS_MENU_PRINT_SAVE_BNK_Inv
	movf	CS_MENU_SAVE_BANK, W
	addlw	'A'
	rcall	CS_MENU_PRINT_Char
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	rgoto	CS_MENU_PRINT_Char

CS_MENU_PRINT_SAVE_BAK_Ok
	movff	CS_MENU_SAVE_BANK, PRODL
	movff	CS_MENU_SAVE_PATCH, PRODH
	goto	SID_LCD_PrintPatchNumber

; --------------------------------------------------------------------------
CS_MENU_PRINT_SAVE_PAT
	return			; (already print)

; --------------------------------------------------------------------------
CS_MENU_PRINT_SAVE_NAME
	;; (dirty but required for save menu) set cursor
	movlw	0x45
	call	MIOS_LCD_CursorSet

	movff	CS_MENU_SAVE_PATCH, SID_PATCH
	movff	CS_MENU_SAVE_BANK, SID_BANK
	goto	SID_LCD_PrintPatchEE

; --------------------------------------------------------------------------
CS_MENU_PRINT_SAVE_ENS
	call	SID_EBANK_CheckValid
	bz	CS_MENU_PRINT_SAVE_ENS_Ok
	movf	CS_MENU_SAVE_ENS, W	; always allow to store in bank #0
	bz	CS_MENU_PRINT_SAVE_ENS_Ok
	
CS_MENU_PRINT_SAVE_ENS_Inv
	movlw	'E'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	rgoto	CS_MENU_PRINT_Char

CS_MENU_PRINT_SAVE_ENS_Ok
	movff	CS_MENU_SAVE_ENS, PRODL
	goto	SID_LCD_PrintEnsembleNumberPRODL


; --------------------------------------------------------------------------
CS_MENU_PRINT_ENSxIx_DECP1
	rcall	CS_MENU_PRINT_GetEnsIx
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENSxIx_CHN
	rcall	CS_MENU_PRINT_GetEnsIx
	andlw	0xf0
	bnz	CS_MENU_PRINT_ENSxIx_CHN_Dis
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_DecPlus1
CS_MENU_PRINT_ENSxIx_CHN_Dis
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENSxIx_NOTE
	rcall	CS_MENU_PRINT_GetEnsIx
	goto	SID_NOTE_Print

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENSxIx_PMDEC7
	rcall	CS_MENU_PRINT_GetEnsIx
	rgoto	CS_MENU_PRINT_PMDec7

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENSx_X2A
	call	CS_MENU_FSR0_ENS_CTRL2
	movf	CS_MENU_PARAMETER_IX, W		; contains x2A flag
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENSx_RESERVED
	return

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENSx_MONO
	call	CS_MENU_FSR0_ENS
	movlw	SID_ENS_CTRL1_MONO
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENSx_SPV
	call	CS_MENU_FSR0_ENS
	movf	INDF0, W
	andlw	0x07
	bz	CS_MENU_PRINT_ENSx_SPV_Zero
	call	MIOS_LCD_PrintBCD2
	movlw	' '
	rgoto	CS_MENU_PRINT_Char

CS_MENU_PRINT_ENSx_SPV_Zero
	movlw	'o'
	rcall	CS_MENU_PRINT_Char
	movlw	'f'
	rcall	CS_MENU_PRINT_Char
	movlw	'f'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENSx_EXTINV
	call	CS_MENU_FSR0_ENS
	movf	FSR0L, W	; (CS_MENU_PARAMETER_IX and FSR0L contain offset to bit instead of address)
	andlw	0xf0
	addlw	SID_ENSx_AOUT_INVERTED
	movwf	FSR0L
	movf	CS_MENU_PARAMETER_IX, W
	rgoto	CS_MENU_PRINT_OnOff_INDF0
	

; ==========================================================================
; ==========================================================================
;  Help Functions
; ==========================================================================
; ==========================================================================

; --------------------------------------------------------------------------
;; get variable (value in WREG, pointer in FSR0)
CS_MENU_PRINT_GetVAR
	clrf	FSR0H
	movf	INDF0, W
	return
	
; --------------------------------------------------------------------------
;; get ENSx_Ix variable (value in WREG, pointer in FSR0)
CS_MENU_PRINT_GetEnsIx
	call	CS_MENU_FSR0_ENS_Ix
	movf	INDF0, W
	return

; --------------------------------------------------------------------------
;; get Vx variable (value in WREG, pointer in FSR0)
CS_MENU_PRINT_GetVx
	call	CS_MENU_FSR0_Voice
	movf	INDF0, W
	return

; --------------------------------------------------------------------------
;; get LFOx variable (value in WREG, pointer in FSR0)
CS_MENU_PRINT_GetLFOx
	call	CS_MENU_FSR0_LFO
	movf	INDF0, W
	return

; --------------------------------------------------------------------------
;; get ENVx variable (value in WREG, pointer in FSR0)
CS_MENU_PRINT_GetENVx
	call	CS_MENU_FSR0_ENV
	movf	INDF0, W
	return

; --------------------------------------------------------------------------
;; get MODx variable (value in WREG, pointer in FSR0)
CS_MENU_PRINT_GetMODx
	call	CS_MENU_FSR0_MOD
	movf	INDF0, W
	return

; --------------------------------------------------------------------------
;; print character
CS_MENU_PRINT_Char
	goto	MIOS_LCD_PrintChar

; --------------------------------------------------------------------------
;; print hex value 00..FF
CS_MENU_PRINT_Hex
	movwf	TMP1
	movlw	' '		; fill with space
	rcall	CS_MENU_PRINT_Char
	movf	TMP1, W
	goto	MIOS_LCD_PrintHex2

; --------------------------------------------------------------------------
;; print hex value -0x40..0x3f
CS_MENU_PRINT_PMHex
	movwf	TMP1
	movlw	'+'		; "+" or "-"?
	btfss	TMP1, 6; (if value <= 0x3f (6th bit cleared), print "-"
	movlw '-'
	rcall	CS_MENU_PRINT_Char

	movf	TMP1, W		; calc: 0x40-value
	andlw	0x7f
	sublw	0x40
	btfsc	WREG, 7			; negate value if negative to get absolute value
	negf	WREG, ACCESS
	goto	MIOS_LCD_PrintHex2	; and print it

; --------------------------------------------------------------------------
;; print dec value 1..128
CS_MENU_PRINT_DecPlus1
	addlw	1
CS_MENU_PRINT_Dec
;; print dec value 0..127
	goto	MIOS_LCD_PrintBCD3

; --------------------------------------------------------------------------
;; print dec value patted with 0
CS_MENU_PRINT_Dec000
	clrf	MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	movf	MIOS_PARAMETER2, W
	call	MIOS_LCD_PrintHex1
	movf	MIOS_PARAMETER1, W
	goto	MIOS_LCD_PrintHex2

; --------------------------------------------------------------------------
;; print dec value -64..63
CS_MENU_PRINT_PMDec7
	movwf	TMP1
	movlw	' '		; space or "-"?
	btfss	TMP1, 6; (if value <= 0x3f (6th bit cleared), print "-"
	movlw '-'
	rcall	CS_MENU_PRINT_Char

CS_MENU_PRINT_PMDec7_WO_Sign
	movf	TMP1, W		; calc: 0x40-value
	sublw	0x40
	btfsc	WREG, 7			; negate value if negative to get absolute value
	negf	WREG, ACCESS
	goto	MIOS_LCD_PrintBCD2	; and print it

; --------------------------------------------------------------------------
;; print dec value -128..127
CS_MENU_PRINT_PMDec8
	BRA_IFCLR WREG, 7, ACCESS, CS_MENU_PRINT_PMDec8_Neg
CS_MENU_PRINT_PMDec8_Pos
	;; positive value: always 3 characters
	andlw	0x7f
	goto	MIOS_LCD_PrintBCD3

CS_MENU_PRINT_PMDec8_Neg
	;; negative value: if <= -100, use special character for "-1"
	sublw	0x80
	movwf	TMP1
	addlw	-100
	bc	CS_MENU_PRINT_PMDec8_Neg_3
CS_MENU_PRINT_PMDec8_Neg_2
	movlw	'-'
	rcall	CS_MENU_PRINT_Char
	movf	TMP1, W
	goto	MIOS_LCD_PrintBCD2
CS_MENU_PRINT_PMDec8_Neg_3
	movlw	0x06		; "-1" in one character
	rcall	CS_MENU_PRINT_Char
	movf	TMP1, W
	addlw	-100
#if 0
	goto	MIOS_LCD_PrintBCD2
#else
	clrf	MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	goto	MIOS_LCD_PrintHex2
#endif


; --------------------------------------------------------------------------
;; bit which should be selected in WREG, byte in MIOS_PARAMETER1
CS_MENU_PRINT_OnOff_INDF0
	movff	INDF0, MIOS_PARAMETER1
CS_MENU_PRINT_OnOff
	call	MIOS_HLP_GetBitORMask
	andwf	MIOS_PARAMETER1, W
	bnz 	CS_MENU_PRINT_OnOff1
CS_MENU_PRINT_OnOff0
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	movlw	'o'
	rcall	CS_MENU_PRINT_Char
	movlw	' '
	rgoto	CS_MENU_PRINT_Char
CS_MENU_PRINT_OnOff1
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	movlw	'*'
	rcall	CS_MENU_PRINT_Char
	movlw	' '
	rgoto	CS_MENU_PRINT_Char


; --------------------------------------------------------------------------
;; print string from table, offset in WREG
CS_MENU_PRINT_StringFromTable
	rlf	WREG, F
	rlf	WREG, W
	andlw	0xfc
	TABLE_ADD_W
	movlw	0x04
	goto	MIOS_LCD_PrintPreconfString

; --------------------------------------------------------------------------
;; print 123 depending on TMP[2..0]
CS_MENU_PRINT_123
	movwf	TMP1
	movlw	'-'
	btfsc	TMP1, 0
	movlw '1'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	TMP1, 1
	movlw '2'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	TMP1, 2
	movlw '3'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
;; prints the name of a drum model
CS_MENU_PRINT_DModel
	TABLE_ADDR_MUL_W SID_DMODEL_TABLE, 2
	tblrd*+
	movff	TABLAT, PRODL
	tblrd*+
	movff	TABLAT, TBLPTRH
	movff	PRODL, TBLPTRL
	movlw	0x04
	goto	MIOS_LCD_PrintPreconfString


; --------------------------------------------------------------------------
;; print the 16 selected sequencer steps (called from cs_menu.inc, 
CS_MENU_PRINT_SeqSteps
	;; pointer to 16bit entry -> FSR0
	call	CS_MENU_FSR0_WTPos
	;; print 16 steps
	movff	POSTINC0, TMP1	; gate shift register (Step 1-8)
	movff	POSTINC0, TMP3	; accent shift register (Step 1-8)
	movff	POSTINC0, TMP2	; gate shift register (Step 9-16)
	movff	POSTINC0, TMP4	; accent shift register (Step 9-16)
	movlw	16
	movwf	PRODL		; counter
CS_MENU_PRINT_SeqSteps_Loop
	;; coding:
	;; Gate  Accent  Result
	;;    0       0  Don't play note
	;;    1       0  Play Note w/o accent
	;;    0       1  Play Note w/ accent
	;;    1       1  Play Secondary Note

	BRA_IFSET TMP1, 0, ACCESS, CS_MENU_PRINT_SeqSteps_Loop_G1
CS_MENU_PRINT_SeqSteps_Loop_G0
	BRA_IFSET TMP3, 0, ACCESS, CS_MENU_PRINT_SeqSteps_Loop_G0A1
CS_MENU_PRINT_SeqSteps_Loop_G0A0
	movlw	0xa5	; (dot)
	rgoto	CS_MENU_PRINT_SeqSteps_Loop_Cont
CS_MENU_PRINT_SeqSteps_Loop_G0A1
	movlw	'A'
	rgoto	CS_MENU_PRINT_SeqSteps_Loop_Cont

CS_MENU_PRINT_SeqSteps_Loop_G1
	BRA_IFSET TMP3, 0, ACCESS, CS_MENU_PRINT_SeqSteps_Loop_G1A1
CS_MENU_PRINT_SeqSteps_Loop_G1A0
	movlw	'*'
	rgoto	CS_MENU_PRINT_SeqSteps_Loop_Cont
CS_MENU_PRINT_SeqSteps_Loop_G1A1
	movlw	'S'
	;; 	rgoto	CS_MENU_PRINT_SeqSteps_Loop_Cont

CS_MENU_PRINT_SeqSteps_Loop_Cont
	rcall	CS_MENU_PRINT_Char
	rrf	TMP2, F
	rrf	TMP1, F
	rrf	TMP4, F
	rrf	TMP3, F
	decfsz	PRODL, F
	rgoto	CS_MENU_PRINT_SeqSteps_Loop

	return	
	

; --------------------------------------------------------------------------
;; print 7bit hex/dec number depending on WT_DEC_VIEW flag
;; value in WREG
CS_MENU_PRINT_WTVal
	BRA_IFCLR CS_STAT2, CS_STAT2_WT_DEC_VIEW, ACCESS, CS_MENU_PRINT_Hex
	rgoto	CS_MENU_PRINT_Dec

CS_MENU_PRINT_PMWTVal
	BRA_IFCLR CS_STAT2, CS_STAT2_WT_DEC_VIEW, ACCESS, CS_MENU_PRINT_PMHex

	movwf	TMP1
	movlw	'+'		; "+" or "-"?
	btfss	TMP1, 6; (if value <= 0x3f (6th bit cleared), print "-"
	movlw '-'
	rcall	CS_MENU_PRINT_Char
	rgoto	CS_MENU_PRINT_PMDec7_WO_Sign



; --------------------------------------------------------------------------
;  Large String tables (located here to ensure that CS_MENU_PRINT_Handler can
;  work with 2byte "rgoto"'s
; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_VAS_Tab
	dw	" LR "
	dw	" L- "
	dw	" -R "
	dw	" O1 "
	dw	" O2 "
	dw	" O3 "
	dw	" O4 "
	dw	" O5 "
	dw	" O6 "

CS_MENU_PRINT_Vx_ARP_DIR_Tab
	dw	"Up  "
	dw	"Dwn "
	dw	"U&D "
	dw	"D&U "
	dw	"UD2 "
	dw	"DU2 "
	dw	"Rnd "
	dw	"r.7 "		; reserved

CS_MENU_PRINT_LFOx_WAV_Tab
	dw	"off "
	dw	"Sin "
	dw	"Tri "
	dw	"Saw "
	dw	"Pul "
	dw	"Ran "
	dw	"PSn "
	dw	"PTr "
	dw	"PSw "
	dw	"PPl "

CS_MENU_PRINT_LFOx_RATE_Tab
	dw	"64/1"
	dw	"32/1"
	dw	"16/1"
	dw	"8/1 "
	dw	"4/1 "
	dw	"2/1 "
	dw	"1/1 "
	dw	"1/2 "
	dw	"1/4 "
	dw	"1/8 "
	dw	"1/16"

CS_MENU_PRINT_ENVx_Loop_Tab
	dw	"--- "
	dw	"At1 "
	dw	"At2 "
	dw	"De1 "
	dw	"De2 "
	dw	"Sus "
	dw	"Re1 "
	dw	"Re2 "

CS_MENU_PRINT_ENVx_CAS_Tab
	dw	"--- "
	dw	"A-- "
	dw	"-D- "
	dw	"AD- "
	dw	"--R "
	dw	"A-R "
	dw	"-DR "
	dw	"ADR "

CS_MENU_PRINT_MODx_SRC_Tab
	dw	"--- "
	dw	"EG1 "
	dw	"EG2 "
	dw	"LF1 "
	dw	"LF2 "
	dw	"LF3 "
	dw	"LF4 "
	dw	"LF5 "
	dw	"LF6 "
	dw	"MP1 "
	dw	"MP2 "
	dw	"MP3 "
	dw	"MP4 "
	dw	"MP5 "
	dw	"MP6 "
	dw	"MP7 "
	dw	"MP8 "
	dw	"MdW "
	dw	"Key "
	dw	"K#1 "
	dw	"K#2 "
	dw	"K#3 "
	dw	"K#4 "
	dw	"K#5 "
	dw	"K#V "
	dw	"K#P "
	dw	"K#A "
	dw	"WT1 "
	dw	"WT2 "
	dw	"WT3 "
	dw	"WT4 "

CS_MENU_PRINT_MODx_OP_Tab
	dw	"--- "
	dw	"Sr1 "
	dw	"Sr2 "
	dw	"1+2 "
	dw	"1-2 "
	dw	"1*2 "
	dw	"XOR "
	dw	"OR  "
	dw	"AND "
	dw	"MIN "
	dw	"MAX "
	dw	"1<2 "
	dw	"1>2 "
	dw	"1=2 "
	dw	"S&H "
	dw	"r.2 "

CS_MENU_PRINT_MODx_TAR_Tab
	dw	"--- "
	dw	"Pt1 "
	dw	"Pt2 "
	dw	"Pt3 "
	dw	"Pt4 "
	dw	"Pt5 "
	dw	"Pt6 "
	dw	"PW1 "
	dw	"PW2 "
	dw	"PW3 "
	dw	"PW4 "
	dw	"PW5 "
	dw	"PW6 "
	dw	"Fl1 "
	dw	"Fl2 "
	dw	"Vl1 "
	dw	"Vl2 "
	dw	"LD1 "
	dw	"LD2 "
	dw	"LD3 "
	dw	"LD4 "
	dw	"LD5 "
	dw	"LD6 "
	dw	"LR1 "
	dw	"LR2 "
	dw	"LR3 "
	dw	"LR4 "
	dw	"LR5 "
	dw	"LR6 "
	dw	"Ex1 "
	dw	"Ex2 "
	dw	"Ex3 "
	dw	"Ex4 "
	dw	"Ex5 "
	dw	"Ex6 "
	dw	"Ex7 "
	dw	"Ex8 "
	dw	"WT1 "
	dw	"WT2 "
	dw	"WT3 "
	dw	"WT4 "

CS_MENU_PRINT_TRGx_SRC_Tab
	dw	"NOn "
	dw	"NOf "
	dw	"E1S "
	dw	"E2S "
	dw	"L1P "
	dw	"L2P "
	dw	"L3P "
	dw	"L4P "
	dw	"L5P "
	dw	"L6P "
	dw	"Clk "
	dw	"Cl6 "
	dw	"C24 "
	dw	"MSt "

CS_MENU_PRINT_KNB_Tab
	dw	"K#1 "
	dw	"K#2 "
	dw	"K#3 "
	dw	"K#4 "
	dw	"K#5 "
	dw	"K#V "
	dw	"K#P "
	dw	"K#A "

CS_MENU_PRINT_SEQ_NOTE_Tab
	db	"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B ", "C+"
