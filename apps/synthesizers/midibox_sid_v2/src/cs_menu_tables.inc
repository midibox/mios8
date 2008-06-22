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
#include "src/cs_menu_tables_struct.inc"


; ==========================================================================
;  References to various CS_MENU_x_xxx IDs depending on selected engine
; ==========================================================================
	;; on order changes, the CS_MENU_GetMenuID_xxx functions have to be adapted!
CS_MENU_TABLES_IDS
	;;      lead           bassline       drum           multi
	db	CS_MENU_L_OSC, CS_MENU_B_OSC, CS_MENU_D_DRM, CS_MENU_M_OSC
	db	CS_MENU_L_FIL, CS_MENU_B_FIL, CS_MENU_D_FIL, CS_MENU_M_FIL
	db	CS_MENU_L_LFO, CS_MENU_B_LFO, CS_MENU_ROOT,  CS_MENU_M_LFO
	db	CS_MENU_L_ENV, CS_MENU_B_ENV, CS_MENU_ROOT,  CS_MENU_M_ENV
	db	CS_MENU_L_MOD, CS_MENU_ROOT,  CS_MENU_ROOT,  CS_MENU_ROOT
	db	CS_MENU_L_KNB, CS_MENU_B_KNB, CS_MENU_D_KNB, CS_MENU_M_KNB
	db	CS_MENU_L_SAV, CS_MENU_B_SAV, CS_MENU_D_SAV, CS_MENU_M_SAV

; ==========================================================================
;  CS_MENU_TABLES contains the number of tables and a pointer to all
;  available menu tables and the appr. table handler function
; ==========================================================================
CS_MENU_TABLES_ENGINE
	dw	CS_MENU_TABLES_L & 0xffff	; lead engine
	dw	CS_MENU_TABLES_B & 0xffff	; bassline engine
	dw	CS_MENU_TABLES_D & 0xffff	; drum engine
	dw	CS_MENU_TABLES_M & 0xffff	; multi engine
	dw	CS_MENU_TABLES_E & 0xffff	; ensembles menu (selected if CS_MENU[6] set)
 
; ==========================================================================
; --------------------------------------------------------------------------
CS_MENU_TABLES_E	; the environment table (works independent from the synth menu)
	;;              pointer to table      par. button pressed      exec button pressed   page name          var            max  print function
	CS_MENU_T_ENTRY	CS_MENU_TABLE_E_ROOT, CS_MENU_Page_Root,       CS_MENU_EXEC_GoToMain,   "---", SID_ENSEMBLE,          0x7f, PRINT_ENS  ; (special handling in CS_MENU_PageUpDown selected with SID_ENSEMBLE)
	CS_MENU_T_ENTRY CS_MENU_TABLE_E_SID,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToEnsRoot,"---", 0,                     0x00, PRINT_NOP
	CS_MENU_T_ENTRY CS_MENU_TABLE_E_CLK,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToEnsRoot,"---", 0,                     0x00, PRINT_NOP
	CS_MENU_T_ENTRY CS_MENU_TABLE_E_FIL,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToEnsRoot,"---", 0,                     0x00, PRINT_NOP
	CS_MENU_T_ENTRY CS_MENU_TABLE_E_INS,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToEnsRoot,"INS", CS_MENU_SELECTED_INS,  0x05, PRINT_VAR_DECP1
	CS_MENU_T_ENTRY CS_MENU_TABLE_E_EXT,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToEnsRoot,"---", 0,                     0x00, PRINT_NOP
	CS_MENU_T_ENTRY CS_MENU_TABLE_E_SAV,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToEnsRoot,"---", 0,                     0x00, PRINT_NOP

	;; ID's for seperate menu structure of ensemble menu
CS_MENU_E_ROOT	EQU	0x40
CS_MENU_E_SID	EQU	0x41
CS_MENU_E_CLK	EQU	0x42
CS_MENU_E_FIL	EQU	0x43
CS_MENU_E_INS	EQU	0x44
CS_MENU_E_EXT	EQU	0x45
CS_MENU_E_SAV	EQU	0x46

; ==========================================================================
; --------------------------------------------------------------------------
CS_MENU_TABLES_L
	;;              pointer to table      par. button pressed      exec button pressed   page name          var            max  print function
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_ROOT, CS_MENU_Page_Root,       CS_MENU_EXEC_GoToMain,   "---", SID_PATCH,             0x7f, PRINT_PAT  ; (special handling in CS_MENU_PageUpDown selected with SID_PATCH)
	CS_MENU_T_ENTRY CS_MENU_TABLE_L_OSC,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "OSC", CS_MENU_SELECTED_OSC,  0x02, PRINT_VAR_123
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_FIL,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ACh", CS_MENU_SELECTED_SID_LR, 0x03, PRINT_VAR_LR
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_LFO,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "LFO", CS_MENU_SELECTED_LFO,  0x05, PRINT_VAR_DECP1
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_ENV,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ENV", CS_MENU_SELECTED_ENV,  0x01, PRINT_VAR_DECP1
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_MOD,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "MOD", CS_MENU_SELECTED_MOD,  0x07, PRINT_VAR_DECP1
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_TRG,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "TRG", CS_MENU_SELECTED_TRG,  0x0d, PRINT_TRGx_SRC
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_CFG,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ACh", CS_MENU_SELECTED_SID_LR, 0x03, PRINT_VAR_LR
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_ARP,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "OSC", CS_MENU_SELECTED_OSC,  0x02, PRINT_VAR_123
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_WTC,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "WT ", CS_MENU_SELECTED_WT,   0x03, PRINT_VAR_DECP1
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_WTE,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "Pos", CS_MENU_WT_POS,        0x80, PRINT_WTE_POS ; 
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_KNB,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "KNB", CS_MENU_SELECTED_KNOB, 0x07, PRINT_KNB
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_EXT,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "---", 0,                     0x00, PRINT_NOP
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_RND,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "---", 0,                     0x00, PRINT_NOP
	CS_MENU_T_ENTRY	CS_MENU_TABLE_L_SAV,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "---", 0,                     0x00, PRINT_NOP

	;; ID's for menu structures of the lead engine
CS_MENU_L_OSC	EQU	0x01
CS_MENU_L_FIL	EQU	0x02
CS_MENU_L_LFO	EQU	0x03
CS_MENU_L_ENV	EQU	0x04
CS_MENU_L_MOD	EQU	0x05
CS_MENU_L_TRG	EQU	0x06
CS_MENU_L_CFG	EQU	0x07
CS_MENU_L_ARP	EQU	0x08
CS_MENU_L_WTC	EQU	0x09
CS_MENU_L_WTE	EQU	0x0a
CS_MENU_L_KNB	EQU	0x0b
CS_MENU_L_EXT	EQU	0x0c
CS_MENU_L_RND	EQU	0x0d
CS_MENU_L_SAV	EQU	0x0e


; --------------------------------------------------------------------------
CS_MENU_TABLES_B
	;;              pointer to table      par. button pressed      exec button pressed   page name          var            max  print function
	CS_MENU_T_ENTRY	CS_MENU_TABLE_B_ROOT, CS_MENU_Page_Root,       CS_MENU_EXEC_GoToMain,   "---", SID_PATCH,               0x7f, PRINT_PAT  ; (special handling in CS_MENU_PageUpDown selected with SID_PATCH)
	CS_MENU_T_ENTRY	CS_MENU_TABLE_B_SEQ,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "Pos", CS_MENU_WT_POS,	        0x0f, PRINT_SEQ_POS
	CS_MENU_T_ENTRY	CS_MENU_TABLE_B_SQC,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ACh", CS_MENU_SELECTED_SID_LR, 0x03, PRINT_VAR_LR
	CS_MENU_T_ENTRY CS_MENU_TABLE_B_OSC,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ACh", CS_MENU_SELECTED_SID_LR, 0x03, PRINT_VAR_LR
	CS_MENU_T_ENTRY CS_MENU_TABLE_B_O23,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ACh", CS_MENU_SELECTED_SID_LR, 0x03, PRINT_VAR_LR
	CS_MENU_T_ENTRY	CS_MENU_TABLE_B_FIL,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ACh", CS_MENU_SELECTED_SID_LR, 0x03, PRINT_VAR_LR
	CS_MENU_T_ENTRY	CS_MENU_TABLE_B_LFO,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ACh", CS_MENU_SELECTED_SID_LR, 0x03, PRINT_VAR_LR
	CS_MENU_T_ENTRY	CS_MENU_TABLE_B_ENV,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ACh", CS_MENU_SELECTED_SID_LR, 0x03, PRINT_VAR_LR
	CS_MENU_T_ENTRY	CS_MENU_TABLE_B_ARP,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ACh", CS_MENU_SELECTED_SID_LR, 0x03, PRINT_VAR_LR
	CS_MENU_T_ENTRY	CS_MENU_TABLE_B_CFG,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ACh", CS_MENU_SELECTED_SID_LR, 0x03, PRINT_VAR_LR
	CS_MENU_T_ENTRY	CS_MENU_TABLE_B_KNB,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "KNB", CS_MENU_SELECTED_KNOB, 0x07, PRINT_KNB
	CS_MENU_T_ENTRY	CS_MENU_TABLE_B_EXT,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "---", 0,                     0x00, PRINT_NOP
	CS_MENU_T_ENTRY	CS_MENU_TABLE_B_RND,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "---", 0,                     0x00, PRINT_NOP
	CS_MENU_T_ENTRY	CS_MENU_TABLE_B_SAV,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "---", 0,                     0x00, PRINT_NOP

	;; ID's for menu structures of the bassline engine
CS_MENU_B_SEQ	EQU	0x01
CS_MENU_B_SQC	EQU	0x02
CS_MENU_B_OSC	EQU	0x03
CS_MENU_B_O23	EQU	0x04
CS_MENU_B_FIL	EQU	0x05
CS_MENU_B_LFO	EQU	0x06
CS_MENU_B_ENV	EQU	0x07
CS_MENU_B_ARP	EQU	0x08
CS_MENU_B_CFG	EQU	0x09
CS_MENU_B_KNB	EQU	0x0a
CS_MENU_B_EXT	EQU	0x0b
CS_MENU_B_RND	EQU	0x0c
CS_MENU_B_SAV	EQU	0x0d


; --------------------------------------------------------------------------
CS_MENU_TABLES_D
	;;              pointer to table      par. button pressed      exec button pressed   page name          var            max  print function
	CS_MENU_T_ENTRY	CS_MENU_TABLE_D_ROOT, CS_MENU_Page_Root,       CS_MENU_EXEC_GoToMain,   "---", SID_PATCH,             0x7f, PRINT_PAT  ; (special handling in CS_MENU_PageUpDown selected with SID_PATCH)
	CS_MENU_T_ENTRY CS_MENU_TABLE_D_SEQ,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "Trk", CS_MENU_SELECTED_TRK,  0x07, PRINT_VAR_DECP1
	CS_MENU_T_ENTRY	CS_MENU_TABLE_D_SQC,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "---", 0,                     0x00, PRINT_NOP
	CS_MENU_T_ENTRY CS_MENU_TABLE_D_DRM,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "Ins", CS_MENU_SELECTED_INS,  0x0f, PRINT_VAR_DECP1
	CS_MENU_T_ENTRY	CS_MENU_TABLE_D_FIL,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ACh", CS_MENU_SELECTED_SID_LR, 0x03, PRINT_VAR_LR
	CS_MENU_T_ENTRY	CS_MENU_TABLE_D_CFG,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ACh", CS_MENU_SELECTED_SID_LR, 0x03, PRINT_VAR_LR
	CS_MENU_T_ENTRY	CS_MENU_TABLE_D_KNB,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "KNB", CS_MENU_SELECTED_KNOB, 0x07, PRINT_KNB
	CS_MENU_T_ENTRY	CS_MENU_TABLE_D_EXT,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "---", 0,                     0x00, PRINT_NOP
	CS_MENU_T_ENTRY	CS_MENU_TABLE_D_RND,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "---", 0,                     0x00, PRINT_NOP
	CS_MENU_T_ENTRY	CS_MENU_TABLE_D_SAV,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "---", 0,                     0x00, PRINT_NOP

	;; ID's for menu structures of the drum engine
