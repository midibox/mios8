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
#define CS_MENU_LABEL_EMPTY	0x00
#define CS_MENU_LABEL_LEAD	0x01
#define CS_MENU_LABEL_BASSLINE	0x02
#define CS_MENU_LABEL_DRUM	0x03
#define CS_MENU_LABEL_MULTI	0x04
#define CS_MENU_LABEL_ENS_SID	0x05
#define CS_MENU_LABEL_ENS_CLK	0x06
#define CS_MENU_LABEL_ENS_FIL	0x07
#define CS_MENU_LABEL_ENS_INS	0x08
#define CS_MENU_LABEL_ENS_EXT	0x09
#define CS_MENU_LABEL_SAVE	0x0a
#define CS_MENU_LABEL_OSC	0x0b
#define CS_MENU_LABEL_O23	0x0c
#define CS_MENU_LABEL_INS	0x0d
#define CS_MENU_LABEL_FIL	0x0e
#define CS_MENU_LABEL_LFO	0x0f
#define CS_MENU_LABEL_ENV	0x10
#define CS_MENU_LABEL_MOD	0x11
#define CS_MENU_LABEL_TRG	0x12
#define CS_MENU_LABEL_CFG	0x13
#define CS_MENU_LABEL_ARP	0x14
#define CS_MENU_LABEL_KNB	0x15
#define CS_MENU_LABEL_EXT	0x16
#define CS_MENU_LABEL_WTC	0x17
#define CS_MENU_LABEL_WTE	0x18
#define CS_MENU_LABEL_SQC	0x19
#define CS_MENU_LABEL_SEQ	0x1a
#define CS_MENU_LABEL_RND	0x1b

	;; order of entries has to be aligned with definitions above and the label handler below!
CS_MENU_LABEL_TABLE_ENTRY_SIZE EQU 12 ; characters
CS_MENU_LABEL_TABLE
	db	"            "	; 0x00
	db	"Lead Engine "	; 0x01
	db	"Bassline    "	; 0x02
	db	"Drum Engine "	; 0x03
	db	"Multi Engine"	; 0x04
	db	"Ens. SID    "	; 0x05
	db	"Ens. Clock  "	; 0x06
	db	"Ens. Filter "	; 0x07
	db	"Ens. Instr. "	; 0x08
	db	"Ens. Extern "	; 0x09
	db	"Save        "	; 0x0a
	db	"Oscillator  "	; 0x0b
	db	"Oscillator23"	; 0x0c
	db	"Instrument  "	; 0x0d
	db	"Filter      "	; 0x0e
	db	"Low Frq. Osc"	; 0x0f
	db	"Envelope    "	; 0x10
	db	"Modulation  "	; 0x11
	db	"Triggers    "	; 0x12
	db	"Config.     "	; 0x13
	db	"Arpeggiator "	; 0x14
	db	"Knobs       "	; 0x15
	db	"Externals   "	; 0x16
	db	"WT Config.  "	; 0x17
	db	"WT Editor   "	; 0x18
	db	"Seq. Config."	; 0x19
	db	"Sequencer   "	; 0x1a
	db	"Randomizer  "	; 0x1b


;; --------------------------------------------------------------------------
;;  Prints label at upper line of 4x20 display (if enabled)
;;  IN: label ID in CS_MENU_LABEL
;;      4-character segment number in CS_MENU_LABEL_CTR
;; --------------------------------------------------------------------------
CS_MENU_LABEL_Handler
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
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x01
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x02
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x03
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x04
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x05
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x06
	rgoto	CS_MENU_LABEL_Handler_SIDLR	; 0x07
	rgoto	CS_MENU_LABEL_Handler_SIDINS	; 0x08
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x09
	rgoto	CS_MENU_LABEL_Handler_SID	; 0x0a
	rgoto	CS_MENU_LABEL_Handler_OSCLR	; 0x0b
	rgoto	CS_MENU_LABEL_Handler_OSCLR	; 0x0c
	rgoto	CS_MENU_LABEL_Handler_INS	; 0x0d
	rgoto	CS_MENU_LABEL_Handler_LR	; 0x0e
	rgoto	CS_MENU_LABEL_Handler_LFO	; 0x0f
	rgoto	CS_MENU_LABEL_Handler_ENV	; 0x10
	rgoto	CS_MENU_LABEL_Handler_MOD	; 0x11
	rgoto	CS_MENU_LABEL_Handler_TRG	; 0x12
	rgoto	CS_MENU_LABEL_Handler_LR_B	; 0x13
	rgoto	CS_MENU_LABEL_Handler_OSCLR	; 0x14
	rgoto	CS_MENU_LABEL_Handler_KNOB	; 0x15
	rgoto	CS_MENU_LABEL_Handler_Empty	; 0x16
	rgoto	CS_MENU_LABEL_Handler_WT	; 0x17
	rgoto	CS_MENU_LABEL_Handler_Empty	; 0x18
	rgoto	CS_MENU_LABEL_Handler_LR_B	; 0x19
	rgoto	CS_MENU_LABEL_Handler_Empty	; 0x1a
	rgoto	CS_MENU_LABEL_Handler_Empty	; 0x1b


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
;; help function
CS_MENU_LABEL_Handler_PrnSpace
	movlw	' '
CS_MENU_LABEL_Handler_PrnChar
	goto	MIOS_LCD_PrintChar
