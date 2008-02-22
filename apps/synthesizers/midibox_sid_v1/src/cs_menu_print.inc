; $Id$
;
; SID Control Surface Menu Parameter Print Functions
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
PRINT_NOP		EQU	0x00

PRINT_DEC		EQU	0x01
PRINT_DECP1		EQU	0x02
PRINT_PMDEC		EQU	0x03
PRINT_CHN		EQU	0x04
PRINT_DEV		EQU	0x05
PRINT_BNK		EQU	0x06
PRINT_PAT		EQU	0x07
PRINT_NAME		EQU	0x08
PRINT_PLY		EQU	0x09
PRINT_SUK		EQU	0x0a
PRINT_CLK		EQU	0x0b
PRINT_NOTE		EQU	0x0c

PRINT_VAR_123		EQU	0x0d
PRINT_VAR_DEC		EQU	0x0e
PRINT_VAR_DECP1		EQU	0x0f
PRINT_VAR_MOD		EQU	0x10
PRINT_VAR_MOD_N		EQU	0x11
PRINT_VAR_WTPOS		EQU	0x12
PRINT_VAR_WTMODE	EQU	0x13
PRINT_VAR_WTPAR		EQU	0x14
PRINT_VAR_303SG		EQU	0x15
PRINT_VAR_303ACC	EQU	0x16
	
PRINT_Vx_DEC		EQU	0x17
PRINT_Vx_PMDEC		EQU	0x18
PRINT_Vx_WAV		EQU	0x19
PRINT_Vx_S_R		EQU	0x1a
PRINT_Vx_OPS		EQU	0x1b

PRINT_FILTER_CHN	EQU	0x1c
PRINT_FILTER_MOD	EQU	0x1d
PRINT_FILTER_EXT	EQU	0x1e
PRINT_FILTER_3OF	EQU	0x1f

PRINT_LFOx_WAV		EQU	0x20
PRINT_LFOx_DEC		EQU	0x21
PRINT_LFOx_PMDEC	EQU	0x22
PRINT_LFOx_SYN		EQU	0x23

PRINT_ENVx_DEC		EQU	0x24
PRINT_ENVx_PMDEC	EQU	0x25
PRINT_ENVx_CUR		EQU	0x26
PRINT_ENVx_CAS		EQU	0x27

PRINT_303_MOD		EQU	0x28
PRINT_303_DEC		EQU	0x29

PRINT_SAVE_BNK		EQU	0x2a
PRINT_SAVE_PAT		EQU	0x2b
PRINT_SAVE_NAME		EQU	0x2c

PRINT_KTR		EQU	0x2d

PRINT_SEO_303		EQU	0x2e
PRINT_SEO_FIP		EQU	0x2f
PRINT_SEO_E2P		EQU	0x30
PRINT_SEO_E2V		EQU	0x31
PRINT_SEO_GSA		EQU	0x32

