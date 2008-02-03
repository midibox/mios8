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
	CS_MENU_HEADER	(CS_MENU_TABLE_ROOT_End-CS_MENU_TABLE_ROOT_Begin)/CS_MENU_ENTRY_LEN, SEQ_LCD_PrintHeader, CS_M_NOP, SEQ_LED_Update, SEQ_MSG_Handler
CS_MENU_TABLE_ROOT_Begin
	CS_MENU_ENTRY	"Edit          ", CS_M_DISPLAY_Init, CS_M_DISPLAY_Tick, CS_M_DISPLAY_Exit, CS_M_DISPLAY_Select
	CS_MENU_ENTRY	"Mute Tracks   ", CS_M_MUTE_Init,    CS_M_MUTE_Tick,    CS_M_NOP,          CS_M_NOP
	CS_MENU_ENTRY	"Patterns      ", CS_M_PATTERN_Init, CS_M_PATTERN_Tick, CS_M_NOP,          CS_M_PATTERN_Select
	CS_MENU_ENTRY	"Song          ", CS_M_SONG_Init,    CS_M_SONG_Tick,    CS_M_NOP,          CS_M_SONG_Select
	CS_MENU_ENTRY	"Track Events  ", CS_M_TRKEVNT_Init, CS_M_TRKEVNT_Tick, CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Track Mode    ", CS_M_TRKMODE_Init, CS_M_TRKMODE_Tick, CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Direction     ", CS_M_TRKDIR_Init,  CS_M_TRKDIR_Tick,  CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Clock Divider ", CS_M_TRKDIV_Init,  CS_M_TRKDIV_Tick,  CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Track Length  ", CS_M_TRKLEN_Init,  CS_M_TRKLEN_Tick,  CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Track Transp. ", CS_M_TRKTRAN_Init, CS_M_TRKTRAN_Tick, CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Track Groove  ", CS_M_TRKGRV_Init,  CS_M_TRKGRV_Tick,  CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Random Gener. ", CS_M_TRKRND_Init,  CS_M_TRKRND_Tick,  CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Trigger Asgn. ", CS_M_TRKASG_Init,  CS_M_TRKASG_Tick,  CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Utilities     ", CS_M_UTILS_Init,   CS_M_UTILS_Tick,   CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Manual Trg.   ", CS_M_TRG_Init,     CS_M_TRG_Tick,     CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Record        ", CS_M_RECORD_Init,  CS_M_RECORD_Tick,  CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Morphing      ", CS_M_MORPH_Init,   CS_M_MORPH_Tick,   SEQ_DUMP_StoreGlobal, CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"BPM           ", CS_M_BPM_Init,     CS_M_BPM_Tick,     SEQ_DUMP_StoreGlobal, CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Metronome     ", CS_M_METRON_Init,  CS_M_METRON_Tick,  SEQ_DUMP_StoreGlobal, CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Save Pattern  ", CS_M_SAVE_Init,    CS_M_SAVE_Tick,    CS_M_NOP,          CS_M_SAVE_Select
	CS_MENU_ENTRY	"Format BankSt.", CS_M_FORMAT_Init,  CS_M_FORMAT_Tick,  CS_M_NOP,          CS_M_FORMAT_Select
	CS_MENU_ENTRY	"MBSEQ Options ", CS_M_OPT_Init,     CS_M_OPT_Tick,     SEQ_DUMP_StoreGlobal, CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"MIDI Config.  ", CS_M_MIDICFG_Init, CS_M_MIDICFG_Tick, SEQ_DUMP_StoreGlobal, CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Port Mutes    ", CS_M_PMUTE_Init,   CS_M_PMUTE_Tick,   CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Send SysEx    ", CS_M_SYSEX_Init,   CS_M_SYSEX_Tick,   SEQ_DUMP_StoreGlobal, CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Mixer         ", CS_M_MIXER_Init,   CS_M_MIXER_Tick,   CS_M_NOP,          CS_M_MIXER_Select
CS_MENU_TABLE_ROOT_End


;; following defines have to be kept consistent with the table above!
CS_MENU_PAGE_VIEW	EQU	0x00
CS_MENU_PAGE_MUTE	EQU	0x01
CS_MENU_PAGE_PATTERN	EQU	0x02
CS_MENU_PAGE_SONG	EQU	0x03
CS_MENU_PAGE_EVENTS	EQU	0x04
CS_MENU_PAGE_MODE	EQU	0x05
CS_MENU_PAGE_DIRECTION	EQU	0x06
CS_MENU_PAGE_DIVIDER	EQU	0x07
CS_MENU_PAGE_LENGTH	EQU	0x08
CS_MENU_PAGE_TRANSPOSE	EQU	0x09
CS_MENU_PAGE_GROOVE	EQU	0x0a
CS_MENU_PAGE_RANDOM	EQU	0x0b
CS_MENU_PAGE_ASSIGN	EQU	0x0c
CS_MENU_PAGE_UTILS	EQU	0x0d
CS_MENU_PAGE_MANUAL	EQU	0x0e
CS_MENU_PAGE_RECORD	EQU	0x0f
CS_MENU_PAGE_MORPH	EQU	0x10
CS_MENU_PAGE_BPM	EQU	0x11
CS_MENU_PAGE_METRONOME	EQU	0x12
CS_MENU_PAGE_SAVE	EQU	0x13
CS_MENU_PAGE_FORMAT	EQU	0x14
CS_MENU_PAGE_OPT	EQU	0x15
CS_MENU_PAGE_MIDI	EQU	0x16
CS_MENU_PAGE_PMUTE	EQU	0x17
CS_MENU_PAGE_SYSEX	EQU	0x18
CS_MENU_PAGE_MIXER	EQU	0x19


; ==========================================================================
;  the Menu Pos <-> General Purpose Buttons Map
; ==========================================================================
CS_MENU_TABLE_GPMAP
	db	CS_MENU_PAGE_MIXER, CS_MENU_PAGE_EVENTS
	db	CS_MENU_PAGE_MODE, CS_MENU_PAGE_DIRECTION
	db	CS_MENU_PAGE_DIVIDER, CS_MENU_PAGE_LENGTH
	db	CS_MENU_PAGE_TRANSPOSE, CS_MENU_PAGE_GROOVE
	db	CS_MENU_PAGE_ASSIGN, CS_MENU_PAGE_UTILS
	db	CS_MENU_PAGE_MANUAL, CS_MENU_PAGE_MORPH
	db	CS_MENU_PAGE_BPM, CS_MENU_PAGE_SAVE
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

