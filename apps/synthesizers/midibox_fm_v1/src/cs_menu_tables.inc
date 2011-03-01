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
	dw	CS_MENU_TABLE_ENS,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_INS,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_OP,   CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_LFO,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_EG,   CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_WT,   CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRootWT
	dw	CS_MENU_TABLE_DRUM, CS_MENU_Page_Drum,       CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_SAVE, CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_AOUT, CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRootAOUT
	dw	CS_MENU_TABLE_USER, CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot
	dw	CS_MENU_TABLE_RND,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToRoot

	dw	CS_MENU_TABLE_BD,   CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToDRUM
	dw	CS_MENU_TABLE_SD,   CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToDRUM
	dw	CS_MENU_TABLE_HH,   CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToDRUM
	dw	CS_MENU_TABLE_CYM,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToDRUM
	dw	CS_MENU_TABLE_TOM,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToDRUM
	dw	CS_MENU_TABLE_FRQ,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToDRUM
	dw	CS_MENU_TABLE_OUT,  CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToDRUM
	dw	CS_MENU_TABLE_DM,   CS_MENU_Page_Parameters, CS_MENU_EXEC_GoToDRUM


	;; ID's for all menu structures
CS_MENU_ROOT	EQU	0x00
CS_MENU_ENS	EQU	0x01
CS_MENU_INS	EQU	0x02
CS_MENU_OP	EQU	0x03
CS_MENU_LFO	EQU	0x04
CS_MENU_EG	EQU	0x05
CS_MENU_WT	EQU	0x06
CS_MENU_DRUM	EQU	0x07
CS_MENU_SAVE	EQU	0x08
CS_MENU_AOUT	EQU	0x09
CS_MENU_USER	EQU	0x0a
CS_MENU_RND	EQU	0x0b

CS_MENU_BD	EQU	0x0c
CS_MENU_SD	EQU	0x0d
CS_MENU_HH	EQU	0x0e
CS_MENU_CYM	EQU	0x0f
CS_MENU_TOM	EQU	0x10
CS_MENU_FRQ	EQU	0x11
CS_MENU_OUT	EQU	0x12
CS_MENU_DM	EQU	0x13


	;; ID for main page (7th bit must be set)
CS_MENU_MAIN	EQU	0x80
	

; ==========================================================================
;  The main menu
; ==========================================================================
CS_MENU_TABLE_ROOT
	db	(CS_MENU_TABLE_ROOT_End-CS_MENU_TABLE_ROOT)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_ENS,               "ENS ", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_INS,               "INS ", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_OP,                " OP ", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_LFO,               "LFO ", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_EG,                "EG5 ", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_WT,                " WT ", 0x00, PRINT_NOP,        EXEC_MENU_WT,    R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_DRUM,              "DRUM", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_SAVE,              "SAVE", 0x00, PRINT_NOP,        EXEC_MENU_SAVE,  R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_AOUT,              "AOUT", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_USER,              "USER", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
#if ENABLE_RANDOM_GENERATOR
	CS_MENU_ENTRY	CS_MENU_RND,               "RND ", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
#endif
CS_MENU_TABLE_ROOT_End

; ==========================================================================
;  The ensemble menu
; ==========================================================================
CS_MENU_TABLE_ENS
	db	(CS_MENU_TABLE_ENS_End-CS_MENU_TABLE_ENS)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	MBFM_EBANK,                "EBnk", 0x07, PRINT_VAR_BANK,   EXEC_SELPAR,     R2PP2R_VAR_ENSINIT
	CS_MENU_ENTRY	MBFM_ENSEMBLE,             "Ens.", 0x1f, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR_ENSINIT
	CS_MENU_ENTRY	CS_MENU_SELECTED_INS_FLAGS,"Inst", 0x06, PRINT_VAR_1234,   EXEC_SELPAR,     R2PP2R_VAR_INS
	CS_MENU_ENTRY	MBFM_PEx_BANK,	           "IBnk", 0x07, PRINT_Px_BANK,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PEx_PATCH,	           "Ptch", 0x7f, PRINT_Px_DECP1,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PEx_MIDI_CHANNEL,	   "Chan", 0x0f, PRINT_Px_DECP1,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PEx_SPLIT_LOWER,	   "SPLw", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PEx_SPLIT_UPPER,	   "SPUp", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PEx_VOLUME,	   "Vol.", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PEx_CTRLE,		   "Vel.", 0x02, PRINT_Px_VEL,     EXEC_TOGPAR,     R2PP2R_Px_VEL
	CS_MENU_ENTRY	MBFM_PEx_TRANSPOSE,	   "Trns", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PEx_UNISONO,	   "Uni.", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PEx_ACHN_OP12,        "OP12", 0x0f, PRINT_Px_1234,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PEx_ACHN_OP34,        "OP34", 0x0f, PRINT_Px_1234,    EXEC_SELPAR,     R2PP2R_Px
