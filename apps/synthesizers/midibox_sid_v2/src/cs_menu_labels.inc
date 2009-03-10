; $Id$
;
; Control Surface Menu Labels
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; index numbers, used in cs_menu_tables.inc
CS_MENU_LABEL_EMPTY	EQU	0x00
CS_MENU_LABEL_ENS	EQU	0x01
CS_MENU_LABEL_LEAD	EQU	0x02
CS_MENU_LABEL_BASSLINE	EQU	0x03
CS_MENU_LABEL_DRUM	EQU	0x04
CS_MENU_LABEL_MULTI	EQU	0x05
CS_MENU_LABEL_ENS_SID	EQU	0x06
CS_MENU_LABEL_ENS_CLK	EQU	0x07
CS_MENU_LABEL_ENS_FIL	EQU	0x08
CS_MENU_LABEL_ENS_INS	EQU	0x09
CS_MENU_LABEL_ENS_EXT	EQU	0x0a
CS_MENU_LABEL_SAVE	EQU	0x0b
CS_MENU_LABEL_OSC	EQU	0x0c
CS_MENU_LABEL_O23	EQU	0x0d
CS_MENU_LABEL_INS	EQU	0x0e
CS_MENU_LABEL_FIL	EQU	0x0f
CS_MENU_LABEL_LFO	EQU	0x10
CS_MENU_LABEL_ENV	EQU	0x11
CS_MENU_LABEL_MOD	EQU	0x12
CS_MENU_LABEL_TRG	EQU	0x13
CS_MENU_LABEL_CFG	EQU	0x14
CS_MENU_LABEL_ARP	EQU	0x15
CS_MENU_LABEL_KNB	EQU	0x16
CS_MENU_LABEL_EXT	EQU	0x17
CS_MENU_LABEL_WTC	EQU	0x18
CS_MENU_LABEL_WTE	EQU	0x19
CS_MENU_LABEL_SQC	EQU	0x1a
CS_MENU_LABEL_SEQ	EQU	0x1b
CS_MENU_LABEL_RND	EQU	0x1c

	;; order of entries has to be aligned with definitions above and the label handler below!
CS_MENU_LABEL_TABLE_ENTRY_SIZE EQU 12 ; characters
CS_MENU_LABEL_TABLE
	db	"            "	; 0x00
	db	"xxxxxxxxxxxx"  ; 0x01 (not taken - ENS label is build up by a separate routine, see code below)
	db	"Lead Engine "	; 0x02
	db	"Bassline    "	; 0x03
	db	"Drum Engine "	; 0x04
	db	"Multi Engine"	; 0x05
	db	"Ens. SID    "	; 0x06
	db	"Ens. Clock  "	; 0x07
	db	"Ens. Filter "	; 0x08
	db	"Ens. Instr. "	; 0x09
	db	"Ens. Extern "	; 0x0a
	db	"Save        "	; 0x0b
	db	"Oscillator  "	; 0x0c
	db	"Oscillator23"	; 0x0d
	db	"Instrument  "	; 0x0e
	db	"Filter      "	; 0x0f
	db	"Low Frq. Osc"	; 0x10
	db	"Envelope    "	; 0x11
	db	"Modulation  "	; 0x12
	db	"Triggers    "	; 0x13
	db	"Config.     "	; 0x14
	db	"Arpeggiator "	; 0x15
	db	"Knobs       "	; 0x16
	db	"Externals   "	; 0x17
	db	"WT Config.  "	; 0x18
	db	"WT Editor   "	; 0x19
	db	"Seq. Config."	; 0x1a
	db	"Sequencer   "	; 0x1b
	db	"Randomizer  "	; 0x1c