CS_MENU_D_SEQ	EQU	0x01
CS_MENU_D_SQC	EQU	0x02
CS_MENU_D_DRM	EQU	0x03
CS_MENU_D_FIL	EQU	0x04
CS_MENU_D_CFG	EQU	0x05
CS_MENU_D_KNB	EQU	0x06
CS_MENU_D_EXT	EQU	0x07
CS_MENU_D_RND	EQU	0x08
CS_MENU_D_SAV	EQU	0x09


; --------------------------------------------------------------------------
CS_MENU_TABLES_M
	;;              pointer to table      par. button pressed      exec button pressed   page name          var            max  print function
	CS_MENU_T_ENTRY	CS_MENU_TABLE_M_ROOT, CS_MENU_Page_Root,       CS_MENU_EXEC_GoToMain,   "---", SID_PATCH,             0x7f, PRINT_PAT  ; (special handling in CS_MENU_PageUpDown selected with SID_PATCH)
	CS_MENU_T_ENTRY CS_MENU_TABLE_M_OSC,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "Ins", CS_MENU_SELECTED_INS,  0x05, PRINT_VAR_DECP1
	CS_MENU_T_ENTRY	CS_MENU_TABLE_M_FIL,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "ACh", CS_MENU_SELECTED_SID_LR, 0x03, PRINT_VAR_LR
	CS_MENU_T_ENTRY	CS_MENU_TABLE_M_LFO,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "Ins", CS_MENU_SELECTED_INS,  0x05, PRINT_VAR_DECP1
	CS_MENU_T_ENTRY	CS_MENU_TABLE_M_ENV,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "Ins", CS_MENU_SELECTED_INS,  0x05, PRINT_VAR_DECP1
	CS_MENU_T_ENTRY	CS_MENU_TABLE_M_CFG,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "Ins", CS_MENU_SELECTED_INS,  0x05, PRINT_VAR_DECP1
	CS_MENU_T_ENTRY	CS_MENU_TABLE_M_ARP,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "Ins", CS_MENU_SELECTED_INS,  0x05, PRINT_VAR_DECP1
	CS_MENU_T_ENTRY	CS_MENU_TABLE_M_WTC,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "Ins", CS_MENU_SELECTED_INS,  0x05, PRINT_VAR_DECP1
	CS_MENU_T_ENTRY	CS_MENU_TABLE_M_WTE,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "Pos", CS_MENU_WT_POS,        0x80, PRINT_WTE_POS ; 
	CS_MENU_T_ENTRY	CS_MENU_TABLE_M_KNB,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "KNB", CS_MENU_SELECTED_KNOB, 0x07, PRINT_KNB
	CS_MENU_T_ENTRY	CS_MENU_TABLE_M_EXT,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "---", 0,                     0x00, PRINT_NOP
	CS_MENU_T_ENTRY	CS_MENU_TABLE_M_RND,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "---", 0,                     0x00, PRINT_NOP
	CS_MENU_T_ENTRY	CS_MENU_TABLE_M_SAV,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot,   "---", 0,                     0x00, PRINT_NOP

	;; ID's for menu structures of the multi engine
CS_MENU_M_OSC	EQU	0x01
CS_MENU_M_FIL	EQU	0x02
CS_MENU_M_LFO	EQU	0x03
CS_MENU_M_ENV	EQU	0x04
CS_MENU_M_CFG	EQU	0x05
CS_MENU_M_ARP	EQU	0x06
CS_MENU_M_WTC	EQU	0x07
CS_MENU_M_WTE	EQU	0x08
CS_MENU_M_KNB	EQU	0x09
CS_MENU_M_EXT	EQU	0x0a
CS_MENU_M_RND	EQU	0x0b
CS_MENU_M_SAV	EQU	0x0c


; --------------------------------------------------------------------------

	;; ID for root and main page (7th bit must be set)
CS_MENU_ROOT	EQU	0x00
CS_MENU_MAIN	EQU	0x80
	


; ==========================================================================
;  The main menu
; ==========================================================================
CS_MENU_TABLE_E_ROOT
	db	(CS_MENU_TABLE_E_ROOT_End-CS_MENU_TABLE_E_ROOT)/CS_MENU_ENTRY_LEN, 0x00	; (no label used to avoid, that "ensemble" is print twice on screen, which looks ugly)

	;;              Register (00=dummy)        |<->|   max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_E_SID,             "SID",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_E_CLK,             "CLK",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_E_FIL,             "FIL",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_E_INS,             "INS",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_E_EXT,             "EXT",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_E_SAV,             "SAV",  0x000, PRINT_NOP,        EXEC_MENU_SAVE_ENS, R2PP2R_NOP
CS_MENU_TABLE_E_ROOT_End


CS_MENU_TABLE_L_ROOT
	db	(CS_MENU_TABLE_L_ROOT_End-CS_MENU_TABLE_L_ROOT)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_LEAD

	;;              Register (00=dummy)        |<->|   max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_L_OSC,             "OSC",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_L_FIL,             "FIL",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_L_LFO,             "LFO",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_L_ENV,             "ENV",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_L_MOD,             "MOD",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_L_TRG,             "TRG",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_L_CFG,             "CFG",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_L_ARP,             "ARP",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_L_WTC,             "WTC",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_L_WTE,             "WTE",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_L_KNB,             "KNB",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_L_EXT,             "EXT",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_L_RND,             "RND",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_L_SAV,             "SAV",  0x000, PRINT_NOP,        EXEC_MENU_SAVE,  R2PP2R_NOP
CS_MENU_TABLE_L_ROOT_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_B_ROOT
	db	(CS_MENU_TABLE_B_ROOT_End-CS_MENU_TABLE_B_ROOT)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_BASSLINE

	;;              Register (00=dummy)        |<->|   max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_B_SEQ,             "SEQ",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_B_SQC,             "SQC",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_B_OSC,             "OSC",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_B_O23,             "O23",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_B_FIL,             "FIL",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_B_LFO,             "LFO",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_B_ENV,             "ENV",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_B_ARP,             "ARP",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_B_CFG,             "CFG",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_B_KNB,             "KNB",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_B_EXT,             "EXT",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_B_RND,             "RND",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_B_SAV,             "SAV",  0x000, PRINT_NOP,        EXEC_MENU_SAVE,  R2PP2R_NOP
CS_MENU_TABLE_B_ROOT_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_D_ROOT
	db	(CS_MENU_TABLE_D_ROOT_End-CS_MENU_TABLE_D_ROOT)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_DRUM

	;;              Register (00=dummy)        |<->|   max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_D_SEQ,             "SEQ",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_D_SQC,             "SQC",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_D_DRM,             "DRM",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_D_FIL,             "FIL",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_D_CFG,             "CFG",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_D_KNB,             "KNB",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_D_EXT,             "EXT",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_D_RND,             "RND",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_D_SAV,             "SAV",  0x000, PRINT_NOP,        EXEC_MENU_SAVE,  R2PP2R_NOP
CS_MENU_TABLE_D_ROOT_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_M_ROOT
	db	(CS_MENU_TABLE_M_ROOT_End-CS_MENU_TABLE_M_ROOT)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_MULTI

	;;              Register (00=dummy)        |<->|   max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_M_OSC,             "OSC",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_M_FIL,             "FIL",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_M_LFO,             "LFO",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_M_ENV,             "ENV",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_M_CFG,             "CFG",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_M_ARP,             "ARP",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_M_WTC,             "WTC",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_M_WTE,             "WTE",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_M_KNB,             "KNB",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_M_EXT,             "EXT",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_M_RND,             "RND",  0x000, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_M_SAV,             "SAV",  0x000, PRINT_NOP,        EXEC_MENU_SAVE,  R2PP2R_NOP
CS_MENU_TABLE_M_ROOT_End


; ==========================================================================
;  Special Ensemble Menu Pages
; ==========================================================================
CS_MENU_TABLE_E_SID
	db	(CS_MENU_TABLE_E_SID_End-CS_MENU_TABLE_E_SID)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_ENS_SID

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_SID_FLAGS, "SID", 0x003, PRINT_VAR_SID1234,EXEC_SELPAR,     R2PP2R_VAR_SID
	CS_MENU_ENTRY	SID_ENSx_BANK,              "Bnk", 0x007, PRINT_BNK,        EXEC_SELPAR_BANK,  R2PP2R_BNK
	CS_MENU_ENTRY	SID_ENSx_PATCH,             "Pat", 0x07f, PRINT_PAT,        EXEC_SELPAR_PATCH, R2PP2R_PAT
	CS_MENU_ENTRY	SID_ENSx_CTRL1,             "Mon", 0x001, PRINT_ENSx_MONO,  EXEC_TOGPAR,     R2PP2R_ENSx_MONO
	CS_MENU_ENTRY	SID_ENSx_SUPERPOLY_CTRL,    "SPV", 0x004, PRINT_ENSx_SPV,   EXEC_SELPAR,     R2PP2R_ENSx_SPV
	CS_MENU_ENTRY	SID_ENSx_CTRL1,             "DOR", 0x001, PRINT_ENSx_DOR,   EXEC_TOGPAR,     R2PP2R_ENSx_DOR
CS_MENU_TABLE_E_SID_End


CS_MENU_TABLE_E_CLK
	db	(CS_MENU_TABLE_E_CLK_End-CS_MENU_TABLE_E_CLK)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_ENS_CLK

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_SID_FLAGS, "SID", 0x003, PRINT_VAR_SID1234,EXEC_SELPAR,     R2PP2R_VAR_SID
	CS_MENU_ENTRY	SID_ENSx_BPM,               "BPM", 0x0ff-50, PRINT_CLK_BPM,    EXEC_SELPAR,     R2PP2R_CLK_BPM
	CS_MENU_ENTRY	SID_ENSx_CTRL1,             "Mod", 0x002, PRINT_CLK_SLAVE,  EXEC_TOGPAR,     R2PP2R_CLK_SLAVE
CS_MENU_TABLE_E_CLK_End


CS_MENU_TABLE_E_FIL
	db	(CS_MENU_TABLE_E_FIL_End-CS_MENU_TABLE_E_FIL)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_ENS_FIL

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_SID_FLAGS, "SID", 0x003, PRINT_VAR_SID1234,EXEC_SELPAR,     R2PP2R_VAR_SID
	CS_MENU_ENTRY	CS_MENU_SELECTED_SID_LR,    "ACh", 0x002, PRINT_VAR_LR,     EXEC_TOGPAR,     R2PP2R_VAR_SIDLR
	CS_MENU_ENTRY	SID_ENSx_CALI_FIL1_MIN_L,   "Min", 0xfff, PRINT_VAR_CALI,   EXEC_SELPAR,     R2PP2R_VAR_CALI
	CS_MENU_ENTRY	SID_ENSx_CALI_FIL1_MAX_L,   "Max", 0xfff, PRINT_VAR_CALI,   EXEC_SELPAR,     R2PP2R_VAR_CALI
	CS_MENU_ENTRY	SID_ENSx_CTRL1,             "Log", 0x001, PRINT_ENSx_FILLOG,EXEC_TOGPAR,     R2PP2R_ENSx_FILLOG
CS_MENU_TABLE_E_FIL_End