CS_MENU_TABLE_ENS_End

; ==========================================================================
;  The instrument menu
; ==========================================================================
CS_MENU_TABLE_INS
	db	(CS_MENU_TABLE_INS_End-CS_MENU_TABLE_INS)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_INS_FLAGS,"Inst", 0x06, PRINT_VAR_1234,   EXEC_SELPAR,     R2PP2R_VAR_INS
	CS_MENU_ENTRY	MBFM_Px_CTRL1_L,	   "Play", 0x02, PRINT_Px_PLAY,    EXEC_TOGPAR,     R2PP2R_PLAY
	CS_MENU_ENTRY	MBFM_Px_CON,		   "Con.", 0x03, PRINT_Px_CON,     EXEC_TOGPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_FEEDBACK,	   "Fdbk", 0x07, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_PITCHRANGE,	   "PRng", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_FINETUNE,	   "Fine", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_PORTA_RATE,	   "Port", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_CTRL1_L,	   "SusK", 0x01, PRINT_Px_SUSK,    EXEC_TOGPAR,     R2PP2R_SUSK
	CS_MENU_ENTRY	MBFM_Px_CTRL2_L,	   "MSyn", 0x07, PRINT_Px_MSYN,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_VELOCITY_ASSIGN,   "VCC ", 0x7f, PRINT_Px_CCASG,   EXEC_SELPAR_ASG, R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_VELOCITY_INIT,     "VIni", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_VELOCITY_DEPTH,    "VDep", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_MODWHEEL_ASSIGN,   "MCC ", 0x7f, PRINT_Px_CCASG,   EXEC_SELPAR_ASG, R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_MODWHEEL_INIT,     "MIni", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_MODWHEEL_DEPTH,    "MDep", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_AFTERTOUCH_ASSIGN, "ACC ", 0x7f, PRINT_Px_CCASG,   EXEC_SELPAR_ASG, R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_AFTERTOUCH_INIT,   "AIni", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_AFTERTOUCH_DEPTH,  "ADep", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
CS_MENU_TABLE_INS_End

; ==========================================================================
;  The operator menu
; ==========================================================================
CS_MENU_TABLE_OP
	db	(CS_MENU_TABLE_OP_End-CS_MENU_TABLE_OP)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_OP_FLAGS, " OP ", 0x06, PRINT_VAR_1234,   EXEC_SELPAR,     R2PP2R_VAR_OP
	CS_MENU_ENTRY	MBFM_Px_OPx_WS,            "Wave", 0x07, PRINT_PxOx_WAVE,  EXEC_SELPAR,     R2PP2R_PxOx
	CS_MENU_ENTRY	MBFM_Px_OPx_TL,            "Vol.", 0x3f, PRINT_PxOx_DEC,   EXEC_SELPAR,     R2PP2R_PxOx
	CS_MENU_ENTRY	MBFM_Px_OPx_MULT,          "Mult", 0x0f, PRINT_PxOx_DEC,   EXEC_SELPAR,     R2PP2R_PxOx
	CS_MENU_ENTRY	MBFM_Px_OPx_AR,            "Atck", 0x0f, PRINT_PxOx_DEC,   EXEC_SELPAR,     R2PP2R_PxOx
	CS_MENU_ENTRY	MBFM_Px_OPx_DR,            "Dec.", 0x0f, PRINT_PxOx_DEC,   EXEC_SELPAR,     R2PP2R_PxOx
	CS_MENU_ENTRY	MBFM_Px_OPx_SL,            "Sust", 0x0f, PRINT_PxOx_DEC,   EXEC_SELPAR,     R2PP2R_PxOx
	CS_MENU_ENTRY	MBFM_Px_OPx_RR,            "Rel.", 0x0f, PRINT_PxOx_DEC,   EXEC_SELPAR,     R2PP2R_PxOx
	CS_MENU_ENTRY	MBFM_Px_OPx_FLAGS,         "Trem", 0x01, PRINT_PxOx_TREM,  EXEC_TOGPAR,     R2PP2R_PxOxTREM
	CS_MENU_ENTRY	MBFM_Px_OPx_FLAGS,         "Vibr", 0x01, PRINT_PxOx_VIBR,  EXEC_TOGPAR,     R2PP2R_PxOxVIBR
	CS_MENU_ENTRY	MBFM_Px_OPx_FLAGS,         "EGT ", 0x01, PRINT_PxOx_EGT,   EXEC_TOGPAR,     R2PP2R_PxOxEGT
	CS_MENU_ENTRY	MBFM_Px_OPx_FLAGS,         "KSR ", 0x01, PRINT_PxOx_KSR,   EXEC_TOGPAR,     R2PP2R_PxOxKSR
	CS_MENU_ENTRY	MBFM_Px_OPx_KSL,           "KSL ", 0x03, PRINT_PxOx_KSL,   EXEC_TOGPAR,     R2PP2R_PxOx