;; --------------------------------------------------------------------------
;;  Prints label at upper line of 4x20 display (if enabled)
;;  IN: label ID in CS_MENU_LABEL
;;      4-character segment number in CS_MENU_LABEL_CTR
;; --------------------------------------------------------------------------
CS_MENU_LABEL_Handler
	;; branch of dynamically generated ENS label is selected
	movf	CS_MENU_LABEL, W
	xorlw	CS_MENU_LABEL_ENS
	skpnz
	rgoto	CS_MENU_LABEL_Handler_Ens
	
	;; cursor pos = 4 * (4-LABEL_CTR)
	movf	CS_MENU_LABEL_CTR, W
	sublw	4
	mullw	4
	movf	PRODL, W
	iorlw	0x80		; select upper line
	call	MIOS_LCD_CursorSet

	;; branch depending on display piece
	movf	CS_MENU_LABEL_CTR, W
	bz	CS_MENU_LABEL_Handler_34
	addlw	-1
	bz	CS_MENU_LABEL_Handler_34
CS_MENU_LABEL_Handler_012
	;; get pointer to label
	movf	CS_MENU_LABEL, W
	TABLE_ADDR_MUL_W CS_MENU_LABEL_TABLE, CS_MENU_LABEL_TABLE_ENTRY_SIZE
	;; add offset
	movf	CS_MENU_LABEL_CTR, W
	sublw	4
	mullw	4
	movf	PRODL, W
	TABLE_ADD_W

	;; print 4 characters and exit
	movlw	4
	goto	MIOS_LCD_PrintPreconfString


CS_MENU_LABEL_Handler_34
	;; jump depending on label ID
	movf	CS_MENU_LABEL, W
	JUMPTABLE_2BYTES (0x1b + 1) ; entries
	rgoto	CS_MENU_LABEL_Handler_Empty	; 0x00
	rgoto	CS_MENU_LABEL_Handler_Empty	; 0x01 (_Ens label already selected above)
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x02
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x03
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x04
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x05
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x06
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x07
	rgoto	CS_MENU_LABEL_Handler_SIDLR	; 0x08
	rgoto	CS_MENU_LABEL_Handler_SIDINS	; 0x09
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x0a
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x0b
	rgoto	CS_MENU_LABEL_Handler_OSCLR	; 0x0c
	rgoto	CS_MENU_LABEL_Handler_OSCLR	; 0x0d
	rgoto	CS_MENU_LABEL_Handler_INS	; 0x0e
	rgoto	CS_MENU_LABEL_Handler_LR	; 0x0f
	rgoto	CS_MENU_LABEL_Handler_LFO	; 0x10
	rgoto	CS_MENU_LABEL_Handler_ENV	; 0x11
	rgoto	CS_MENU_LABEL_Handler_MOD	; 0x12
	rgoto	CS_MENU_LABEL_Handler_TRG	; 0x13
	rgoto	CS_MENU_LABEL_Handler_LR_B	; 0x14
	rgoto	CS_MENU_LABEL_Handler_OSCLR	; 0x15
	rgoto	CS_MENU_LABEL_Handler_KNOB	; 0x16
	rgoto	CS_MENU_LABEL_Handler_Empty	; 0x17
	rgoto	CS_MENU_LABEL_Handler_WT	; 0x18
	rgoto	CS_MENU_LABEL_Handler_Empty	; 0x19
	rgoto	CS_MENU_LABEL_Handler_LR_B	; 0x1a
	rgoto	CS_MENU_LABEL_Handler_Empty	; 0x1b
	rgoto	CS_MENU_LABEL_Handler_Empty	; 0x1c


;; ---
CS_MENU_LABEL_Handler_Empty
	movlw	4		; empty piece
	goto	SID_LCD_PrintSpaces

;; ---
CS_MENU_LABEL_Handler_SID
	movf	CS_MENU_LABEL_CTR, W
	bnz	CS_MENU_LABEL_Handler_Empty	; 4 characters
CS_MENU_LABEL_Handler_SID_4
CS_MENU_LABEL_Handler_SIDLR_4
CS_MENU_LABEL_Handler_SIDINS_4
	lfsr	FSR0, CS_MENU_SELECTED_SID_FLAGS
	goto	CS_MENU_PRINT_VAR_SID1234	; 4 characters

