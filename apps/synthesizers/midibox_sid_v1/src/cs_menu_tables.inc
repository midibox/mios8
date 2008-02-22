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
;  CS_MENU_TABLES contains the number of tables and a pointer to all
;  available menu tables and the appr. table handler function
; ==========================================================================
CS_MENU_TABLES
	;;      pointer to table    par. button pressed      exec button pressed
	dw	CS_MENU_TABLE_ROOT, CS_MENU_Page_Root,       CS_MENU_EXEC_GoToMain
	dw	CS_MENU_TABLE_OSC,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_FIL,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_LFO,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_ENV,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_MOD,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_WT,   CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRootWT
	dw	CS_MENU_TABLE_303,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRootWT
	dw	CS_MENU_TABLE_VMA,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_SEO,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_CFG,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_SAV,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot

	;; ID's for all menu structures
CS_MENU_ROOT	EQU	0x00
CS_MENU_OSC	EQU	0x01
CS_MENU_FIL	EQU	0x02
CS_MENU_LFO	EQU	0x03
CS_MENU_ENV	EQU	0x04
CS_MENU_MOD	EQU	0x05
CS_MENU_WT	EQU	0x06
CS_MENU_303	EQU	0x07
CS_MENU_VMA	EQU	0x08
CS_MENU_SEO	EQU	0x09
CS_MENU_CFG	EQU	0x0a
CS_MENU_SAV	EQU	0x0b

	;; ID for main page (7th bit must be set)
CS_MENU_MAIN	EQU	0x80
	

; ==========================================================================
;  The main menu
; ==========================================================================
CS_MENU_TABLE_ROOT
	db	(CS_MENU_TABLE_ROOT_End-CS_MENU_TABLE_ROOT)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<->|   max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_OSC,               "OSC",  0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_FIL,               "FIL",  0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_LFO,               "LFO",  0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_ENV,               "ENV",  0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_MOD,               "MOD",  0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_WT,                "WT ",  0x00, PRINT_NOP,        EXEC_MENU_WT,    R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_VMA,               "VMA",  0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_303,               "303",  0x00, PRINT_NOP,        EXEC_MENU_303,   R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_SEO,               "SEO",  0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_CFG,               "CFG",  0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_SAV,               "SAV",  0x00, PRINT_NOP,        EXEC_MENU_SAVE,  R2PP2R_NOP
CS_MENU_TABLE_ROOT_End

; ==========================================================================
;  The oscillator menu
; ==========================================================================
CS_MENU_TABLE_OSC
	db	(CS_MENU_TABLE_OSC_End-CS_MENU_TABLE_OSC)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_OSC_FLAGS, "OSC", 0x06, PRINT_VAR_123,    EXEC_SELPAR,     R2PP2R_VAR_OSC
	CS_MENU_ENTRY	CS_SID_VOICEx_WAVEFORM,     "Wav", 0x08, PRINT_Vx_WAV,     EXEC_SELPAR,     R2PP2R_Vx_WAV
	CS_MENU_ENTRY	CS_SID_VOICEx_WAVEFORM,     "S/R", 0x03, PRINT_Vx_S_R,     EXEC_SELPAR,     R2PP2R_Vx_S_R
	CS_MENU_ENTRY	CS_SID_VOICEx_DELAY,        "Del", 0x7f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	CS_SID_VOICEx_ATTACK,       "Atk", 0x7f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	CS_SID_VOICEx_DECAY,        "Dec", 0x7f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	CS_SID_VOICEx_SUSTAIN,      "Sus", 0x7f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	CS_SID_VOICEx_RELEASE,      "Rel", 0x7f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	CS_SID_VOICEx_PITCHRANGE,   "PRn", 0x7f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	CS_SID_VOICEx_TRANSPOSE,    "Trn", 0x7f, PRINT_Vx_PMDEC,   EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	CS_SID_VOICEx_FINETUNE,     "Fin", 0x7f, PRINT_Vx_PMDEC,   EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	CS_SID_VOICEx_PORTAMENTO,   "Por", 0x7f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	CS_SID_VOICEx_ARPEGGIATOR,  "Arp", 0x7f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	CS_SID_VOICEx_PULSEWIDTH,   "PW ", 0x7f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	CS_SID_OSC_PHASE_SYNC,	    "OPS", 0x07, PRINT_Vx_OPS,     EXEC_SELPAR,     R2PP2R_Vx_OPS
CS_MENU_TABLE_OSC_End

