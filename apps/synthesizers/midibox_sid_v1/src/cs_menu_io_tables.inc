; $Id$
;
; MIDIbox SID Control Surface Input/Output table for buttons and LEDs
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

; ==========================================================================
;  In this table all button functions are mapped to the DIN pins
;
;  The function name can be found on the left, the shift register and pin
;  number on the right side.
;
;  SR/pin numbers:
;     SR =  1 for the first DIN shift register
;     SR =  2 for the second DIN shift register
;     ...
;     SR = 16 for the last DIN shift register
;
;     Pin = 0 for the D0 input pin of the shift register
;     Pin = 1 for the D1 input pin of the shift register
;     ...
;     Pin = 7 for the last input pin (D7) of the shift register
;
;  Set the SR and pin number to 0 if a button function should not be used
;
;  The table must end with DIN_ENTRY_EOT!
; ==========================================================================

DIN_ENTRY MACRO function, sr, pin
	dw	function & 0xffff, (pin + 8*(sr-1)) & 0xffff
	ENDM

DIN_ENTRY_EOT MACRO
	dw	0x0000, 0x0000
	ENDM
	
CS_MENU_DIN_TABLE
	;;		Function name		SR#	Pin#
	DIN_ENTRY	CS_MENU_BUTTON_Dec,	 1,	 0	; only valid if rotary encoder not assigned to these pins
	DIN_ENTRY	CS_MENU_BUTTON_Inc,	 1,	 1	; (see mios_tables.inc) and CS_MENU_USE_INCDEC_BUTTONS == 1
	DIN_ENTRY	CS_MENU_BUTTON_Exec,	 1,	 2
	DIN_ENTRY	CS_MENU_BUTTON_Sel1,	 1,	 7
	DIN_ENTRY	CS_MENU_BUTTON_Sel2,	 1,	 6
	DIN_ENTRY	CS_MENU_BUTTON_Sel3,	 1,	 5
	DIN_ENTRY	CS_MENU_BUTTON_Sel4,	 1,	 4
	DIN_ENTRY	CS_MENU_BUTTON_Sel5,	 1,	 3
	DIN_ENTRY	CS_MENU_BUTTON_Sel6,	 0,	 0	; define this if CS_MENU_DISPLAYED_ITEMS > 5
	DIN_ENTRY	CS_MENU_BUTTON_Sel7,	 0,	 0	; define this if CS_MENU_DISPLAYED_ITEMS > 5
	DIN_ENTRY	CS_MENU_BUTTON_Sel8,	 0,	 0	; define this if CS_MENU_DISPLAYED_ITEMS > 5
	DIN_ENTRY	CS_MENU_BUTTON_Sel9,	 0,	 0	; define this if CS_MENU_DISPLAYED_ITEMS > 5
	DIN_ENTRY	CS_MENU_BUTTON_Sel10,	 0,	 0	; define this if CS_MENU_DISPLAYED_ITEMS > 5

	DIN_ENTRY	CS_MENU_BUTTON_SID1,	 2,      0
	DIN_ENTRY	CS_MENU_BUTTON_SID2,	 2,      1
	DIN_ENTRY	CS_MENU_BUTTON_SID3,	 2,      2
	DIN_ENTRY	CS_MENU_BUTTON_SID4,	 2,      3
	DIN_ENTRY	CS_MENU_BUTTON_Link,	 2,	 4
	DIN_ENTRY	CS_MENU_BUTTON_CC,	 2,	 5
	DIN_ENTRY	CS_MENU_BUTTON_Edit,	 2,	 6

	DIN_ENTRY	CS_MENU_BUTTON_Osc_Sel,	 4,	 2
	DIN_ENTRY	CS_MENU_BUTTON_Osc_Ctrl, 4,	 3
	DIN_ENTRY	CS_MENU_BUTTON_Osc_Wav,  4,	 4
	DIN_ENTRY	CS_MENU_BUTTON_Osc_RS,   4,	 5

 	DIN_ENTRY	CS_MENU_BUTTON_LFO_Sel,	 5,	 2
	DIN_ENTRY	CS_MENU_BUTTON_LFO_Wav,	 5,	 3

	DIN_ENTRY	CS_MENU_BUTTON_Env_Sel,	 7,	 2
	DIN_ENTRY	CS_MENU_BUTTON_Env_Ctrl, 7,	 3

	DIN_ENTRY	CS_MENU_BUTTON_Fil_Sel,	 7,	 4
	DIN_ENTRY	CS_MENU_BUTTON_Fil_Mod,  7,	 5

	DIN_ENTRY	CS_MENU_BUTTON_M_O1Ptch, 7,	 6
	DIN_ENTRY	CS_MENU_BUTTON_M_O2Ptch, 7,	 7
	DIN_ENTRY	CS_MENU_BUTTON_M_O3Ptch, 8,	 0
	DIN_ENTRY	CS_MENU_BUTTON_M_O1PW,   8,	 1
	DIN_ENTRY	CS_MENU_BUTTON_M_O2PW,   8,	 2
	DIN_ENTRY	CS_MENU_BUTTON_M_O3PW,   8,	 3
	DIN_ENTRY	CS_MENU_BUTTON_M_Filter, 8,	 4
	DIN_ENTRY	CS_MENU_BUTTON_M_E1,	 8,	 5
	DIN_ENTRY	CS_MENU_BUTTON_M_E2,	 8,	 6
	DIN_ENTRY	CS_MENU_BUTTON_M_L1,	 8,	 7
	DIN_ENTRY	CS_MENU_BUTTON_M_L2,	 9,	 0
	DIN_ENTRY	CS_MENU_BUTTON_M_L3,	 9,	 1
	DIN_ENTRY	CS_MENU_BUTTON_M_L4,	 9,	 2
	DIN_ENTRY	CS_MENU_BUTTON_M_L5,	 9,	 3
	DIN_ENTRY	CS_MENU_BUTTON_M_L6,	 9,	 4

	DIN_ENTRY_EOT



