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
#define CS_MENU_LABEL_INS	0x0c
#define CS_MENU_LABEL_FIL	0x0d
#define CS_MENU_LABEL_LFO	0x0e
#define CS_MENU_LABEL_ENV	0x0f
#define CS_MENU_LABEL_MOD	0x10
#define CS_MENU_LABEL_TRG	0x11
#define CS_MENU_LABEL_CFG	0x12
#define CS_MENU_LABEL_ARP	0x13
#define CS_MENU_LABEL_KNB	0x14
#define CS_MENU_LABEL_EXT	0x15
#define CS_MENU_LABEL_WTC	0x16
#define CS_MENU_LABEL_WTE	0x17
#define CS_MENU_LABEL_SQC	0x18
#define CS_MENU_LABEL_SEQ	0x19
#define CS_MENU_LABEL_RND	0x1a

	;; order of entries has to be aligned with definitions above!
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
	db	"Instrument  "	; 0x0c
	db	"Filter      "	; 0x0d
	db	"Low Frq. Osc"	; 0x0e
	db	"Envelope    "	; 0x0f
	db	"Modulation  "	; 0x10
	db	"Triggers    "	; 0x11
	db	"Config.     "	; 0x12
	db	"Arpeggiator "	; 0x13
	db	"Knobs       "	; 0x14
	db	"Externals   "	; 0x15
	db	"WT Config.  "	; 0x16
	db	"WT Editor   "	; 0x17
	db	"Seq. Config."	; 0x18
	db	"Sequencer   "	; 0x19
	db	"Randomizer  "	; 0x1a