; ==========================================================================
;  The filter menu
; ==========================================================================
CS_MENU_TABLE_FIL
	db	(CS_MENU_TABLE_FIL_End-CS_MENU_TABLE_FIL)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_SID_FILTER_CHANNELS,     "Chn", 0x07, PRINT_FILTER_CHN, EXEC_SELPAR,     R2PP2R_FILTER_CHN
	CS_MENU_ENTRY	CS_SID_FILTER_CUTOFF,       "Cut", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_FILTER_RESONANCE,    "Res", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_SUSKEY,              "KTr", 0x3f, PRINT_KTR,        EXEC_SELPAR,     R2PP2R_KTR
	CS_MENU_ENTRY	CS_SID_FILTER_MODE,         "Mod", 0x07, PRINT_FILTER_MOD, EXEC_SELPAR,     R2PP2R_FILTER_MOD
	CS_MENU_ENTRY	CS_SID_FILTER_CHANNELS,     "Ext", 0x01, PRINT_FILTER_EXT, EXEC_TOGPAR,     R2PP2R_FILTER_EXT
	CS_MENU_ENTRY	CS_SID_FILTER_MODE,         "3Of", 0x01, PRINT_FILTER_3OF, EXEC_TOGPAR,     R2PP2R_FILTER_3OF
CS_MENU_TABLE_FIL_End

; ==========================================================================
;  The LFO menu
; ==========================================================================
CS_MENU_TABLE_LFO
	db	(CS_MENU_TABLE_LFO_End-CS_MENU_TABLE_LFO)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_LFO,       "LFO", 0x05, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
#if ENABLE_AIN_LFO_WAVEFORM
	CS_MENU_ENTRY	CS_SID_LFOx_MODE,           "Wav", 0x06, PRINT_LFOx_WAV,   EXEC_SELPAR,     R2PP2R_LFOx_WAV
#else
	CS_MENU_ENTRY	CS_SID_LFOx_MODE,           "Wav", 0x05, PRINT_LFOx_WAV,   EXEC_SELPAR,     R2PP2R_LFOx_WAV
#endif
	CS_MENU_ENTRY	CS_SID_LFOx_RATE,           "Rte", 0x7f, PRINT_LFOx_DEC,   EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	CS_SID_LFOx_DEPTH,          "Dep", 0x7f, PRINT_LFOx_PMDEC, EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	CS_SID_LFOx_MODE,           "Syn", 0x02, PRINT_LFOx_SYN,   EXEC_SELPAR,     R2PP2R_LFOx_SYN
CS_MENU_TABLE_LFO_End

; ==========================================================================
;  The envelope menu
; ==========================================================================
CS_MENU_TABLE_ENV
	db	(CS_MENU_TABLE_ENV_End-CS_MENU_TABLE_ENV)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_ENV,       "ENV", 0x01, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	CS_SID_ENVx_DEPTH,          "Dep", 0x7f, PRINT_ENVx_PMDEC, EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	CS_SID_ENVx_ATTACK,         "Atk", 0x7f, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	CS_SID_ENVx_DECAY,          "Dec", 0x7f, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	CS_SID_ENVx_SUSTAIN,        "Sus", 0x7f, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	CS_SID_ENVx_RELEASE,        "Rel", 0x7f, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	CS_SID_ENV_1_CURVE,         "Cur", 0x7f, PRINT_ENVx_CUR,   EXEC_SELPAR,     R2PP2R_ENVx_CUR
	CS_MENU_ENTRY	CS_SID_ENV_CURVES,          "CAs", 0x07, PRINT_ENVx_CAS,   EXEC_SELPAR,     R2PP2R_ENVx_CAS
CS_MENU_TABLE_ENV_End

; ==========================================================================
;  The modulation matrix menu
; ==========================================================================
CS_MENU_TABLE_MOD
	db	(CS_MENU_TABLE_MOD_End-CS_MENU_TABLE_MOD)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_MOD,       "Mod", 0x06, PRINT_VAR_MOD,    EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	0x00,                       "E1 ", 0x01, PRINT_VAR_MOD_N,  EXEC_TOGPAR,     R2PP2R_MOD_N
	CS_MENU_ENTRY	0x01,                       "E2 ", 0x01, PRINT_VAR_MOD_N,  EXEC_TOGPAR,     R2PP2R_MOD_N
	CS_MENU_ENTRY	0x02,                       "L1 ", 0x01, PRINT_VAR_MOD_N,  EXEC_TOGPAR,     R2PP2R_MOD_N
	CS_MENU_ENTRY	0x03,                       "L2 ", 0x01, PRINT_VAR_MOD_N,  EXEC_TOGPAR,     R2PP2R_MOD_N
	CS_MENU_ENTRY	0x04,                       "L3 ", 0x01, PRINT_VAR_MOD_N,  EXEC_TOGPAR,     R2PP2R_MOD_N
	CS_MENU_ENTRY	0x05,                       "L4 ", 0x01, PRINT_VAR_MOD_N,  EXEC_TOGPAR,     R2PP2R_MOD_N
	CS_MENU_ENTRY	0x06,                       "L5 ", 0x01, PRINT_VAR_MOD_N,  EXEC_TOGPAR,     R2PP2R_MOD_N
	CS_MENU_ENTRY	0x07,                       "L6 ", 0x01, PRINT_VAR_MOD_N,  EXEC_TOGPAR,     R2PP2R_MOD_N