CS_MENU_TABLE_OP_End

; ==========================================================================
;  The LFO menu
; ==========================================================================
#if CS_SAMMICH_FM
CS_MENU_TABLE_LFO
	db	(CS_MENU_TABLE_LFO_End-CS_MENU_TABLE_LFO)/CS_MENU_ENTRY_LEN, 0x00
	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_LFO,      "LFO ", 0x01, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	MBFM_Px_LFOx_MODE,         "Wave", 0x05, PRINT_PxLFO_WAVE, EXEC_SELPAR,     R2PP2R_PxLFO_WAVE
	CS_MENU_ENTRY	MBFM_Px_LFOx_RATE,         "Rate", 0x7f, PRINT_PxLFO_DEC,  EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_PHASE,        "Phas", 0x7f, PRINT_PxLFO_DEC,  EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_MODE,         "Sync", 0x01, PRINT_PxLFO_SYNC, EXEC_TOGPAR,     R2PP2R_PxLFO_SYNC
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_P,      "DepP", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_L,      "DepL", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_A,      "DepA", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_V1,     "DVo1", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_V2,     "DVo2", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_V3,     "DVo3", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_V4,     "DVo4", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
CS_MENU_TABLE_LFO_End
#else
CS_MENU_TABLE_LFO
	db	(CS_MENU_TABLE_LFO_End-CS_MENU_TABLE_LFO)/CS_MENU_ENTRY_LEN, 0x00
	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_LFO,      "LFO ", 0x01, PRINT_VAR_DECP1,  EXEC_TOGPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	MBFM_Px_LFOx_MODE,         "Wave", 0x05, PRINT_PxLFO_WAVE, EXEC_SELPAR,     R2PP2R_PxLFO_WAVE
	CS_MENU_ENTRY	MBFM_Px_LFOx_MODE,         "Sync", 0x01, PRINT_PxLFO_SYNC, EXEC_TOGPAR,     R2PP2R_PxLFO_SYNC
	CS_MENU_ENTRY	MBFM_Px_LFOx_PHASE,        "Phas", 0x7f, PRINT_PxLFO_DEC,  EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_RATE,         "Rate", 0x7f, PRINT_PxLFO_DEC,  EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_P,      "DepP", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_V1,     "DVo1", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_V2,     "DVo2", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_V3,     "DVo3", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_V4,     "DVo4", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_L,      "DepL", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_A,      "DepA", 0x7f, PRINT_PxLFO_PMDEC,EXEC_SELPAR,     R2PP2R_PxLFO
CS_MENU_TABLE_LFO_End
#endif

; ==========================================================================
;  The envelope generator menu
; ==========================================================================
CS_MENU_TABLE_EG
	db	(CS_MENU_TABLE_EG_End-CS_MENU_TABLE_EG)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	MBFM_Px_EGx_ATTACK,        "Atck", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_ATTLVL,        "ALvl", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_DECAY1,        "Dec1", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_DECLVL,        "DLvl", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_DECAY2,        "Dec2", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_SUSTAIN,       "Sust", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_RELEASE,       "Rel.", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_CURVE,         "Curv", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_MODE,          "CAs ", 0x0f, PRINT_Px_CAS,     EXEC_SELPAR,     R2PP2R_Px_CAS
	CS_MENU_ENTRY	MBFM_Px_EGx_DEPTH_P,       "DepP", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_DEPTH_V1,      "DVo1", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_DEPTH_V2,      "DVo2", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_DEPTH_V3,      "DVo3", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_DEPTH_V4,      "DVo4", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_DEPTH_L,       "DepL", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_EGx_DEPTH_A,       "DepA", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
