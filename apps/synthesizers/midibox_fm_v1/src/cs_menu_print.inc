; $Id$
;
; MBFM Control Surface Menu Parameter Print Functions
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
;  All supported PRINT_* indices
;; (max allowed number: 0x3f!)
; ==========================================================================
PRINT_NOP			EQU	0x00

PRINT_VAR_BANK		EQU	0x01
PRINT_VAR_DEC		EQU	0x02
PRINT_VAR_DECP1		EQU	0x03
PRINT_VAR_1234		EQU	0x04
PRINT_VAR_WTSTEP	EQU	0x05
PRINT_VAR_WTMODE	EQU	0x06
PRINT_VAR_WTPAR		EQU	0x07
PRINT_VAR_BD_OP		EQU	0x08
PRINT_VAR_HH		EQU	0x09

PRINT_Px_BANK		EQU	0x0a
PRINT_Px_DEC		EQU	0x0b
PRINT_Px_PMDEC		EQU	0x0c
PRINT_Px_DECP1		EQU	0x0d
PRINT_Px_NOTE		EQU	0x0e
PRINT_Px_WAVE		EQU	0x0f
PRINT_Px_VEL		EQU	0x10
PRINT_Px_1234		EQU	0x11
PRINT_Px_PLAY		EQU	0x12
PRINT_Px_CON		EQU	0x13
PRINT_Px_SUSK		EQU	0x14
PRINT_Px_MSYN		EQU	0x15
PRINT_PxOx_DEC		EQU	0x16
PRINT_PxOx_DECP1	EQU	0x17
PRINT_PxOx_WAVE		EQU	0x18
PRINT_PxOx_TREM		EQU	0x19
PRINT_PxOx_VIBR		EQU	0x1a
PRINT_PxOx_EGT		EQU	0x1b
PRINT_PxOx_KSR		EQU	0x1c
PRINT_PxOx_KSL		EQU	0x1d
PRINT_Px_CAS		EQU	0x1e

PRINT_PxLFO_DEC		EQU	0x1f
PRINT_PxLFO_PMDEC	EQU	0x20
PRINT_PxLFO_WAVE	EQU	0x21
PRINT_PxLFO_SYNC	EQU	0x22
PRINT_Px_WTSYNC		EQU	0x23

PRINT_SAVE_TYPE		EQU	0x24
PRINT_SAVE_BANK		EQU	0x25
PRINT_SAVE_SLOT		EQU	0x26
PRINT_SAVE_NAME		EQU	0x27

PRINT_PDxBD_DEC		EQU	0x28
PRINT_PDxBD_DECP1	EQU	0x29
PRINT_PDxBD_WAVE	EQU	0x2a
PRINT_PDxHH_DEC		EQU	0x2b
PRINT_PDxHH_DECP1	EQU	0x2c
PRINT_PDxHH_WAVE	EQU	0x2d
PRINT_PDxBD_CON		EQU	0x2e
PRINT_DRUM_CHN		EQU	0x2f

PRINT_I1Px_DEC		EQU	0x30
PRINT_I2Px_DEC		EQU	0x31
PRINT_I3Px_DEC		EQU	0x32
PRINT_I4Px_DEC		EQU	0x33

PRINT_I1Px_PMDEC	EQU	0x34
PRINT_I2Px_PMDEC	EQU	0x35
PRINT_I3Px_PMDEC	EQU	0x36
PRINT_I4Px_PMDEC	EQU	0x37

PRINT_AOUTxINST 	EQU	0x38
PRINT_AOUTxASSG 	EQU	0x39
PRINT_AOUTxOFFS 	EQU	0x3a

PRINT_RND_GEN		EQU	0x3b