; ==========================================================================
;  The PRINT handler
;; IN: PRINT_xxx ID in WREG
; ==========================================================================
CS_MENU_PRINT_Handler
	JUMPTABLE_2BYTES (0x32+1) ; entries
	rgoto	CS_MENU_PRINT_NOP

	rgoto	CS_MENU_PRINT_DEC
	rgoto	CS_MENU_PRINT_DECP1
	rgoto	CS_MENU_PRINT_PMDEC
	rgoto	CS_MENU_PRINT_CHN
	rgoto	CS_MENU_PRINT_DEV
	rgoto	CS_MENU_PRINT_BNK
	rgoto	CS_MENU_PRINT_PAT
	rgoto	CS_MENU_PRINT_NAME
	rgoto	CS_MENU_PRINT_PLY
	rgoto	CS_MENU_PRINT_SUK
	rgoto	CS_MENU_PRINT_CLK
	rgoto	CS_MENU_PRINT_NOTE

	rgoto	CS_MENU_PRINT_VAR_123
	rgoto	CS_MENU_PRINT_VAR_DEC
	rgoto	CS_MENU_PRINT_VAR_DECP1
	rgoto	CS_MENU_PRINT_VAR_MOD
	rgoto	CS_MENU_PRINT_VAR_MOD_N
	rgoto	CS_MENU_PRINT_VAR_WTPOS
	rgoto	CS_MENU_PRINT_VAR_WTMODE
	rgoto	CS_MENU_PRINT_VAR_WTPAR
	rgoto	CS_MENU_PRINT_VAR_303SG
	rgoto	CS_MENU_PRINT_VAR_303ACC
	
	rgoto	CS_MENU_PRINT_Vx_DEC
	rgoto	CS_MENU_PRINT_Vx_PMDEC
	rgoto	CS_MENU_PRINT_Vx_WAV
	rgoto	CS_MENU_PRINT_Vx_S_R
	rgoto	CS_MENU_PRINT_Vx_OPS

	rgoto	CS_MENU_PRINT_FILTER_CHN
	rgoto	CS_MENU_PRINT_FILTER_MOD
	rgoto	CS_MENU_PRINT_FILTER_EXT
	rgoto	CS_MENU_PRINT_FILTER_3OF

	rgoto	CS_MENU_PRINT_LFOx_WAV
	rgoto	CS_MENU_PRINT_LFOx_DEC
	rgoto	CS_MENU_PRINT_LFOx_PMDEC
	rgoto	CS_MENU_PRINT_LFOx_SYN

	rgoto	CS_MENU_PRINT_ENVx_DEC
	rgoto	CS_MENU_PRINT_ENVx_PMDEC
	rgoto	CS_MENU_PRINT_ENVx_CUR
	rgoto	CS_MENU_PRINT_ENVx_CAS

	rgoto	CS_MENU_PRINT_303_MOD
	rgoto	CS_MENU_PRINT_303_DEC

	rgoto	CS_MENU_PRINT_SAVE_BNK
	rgoto	CS_MENU_PRINT_SAVE_PAT
	rgoto	CS_MENU_PRINT_SAVE_NAME

	rgoto	CS_MENU_PRINT_KTR

	rgoto	CS_MENU_PRINT_SEO_303
	rgoto	CS_MENU_PRINT_SEO_FIP
	rgoto	CS_MENU_PRINT_SEO_E2P
	rgoto	CS_MENU_PRINT_SEO_E2V
	rgoto	CS_MENU_PRINT_SEO_GSA


; ==========================================================================
; ==========================================================================
; ==========================================================================

CS_MENU_PRINT_NOP
	return

; --------------------------------------------------------------------------
CS_MENU_PRINT_DEC
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_DECP1
	incf	INDF0, W
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_PMDEC
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_PMDec

; --------------------------------------------------------------------------
CS_MENU_PRINT_CHN
	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	andlw	0x03
	addwf	FSR1L, F
	incf	INDF1, W
	andlw	0x7f		; mask out "sent" flag
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_DEV
#if AUTO_DEVICE_ID == 0
	movf	CS_MENU_SID, W
	andlw	0x03
	bnz	CS_MENU_PRINT_DEV_Invalid
	incf	SID_MIDI_DEVICE, W
	rgoto	CS_MENU_PRINT_Dec
#endif
	rgoto	CS_MENU_PRINT_Invalid

; --------------------------------------------------------------------------
CS_MENU_PRINT_BNK
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	call	CS_MENU_MS_GetSIDBank
	movwf	MIOS_PARAMETER1
	addlw	'A'
	rcall	CS_MENU_PRINT_Char
	
	movf	MIOS_PARAMETER1, W	; add '*' if BankStick not available!
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_STATUS, W
	movlw	' '
	skpnz
	movlw	'*'
	rgoto	CS_MENU_PRINT_Char
	
; --------------------------------------------------------------------------
CS_MENU_PRINT_PAT
	call	CS_MENU_MS_GetSIDPatch
	addlw	1
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_NAME
	clrf	TMP1
CS_MENU_PRINT_NAME_Loop
	movf	POSTINC0, W
	skpnz
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	incf	TMP1, F
	movlw	4-1
	cpfsgt	TMP1, ACCESS
	rgoto CS_MENU_PRINT_NAME_Loop
	return

; --------------------------------------------------------------------------
CS_MENU_PRINT_PLY
	TABLE_ADDR CS_MENU_PRINT_PLY_Tab
	movlw	0
	btfsc	INDF0, 0
	movlw 1
	btfsc	INDF0, 1
	movlw 2
	btfsc	INDF0, 2
	movlw 3
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_PLY_Tab
	dw	"Mon "
	dw	"Leg "
	dw	"WTS "
	dw	"Pol "

; --------------------------------------------------------------------------
CS_MENU_PRINT_SUK
	movff	INDF0, MIOS_PARAMETER1
	movlw	0
	rgoto	CS_MENU_PRINT_OnOff