;; ---
CS_MENU_LABEL_Handler_SIDLR
	movf	CS_MENU_LABEL_CTR, W
	bz	CS_MENU_LABEL_Handler_SIDLR_4
CS_MENU_LABEL_Handler_SIDLR_3
	lfsr	FSR0, CS_MENU_SELECTED_SID_LR	; 3 characters
	call	CS_MENU_PRINT_VAR_LR
	rgoto	CS_MENU_LABEL_Handler_PrnSpace	; 1 character

;; ---
CS_MENU_LABEL_Handler_SIDINS
	movf	CS_MENU_LABEL_CTR, W
	bz	CS_MENU_LABEL_Handler_SIDINS_4
CS_MENU_LABEL_Handler_SIDINS_3
CS_MENU_LABEL_Handler_INS_4
	rcall	CS_MENU_LABEL_Handler_PrnSpace	; 1 character
	movlw	'I'
	rcall	CS_MENU_LABEL_Handler_PrnChar	; 1 character
	incf	CS_MENU_SELECTED_INS, W
	goto	MIOS_LCD_PrintBCD2		; 2 characters

;; ---
CS_MENU_LABEL_Handler_OSCLR
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG ; don't print oscillator number in bassline mode
	andlw	0x03
	xorlw	0x01
	bz	CS_MENU_LABEL_Handler_OSCLR_Bl
	xorlw	0x03 ^ 0x01				; print instrument number in multi mode
	bz	CS_MENU_LABEL_Handler_OSCLR_Mu

	movf	CS_MENU_LABEL_CTR, W
	bz	CS_MENU_LABEL_Handler_OSCLR_4
CS_MENU_LABEL_Handler_OSCLR_3
	lfsr	FSR0, CS_MENU_SELECTED_SID_LR	; 3 characters
	call	CS_MENU_PRINT_VAR_LR
	rgoto	CS_MENU_LABEL_Handler_PrnSpace	; 1 character

CS_MENU_LABEL_Handler_OSCLR_4
	movlw	'O'
	rcall	CS_MENU_LABEL_Handler_PrnChar	; 1 character
	lfsr	FSR0, CS_MENU_SELECTED_OSC_FLAGS; 3 characters
	goto	CS_MENU_PRINT_VAR_123

CS_MENU_LABEL_Handler_OSCLR_Bl
	movf	CS_MENU_LABEL_CTR, W
	bnz	CS_MENU_LABEL_Handler_Empty
	rgoto	CS_MENU_LABEL_Handler_OSCLR_3

CS_MENU_LABEL_Handler_OSCLR_Mu
CS_MENU_LABEL_Handler_WT_Mu
	movf	CS_MENU_LABEL_CTR, W
	bnz	CS_MENU_LABEL_Handler_Empty	; 4 characters
CS_MENU_LABEL_Handler_OSCLR_Mu_4
CS_MENU_LABEL_Handler_LFO_Mu_4
CS_MENU_LABEL_Handler_ENV_Mu_4
	rcall	CS_MENU_LABEL_Handler_PrnSpace	; 1 character
	rcall	CS_MENU_LABEL_Handler_PrnSpace	; 1 character
	movlw	'I'
	rcall	CS_MENU_LABEL_Handler_PrnChar	; 1 character
	incf	CS_MENU_SELECTED_INS, W
	goto	MIOS_LCD_PrintBCD1		; 1 character

;; ---
CS_MENU_LABEL_Handler_INS
	movf	CS_MENU_LABEL_CTR, W
	bz	CS_MENU_LABEL_Handler_INS_4
	rgoto	CS_MENU_LABEL_Handler_Empty

;; ---
CS_MENU_LABEL_Handler_LR
	movf	CS_MENU_LABEL_CTR, W
	bz	CS_MENU_LABEL_Handler_LR_4
	rgoto	CS_MENU_LABEL_Handler_Empty
