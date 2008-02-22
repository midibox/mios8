; $Id$
;
; MIDIbox FM Control Surface Input/Output table for buttons and LEDs
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
	dw	function, (pin + 8*(sr-1))
	ENDM

DIN_ENTRY_EOT MACRO
	dw	0x0000, 0x0000
	ENDM
	
CS_MENU_DIN_TABLE
	;;		Function name		SR#	Pin#
	DIN_ENTRY	CS_MENU_BUTTON_Dec,	 1,	 0	; only valid if rotary encoder not assigned to these pins
	DIN_ENTRY	CS_MENU_BUTTON_Inc,	 1,	 1	; (see mios_tables.inc) and CS_MENU_USE_INCDEC_BUTTONS == 1
	DIN_ENTRY	CS_MENU_BUTTON_Esc,	 1,	 2
	
	DIN_ENTRY	CS_MENU_BUTTON_Sel1,	 1,	 3
	DIN_ENTRY	CS_MENU_BUTTON_Sel2,	 1,	 4
	DIN_ENTRY	CS_MENU_BUTTON_Sel3,	 1,	 5
	DIN_ENTRY	CS_MENU_BUTTON_Sel4,	 1,	 6
	DIN_ENTRY	CS_MENU_BUTTON_Sel5,	 1,	 7
	DIN_ENTRY	CS_MENU_BUTTON_Sel6,	 2,	 0
	DIN_ENTRY	CS_MENU_BUTTON_Sel7,	 2,	 1
	DIN_ENTRY	CS_MENU_BUTTON_Sel8,	 2,	 2
	DIN_ENTRY	CS_MENU_BUTTON_Cfg,	 2,	 3
	
	DIN_ENTRY	CS_MENU_BUTTON_InsOP1,	 2,      4
	DIN_ENTRY	CS_MENU_BUTTON_InsOP2,	 2,      5
	DIN_ENTRY	CS_MENU_BUTTON_InsOP3,	 2,      6
	DIN_ENTRY	CS_MENU_BUTTON_InsOP4,	 2,      7
	DIN_ENTRY	CS_MENU_BUTTON_InsOPSel, 3,	 0

	;; the M_Row[1234] buttons are handled from an interrupt service routine!
	DIN_ENTRY_EOT


; ==========================================================================
;  This short table lists the SR/pin numbers of the four
;  Matrix Row Select Buttons. 
;  Since these buttons are multiplexed (4 DOUT pins -> 1 DIN pin) to save
;  one DIN shift register, we have to define the DIN pin and 4 DOUT pins
;  which are connected to these buttons (see also schematic)
; ==========================================================================
DIO_M_COL_ENTRY MACRO sr0, pin0, sr1, pin1, sr2, pin2, sr3, pin3, sr4, pin4, sr5, pin5
	db	(pin0 + 8*(sr0-1)), (pin1 + 8*(sr1-1)), (pin2 + 8*(sr2-1)), (pin3 + 8*(sr3-1)), (pin4 + 8*(sr4-1)), (pin5 + 8*(sr5-1))
	ENDM

DIO_M_ROW_ENTRY MACRO sr_in, pin_in, sr0, pin0, sr1, pin1, sr2, pin2, sr3, pin3
	db	(pin_in + 8*(sr_in-1)), (pin0 + 8*(sr0-1)), (pin1 + 8*(sr1-1)), (pin2 + 8*(sr2-1)), (pin3 + 8*(sr3-1))
	ENDM

DIO_M_TABLE
	;; --> DIN <-- pins of the 6 column select buttons
	;;		 Col1 SR/Pin, Col2 SR/Pin,  Col3 SR/Pin,  Col4 SR/Pin,  Col5 SR/Pin,  Col6 SR/Pin
	DIO_M_COL_ENTRY  4, 2,        4, 3,         4, 4,         4, 5,         4, 6,         4, 7

	;; --> DOUT <-- pins of the 6 LED columns
	;;		 Col1 SR/Pin, Col2 SR/Pin,  Col3 SR/Pin,  Col4 SR/Pin,  Col5 SR/Pin,  Col6 SR/Pin
	DIO_M_COL_ENTRY  2, 6,        2, 7,         3, 0,         3, 1,         3, 2,         3, 3

	;; --> 1*DIN/4*DOUT <-- pins of the 4 row select buttons
	;;		 DIN SR/Pin,  Row1 SR/Pin,  Row2 SR/Pin,  Row3 SR/Pin,  Row4 SR/Pin
	DIO_M_ROW_ENTRY  3, 1,        2, 2,         2, 3,         2, 4,         2, 5


; ==========================================================================
;  The following table defines all available DOUT pins with the appr.
;  register and bit which is assigned to the pin
;  CS_MENU_LED_Update uses this table to update all LEDs
; 
;  The register name and bit number can be found on the left, 
;  the shift register and pin number on the right side.
;
;  SR/pin numbers:
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
		dw	reg, bit | ((pin + 8*(sr-1)) << 8)
	ENDM

DOUT_ENTRY_EOT MACRO
	dw	0x0000, 0x0000
	ENDM
	
CS_MENU_DOUT_TABLE
	;;		Register and bit			SR#	Pin#	  Description
	DOUT_ENTRY	CS_MENU_SELECTED_INS_FLAGS, 0,		1,	0	; INS1 LED (Note: Pin #0 is the D7 output of first SR)
	DOUT_ENTRY	CS_MENU_SELECTED_INS_FLAGS, 1,		1,	1	; INS2 LED
	DOUT_ENTRY	CS_MENU_SELECTED_INS_FLAGS, 2,		1,	2	; INS3 LED
	DOUT_ENTRY	CS_MENU_SELECTED_INS_FLAGS, 3,		1,	3	; INS4 LED

	DOUT_ENTRY	CS_MENU_SELECTED_OP_FLAGS, 0,		1,	4	; OP1 LED
	DOUT_ENTRY	CS_MENU_SELECTED_OP_FLAGS, 1,		1,	5	; OP2 LED
	DOUT_ENTRY	CS_MENU_SELECTED_OP_FLAGS, 2,		1,	6	; OP3 LED
	DOUT_ENTRY	CS_MENU_SELECTED_OP_FLAGS, 3,		1,	7	; OP4 LED

	DOUT_ENTRY	TMP1, 0,				2,	0	; Instrument LED
	DOUT_ENTRY	TMP1, 1,				2,	1	; OP LED

	;; NOTE: the pins of the MIDI Rx/Tx LEDs are assigned at main.asm
	DOUT_ENTRY_EOT