; ==========================================================================
;  The PRINT handler
;; IN: PRINT_xxx ID in WREG
; ==========================================================================
CS_MENU_PRINT_Handler
	JUMPTABLE_2BYTES (0x3b+1) ; entries
	rgoto	CS_MENU_PRINT_NOP

	rgoto	CS_MENU_PRINT_VAR_BANK
	rgoto	CS_MENU_PRINT_VAR_DEC
	rgoto	CS_MENU_PRINT_VAR_DECP1
	rgoto	CS_MENU_PRINT_VAR_1234
	rgoto	CS_MENU_PRINT_VAR_WTSTEP
	rgoto	CS_MENU_PRINT_VAR_WTMODE
	rgoto	CS_MENU_PRINT_VAR_WTPAR
	rgoto	CS_MENU_PRINT_VAR_BD_OP
	rgoto	CS_MENU_PRINT_VAR_HH

	rgoto	CS_MENU_PRINT_Px_BANK
	rgoto	CS_MENU_PRINT_Px_DEC
	rgoto	CS_MENU_PRINT_Px_PMDEC
	rgoto	CS_MENU_PRINT_Px_DECP1
	rgoto	CS_MENU_PRINT_Px_NOTE
	rgoto	CS_MENU_PRINT_Px_WAVE
	rgoto	CS_MENU_PRINT_Px_VEL
	rgoto	CS_MENU_PRINT_Px_1234
	rgoto	CS_MENU_PRINT_Px_PLAY
	rgoto	CS_MENU_PRINT_Px_CON
	rgoto	CS_MENU_PRINT_Px_SUSK
	rgoto	CS_MENU_PRINT_Px_MSYN
	rgoto	CS_MENU_PRINT_PxOx_DEC
	rgoto	CS_MENU_PRINT_PxOx_DECP1
	rgoto	CS_MENU_PRINT_PxOx_WAVE
	rgoto	CS_MENU_PRINT_PxOx_TREM
	rgoto	CS_MENU_PRINT_PxOx_VIBR
	rgoto	CS_MENU_PRINT_PxOx_EGT
	rgoto	CS_MENU_PRINT_PxOx_KSR
	rgoto	CS_MENU_PRINT_PxOx_KSL
	rgoto	CS_MENU_PRINT_Px_CAS

	rgoto	CS_MENU_PRINT_PxLFO_DEC
	rgoto	CS_MENU_PRINT_PxLFO_PMDEC
	rgoto	CS_MENU_PRINT_PxLFO_WAVE
	rgoto	CS_MENU_PRINT_PxLFO_SYNC

	rgoto	CS_MENU_PRINT_Px_WTSYNC

	rgoto	CS_MENU_PRINT_SAVE_TYPE
	rgoto	CS_MENU_PRINT_SAVE_BANK
	rgoto	CS_MENU_PRINT_SAVE_SLOT
	rgoto	CS_MENU_PRINT_SAVE_NAME

	rgoto	CS_MENU_PRINT_PDxBD_DEC
	rgoto	CS_MENU_PRINT_PDxBD_DECP1
	rgoto	CS_MENU_PRINT_PDxBD_WAVE
	rgoto	CS_MENU_PRINT_PDxHH_DEC
	rgoto	CS_MENU_PRINT_PDxHH_DECP1
	rgoto	CS_MENU_PRINT_PDxHH_WAVE
	rgoto	CS_MENU_PRINT_PDxBD_CON
	rgoto	CS_MENU_PRINT_DRUM_CHN
	
	rgoto	CS_MENU_PRINT_I1Px_DEC
	rgoto	CS_MENU_PRINT_I2Px_DEC
	rgoto	CS_MENU_PRINT_I3Px_DEC
	rgoto	CS_MENU_PRINT_I4Px_DEC

	rgoto	CS_MENU_PRINT_I1Px_PMDEC
	rgoto	CS_MENU_PRINT_I2Px_PMDEC
	rgoto	CS_MENU_PRINT_I3Px_PMDEC
	rgoto	CS_MENU_PRINT_I4Px_PMDEC

	rgoto	CS_MENU_PRINT_AOUTxINST 
	rgoto	CS_MENU_PRINT_AOUTxASSG
	rgoto	CS_MENU_PRINT_AOUTxOFFS

	rgoto	CS_MENU_PRINT_RND_GEN

; ==========================================================================
; ==========================================================================
; ==========================================================================