CS_MENU_TABLE_E_INS
	db	(CS_MENU_TABLE_E_INS_End-CS_MENU_TABLE_E_INS)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_ENS_INS

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_SID_FLAGS, "SID", 0x003, PRINT_VAR_SID1234,EXEC_SELPAR,     R2PP2R_VAR_SID
	CS_MENU_ENTRY	CS_MENU_SELECTED_INS,       "Ins", 0x005, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_ENSx_Ix_CHN,            "Chn", 0x010, PRINT_ENSxIx_CHN, EXEC_SELPAR,     R2PP2R_CHN
	CS_MENU_ENTRY	SID_ENSx_Ix_SPLIT_L,        "SpL", 0x07f, PRINT_ENSxIx_NOTE,EXEC_SELPAR,     R2PP2R_ENSxIx_VAL
	CS_MENU_ENTRY	SID_ENSx_Ix_SPLIT_U,        "SpU", 0x07f, PRINT_ENSxIx_NOTE,EXEC_SELPAR,     R2PP2R_ENSxIx_VAL
	CS_MENU_ENTRY	SID_ENSx_Ix_TRN,            "Trn", 0x07f, PRINT_ENSxIx_PMDEC7,EXEC_SELPAR,   R2PP2R_ENSxIx_VAL
CS_MENU_TABLE_E_INS_End

CS_MENU_TABLE_E_EXT
	db	(CS_MENU_TABLE_E_EXT_End-CS_MENU_TABLE_E_EXT)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_ENS_EXT

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_SID_FLAGS, "SID", 0x003, PRINT_VAR_SID1234,EXEC_SELPAR,     R2PP2R_VAR_SID
	CS_MENU_ENTRY	SID_ENS_CTRL2_F2A,          "F2A", 0x001, PRINT_ENSx_X2A,   EXEC_TOGPAR,     R2PP2R_ENSx_X2A
	CS_MENU_ENTRY	SID_ENS_CTRL2_V2A,          "V2A", 0x001, PRINT_ENSx_X2A,   EXEC_TOGPAR,     R2PP2R_ENSx_X2A
	CS_MENU_ENTRY	SID_ENS_CTRL2_P2A,          "P2A", 0x001, PRINT_ENSx_X2A,   EXEC_TOGPAR,     R2PP2R_ENSx_X2A
	CS_MENU_ENTRY	SID_ENS_CTRL2_K2A,          "K2A", 0x001, PRINT_ENSx_X2A,   EXEC_TOGPAR,     R2PP2R_ENSx_X2A
	CS_MENU_ENTRY	SID_ENS_CTRL2_O2A,          "O2A", 0x001, PRINT_ENSx_X2A,   EXEC_TOGPAR,     R2PP2R_ENSx_X2A
	CS_MENU_ENTRY	0,                          "Iv1", 0x001, PRINT_ENSx_EXTINV,EXEC_TOGPAR,     R2PP2R_ENSx_EXTINV
	CS_MENU_ENTRY	1,                          "Iv2", 0x001, PRINT_ENSx_EXTINV,EXEC_TOGPAR,     R2PP2R_ENSx_EXTINV
	CS_MENU_ENTRY	2,                          "Iv3", 0x001, PRINT_ENSx_EXTINV,EXEC_TOGPAR,     R2PP2R_ENSx_EXTINV
	CS_MENU_ENTRY	3,                          "Iv4", 0x001, PRINT_ENSx_EXTINV,EXEC_TOGPAR,     R2PP2R_ENSx_EXTINV
	CS_MENU_ENTRY	4,                          "Iv5", 0x001, PRINT_ENSx_EXTINV,EXEC_TOGPAR,     R2PP2R_ENSx_EXTINV
	CS_MENU_ENTRY	5,                          "Iv6", 0x001, PRINT_ENSx_EXTINV,EXEC_TOGPAR,     R2PP2R_ENSx_EXTINV
	CS_MENU_ENTRY	6,                          "Iv7", 0x001, PRINT_ENSx_EXTINV,EXEC_TOGPAR,     R2PP2R_ENSx_EXTINV
	CS_MENU_ENTRY	7,                          "Iv8", 0x001, PRINT_ENSx_EXTINV,EXEC_TOGPAR,     R2PP2R_ENSx_EXTINV
CS_MENU_TABLE_E_EXT_End


CS_MENU_TABLE_E_SAV
	db	(CS_MENU_TABLE_E_SAV_End-CS_MENU_TABLE_E_SAV)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_SAVE

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SAVE_ENS,           "E# ", 0x07f, PRINT_SAVE_ENS,   EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	0x00,                       "do!", 0x000, PRINT_NOP,        EXEC_SAVE_ENS,   R2PP2R_NOP
CS_MENU_TABLE_E_SAV_End


; ==========================================================================
;  The oscillator menu
; ==========================================================================
CS_MENU_TABLE_L_OSC
	db	(CS_MENU_TABLE_L_OSC_End-CS_MENU_TABLE_L_OSC)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_OSC

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_OSC_FLAGS, "OSC", 0x006, PRINT_VAR_123,    EXEC_SELPAR,     R2PP2R_VAR_OSC
	CS_MENU_ENTRY	SID_Ix_Vx_WAVEFORM,         "Wav", 0x008, PRINT_Vx_WAV,     EXEC_SELPAR,     R2PP2R_Vx_WAV
	CS_MENU_ENTRY	SID_Ix_Vx_WAVEFORM,         "S/R", 0x003, PRINT_Vx_S_R,     EXEC_TOGPAR,     R2PP2R_Vx_S_R
	CS_MENU_ENTRY	SID_Ix_Vx_DELAY,            "Del", 0x0ff, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_AD,               "Atk", 0x00f, PRINT_Vx_DEC4U,   EXEC_SELPAR,     R2PP2R_Vx_4U
	CS_MENU_ENTRY	SID_Ix_Vx_AD,               "Dec", 0x00f, PRINT_Vx_DEC4L,   EXEC_SELPAR,     R2PP2R_Vx_4L
	CS_MENU_ENTRY	SID_Ix_Vx_SR,               "Sus", 0x00f, PRINT_Vx_DEC4U,   EXEC_SELPAR,     R2PP2R_Vx_4U
	CS_MENU_ENTRY	SID_Ix_Vx_SR,               "Rel", 0x00f, PRINT_Vx_DEC4L,   EXEC_SELPAR,     R2PP2R_Vx_4L
	CS_MENU_ENTRY	SID_Ix_Vx_PITCHRANGE,       "PRn", 0x07f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_TRANSPOSE,        "Trn", 0x07f, PRINT_Vx_PMDEC7,  EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_FINETUNE,         "Fin", 0x0ff, PRINT_Vx_PMDEC8,  EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_PORTAMENTO,       "Por", 0x0ff, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_L_OSC_PHASE,         "Phs", 0x0ff, PRINT_DEC8,       EXEC_SELPAR,     R2PP2R_PAR8
	CS_MENU_ENTRY	SID_Ix_Vx_PULSEWIDTH_L,     "PW ", 0xfff, PRINT_Vx_HEX12,   EXEC_SELPAR,     R2PP2R_Vx_12
	CS_MENU_ENTRY	SID_Ix_Vx_FLAGS1,	    "PMd", 0x002, PRINT_Vx_PMD,     EXEC_TOGPAR,     R2PP2R_Vx_PMD
	CS_MENU_ENTRY	SID_Ix_Vx_FLAGS1,	    "GSA", 0x001, PRINT_Vx_GSA,     EXEC_TOGPAR,     R2PP2R_Vx_GSA
	CS_MENU_ENTRY	SID_Ix_L_OSC_DETUNE,        "Det", 0x0ff, PRINT_DEC8,       EXEC_SELPAR,     R2PP2R_PAR8
#if ENABLE_SWINSID
	CS_MENU_ENTRY	SID_Ix_Vx_SWINSID_MODE,     "SwM", 0x0ff, PRINT_Vx_HEX8,    EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_SWINSID_PHASE,    "SwP", 0x0ff, PRINT_Vx_HEX8,    EXEC_SELPAR,     R2PP2R_Vx
#endif
CS_MENU_TABLE_L_OSC_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_M_OSC
	db	(CS_MENU_TABLE_M_OSC_End-CS_MENU_TABLE_M_OSC)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_OSC

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_INS,       "Ins", 0x005, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_Vx_WAVEFORM,         "Wav", 0x008, PRINT_Vx_WAV,     EXEC_SELPAR,     R2PP2R_Vx_WAV
	CS_MENU_ENTRY	SID_Ix_Vx_WAVEFORM,         "S/R", 0x003, PRINT_Vx_S_R,     EXEC_TOGPAR,     R2PP2R_Vx_S_R
	CS_MENU_ENTRY	SID_Ix_Vx_DELAY,            "Del", 0x0ff, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_AD,               "Atk", 0x00f, PRINT_Vx_DEC4U,   EXEC_SELPAR,     R2PP2R_Vx_4U
	CS_MENU_ENTRY	SID_Ix_Vx_AD,               "Dec", 0x00f, PRINT_Vx_DEC4L,   EXEC_SELPAR,     R2PP2R_Vx_4L
	CS_MENU_ENTRY	SID_Ix_Vx_SR,               "Sus", 0x00f, PRINT_Vx_DEC4U,   EXEC_SELPAR,     R2PP2R_Vx_4U
	CS_MENU_ENTRY	SID_Ix_Vx_SR,               "Rel", 0x00f, PRINT_Vx_DEC4L,   EXEC_SELPAR,     R2PP2R_Vx_4L
	CS_MENU_ENTRY	SID_Ix_Vx_PITCHRANGE,       "PRn", 0x07f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_TRANSPOSE,        "Trn", 0x07f, PRINT_Vx_PMDEC7,  EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_FINETUNE,         "Fin", 0x0ff, PRINT_Vx_PMDEC8,  EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_PORTAMENTO,       "Por", 0x0ff, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_M_Vx_FLAGS2,         "Phs", 0x001, PRINT_M_Vx_PHASE, EXEC_TOGPAR,     R2PP2R_M_Vx_PHASE
	CS_MENU_ENTRY	SID_Ix_Vx_PULSEWIDTH_L,     "PW ", 0xfff, PRINT_Vx_HEX12,   EXEC_SELPAR,     R2PP2R_Vx_12
	CS_MENU_ENTRY	SID_Ix_Vx_FLAGS1,	    "PMd", 0x002, PRINT_Vx_PMD,     EXEC_TOGPAR,     R2PP2R_Vx_PMD
#if ENABLE_SWINSID
	CS_MENU_ENTRY	SID_Ix_Vx_SWINSID_MODE,     "SwM", 0x0ff, PRINT_Vx_HEX8,    EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_SWINSID_PHASE,    "SwP", 0x0ff, PRINT_Vx_HEX8,    EXEC_SELPAR,     R2PP2R_Vx
#endif
CS_MENU_TABLE_M_OSC_End