CS_MENU_LABEL_Handler_LR_4
	rcall	CS_MENU_LABEL_Handler_PrnSpace	; 1 character
	lfsr	FSR0, CS_MENU_SELECTED_SID_LR	; 3 characters
	goto	CS_MENU_PRINT_VAR_LR

;; ---
CS_MENU_LABEL_Handler_LR_B
	;; only print LR in bassline mode
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG ; don't print oscillator number in bassline mode
	andlw	0x03
	xorlw	0x01
	bz	CS_MENU_LABEL_Handler_LR
	rgoto	CS_MENU_LABEL_Handler_Empty	; 4 characters

;; ---
CS_MENU_LABEL_Handler_LFO
	;; print Lx LR in bassline mode
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG ; don't print oscillator number in bassline mode
	andlw	0x03
	xorlw	0x01
	bz	CS_MENU_LABEL_Handler_LFO_B
	xorlw	0x03 ^ 0x01
	bz	CS_MENU_LABEL_Handler_LFO_Mu ; print instrument + env in multi mode

	movf	CS_MENU_LABEL_CTR, W
	bnz	CS_MENU_LABEL_Handler_Empty	; 4 characters
CS_MENU_LABEL_Handler_LFO_4
	rcall	CS_MENU_LABEL_Handler_PrnSpace	; 1 character
	rcall	CS_MENU_LABEL_Handler_PrnSpace	; 1 character
	movlw	'L'
	rcall	CS_MENU_LABEL_Handler_PrnChar	; 1 character
	incf	CS_MENU_SELECTED_LFO, W
	goto	MIOS_LCD_PrintBCD1		; 1 character

CS_MENU_LABEL_Handler_LFO_B
	movf	CS_MENU_LABEL_CTR, W
	bz	CS_MENU_LABEL_Handler_LFO_B_4
CS_MENU_LABEL_Handler_LFO_B_3
	rgoto	CS_MENU_LABEL_Handler_LFO_4
CS_MENU_LABEL_Handler_LFO_B_4
	rgoto	CS_MENU_LABEL_Handler_LR_4

CS_MENU_LABEL_Handler_LFO_Mu
	movf	CS_MENU_LABEL_CTR, W
	bnz	CS_MENU_LABEL_Handler_LFO_4
	rgoto	CS_MENU_LABEL_Handler_LFO_Mu_4

;; ---
CS_MENU_LABEL_Handler_ENV
	;; print LR in bassline mode
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG ; don't print oscillator number in bassline mode
	andlw	0x03
	xorlw	0x01
	bz	CS_MENU_LABEL_Handler_LR
	xorlw	0x03 ^ 0x01
	bz	CS_MENU_LABEL_Handler_ENV_Mu ; print instrument + env in multi mode

	movf	CS_MENU_LABEL_CTR, W
	bnz	CS_MENU_LABEL_Handler_Empty	; 4 characters
CS_MENU_LABEL_Handler_ENV_4
	rcall	CS_MENU_LABEL_Handler_PrnSpace	; 1 character
	rcall	CS_MENU_LABEL_Handler_PrnSpace	; 1 character
	movlw	'E'
	rcall	CS_MENU_LABEL_Handler_PrnChar	; 1 character
	incf	CS_MENU_SELECTED_ENV, W
	goto	MIOS_LCD_PrintBCD1		; 1 character

CS_MENU_LABEL_Handler_ENV_Mu
	movf	CS_MENU_LABEL_CTR, W
	bnz	CS_MENU_LABEL_Handler_ENV_4
	rgoto	CS_MENU_LABEL_Handler_ENV_Mu_4

;; ---
CS_MENU_LABEL_Handler_MOD
	movf	CS_MENU_LABEL_CTR, W
	bnz	CS_MENU_LABEL_Handler_Empty	; 4 characters
CS_MENU_LABEL_Handler_MOD_4
	rcall	CS_MENU_LABEL_Handler_PrnSpace	; 1 character
	rcall	CS_MENU_LABEL_Handler_PrnSpace	; 1 character
	movlw	'M'
	rcall	CS_MENU_LABEL_Handler_PrnChar	; 1 character
	incf	CS_MENU_SELECTED_MOD, W
	goto	MIOS_LCD_PrintBCD1		; 1 character

