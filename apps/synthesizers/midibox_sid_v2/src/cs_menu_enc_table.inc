; $Id$
;
; MIDIbox SID Control Surface Rotary Encoder assign tables
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

; ==========================================================================
;  In this table all encoder functions are listed
;  the number and the position of entries are static and cannot be changed
;  without a modification in cs_menu_enc.inc!
; ==========================================================================

CSENC_ENTRY MACRO menu, page_offset, cursor_pos
	db	menu, page_offset, cursor_pos, 0	; (fill to 4 bytes)
	ENDM

	;; CSENC_ENTRY parameters:
	;;    - the menu in which the variable is located (see cs_menu_tables.inc)
	;;    - the page offset (which menu item should be at the left border)
	;;    - the cursor position of the menu item

	;; if menu is 0xff, the encoder handler won't switch to a new menu, but it will
	;; modify the currently visible values
	;; the menu offset specifies the 1st, 2nd, 3rd, ... parameter (counted from 0)
	;; the cursor pos is ignored

	;; -----------------------------------------------------------------
	;; Lead Engine
	;; -----------------------------------------------------------------
CS_MENU_ENC_TABLE_L
	;; OSC "Env" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_L_OSC,	0x03,	0x03	; Delay
	CSENC_ENTRY	CS_MENU_L_OSC,	0x03,	0x04	; Attack
	CSENC_ENTRY	CS_MENU_L_OSC,	0x03,	0x05	; Decay
	CSENC_ENTRY	CS_MENU_L_OSC,	0x03,	0x06	; Sustain
#if CS_MENU_DISPLAYED_ITEMS == 4
	CSENC_ENTRY	CS_MENU_L_OSC,	0x04,	0x07	; Release
#else
	CSENC_ENTRY	CS_MENU_L_OSC,	0x03,	0x07	; Release
#endif

	;; OSC "Misc" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_L_OSC,	0x09,	0x09	; Transpose
	CSENC_ENTRY	CS_MENU_L_OSC,	0x09,	0x0a	; Finetune
	CSENC_ENTRY	CS_MENU_L_OSC,	0x09,	0x0b	; Portamento
	CSENC_ENTRY	CS_MENU_L_OSC,	0x09,	0x0c	; Oscillator Phase Offset
#if CS_MENU_DISPLAYED_ITEMS == 4
	CSENC_ENTRY	CS_MENU_L_OSC,	0x0a,	0x0d	; Pulsewidth
#else
	CSENC_ENTRY	CS_MENU_L_OSC,	0x09,	0x0d	; Pulsewidth
#endif

	;; OSC "Assign" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_L_KNB,	0x00,	0x00	; Knob #1
	CSENC_ENTRY	CS_MENU_L_KNB,	0x00,	0x01	; Knob #2
	CSENC_ENTRY	CS_MENU_L_KNB,	0x00,	0x02	; Knob #3
	CSENC_ENTRY	CS_MENU_L_KNB,	0x00,	0x03	; Knob #4
	CSENC_ENTRY	CS_MENU_L_KNB,	0x00,	0x04	; Knob #5

	;; LFO section
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_L_LFO,	0x02,	0x02	; Rate
	CSENC_ENTRY	CS_MENU_L_LFO,	0x02,	0x04	; Depth

	;; Filter section
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_L_FIL,	0x00,	0x01	; Cutoff
	CSENC_ENTRY	CS_MENU_L_FIL,	0x00,	0x02	; Resonance

	;; Env "Env" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_L_ENV,	0x01,	0x01	; Depth
	CSENC_ENTRY	CS_MENU_L_ENV,	0x02,	0x03	; Attack
	CSENC_ENTRY	CS_MENU_L_ENV,	0x06,	0x08	; Decay
	CSENC_ENTRY	CS_MENU_L_ENV,	0x09,	0x09	; Sustain
	CSENC_ENTRY	CS_MENU_L_ENV,	0x09,	0x0a	; Release

	;; Env "Assign" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	0xff,		0x00,	0x00	; Menu item #1
	CSENC_ENTRY	0xff,		0x01,	0x00	; Menu item #2
	CSENC_ENTRY	0xff,		0x02,	0x00	; Menu item #3
	CSENC_ENTRY	0xff,		0x03,	0x00	; Menu item #4
	CSENC_ENTRY	0xff,		0x04,	0x00	; Menu item #5