; --------------------------------------------------------------------------
CS_MENU_PRINT_CLK
	TABLE_ADDR CS_MENU_PRINT_CLK_Tab
	movf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_CLK_Tab
	dw	"Int "
	dw	"W-- "
	dw	"-L- "
	dw	"WL- "
	dw	"--E "
	dw	"W-E "
	dw	"-LE "
	dw	"WLE "

; --------------------------------------------------------------------------
CS_MENU_PRINT_NOTE
	movf	INDF0, W
	goto	SID_NOTE_Print

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_123
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_123

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_DEC
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_DECP1
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	rgoto	CS_MENU_PRINT_DecPlus1

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_MOD
	TABLE_ADDR CS_MENU_PRINT_MOD_Tab
	rcall	CS_MENU_PRINT_GetVAR	; pointer to variable in FSR0
	andlw	0x07
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_MOD_Tab
	dw	"O1P "
	dw	"O2P "
	dw	"O3P "
	dw	"O1W "
	dw	"O2W "
	dw	"O3W "
	dw	"Fil "

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_MOD_N
	movf	FSR0L, W
	andlw	0x07
	movwf	TMP1
	movlw	0x80
	andwf	FSR0L, F
	movf	TMP1, W
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_MENU_PRINT_VAR_MOD_E1
	rgoto	CS_MENU_PRINT_VAR_MOD_E2
	rgoto	CS_MENU_PRINT_VAR_MOD_L1
	rgoto	CS_MENU_PRINT_VAR_MOD_L2
	rgoto	CS_MENU_PRINT_VAR_MOD_L3
	rgoto	CS_MENU_PRINT_VAR_MOD_L4
	rgoto	CS_MENU_PRINT_VAR_MOD_L5
	rgoto	CS_MENU_PRINT_VAR_MOD_L6

	;; For all others just print a "o" or "*" depending on status
CS_MENU_PRINT_VAR_MOD_E1
	call	CS_MENU_Hlp_AddModENVOffset
	call	CS_MENU_Hlp_AddModENVBitOffset
	bnz	CS_MENU_PRINT_VAR_MOD_E1_4
CS_MENU_PRINT_VAR_MOD_E1_0
	movlw	0
	rgoto	CS_MENU_PRINT_OnOff_INDF0
CS_MENU_PRINT_VAR_MOD_E1_4
	movlw	4
	rgoto	CS_MENU_PRINT_OnOff_INDF0

CS_MENU_PRINT_VAR_MOD_E2
	call	CS_MENU_Hlp_AddModENVOffset
	call	CS_MENU_Hlp_AddModENVBitOffset
	bnz	CS_MENU_PRINT_VAR_MOD_E2_5
CS_MENU_PRINT_VAR_MOD_E2_4
	movlw	1
	rgoto	CS_MENU_PRINT_OnOff_INDF0
CS_MENU_PRINT_VAR_MOD_E2_5
	movlw	5
	rgoto	CS_MENU_PRINT_OnOff_INDF0

CS_MENU_PRINT_VAR_MOD_L1
	call	CS_MENU_Hlp_AddModLFOOffset
	movlw	0
	rgoto	CS_MENU_PRINT_OnOff_INDF0
CS_MENU_PRINT_VAR_MOD_L2
	call	CS_MENU_Hlp_AddModLFOOffset
	movlw	1
	rgoto	CS_MENU_PRINT_OnOff_INDF0
CS_MENU_PRINT_VAR_MOD_L3
	call	CS_MENU_Hlp_AddModLFOOffset
	movlw	2
	rgoto	CS_MENU_PRINT_OnOff_INDF0
CS_MENU_PRINT_VAR_MOD_L4
	call	CS_MENU_Hlp_AddModLFOOffset
	movlw	3
	rgoto	CS_MENU_PRINT_OnOff_INDF0
CS_MENU_PRINT_VAR_MOD_L5
	call	CS_MENU_Hlp_AddModLFOOffset
	movlw	4
	rgoto	CS_MENU_PRINT_OnOff_INDF0
CS_MENU_PRINT_VAR_MOD_L6
	call	CS_MENU_Hlp_AddModLFOOffset
	movlw	5
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_WTPOS
	;; value 0x00-0x1f: print decimal number
	;; value >= 0x20: print "All"
	movlw	0x1f
	cpfsgt	CS_MENU_WT_POS, ACCESS
	rgoto CS_MENU_PRINT_VAR_DEC

	movlw	'A'
	rcall	CS_MENU_PRINT_Char
	movlw	'l'
	rcall	CS_MENU_PRINT_Char
	movlw	'l'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_WTMODE
	TABLE_ADDR CS_MENU_PRINT_WTMODE_Tab
	movf	INDF0, W
	andlw	0x03
	rcall	CS_MENU_PRINT_StringFromTable

	;; add '!' if value has been modified
	btfss	CS_MENU_WT_MODIFIED, 0
	return
	movlw	'!'
	rgoto	CS_MENU_PRINT_Char

