; $Id$
;
; Control Surface Menu Tables
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
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
#if DEFAULT_PATCHMODE == 0
	CS_MENU_HEADER	CS_MENU_TABLE_ROOT_ENTRIES, MBMF_LCD_PrintBankName, MBMF_BUTTON_Snapshot, MBMF_LED_Update, MBMF_LED_SetMapped, MBMF_MSG_Handler
#else
	CS_MENU_HEADER	CS_MENU_TABLE_ROOT_ENTRIES, MBMF_LCD_PrintPatchName, MBMF_BUTTON_Snapshot, MBMF_LED_Update, MBMF_LED_SetMapped, MBMF_MSG_Handler
#endif

CS_MENU_TABLE_ROOT_Begin
	CS_MENU_ENTRY	"Display       ", CS_M_DISPLAY_Init, CS_M_DISPLAY_Tick, CS_M_DISPLAY_Exit, CS_M_DISPLAY_Snapshot
	CS_MENU_ENTRY	"Select Bank   ", CS_M_BANK_Init,    CS_M_BANK_Tick,    CS_M_BANK_Exit,    CS_M_BANK_Snapshot
	CS_MENU_ENTRY	"Select Patch  ", CS_M_PATCH_Init,   CS_M_PATCH_Tick,   CS_M_PATCH_Exit,   CS_M_PATCH_Snapshot
	CS_MENU_ENTRY	"Global Chn.   ", CS_M_CHANNEL_Init, CS_M_CHANNEL_Tick, CS_M_CHANNEL_Exit, CS_M_CHANNEL_Snapshot
	CS_MENU_ENTRY	"MIDI Learn    ", CS_M_LEARN_Init,   CS_M_LEARN_Tick,   CS_M_LEARN_Exit,   CS_M_LEARN_Snapshot
	CS_MENU_ENTRY	"Morphing Fun  ", CS_M_MORPH_Init,   CS_M_MORPH_Tick,   CS_M_MORPH_Exit,   CS_M_MORPH_Snapshot
	CS_MENU_ENTRY	"Touch Sensor  ", CS_M_TS_Init,      CS_M_TS_Tick,      CS_M_TS_Exit,      CS_M_TS_Snapshot
	CS_MENU_ENTRY	"TS Mode       ", CS_M_TSMODE_Init,  CS_M_TSMODE_Tick,  CS_M_TSMODE_Exit,  CS_M_TSMODE_Snapshot
	CS_MENU_ENTRY	"Calibration   ", CS_M_CALI_Init,    CS_M_CALI_Tick,    CS_M_CALI_Exit,    CS_M_CALI_Snapshot
#if DEFAULT_PATCHMODE == 0
	CS_MENU_ENTRY	"Edit Bankname ", CS_M_BNAME_Init,   CS_M_BNAME_Tick,   CS_M_BNAME_Exit,   CS_M_BNAME_Snapshot
#else
	CS_MENU_ENTRY	"Edit Patchname", CS_M_BNAME_Init,   CS_M_BNAME_Tick,   CS_M_BNAME_Exit,   CS_M_BNAME_Snapshot
#endif
	CS_MENU_ENTRY	"Edit Events   ", CS_M_EVENT_Init,   CS_M_EVENT_Tick,   CS_M_EVENT_Exit,   CS_M_EVENT_Snapshot
	CS_MENU_ENTRY	"Edit Labels   ", CS_M_LABEL_Init,   CS_M_LABEL_Tick,   CS_M_LABEL_Exit,   CS_M_LABEL_Snapshot
#if DEFAULT_PATCHMODE == 0
	CS_MENU_ENTRY	"Copy Bank     ", CS_M_COPY_Init,    CS_M_COPY_Tick,    CS_M_COPY_Exit,    CS_M_COPY_Snapshot
#else
	CS_MENU_ENTRY	"Copy Patch    ", CS_M_COPY_Init,    CS_M_COPY_Tick,    CS_M_COPY_Exit,    CS_M_COPY_Snapshot
#endif
	CS_MENU_ENTRY	"MIDI Config.  ", CS_M_MIDICFG_Init, CS_M_MIDICFG_Tick, CS_M_MIDICFG_Exit, CS_M_MIDICFG_Snapshot
	CS_MENU_ENTRY	"Send SysEx    ", CS_M_SYSEX_Init,   CS_M_SYSEX_Tick,   CS_M_SYSEX_Exit,   CS_M_SYSEX_Snapshot
CS_MENU_TABLE_ROOT_End
CS_MENU_TABLE_ROOT_ENTRIES EQU (CS_MENU_TABLE_ROOT_End-CS_MENU_TABLE_ROOT_Begin)/CS_MENU_ENTRY_LEN
