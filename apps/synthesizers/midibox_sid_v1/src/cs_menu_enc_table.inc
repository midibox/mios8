; $Id$
;
; MIDIbox SID Control Surface Rotary Encoder assign tables
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

; ==========================================================================
;  In this table all encoder functions are listed
;  the number and the position of entries are static and cannot be changed
;  without a modification in cs_menu_enc.inc!
; ==========================================================================

CSENC_ENTRY MACRO function, parameter, menu, page_offset, cursor_pos
	db	function, parameter, menu, ((page_offset&0x0f)<<4) | (cursor_pos&0x0f)
	ENDM

	;; available functions:
	;; CSENC_ENTRY parameters:
	;;    - the ID itself
	;;    - the variable offset (see app_defines.inc, variables begin with CS_SID_*)
	;;    - the menu in which the variable is located (see cs_menu_tables.inc)
	;;    - the page offset (which menu item should be at the left border)
	;;    - the cursor position of the menu item
#define CS_MENU_ENC_CHANGE_SYS	     0x00	; to change any internal parameter directly
#define CS_MENU_ENC_CHANGE_OSC       0x10	; to change parameter of currently selected OSC
#define CS_MENU_ENC_CHANGE_LFO       0x20	; to change parameter of currently selected LFO
#define CS_MENU_ENC_CHANGE_ENV       0x30	; to change parameter of currently selected ENV
	
	;; CSENC_ENTRY parameters:
	;;    - the ID itself
	;;    - other values should be zeroed
#define CS_MENU_ENC_CHANGE_MENU_P1   0x40	; to change menu parameter #1
#define CS_MENU_ENC_CHANGE_MENU_P2   0x41	; to change menu parameter #2
#define CS_MENU_ENC_CHANGE_MENU_P3   0x42	; to change menu parameter #3
#define CS_MENU_ENC_CHANGE_MENU_P4   0x43	; to change menu parameter #4
#define CS_MENU_ENC_CHANGE_MENU_P5   0x44	; to change menu parameter #5 (if 2x20 display)
#define CS_MENU_ENC_CHANGE_MENU_P6   0x45	; to change menu parameter #6 (if >2x20 display)
#define CS_MENU_ENC_CHANGE_MENU_P7   0x46	; to change menu parameter #7 (if >2x20 display)
#define CS_MENU_ENC_CHANGE_MENU_P8   0x47	; to change menu parameter #8 (if >2x20 display)
#define CS_MENU_ENC_CHANGE_MENU_P9   0x48	; to change menu parameter #9 (if >2x20 display)
#define CS_MENU_ENC_CHANGE_MENU_P10  0x49	; to change menu parameter #10 (if 2x40 display)

	;; CSENC_ENTRY parameters:
	;;    - the ID itself
	;;    - the CC number
	;;    - the menu entry will be ignored
	;;    - the page offset will be ignored
	;;    - the cursor pos will be ignored
#define CS_MENU_ENC_CC_Rel1     0x50	; to send a relative CC at channel 1 (for assign function)
#define CS_MENU_ENC_CC_Rel2     0x51	; to send a relative CC at channel 2
#define CS_MENU_ENC_CC_Rel3     0x52	; to send a relative CC at channel 3
#define CS_MENU_ENC_CC_Rel4     0x53	; to send a relative CC at channel 4
#define CS_MENU_ENC_CC_Rel5     0x54	; to send a relative CC at channel 5
#define CS_MENU_ENC_CC_Rel6     0x55	; to send a relative CC at channel 6
#define CS_MENU_ENC_CC_Rel7     0x56	; to send a relative CC at channel 7
#define CS_MENU_ENC_CC_Rel8     0x57	; to send a relative CC at channel 8
#define CS_MENU_ENC_CC_Rel9     0x58	; to send a relative CC at channel 9
#define CS_MENU_ENC_CC_Rel10    0x59	; to send a relative CC at channel 10
#define CS_MENU_ENC_CC_Rel11    0x5a	; to send a relative CC at channel 11
#define CS_MENU_ENC_CC_Rel12    0x5b	; to send a relative CC at channel 12
#define CS_MENU_ENC_CC_Rel13    0x5c	; to send a relative CC at channel 13
#define CS_MENU_ENC_CC_Rel14    0x5d	; to send a relative CC at channel 14
#define CS_MENU_ENC_CC_Rel15    0x5e	; to send a relative CC at channel 15
#define CS_MENU_ENC_CC_Rel16    0x5f	; to send a relative CC at channel 16

	;; CSENC_ENTRY parameters:
	;;    - the ID itself
	;;    - the CC number
	;;    - the menu entry will be ignored
	;;    - the page offset will be ignored
	;;    - cursor pos holds the register number (0x00-0x07) in which the absolute value will be stored