CS_MENU_PRINT_NOP
	return

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_BANK
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	rgoto	CS_MENU_PRINT_Bank

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_DEC
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_DECP1
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_1234
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	rgoto	CS_MENU_PRINT_1234

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_WTSTEP
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	;; value 0x00-0x1f: print decimal number
	;; value >= 0x20: print "All"
	movlw	0x1f
	cpfsgt	INDF0, ACCESS
	rgoto CS_MENU_PRINT_VAR_DEC

	movlw	'A'
	rcall	CS_MENU_PRINT_Char
	movlw	'l'
	rcall	CS_MENU_PRINT_Char
	movlw	'l'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_WTMODE
	rcall	CS_MENU_PRINT_GetWTVAR	; pointer to variable in FSR0
	TABLE_ADDR CS_MENU_PRINT_VAR_WTMODE_Tab
	movf	INDF0, W
	andlw	0x03
	rcall	CS_MENU_PRINT_StringFromTable

	;; add '!' if value has been modified
	btfss	CS_MENU_WT_MODIFIED, 0
	return
	movlw	'!'
	rgoto	CS_MENU_PRINT_Char

CS_MENU_PRINT_VAR_WTMODE_Tab
	dw	" -- "
	dw	"Play"
	dw	"Jump"
	dw	"End "

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_WTPAR
	rcall	CS_MENU_PRINT_GetWTVAR	; pointer to variable in FSR0
	andlw	0x7f
	BRA_IFCLR INDF0, 7, ACCESS, CS_MENU_PRINT_VAR_WTPAR_PM
CS_MENU_PRINT_VAR_WTPAR_P
	rcall	CS_MENU_PRINT_Hex
	rgoto	CS_MENU_PRINT_VAR_WTPAR_End
CS_MENU_PRINT_VAR_WTPAR_PM
	rcall	CS_MENU_PRINT_PMHex
CS_MENU_PRINT_VAR_WTPAR_End
	;; add '!' if value has been modified
	movf	CS_MENU_PARAMETER_IX, W
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	andwf	CS_MENU_WT_MODIFIED, W
	skpnz
	return
	movlw	'!'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_BD_OP
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	TABLE_ADDR CS_MENU_PRINT_VAR_BD_OP_Tab
	movf	INDF0, W
	andlw	0x01
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_VAR_BD_OP_Tab
	dw	"Mod."
	dw	"Car."	

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_HH
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	TABLE_ADDR CS_MENU_PRINT_VAR_HH_Tab
	rrf	INDF0, W
	andlw	0x01
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_VAR_HH_Tab
	dw	"Open"
	dw	"Cls."	

; --------------------------------------------------------------------------
CS_MENU_PRINT_Px_BANK
	rcall	CS_MENU_Hlp_GetPxValue
	rgoto	CS_MENU_PRINT_Bank

; --------------------------------------------------------------------------
CS_MENU_PRINT_Px_DEC
	rcall	CS_MENU_Hlp_GetPxValue
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_Px_PMDEC
	rcall	CS_MENU_Hlp_GetPxValue
	rgoto	CS_MENU_PRINT_PMDec

; --------------------------------------------------------------------------
CS_MENU_PRINT_Px_DECP1
	rcall	CS_MENU_Hlp_GetPxValue
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_Px_NOTE
	rcall	CS_MENU_Hlp_GetPxValue
	rgoto	CS_MENU_PRINT_Note

; --------------------------------------------------------------------------
CS_MENU_PRINT_Px_WAVE
#if USE_CUSTOM_CHAR_HANDLER	
	rcall	CS_MENU_Hlp_GetPxValue
	rgoto	CS_MENU_PRINT_OP_Wave
#else
	rgoto	CS_MENU_PRINT_Px_DECP1
#endif

; --------------------------------------------------------------------------
CS_MENU_PRINT_Px_VEL
	rcall	CS_MENU_Hlp_GetPxValue
	movwf	TMP1
	TABLE_ADDR CS_MENU_PRINT_Px_VEL_Tab
	movf	TMP1, W
	andlw	0x03
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_Px_VEL_Tab
	dw	"Flat"
	dw	"Hard"
	dw	"Soft"
	dw	"----"

