; $Id$
	LIST R=DEC
;
; Default setup File for MIDIbox SEQ Vintage (sequencer for MIDIbox64)
;
; Detailed infos for customization can be found at http://www.ucapps.de/midibox_seq_options.html
;
; define the size of the display which is connected to the core:
;    0: 2x16
;    1: 2x20
;    2: 2x40
;    3: 2 * 2x40
#define DEFAULT_LCD_SIZE 0
;
; Address of BankStick which stores the patterns
; use 0-7 to select the CS address
; or use -1 to deactivate the BankStick (in this case only one internal pattern is available)
#define DEFAULT_BANKSTICK_PATTERN_CS 0
;
; Address of BankStick which stores the song
; use 0-7 to select the CS address
; if the same address is used like for the patterns, a 64k EEPROM has to be used (24LC512)
; use -1 to deactivate the BankStick (in this case the song mode doesn't work)
#define DEFAULT_BANKSTICK_SONG_CS -1
;
; Max. length of the DIN/DOUT shift register chain (1-16)
#define DEFAULT_NUMBER_SR	10
;
; Number of connected pots - allowed values: 1-64
#define DEFAULT_NUMBER_POTS	 64
;
; Are multiplexers (AINX4 modules) connected? -> set this value to 1
; No multiplexer connected -> set this value to 0, allowed number of pots in this case: 1-8
#define DEFAULT_MUX_ENABLED	 1
;
; debounce counter (see the function description of MIOS_SRIO_DebounceSet)
; Use 0 for high-quality buttons, use higher values for low-quality buttons
#define DEFAULT_SRIO_DEBOUNCE_CTR 32
;
; For MIDI activity monitor: define the DOUT pins for the Rx and Tx LED
#define DEFAULT_MIDI_MONITOR_ENABLED 1  ; if 1, the Tx/Rx LEDs are enabled
#define DEFAULT_MIDI_RX_LED 0x1f	; DOUT SR#4, pin D0
#define DEFAULT_MIDI_TX_LED 0x20	; DOUT SR#5, pin D7
;
; The beat indicator LED has to be assigned to a DOUT pin here:
#define DEFAULT_BEAT_INDICATOR_LED 0x17	; DOUT SR#3, pin D0
;
;
; Some menus provide the possibility to use 16 "general purpose" buttons
; Define the two shift registers which are assigned to this function here:
; (valid numbers: 1-16)
#define DEFAULT_GP_DIN_SR0	2	; first GP DIN shift register assigned to SR#2
#define DEFAULT_GP_DIN_SR1	3	; second GP DIN shift register assigned to SR#3
;
; above these buttons LEDs should be mounted to visualize the played MIDI events,
; but also the current sequencer position, the selected pattern, the menu, etc.
; Define the two shift registers which are assigned to this function here:	
; (valid numbers: 1-16)
#define DEFAULT_GP_DOUT_SR0	1	; first GP DOUT shift register assigned to SR#1
#define DEFAULT_GP_DOUT_SR1	2	; second GP DOUT shift register assigned to SR#2
;
; set this value to 1 if each track has its own set of 16 LEDs to display the 
; unmuted steps and the current sequencer position
#define DEFAULT_TRACK_LEDS_ENABLED 0
; define the DOUT pins to which the cathodes of these LEDs are connected
#define DEFAULT_TRACK_LEDS_1_CATHODE 0x27	; Track 1 and GP LEDs
#define DEFAULT_TRACK_LEDS_2_CATHODE 0x26	; Track 2
#define DEFAULT_TRACK_LEDS_3_CATHODE 0x25	; Track 3
#define DEFAULT_TRACK_LEDS_4_CATHODE 0x24	; Track 4
; (the anodes are connected to the shift registers which are defined by DEFAULT_GP_DOUT_SRx)
;
; Datawheel for menu navigation/data entry connected or not?
; A datawheel can replace the left/right buttons!
; if -1: datawheel not connected
; if 0: encoder #0 assigned to datawheel function
; values > 0 not allowed in the MIDIboxSEQ application
; don't forget to define the pins of the datawheel in MIOS_ENC_PIN_TABLE
; (-> mios_tables.inc or setup_*.asm, first entry)
#define DEFAULT_ENC_DATAWHEEL   -1
;
; the speed value for the datawheel (#0) which is used when the "FAST" button is activated:
#define DEFAULT_DATAWHEEL_SPEED_VALUE 3
;
; the speed value for the additional encoders (#1-#16) which is used when the "FAST" button is activated:
#define DEFAULT_ENC_SPEED_VALUE 3

;; Behaviour of FAST button:
;; 0: button pressed: fast encoders, button depressed: slow encoders
;; 1: fast mode toggled when button pressed
#define DEFAULT_BEHAVIOUR_BUTTON_FAST	1
;; The same for the "ALL" button (if flag set, all step parameters will be changed with a single encoder)
#define DEFAULT_BEHAVIOUR_BUTTON_ALL	0
;; the same for the solo button (if flag set, the current selected track will be played solo)
#define DEFAULT_BEHAVIOUR_BUTTON_SOLO	1
;; the same for the metronome button
#define DEFAULT_BEHAVIOUR_BUTTON_METRON	1
;; the same for the scrub button
#define DEFAULT_BEHAVIOUR_BUTTON_SCRUB	0

; MIDI IN Channel which is used for the transpose/arpeggiator/remote function
; Allowed values: 1-16, select 0 to disable
#define DEFAULT_SEQUENCER_CHANNEL	1
;
; The Note number which activates the remote function (examples: 0x24 == C-2, 0x60 == C-7)
#define DEFAULT_MIDI_REMOTE_KEY		0x60	; C-7
;
; Optional channels for configuration via CC (1-16, select 0 to disable)
#define DEFAULT_MIDI_CHANNEL_LAYER_A	0
#define DEFAULT_MIDI_CHANNEL_LAYER_B	0
#define DEFAULT_MIDI_CHANNEL_LAYER_C	0
#define DEFAULT_MIDI_CHANNEL_CFG	0
;
; which notes should be played by the metronome?
; (the channel can be changed in the appr. menu)
; every 16th step:
#define DEFAULT_METRONOME_NOTE_MEASURE	0x25	; (C#1)
#define DEFAULT_METRONOME_VEL_MEASURE	0x7f	; 127
; every 4th step:
#define DEFAULT_METRONOME_NOTE_BEAT	0x25	; (C#1)
#define DEFAULT_METRONOME_VEL_BEAT	0x60	; 96

;; 0: old style - BPM can be divided by 1, 2, 4, 8, 16, 32, 64
;; 1: new style - BPM can be divided by 1..16
#define DEFAULT_BPM_DIVIDER_BEHAVIOUR	1

;; define the AOUT interface which is used here:
;;   1: one MBHP_AOUT module
;;   2: up to 4 (chained) MBHP_AOUT_LC modules
;;   3: one MBHP_AOUT_NG module
;; all other values invalid!
#define AOUT_INTERFACE_TYPE 1

;; only relevant if one or more AOUT_LC modules are used:
;; define the resolution configuration here
;;   0: first channel 12bit, second channel 4bit
;;   1: first channel 8bit, second channel 8bit
;;   2: combines M1,M2 and/or M3/M4: first channel 12bit, second channel 12bit, third channel 8bit, fourth channel ignored!
;; all other values invalid!
#define AOUT_LC_RESOLUTION_OPTION_M1 1
#define AOUT_LC_RESOLUTION_OPTION_M2 1
#define AOUT_LC_RESOLUTION_OPTION_M3 1
#define AOUT_LC_RESOLUTION_OPTION_M4 1

;; define the pin which should be used as external 24ppqn clock output here
;; DEFAULT_EXT_CLK_LAT can be LATC (Pin 0, 1, 2, 4, 5) or LATD (Pin 4)
;; Note that this should be an exclusive pin. E.g., if an AOUT module is
;; be connected, LATC.5 and LATD.4 are already allocated. If an AIN module
;; is connected, LATC.0, .1 and .2 are already allocated
;; if a second LCD is connected, LATC.4 is already allocated
;; The external pin is disabled with DEFAULT_EXT_CLK_LAT == 0
#define DEFAULT_EXT_CLK_LAT	0	; don't use with AIN module!
#define DEFAULT_EXT_CLK_PIN	0

;; the external clock can be divided, the setting can also be changed in the BPM menu
;; 0: 96 ppqn (won't work, clock permanently 1)
;; 1: 48 ppqn
;; 2: 32 ppqn
;; 3: 24 ppqn (standard)
;; 4: 24 ppqn / 2
;; 5: 24 ppqn / 3
;; etc...
#define DEFAULT_EXT_CLK_DIVIDER 3


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
;
;  The table must end with DIN_ENTRY_EOT!
; ==========================================================================

DIN_ENTRY MACRO function, sr, pin
	dw	function, (pin + 8*(sr-1))
	ENDM

DIN_ENTRY_EOT MACRO
	dw	0x0000, 0x0000
	ENDM
	
SEQ_IO_TABLE_DIN
	DIN_ENTRY	SEQ_BUTTON_All,		 1,	 0
	DIN_ENTRY	SEQ_BUTTON_LayerA,	 1,	 1
	DIN_ENTRY	SEQ_BUTTON_LayerB,	 1,	 2
	DIN_ENTRY	SEQ_BUTTON_LayerC,	 1,	 3
	DIN_ENTRY	SEQ_BUTTON_Select,	 1,	 4
	DIN_ENTRY	SEQ_BUTTON_Left,	 1,	 5
	DIN_ENTRY	SEQ_BUTTON_Right,	 1,	 6
	DIN_ENTRY	SEQ_BUTTON_Exit,	 1,	 7

	;; GP buttons assigned to the shift registers above!

	DIN_ENTRY	SEQ_BUTTON_Track1,	 4,	 0
	DIN_ENTRY	SEQ_BUTTON_Track2,	 4,	 1
	DIN_ENTRY	SEQ_BUTTON_Track3,	 4,	 2
	DIN_ENTRY	SEQ_BUTTON_Track4,	 4,	 3
	DIN_ENTRY	SEQ_BUTTON_Solo,	 4,	 4
	DIN_ENTRY	SEQ_BUTTON_Menu,	 4,	 5
	DIN_ENTRY	SEQ_BUTTON_Stop,	 4,	 6
	DIN_ENTRY	SEQ_BUTTON_Play,	 4,	 7

	DIN_ENTRY	SEQ_BUTTON_Pause,	 5,	 0
	DIN_ENTRY	SEQ_BUTTON_Rew,		 5,	 1
	DIN_ENTRY	SEQ_BUTTON_Fwd,		 5,	 2
	DIN_ENTRY	SEQ_BUTTON_Metronome,	 5,	 3
	DIN_ENTRY	SEQ_BUTTON_Edit,	 5,	 4
	DIN_ENTRY	SEQ_BUTTON_Mute,	 5,	 5
	DIN_ENTRY	SEQ_BUTTON_Pattern,	 5,	 6
	DIN_ENTRY	SEQ_BUTTON_Song,	 5,	 7

	DIN_ENTRY	SEQ_BUTTON_F1,		 6,	 0
	DIN_ENTRY	SEQ_BUTTON_F2,		 6,	 1
	DIN_ENTRY	SEQ_BUTTON_F3,		 6,	 2
	DIN_ENTRY	SEQ_BUTTON_F4,		 6,	 3
	DIN_ENTRY	SEQ_BUTTON_Scrub,	 6,	 4
	DIN_ENTRY	SEQ_BUTTON_Fast,	 6,	 5	; not relevant for MB64SEQ Vintage

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
;     SR =  1 for the first DOUT shift register
;     SR =  2 for the second DOUT shift register
;     ...
;     SR = 16 for the last DOUT shift register
;
;     Pin = 0 for the D7 output pin of the shift register
;     Pin = 1 for the D6 output pin of the shift register
;     ...
;     Pin = 7 for the last input pin (D0) of the shift register
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
	
SEQ_IO_TABLE_DOUT
	;;		Register and bit			SR#	Pin#	  Description
	DOUT_ENTRY	TMP1, 0,				3,	0	; Track #1 selected (Note: Pin #0 is the D7 output of first SR)
	DOUT_ENTRY	TMP1, 1,				3,	1	; Track #2 selected
	DOUT_ENTRY	TMP1, 2,				3,	2	; Track #3 selected
	DOUT_ENTRY	TMP1, 3,				3,	3	; Track #4 selected

	DOUT_ENTRY	TMP1, 4,				3,	4	; Layer A selected
	DOUT_ENTRY	TMP1, 5,				3,	5	; Layer B selected
	DOUT_ENTRY	TMP1, 6,				3,	6	; Layer C selected

	;; NOTE: the pin of the beat indicator LED has to be assigned at the header

	DOUT_ENTRY	TMP2, 0,				4,	0	; Edit Step LED
	DOUT_ENTRY	TMP2, 1,				4,	1	; Mute LED
	DOUT_ENTRY	TMP2, 2,				4,	2	; Pattern LED
	DOUT_ENTRY	TMP2, 3,				4,	3	; Song LED
	DOUT_ENTRY	TMP2, 7,				4,	4	; Solo LED
	DOUT_ENTRY	TMP2, 5,				4,	5	; Fast Encoder LED
	DOUT_ENTRY	TMP2, 6,				4,	6	; Change All Steps LED

	;; NOTE: the pins of the MIDI Rx/Tx LEDs are assigned at the header
	;; NOTE: the pins of the 16 general purpose LEDs are assigned at the header
	DOUT_ENTRY_EOT


	org	0x3280		; never change the origin!
;; --------------------------------------------------------------------------
;; In this table DIN pins have to be assigned to rotary encoders for the
;; MIOS_ENC driver 
;; 
;; up to 64 entries are provided
;; 
;; The table must be terminated with an ENC_EOT entry. Unused entries should
;; be filled with ENC_EOT
;; 
;; ENC_ENTRY provides following parameters
;;    o first parameter: number of shift register - 1, 2, 3, ... 16
;;    o second parameter: number of pin; since two pins are necessary
;;      for each encoder, an even number is expected: 0, 2, 4 or 6
;;    o the third parameter contains the encoder mode:
;;      either MIOS_ENC_MODE_NON_DETENTED
;;          or MIOS_ENC_MODE_DETENTED
;;          or MIOS_ENC_MODE_DETENTED2
;;          or MIOS_ENC_MODE_DETENTED3
;;
;; Configuration Examples:
;;    ENC_ENTRY  1,  0,  MIOS_ENC_MODE_NON_DETENTED    ; non-detented encoder at pin 0 and 1 of SR 1
;;    ENC_ENTRY  1,  2,  MIOS_ENC_MODE_DETENTED        ; detented encoder at pin 2 and 3 of SR 1
;;    ENC_ENTRY  9,  6,  MIOS_ENC_MODE_NON_DETENTED    ; non-detented encoder at pin 6 and 7 of SR 9
;; --------------------------------------------------------------------------

	;; encoder entry structure
ENC_ENTRY MACRO sr, din_0, mode
	dw	(mode << 8) | (din_0 + 8*(sr-1))
	ENDM	
ENC_EOT	MACRO
	dw	0xffff
	ENDM

MIOS_ENC_PIN_TABLE
	;; encoders 1-16
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT

	;; encoders 17-32
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT

	;; encoders 33-48
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT

	;; encoders 49-64
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT


;; include the rest of the application
#include "main.inc"