; --------------------------------------------------------------------------
CS_MENU_TABLE_B_OSC
	db	(CS_MENU_TABLE_B_OSC_End-CS_MENU_TABLE_B_OSC)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_OSC

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_SID_LR,    "ACh", 0x002, PRINT_VAR_LR,     EXEC_TOGPAR,     R2PP2R_VAR_SIDLR
	CS_MENU_ENTRY	SID_Ix_Vx_WAVEFORM,         "Wav", 0x008, PRINT_Vx_WAV,     EXEC_SELPAR,     R2PP2R_Vx_WAV
	CS_MENU_ENTRY	SID_Ix_Vx_WAVEFORM,         "S/R", 0x003, PRINT_Vx_S_R,     EXEC_TOGPAR,     R2PP2R_Vx_S_R
	CS_MENU_ENTRY	SID_Ix_Vx_DELAY,            "Del", 0x0ff, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_AD,               "Atk", 0x00f, PRINT_Vx_DEC4U,   EXEC_SELPAR,     R2PP2R_Vx_4U
	CS_MENU_ENTRY	SID_Ix_Vx_AD,               "Dec", 0x00f, PRINT_Vx_DEC4L,   EXEC_SELPAR,     R2PP2R_Vx_4L
	CS_MENU_ENTRY	SID_Ix_Vx_SR,               "Sus", 0x00f, PRINT_Vx_DEC4U,   EXEC_SELPAR,     R2PP2R_Vx_4U
	CS_MENU_ENTRY	SID_Ix_Vx_SR,               "Rel", 0x00f, PRINT_Vx_DEC4L,   EXEC_SELPAR,     R2PP2R_Vx_4L
	CS_MENU_ENTRY	SID_Ix_Vx_PITCHRANGE,       "PRn", 0x07f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_TRANSPOSE,        "Trn", 0x07f, PRINT_Vx_PMDEC7,  EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_FINETUNE,         "Fin", 0x0ff, PRINT_Vx_PMDEC8,  EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_PORTAMENTO,       "Por", 0x0ff, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_M_Vx_FLAGS2,         "Phs", 0x001, PRINT_M_Vx_PHASE, EXEC_TOGPAR,     R2PP2R_M_Vx_PHASE
	CS_MENU_ENTRY	SID_Ix_Vx_PULSEWIDTH_L,     "PW ", 0xfff, PRINT_Vx_HEX12,   EXEC_SELPAR,     R2PP2R_Vx_12
	CS_MENU_ENTRY	SID_Ix_Vx_FLAGS1,	    "PMd", 0x002, PRINT_Vx_PMD,     EXEC_TOGPAR,     R2PP2R_Vx_PMD
#if ENABLE_SWINSID
	CS_MENU_ENTRY	SID_Ix_Vx_SWINSID_MODE,     "SwM", 0x0ff, PRINT_Vx_HEX8,    EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Vx_SWINSID_PHASE,    "SwP", 0x0ff, PRINT_Vx_HEX8,    EXEC_SELPAR,     R2PP2R_Vx
#endif
CS_MENU_TABLE_B_OSC_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_D_DRM
	db	(CS_MENU_TABLE_D_DRM_End-CS_MENU_TABLE_D_DRM)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_INS

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_INS,       "Ins", 0x00f, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_Dx_MODEL,            "Mdl", SID_DMODEL_NUM-1, PRINT_D_MODEL, EXEC_SELPAR, R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Dx_AD,               "Atk", 0x00f, PRINT_Vx_DEC4U,   EXEC_SELPAR,     R2PP2R_Vx_4U
	CS_MENU_ENTRY	SID_Ix_Dx_AD,               "Dec", 0x00f, PRINT_Vx_DEC4L,   EXEC_SELPAR,     R2PP2R_Vx_4L
	CS_MENU_ENTRY	SID_Ix_Dx_SR,               "Sus", 0x00f, PRINT_Vx_DEC4U,   EXEC_SELPAR,     R2PP2R_Vx_4U
	CS_MENU_ENTRY	SID_Ix_Dx_SR,               "Rel", 0x00f, PRINT_Vx_DEC4L,   EXEC_SELPAR,     R2PP2R_Vx_4L
	CS_MENU_ENTRY	SID_Ix_Dx_TUNE,             "Tun", 0x0ff, PRINT_Vx_PMDEC8,  EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Dx_PAR1,             "GL.", 0x0ff, PRINT_Vx_PMDEC8,  EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Dx_PAR2,             "Spd", 0x0ff, PRINT_Vx_PMDEC8,  EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Dx_PAR3,             "PAR", 0x0ff, PRINT_Vx_PMDEC8,  EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_Dx_FLAGS1,           "VAs", 0x008, PRINT_D_Vx_VAS,   EXEC_SELPAR,     R2PP2R_D_Vx_VAS
	CS_MENU_ENTRY	SID_Ix_Dx_VELOCITY_ASSGN,   "VeA", 0x0ff, PRINT_M_D_Vx_ASG, EXEC_SELPAR_ASG, R2PP2R_M_D_Vx_ASG
	CS_MENU_ENTRY	0x00,                       "Rnd", 0x000, PRINT_WT_GO,      EXEC_DRM_RND,    R2PP2R_NOP
CS_MENU_TABLE_D_DRM_End

; ==========================================================================
;  The O23 menu (bassline only)
; ==========================================================================
CS_MENU_TABLE_B_O23
	db	(CS_MENU_TABLE_B_O23_End-CS_MENU_TABLE_B_O23)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_O23

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_SID_LR,    "ACh", 0x002, PRINT_VAR_LR,     EXEC_TOGPAR,     R2PP2R_VAR_SIDLR
	CS_MENU_ENTRY	SID_Ix_B_V2_WAVEFORM,       "Wv2", 0x008, PRINT_Vx_WAV,     EXEC_SELPAR,     R2PP2R_Vx_WAV
	CS_MENU_ENTRY	SID_Ix_B_V3_WAVEFORM,       "Wv3", 0x008, PRINT_Vx_WAV,     EXEC_SELPAR,     R2PP2R_Vx_WAV
	CS_MENU_ENTRY	SID_Ix_B_V2_OCT_TRANSPOSE,  "Oc2", 0x007, PRINT_Vx_DECPM3,  EXEC_SELPAR,     R2PP2R_Vx_PM3
	CS_MENU_ENTRY	SID_Ix_B_V3_OCT_TRANSPOSE,  "Oc3", 0x007, PRINT_Vx_DECPM3,  EXEC_SELPAR,     R2PP2R_Vx_PM3
	CS_MENU_ENTRY	SID_Ix_B_V2_PULSEWIDTH_L,   "PW2", 0xfff, PRINT_Vx_HEX12,   EXEC_SELPAR,     R2PP2R_Vx_12
	CS_MENU_ENTRY	SID_Ix_B_V3_PULSEWIDTH_L,   "PW3", 0xfff, PRINT_Vx_HEX12,   EXEC_SELPAR,     R2PP2R_Vx_12
	CS_MENU_ENTRY	SID_Ix_B_V2_STATIC_NOTE,    "Nt2", 0x07f, PRINT_Vx_NOTE,    EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_B_V3_STATIC_NOTE,    "Nt3", 0x07f, PRINT_Vx_NOTE,    EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_B_OSC_PHASE,         "Phs", 0x0ff, PRINT_DEC8,       EXEC_SELPAR,     R2PP2R_PAR8
	CS_MENU_ENTRY	SID_Ix_B_OSC_DETUNE,        "Det", 0x0ff, PRINT_DEC8,       EXEC_SELPAR,     R2PP2R_PAR8
CS_MENU_TABLE_B_O23_End

; ==========================================================================
;  The filter menu
; ==========================================================================
CS_MENU_TABLE_L_FIL
	db	(CS_MENU_TABLE_L_FIL_End-CS_MENU_TABLE_L_FIL)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_FIL

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	SID_Ix_L_Fx_CHN_MODE,       "Chn", 0x007, PRINT_Fx_CHN,     EXEC_SELPAR,     R2PP2R_Fx_CHN
	CS_MENU_ENTRY	SID_Ix_L_Fx_CUTOFF_L,       "Cut", 0xfff, PRINT_Fx_CUTOFF,  EXEC_SELPAR,     R2PP2R_Fx_CUTOFF
	CS_MENU_ENTRY	SID_Ix_L_Fx_RESONANCE,      "Res", 0x0ff, PRINT_Fx_RESON,   EXEC_SELPAR,     R2PP2R_Fx_RESON
	CS_MENU_ENTRY	SID_Ix_L_Fx_CHN_MODE,       "Mod", 0x007, PRINT_Fx_MOD,     EXEC_SELPAR,     R2PP2R_Fx_MOD
	CS_MENU_ENTRY	SID_Ix_L_Fx_KEYTRACK,       "KTr", 0x0ff, PRINT_Fx_KTR,     EXEC_SELPAR,     R2PP2R_Fx_KTR
	CS_MENU_ENTRY	SID_Ix_L_Fx_CHN_MODE,       "Ext", 0x001, PRINT_Fx_EXT,     EXEC_TOGPAR,     R2PP2R_Fx_EXT
	CS_MENU_ENTRY	SID_Ix_L_Fx_CHN_MODE,       "3Of", 0x001, PRINT_Fx_3OF,     EXEC_TOGPAR,     R2PP2R_Fx_3OF
	CS_MENU_ENTRY	SID_Ix_L_Fx_CUTOFF_H,       "FIP", 0x001, PRINT_Fx_FIP,     EXEC_TOGPAR,     R2PP2R_Fx_FIP
CS_MENU_TABLE_L_FIL_End


CS_MENU_TABLE_M_FIL
CS_MENU_TABLE_D_FIL
	db	(CS_MENU_TABLE_M_FIL_End-CS_MENU_TABLE_M_FIL)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_FIL

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	SID_Ix_L_Fx_CHN_MODE,       "Chn", 0x007, PRINT_Fx_CHN,     EXEC_SELPAR,     R2PP2R_Fx_CHN
	CS_MENU_ENTRY	SID_Ix_L_Fx_CUTOFF_L,       "Cut", 0xfff, PRINT_Fx_CUTOFF,  EXEC_SELPAR,     R2PP2R_Fx_CUTOFF
	CS_MENU_ENTRY	SID_Ix_L_Fx_RESONANCE,      "Res", 0x0ff, PRINT_Fx_RESON,   EXEC_SELPAR,     R2PP2R_Fx_RESON
	CS_MENU_ENTRY	SID_Ix_L_Fx_CHN_MODE,       "Mod", 0x007, PRINT_Fx_MOD,     EXEC_SELPAR,     R2PP2R_Fx_MOD
	CS_MENU_ENTRY	SID_Ix_L_Fx_CHN_MODE,       "Ext", 0x001, PRINT_Fx_EXT,     EXEC_TOGPAR,     R2PP2R_Fx_EXT
	CS_MENU_ENTRY	SID_Ix_L_Fx_CHN_MODE,       "3Of", 0x001, PRINT_Fx_3OF,     EXEC_TOGPAR,     R2PP2R_Fx_3OF
	CS_MENU_ENTRY	SID_Ix_L_Fx_CUTOFF_H,       "FIP", 0x001, PRINT_Fx_FIP,     EXEC_TOGPAR,     R2PP2R_Fx_FIP
CS_MENU_TABLE_M_FIL_End


CS_MENU_TABLE_B_FIL
	db	(CS_MENU_TABLE_B_FIL_End-CS_MENU_TABLE_B_FIL)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_FIL

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	SID_Ix_L_Fx_CHN_MODE,       "Chn", 0x007, PRINT_Fx_CHN,     EXEC_SELPAR,     R2PP2R_Fx_CHN
	CS_MENU_ENTRY	SID_Ix_L_Fx_CUTOFF_L,       "Cut", 0xfff, PRINT_Fx_CUTOFF,  EXEC_SELPAR,     R2PP2R_Fx_CUTOFF
	CS_MENU_ENTRY	SID_Ix_L_Fx_RESONANCE,      "Res", 0x0ff, PRINT_Fx_RESON,   EXEC_SELPAR,     R2PP2R_Fx_RESON
	CS_MENU_ENTRY	SID_Ix_L_Fx_CHN_MODE,       "Mod", 0x007, PRINT_Fx_MOD,     EXEC_SELPAR,     R2PP2R_Fx_MOD
	CS_MENU_ENTRY	SID_Ix_L_Fx_KEYTRACK,       "KTr", 0x0ff, PRINT_Fx_KTR,     EXEC_SELPAR,     R2PP2R_Fx_KTR
	CS_MENU_ENTRY	SID_Ix_L_Fx_CHN_MODE,       "Ext", 0x001, PRINT_Fx_EXT,     EXEC_TOGPAR,     R2PP2R_Fx_EXT
	CS_MENU_ENTRY	SID_Ix_L_Fx_CHN_MODE,       "3Of", 0x001, PRINT_Fx_3OF,     EXEC_TOGPAR,     R2PP2R_Fx_3OF
	CS_MENU_ENTRY	SID_Ix_L_Fx_CUTOFF_H,       "FIP", 0x001, PRINT_Fx_FIP,     EXEC_TOGPAR,     R2PP2R_Fx_FIP