#if DEFAULT_MB6582_CS
	;; Env "Misc" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_L_ENV,	0x00,	0x02	; Delay
	CSENC_ENTRY	CS_MENU_L_ENV,	0x0d,	0x0d	; Curve Attack
	CSENC_ENTRY	CS_MENU_L_ENV,	0x0d,	0x0e	; Curve Decay
	CSENC_ENTRY	CS_MENU_L_ENV,	0x09,	0x09	; Sustain
	CSENC_ENTRY	CS_MENU_L_ENV,	0x0d,	0x0f	; Curve Release
#endif

	;; -----------------------------------------------------------------
	;; Bassline Engine
	;; -----------------------------------------------------------------
CS_MENU_ENC_TABLE_B
	;; OSC "303" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_B_OSC,	0x03,	0x03	; Delay
	CSENC_ENTRY	CS_MENU_B_OSC,	0x03,	0x04	; Attack
	CSENC_ENTRY	CS_MENU_B_OSC,	0x03,	0x05	; Decay
	CSENC_ENTRY	CS_MENU_B_OSC,	0x03,	0x06	; Sustain
#if CS_MENU_DISPLAYED_ITEMS == 4
	CSENC_ENTRY	CS_MENU_B_OSC,	0x04,	0x07	; Release
#else
	CSENC_ENTRY	CS_MENU_B_OSC,	0x03,	0x07	; Release
#endif

	;; OSC "Misc" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_B_OSC,	0x09,	0x09	; Transpose
	CSENC_ENTRY	CS_MENU_B_OSC,	0x09,	0x0a	; Finetune
	CSENC_ENTRY	CS_MENU_B_OSC,	0x09,	0x0b	; Portamento
	CSENC_ENTRY	CS_MENU_B_OSC,	0x09,	0x0c	; Oscillator Phase Offset
#if CS_MENU_DISPLAYED_ITEMS == 4
	CSENC_ENTRY	CS_MENU_B_OSC,	0x0a,	0x0d	; Pulsewidth
#else
	CSENC_ENTRY	CS_MENU_B_OSC,	0x09,	0x0d	; Pulsewidth
#endif

	;; OSC "Assign" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_B_KNB,	0x00,	0x00	; Knob #1
	CSENC_ENTRY	CS_MENU_B_KNB,	0x00,	0x01	; Knob #2
	CSENC_ENTRY	CS_MENU_B_KNB,	0x00,	0x02	; Knob #3
	CSENC_ENTRY	CS_MENU_B_KNB,	0x00,	0x03	; Knob #4
	CSENC_ENTRY	CS_MENU_B_KNB,	0x00,	0x04	; Knob #5

	;; LFO section
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_B_LFO,	0x03,	0x03	; Rate
	CSENC_ENTRY	CS_MENU_B_LFO,	0x03,	0x06	; Depth

	;; Filter section
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_B_FIL,	0x00,	0x01	; Cutoff
	CSENC_ENTRY	CS_MENU_B_FIL,	0x00,	0x02	; Resonance

	;; Env "Env" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_B_ENV,	0x00,	0x03	; Depth
	CSENC_ENTRY	CS_MENU_B_ENV,	0x04,	0x04	; Attack
	CSENC_ENTRY	CS_MENU_B_ENV,	0x04,	0x05	; Decay
	CSENC_ENTRY	CS_MENU_B_ENV,	0x04,	0x06	; Sustain
	CSENC_ENTRY	CS_MENU_B_ENV,	0x04,	0x07	; Release

	;; Env "Assign" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	0xff,		0x00,	0x00	; Menu item #1
	CSENC_ENTRY	0xff,		0x01,	0x00	; Menu item #2
	CSENC_ENTRY	0xff,		0x02,	0x00	; Menu item #3
	CSENC_ENTRY	0xff,		0x03,	0x00	; Menu item #4
	CSENC_ENTRY	0xff,		0x04,	0x00	; Menu item #5