; ==========================================================================
;  The following table defines all available DOUT pins with the appr.
;  register and bit which is assigned to the pin
;  CS_MENU_LED_Update uses this table to update all LEDs
; 
;  The register name and bit number can be found on the left, 
;  the shift register and pin number on the right side.
;
;  SR/pin numbers:
;     SR =  0 for J5 pins (note: the ENABLE_J5 switch in main.asm has to be set to "1" to get this working)
;
;     SR =  1 for the first DOUT shift register
;     SR =  2 for the second DOUT shift register
;     ...
;     SR = 16 for the last DOUT shift register
;
;     Pin = 0 for the D7 output pin of the shift register
;     Pin = 1 for the D6 output pin of the shift register
;     ...
;     Pin = 7 for the last output pin (D0) of the shift register
;
;  Set the SR and pin number to 0 if a LED function should not be used
;
;  The table must end with DOUT_ENTRY_EOT!
; ==========================================================================

DOUT_ENTRY MACRO reg, bit, sr, pin
        if sr == 0		; J5 selected
		dw	reg, bit | ((pin | 0x80) << 8)
	else			; SR selected
		dw	reg, bit | ((pin + 8*(sr-1)) << 8)
	endif
	ENDM

DOUT_ENTRY_EOT MACRO
	dw	0x0000, 0x0000
	ENDM
	