CS_MENU_TABLE_EG_End

; ==========================================================================
;  The wavetable menu
; ==========================================================================
CS_MENU_TABLE_WT
	db	(CS_MENU_TABLE_WT_End-CS_MENU_TABLE_WT)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_WT_STEP,           "Step", 0x1f+1,PRINT_VAR_WTSTEP, EXEC_SELPAR,     R2PP2R_VAR_WTSTEP
	CS_MENU_ENTRY	0x00,                      "Mode", 0x03, PRINT_VAR_WTMODE, EXEC_TOGPAR,     R2PP2R_VAR_WTPAR
	CS_MENU_ENTRY	0x01,                      " #1 ", 0xff, PRINT_VAR_WTPAR,  EXEC_SELPAR,     R2PP2R_VAR_WTPAR
	CS_MENU_ENTRY	0x02,                      " #2 ", 0xff, PRINT_VAR_WTPAR,  EXEC_SELPAR,     R2PP2R_VAR_WTPAR
	CS_MENU_ENTRY	0x03,                      " #3 ", 0xff, PRINT_VAR_WTPAR,  EXEC_SELPAR,     R2PP2R_VAR_WTPAR
	CS_MENU_ENTRY	MBFM_Px_WT_RATE,           "Rate", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_CTRL2_U,           "Sync", 0x02, PRINT_Px_WTSYNC,  EXEC_TOGPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_WT_PAR1,           "CC#1", 0x7f, PRINT_Px_CCASG,   EXEC_SELPAR_ASG, R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_WT_PAR2,           "CC#2", 0x7f, PRINT_Px_CCASG,   EXEC_SELPAR_ASG, R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_WT_PAR3,           "CC#3", 0x7f, PRINT_Px_CCASG,   EXEC_SELPAR_ASG, R2PP2R_Px
CS_MENU_TABLE_WT_End

; ==========================================================================
;  The drum submenu
; ==========================================================================
CS_MENU_TABLE_DRUM
	db	(CS_MENU_TABLE_DRUM_End-CS_MENU_TABLE_DRUM)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_BD,                " BD ", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_SD,                " SD ", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_TOM,               "TOM ", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_HH,                " HH ", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_CYM,               "CYM ", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_FRQ,               "FRQ ", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_OUT,               "OUT ", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
	CS_MENU_ENTRY	CS_MENU_DM,                "MIDI", 0x00, PRINT_NOP,        EXEC_MENU,       R2PP2R_NOP
CS_MENU_TABLE_DRUM_End

v; ==========================================================================
;  The save menu
; ==========================================================================
CS_MENU_TABLE_SAVE
	db	(CS_MENU_TABLE_SAVE_End-CS_MENU_TABLE_SAVE)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SAVE_TYPE,         "Type", 0x02, PRINT_SAVE_TYPE,  EXEC_TOGPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	0x00,                      "Bank", 0x07, PRINT_SAVE_BANK,  EXEC_SELPAR,     R2PP2R_SAVE_BANK
	CS_MENU_ENTRY	0x00,                      "Slot", 0x7f, PRINT_SAVE_SLOT,  EXEC_SELPAR,     R2PP2R_SAVE_SLOT
	CS_MENU_ENTRY	0x00,                      "DoIt", 0x7f, PRINT_SAVE_NAME,  EXEC_SAVE_IT,    R2PP2R_SAVE_NAME
CS_MENU_TABLE_SAVE_End