CS_MENU_TABLE_B_FIL_End


; ==========================================================================
;  The LFO menu
; ==========================================================================
CS_MENU_TABLE_L_LFO
	db	(CS_MENU_TABLE_L_LFO_End-CS_MENU_TABLE_L_LFO)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_LFO

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_LFO,       "LFO", 0x005, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_LFOx_MODE,           "Wav", 0x009, PRINT_LFOx_WAV,   EXEC_SELPAR,     R2PP2R_LFOx_WAV
	CS_MENU_ENTRY	SID_Ix_LFOx_RATE,           "Rte", 0x0ff, PRINT_LFOx_RATE,  EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_LFOx_MODE,           "CSn", 0x001, PRINT_LFOx_CSN,   EXEC_TOGPAR,     R2PP2R_LFOx_CSN
	CS_MENU_ENTRY	SID_Ix_LFOx_DEPTH,          "Dep", 0x0ff, PRINT_LFOx_PMDEC8,EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_TRG_TARGET_H_L1+8,      "Snc", 0x001, PRINT_LFOx_SNC,   EXEC_TOGPAR,     R2PP2R_LFOx_SNC
	CS_MENU_ENTRY	SID_Ix_LFOx_DELAY,          "Del", 0x0ff, PRINT_LFOx_DEC,   EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_LFOx_PHASE,          "Phs", 0x0ff, PRINT_LFOx_DEC,   EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_LFOx_MODE,           "One", 0x001, PRINT_LFOx_ONE,   EXEC_TOGPAR,     R2PP2R_LFOx_ONE
CS_MENU_TABLE_L_LFO_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_M_LFO
	db	(CS_MENU_TABLE_M_LFO_End-CS_MENU_TABLE_M_LFO)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_LFO

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_INS,       "Ins", 0x005, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	CS_MENU_SELECTED_LFO,       "LFO", 0x001, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_MODE,      "Wav", 0x009, PRINT_LFOx_WAV,   EXEC_SELPAR,     R2PP2R_LFOx_WAV
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_RATE,      "Rte", 0x0ff, PRINT_LFOx_RATE,  EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_MODE,      "CSn", 0x001, PRINT_LFOx_CSN,   EXEC_TOGPAR,     R2PP2R_LFOx_CSN
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_DEPTH_P,   "D.P", 0x0ff, PRINT_LFOx_PMDEC8,EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_DEPTH_PW,  "DPW", 0x0ff, PRINT_LFOx_PMDEC8,EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_DEPTH_F,   "D.F", 0x0ff, PRINT_LFOx_PMDEC8,EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_MODE,      "Snc", 0x001, PRINT_LFOx_SNC_M, EXEC_TOGPAR,     R2PP2R_LFOx_SNC_M
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_DELAY,     "Del", 0x0ff, PRINT_LFOx_DEC,   EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_PHASE,     "Phs", 0x0ff, PRINT_LFOx_DEC,   EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_MODE,      "One", 0x001, PRINT_LFOx_ONE,   EXEC_TOGPAR,     R2PP2R_LFOx_ONE
CS_MENU_TABLE_M_LFO_End


; --------------------------------------------------------------------------
CS_MENU_TABLE_B_LFO
	db	(CS_MENU_TABLE_B_LFO_End-CS_MENU_TABLE_B_LFO)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_LFO

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_SID_LR,    "ACh", 0x002, PRINT_VAR_LR,     EXEC_TOGPAR,     R2PP2R_VAR_SIDLR
	CS_MENU_ENTRY	CS_MENU_SELECTED_LFO,       "LFO", 0x001, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_MODE,      "Wav", 0x009, PRINT_LFOx_WAV,   EXEC_SELPAR,     R2PP2R_LFOx_WAV
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_RATE,      "Rte", 0x0ff, PRINT_LFOx_RATE,  EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_MODE,      "CSn", 0x001, PRINT_LFOx_CSN,   EXEC_TOGPAR,     R2PP2R_LFOx_CSN
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_DEPTH_P,   "D.P", 0x0ff, PRINT_LFOx_PMDEC8,EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_DEPTH_PW,  "DPW", 0x0ff, PRINT_LFOx_PMDEC8,EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_DEPTH_F,   "D.F", 0x0ff, PRINT_LFOx_PMDEC8,EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_MODE,      "Snc", 0x001, PRINT_LFOx_SNC_M, EXEC_TOGPAR,     R2PP2R_LFOx_SNC_M
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_DELAY,     "Del", 0x0ff, PRINT_LFOx_DEC,   EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_PHASE,     "Phs", 0x0ff, PRINT_LFOx_DEC,   EXEC_SELPAR,     R2PP2R_LFOx
	CS_MENU_ENTRY	SID_Ix_M_Vx_LFO1_MODE,      "One", 0x001, PRINT_LFOx_ONE,   EXEC_TOGPAR,     R2PP2R_LFOx_ONE
	CS_MENU_ENTRY	SID_Ix_Vx_ACCENT,	    "Acc", 0x07f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
CS_MENU_TABLE_B_LFO_End


; ==========================================================================
;  The envelope menu
; ==========================================================================
CS_MENU_TABLE_L_ENV
	db	(CS_MENU_TABLE_L_ENV_End-CS_MENU_TABLE_L_ENV)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_ENV

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_ENV,       "ENV", 0x001, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_L_ENVx_DEPTH,        "Dep", 0x0ff, PRINT_ENVx_PMDEC8,EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_DELAY,        "Del", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_ATTACK1,      "At1", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_ATTLVL,       "ALv", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_ATTACK2,      "At2", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_DECAY1,       "De1", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_DECLVL,       "DLv", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_DECAY2,       "De2", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_SUSTAIN,      "Sus", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_RELEASE1,     "Re1", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_RELLVL,       "RLv", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_RELEASE2,     "Re2", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_ATT_CURVE,    "CuA", 0x0ff, PRINT_ENVx_PMDEC8,EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_DEC_CURVE,    "CuD", 0x0ff, PRINT_ENVx_PMDEC8,EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_REL_CURVE,    "CuR", 0x0ff, PRINT_ENVx_PMDEC8,EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_L_ENVx_MODE,         "LpB", 0x007, PRINT_ENVx_LPB,   EXEC_SELPAR,     R2PP2R_ENVx_LPB
	CS_MENU_ENTRY	SID_Ix_L_ENVx_MODE,         "LpE", 0x007, PRINT_ENVx_LPE,   EXEC_SELPAR,     R2PP2R_ENVx_LPE
	CS_MENU_ENTRY	SID_Ix_L_ENVx_MODE,         "CSn", 0x001, PRINT_ENVx_CSN,   EXEC_TOGPAR,     R2PP2R_ENVx_CSN
CS_MENU_TABLE_L_ENV_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_M_ENV
	db	(CS_MENU_TABLE_M_ENV_End-CS_MENU_TABLE_M_ENV)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_ENV

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_INS,       "Ins", 0x005, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_M_Vx_ENV_DEPTH_P,    "D.P", 0x0ff, PRINT_ENVx_PMDEC8,EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_M_Vx_ENV_DEPTH_PW,   "DPW", 0x0ff, PRINT_ENVx_PMDEC8,EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_M_Vx_ENV_DEPTH_F,    "D.F", 0x0ff, PRINT_ENVx_PMDEC8,EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_M_Vx_ENV_ATTACK,     "Att", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_M_Vx_ENV_DECAY,      "Dec", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_M_Vx_ENV_SUSTAIN,    "Sus", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_M_Vx_ENV_RELEASE,    "Rel", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_M_Vx_ENV_CURVE,      "Cur", 0x0ff, PRINT_ENVx_PMDEC8,EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_M_Vx_ENV_MODE,       "CAs", 0x007, PRINT_ENVx_CAS,   EXEC_SELPAR,     R2PP2R_ENVx_CAS
CS_MENU_TABLE_M_ENV_End


; --------------------------------------------------------------------------
CS_MENU_TABLE_B_ENV
	db	(CS_MENU_TABLE_B_ENV_End-CS_MENU_TABLE_B_ENV)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_ENV

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_SID_LR,    "ACh", 0x002, PRINT_VAR_LR,     EXEC_TOGPAR,     R2PP2R_VAR_SIDLR
	CS_MENU_ENTRY	SID_Ix_B_Vx_ENV_DEPTH_P,    "D.P", 0x0ff, PRINT_ENVx_PMDEC8,EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_B_Vx_ENV_DEPTH_PW,   "DPW", 0x0ff, PRINT_ENVx_PMDEC8,EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_B_Vx_ENV_DEPTH_F,    "D.F", 0x0ff, PRINT_ENVx_PMDEC8,EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_B_Vx_ENV_ATTACK,     "Att", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_B_Vx_ENV_DECAY,      "Dec", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_B_Vx_ENV_SUSTAIN,    "Sus", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_B_Vx_ENV_RELEASE,    "Rel", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_B_Vx_ENV_CURVE,      "Cur", 0x0ff, PRINT_ENVx_PMDEC8,EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_M_Vx_ENV_MODE,       "CAs", 0x007, PRINT_ENVx_CAS,   EXEC_SELPAR,     R2PP2R_ENVx_CAS
	CS_MENU_ENTRY	SID_Ix_B_Vx_ENV_DECAY_A,    "DcA", 0x0ff, PRINT_ENVx_DEC,   EXEC_SELPAR,     R2PP2R_ENVx
	CS_MENU_ENTRY	SID_Ix_Vx_ACCENT,	    "Acc", 0x07f, PRINT_Vx_DEC,     EXEC_SELPAR,     R2PP2R_Vx
CS_MENU_TABLE_B_ENV_End


; ==========================================================================
;  The modulation matrix menu
; ==========================================================================
CS_MENU_TABLE_L_MOD
	db	(CS_MENU_TABLE_L_MOD_End-CS_MENU_TABLE_L_MOD)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_MOD

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_MOD,       "MOD", 0x007, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_MODx_SRC1,           "Sr1", SID_MOD_SRC_NUM+0x80, PRINT_MODx_SRC, EXEC_SELPAR, R2PP2R_MODx_SRC
	CS_MENU_ENTRY	SID_Ix_MODx_SRC2,           "Sr2", SID_MOD_SRC_NUM+0x80, PRINT_MODx_SRC, EXEC_SELPAR, R2PP2R_MODx_SRC
	CS_MENU_ENTRY	SID_Ix_MODx_OP,             "Op.", 0x00e, PRINT_MODx_OP,    EXEC_SELPAR,     R2PP2R_MODx_OP
	CS_MENU_ENTRY	SID_Ix_MODx_TARG_X1,        "Tr1", SID_MOD_TARG_NUM, PRINT_MODx_TAR, EXEC_SELPAR, R2PP2R_MODx
	CS_MENU_ENTRY	SID_Ix_MODx_TARG_X2,        "Tr2", SID_MOD_TARG_NUM, PRINT_MODx_TAR, EXEC_SELPAR, R2PP2R_MODx
	CS_MENU_ENTRY	SID_Ix_MODx_DEPTH,          "Dep", 0x0ff, PRINT_MODx_PMDEC8,EXEC_SELPAR,     R2PP2R_MODx
	CS_MENU_ENTRY	SID_Ix_MODx_OP,             "Iv1", 0x001, PRINT_MODx_INV1,  EXEC_TOGPAR,     R2PP2R_MODx_INV1
	CS_MENU_ENTRY	SID_Ix_MODx_OP,             "Iv2", 0x001, PRINT_MODx_INV2,  EXEC_TOGPAR,     R2PP2R_MODx_INV2
	CS_MENU_ENTRY	0,                          "Pt1", 0x003, PRINT_MODx_Nx,    EXEC_TOGPAR,     R2PP2R_MODx_Nx
	CS_MENU_ENTRY	1,                          "Pt2", 0x003, PRINT_MODx_Nx,    EXEC_TOGPAR,     R2PP2R_MODx_Nx
	CS_MENU_ENTRY	2,                          "Pt3", 0x003, PRINT_MODx_Nx,    EXEC_TOGPAR,     R2PP2R_MODx_Nx
	CS_MENU_ENTRY	3,                          "PW1", 0x003, PRINT_MODx_Nx,    EXEC_TOGPAR,     R2PP2R_MODx_Nx
	CS_MENU_ENTRY	4,                          "PW2", 0x003, PRINT_MODx_Nx,    EXEC_TOGPAR,     R2PP2R_MODx_Nx
	CS_MENU_ENTRY	5,                          "PW3", 0x003, PRINT_MODx_Nx,    EXEC_TOGPAR,     R2PP2R_MODx_Nx
	CS_MENU_ENTRY	6,                          "Fil", 0x003, PRINT_MODx_Nx,    EXEC_TOGPAR,     R2PP2R_MODx_Nx
	CS_MENU_ENTRY	7,                          "Vol", 0x003, PRINT_MODx_Nx,    EXEC_TOGPAR,     R2PP2R_MODx_Nx