; --------------------------------------------------------------------------
CS_MENU_PRINT_Px_1234
	rcall	CS_MENU_Hlp_GetPxValue
	rgoto	CS_MENU_PRINT_1234

; --------------------------------------------------------------------------
CS_MENU_PRINT_Px_PLAY
	rcall	CS_MENU_Hlp_GetPxValue
	movwf	TMP1
	TABLE_ADDR CS_MENU_PRINT_Px_PLAY_Tab
	movf	TMP1, W
	andlw	0x03
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_Px_PLAY_Tab
	dw	"Mono"
	dw	"Leg."
	dw	"Poly"
	dw	"----"

; --------------------------------------------------------------------------
#if USE_CUSTOM_CHAR_HANDLER
CS_MENU_PRINT_Px_CON
	rcall	CS_MENU_Hlp_GetPxValue
	andlw 	0x03
	movwf	TMP1
	btfsc	TMP1, 1
	rgoto	CS_MENU_PRINT_Px_CON_SECOND_SET
CS_MENU_PRINT_Px_CON_FIRST_SET
	movlw	0
	call	MIOS_CUSTOM_CHAR_HANDLER_LoadCharset
	rgoto 	CS_MENU_PRINT_Px_CON_DoPrint
CS_MENU_PRINT_Px_CON_SECOND_SET
	movlw	1
	call	MIOS_CUSTOM_CHAR_HANDLER_LoadCharset
	;; rgoto 	CS_MENU_PRINT_Px_CON_DoPrint
CS_MENU_PRINT_Px_CON_DoPrint
	TABLE_ADDR CS_MENU_PRINT_Px_CON_Tab
	movf	TMP1, W
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_Px_CON_Tab
	dw	0x0304, 0x0303
	dw	0x0605, 0x0706
	dw	0x0405, 0x2D07
	dw	0x0305, 0x0706

#else
CS_MENU_PRINT_Px_CON
	rcall	CS_MENU_Hlp_GetPxValue
	movwf	TMP1
	TABLE_ADDR CS_MENU_PRINT_Px_CON_Tab
	movf	TMP1, W
	andlw	0x03
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_Px_CON_Tab
	dw	"F*F "
	dw	"1+F "
	dw	"F+F "
	dw	"AFA "
#endif

; --------------------------------------------------------------------------
CS_MENU_PRINT_Px_SUSK
	rcall	CS_MENU_Hlp_GetPxValue
	movwf	MIOS_PARAMETER1
	movlw	2
	rgoto	CS_MENU_PRINT_OnOff

; --------------------------------------------------------------------------
CS_MENU_PRINT_Px_MSYN
	movlw	0x01
	rcall	CS_MENU_PRINT_Spaces
	rcall	CS_MENU_Hlp_GetPxValue
	movwf	TMP1
	movlw	'-'
	btfsc	TMP1, 2
	movlw 'E'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	TMP1, 1
	movlw 'L'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	TMP1, 0
	movlw 'W'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_PxOx_DEC
	rcall	CS_MENU_Hlp_GetPxOxValue
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_PxOx_DECP1
	rcall	CS_MENU_Hlp_GetPxOxValue
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_PxOx_WAVE
#if USE_CUSTOM_CHAR_HANDLER	
	rcall	CS_MENU_Hlp_GetPxOxValue
	goto	CS_MENU_PRINT_OP_Wave
#else
	rgoto	CS_MENU_PRINT_PxOx_DECP1
#endif

; --------------------------------------------------------------------------
CS_MENU_PRINT_PxOx_TREM
	movlw	3
	rgoto	CS_MENU_PRINT_PxOx_Flags_Cont

; --------------------------------------------------------------------------
CS_MENU_PRINT_PxOx_VIBR
	movlw	2
	rgoto	CS_MENU_PRINT_PxOx_Flags_Cont

