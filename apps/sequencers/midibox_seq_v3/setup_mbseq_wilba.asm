;$Id$
	LIST R=DEC
;
; Default setup File for Wilba's MIDIbox SEQ
;
; Detailed infos for customization can be found at http://www.ucapps.de/midibox_seq_options.html
;
; BankStick Mode & Allocation Map
; Each BankStick (CS0..CS7) has an own entry
; Set the value to:
;     0: ignore BankStick which could be (or is) connected to the CS address
;     1: if a 32k BankStick (24LC256) is connected and should be used as Pattern Storage (64 patterns)
;     2: if a 64k BankStick (24LC512) is connected and should be used as Pattern Storage (128 patterns)
;     3: if a 32k BankStick is connected and should be used as Song Storage (16 Songs)
;     4: if a 64k BankStick is connected and should be used as Song Storage (32 Songs)
;     5: if a 32k or 64k BankStick is connected and should be used as Mixer Map Storage
;        only the first 32k are used, so there is no advantage on using a 24LC512 here.
; NOTE: only one BankStick can be used as Song Storage, only one for Mixer Maps
;       If no entry with "mode 3" or "mode 4" is defined, Song mode will be disabled.
;       If no entry with "mode 5" is defined, there will only be one Mixer Map which cannot be stored
; If a BankStick is not connected to the MBSEQ, the appr. entry will have no effect
#define DEFAULT_BANKSTICK_MODE_CS0	2	; 64k
#define DEFAULT_BANKSTICK_MODE_CS1	2	; 64k
#define DEFAULT_BANKSTICK_MODE_CS2	5	; Mixer Map (using CS2 to ensure compatibility with Atmel EEPROMs which only provide 4 CS addresses)
#define DEFAULT_BANKSTICK_MODE_CS3	4	; Song mode (using CS3 to ensure compatibility with Atmel EEPROMs which only provide 4 CS addresses)
#define DEFAULT_BANKSTICK_MODE_CS4	2	; 64k
#define DEFAULT_BANKSTICK_MODE_CS5	2	; 64k
#define DEFAULT_BANKSTICK_MODE_CS6	2	; 64k
#define DEFAULT_BANKSTICK_MODE_CS7	2	; 64k
;
;
; Max. length of the DIN/DOUT shift register chain (1-16)
#define DEFAULT_NUMBER_SR	8
;
; debounce counter (see the function description of MIOS_SRIO_DebounceSet)
; Use 0 for high-quality buttons, use higher values for low-quality buttons
#define DEFAULT_SRIO_DEBOUNCE_CTR 32
;
;
; above these buttons LEDs should be mounted to visualize the played MIDI events,
; but also the current sequencer position, the selected pattern, the menu, etc.
; Define the two shift registers which are assigned to this function here:	
; (valid numbers: 1-16)
#define DEFAULT_GP_DOUT_SR_L	0	; first GP DOUT shift register assigned to SR#3
#define DEFAULT_GP_DOUT_SR_R	0	; second GP DOUT shift register assigned to SR#4
;
;
; === Shift Register Matrix ===
;
; set this value to 1 if each track has its own set of 16 LEDs to display unmuted steps and current sequencer position
; or if you are using a button/led matrix for misc. button/LED functions
#define DEFAULT_SRM_ENABLED     1
;
; define the shift registers to which the anodes of these LEDs are connected
; Note: they can be equal to DEFAULT_GP_DOUT_SR_[LH], this saves two shift registers, but doesn't allow a separate view of UI selections
#define DEFAULT_SRM_DOUT_L1	0
#define DEFAULT_SRM_DOUT_R1	0
;
; for misc. LED functions
#define DEFAULT_SRM_DOUT_M	2
;
; define the shift register to which the cathodes of these LEDs are connected
; Note that the whole shift register (8 pins) will be allocated! The 4 select lines are duplicated (4 for LED matrix, 4 for button matrix)
; The second DOUT_CATHODES2 selection is optional if LEDs with high power consumption are used - set this to 0 if not used
#define DEFAULT_SRM_DOUT_CATHODES1	0
#define DEFAULT_SRM_DOUT_CATHODES2	0
;
; another select line for misc. button/led functions - all 8 select pins are used for a 8x8 button/led matrix
#define DEFAULT_SRM_DOUT_CATHODESM	1
;
; set an inversion mask for the DOUT shift registers if sink drivers (transistors)
; have been added to the cathode lines
; Settings: 0x00 - no sink drivers
;           0xf0 - sink drivers connected to D0..D3
;           0x0f - sink drivers connected to D7..D4
#define DEFAULT_SRM_CATHODES_INV_MASK   0x00
;
; same for misc. button/led functions
#define DEFAULT_SRM_CATHODES_INV_MASK_M 0x00
;
; set this to 1, if DUO colour LEDs are connected to the LED matrix
#define DEFAULT_SRM_DOUT_DUOCOLOUR	0
;
; define the shift registers to which the anodes of the "second colour" (red) LEDs are connected
#define DEFAULT_SRM_DOUT_L2	0
#define DEFAULT_SRM_DOUT_R2	0
;
; set this to 1 if a button matrix is connected
#define DEFAULT_SRM_BUTTONS_ENABLED 0
; set this to 1 if these buttons should only control the "step triggers" (gate, and other assigned triggers) - and no UI functions
#define DEFAULT_SRM_BUTTONS_NO_UI   1
; define the DIN shift registers to which the button matrix is connected
#define DEFAULT_SRM_DIN_L	0
#define DEFAULT_SRM_DIN_R	0
;
; 8x8 matrix for misc. button functions
#define DEFAULT_SRM_DIN_M	2
;
;
; === BPM digits ===
;
; set to 1 or 2 to enable the 3 optional BPM digits
;    0: BPM digits disabled
;    1: digits with common cathode
;    2: digits with common anode
#define DEFAULT_BPM_DIGITS_ENABLED 0
;
; define the shift register to which the segments are connected (0=disabled)
#define DEFAULT_BPM_DIGITS_SEGMENTS	5
; define the shift register to which the common pins (cathodes or anodes) are connected (0=disabled)
#define DEFAULT_BPM_DIGITS_COMMON	6
;
;
; Datawheel for menu navigation/data entry connected or not?
; A datawheel can replace the left/right buttons!
; if -1: datawheel not connected
; if 0: encoder #0 assigned to datawheel function
; values > 0 not allowed in the MIDIboxSEQ application
; don't forget to define the pins of the datawheel in MIOS_ENC_PIN_TABLE
; (-> mios_tables.inc or setup_*.asm, first entry)
#define DEFAULT_ENC_DATAWHEEL	0
;
; the speed value for the datawheel (#0) which is used when the "FAST" button is activated:
#define DEFAULT_DATAWHEEL_SPEED_VALUE	3
;
; the speed value for the additional encoders (#1-#16) which is used when the "FAST" button is activated:
#define DEFAULT_ENC_SPEED_VALUE		3

;; Auto FAST mode: if a layer is assigned to velocity or CC, the fast button will be automatically
;; enabled - in other cases (e.g. Note or Length), the fast button will be automatically disabled
#define DEFAULT_AUTO_FAST_BUTTON        1

;; Behaviour of FAST button:
;; 0: button pressed: fast encoders, button depressed: slow encoders
;; 1: fast mode toggled when button pressed
#define DEFAULT_BEHAVIOUR_BUTTON_FAST	1
;; The same for the "ALL" button (if flag set, all step parameters will be changed with a single encoder)
#define DEFAULT_BEHAVIOUR_BUTTON_ALL	1
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

;; if enabled (1), patterns can be changed via CC#112-CC#115
;; if disabled (0), these CCs are ignored
#define DEFAULT_MIDI_PATTERN_CHANGE_CC	1

;; if enabled (1), song can be changed via program change
;; if disabled (0), program changes are ignored
#define DEFAULT_MIDI_SONG_CHANGE_VIA_PC	1


;; define the pin which should be used as external 24ppqn clock output here
;; DEFAULT_EXT_CLK_LAT can be LATC (Pin 0, 1, 2, 4, 5) or LATD (Pin 4)
;; Note that this should be an exclusive pin. E.g., if an AOUT module is
;; be connected, LATC.5 and LATD.4 are already allocated.
;; if a second LCD is connected, LATC.4 is already allocated
;; The external pin is disabled with DEFAULT_EXT_CLK_LAT == 0
;; default setting: RC0 (available at J6:RC of the core module)
#define DEFAULT_EXT_CLK_LAT	LATC
#define DEFAULT_EXT_CLK_PIN	0

;; define the pin which should be used as external Start/Stop output here
;; default setting: RC1 (available at J6:SC of the core module)
#define DEFAULT_EXT_START_LAT	LATC
#define DEFAULT_EXT_START_PIN	1


;; define the pin to which the RI_N line of the IIC1 module is connected
;; this connections is required for the second MIDI IN port
;; Note: MIDI In is only supported for one MBHP_IIC_MIDI module,
;; and it must be the first one (IIC address 10, both jumpers stuffed)
;; By default, RC3 is used (available at J6:SI of the core module)
;; if RI_N_PORT == 0, the second MIDI Input will be disabled.
#define DEFAULT_IIC1_RI_N_PORT	PORTC
#define DEFAULT_IIC1_RI_N_TRIS	TRISC
#define DEFAULT_IIC1_RI_N_PIN	3


;; define the AOUT interface which is used here:
;;   1: one MBHP_AOUT module
;;   2: up to 4 (chained) MBHP_AOUT_LC modules
;;   3: one MBHP_AOUT_NG module
;; all other values invalid!
#define AOUT_INTERFACE_TYPE 3

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


;; use PORTA and PORTE (J5 of the core module) as trigger output
;; NEVER USE THIS TOGETHER WITH ANALOG POTS - IT WILL CAUSE A SHORT CIRCUIT!
#define DEFAULT_ENABLE_J5_GATES	0

;; additional gate triggers are available on common digital output pins of the
;; DOUT shift register chain - they are assigned to AOUT channel #16 (Note C-1, C#1, D-1, ...)
;; define the shift registers which should be used here (each provides 8 gates)
;; Note that SRs assigned to this function cannot be used as LED outputs (exclusive function)
;; Allowed values: 1-16, 0 disables the function, all other values invalid and not allowed
;; Note: depending on the shift registers you are using, you have to adjust
;; the DEFAULT_NUMBER_SR value at the top of this configuration
#define DEFAULT_ENABLE_DOUT_GATE_09_16   0
#define DEFAULT_ENABLE_DOUT_GATE_17_24   0
#define DEFAULT_ENABLE_DOUT_GATE_25_31   0
#define DEFAULT_ENABLE_DOUT_GATE_32_39   0
#define DEFAULT_ENABLE_DOUT_GATE_40_47   0
#define DEFAULT_ENABLE_DOUT_GATE_48_55   0

;; if set to 1, the DOUT "gates" will send 1mS pulses
;; useful for analog drums
#define DEFAULT_DOUT_1MS_TRIGGER	0


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
;     SR = 17 for the first DIN shift register of optional "misc. button matrix"
;     ...
;     SR = 24 for the last DIN shift register of optional "misc. button matrix"
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
	dw	function, (pin + 8*((sr-1)&0xff))
	ENDM

DIN_ENTRY_EOT MACRO
	dw	0x0000, 0x0000
	ENDM
	
SEQ_IO_TABLE_DIN
	;;		Function name		SR#	Pin#
#if DEFAULT_ENC_DATAWHEEL < 0
	DIN_ENTRY	CS_MENU_BUTTON_Left,	 0,	 0
	DIN_ENTRY	CS_MENU_BUTTON_Right,	 0,	 0
#endif

	DIN_ENTRY	SEQ_BUTTON_Scrub,	 21,	 4
	DIN_ENTRY	SEQ_BUTTON_Metronome,	 21,	 5

	DIN_ENTRY	SEQ_BUTTON_Stop,	 22,	 5
	DIN_ENTRY	SEQ_BUTTON_Pause,	 23,	 4
	DIN_ENTRY	SEQ_BUTTON_Play,	 23,	 5
	DIN_ENTRY	SEQ_BUTTON_Rew,		 22,	 4
	DIN_ENTRY	SEQ_BUTTON_Fwd,		 24,	 5

	DIN_ENTRY	SEQ_BUTTON_Utility,	 22,	 2
	DIN_ENTRY	SEQ_BUTTON_F1,		 22,	 3
	DIN_ENTRY	SEQ_BUTTON_F2,		 23,	 2
	DIN_ENTRY	SEQ_BUTTON_F3,		 23,	 3
	DIN_ENTRY	SEQ_BUTTON_F4,		 24,	 2

	DIN_ENTRY	SEQ_BUTTON_Menu,	 18,	 5
	DIN_ENTRY	SEQ_BUTTON_Select,	 17,	 4
	DIN_ENTRY	SEQ_BUTTON_Exit,	 17,	 5

	DIN_ENTRY	SEQ_BUTTON_Track1,	 18,	 6
	DIN_ENTRY	SEQ_BUTTON_Track2,	 18,	 7
	DIN_ENTRY	SEQ_BUTTON_Track3,	 20,	 6
	DIN_ENTRY	SEQ_BUTTON_Track4,	 20,	 7

	DIN_ENTRY	SEQ_BUTTON_LayerA,	 21,	 6
	DIN_ENTRY	SEQ_BUTTON_LayerB,	 21,	 7
	DIN_ENTRY	SEQ_BUTTON_LayerC,	 22,	 6

	DIN_ENTRY	SEQ_BUTTON_Edit,	 20,	 3
	DIN_ENTRY	SEQ_BUTTON_Mute,	 19,	 2
	DIN_ENTRY	SEQ_BUTTON_Pattern,	 19,	 3
	DIN_ENTRY	SEQ_BUTTON_Song,	 18,	 2

	DIN_ENTRY	SEQ_BUTTON_Solo,	 19,	 4
	DIN_ENTRY	SEQ_BUTTON_Fast,	 19,	 5
	DIN_ENTRY	SEQ_BUTTON_All,		 18,	 4

	DIN_ENTRY	SEQ_BUTTON_GP1,		 20,	 0
	DIN_ENTRY	SEQ_BUTTON_GP2,		 20,	 1
	DIN_ENTRY	SEQ_BUTTON_GP3,		 19,	 0
	DIN_ENTRY	SEQ_BUTTON_GP4,		 19,	 1
	DIN_ENTRY	SEQ_BUTTON_GP5,		 18,	 0
	DIN_ENTRY	SEQ_BUTTON_GP6,		 18,	 1
	DIN_ENTRY	SEQ_BUTTON_GP7,		 17,	 0
	DIN_ENTRY	SEQ_BUTTON_GP8,		 17,	 1
	DIN_ENTRY	SEQ_BUTTON_GP9,		21,	 0
	DIN_ENTRY	SEQ_BUTTON_GP10,	21,	 1
	DIN_ENTRY	SEQ_BUTTON_GP11,	22,	 0
	DIN_ENTRY	SEQ_BUTTON_GP12,	22,	 1
	DIN_ENTRY	SEQ_BUTTON_GP13,	23,	 0
	DIN_ENTRY	SEQ_BUTTON_GP14,	23,	 1
	DIN_ENTRY	SEQ_BUTTON_GP15,	24,	 0
	DIN_ENTRY	SEQ_BUTTON_GP16,	24,	 1

	;; OPTIONAL! see CHANGELOG.txt
	DIN_ENTRY	SEQ_BUTTON_Group1,	17,	 6
	DIN_ENTRY	SEQ_BUTTON_Group2,	17,	 7
	DIN_ENTRY	SEQ_BUTTON_Group3,	19,	 6
	DIN_ENTRY	SEQ_BUTTON_Group4,	19,	 7

	;; OPTIONAL! see CHANGELOG.txt
	DIN_ENTRY	SEQ_BUTTON_TrgLayerA,	23,	 6
	DIN_ENTRY	SEQ_BUTTON_TrgLayerB,	23,	 7
	DIN_ENTRY	SEQ_BUTTON_TrgLayerC,	24,	 6

	;; OPTIONAL! see CHANGELOG.txt
	DIN_ENTRY	SEQ_BUTTON_ToggleStepView, 20,	 5

	;; OPTIONAL! see CHANGELOG.txt
	DIN_ENTRY	SEQ_BUTTON_TapTempo,	1,	 0

	DIN_ENTRY	CS_MENU_BUTTON_Left,	 21,	 2	; "Dec" button. Can replace with CS_MENU_BUTTON_LeftRemote
	DIN_ENTRY	CS_MENU_BUTTON_Right,	 21,	 3	; "Inc" button. Can replace with CS_MENU_BUTTON_RightRemote

	DIN_ENTRY_EOT


; ==========================================================================
;  Following statements are used to assign LED functions to DOUT pins
;
;  To enable a LED function, specify the shift register number SR (1-16),
;  and the pin number (0-7).
;  Note that Pin 0 is D7 of the DOUT register, Pin 1 is D6, ... Pin 7 is D0
;
;  LED assignments for optional "misc. LED matrix": use SR 17-24
;
;  With SR value = 0, the LED function will be disabled
; ==========================================================================

;;                         SR    ignore    Pin
LED_TRACK1	EQU	(((18   -1)<<3)+    2)
LED_TRACK2	EQU	(((18   -1)<<3)+    1)
LED_TRACK3	EQU	(((20   -1)<<3)+    2)
LED_TRACK4	EQU	(((20   -1)<<3)+    1)

;;                         SR    ignore    Pin
LED_LAYER_A	EQU	(((21   -1)<<3)+    2)
LED_LAYER_B	EQU	(((21   -1)<<3)+    1)
LED_LAYER_C	EQU	(((21   -1)<<3)+    0)

;;                         SR    ignore    Pin
LED_BEAT	EQU	(((24   -1)<<3)+    1)

;;                         SR    ignore    Pin
LED_EDIT	EQU	(((20   -1)<<3)+    3)
LED_MUTE	EQU	(((19   -1)<<3)+    3)
LED_PATTERN	EQU	(((19   -1)<<3)+    2)
LED_SONG	EQU	(((18   -1)<<3)+    3)

;;                         SR    ignore    Pin
LED_SOLO	EQU	(((19   -1)<<3)+    1)
LED_FAST	EQU	(((19   -1)<<3)+    0)
LED_ALL		EQU	(((18   -1)<<3)+    0)

;;                         SR    ignore    Pin
LED_GROUP1	EQU	(((17   -1)<<3)+    3)
LED_GROUP2	EQU	(((17   -1)<<3)+    2)
LED_GROUP3	EQU	(((17   -1)<<3)+    1)
LED_GROUP4	EQU	(((17   -1)<<3)+    0)

;;                         SR    ignore    Pin
LED_TRG_LAYER_A	EQU	(((23   -1)<<3)+    2)
LED_TRG_LAYER_B	EQU	(((23   -1)<<3)+    1)
LED_TRG_LAYER_C	EQU	(((23   -1)<<3)+    0)

;;                         SR    ignore    Pin
LED_PLAY	EQU	(((24   -1)<<3)+    3)
LED_STOP	EQU	(((22   -1)<<3)+    3)
LED_PAUSE	EQU	(((23   -1)<<3)+    3)
LED_REW		EQU	(((22   -1)<<3)+    2)	; new
LED_FWD		EQU	(((24   -1)<<3)+    2)	; new

;;                         SR    ignore    Pin
LED_STEP_1_16	EQU	(((22   -1)<<3)+    0)	; optional
LED_STEP_17_32	EQU	(((22   -1)<<3)+    1)

;;                         SR    ignore    Pin
LED_MIDI_RX	EQU	(((0    -1)<<3)+    0)
LED_MIDI_TX	EQU	(((0    -1)<<3)+    0)


	;; NOTE: the pins of the 16 general purpose LEDs are assigned above, search for DEFAULT_GP_DOUT_SR_L and _R


;; --------------------------------------------------------------------------
;; In this table DIN pins have to be assigned to rotary encoders for the
;; MIOS_ENC driver 
;; 
;; up to 64 entries are provided, for MIDIbox SEQ 17 entries are expected
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
;;          or MIOS_ENC_MODE_DETENTED2 (prefered for Encoders from Voti and SmashTV!)
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
#if DEFAULT_ENC_DATAWHEEL >= 0
	ENC_ENTRY  6,  2,  MIOS_ENC_MODE_DETENTED2	; Data Wheel
#endif

	ENC_ENTRY  1,  6,  MIOS_ENC_MODE_DETENTED2	; V-Pot 1
	ENC_ENTRY  1,  4,  MIOS_ENC_MODE_DETENTED2	; V-Pot 2
	ENC_ENTRY  1,  2,  MIOS_ENC_MODE_DETENTED2	; V-Pot 3
	ENC_ENTRY  1,  0,  MIOS_ENC_MODE_DETENTED2	; V-Pot 4
	ENC_ENTRY  3,  6,  MIOS_ENC_MODE_DETENTED2	; V-Pot 5
	ENC_ENTRY  3,  4,  MIOS_ENC_MODE_DETENTED2	; V-Pot 6
	ENC_ENTRY  3,  2,  MIOS_ENC_MODE_DETENTED2	; V-Pot 7
	ENC_ENTRY  3,  0,  MIOS_ENC_MODE_DETENTED2	; V-Pot 8
	ENC_ENTRY  4,  6,  MIOS_ENC_MODE_DETENTED2	; V-Pot 9
	ENC_ENTRY  4,  4,  MIOS_ENC_MODE_DETENTED2	; V-Pot 10
	ENC_ENTRY  4,  2,  MIOS_ENC_MODE_DETENTED2	; V-Pot 11
	ENC_ENTRY  4,  0,  MIOS_ENC_MODE_DETENTED2	; V-Pot 12
	ENC_ENTRY  5,  6,  MIOS_ENC_MODE_DETENTED2	; V-Pot 13
	ENC_ENTRY  5,  4,  MIOS_ENC_MODE_DETENTED2	; V-Pot 14
	ENC_ENTRY  5,  2,  MIOS_ENC_MODE_DETENTED2	; V-Pot 15
	ENC_ENTRY  5,  0,  MIOS_ENC_MODE_DETENTED2	; V-Pot 16

	;; don't remove this "end-of-table" entry!
	ENC_EOT			


;; include the rest of the application
#include "main.inc"