CS_MENU_TABLE_L_MOD_End

; ==========================================================================
;  The trigger matrix menu
; ==========================================================================
CS_MENU_TABLE_L_TRG
	db	(CS_MENU_TABLE_L_TRG_End-CS_MENU_TABLE_L_TRG)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_TRG

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_TRG,       "TRG", 0x00d, PRINT_TRGx_SRC,   EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	0,                          "O1L", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	1,                          "O2L", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	2,                          "O3L", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	3,                          "O1R", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	4,                          "O2R", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	5,                          "O3R", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	6,                          "E1A", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	7,                          "E2A", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	8,                          "E1R", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	9,                          "E2R", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	10,                         "L1 ", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	11,                         "L2 ", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	12,                         "L3 ", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	13,                         "L4 ", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	14,                         "L5 ", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	15,                         "L6 ", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	16,                         "W1R", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	17,                         "W2R", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	18,                         "W3R", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	19,                         "W4R", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	20,                         "W1S", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	21,                         "W2S", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	22,                         "W3S", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
	CS_MENU_ENTRY	23,                         "W4S", 0x001, PRINT_TRGx_Nx,    EXEC_TOGPAR,     R2PP2R_TRGx_Nx
CS_MENU_TABLE_L_TRG_End


; ==========================================================================
;  The configuration menu
; ==========================================================================
CS_MENU_TABLE_L_CFG
	db	(CS_MENU_TABLE_L_CFG_End-CS_MENU_TABLE_L_CFG)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_CFG

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	SID_Ix_L_VOLUME,            "Vol", 0x07f, PRINT_DEC8,       EXEC_SELPAR,     R2PP2R_PAR8
	CS_MENU_ENTRY	SID_Ix_L_FLAGS1,            "Leg", 0x001, PRINT_B0,         EXEC_TOGPAR,     R2PP2R_B0
	CS_MENU_ENTRY	SID_Ix_L_FLAGS1,            "WTO", 0x001, PRINT_B1,         EXEC_TOGPAR,     R2PP2R_B1
	CS_MENU_ENTRY	SID_Ix_L_FLAGS1,            "SuK", 0x001, PRINT_B2,         EXEC_TOGPAR,     R2PP2R_B2
	CS_MENU_ENTRY	SID_Ix_OPT1_FLAGS,          "ABW", 0x001, PRINT_B0,         EXEC_TOGPAR,     R2PP2R_B0
CS_MENU_TABLE_L_CFG_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_M_CFG
	db	(CS_MENU_TABLE_M_CFG_End-CS_MENU_TABLE_M_CFG)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_CFG

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_INS,       "Ins", 0x005, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_M_VOLUME,            "Vol", 0x07f, PRINT_DEC8,       EXEC_SELPAR,     R2PP2R_PAR8
	CS_MENU_ENTRY	SID_Ix_M_Vx_VOICE_ASSGN,    "VAs", 0x008, PRINT_M_Vx_VAS,   EXEC_SELPAR,     R2PP2R_M_Vx_VAS
	CS_MENU_ENTRY	SID_Ix_M_Vx_FLAGS2,         "Pol", 0x001, PRINT_M_Vx_POL,   EXEC_TOGPAR,     R2PP2R_M_Vx_POL
	CS_MENU_ENTRY	SID_Ix_M_Vx_FLAGS2,         "Leg", 0x001, PRINT_BM_Vx_LEG,  EXEC_TOGPAR,     R2PP2R_BM_Vx_LEG
	CS_MENU_ENTRY	SID_Ix_M_Vx_FLAGS2,         "WTO", 0x001, PRINT_M_Vx_WTO,   EXEC_TOGPAR,     R2PP2R_M_Vx_WTO
	CS_MENU_ENTRY	SID_Ix_M_Vx_FLAGS2,         "SuK", 0x001, PRINT_BM_Vx_SUK,  EXEC_TOGPAR,     R2PP2R_BM_Vx_SUK
	CS_MENU_ENTRY	SID_Ix_M_Vx_VELOCITY_ASSGN, "VeA", 0x0ff, PRINT_M_D_Vx_ASG, EXEC_SELPAR_ASG, R2PP2R_M_D_Vx_ASG
	CS_MENU_ENTRY	SID_Ix_M_Vx_PITCHBENDER_ASSGN,"PbA", 0x0ff, PRINT_M_D_Vx_ASG, EXEC_SELPAR_ASG, R2PP2R_M_D_Vx_ASG
	CS_MENU_ENTRY	SID_Ix_OPT1_FLAGS,          "ABW", 0x001, PRINT_B0,         EXEC_TOGPAR,     R2PP2R_B0
CS_MENU_TABLE_M_CFG_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_B_CFG
CS_MENU_TABLE_D_CFG
	db	(CS_MENU_TABLE_B_CFG_End-CS_MENU_TABLE_B_CFG)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_CFG

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_SID_LR,    "ACh", 0x002, PRINT_VAR_LR,     EXEC_TOGPAR,     R2PP2R_VAR_SIDLR
	CS_MENU_ENTRY	SID_Ix_B_VOLUME,            "Vol", 0x07f, PRINT_DEC8,       EXEC_SELPAR,     R2PP2R_PAR8
	CS_MENU_ENTRY	SID_Ix_B_Vx_FLAGS2,         "Leg", 0x001, PRINT_BM_Vx_LEG,  EXEC_TOGPAR,     R2PP2R_BM_Vx_LEG
	CS_MENU_ENTRY	SID_Ix_B_Vx_FLAGS2,         "SuK", 0x001, PRINT_BM_Vx_SUK,  EXEC_TOGPAR,     R2PP2R_BM_Vx_SUK
	CS_MENU_ENTRY	SID_Ix_OPT1_FLAGS,          "ABW", 0x001, PRINT_B0,         EXEC_TOGPAR,     R2PP2R_B0
CS_MENU_TABLE_B_CFG_End

; ==========================================================================
;  The arpeggiator menu
; ==========================================================================
CS_MENU_TABLE_L_ARP
	db	(CS_MENU_TABLE_L_ARP_End-CS_MENU_TABLE_L_ARP)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_ARP

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_OSC_FLAGS, "OSC", 0x006, PRINT_VAR_123,    EXEC_SELPAR,     R2PP2R_VAR_OSC
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "On ", 0x001, PRINT_Vx_ARP_ON,  EXEC_TOGPAR,     R2PP2R_Vx_ARP_ON
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "Dir", 0x006, PRINT_Vx_ARP_DIR, EXEC_SELPAR,     R2PP2R_Vx_ARP_DIR
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "Sor", 0x001, PRINT_Vx_ARP_SOR, EXEC_TOGPAR,     R2PP2R_Vx_ARP_SOR
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "Hld", 0x001, PRINT_Vx_ARP_HLD, EXEC_TOGPAR,     R2PP2R_Vx_ARP_HLD
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_SPEED_DIV,    "Spd", 0x03f, PRINT_Vx_ARP_SPD, EXEC_SELPAR,     R2PP2R_Vx_ARP_SPD
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_GL_RNG,       "GLn", 0x01f, PRINT_Vx_ARP_GLN, EXEC_SELPAR,     R2PP2R_Vx_ARP_GLN
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_GL_RNG,       "Rng", 0x007, PRINT_Vx_ARP_RNG, EXEC_SELPAR,     R2PP2R_Vx_ARP_RNG
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "Snc", 0x001, PRINT_Vx_ARP_SNC, EXEC_TOGPAR,     R2PP2R_Vx_ARP_SNC
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "CAC", 0x001, PRINT_Vx_ARP_CAC, EXEC_TOGPAR,     R2PP2R_Vx_ARP_CAC
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_SPEED_DIV,    "One", 0x001, PRINT_Vx_ARP_ONE, EXEC_TOGPAR,     R2PP2R_Vx_ARP_ONE
CS_MENU_TABLE_L_ARP_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_M_ARP
	db	(CS_MENU_TABLE_M_ARP_End-CS_MENU_TABLE_M_ARP)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_ARP

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_INS,       "Ins", 0x005, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "On ", 0x001, PRINT_Vx_ARP_ON,  EXEC_TOGPAR,     R2PP2R_Vx_ARP_ON
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "Dir", 0x006, PRINT_Vx_ARP_DIR, EXEC_SELPAR,     R2PP2R_Vx_ARP_DIR
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "Sor", 0x001, PRINT_Vx_ARP_SOR, EXEC_TOGPAR,     R2PP2R_Vx_ARP_SOR
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "Hld", 0x001, PRINT_Vx_ARP_HLD, EXEC_TOGPAR,     R2PP2R_Vx_ARP_HLD
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_SPEED_DIV,    "Spd", 0x03f, PRINT_Vx_ARP_SPD, EXEC_SELPAR,     R2PP2R_Vx_ARP_SPD
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_GL_RNG,       "GLn", 0x01f, PRINT_Vx_ARP_GLN, EXEC_SELPAR,     R2PP2R_Vx_ARP_GLN
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_GL_RNG,       "Rng", 0x007, PRINT_Vx_ARP_RNG, EXEC_SELPAR,     R2PP2R_Vx_ARP_RNG
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "Snc", 0x001, PRINT_Vx_ARP_SNC, EXEC_TOGPAR,     R2PP2R_Vx_ARP_SNC
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "CAC", 0x001, PRINT_Vx_ARP_CAC, EXEC_TOGPAR,     R2PP2R_Vx_ARP_CAC
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_SPEED_DIV,    "One", 0x001, PRINT_Vx_ARP_ONE, EXEC_TOGPAR,     R2PP2R_Vx_ARP_ONE
CS_MENU_TABLE_M_ARP_End