; --------------------------------------------------------------------------
CS_MENU_PRINT_PxOx_EGT
	movlw	1
	rgoto	CS_MENU_PRINT_PxOx_Flags_Cont

; --------------------------------------------------------------------------
CS_MENU_PRINT_PxOx_KSR
	movlw	0
	;; 	rgoto	CS_MENU_PRINT_PxOx_Flags_Cont
CS_MENU_PRINT_PxOx_Flags_Cont
	movwf	TMP1
	rcall	CS_MENU_Hlp_GetPxOxValue
	movwf	MIOS_PARAMETER1
	movf	TMP1, W
	rgoto	CS_MENU_PRINT_OnOff

; --------------------------------------------------------------------------
CS_MENU_PRINT_PxOx_KSL
	rcall	CS_MENU_Hlp_GetPxOxValue
	movwf	TMP1
	TABLE_ADDR CS_MENU_PRINT_PxOx_KSL_Tab
	movf	TMP1, W
	andlw	0x03
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_PxOx_KSL_Tab
	dw	"0 dB"
	dw	"1.5 "
	dw	"3.0 "
	dw	"6.0 "

; --------------------------------------------------------------------------
CS_MENU_PRINT_Px_CAS
	rcall	CS_MENU_Hlp_GetPxValue
	movwf	TMP1
	movlw	'-'
	btfsc	TMP1, 0
	movlw 'A'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	TMP1, 1
	movlw 'D'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	TMP1, 2
	movlw 'D'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	TMP1, 3
	movlw 'R'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_PxLFO_DEC
	rcall	CS_MENU_Hlp_GetPxLFOxValue
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_PxLFO_PMDEC
	rcall	CS_MENU_Hlp_GetPxLFOxValue
	rgoto	CS_MENU_PRINT_PMDec

; --------------------------------------------------------------------------
CS_MENU_PRINT_PxLFO_WAVE
	rcall	CS_MENU_Hlp_GetPxLFOxValue
#if USE_CUSTOM_CHAR_HANDLER	
	movwf	TMP1
	;; waveform stored in TMP1[6..4]
	;; 5 waveforms provided
	;; add 1, index 0 notifies "LFO Off"
	swapf	TMP1, W
	andlw	0x07
	addlw	1
	btfss	TMP1, 0
	movlw 0x00
	andlw	0x07
	;; backup value 
	movwf	TMP1
	;; load charset (these start @ 5)
	JUMPTABLE_2BYTES 6
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_NoLoad
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Set0
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Set0
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Set1
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Set2
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Set1
                           
CS_MENU_PRINT_WRITE_LFO_Waveform_Set0
	movlw	5       
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Load
CS_MENU_PRINT_WRITE_LFO_Waveform_Set1
	movlw	6       
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Load
CS_MENU_PRINT_WRITE_LFO_Waveform_Set2
	movlw	4
	;; rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Load
CS_MENU_PRINT_WRITE_LFO_Waveform_Load
	call	MIOS_CUSTOM_CHAR_HANDLER_LoadCharset
CS_MENU_PRINT_WRITE_LFO_Waveform_NoLoad
	;; print it
	TABLE_ADDR CS_MENU_PRINT_WRITE_LFO_Waveform_Tab
	movf	TMP1, W
	goto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_WRITE_LFO_Waveform_Tab
	dw	"--- "
	dw	0x0403, 0x2005
	dw	0x5E06, 0x2007
	dw	0x0403, 0x2005
	dw	0x5F06, 0x2007
	dw	0x0706, 0x2006
#else
	movwf	TMP1
	TABLE_ADDR CS_MENU_PRINT_PxLFO_WAVE_Tab
	;; waveform stored in TMP1[6..4]
	;; 5 waveforms provided
	;; add 1, index 0 notifies "LFO Off"
	swapf	TMP1, W
	andlw	0x07
	addlw	1
	btfss	TMP1, 0
	movlw 0x00
	andlw	0x07
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_PxLFO_WAVE_Tab
	dw	"off "
	dw	"Sine"
	dw	"Tri."
	dw	"Saw "
	dw	"Puls"
	dw	"Rand"
