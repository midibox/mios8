; $Id$
;
; Control Surface Menu Tables
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
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
	CS_MENU_HEADER	(CS_MENU_TABLE_ROOT_End-CS_MENU_TABLE_ROOT_Begin)/CS_MENU_ENTRY_LEN, CV_LCD_PrintHeader, CS_M_NOP, CS_M_NOP, CV_MSG_Handler
CS_MENU_TABLE_ROOT_Begin
	CS_MENU_ENTRY	"Patch         ", CS_M_DISPLAY_Init, CS_M_DISPLAY_Tick, CS_M_DISPLAY_Exit, CS_M_DISPLAY_Select
	CS_MENU_ENTRY	"MIDI Events   ", CS_M_EVENT_Init,   CS_M_EVENT_Tick  , CS_M_NOP,          CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Transpose     ", CS_M_TRANSP_Init,  CS_M_TRANSP_Tick,  CS_M_NOP,	  CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Keyboard Zone ", CS_M_SPLIT_Init,   CS_M_SPLIT_Tick,   CS_M_NOP,	  CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Pitchrange    ", CS_M_PITCHR_Init,  CS_M_PITCHR_Tick,  CS_M_NOP,	  CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Note Handling ", CS_M_NOTEH_Init,   CS_M_NOTEH_Tick,   CS_M_NOP,	  CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"CC/NRPN Number", CS_M_CCNUM_Init,   CS_M_CCNUM_Tick,   CS_M_NOP,	  CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Gate Polarity ", CS_M_GATEP_Init,   CS_M_GATEP_Tick,   CS_M_NOP,	  CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Output Curve  ", CS_M_CURVE_Init,   CS_M_CURVE_Tick,   CS_M_NOP,	  CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Interpolation ", CS_M_IP_Init,      CS_M_IP_Tick,      CS_M_NOP,	  CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Clock Output  ", CS_M_CLK_Init,     CS_M_CLK_Tick,     CS_M_NOP,	  CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Calibration   ", CS_M_CALIB_Init,   CS_M_CALIB_Tick,   CS_M_CALIB_Exit,   CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"MIDI Config.  ", CS_M_MIDICFG_Init, CS_M_MIDICFG_Tick, CS_M_NOP,	  CS_MENU_IncCursorPos
	CS_MENU_ENTRY	"Send SysEx    ", CS_M_SYSEX_Init,   CS_M_SYSEX_Tick,   CS_M_NOP,	  CS_M_SYSEX_Select
CS_MENU_TABLE_ROOT_End



; ==========================================================================
;  dummy function for "no operation"
; ==========================================================================
CS_M_NOP
	return