; --------------------------------------------------------------------------
CS_MENU_TABLE_B_ARP
	db	(CS_MENU_TABLE_B_ARP_End-CS_MENU_TABLE_B_ARP)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_ARP

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_SID_LR,    "ACh", 0x002, PRINT_VAR_LR,     EXEC_TOGPAR,     R2PP2R_VAR_SIDLR
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "On ", 0x001, PRINT_Vx_ARP_ON,  EXEC_TOGPAR,     R2PP2R_Vx_ARP_ON
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "Dir", 0x006, PRINT_Vx_ARP_DIR, EXEC_SELPAR,     R2PP2R_Vx_ARP_DIR
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "Sor", 0x001, PRINT_Vx_ARP_SOR, EXEC_TOGPAR,     R2PP2R_Vx_ARP_SOR
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "Hld", 0x001, PRINT_Vx_ARP_HLD, EXEC_TOGPAR,     R2PP2R_Vx_ARP_HLD
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_SPEED_DIV,    "Spd", 0x03f, PRINT_Vx_ARP_SPD, EXEC_SELPAR,     R2PP2R_Vx_ARP_SPD
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_GL_RNG,       "GLn", 0x01f, PRINT_Vx_ARP_GLN, EXEC_SELPAR,     R2PP2R_Vx_ARP_GLN
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_GL_RNG,       "Rng", 0x007, PRINT_Vx_ARP_RNG, EXEC_SELPAR,     R2PP2R_Vx_ARP_RNG
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "Snc", 0x001, PRINT_Vx_ARP_SNC, EXEC_TOGPAR,     R2PP2R_Vx_ARP_SNC
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_MODE,         "CAC", 0x001, PRINT_Vx_ARP_CAC, EXEC_TOGPAR,     R2PP2R_Vx_ARP_CAC
	CS_MENU_ENTRY	SID_Ix_Vx_ARP_SPEED_DIV,    "One", 0x001, PRINT_Vx_ARP_ONE, EXEC_TOGPAR,     R2PP2R_Vx_ARP_ONE
CS_MENU_TABLE_B_ARP_End


; ==========================================================================
;  The KNB menu
; ==========================================================================
CS_MENU_TABLE_L_KNB
CS_MENU_TABLE_M_KNB
CS_MENU_TABLE_D_KNB
	db	(CS_MENU_TABLE_L_KNB_End-CS_MENU_TABLE_L_KNB)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_KNB

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	SID_KNOB_1,                 "K#1", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_2,                 "K#2", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_3,                 "K#3", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_4,                 "K#4", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_5,                 "K#5", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_VEL,               "K#V", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_PB,                "K#P", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_ATH,               "K#A", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	CS_MENU_SELECTED_KNOB,      "Knb", 0x007, PRINT_KNB,        EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_Px_ASSIGN1,          "Pa1", 0x0ff, PRINT_KNB_PAR,    EXEC_SELPAR_ASG, R2PP2R_KNB_ASG
	CS_MENU_ENTRY	SID_Ix_Px_ASSIGN2,          "Pa2", 0x0ff, PRINT_KNB_PAR,    EXEC_SELPAR_ASG, R2PP2R_KNB_ASG
	CS_MENU_ENTRY	SID_Ix_Px_MIN,              "Min", 0x0ff, PRINT_KNB_MINMAX, EXEC_SELPAR,     R2PP2R_KNB_P
	CS_MENU_ENTRY	SID_Ix_Px_MAX,              "Max", 0x0ff, PRINT_KNB_MINMAX, EXEC_SELPAR,     R2PP2R_KNB_P
CS_MENU_TABLE_L_KNB_End


CS_MENU_TABLE_B_KNB
	db	(CS_MENU_TABLE_B_KNB_End-CS_MENU_TABLE_B_KNB)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_KNB

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	SID_KNOB_1,                 "Cut", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_2,                 "Res", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_3,                 "EMd", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_4,                 "Dec", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_5,                 "Acc", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_VEL,               "K#V", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_PB,                "K#P", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	SID_KNOB_ATH,               "K#A", 0x0ff, PRINT_KNB_VALUE,  EXEC_SELPAR,     R2PP2R_KNB_VALUE
	CS_MENU_ENTRY	CS_MENU_SELECTED_KNOB,      "Knb", 0x007, PRINT_KNB,        EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_Px_ASSIGN1,          "Pa1", 0x0ff, PRINT_KNB_PAR,    EXEC_SELPAR_ASG, R2PP2R_KNB_ASG
	CS_MENU_ENTRY	SID_Ix_Px_ASSIGN2,          "Pa2", 0x0ff, PRINT_KNB_PAR,    EXEC_SELPAR_ASG, R2PP2R_KNB_ASG
	CS_MENU_ENTRY	SID_Ix_Px_MIN,              "Min", 0x0ff, PRINT_KNB_MINMAX, EXEC_SELPAR,     R2PP2R_KNB_P
	CS_MENU_ENTRY	SID_Ix_Px_MAX,              "Max", 0x0ff, PRINT_KNB_MINMAX, EXEC_SELPAR,     R2PP2R_KNB_P
CS_MENU_TABLE_B_KNB_End


; ==========================================================================
;  The EXT menu
; ==========================================================================
CS_MENU_TABLE_L_EXT
CS_MENU_TABLE_M_EXT
CS_MENU_TABLE_B_EXT
CS_MENU_TABLE_D_EXT
	db	(CS_MENU_TABLE_L_EXT_End-CS_MENU_TABLE_L_EXT)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_EXT

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	SID_Ix_EXT_PAR1_L,          "A#1", 0xfff, PRINT_EXT_VALUE, EXEC_SELPAR,     R2PP2R_EXT_VALUE
	CS_MENU_ENTRY	SID_Ix_EXT_PAR2_L,          "A#2", 0xfff, PRINT_EXT_VALUE, EXEC_SELPAR,     R2PP2R_EXT_VALUE
	CS_MENU_ENTRY	SID_Ix_EXT_PAR3_L,          "A#3", 0xfff, PRINT_EXT_VALUE, EXEC_SELPAR,     R2PP2R_EXT_VALUE
	CS_MENU_ENTRY	SID_Ix_EXT_PAR4_L,          "A#4", 0xfff, PRINT_EXT_VALUE, EXEC_SELPAR,     R2PP2R_EXT_VALUE
	CS_MENU_ENTRY	SID_Ix_EXT_PAR5_L,          "A#5", 0xfff, PRINT_EXT_VALUE, EXEC_SELPAR,     R2PP2R_EXT_VALUE
	CS_MENU_ENTRY	SID_Ix_EXT_PAR6_L,          "A#6", 0xfff, PRINT_EXT_VALUE, EXEC_SELPAR,     R2PP2R_EXT_VALUE
	CS_MENU_ENTRY	SID_Ix_EXT_PAR7_L,          "A#7", 0xfff, PRINT_EXT_VALUE, EXEC_SELPAR,     R2PP2R_EXT_VALUE
	CS_MENU_ENTRY	SID_Ix_EXT_PAR8_L,          "A#8", 0xfff, PRINT_EXT_VALUE, EXEC_SELPAR,     R2PP2R_EXT_VALUE
	CS_MENU_ENTRY	0,                          "S#1", 0x001, PRINT_EXT_SWITCH,EXEC_TOGPAR,     R2PP2R_EXT_SWITCH
	CS_MENU_ENTRY	1,                          "S#2", 0x001, PRINT_EXT_SWITCH,EXEC_TOGPAR,     R2PP2R_EXT_SWITCH
	CS_MENU_ENTRY	2,                          "S#3", 0x001, PRINT_EXT_SWITCH,EXEC_TOGPAR,     R2PP2R_EXT_SWITCH
	CS_MENU_ENTRY	3,                          "S#4", 0x001, PRINT_EXT_SWITCH,EXEC_TOGPAR,     R2PP2R_EXT_SWITCH
	CS_MENU_ENTRY	4,                          "S#5", 0x001, PRINT_EXT_SWITCH,EXEC_TOGPAR,     R2PP2R_EXT_SWITCH
	CS_MENU_ENTRY	5,                          "S#6", 0x001, PRINT_EXT_SWITCH,EXEC_TOGPAR,     R2PP2R_EXT_SWITCH
	CS_MENU_ENTRY	6,                          "S#7", 0x001, PRINT_EXT_SWITCH,EXEC_TOGPAR,     R2PP2R_EXT_SWITCH
	CS_MENU_ENTRY	7,                          "S#8", 0x001, PRINT_EXT_SWITCH,EXEC_TOGPAR,     R2PP2R_EXT_SWITCH
CS_MENU_TABLE_L_EXT_End


; ==========================================================================
;  The wavetable configuration menu
; ==========================================================================
CS_MENU_TABLE_L_WTC
	db	(CS_MENU_TABLE_L_WTC_End-CS_MENU_TABLE_L_WTC)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_WTC

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_WT,        " WT", 0x003, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_WTx_ASSGN,           "Par", 0x0ff, PRINT_WTx_PAR,    EXEC_SELPAR_ASG, R2PP2R_WTx_PAR
	CS_MENU_ENTRY	SID_Ix_WTx_SPEED,           "SID", 0x003, PRINT_WTx_SID,    EXEC_TOGPAR,     R2PP2R_WTx_SID
	CS_MENU_ENTRY	SID_Ix_WTx_SPEED,           "Spd", 0x03f, PRINT_WTx_SPD,    EXEC_SELPAR,     R2PP2R_WTx_SPD
	CS_MENU_ENTRY	SID_Ix_WTx_BEGIN,           "MOD", 0x001, PRINT_WTx_B7,     EXEC_TOGPAR,     R2PP2R_WTx_B7
	CS_MENU_ENTRY	SID_Ix_WTx_BEGIN,           "Beg", 0x07f, PRINT_WTx_POS,    EXEC_SELPAR,     R2PP2R_WTx_POS
	CS_MENU_ENTRY	SID_Ix_WTx_END,             "End", 0x07f, PRINT_WTx_POS,    EXEC_SELPAR,     R2PP2R_WTx_POS
	CS_MENU_ENTRY	SID_Ix_WTx_LOOP,            "Lop", 0x080, PRINT_WTx_LOP,    EXEC_SELPAR,     R2PP2R_WTx_LOP
	CS_MENU_ENTRY	CS_STAT2,                   "Val", 0x001, PRINT_WT_VAL,     EXEC_TOGPAR,     R2PP2R_WT_VAL
	CS_MENU_ENTRY	CS_MENU_L_WTE,              "WTE", 0x000, PRINT_WT_GO,      EXEC_MENU,       R2PP2R_NOP
CS_MENU_TABLE_L_WTC_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_M_WTC
	db	(CS_MENU_TABLE_M_WTC_End-CS_MENU_TABLE_M_WTC)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_WTC

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_INS,       "Ins", 0x005, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_M_Vx_WT_ASSGN,       "Par", 0x0ff, PRINT_WTx_PAR,    EXEC_SELPAR_ASG, R2PP2R_WTx_PAR
	CS_MENU_ENTRY	SID_Ix_M_Vx_WT_SPEED,       "Spd", 0x03f, PRINT_WTx_SPD,    EXEC_SELPAR,     R2PP2R_WTx_SPD
	CS_MENU_ENTRY	SID_Ix_M_Vx_WT_BEGIN,       "Beg", 0x07f, PRINT_WTx_POS,    EXEC_SELPAR,     R2PP2R_WTx_POS
	CS_MENU_ENTRY	SID_Ix_M_Vx_WT_END,         "End", 0x07f, PRINT_WTx_POS,    EXEC_SELPAR,     R2PP2R_WTx_POS
	CS_MENU_ENTRY	SID_Ix_M_Vx_WT_LOOP,        "Lop", 0x080, PRINT_WTx_LOP,    EXEC_SELPAR,     R2PP2R_WTx_LOP
	CS_MENU_ENTRY	CS_STAT2,                   "Val", 0x001, PRINT_WT_VAL,     EXEC_TOGPAR,     R2PP2R_WT_VAL
	CS_MENU_ENTRY	CS_MENU_M_WTE,              "WTE", 0x000, PRINT_WT_GO,      EXEC_MENU,       R2PP2R_NOP