#endif

; --------------------------------------------------------------------------
CS_MENU_PRINT_PxLFO_SYNC
	rcall	CS_MENU_Hlp_GetPxLFOxValue
	movwf	MIOS_PARAMETER1
	movlw	2
	rgoto	CS_MENU_PRINT_OnOff

; --------------------------------------------------------------------------
CS_MENU_PRINT_Px_WTSYNC
	rcall	CS_MENU_Hlp_GetPxValue

	movwf	TMP1
	TABLE_ADDR CS_MENU_PRINT_Px_WTSYNC_Tab
	movf	TMP1, W
	andlw	0x03
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_Px_WTSYNC_Tab
	dw	"Note"
	dw	"Step"
	dw	"Free"
	dw	"----"

; --------------------------------------------------------------------------
CS_MENU_PRINT_SAVE_TYPE
	TABLE_ADDR CS_MENU_PRINT_SAVE_TYPE_Tab
	movf	CS_MENU_SAVE_TYPE, W
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_SAVE_TYPE_Tab
	dw	"Ptch"
	dw	"Drum"	
	dw	"Ens."

; --------------------------------------------------------------------------
CS_MENU_PRINT_SAVE_BANK
	movf	CS_MENU_SAVE_BANK, W
	btfsc	CS_MENU_SAVE_TYPE, 0
	movf	CS_MENU_SAVE_DBANK, W
	btfsc	CS_MENU_SAVE_TYPE, 1
	movf	CS_MENU_SAVE_EBANK, W
	rgoto	CS_MENU_PRINT_Bank

; --------------------------------------------------------------------------
CS_MENU_PRINT_SAVE_SLOT
	movf	CS_MENU_SAVE_PATCH, W
	btfsc	CS_MENU_SAVE_TYPE, 0
	movf	CS_MENU_SAVE_DRUMSET, W
	btfsc	CS_MENU_SAVE_TYPE, 1
	movf	CS_MENU_SAVE_ENSEMBLE, W
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_SAVE_NAME
	movf	CS_MENU_SAVE_TYPE, W
	bz	CS_MENU_PRINT_SAVE_NAME_P
CS_MENU_PRINT_SAVE_NAME_D
CS_MENU_PRINT_SAVE_NAME_E
	movlw	0x10
	rgoto	CS_MENU_PRINT_Spaces

CS_MENU_PRINT_SAVE_NAME_P	
	movff	MBFM_PATCH, TMP4
	movff	MBFM_BANK, TMP5

	movff	CS_MENU_SAVE_PATCH, MBFM_PATCH
	movff	CS_MENU_SAVE_BANK, MBFM_BANK

	movlw	16
	call	MBFM_LCD_PrintPatchEE

	movff	TMP4, MBFM_PATCH
	movff	TMP5, MBFM_BANK
	return

; --------------------------------------------------------------------------
CS_MENU_PRINT_PDxBD_DEC
	rcall	CS_MENU_Hlp_GetPxBDxValue
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_PDxBD_DECP1
	rcall	CS_MENU_Hlp_GetPxBDxValue
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_PDxBD_WAVE
#if USE_CUSTOM_CHAR_HANDLER	
	rcall	CS_MENU_Hlp_GetPxBDxValue
	rgoto	CS_MENU_PRINT_OP_Wave
#else
	rgoto	CS_MENU_PRINT_PDxBD_DECP1
#endif

; --------------------------------------------------------------------------
CS_MENU_PRINT_PDxHH_DEC
	rcall	CS_MENU_Hlp_GetPxHHxValue
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_PDxHH_DECP1
	rcall	CS_MENU_Hlp_GetPxHHxValue
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_PDxHH_WAVE
#if USE_CUSTOM_CHAR_HANDLER	
	rcall	CS_MENU_Hlp_GetPxHHxValue
	rgoto	CS_MENU_PRINT_OP_Wave
#else
	rgoto	CS_MENU_PRINT_PDxHH_DECP1
#endif

