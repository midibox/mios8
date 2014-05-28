; $Id$
	LIST R=DEC
;
; Prepared Setup File for 8580 SID
;

	;; BankStick Chip Select Address for "ensembles"
	;; -1: only one ensemble available, stored in internal EEPROM
	;; 0..7: CS# configuration of BankStick (Pin 1, 2 and 3)
	;; reminder for default value 7: Pin 1, 2 and 3 have connected to +5V
	;; a 32k BankStick 24LC256 is sufficient for 128 storable ensembles
	;; a 64k BankStick will also work, but doesn't increase the number of available ensembles
#define DEFAULT_ENS_BS_CS	7

	;; select default filter calibration value
	;; expecting 12bit values (0..4095)
	;; will be overwritten by ensemble setting - this is only used as default value for BankStick initialisation!
#define DEFAULT_FILTER_CALI_MIN 0
#define DEFAULT_FILTER_CALI_MAX 1536

	;; number of visible menu items on LCD
	;; use:  4 for 2x16 displays
	;;       5 for 2x20 displays
	;;      10 for 2x40 displays
#define CS_MENU_DISPLAYED_ITEMS 5
	;; NOTE: if CS_MENU_DISPLAYED_ITEMS > 5, you have to adapt the DIN settings
	;; in CS_MENU_DIN_TABLE below

	;; number of LCD lines (supported: 2 for 2x* LCDs and 4 for 4x* LCDs)
#define DEFAULT_LCD_LINES 2

	;; LCD line -> cursor offsets
#define DEFAULT_LCD_LINE_Y0	0x00
#define DEFAULT_LCD_LINE_Y1	0x40
#define DEFAULT_LCD_LINE_Y2	0x14
#define DEFAULT_LCD_LINE_Y3	0x54

	;; 1: LCD cursor will be set to current parameter position (clashes with bar graph which is enabled with DEFAULT_LCD_LINES >= 4)
	;; 0: disable this feature
#define DEFAULT_LCD_PRINT_CURSOR 0

	;; Inc/Dec buttons or rotary encoder of data entry?
	;; use:  0 if rotary encoder should be used 
	;;         (pins of encoder are defined in MIOS_ENC_PIN_TABLE in this file)
	;;       1 if buttons should be used instead