CS_MENU_TABLE_M_WTC_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_B_SQC
	db	(CS_MENU_TABLE_B_SQC_End-CS_MENU_TABLE_B_SQC)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_SQC

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_SID_LR,    "ACh", 0x002, PRINT_VAR_LR,     EXEC_TOGPAR,     R2PP2R_VAR_SIDLR
	CS_MENU_ENTRY	SID_Ix_B_Vx_FLAGS2,         "On ", 0x001, PRINT_M_Vx_WTO,   EXEC_TOGPAR,     R2PP2R_B_Vx_MODE
	CS_MENU_ENTRY	SID_Ix_B_Vx_SEQ_NUM,        "Seq", 0x008, PRINT_SEQ_NUM,    EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_B_Vx_SEQ_SPEED,      "Spd", 0x03f, PRINT_WTx_SPD,    EXEC_SELPAR,     R2PP2R_B_Vx_SEQ_SPD
	CS_MENU_ENTRY	SID_Ix_B_Vx_SEQ_LENGTH,     "Len", 0x00f, PRINT_Vx_DECP1,   EXEC_SELPAR,     R2PP2R_Vx
	CS_MENU_ENTRY	SID_Ix_B_Vx_SEQ_SPEED,      "S16", 0x001, PRINT_SEQ_S16,    EXEC_TOGPAR,     R2PP2R_B_Vx_SEQ_S16
	CS_MENU_ENTRY	SID_Ix_B_Vx_SEQ_ASSGN,      "Par", 0x0ff, PRINT_WTx_PAR,    EXEC_SELPAR_ASG, R2PP2R_WTx_PAR
	CS_MENU_ENTRY	CS_MENU_B_SEQ,              "SEQ", 0x000, PRINT_WT_GO,      EXEC_MENU,       R2PP2R_NOP
CS_MENU_TABLE_B_SQC_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_D_SQC
	db	(CS_MENU_TABLE_D_SQC_End-CS_MENU_TABLE_D_SQC)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_SQC

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	SID_Ix_D_SEQ_SPEED,         "On ", 0x001, PRINT_D_SEQ_ON,   EXEC_TOGPAR,     R2PP2R_WTx_B6
	CS_MENU_ENTRY	SID_Ix_D_SEQ_NUM,           "Seq", 0x008, PRINT_SEQ_NUM,    EXEC_SELPAR,     R2PP2R_WTx_PAR
	CS_MENU_ENTRY	SID_Ix_D_SEQ_SPEED,         "Spd", 0x03f, PRINT_WTx_SPD,    EXEC_SELPAR,     R2PP2R_WTx_SPD
	CS_MENU_ENTRY	SID_Ix_D_SEQ_LENGTH,        "Len", 0x00f, PRINT_WTx_PAR_P1, EXEC_SELPAR_ASG, R2PP2R_WTx_PAR
	CS_MENU_ENTRY	SID_Ix_D_SEQ_SPEED,         "S16", 0x001, PRINT_SEQ_S16,    EXEC_TOGPAR,     R2PP2R_WTx_B7
	CS_MENU_ENTRY	CS_MENU_D_SEQ,              "SEQ", 0x000, PRINT_WT_GO,      EXEC_MENU,       R2PP2R_NOP
CS_MENU_TABLE_D_SQC_End


; ==========================================================================
;  The wavetable editor
; ==========================================================================
CS_MENU_TABLE_L_WTE
	db	(CS_MENU_TABLE_L_WTE_End-CS_MENU_TABLE_L_WTE)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_WTE

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_WT_POS,             "Pos", 0x080, PRINT_WTE_POS,   EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	0,                          "$!!", 0x0ff, PRINT_WTE_PAR,   EXEC_SELPAR,     R2PP2R_WTE_PAR
	CS_MENU_ENTRY	1,                          "$!!", 0x0ff, PRINT_WTE_PAR,   EXEC_SELPAR,     R2PP2R_WTE_PAR
	CS_MENU_ENTRY	2,                          "$!!", 0x0ff, PRINT_WTE_PAR,   EXEC_SELPAR,     R2PP2R_WTE_PAR
	CS_MENU_ENTRY	3,                          "$!!", 0x0ff, PRINT_WTE_PAR,   EXEC_SELPAR,     R2PP2R_WTE_PAR
	CS_MENU_ENTRY	CS_STAT2,                   "Val", 0x001, PRINT_WT_VAL,    EXEC_TOGPAR,     R2PP2R_WT_VAL
	CS_MENU_ENTRY	CS_MENU_L_WTC,              "WTC", 0x000, PRINT_WT_GO,     EXEC_MENU,       R2PP2R_NOP
CS_MENU_TABLE_L_WTE_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_M_WTE
	db	(CS_MENU_TABLE_M_WTE_End-CS_MENU_TABLE_M_WTE)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_WTE

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_WT_POS,             "Pos", 0x080, PRINT_WTE_POS,   EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	0,                          "$!!", 0x0ff, PRINT_WTE_PAR,   EXEC_SELPAR,     R2PP2R_WTE_PAR
	CS_MENU_ENTRY	1,                          "$!!", 0x0ff, PRINT_WTE_PAR,   EXEC_SELPAR,     R2PP2R_WTE_PAR
	CS_MENU_ENTRY	2,                          "$!!", 0x0ff, PRINT_WTE_PAR,   EXEC_SELPAR,     R2PP2R_WTE_PAR
	CS_MENU_ENTRY	3,                          "$!!", 0x0ff, PRINT_WTE_PAR,   EXEC_SELPAR,     R2PP2R_WTE_PAR
	CS_MENU_ENTRY	4,                          "$!!", 0x0ff, PRINT_WTE_PAR,   EXEC_SELPAR,     R2PP2R_WTE_PAR
	CS_MENU_ENTRY	5,                          "$!!", 0x0ff, PRINT_WTE_PAR,   EXEC_SELPAR,     R2PP2R_WTE_PAR
	CS_MENU_ENTRY	CS_STAT2,                   "Val", 0x001, PRINT_WT_VAL,    EXEC_TOGPAR,     R2PP2R_WT_VAL
	CS_MENU_ENTRY	CS_MENU_M_WTC,              "WTC", 0x000, PRINT_WT_GO,     EXEC_MENU,       R2PP2R_NOP
CS_MENU_TABLE_M_WTE_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_B_SEQ
	db	(CS_MENU_TABLE_B_SEQ_End-CS_MENU_TABLE_B_SEQ)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_SEQ

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_WT_SEQ,             "Seq", 0x007, PRINT_VAR_DECP1, EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	CS_MENU_WT_POS,             "Pos", 0x010, PRINT_SEQ_POS,   EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	0,                          "Nte", 0x00c, PRINT_B_SEQ_VAL, EXEC_SELPAR,     R2PP2R_B_SEQ_VAL
	CS_MENU_ENTRY	1,                          "Oct", 0x003, PRINT_B_SEQ_VAL, EXEC_TOGPAR,     R2PP2R_B_SEQ_VAL
	CS_MENU_ENTRY	2,                          "S/G", 0x003, PRINT_B_SEQ_VAL, EXEC_TOGPAR,     R2PP2R_B_SEQ_VAL
	CS_MENU_ENTRY	3,                          "Acc", 0x001, PRINT_B_SEQ_VAL, EXEC_TOGPAR,     R2PP2R_B_SEQ_VAL
	CS_MENU_ENTRY	4,                          "Par", 0x07f, PRINT_B_SEQ_VAL, EXEC_SELPAR,     R2PP2R_B_SEQ_VAL
	CS_MENU_ENTRY	0,                          "Cpy", 0x000, PRINT_NOP,       EXEC_SEQ_CPY,    R2PP2R_NOP
	CS_MENU_ENTRY	0,                          "Pst", 0x000, PRINT_NOP,       EXEC_SEQ_PST,    R2PP2R_NOP
	CS_MENU_ENTRY	0,                          "Clr", 0x000, PRINT_NOP,       EXEC_SEQ_CLR,    R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_B_SQC,              "SQC", 0x000, PRINT_WT_GO,     EXEC_MENU,       R2PP2R_NOP
CS_MENU_TABLE_B_SEQ_End

; --------------------------------------------------------------------------
CS_MENU_TABLE_D_SEQ
	db	(CS_MENU_TABLE_D_SEQ_End-CS_MENU_TABLE_D_SEQ)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_SEQ

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_WT_SEQ,             "Seq", 0x007, PRINT_VAR_DECP1, EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	CS_MENU_SELECTED_TRK,       "Trk", 0x007, PRINT_VAR_DECP1, EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	CS_MENU_NAME_POS,           "EDT", 0x00f, PRINT_WT_GO,     EXEC_EDIT_SEQ,   R2PP2R_EDIT_SEQ
	CS_MENU_ENTRY	0,                          "Cpy", 0x000, PRINT_NOP,       EXEC_SEQ_CPY,    R2PP2R_NOP
	CS_MENU_ENTRY	0,                          "Pst", 0x000, PRINT_NOP,       EXEC_SEQ_PST,    R2PP2R_NOP
	CS_MENU_ENTRY	0,                          "Clr", 0x000, PRINT_NOP,       EXEC_SEQ_CLR,    R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_D_SQC,              "SQC", 0x000, PRINT_WT_GO,     EXEC_MENU,       R2PP2R_NOP
CS_MENU_TABLE_D_SEQ_End


; ==========================================================================
;  The random generator menu
; ==========================================================================
CS_MENU_TABLE_L_RND
	db	(CS_MENU_TABLE_L_RND_End-CS_MENU_TABLE_L_RND)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_RND

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	0x00,                       "All", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x01,                       "Osc", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x02,                       "Fil", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x03,                       "LFO", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x04,                       "ENV", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x05,                       "Mod", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x06,                       "Trg", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x07,                       "WTC", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x08,                       "WTE", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x09,                       "Knb", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x0a,                       "Ext", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
CS_MENU_TABLE_L_RND_End

CS_MENU_TABLE_M_RND
	db	(CS_MENU_TABLE_M_RND_End-CS_MENU_TABLE_M_RND)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_RND

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	0x00,                       "All", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x01,                       "Osc", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x02,                       "Fil", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x03,                       "LFO", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x04,                       "ENV", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x07,                       "WTC", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x08,                       "WTE", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x09,                       "Knb", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x0a,                       "Ext", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
CS_MENU_TABLE_M_RND_End


CS_MENU_TABLE_B_RND
	db	(CS_MENU_TABLE_B_RND_End-CS_MENU_TABLE_B_RND)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_RND

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	0x00,                       "All", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x0b,                       "Nte", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x0c,                       "Gte", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x0d,                       "Sld", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x0e,                       "Acc", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x0f,                       "Par", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x01,                       "Osc", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x02,                       "Fil", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x03,                       "LFO", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x04,                       "ENV", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x09,                       "Knb", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x0a,                       "Ext", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
CS_MENU_TABLE_B_RND_End


CS_MENU_TABLE_D_RND
	db	(CS_MENU_TABLE_D_RND_End-CS_MENU_TABLE_D_RND)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_RND

	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	0x00,                       "All", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x10,                       "Seq", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x01,                       "Drm", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x02,                       "Fil", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x09,                       "Knb", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x0a,                       "Ext", 0x000, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
CS_MENU_TABLE_D_RND_End


; ==========================================================================
;  The save menu
; ==========================================================================
CS_MENU_TABLE_L_SAV
CS_MENU_TABLE_B_SAV
CS_MENU_TABLE_D_SAV
CS_MENU_TABLE_M_SAV
	db	(CS_MENU_TABLE_L_SAV_End-CS_MENU_TABLE_L_SAV)/CS_MENU_ENTRY_LEN, CS_MENU_LABEL_SAVE

	;;              Register (00=dummy)         |<->|  max    print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SAVE_BANK,          "B# ", 0x007, PRINT_SAVE_BNK,   EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	CS_MENU_SAVE_PATCH,         "P# ", 0x07f, PRINT_SAVE_PAT,   EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	SID_Ix_NAME_x,              "do!", 0x07f, PRINT_SAVE_NAME,  EXEC_SAVE_PATCH, R2PP2R_SAVE_NAME
CS_MENU_TABLE_L_SAV_End