; ==========================================================================
;  The BD submenu
; ==========================================================================
CS_MENU_TABLE_BD
	db	(CS_MENU_TABLE_BD_End-CS_MENU_TABLE_BD)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_BD_HH,    " OP ", 0x01, PRINT_VAR_BD_OP,  EXEC_TOGPAR,     R2PP2R_VAR_BD_OP
	CS_MENU_ENTRY	MBFM_PDx_BD_M_TL,          "Vol ", 0x3f, PRINT_PDxBD_DEC,  EXEC_SELPAR,     R2PP2R_PDxBD
	CS_MENU_ENTRY	MBFM_PDx_BD_M_MULT,        "Mult", 0x0f, PRINT_PDxBD_DEC,  EXEC_SELPAR,     R2PP2R_PDxBD
	CS_MENU_ENTRY	MBFM_PDx_BD_M_WS,          "Wave", 0x07, PRINT_PDxBD_WAVE,  EXEC_SELPAR,     R2PP2R_PDxBD
	CS_MENU_ENTRY	MBFM_PDx_BD_M_AR,          "Atck", 0x0f, PRINT_PDxBD_DEC,  EXEC_SELPAR,     R2PP2R_PDxBD
	CS_MENU_ENTRY	MBFM_PDx_BD_M_DR,          "Dec.", 0x0f, PRINT_PDxBD_DEC,  EXEC_SELPAR,     R2PP2R_PDxBD
	CS_MENU_ENTRY	MBFM_PDx_BD_M_SL,          "Sust", 0x0f, PRINT_PDxBD_DEC,  EXEC_SELPAR,     R2PP2R_PDxBD
	CS_MENU_ENTRY	MBFM_PDx_BD_M_RR,          "Rel.", 0x0f, PRINT_PDxBD_DEC,  EXEC_SELPAR,     R2PP2R_PDxBD
	CS_MENU_ENTRY	MBFM_PDx_BD_FEEDBACK,      "Fdbk", 0x07, PRINT_PDxBD_DEC,  EXEC_SELPAR,     R2PP2R_PDxBD
	CS_MENU_ENTRY	MBFM_PDx_BD_CON,           "Con ", 0x01, PRINT_PDxBD_CON,  EXEC_TOGPAR,     R2PP2R_PDxBD
CS_MENU_TABLE_BD_End

; ==========================================================================
;  The SD submenu
; ==========================================================================
CS_MENU_TABLE_SD
	db	(CS_MENU_TABLE_SD_End-CS_MENU_TABLE_SD)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	MBFM_PDx_SD_TL,            "Vol ", 0x3f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_SD_MULT,          "Mult", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_SD_WS,            "Wave", 0x07, PRINT_Px_WAVE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_SD_AR,            "Atck", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_SD_DR,            "Dec.", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_SD_SL,            "Sust", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_SD_RR,            "Rel.", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
CS_MENU_TABLE_SD_End

; ==========================================================================
;  The TOM submenu
; ==========================================================================
CS_MENU_TABLE_TOM
	db	(CS_MENU_TABLE_TOM_End-CS_MENU_TABLE_TOM)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	MBFM_PDx_TOM_TL,           "Vol ", 0x3f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_TOM_MULT,         "Mult", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_TOM_WS,           "Wave", 0x07, PRINT_Px_WAVE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_TOM_AR,           "Atck", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_TOM_DR,           "Dec.", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_TOM_SL,           "Sust", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_TOM_RR,           "Rel.", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
CS_MENU_TABLE_TOM_End


; ==========================================================================
;  The HH submenu
; ==========================================================================
CS_MENU_TABLE_HH
	db	(CS_MENU_TABLE_HH_End-CS_MENU_TABLE_HH)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	CS_MENU_SELECTED_BD_HH,    " HH ", 0x01, PRINT_VAR_HH,     EXEC_TOGPAR,     R2PP2R_VAR_HH
	CS_MENU_ENTRY	MBFM_PDx_HH_O_TL,          "Vol ", 0x3f, PRINT_PDxHH_DEC,  EXEC_SELPAR,     R2PP2R_PDxHH
	CS_MENU_ENTRY	MBFM_PDx_HH_O_MULT,        "Mult", 0x0f, PRINT_PDxHH_DEC,  EXEC_SELPAR,     R2PP2R_PDxHH
	CS_MENU_ENTRY	MBFM_PDx_HH_O_WS,          "Wave", 0x07, PRINT_PDxHH_WAVE, EXEC_SELPAR,     R2PP2R_PDxHH
	CS_MENU_ENTRY	MBFM_PDx_HH_O_AR,          "Atck", 0x0f, PRINT_PDxHH_DEC,  EXEC_SELPAR,     R2PP2R_PDxHH
	CS_MENU_ENTRY	MBFM_PDx_HH_O_DR,          "Dec.", 0x0f, PRINT_PDxHH_DEC,  EXEC_SELPAR,     R2PP2R_PDxHH
	CS_MENU_ENTRY	MBFM_PDx_HH_O_SL,          "Sust", 0x0f, PRINT_PDxHH_DEC,  EXEC_SELPAR,     R2PP2R_PDxHH
	CS_MENU_ENTRY	MBFM_PDx_HH_O_RR,          "Rel.", 0x0f, PRINT_PDxHH_DEC,  EXEC_SELPAR,     R2PP2R_PDxHH