#define CS_MENU_ENC_CC_Abs1     0x60	; to send a absolute CC at channel 1 (for assign function)
#define CS_MENU_ENC_CC_Abs2     0x61	; to send a absolute CC at channel 2
#define CS_MENU_ENC_CC_Abs3     0x62	; to send a absolute CC at channel 3
#define CS_MENU_ENC_CC_Abs4     0x63	; to send a absolute CC at channel 4
#define CS_MENU_ENC_CC_Abs5     0x64	; to send a absolute CC at channel 5
#define CS_MENU_ENC_CC_Abs6     0x65	; to send a absolute CC at channel 6
#define CS_MENU_ENC_CC_Abs7     0x66	; to send a absolute CC at channel 7
#define CS_MENU_ENC_CC_Abs8     0x67	; to send a absolute CC at channel 8
#define CS_MENU_ENC_CC_Abs9     0x68	; to send a absolute CC at channel 9
#define CS_MENU_ENC_CC_Abs10    0x69	; to send a absolute CC at channel 10
#define CS_MENU_ENC_CC_Abs11    0x6a	; to send a absolute CC at channel 11
#define CS_MENU_ENC_CC_Abs12    0x6b	; to send a absolute CC at channel 12
#define CS_MENU_ENC_CC_Abs13    0x6c	; to send a absolute CC at channel 13
#define CS_MENU_ENC_CC_Abs14    0x6d	; to send a absolute CC at channel 14
#define CS_MENU_ENC_CC_Abs15    0x6e	; to send a absolute CC at channel 15
#define CS_MENU_ENC_CC_Abs16    0x6f	; to send a absolute CC at channel 16

CS_MENU_ENC_TABLE
	;; OSC "Env" Layer
	;;		Function name		parameter		  menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_OSC, CS_SID_VOICEx_DELAY,	  CS_MENU_OSC,	0x03,	0x03
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_OSC, CS_SID_VOICEx_ATTACK,	  CS_MENU_OSC,	0x03,	0x04
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_OSC, CS_SID_VOICEx_DECAY,	  CS_MENU_OSC,	0x03,	0x05
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_OSC, CS_SID_VOICEx_SUSTAIN,	  CS_MENU_OSC,	0x03,	0x06
#if CS_MENU_DISPLAYED_ITEMS == 4
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_OSC, CS_SID_VOICEx_RELEASE,	  CS_MENU_OSC,	0x04,	0x07
#else
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_OSC, CS_SID_VOICEx_RELEASE,	  CS_MENU_OSC,	0x03,	0x07
#endif

	;; OSC "Misc" Layer
	;;		Function name		parameter		  menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_OSC, CS_SID_VOICEx_TRANSPOSE,  CS_MENU_OSC,	0x09,	0x09
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_OSC, CS_SID_VOICEx_FINETUNE,	  CS_MENU_OSC,	0x09,	0x0a
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_OSC, CS_SID_VOICEx_PORTAMENTO, CS_MENU_OSC,	0x09,	0x0b
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_OSC, CS_SID_VOICEx_ARPEGGIATOR,CS_MENU_OSC,	0x09,	0x0c
#if CS_MENU_DISPLAYED_ITEMS == 4
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_OSC, CS_SID_VOICEx_PULSEWIDTH, CS_MENU_OSC,	0x0a,	0x0d
#else
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_OSC, CS_SID_VOICEx_PULSEWIDTH, CS_MENU_OSC,	0x09,	0x0d
#endif

	;; OSC "Assign" Layer
	;;		Function name		parameter		  menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_ENC_CC_Abs16,	0x10,			  0x00,		0x00,	0x00; HERE:
	CSENC_ENTRY	CS_MENU_ENC_CC_Abs16,	0x11,			  0x00,		0x00,	0x01; Abs. Register
	CSENC_ENTRY	CS_MENU_ENC_CC_Abs16,	0x12,			  0x00,		0x00,	0x02; number
	CSENC_ENTRY	CS_MENU_ENC_CC_Abs16,	0x13,			  0x00,		0x00,	0x03; (0x00-0x0f)
	CSENC_ENTRY	CS_MENU_ENC_CC_Abs16,	0x14,			  0x00,		0x00,	0x04

	;; LFO section
	;;		Function name		parameter		  menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_LFO, CS_SID_LFOx_RATE,	  CS_MENU_LFO,	0x00,	0x02
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_LFO, CS_SID_LFOx_DEPTH,	  CS_MENU_LFO,	0x00,	0x03

	;; Filter section
	;;		Function name		parameter		  menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_SYS, CS_SID_FILTER_CUTOFF,	  CS_MENU_FIL,	0x00,	0x01
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_SYS, CS_SID_FILTER_RESONANCE,  CS_MENU_FIL,	0x00,	0x02

	;; Env "Env" Layer
	;;		Function name		parameter		  menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_ENV, CS_SID_ENVx_DEPTH,	  CS_MENU_ENV,	0x01,	0x01
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_ENV, CS_SID_ENVx_ATTACK,	  CS_MENU_ENV,	0x01,	0x02
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_ENV, CS_SID_ENVx_DECAY,	  CS_MENU_ENV,	0x01,	0x03
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_ENV, CS_SID_ENVx_SUSTAIN,	  CS_MENU_ENV,	0x01,	0x04
#if CS_MENU_DISPLAYED_ITEMS == 4
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_ENV, CS_SID_ENVx_RELEASE,	  CS_MENU_ENV,	0x02,	0x05
#else
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_ENV, CS_SID_ENVx_RELEASE,	  CS_MENU_ENV,	0x01,	0x05
#endif

	;; Env "Assign" Layer
	;;		Function name		parameter		  menu		offset	cursor pos
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_MENU_P1, 0x00,		  0x00,		0x00,	0x00
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_MENU_P2, 0x00,		  0x00,		0x00,	0x00
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_MENU_P3, 0x00,		  0x00,		0x00,	0x00
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_MENU_P4, 0x00,		  0x00,		0x00,	0x00
	CSENC_ENTRY	CS_MENU_ENC_CHANGE_MENU_P5, 0x00,		  0x00,		0x00,	0x00