CS_MENU_DOUT_TABLE
	;;		Register and bit			SR#	Pin#	  Description
	DOUT_ENTRY	CS_MENU_SELECTED_SID_FLAGS, 0,		1,	0	; SID1 LED (Note: Pin #0 is the D7 output of first SR)
	DOUT_ENTRY	CS_MENU_SELECTED_SID_FLAGS, 1,		1,	1	; SID2 LED
	DOUT_ENTRY	CS_MENU_SELECTED_SID_FLAGS, 2,		1,	2	; SID3 LED
	DOUT_ENTRY	CS_MENU_SELECTED_SID_FLAGS, 3,		1,	3	; SID4 LED

	DOUT_ENTRY	CS_MENU_MODE, CS_MENU_MODE_LINK,	1,	4	; Link LED
	DOUT_ENTRY	CS_MENU_MODE, CS_MENU_MODE_CC,		1,	5	; CC LED
	DOUT_ENTRY	CS_MENU_MODE, CS_MENU_MODE_EDIT_FLASHED,1,	6	; Edit LED

	DOUT_ENTRY	CS_MENU_SELECTED_OSC_FLAGS, 0,		2,	0	; OSC1 LED
	DOUT_ENTRY	CS_MENU_SELECTED_OSC_FLAGS, 1,		2,	1	; OSC2 LED
	DOUT_ENTRY	CS_MENU_SELECTED_OSC_FLAGS, 2,		2,	2	; OSC3 LED

	DOUT_ENTRY	TMP1, 0,				2,	3	; OSC Env LED
	DOUT_ENTRY	TMP1, 1,				2,	4	; OSC Misc LED
	DOUT_ENTRY	TMP1, 2,				2,	5	; OSC Assign LED

	DOUT_ENTRY	TMP1, 4,				3,	0	; OSC Triangle LED
	DOUT_ENTRY	TMP1, 5,				3,	1	; OSC Saw LED
	DOUT_ENTRY	TMP1, 6,				3,	2	; OSC Pulse LED
	DOUT_ENTRY	TMP1, 7,				3,	3	; OSC Noise LED

	DOUT_ENTRY	TMP2, 0,				2,	6	; OSC Sync LED
	DOUT_ENTRY	TMP2, 1,				2,	7	; OSC Ring LED

	DOUT_ENTRY	TMP3, 0,				3,	4	; Filter O1 LED
	DOUT_ENTRY	TMP3, 1,				3,	5	; Filter O2 LED
	DOUT_ENTRY	TMP3, 2,				3,	6	; Filter O3 LED
	DOUT_ENTRY	TMP3, 3,				3,	7	; Filter Ext LED

	DOUT_ENTRY	TMP3, 4,				4,	0	; Filter LP LED
	DOUT_ENTRY	TMP3, 5,				4,	1	; Filter BP LED
	DOUT_ENTRY	TMP3, 6,				4,	2	; Filter HP LED
	DOUT_ENTRY	TMP3, 7,				4,	3	; Filter 3O LED

	DOUT_ENTRY	TMP2, 4,				4,	4	; ENV1 LED
	DOUT_ENTRY	TMP2, 5,				4,	5	; ENV2 LED

	DOUT_ENTRY	TMP2, 6,				4,	6	; ENV Ctrl LED
	DOUT_ENTRY	TMP2, 7,				4,	7	; ENV Assign LED

	DOUT_ENTRY	TMP4, 0,				5,	0	; LFO1 LED
	DOUT_ENTRY	TMP4, 1,				5,	1	; LFO2 LED
	DOUT_ENTRY	TMP4, 2,				5,	2	; LFO3 LED
	DOUT_ENTRY	TMP4, 3,				5,	3	; LFO4 LED
	DOUT_ENTRY	TMP4, 4,				5,	4	; LFO5 LED
	DOUT_ENTRY	TMP4, 5,				5,	5	; LFO6 LED

	DOUT_ENTRY	TMP5, 0,				5,	6	; LFO Sine LED
	DOUT_ENTRY	TMP5, 1,				5,	7	; LFO Triangle LED
	DOUT_ENTRY	TMP5, 2,				6,	0	; LFO Saw LED
	DOUT_ENTRY	TMP5, 3,				6,	1	; LFO Pulse LED
	DOUT_ENTRY	TMP5, 4,				6,	2	; LFO Random LED


	;; NOTE: the pins of the MIDI Rx/Tx LEDs are assigned at main.asm
	DOUT_ENTRY_EOT