CS_MENU_TABLE_HH_End

; ==========================================================================
;  The CYM submenu
; ==========================================================================
CS_MENU_TABLE_CYM
	db	(CS_MENU_TABLE_CYM_End-CS_MENU_TABLE_CYM)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	MBFM_PDx_CYM_TL,           "Vol ", 0x3f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_CYM_MULT,         "Mult", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_CYM_WS,           "Wave", 0x07, PRINT_Px_WAVE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_CYM_AR,           "Atck", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_CYM_DR,           "Dec.", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_CYM_SL,           "Sust", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_CYM_RR,           "Rel.", 0x0f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
CS_MENU_TABLE_CYM_End


; ==========================================================================
;  The Frequence Generator submenu
; ==========================================================================
CS_MENU_TABLE_FRQ
	db	(CS_MENU_TABLE_FRQ_End-CS_MENU_TABLE_FRQ)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	MBFM_PDx_BD_FRQ,           "BD F", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_BD_FRQ_DECR,      "Decr", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_HH_FRQ,           "HH F", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_HH_FRQ_DECR,      "Decr", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_TOM_FRQ,          "TomF", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_TOM_FRQ_DECR,     "Decr", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
CS_MENU_TABLE_FRQ_End

; ==========================================================================
;  The Audio Out submenu
; ==========================================================================
CS_MENU_TABLE_OUT
	db	(CS_MENU_TABLE_OUT_End-CS_MENU_TABLE_OUT)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	MBFM_PDx_BD_ACHN,          " BD ", 0x0f, PRINT_Px_1234,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_SD_ACHN,          "S/HH", 0x0f, PRINT_Px_1234,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_TOM_ACHN,         "T/Cy", 0x0f, PRINT_Px_1234,    EXEC_SELPAR,     R2PP2R_Px
CS_MENU_TABLE_OUT_End

; ==========================================================================
;  The Drum MIDI submenu
; ==========================================================================
CS_MENU_TABLE_DM
	db	(CS_MENU_TABLE_DM_End-CS_MENU_TABLE_DM)/CS_MENU_ENTRY_LEN, 0x00

	;;              Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	0x00,                      "Chn.", 0x0f, PRINT_DRUM_CHN,   EXEC_SELPAR,     R2PP2R_DRUM_CHN
	CS_MENU_ENTRY	MBFM_PDx_BD_SPLIT_LOWER,   "BD L", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_BD_SPLIT_UPPER,   "BD U", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_SD_SPLIT_LOWER,   "SD L", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_SD_SPLIT_UPPER,   "SD U", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_HH_O_SPLIT_LOWER, "HHOL", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_HH_O_SPLIT_UPPER, "HHOU", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_HH_C_SPLIT_LOWER, "HHCL", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_HH_C_SPLIT_UPPER, "HHCU", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_TOM_SPLIT_LOWER,  "TomL", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_TOM_SPLIT_UPPER,  "TomU", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_CYM_SPLIT_LOWER,  "CymL", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_CYM_SPLIT_UPPER,  "CymU", 0x7f, PRINT_Px_NOTE,    EXEC_SELPAR,     R2PP2R_Px
CS_MENU_TABLE_DM_End


; ==========================================================================
;  The AOUT submenu
; ==========================================================================
CS_MENU_TABLE_AOUT
	db	(CS_MENU_TABLE_AOUT_End-CS_MENU_TABLE_AOUT)/CS_MENU_ENTRY_LEN, 0x00

	CS_MENU_ENTRY	CS_MENU_SELECTED_AOUT,     "AOUT", 0x07, PRINT_VAR_DECP1,  EXEC_SELPAR,     R2PP2R_VAR
	CS_MENU_ENTRY	MBFM_AOUTx_ASSIGN,         "Inst", 0x03, PRINT_AOUTxINST,  EXEC_TOGPAR,     R2PP2R_AOUTxINST
	CS_MENU_ENTRY	MBFM_AOUTx_ASSIGN,         "Assg", 0x07, PRINT_AOUTxASSG,  EXEC_SELPAR,     R2PP2R_AOUTxASSG
	CS_MENU_ENTRY	MBFM_AOUTx_OFFSET,         "Offs", 0x7f, PRINT_AOUTxOFFS,  EXEC_SELPAR,     R2PP2R_AOUTxOFFS