CS_MENU_TABLE_MOD_End

; ==========================================================================
;  The wavetable menu
; ==========================================================================
CS_MENU_TABLE_WT
	db	(CS_MENU_TABLE_WT_End-CS_MENU_TABLE_WT)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_WT_POS,             "Pos", 0x1f+1, PRINT_VAR_WTPOS,EXEC_SELPAR,     R2PP2R_VAR_WTPOS
	CS_MENU_ENTRY	CS_SID_WT_MODE,             "Mod", 0x03, PRINT_VAR_WTMODE, EXEC_SELPAR,     R2PP2R_VAR_WTPAR
	CS_MENU_ENTRY	CS_SID_WT_P1,               "#1 ", 0xff, PRINT_VAR_WTPAR,  EXEC_SELPAR,     R2PP2R_VAR_WTPAR
	CS_MENU_ENTRY	CS_SID_WT_P2,               "#2 ", 0xff, PRINT_VAR_WTPAR,  EXEC_SELPAR,     R2PP2R_VAR_WTPAR
	CS_MENU_ENTRY	CS_SID_WT_P3,               "#3 ", 0xff, PRINT_VAR_WTPAR,  EXEC_SELPAR,     R2PP2R_VAR_WTPAR
	CS_MENU_ENTRY	CS_SID_WAVETABLE_RATE,      "Rte", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_WAVETABLE_P1_ASSIGN, "CC1", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_WTASG
	CS_MENU_ENTRY	CS_SID_WAVETABLE_P2_ASSIGN, "CC2", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_WTASG
	CS_MENU_ENTRY	CS_SID_WAVETABLE_P3_ASSIGN, "CC3", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_WTASG
	CS_MENU_ENTRY	CS_SID_WT_PATCH,            "P# ", 0x7f, PRINT_DECP1,      EXEC_SELPAR,     R2PP2R_CC
CS_MENU_TABLE_WT_End

; ==========================================================================
;  The velocity/modulation/aftertouch menu
; ==========================================================================
CS_MENU_TABLE_VMA
	db	(CS_MENU_TABLE_VMA_End-CS_MENU_TABLE_VMA)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_SID_VELOCITY_ASSIGN,     "VCC", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_VELOCITY_INIT,       "VIn", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_VELOCITY_DEPTH,      "VDp", 0x7f, PRINT_PMDEC,      EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_MODULATION_ASSIGN,   "MCC", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_MODULATION_INIT,     "MIn", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_MODULATION_DEPTH,    "MDp", 0x7f, PRINT_PMDEC,      EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_AFTERTOUCH_ASSIGN,   "ACC", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_AFTERTOUCH_INIT,     "AIn", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_AFTERTOUCH_DEPTH,    "ADp", 0x7f, PRINT_PMDEC,      EXEC_SELPAR,     R2PP2R_CC
CS_MENU_TABLE_VMA_End

; ==========================================================================
;  The 303 menu
; ==========================================================================
CS_MENU_TABLE_303
	db	(CS_MENU_TABLE_303_End-CS_MENU_TABLE_303)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_WT_POS,             "Pos", 0x1f+1, PRINT_VAR_WTPOS,EXEC_SELPAR,     R2PP2R_VAR_WTPOS
	CS_MENU_ENTRY	CS_SID_WT_MODE,             "Mod", 0x03, PRINT_VAR_WTMODE, EXEC_SELPAR,     R2PP2R_VAR_WTPAR
	CS_MENU_ENTRY	CS_SID_WT_P1,               "S/G", 0x03, PRINT_VAR_303SG,  EXEC_SELPAR,     R2PP2R_VAR_303SG
	CS_MENU_ENTRY	CS_SID_WT_P1,               "Acc", 0x0f, PRINT_VAR_303ACC, EXEC_SELPAR,     R2PP2R_VAR_303ACC
	CS_MENU_ENTRY	CS_SID_WT_P2,               "Nte", 0x7f, PRINT_VAR_WTPAR,  EXEC_SELPAR,     R2PP2R_VAR_303NOTE
	CS_MENU_ENTRY	CS_SID_WT_P3,               "#3 ", 0xff, PRINT_VAR_WTPAR,  EXEC_SELPAR,     R2PP2R_VAR_WTPAR
	CS_MENU_ENTRY	CS_SID_WAVETABLE_RATE,      "Rte", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_WT_PATCH,            "P# ", 0x7f, PRINT_DECP1,      EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_FILTER_CUTOFF,       "Cut", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_FILTER_RESONANCE,    "Res", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_ENVx_DEPTH,          "Mod", 0x7f, PRINT_303_MOD,    EXEC_SELPAR,     R2PP2R_303_MOD
	CS_MENU_ENTRY	CS_SID_ENVx_DECAY,          "Dec", 0x7f, PRINT_303_DEC,    EXEC_SELPAR,     R2PP2R_303_DEC