;; ---
CS_MENU_LABEL_Handler_TRG
	movf	CS_MENU_LABEL_CTR, W
	bnz	CS_MENU_LABEL_Handler_Empty	; 4 characters
CS_MENU_LABEL_Handler_TRG_4
	rcall	CS_MENU_LABEL_Handler_PrnSpace	; 1 character
	lfsr	FSR0, CS_MENU_SELECTED_TRG
	goto	CS_MENU_PRINT_TRGx_SRC		; 3 characters

;; ---
CS_MENU_LABEL_Handler_KNOB
	movf	CS_MENU_LABEL_CTR, W
	bz	CS_MENU_LABEL_Handler_KNOB_4
	rgoto	CS_MENU_LABEL_Handler_Empty	; 4 characters
CS_MENU_LABEL_Handler_KNOB_4
	rcall	CS_MENU_LABEL_Handler_PrnSpace	; 1 character
	lfsr	FSR0, CS_MENU_SELECTED_KNOB
	goto	CS_MENU_PRINT_KNB		; 3 characters
	
;; ---
CS_MENU_LABEL_Handler_WT
	;; print instrument number in multi mode
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x03
	bz	CS_MENU_LABEL_Handler_WT_Mu

	movf	CS_MENU_LABEL_CTR, W
	bz	CS_MENU_LABEL_Handler_WT_4
	rgoto	CS_MENU_LABEL_Handler_Empty	; 4 characters
CS_MENU_LABEL_Handler_WT_4
	rcall	CS_MENU_LABEL_Handler_PrnSpace	; 1 character
	movlw	'W'
	rcall	CS_MENU_LABEL_Handler_PrnChar	; 1 character
	movlw	'T'
	rcall	CS_MENU_LABEL_Handler_PrnChar	; 1 character
	incf	CS_MENU_SELECTED_WT, W
	goto	MIOS_LCD_PrintBCD1		; 1 character


;; ---
	;; exception: all 20 characters are print by this label code
CS_MENU_LABEL_Handler_Ens
	;; select SID
	;; cursor pos = 5 * (4-LABEL_CTR)
	movf	CS_MENU_LABEL_CTR, W
	sublw	4

	btfsc	WREG, 2		; ignore LABEL_CTR == 4
	return

	movwf	CS_MENU_SID

	mullw	5
	movf	PRODL, W
	iorlw	0x80		; select upper line
	call	MIOS_LCD_CursorSet

	;; print engine (2 chars)
	;; or print '--' if SID slave not available
	movff	MBNET_NODE_AVAIL, PRODL
	movf	CS_MENU_SID, W
	call	MIOS_HLP_GetBitORMask
	andwf	PRODL, W
	bnz	CS_MENU_LABEL_Handler_Ens_SIDAv
CS_MENU_LABEL_Handler_Ens_SIDNAv
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	rgoto	CS_MENU_LABEL_Handler_Ens_Cont

CS_MENU_LABEL_Handler_Ens_SIDAv
	call	CS_MENU_MS_GetEngineCode ; returns 2-bit engine code depending on selected SID in CS_MENU_SID
	call	SID_LCD_PrintEngine

CS_MENU_LABEL_Handler_Ens_Cont
	;; print MIDI channel (2 chars, padded with 0)
	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	andlw	0x03
	addwf	FSR1L, F
	incf	INDF1, W
	andlw	0x7f		; mask out "sent" flag
	clrf	MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	movf	MIOS_PARAMETER1, W
	call	MIOS_LCD_PrintHex2

	;; print space
	movlw	' '
	goto	MIOS_LCD_PrintChar


;; ---
;; help function
CS_MENU_LABEL_Handler_PrnSpace
	movlw	' '
CS_MENU_LABEL_Handler_PrnChar
	goto	MIOS_LCD_PrintChar