#define CS_MENU_USE_INCDEC_BUTTONS 0

	;; 0: J5 has no function at all (analog inputs don't need to be connected to ground)
	;; 1: J5 used for analog inputs - A0..A4 control Knob#1..#5, A5..A7 are not used at all
	;; 2: J5 used as digital output (pins can be added in CS_MENU_DOUT_TABLE as "shift register #0")
	;; 3: J5 used as digital output for external switches (replacement for DEFAULT_EXT_SWITCH_DOUT, AOUT_INTERFACE_TYPE must be != 0)
#define DEFAULT_J5_FUNCTION	0

	;; DOUT/DIN shift register matrix:
	;; define the DOUT shift register (1-16) to which the cathodes are connected (common line driver)
	;; 0 disables this shift register (doesn't make much sense)
#define DEFAULT_SRM_CATHODES		8

	;; define the DOUT shift register (1-16) to which the anodes of the modulation matrix are connected
	;; 0 disables this shift register
#define DEFAULT_SRM_MODMATRIX_DOUT	7

	;; optional LED/Button Matrix (used by "Wilba's Board" to reduce number of shift registers)
	;; define the DOUT shift register (1-16) to which the anodes of the LED matrix are connected
	;; 0 disables the LED matrix function - LEDs have to be connected to individual DOUT pins
#define DEFAULT_SRM_LEDS_DOUT		0

	;; define the DIN shift register (1-16) to which the button matrix is connected
	;; 0 disables the button matrix function - Buttons have to be connected to individual DIN pins
#define DEFAULT_SRM_BUTTONS_DIN		0

	;; define the number of shift registers (1-16) in the SRIO chain (DIN/DOUT shift registers)
	;; use the number of the longest chain - e.g. 4 DOUT registers, 10 DIN registers: use 10
	;; 0 disables the SRIO chain
#define DEFAULT_SRIO_NUMBER		10


	;; define the AOUT interface which is used here:
	;;   1: one MBHP_AOUT module
	;;   2: up to 4 (chained) MBHP_AOUT_LC modules
	;;   3: one MBHP_AOUT_NG module
	;; all other values invalid!
#define AOUT_INTERFACE_TYPE 0

	;; only relevant if one or more AOUT_LC modules are used:
	;; define the resolution configuration here
	;;   0: first channel 12bit, second channel 4bit
	;;   1: first channel 8bit, second channel 8bit
	;;   2: combines M1,M2 and/or M3/M4: first channel 12bit, second channel 12bit, third channel 8bit, fourth channel ignored!
	;; all other values invalid!
#define AOUT_LC_RESOLUTION_OPTION_M1 0
#define AOUT_LC_RESOLUTION_OPTION_M2 0
#define AOUT_LC_RESOLUTION_OPTION_M3 0
#define AOUT_LC_RESOLUTION_OPTION_M4 0


	;; the 8 external switches can be routed to a DOUT shift register:
	;; 0: not used
	;; 1-16: DOUT SR#1..#16
#define DEFAULT_EXT_SWITCH_DOUT	0

	;; 0: SwinSID extensions disabled
	;; 1: SwinSID extensions enabled
#define DEFAULT_ENABLE_SWINSID 0

	;; 0: only waveform 0-8 selectable
	;; 1: waveform 0-15 selectable (e.g. for selecting additional SwinSID Waveforms)
#define DEFAULT_ENABLE_UPPER_WAVEFORMS 0

	;; assign the AOUT channels for the F2A (Filter to AOUT function) here
	;; we have 4 assignments for CutOff Left/Right and Resonance Left/Right
	;; 0 disables an assignment
#define DEFAULT_F2A_CUTOFF_L_AOUT	1
#define DEFAULT_F2A_RESONANCE_L_AOUT	2
#define DEFAULT_F2A_CUTOFF_R_AOUT	3
#define DEFAULT_F2A_RESONANCE_R_AOUT	4

	;; assign the AOUT channel for the V2A (Volume to AOUT function) here
	;; we have two assignments for the left/right SID channel
	;; 0 disables an assignment
#define DEFAULT_V2A_VOLUME_L_AOUT	5
#define DEFAULT_V2A_VOLUME_R_AOUT	6

	;; assign the AOUT channel for the P2A (Pulsewidth to AOUT function) here
	;; we have six assignments for all oscillators (left/right channel)
	;; note that there is a channel conflict between other *2A functions - adaption for your setup required!
	;; 0 disables an assignment
#define DEFAULT_P2A_OSC1_L_AOUT		5
#define DEFAULT_P2A_OSC2_L_AOUT		0
#define DEFAULT_P2A_OSC3_L_AOUT		0
#define DEFAULT_P2A_OSC1_R_AOUT		6
#define DEFAULT_P2A_OSC2_R_AOUT		0
#define DEFAULT_P2A_OSC3_R_AOUT		0

	;; assign the AOUT channel for the K2A (Key to AOUT function) here
	;; we have six assignments for all oscillators (left/right channel)
	;; note that there is a channel conflict between other *2A functions - adaption for your setup required!
	;; 0 disables an assignment
#define DEFAULT_K2A_OSC1_L_AOUT		7
#define DEFAULT_K2A_OSC2_L_AOUT		0
#define DEFAULT_K2A_OSC3_L_AOUT		0
#define DEFAULT_K2A_OSC1_R_AOUT		8
#define DEFAULT_K2A_OSC2_R_AOUT		0
#define DEFAULT_K2A_OSC3_R_AOUT		0

	;; assign the AOUT channel for the O2A (Oscillator Frequency to AOUT function) here
	;; we have six assignments for all oscillators (left/right channel)
	;; note that there is a channel conflict between other *2A functions - adaption for your setup required!
	;; note also, that with O2A enabled, the connected SIDs won't output the correct frequencies anymore
	;; 0 disables an assignment
#define DEFAULT_O2A_OSC1_L_AOUT		7
#define DEFAULT_O2A_OSC2_L_AOUT		0
#define DEFAULT_O2A_OSC3_L_AOUT		0
#define DEFAULT_O2A_OSC1_R_AOUT		8
#define DEFAULT_O2A_OSC2_R_AOUT		0
#define DEFAULT_O2A_OSC3_R_AOUT		0

	;; gate/slide/accent of all 6 oscillators + filter flags can be forwarded to the 8 digital outputs at J5
	;; note that this overlays the switch assignments of the patch structure
	;; DEFAULT_J5_FUNCTION should be set to 3 if you are plannung to use this function.
	;; possible values: 0..8 - 0 disables an assignment
	
	;; (gate supported by all engines)
#define DEFAULT_GATE_OSC1_L_OUT		1
#define DEFAULT_GATE_OSC2_L_OUT		0
#define DEFAULT_GATE_OSC3_L_OUT		0
#define DEFAULT_GATE_OSC1_R_OUT		2
#define DEFAULT_GATE_OSC2_R_OUT		0
#define DEFAULT_GATE_OSC3_R_OUT		0

	;; (slide flag currently only supported by bassline engine)
#define DEFAULT_SLIDE_OSC1_L_OUT	0
#define DEFAULT_SLIDE_OSC2_L_OUT	0
#define DEFAULT_SLIDE_OSC3_L_OUT	0
#define DEFAULT_SLIDE_OSC1_R_OUT	0
#define DEFAULT_SLIDE_OSC2_R_OUT	0
#define DEFAULT_SLIDE_OSC3_R_OUT	0

	;; (accent flag currently only supported by bassline and drum engine)
#define DEFAULT_ACCENT_OSC1_L_OUT	0
#define DEFAULT_ACCENT_OSC2_L_OUT	0
#define DEFAULT_ACCENT_OSC3_L_OUT	0
#define DEFAULT_ACCENT_OSC1_R_OUT	0
#define DEFAULT_ACCENT_OSC2_R_OUT	0
#define DEFAULT_ACCENT_OSC3_R_OUT	0

	;; filter flags supported by all engines
#define DEFAULT_FILTER_LP_L_OUT		0
#define DEFAULT_FILTER_BP_L_OUT		0
#define DEFAULT_FILTER_HP_L_OUT		0

#define DEFAULT_FILTER_LP_R_OUT		0
#define DEFAULT_FILTER_BP_R_OUT		0
#define DEFAULT_FILTER_HP_R_OUT		0

#define DEFAULT_FILTER_O1_L_OUT		0
#define DEFAULT_FILTER_O2_L_OUT		0
#define DEFAULT_FILTER_O3_L_OUT		0
#define DEFAULT_FILTER_EXTIN_L_OUT	0

#define DEFAULT_FILTER_O1_R_OUT		0
#define DEFAULT_FILTER_O2_R_OUT		0
#define DEFAULT_FILTER_O3_R_OUT		0
#define DEFAULT_FILTER_EXTIN_R_OUT	0

	;; selected waveform supported by all engines
#define DEFAULT_WAVE_O1_L_TRI		0
#define DEFAULT_WAVE_O1_L_SAW		0
#define DEFAULT_WAVE_O1_L_PULSE		0
#define DEFAULT_WAVE_O1_L_NOISE		0

#define DEFAULT_WAVE_O2_L_TRI		0
#define DEFAULT_WAVE_O2_L_SAW		0
#define DEFAULT_WAVE_O2_L_PULSE		0
#define DEFAULT_WAVE_O2_L_NOISE		0

#define DEFAULT_WAVE_O3_L_TRI		0
#define DEFAULT_WAVE_O3_L_SAW		0
#define DEFAULT_WAVE_O3_L_PULSE		0
#define DEFAULT_WAVE_O3_L_NOISE		0

#define DEFAULT_WAVE_O1_R_TRI		0
#define DEFAULT_WAVE_O1_R_SAW		0
#define DEFAULT_WAVE_O1_R_PULSE		0
#define DEFAULT_WAVE_O1_R_NOISE		0

#define DEFAULT_WAVE_O2_R_TRI		0
#define DEFAULT_WAVE_O2_R_SAW		0
#define DEFAULT_WAVE_O2_R_PULSE		0
#define DEFAULT_WAVE_O2_R_NOISE		0

#define DEFAULT_WAVE_O3_R_TRI		0
#define DEFAULT_WAVE_O3_R_SAW		0
#define DEFAULT_WAVE_O3_R_PULSE		0
#define DEFAULT_WAVE_O3_R_NOISE		0


	;; sets the default display mode for the LED matrix
	;;    0: shows modulation assignments after power-on
	;;    1: shows modulation meters after power-on
	;; mode 0/1 can be toggled by pressing two mod target buttons at the same time (e.g. O1 and O2 Pitch)
	;; or by using a dedicated button (-> CS_MENU_BUTTON_M_Mode)
#define DEFAULT_LEDMATRIX_MODE	0

	;; if != 0, special variations for MB6582 hardware will be selected
#define DEFAULT_MB6582_CS	0

	;; if != 0, special variations for sammichSID hardware will be selected
#define DEFAULT_SAMMICHSID_CS	0

	;; if != 0, EDIT button won't store patch
	;; actually this doesn't really make sense - but this function could be useful if you
	;; want to demonstrate your MBSID to friends and ensure, that they won't unintentionally
	;; overwrite your patches
#define DEFAULT_DISABLE_EDIT_STORE_FUNCTION 0

	;; this switch enables the CC#32 filter for programs which send this CC always
	;; together with CC#0 on bank changes
#define DEFAULT_CC32_FILTER 0

	;; Enables special testmode to check different encoder speed values.
	;; Should be disabled again for "productive usage" of the MIDIbox SID!
	;; If enabled, the encoder speed doesn't depend on the value range of the modified parameter.
	;; Instead, it can be set in a special configuration page by pressing the SHIFT button.
	;; Use this mode to find the best matching values for your encoders.
#define DEFAULT_TESTMODE_ENC_SPEED 0


	;; the SHIFT button allows to alternate the encoder speed
	;; select the desired behaviour here:
	;; 0: (default) encoders are fast by default, by pressing the SHIFT button encoders are slower
	;; 1: encoders are slow by default, by pressing the SHIFT button encoders are faster
#define DEFAULT_SHIFT_SPEED_CONTROL_MODE 0


	org	0x3082		; never change the origin!
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
;  or remove the entry
;
;  Add +16 to the SR number if the button is part of the button matrix
;  (DEFAULT_SRM_BUTTONS_DIN must be > 0!)
;
;  The table must be terminated with DIN_ENTRY_EOT!
; ==========================================================================

DIN_ENTRY MACRO function, sr, pin
	dw	function, (pin + 8*(sr-1)) & 0xff
	ENDM

DIN_ENTRY_EOT MACRO
	dw	0x0000, 0x0000
	ENDM
	
CS_MENU_DIN_TABLE
	;;		Function name		SR#	Pin#
	DIN_ENTRY	CS_MENU_BUTTON_Dec,	 1,	 0	; only valid if rotary encoder not assigned to these pins
	DIN_ENTRY	CS_MENU_BUTTON_Inc,	 1,	 1	; (see MIOS_ENC_PIN_TABLE) and CS_MENU_USE_INCDEC_BUTTONS == 1
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
	DIN_ENTRY	CS_MENU_BUTTON_Shift,	 2,	 4	; was: link button
	DIN_ENTRY	CS_MENU_BUTTON_CC_PageUp, 2,     5	; combined CC/PageUp -- CC actived together with shift button (no error)
	DIN_ENTRY	CS_MENU_BUTTON_Edit_PageDown, 2, 6	; combined Edit/PageDown -- Edit actived together with shift button (no error)

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

	;; new for MBSID V2 (additional ***optional*** buttons)
	;; don't worry, you still have full access to all functions w/o these buttons!
	;; note that you could also re-arrange the pin assignments if required (e.g. if you don't like a certain button function)
	DIN_ENTRY	CS_MENU_BUTTON_M_Vol,	 9,	 5	; matrix: button below M_Filter
	DIN_ENTRY	CS_MENU_BUTTON_Play,	 9,	 6	; direct access to play function
	DIN_ENTRY	CS_MENU_BUTTON_SID_LR,	 9,	 7	; direct access to L/R toggling
	DIN_ENTRY	CS_MENU_BUTTON_M_Mode,	10,	 0	; direct access to meter on/off function
	DIN_ENTRY	CS_MENU_BUTTON_Fil_ExtIn, 10,	 1	; direct access to Filter ExtIn Flag
	DIN_ENTRY	CS_MENU_BUTTON_Sync,	10,	 2	; jumps to ENS->CLK menu
	DIN_ENTRY	CS_MENU_BUTTON_Arp,	 0,	 0	; enables/disables the arpeggiator of *all* oscillators
	DIN_ENTRY	CS_MENU_BUTTON_Knob,     0,      0	; changes to knob menu

	;; don't remove this "end-of-table" entry!
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
;     SR =  0 for J5 pins (note: the DEFAULT_J5_FUNCTION switch has to be set to "2" to get this working)
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
;  Add +16 to the SR number if the LED is part of the LED matrix
;  (DEFAULT_SRM_LEDS_DOUT must be > 0!)
;
;  The table must be terminated with DOUT_ENTRY_EOT!
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

	DOUT_ENTRY	CS_MENU_MODE, 0,			1,	4	; Shift LED
	DOUT_ENTRY	CS_MENU_MODE, 1,			1,	5	; CC LED
	DOUT_ENTRY	CS_MENU_MODE, 4,			1,	6	; Edit LED

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


	;; additional LED functions which could be added:
	;; o Play LED (TMP5, 6)
	;; o Mode Meter LED (TMP2, 2)
	;; o Mode Matrix LED (TMP2, 3)
	;; o SID L LED (TMP4, 6)
	;; o SID R LED (TMP4, 7)
	;; o LFO Positive LED (TMP5, 5)

	;; don't remove this "end-of-table" entry!
	DOUT_ENTRY_EOT


;; --------------------------------------------------------------------------
;; In this table DIN pins have to be assigned to rotary encoders for the
;; MIOS_ENC driver 
;; 
;; up to 64 entries are provided, for MIDIbox SID 15 entries are expected
;; 
;; The table must be terminated with an ENC_EOT entry.
;; 
;; ENC_ENTRY provides following parameters
;;    o first parameter: number of shift register - 1, 2, 3, ... 16
;;    o second parameter: number of pin; since two pins are necessary
;;      for each encoder, an even number is expected: 0, 2, 4 or 6
;;    o the third parameter contains the encoder mode:
;;      either MIOS_ENC_MODE_NON_DETENTED
;;          or MIOS_ENC_MODE_DETENTED1
;;          or MIOS_ENC_MODE_DETENTED2
;;          or MIOS_ENC_MODE_DETENTED3
;;      see also http://www.midibox.org/dokuwiki/doku.php?id=encoder_types
;;
;; Configuration Examples:
;;    ENC_ENTRY  1,  0,  MIOS_ENC_MODE_NON_DETENTED    ; non-detented encoder at pin 0 and 1 of SR 1
;;    ENC_ENTRY  1,  2,  MIOS_ENC_MODE_DETENTED        ; detented encoder at pin 2 and 3 of SR 1
;;    ENC_ENTRY  9,  6,  MIOS_ENC_MODE_NON_DETENTED    ; non-detented encoder at pin 6 and 7 of SR 9
;; --------------------------------------------------------------------------
	org	0x3280		; never change the origin!

	;; encoder entry structure
ENC_ENTRY MACRO sr, din_0, mode
	dw	(mode << 8) | (din_0 + 8*(sr-1))
	ENDM	
ENC_EOT	MACRO
	dw	0xffff
	ENDM

MIOS_ENC_PIN_TABLE
	;;        SR  Pin  Mode
#if CS_MENU_USE_INCDEC_BUTTONS == 0
	ENC_ENTRY  1,  0,  MIOS_ENC_MODE_DETENTED3	; menu encoder
#endif

	;; additional CS encoders
	;;        SR  Pin  Mode
	ENC_ENTRY  3,  0,  MIOS_ENC_MODE_DETENTED3	; Osc delay/transpose/assign #1
	ENC_ENTRY  3,  2,  MIOS_ENC_MODE_DETENTED3	; Osc attack/finetune/assign #2
	ENC_ENTRY  3,  4,  MIOS_ENC_MODE_DETENTED3	; Osc decay/portamento/assign #3
	ENC_ENTRY  3,  6,  MIOS_ENC_MODE_DETENTED3	; Osc sustain/release/assign #4
	ENC_ENTRY  4,  0,  MIOS_ENC_MODE_DETENTED3	; Osc release/pulsewidth/assign #5

	ENC_ENTRY  4,  6,  MIOS_ENC_MODE_DETENTED3	; LFO rate
	ENC_ENTRY  5,  0,  MIOS_ENC_MODE_DETENTED3	; LFO depth

	ENC_ENTRY  5,  4,  MIOS_ENC_MODE_DETENTED3	; Filter CutOff
	ENC_ENTRY  5,  6,  MIOS_ENC_MODE_DETENTED3	; Filter Resonance

	ENC_ENTRY  6,  0,  MIOS_ENC_MODE_DETENTED3	; Env depth/assign #1
	ENC_ENTRY  6,  2,  MIOS_ENC_MODE_DETENTED3	; Env attack/assign #2
	ENC_ENTRY  6,  4,  MIOS_ENC_MODE_DETENTED3	; Env decay/assign #3
	ENC_ENTRY  6,  6,  MIOS_ENC_MODE_DETENTED3	; Env sustain/assign #4
	ENC_ENTRY  7,  0,  MIOS_ENC_MODE_DETENTED3	; Env release/assign #5

	;; don't remove this "end-of-table" entry!
	ENC_EOT


;; include the rest of the application
#include "src/main.inc"
