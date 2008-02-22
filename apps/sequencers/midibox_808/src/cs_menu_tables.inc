; $Id$
;
; Control Surface Menu Tables
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; structures are defined here:
#include "cs_menu_tables_struct.inc"

; ==========================================================================
;  CS_MENU_TABLE_ROOT --- 
;  structure defined and commented in cs_menu_tables_struct.inc
; ==========================================================================
CS_MENU_TABLE_ROOT
CS_MENU_TABLE_ROOT_Header
	CS_MENU_HEADER	(CS_MENU_TABLE_ROOT_End-CS_MENU_TABLE_ROOT_Begin)/CS_MENU_ENTRY_LEN, SEQ_LCD_PrintHeader, CS_M_NOP, SEQ_LED_Update, CS_M_NOP
CS_MENU_TABLE_ROOT_Begin
	CS_MENU_ENTRY	"Edit          ", CS_M_EDIT_Init,    CS_M_EDIT_Tick,    CS_M_NOP,          CS_M_NOP
	CS_MENU_ENTRY	"Mute Tracks   ", CS_M_MUTE_Init,    CS_M_MUTE_Tick,    CS_M_NOP,          CS_M_NOP
	CS_MENU_ENTRY	"Pattern       ", CS_M_PATTERN_Init, CS_M_PATTERN_Tick, CS_M_NOP,          CS_M_NOP
	CS_MENU_ENTRY	"Pattern Bank  ", CS_M_PATTERN_B_Init,CS_M_PATTERN_B_Tick,CS_M_NOP,        CS_M_NOP
	CS_MENU_ENTRY	"Pattern Save  ", CS_M_PATTERN_S_Init,CS_M_PATTERN_S_Tick,CS_M_NOP,        CS_M_NOP
	CS_MENU_ENTRY	"Song          ", CS_M_SONG_Init,    CS_M_SONG_Tick,    CS_M_NOP,          CS_M_NOP
	CS_MENU_ENTRY	"Utilities     ", CS_M_UTILS_Init,   CS_M_UTILS_Tick,   CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"MB808 Options ", CS_M_OPT_Init,     CS_M_OPT_Tick,     SEQ_DUMP_StoreGlobal, CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Direction     ", CS_M_TRKDIR_Init,  CS_M_TRKDIR_Tick,  CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Timebase      ", CS_M_TRKDIV_Init,  CS_M_TRKDIV_Tick,  CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Length/Loop   ", CS_M_TRKLOOP_Init, CS_M_TRKLOOP_Tick, CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Progression   ", CS_M_TRKPROG_Init, CS_M_TRKPROG_Tick, CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Groove        ", CS_M_TRKGRV_Init,  CS_M_TRKGRV_Tick,  CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Aux Trigger   ", CS_M_TRKASG_Init,  CS_M_TRKASG_Tick,  CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Spare         ", CS_M_SPARE_Init,   CS_M_SPARE_Tick,   CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Random Genert.", CS_M_TRKRND_Init,  CS_M_TRKRND_Tick,  CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"BPM           ", CS_M_BPM_Init,     CS_M_BPM_Tick,     SEQ_DUMP_StoreGlobal, CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Velocity      ", CS_M_TRKVEL_Init,  CS_M_TRKVEL_Tick,  CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"MIDI Config.  ", CS_M_TRKMIDI_Init, CS_M_TRKMIDI_Tick, CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Send SysEx    ", CS_M_SYSEX_Init,   CS_M_SYSEX_Tick,   CS_M_NOP,          CS_MENU_IncCursorPos
CS_MENU_TABLE_ROOT_End


;; following defines have to be kept consistent with the table above!
CS_MENU_PAGE_EDIT	EQU	0x00
CS_MENU_PAGE_MUTE	EQU	0x01
CS_MENU_PAGE_PATTERN	EQU	0x02
CS_MENU_PAGE_PATTERN_B	EQU	0x03
CS_MENU_PAGE_PATTERN_S	EQU	0x04
CS_MENU_PAGE_SONG	EQU	0x05
CS_MENU_PAGE_UTILS	EQU	0x06
CS_MENU_PAGE_OPT	EQU	0x07
CS_MENU_PAGE_DIRECTION	EQU	0x08
CS_MENU_PAGE_DIVIDER	EQU	0x09
CS_MENU_PAGE_LOOP	EQU	0x0a
CS_MENU_PAGE_PROGR	EQU	0x0b
CS_MENU_PAGE_GROOVE	EQU	0x0c
CS_MENU_PAGE_ASSIGN	EQU	0x0d
CS_MENU_PAGE_SPARE	EQU	0x0e
CS_MENU_PAGE_RANDOM	EQU	0x0f
CS_MENU_PAGE_BPM	EQU	0x10
CS_MENU_PAGE_VEL	EQU	0x11
CS_MENU_PAGE_MIDI	EQU	0x12
CS_MENU_PAGE_SYSEX	EQU	0x13


; ==========================================================================
;  the Menu Pos <-> General Purpose Buttons Map
; ==========================================================================
CS_MENU_TABLE_GPMAP
	db	CS_MENU_PAGE_EDIT, CS_MENU_PAGE_UTILS
	db	CS_MENU_PAGE_OPT, CS_MENU_PAGE_DIRECTION
	db	CS_MENU_PAGE_DIVIDER, CS_MENU_PAGE_LOOP
	db	CS_MENU_PAGE_PROGR, CS_MENU_PAGE_GROOVE
	db	CS_MENU_PAGE_ASSIGN, CS_MENU_PAGE_SPARE
	db	CS_MENU_PAGE_SPARE, CS_MENU_PAGE_RANDOM
	db	CS_MENU_PAGE_BPM, CS_MENU_PAGE_VEL
	db	CS_MENU_PAGE_MIDI, CS_MENU_PAGE_SYSEX

; ==========================================================================
;  this function maps the 16 GP buttons to 16 most wanted menu entries (shortcuts)
;  IN: number of GP button
;  OUT: appr. menu position
;  USES: TBLPTR[LH]
; ==========================================================================
CS_MENU_TABLE_GPMapGetPos
	addlw	CS_MENU_TABLE_GPMAP & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	CS_MENU_TABLE_GPMAP >> 8
	addwfc	TBLPTRH, F
	tblrd*+
	movf	TABLAT, W
	return

; ==========================================================================
;  this function returns a LED pattern which should matches with the 
;  selected menu
;  IN: CS_MENU_POS
;  OUT: LED pattern in MIOS_PARAMETER[12]
;  USES: TBLPTR[LH], MIOS_PARAMETER3
; ==========================================================================
CS_MENU_TABLE_GPMapGetLED
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	clrf	MIOS_PARAMETER3
	TABLE_ADDR CS_MENU_TABLE_GPMAP
CS_MENU_TABLE_GPMapGetLEDLoop
	tblrd*+
	movf	TABLAT, W
	cpfseq	CS_MENU_POS, ACCESS
	rgoto CS_MENU_TABLE_GPMapGetLEDLoopN
	movf	MIOS_PARAMETER3, W
	call	MIOS_HLP_GetBitORMask
	btfss	MIOS_PARAMETER3, 3
	movwf MIOS_PARAMETER1
	btfsc	MIOS_PARAMETER3, 3
	movwf MIOS_PARAMETER2
CS_MENU_TABLE_GPMapGetLEDLoopN
	incf	MIOS_PARAMETER3, F
	BRA_IFCLR MIOS_PARAMETER3, 4, ACCESS, CS_MENU_TABLE_GPMapGetLEDLoop
	return



; ==========================================================================
;  dummy function for "no operation"
; ==========================================================================
CS_M_NOP
	return