; --------------------------------------------------------------------------
CS_MENU_PRINT_PDxBD_CON
	rcall	CS_MENU_Hlp_GetPxBDxValue
	movwf	TMP1
	TABLE_ADDR CS_MENU_PRINT_PDxBD_CON_Tab
	movf	TMP1, W
	andlw	0x01
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_PDxBD_CON_Tab
	dw	" FM "
	dw	" AM "	

; --------------------------------------------------------------------------
CS_MENU_PRINT_DRUM_CHN
	movff	MBFM_DRUM_MIDI_CHANNEL, WREG
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_I1Px_DEC
	movlw	0x00
	;; 	rgoto	CS_MENU_PRINT_IxPx_DEC_Cont

CS_MENU_PRINT_IxPx_DEC_Cont
	movff	WREG, MBFM_CURRENT_INSTRUMENT
	rgoto	CS_MENU_PRINT_Px_DEC

CS_MENU_PRINT_I2Px_DEC
	movlw	0x01
	rgoto	CS_MENU_PRINT_IxPx_DEC_Cont

CS_MENU_PRINT_I3Px_DEC
	movlw	0x02
	rgoto	CS_MENU_PRINT_IxPx_DEC_Cont

CS_MENU_PRINT_I4Px_DEC
	movlw	0x03
	rgoto	CS_MENU_PRINT_IxPx_DEC_Cont

; --------------------------------------------------------------------------
CS_MENU_PRINT_I1Px_PMDEC
	movlw	0x00
	;; 	rgoto	CS_MENU_PRINT_IxPx_PMDEC_Cont

CS_MENU_PRINT_IxPx_PMDEC_Cont
	movff	WREG, MBFM_CURRENT_INSTRUMENT
	rgoto	CS_MENU_PRINT_Px_PMDEC

CS_MENU_PRINT_I2Px_PMDEC
	movlw	0x01
	rgoto	CS_MENU_PRINT_IxPx_PMDEC_Cont

CS_MENU_PRINT_I3Px_PMDEC
	movlw	0x02
	rgoto	CS_MENU_PRINT_IxPx_PMDEC_Cont

CS_MENU_PRINT_I4Px_PMDEC
	movlw	0x03
	rgoto	CS_MENU_PRINT_IxPx_PMDEC_Cont


; --------------------------------------------------------------------------
CS_MENU_PRINT_AOUTxINST
	call	CS_MENU_Hlp_GetPxAOUTxValue
	swapf	WREG, W
	andlw	0x03
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_AOUTxASSG
	movlw	0x01
	rcall	CS_MENU_PRINT_Spaces
	call	CS_MENU_Hlp_GetPxAOUTxValue
	movwf	TMP1
	movlw	'-'
	btfsc	TMP1, 0
	movlw 'L'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	TMP1, 1
	movlw 'L'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	TMP1, 2
	movlw 'E'
	rgoto	CS_MENU_PRINT_Char
	
	
; --------------------------------------------------------------------------
CS_MENU_PRINT_AOUTxOFFS
	call	CS_MENU_Hlp_GetPxAOUTxValue
	rgoto	CS_MENU_PRINT_PMDec

; --------------------------------------------------------------------------
CS_MENU_PRINT_RND_GEN
	movlw	'G'
	rcall	CS_MENU_PRINT_Char
	movlw	'e'
	rcall	CS_MENU_PRINT_Char
	movlw	'n'
	rcall	CS_MENU_PRINT_Char
	movlw	' '
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------


; ==========================================================================
; ==========================================================================
;  Help Functions
; ==========================================================================
; ==========================================================================

; --------------------------------------------------------------------------
;; get variable (value in WREG, pointer in FSR0)
CS_MENU_PRINT_GetVAR
	movff	CS_MENU_PARAMETER_IX, FSR0L
	clrf	FSR0H
	movf	INDF0, W
	return
	