CS_MENU_PRINT_WTMODE_Tab
	dw	"  - "
	dw	"Ply "
	dw	"Jmp "
	dw	"End "

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_WTPAR
	movf	INDF0, W
	andlw	0x7f
	BRA_IFCLR INDF0, 7, ACCESS, CS_MENU_PRINT_VAR_WTPAR_PM
CS_MENU_PRINT_VAR_WTPAR_Px
	;; special case: if Mode == Ply, and CC# in between 0x08..0x0c: print note instead of hex number

	;; temporary switch to CS_SID_WT_MODE, store FSR0L so that we are able to restore the old pointer
	movf	FSR0L, W
	movwf	TMP1
	andlw	0x80
	addlw	CS_SID_WT_MODE
	movwf	FSR0L
	movf	INDF0, W	; get WT mode
	movff	TMP1, FSR0L	; restore FSR0 pointer
	addlw	-1		; only print note if WT mode == 1 (Ply)
	bnz	CS_MENU_PRINT_VAR_WTPAR_Px_Hex

	;; get WT assignment which corresponds to displayed entry
	movlw	-CS_SID_WT_P1 + CS_SID_WAVETABLE_P1_ASSIGN
	addwf	FSR0L, F
	movf	INDF0, W
	movff	TMP1, FSR0L	; restore FSR0 pointer
	andlw	0xfc
	xorlw	0x08
	bnz	CS_MENU_PRINT_VAR_WTPAR_Px_Hex
CS_MENU_PRINT_VAR_WTPAR_Px_Note
	;; if INDF0 >= 0xfc, print Ky1...Ky4 (for arpeggiator)
	movf	INDF0, W
	andlw	0xfc
	xorlw	0xfc
	bnz	CS_MENU_PRINT_VAR_WTPAR_Px_NoArp
CS_MENU_PRINT_VAR_WTPAR_Px_Arp
	movlw	'K'
	rcall	CS_MENU_PRINT_Char
	movlw	'y'
	rcall	CS_MENU_PRINT_Char
	movf	INDF0, W
	andlw	0x03
	addlw	1
	call	MIOS_LCD_PrintBCD1
	rgoto	CS_MENU_PRINT_VAR_WTPAR_End

CS_MENU_PRINT_VAR_WTPAR_Px_NoArp
	;; if INDF0 == 0, print "---" (for no note)
	movf	INDF0, W
	andlw	0x7f
	bz	CS_MENU_PRINT_VAR_WTPAR_Px_NoN
	;; if INDF0 == 1, print "+++" (for hold note)
	addlw	-1
	bz	CS_MENU_PRINT_VAR_WTPAR_Px_Hold
	;; else print note name
CS_MENU_PRINT_VAR_WTPAR_Px_NoHld
	addlw	+1
	rcall	CS_MENU_PRINT_Note
	rgoto	CS_MENU_PRINT_VAR_WTPAR_End

CS_MENU_PRINT_VAR_WTPAR_Px_NoN
	movlw	'-'
	rgoto	CS_MENU_PRINT_VAR_WTPAR_Px_NoNC
CS_MENU_PRINT_VAR_WTPAR_Px_Hold
	movlw	'+'
CS_MENU_PRINT_VAR_WTPAR_Px_NoNC
	movwf	TABLAT
	rcall	CS_MENU_PRINT_Char
	movf	TABLAT, W
	rcall	CS_MENU_PRINT_Char
	movf	TABLAT, W
	rcall	CS_MENU_PRINT_Char
	rgoto	CS_MENU_PRINT_VAR_WTPAR_End

CS_MENU_PRINT_VAR_WTPAR_Px_Hex
	movf	INDF0, W
	andlw	0x7f
	rcall	CS_MENU_PRINT_Hex
	rgoto	CS_MENU_PRINT_VAR_WTPAR_End

CS_MENU_PRINT_VAR_WTPAR_PM
	rcall	CS_MENU_PRINT_PMHex
	;; 	rgoto	CS_MENU_PRINT_VAR_WTPAR_End