#if DEFAULT_MB6582_CS
	;; Env "Misc" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_B_ENV,	0x00,	0x03	; Depth Filter
	CSENC_ENTRY	CS_MENU_B_ENV,	0x04,	0x04	; Attack
	CSENC_ENTRY	CS_MENU_B_ENV,	0x04,	0x05	; Decay
	CSENC_ENTRY	CS_MENU_B_ENV,	0x04,	0x06	; Sustain
	CSENC_ENTRY	CS_MENU_B_ENV,	0x04,	0x07	; Release
#endif

	;; -----------------------------------------------------------------
	;; Drum Engine
	;; -----------------------------------------------------------------
CS_MENU_ENC_TABLE_D
	;; OSC "Env" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_D_DRM,	0x01,	0x01	; Model
	CSENC_ENTRY	CS_MENU_D_DRM,	0x01,	0x02	; Attack
	CSENC_ENTRY	CS_MENU_D_DRM,	0x01,	0x03	; Decay
	CSENC_ENTRY	CS_MENU_D_DRM,	0x01,	0x04	; Sustain
#if CS_MENU_DISPLAYED_ITEMS == 4
	CSENC_ENTRY	CS_MENU_D_DRM,	0x02,	0x05	; Release
#else
	CSENC_ENTRY	CS_MENU_D_DRM,	0x01,	0x05	; Release
#endif

	;; OSC "Misc" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_D_DRM,	0x06,	0x06	; Tune
	CSENC_ENTRY	CS_MENU_D_DRM,	0x06,	0x07	; P.1
	CSENC_ENTRY	CS_MENU_D_DRM,	0x06,	0x08	; P.2
	CSENC_ENTRY	CS_MENU_D_DRM,	0x06,	0x09	; P.3
#if CS_MENU_DISPLAYED_ITEMS == 4
	CSENC_ENTRY	CS_MENU_D_DRM,	0x07,	0x0a	; VAs
#else
	CSENC_ENTRY	CS_MENU_D_DRM,	0x06,	0x0a	; VAs
#endif

	;; OSC "Assign" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_D_KNB,	0x00,	0x00	; Knob #1
	CSENC_ENTRY	CS_MENU_D_KNB,	0x00,	0x01	; Knob #2
	CSENC_ENTRY	CS_MENU_D_KNB,	0x00,	0x02	; Knob #3
	CSENC_ENTRY	CS_MENU_D_KNB,	0x00,	0x03	; Knob #4
	CSENC_ENTRY	CS_MENU_D_KNB,	0x00,	0x04	; Knob #5

	;; LFO section
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_ROOT,	0x00,	0x00	; DISABLED
	CSENC_ENTRY	CS_MENU_ROOT,	0x00,	0x00	; DISABLED

	;; Filter section
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_D_FIL,	0x00,	0x01	; Cutoff
	CSENC_ENTRY	CS_MENU_D_FIL,	0x00,	0x02	; Resonance

	;; Env "Env" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_ROOT,	0x00,	0x00	; DISABLED
	CSENC_ENTRY	CS_MENU_ROOT,	0x00,	0x00	; DISABLED
	CSENC_ENTRY	CS_MENU_ROOT,	0x00,	0x00	; DISABLED
	CSENC_ENTRY	CS_MENU_ROOT,	0x00,	0x00	; DISABLED
	CSENC_ENTRY	CS_MENU_ROOT,	0x00,	0x00	; DISABLED

	;; Env "Assign" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	0xff,		0x00,	0x00	; Menu item #1
	CSENC_ENTRY	0xff,		0x01,	0x00	; Menu item #2
	CSENC_ENTRY	0xff,		0x02,	0x00	; Menu item #3
	CSENC_ENTRY	0xff,		0x03,	0x00	; Menu item #4
	CSENC_ENTRY	0xff,		0x04,	0x00	; Menu item #5

#if DEFAULT_MB6582_CS
	;; Env "Misc" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_ROOT,	0x00,	0x00	; DISABLED
	CSENC_ENTRY	CS_MENU_ROOT,	0x00,	0x00	; DISABLED
	CSENC_ENTRY	CS_MENU_ROOT,	0x00,	0x00	; DISABLED
	CSENC_ENTRY	CS_MENU_ROOT,	0x00,	0x00	; DISABLED
	CSENC_ENTRY	CS_MENU_ROOT,	0x00,	0x00	; DISABLED