CS_MENU_TABLE_AOUT_End


; ==========================================================================
;  The User submenu which can be selected from the button matrix
;  Do your customizations here :-)
; ==========================================================================
CS_MENU_TABLE_USER
	db	(CS_MENU_TABLE_USER_End-CS_MENU_TABLE_USER)/CS_MENU_ENTRY_LEN, 0x00

	;; USER1        Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	MBFM_PEx_VOLUME,	   "I1Vl", 0x7f, PRINT_I1Px_DEC,   EXEC_SELPAR,     R2PP2R_I1Px
	CS_MENU_ENTRY	MBFM_PEx_VOLUME,	   "I2Vl", 0x7f, PRINT_I2Px_DEC,   EXEC_SELPAR,     R2PP2R_I2Px
	CS_MENU_ENTRY	MBFM_PEx_VOLUME,	   "I3Vl", 0x7f, PRINT_I3Px_DEC,   EXEC_SELPAR,     R2PP2R_I3Px
	CS_MENU_ENTRY	MBFM_PEx_VOLUME,	   "I4Vl", 0x7f, PRINT_I4Px_DEC,   EXEC_SELPAR,     R2PP2R_I4Px

	;; USER2        Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	MBFM_PEx_TRANSPOSE,	   "I1Tr", 0x7f, PRINT_I1Px_PMDEC, EXEC_SELPAR,     R2PP2R_I1Px
	CS_MENU_ENTRY	MBFM_PEx_TRANSPOSE,	   "I2Tr", 0x7f, PRINT_I2Px_PMDEC, EXEC_SELPAR,     R2PP2R_I2Px
	CS_MENU_ENTRY	MBFM_PEx_TRANSPOSE,	   "I3Tr", 0x7f, PRINT_I3Px_PMDEC, EXEC_SELPAR,     R2PP2R_I3Px
	CS_MENU_ENTRY	MBFM_PEx_TRANSPOSE,	   "I4Tr", 0x7f, PRINT_I4Px_PMDEC, EXEC_SELPAR,     R2PP2R_I4Px

	;; USER3        Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	MBFM_Px_LFOx_RATE+0,       "L1Rt", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_LFOx_RATE+16,      "L2Rt", 0x7f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_P+0,    "L1DP", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_Px_LFOx_DEPTH_P+16,   "L2DP", 0x7f, PRINT_Px_PMDEC,   EXEC_SELPAR,     R2PP2R_Px

	;; USER4        Register (00=dummy)        |<-->|  max   print ix,         exec ix          parameter transfer
	CS_MENU_ENTRY	MBFM_PDx_BD_C_TL,          "BD V", 0x3f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_SD_TL,            "SD V", 0x3f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_HH_O_TL,          "HHOV", 0x3f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
	CS_MENU_ENTRY	MBFM_PDx_HH_C_TL,          "HHCV", 0x3f, PRINT_Px_DEC,     EXEC_SELPAR,     R2PP2R_Px
CS_MENU_TABLE_USER_End

; ==========================================================================
;  The random generator menu
; ==========================================================================
CS_MENU_TABLE_RND
	db	(CS_MENU_TABLE_RND_End-CS_MENU_TABLE_RND)/CS_MENU_ENTRY_LEN, 0x00

#if ENABLE_RANDOM_GENERATOR
	;;              Register (00=dummy)         |<->|  max   print ix          exec ix          parameter transfer
	CS_MENU_ENTRY	0x00,                       "All ", 0x00, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x01,                       " OP ", 0x00, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x02,                       "LFO ", 0x00, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x03,                       "EG5 ", 0x00, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x04,                       "WTC ", 0x00, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
	CS_MENU_ENTRY	0x05,                       "WTE ", 0x00, PRINT_RND_GEN,   EXEC_RND,        R2PP2R_NOP
#endif
CS_MENU_TABLE_RND_End

	;; end of all tables - required for TBLPTRU check in cs_menu.inc
CS_MENU_TABLE_END