CS_MENU_PRINT_VAR_WTPAR_End
	;; add '!' if value has been modified
	movf	FSR0L, W
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	andwf	CS_MENU_WT_MODIFIED, W
	skpnz
	return
	movlw	'!'
	rgoto	CS_MENU_PRINT_Char
	
; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_303SG
	movlw	'-'
	btfsc	INDF0, 5
	movlw '*'
	rcall	CS_MENU_PRINT_Char
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	btfsc	INDF0, 4
	movlw '*'
	rcall	CS_MENU_PRINT_Char

	;; add '!' if value has been modified
	btfss	CS_MENU_WT_MODIFIED, 1
	return
	movlw	'!'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
CS_MENU_PRINT_VAR_303ACC
	movf	INDF0, W
	andlw	0x0f
	rcall	CS_MENU_PRINT_Dec

	;; add '!' if value has been modified
	btfss	CS_MENU_WT_MODIFIED, 1
	return
	movlw	'!'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
	
CS_MENU_PRINT_Vx_DEC
	rcall	CS_MENU_PRINT_GetVx
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_Vx_PMDEC
	rcall	CS_MENU_PRINT_GetVx
	rgoto	CS_MENU_PRINT_PMDec

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
	dw	"Puls"
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
CS_MENU_PRINT_Vx_OPS
CS_MENU_PRINT_FILTER_CHN
	;; print selected channels
	TABLE_ADDR CS_MENU_PRINT_FILTER_CHN_Tab
	movf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_FILTER_CHN_Tab
	dw	"--- "
	dw	"1-- "
	dw	"-2- "
	dw	"12- "
	dw	"--3 "
	dw	"1-3 "
	dw	"-23 "
	dw	"123 "

; --------------------------------------------------------------------------
CS_MENU_PRINT_FILTER_MOD
	TABLE_ADDR CS_MENU_PRINT_FILTER_MOD_Tab
	movf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_FILTER_MOD_Tab
	dw	"--- "
	dw	"L-- "
	dw	"-B- "
	dw	"LB- "
	dw	"--H "
	dw	"L-H "
	dw	"-BH "
	dw	"LBH "

; --------------------------------------------------------------------------
CS_MENU_PRINT_FILTER_EXT
CS_MENU_PRINT_FILTER_3OF
	movff	INDF0, MIOS_PARAMETER1
	movlw	3
	rgoto	CS_MENU_PRINT_OnOff

; --------------------------------------------------------------------------
CS_MENU_PRINT_LFOx_WAV
	TABLE_ADDR CS_MENU_PRINT_LFOx_WAV_Tab
	;; waveform stored in INDF0[6..4]
	;; 5 waveforms provided
	;; add 1, index 0 notifies "LFO Off"
	rcall	CS_MENU_PRINT_GetLFOx
	swapf	WREG, W
	andlw	0x07
	addlw	1
	btfss	INDF0, 0
	movlw 0x00
	andlw	0x07
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_LFOx_WAV_Tab
	dw	"off "
	dw	"Sin "
	dw	"Tri "
	dw	"Saw "
	dw	"Pul "
	dw	"Ran "
	dw	"AIN "

; --------------------------------------------------------------------------
CS_MENU_PRINT_LFOx_DEC
	rcall	CS_MENU_PRINT_GetLFOx
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_LFOx_PMDEC
	rcall	CS_MENU_PRINT_GetLFOx
	rgoto	CS_MENU_PRINT_PMDec

; --------------------------------------------------------------------------
CS_MENU_PRINT_LFOx_SYN
	rcall	CS_MENU_PRINT_GetLFOx
	TABLE_ADDR CS_MENU_PRINT_LFOx_SYN_Tab
	movf	INDF0, W
	movlw	0x00
	btfsc	INDF0, 2
	movlw 0x01
	btfsc	INDF0, 3
	movlw 0x02
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_LFOx_SYN_Tab
	dw	"no  "
	dw	"Asn "
	dw	"All "

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENVx_DEC
	rcall	CS_MENU_PRINT_GetENVx
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENVx_PMDEC
	rcall	CS_MENU_PRINT_GetENVx
	rgoto	CS_MENU_PRINT_PMDec

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENVx_CUR
	movf	CS_MENU_SELECTED_ENV, W
	addwf	FSR0L, F
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_PMDec