#endif

	;; -----------------------------------------------------------------
	;; Multi Engine
	;; -----------------------------------------------------------------
CS_MENU_ENC_TABLE_M
	;; OSC "Env" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_M_OSC,	0x03,	0x03	; Delay
	CSENC_ENTRY	CS_MENU_M_OSC,	0x03,	0x04	; Attack
	CSENC_ENTRY	CS_MENU_M_OSC,	0x03,	0x05	; Decay
	CSENC_ENTRY	CS_MENU_M_OSC,	0x03,	0x06	; Sustain
#if CS_MENU_DISPLAYED_ITEMS == 4
	CSENC_ENTRY	CS_MENU_M_OSC,	0x04,	0x07	; Release
#else
	CSENC_ENTRY	CS_MENU_M_OSC,	0x03,	0x07	; Release
#endif

	;; OSC "Misc" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_M_OSC,	0x09,	0x09	; Transpose
	CSENC_ENTRY	CS_MENU_M_OSC,	0x09,	0x0a	; Finetune
	CSENC_ENTRY	CS_MENU_M_OSC,	0x09,	0x0b	; Portamento
	CSENC_ENTRY	CS_MENU_M_OSC,	0x09,	0x0c	; Oscillator Phase Offset
#if CS_MENU_DISPLAYED_ITEMS == 4
	CSENC_ENTRY	CS_MENU_M_OSC,	0x0a,	0x0d	; Pulsewidth
#else
	CSENC_ENTRY	CS_MENU_M_OSC,	0x09,	0x0d	; Pulsewidth
#endif

	;; OSC "Assign" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_M_KNB,	0x00,	0x00	; Knob #1
	CSENC_ENTRY	CS_MENU_M_KNB,	0x00,	0x01	; Knob #2
	CSENC_ENTRY	CS_MENU_M_KNB,	0x00,	0x02	; Knob #3
	CSENC_ENTRY	CS_MENU_M_KNB,	0x00,	0x03	; Knob #4
	CSENC_ENTRY	CS_MENU_M_KNB,	0x00,	0x04	; Knob #5

	;; LFO section
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_M_LFO,	0x03,	0x03	; Rate
	CSENC_ENTRY	CS_MENU_M_LFO,	0x03,	0x05	; Depth

	;; Filter section
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_M_FIL,	0x00,	0x01	; Cutoff
	CSENC_ENTRY	CS_MENU_M_FIL,	0x00,	0x02	; Resonance

	;; Env "Env" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_M_ENV,	0x00,	0x02	; Depth
	CSENC_ENTRY	CS_MENU_M_ENV,	0x04,	0x04	; Attack
	CSENC_ENTRY	CS_MENU_M_ENV,	0x04,	0x05	; Decay
	CSENC_ENTRY	CS_MENU_M_ENV,	0x04,	0x06	; Sustain
	CSENC_ENTRY	CS_MENU_M_ENV,	0x04,	0x07	; Release

	;; Env "Assign" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	0xff,		0x00,	0x00	; Menu item #1
	CSENC_ENTRY	0xff,		0x01,	0x00	; Menu item #2
	CSENC_ENTRY	0xff,		0x02,	0x00	; Menu item #3
	CSENC_ENTRY	0xff,		0x03,	0x00	; Menu item #4
	CSENC_ENTRY	0xff,		0x04,	0x00	; Menu item #5

#if DEFAULT_MB6582_CS
	;; Env "Misc" Layer
	;;		menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_M_ENV,	0x00,	0x02	; Depth
	CSENC_ENTRY	CS_MENU_M_ENV,	0x04,	0x04	; Attack
	CSENC_ENTRY	CS_MENU_M_ENV,	0x04,	0x05	; Decay
	CSENC_ENTRY	CS_MENU_M_ENV,	0x04,	0x06	; Sustain
	CSENC_ENTRY	CS_MENU_M_ENV,	0x04,	0x07	; Release
#endif


	;; offsets between Lead/Bassline/Drum/Multi tables
CS_MENU_ENC_TABLE_LBDM_OFFSET EQU CS_MENU_ENC_TABLE_B-CS_MENU_ENC_TABLE_L