; --------------------------------------------------------------------------
;; get WT variable (value in WREG, pointer in FSR0)
CS_MENU_PRINT_GetWTVAR
	lfsr	FSR0, CS_MENU_WT_MODE
	movf	CS_MENU_PARAMETER_IX, W
	addwf	FSR0L, F
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
;; print dec value 0..127
CS_MENU_PRINT_Dec
	goto	MIOS_LCD_PrintBCD3

; --------------------------------------------------------------------------
;; print dec value -64..63
CS_MENU_PRINT_PMDec
	movwf	TMP1
	movlw	' '		; space or "-"?
	btfss	TMP1, 6; (if value <= 0x3f (6th bit cleared), print "-"
	movlw '-'
	rcall	CS_MENU_PRINT_Char

	movf	TMP1, W		; calc: 0x40-value
	sublw	0x40
	btfsc	WREG, 7			; negate value if negative to get absolute value
	negf	WREG, ACCESS
	goto	MIOS_LCD_PrintBCD2	; and print it

; --------------------------------------------------------------------------
;; bit which should be selected in WREG, byte in MIOS_PARAMETER1
CS_MENU_PRINT_OnOff
	call	MIOS_HLP_GetBitORMask
	andwf	MIOS_PARAMETER1, W
	bnz 	CS_MENU_PRINT_OnOff1
CS_MENU_PRINT_OnOff0
	movlw	'['
	rcall	CS_MENU_PRINT_Char
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	movlw	']'
	rgoto	CS_MENU_PRINT_Char
CS_MENU_PRINT_OnOff1
	movlw	'['
	rcall	CS_MENU_PRINT_Char
	movlw	219
	rcall	CS_MENU_PRINT_Char
	movlw	']'
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
;; print spaces
CS_MENU_PRINT_Spaces
	goto	CS_MENU_Hlp_PrintSpaces

; --------------------------------------------------------------------------
;; print a note
CS_MENU_PRINT_Note
	goto	MBFM_NOTE_Print

; --------------------------------------------------------------------------
;; print a bank number
CS_MENU_PRINT_Bank
	movwf	TMP1
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	movf	TMP1, W
	addlw	'A'
	rcall	CS_MENU_PRINT_Char

	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask
	andwf	MBFM_BANKSTICK_STATUS, W
	movlw	' '
	skpnz
	movlw	'*'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
;; print 1234 depending on TMP[3..0]
CS_MENU_PRINT_1234
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
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	TMP1, 3
	movlw '4'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
;; print waveform
CS_MENU_PRINT_OP_Wave
#if USE_CUSTOM_CHAR_HANDLER
	movwf	TMP1

	JUMPTABLE_2BYTES 8 ; entries
	rgoto CS_MENU_PRINT_OP_Wave_Set0
	rgoto CS_MENU_PRINT_OP_Wave_Set0
	rgoto CS_MENU_PRINT_OP_Wave_Set0
	rgoto CS_MENU_PRINT_OP_Wave_Set0
	rgoto CS_MENU_PRINT_OP_Wave_Set1
	rgoto CS_MENU_PRINT_OP_Wave_Set1
	rgoto CS_MENU_PRINT_OP_Wave_Set1
	rgoto CS_MENU_PRINT_OP_Wave_Set0

CS_MENU_PRINT_OP_Wave_Set0
	movlw	3
	goto	CS_MENU_PRINT_OP_Wave_PrintIt
CS_MENU_PRINT_OP_Wave_Set1
	movlw	4
CS_MENU_PRINT_OP_Wave_PrintIt
	call	MIOS_CUSTOM_CHAR_HANDLER_LoadCharset
	TABLE_ADDR CS_MENU_PRINT_OP_Wave_Tab
	movf	TMP1, W
	goto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_OP_Wave_Tab
	dw	0x0403, 0x0605 
	dw	0x0403, 0x2D2D 
	dw	0x0403, 0x0403
	dw	0x2D07, 0x2D07
	dw	0x0403, 0x2D2D
	dw	0x0303, 0x2D2D
	dw	0x0605, 0x075F
	dw	0x2D04, 0x052D	
	return
#else
	rgoto	CS_MENU_PRINT_PxOx_DECP1
#endif