CS_MENU_TABLE_303_End

; ==========================================================================
;  The Sound Engine Option menu
; ==========================================================================
CS_MENU_TABLE_SEO
	db	(CS_MENU_TABLE_SEO_End-CS_MENU_TABLE_SEO)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_SID_SE_OPTION,           "303", 0x1,  PRINT_SEO_303,    EXEC_TOGPAR,     R2PP2R_SEO_303
	CS_MENU_ENTRY	CS_SID_SE_OPTION,           "FIP", 0x1,  PRINT_SEO_FIP,    EXEC_TOGPAR,     R2PP2R_SEO_FIP
	CS_MENU_ENTRY	CS_SID_SE_OPTION,           "E2P", 0x1,  PRINT_SEO_E2P,    EXEC_TOGPAR,     R2PP2R_SEO_E2P
	CS_MENU_ENTRY	CS_SID_SE_OPTION,           "E2V", 0x1,  PRINT_SEO_E2V,    EXEC_TOGPAR,     R2PP2R_SEO_E2V
	CS_MENU_ENTRY	CS_SID_SE_OPTION,           "GSA", 0x1,  PRINT_SEO_GSA,    EXEC_TOGPAR,     R2PP2R_SEO_GSA
CS_MENU_TABLE_SEO_End

; ==========================================================================
;  The configuration menu
; ==========================================================================
CS_MENU_TABLE_CFG
	db	(CS_MENU_TABLE_CFG_End-CS_MENU_TABLE_CFG)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	0x00,                       "Chn", 0x0f, PRINT_CHN,        EXEC_SELPAR,     R2PP2R_CHN
	CS_MENU_ENTRY	0x00,                       "Dev", 0x0f, PRINT_DEV,        EXEC_SELPAR,     R2PP2R_DEV
	CS_MENU_ENTRY	0x00,                       "Bnk", 0x07, PRINT_BNK,        EXEC_SELPAR,     R2PP2R_BNK
	CS_MENU_ENTRY	0x00,                       "Pat", 0x7f, PRINT_PAT,        EXEC_SELPAR,     R2PP2R_PAT
	CS_MENU_ENTRY	CS_SID_NAME_x,              "Nam", 0x7f, PRINT_NAME,       EXEC_EDITNAME,   R2PP2R_NAME
	CS_MENU_ENTRY	CS_SID_VOLUME,              "Vol", 0x7f, PRINT_DEC,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_POLY,                "Ply", 0x04, PRINT_PLY,        EXEC_SELPAR,     R2PP2R_PLY
	CS_MENU_ENTRY	CS_SID_SUSKEY,              "SuK", 0x01, PRINT_SUK,        EXEC_TOGPAR,     R2PP2R_SUK
	CS_MENU_ENTRY	CS_SID_MIDI_SYNC,           "Clk", 0x07, PRINT_CLK,        EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_VOICE1_SPLIT_L,      "S1L", 0x7f, PRINT_NOTE,       EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_VOICE1_SPLIT_U,      "S1U", 0x7f, PRINT_NOTE,       EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_VOICE2_SPLIT_L,      "S2L", 0x7f, PRINT_NOTE,       EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_VOICE2_SPLIT_U,      "S2U", 0x7f, PRINT_NOTE,       EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_VOICE3_SPLIT_L,      "S3L", 0x7f, PRINT_NOTE,       EXEC_SELPAR,     R2PP2R_CC
	CS_MENU_ENTRY	CS_SID_VOICE3_SPLIT_U,      "S3U", 0x7f, PRINT_NOTE,       EXEC_SELPAR,     R2PP2R_CC
CS_MENU_TABLE_CFG_End

; ==========================================================================
;  The save menu
; ==========================================================================
CS_MENU_TABLE_SAV
	db	(CS_MENU_TABLE_SAV_End-CS_MENU_TABLE_SAV)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SAVE_BANK,          "B# ", 0x07, PRINT_SAVE_BNK,   EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	CS_MENU_SAVE_PATCH,         "P# ", 0x7f, PRINT_SAVE_PAT,   EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	0x00,                       "do!", 0x00, PRINT_SAVE_NAME,  EXEC_SAVE_IT,    R2PP2R_NOP
CS_MENU_TABLE_SAV_End