; --------------------------------------------------------------------------
CS_MENU_PRINT_ENVx_CAS
	TABLE_ADDR CS_MENU_PRINT_ENVx_CAS_Tab
	movf	INDF0, W
	btfsc	CS_MENU_SELECTED_ENV, 0
	swapf	INDF0, W
	andlw	0x07
	rgoto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_ENVx_CAS_Tab
	dw	"--- "
	dw	"A-- "
	dw	"-D- "
	dw	"AD- "
	dw	"--R "
	dw	"A-R "
	dw	"-DR "
	dw	"ADR "	

; --------------------------------------------------------------------------
CS_MENU_PRINT_303_MOD
CS_MENU_PRINT_303_DEC
	call	CS_MENU_Hlp_AddENV1Offset
	movf	INDF0, W
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_SAVE_BNK

	;; check if BankStick target available (only required for SID_BANK > 0)
	movf	CS_MENU_SAVE_BANK, W
	bz	CS_MENU_PRINT_SAVE_BAK_NoChk
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_STATUS, W
	bnz	CS_MENU_PRINT_SAVE_BAK_NoChk

	movf	CS_MENU_SAVE_BANK, W
	addlw	'A'
	rcall	CS_MENU_PRINT_Char
	rgoto	CS_MENU_PRINT_Invalid

CS_MENU_PRINT_SAVE_BAK_NoChk
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

	movff	SID_PATCH, TMP4
	movff	SID_BANK, TMP5

	movff	CS_MENU_SAVE_PATCH, SID_PATCH
	movff	CS_MENU_SAVE_BANK, SID_BANK

	call	SID_LCD_PrintPatchEE

	movff	TMP4, SID_PATCH
	movff	TMP5, SID_BANK
	return

; --------------------------------------------------------------------------
CS_MENU_PRINT_KTR
	rrf	INDF0, W
	andlw	0x3f
	rgoto	CS_MENU_PRINT_Dec

; --------------------------------------------------------------------------
CS_MENU_PRINT_SEO_303
	movlw	SE_OPTION_TB303
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_SEO_FIP
	movlw	SE_OPTION_FIP
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_SEO_E2P
	movlw	SE_OPTION_ENV2PORTA
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_SEO_E2V
	movlw	SE_OPTION_ENV2VOL
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------
CS_MENU_PRINT_SEO_GSA
	movlw	SE_OPTION_GSA
	rgoto	CS_MENU_PRINT_OnOff_INDF0

; --------------------------------------------------------------------------

; ==========================================================================
; ==========================================================================
;  Help Functions
; ==========================================================================
; ==========================================================================

; --------------------------------------------------------------------------
;; get variable (value in WREG, pointer in FSR0)
CS_MENU_PRINT_GetVAR
	clrf	FSR0H
	bcf	FSR0L, 7
	movf	INDF0, W
	return
	
; --------------------------------------------------------------------------
;; get WT variable (value in WREG, pointer in FSR0)
CS_MENU_PRINT_GetWTVAR
	movf	INDF0, W
	return

; --------------------------------------------------------------------------
;; get Vx variable (value in WREG, pointer in FSR0)
CS_MENU_PRINT_GetVx
	call	CS_MENU_Hlp_AddVoiceOffset
	movf	INDF0, W
	return

; --------------------------------------------------------------------------
;; get ENVx variable (value in WREG, pointer in FSR0)
CS_MENU_PRINT_GetLFOx
	call	CS_MENU_Hlp_AddLFOOffset
	movf	INDF0, W
	return

; --------------------------------------------------------------------------
;; get ENVx variable (value in WREG, pointer in FSR0)
CS_MENU_PRINT_GetENVx
	call	CS_MENU_Hlp_AddENVOffset
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
	movlw	0x03
	goto	MIOS_LCD_PrintPreconfString

; --------------------------------------------------------------------------
;; print spaces
CS_MENU_PRINT_Spaces
	goto	CS_MENU_Hlp_PrintSpaces

; --------------------------------------------------------------------------
;; print a note
CS_MENU_PRINT_Note
	goto	SID_NOTE_Print

; --------------------------------------------------------------------------
;; print a bank number
CS_MENU_PRINT_Bank
	movwf	TMP1
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	movf	TMP1, W
	addlw	'A'
	rgoto	CS_MENU_PRINT_Char

; --------------------------------------------------------------------------
;; print 1234 depending on TMP[2..0]
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
;; print " --"
CS_MENU_PRINT_Invalid
	movlw	' '
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	rcall	CS_MENU_PRINT_Char
	movlw	'-'
	rgoto	CS_MENU_PRINT_Char

