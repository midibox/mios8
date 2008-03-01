;$Id$
;
; Default setup File for MB808
;
; Detailed infos for customization can be found at http://www.ucapps.de/midibox_seq_options.html
; and http://www.ucapps.de/midibox_808.html
;
; define the LCD display width:
;    16: for a 2x16 display
;    20: for a 2x20 display (no additional information - screen will be centered, thats all)
;    80: for two 2x40 displays
;    other values not supported!
#define DEFAULT_LCD_WIDTH 16
;
; Following table allows you to define
;   - the track names (must consist of exactly 6 characters!)
;   - the DOUT shift registers (SR) to which the drum triggers are connected
;     (1-16; 0 disables assignment)
;   - the DOUT pin to which the drum triggers are connected (0-7)
;   - the MIDI output port (0=disabled, 1=Default, 2=Internal, 3=IIC1, 4=IIC2, 5=IIC3, 6=IIC4, 7=Trigger Only)
;   - the AOUT channel to output velocity (1-8, 0=disabled)
DEFAULT_TRKINFO MACRO
	;;       Name     SR  Pin MPort AChn
	db	"BD    ",  1,  1,   1,    0	; Track 1
	db	"SD    ",  1,  2,   1,    0	; Track 2
	db	"LT/LC ",  1,  3,   1,    0	; Track 3
	db	"MT/MC ",  1,  4,   1,    0	; Track 4
	db	"HT/HC ",  1,  5,   1,    0	; Track 5
	db	"CP    ",  1,  6,   1,    0	; Track 6
	db	"MA    ",  1,  7,   1,    0	; Track 7
	db	"RS/CL ",  4,  0,   1,    0	; Track 8
	db	"CB    ",  4,  1,   1,    0	; Track 9
	db	"CY    ",  4,  3,   1,    0	; Track 10
	db	"OH    ",  4,  4,   1,    0	; Track 11
	db	"CH    ",  4,  5,   1,    0	; Track 12
	db	"Ext1  ",  0,  0,   1,    0	; Track 13
	db	"Ext2  ",  0,  0,   1,    0	; Track 14
	db	"Ext3  ",  0,  0,   1,    0	; Track 15
	db	"Acc.  ",  7,  7,   7,    0	; Track 16
	ENDM
;
; BankStick Mode & Allocation Map
; Each BankStick (CS0..CS7) has an own entry
; Set the value to:
;     0: ignore BankStick which could be (or is) connected to the CS address
;     1: if a 32k BankStick (24LC256) is connected and should be used as Pattern Storage (64 patterns)
;     2: if a 64k BankStick (24LC512) is connected and should be used as Pattern Storage (128 patterns)
;     3: if a 32k BankStick is connected and should be used as Song Storage (16 Songs)
;     4: if a 64k BankStick is connected and should be used as Song Storage (32 Songs)
; NOTE: only one BankStick can be used as Song Storage, only one for Mixer Maps
;       If no entry with "mode 3" or "mode 4" is defined, Song mode will be disabled.
; If a BankStick is not connected to the MBSEQ, the appr. entry will have no effect
; If a "MBSEQ Mixer BankStick" is connected, it will be automatically ignored (and not re-formatted)
#define DEFAULT_BANKSTICK_MODE_CS0	2	; 64k
#define DEFAULT_BANKSTICK_MODE_CS1	2	; 64k
#define DEFAULT_BANKSTICK_MODE_CS2	2	; 64k
#define DEFAULT_BANKSTICK_MODE_CS3	4	; Song mode (using CS3 to ensure compatibility with Atmel EEPROMs which only provide 4 CS addresses)
#define DEFAULT_BANKSTICK_MODE_CS4	2	; 64k
#define DEFAULT_BANKSTICK_MODE_CS5	2	; 64k
#define DEFAULT_BANKSTICK_MODE_CS6	2	; 64k
#define DEFAULT_BANKSTICK_MODE_CS7	2	; 64k
;
;
; Max. length of the DIN/DOUT shift register chain (1-16)
#define DEFAULT_NUMBER_SR	13
;
; debounce counter (see the function description of MIOS_SRIO_DebounceSet)
; Use 0 for high-quality buttons, use higher values for low-quality buttons
#define DEFAULT_SRIO_DEBOUNCE_CTR 32
;
; For MIDI activity monitor: define the DOUT pins for the Rx and Tx LED
#define DEFAULT_MIDI_MONITOR_ENABLED 1  ; if 1, the Tx/Rx LEDs are enabled
;                                    SR            Pin#
#define DEFAULT_MIDI_RX_LED        0xff			; not used
#define DEFAULT_MIDI_TX_LED        0xff			; not used
;
; Some menus are provide the possibility to use 16 "general purpose" buttons
; Define the two shift registers which are assigned to this function here:
; (valid numbers: 1-16)
#define DEFAULT_GP_DIN_SR_L	3	; first GP DIN shift register assigned to SR#3
#define DEFAULT_GP_DIN_SR_R	4	; second GP DIN shift register assigned to SR#4
;
; above these buttons LEDs should be mounted to visualize the played MIDI events,
; but also the current sequencer position, the selected pattern, the menu, etc.
; Define the two shift registers which are assigned to this function here:	
; (valid numbers: 1-16)
#define DEFAULT_GP_DOUT_SR_L	3	; first GP DOUT shift register assigned to SR#3
#define DEFAULT_GP_DOUT_SR_R	5	; second GP DOUT shift register assigned to SR#5
;
;
; === Shift Register Matrix ===
;
; set this value to 1 if each track has its own set of 16 LEDs to display unmuted steps and current sequencer position
#define DEFAULT_SRM_ENABLED     0
;
; define the shift registers to which the anodes of these LEDs are connected
; Note: they can be equal to DEFAULT_GP_DOUT_SR_[LH], this saves two shift registers, but doesn't allow a seperate view of UI selections
#define DEFAULT_SRM_DOUT_L1	6
#define DEFAULT_SRM_DOUT_R1	9
;
; define the shift register to which the cathodes of these LEDs are connected
; Note that the whole shift register (8 pins) will be allocated! The 4 select lines are duplicated (4 for LED matrix, 4 for button matrix)
; The second DOUT_CATHODES2 selection is optional if LEDs with high power consumption are used - set this to 0 if not used
#define DEFAULT_SRM_DOUT_CATHODES1	5
#define DEFAULT_SRM_DOUT_CATHODES2	8
;
; set this to 1, if DUO colour LEDs are connected to the LED matrix
#define DEFAULT_SRM_DOUT_DUOCOLOUR	1
;
; define the shift registers to which the anodes of the "second colour" (red) LEDs are connected
#define DEFAULT_SRM_DOUT_L2	7
#define DEFAULT_SRM_DOUT_R2	10
;
; set this to 1 if a button matrix is connected
#define DEFAULT_SRM_BUTTONS_ENABLED 0
; set this to 1 if these buttons should only control the "step triggers" (gate, and other assigned triggers) - and no UI functions
#define DEFAULT_SRM_BUTTONS_NO_UI   1
; define the DIN shift registers to which the button matrix is connected
#define DEFAULT_SRM_DIN_L	11
#define DEFAULT_SRM_DIN_R	12
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
; the speed value for the tempo encoder (#0) which is used when the "FAST" button is activated:
#define DEFAULT_ENC_TEMPO_SPEED_VALUE	3
;
; the speed value for the additional encoders which is used when the "FAST" button is activated:
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
#define DEFAULT_SEQUENCER_CHANNEL	10
;
; The Note number which activates the remote function (examples: 0x24 == C-2, 0x60 == C-7)
#define DEFAULT_MIDI_REMOTE_KEY		0x60	; C-7
;
; Optional channels for configuration via CC (1-16, select 0 to disable)
#define DEFAULT_MIDI_CHANNEL_CFG	10

;; if enabled (1), patterns can be changed via program change
;; if disabled (0), program changes will be ignored
#define DEFAULT_MIDI_PATTERN_CHANGE_PC	1

;; if enabled (1-127), song can be changed via given CC#
;; if disabled (0), program changes are ignored
#define DEFAULT_MIDI_SONG_CHANGE_VIA_CC	1


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
#define AOUT_INTERFACE_TYPE 1

;; only relevant if one or more AOUT_LC modules are used:
;; define the resolution configuration here
;;   0: first channel 12bit, second channel 4bit
;;   1: first channel 8bit, second channel 8bit
;; all other values invalid!
#define AOUT_LC_RESOLUTION_OPTION_M1 1
#define AOUT_LC_RESOLUTION_OPTION_M2 1
#define AOUT_LC_RESOLUTION_OPTION_M3 1
#define AOUT_LC_RESOLUTION_OPTION_M4 1


;; 0: disables swing pot
;; 1: enables swing pot, connected to pin J5:A0
;;    NOTE: to avoid random swing values, set this #define to 0 when NO pot is connected!
;;    alternatively, you can clamp pin J5:A0 to ground
#ifndef DEFAULT_SWING_POT_CONNECTED
#define DEFAULT_SWING_POT_CONNECTED 0
#endif


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
	dw	function, (pin + 8*((sr-1)&0xff))
	ENDM

DIN_ENTRY_EOT MACRO
	dw	0x0000, 0x0000
	ENDM
	
SEQ_IO_TABLE_DIN
	;;		Function name		SR#	Pin#
	;; NOTE: the pins of the 16 general purpose buttons are assigned above, search for DEFAULT_GP_DIN_SR_L (and _R)
	DIN_ENTRY	SEQ_BUTTON_Scrub,	 0,	 0
	DIN_ENTRY	SEQ_BUTTON_Metronome,	 0,	 0

	DIN_ENTRY	SEQ_BUTTON_Stop,	 5,	 5
	DIN_ENTRY	SEQ_BUTTON_Pause,	 0,	 0
	DIN_ENTRY	SEQ_BUTTON_Play,	 5,	 3
	DIN_ENTRY	SEQ_BUTTON_Rew,		 5,	 6
	DIN_ENTRY	SEQ_BUTTON_Fwd,		 5,	 2
	DIN_ENTRY	SEQ_BUTTON_Live,	 5,	 1

	DIN_ENTRY	SEQ_BUTTON_F1,		 0,	 0
	DIN_ENTRY	SEQ_BUTTON_F2,		 0,	 0
	DIN_ENTRY	SEQ_BUTTON_F3,		 0,	 0
	DIN_ENTRY	SEQ_BUTTON_F4,		 0,	 0

	DIN_ENTRY	SEQ_BUTTON_Alt,		 0,	 0
	DIN_ENTRY	SEQ_BUTTON_Shift,	 2,	 5
	DIN_ENTRY	SEQ_BUTTON_Alt,		 2,	 4

	DIN_ENTRY	SEQ_BUTTON_SectionA,	 1,	 0
	DIN_ENTRY	SEQ_BUTTON_SectionB,	 1,	 5
	DIN_ENTRY	SEQ_BUTTON_SectionC,	 1,	 3
	DIN_ENTRY	SEQ_BUTTON_SectionD,	 1,	 4

	DIN_ENTRY	SEQ_BUTTON_LayerGate,	 2,	 0
	DIN_ENTRY	SEQ_BUTTON_LayerAux,	 2,	 1

	DIN_ENTRY	SEQ_BUTTON_Edit,	 6,	 4
	DIN_ENTRY	SEQ_BUTTON_Mute,	 5,	 0
	DIN_ENTRY	SEQ_BUTTON_Pattern,	 6,	 7
	DIN_ENTRY	SEQ_BUTTON_Song,	 6,	 5

	DIN_ENTRY	SEQ_BUTTON_Solo,	 0,	 0
	DIN_ENTRY	SEQ_BUTTON_Fast,	 0,	 0
	DIN_ENTRY	SEQ_BUTTON_All,		 0,	 0

	;; OPTIONAL! see CHANGELOG.txt
	DIN_ENTRY	SEQ_BUTTON_Group1,	 0,	 0
	DIN_ENTRY	SEQ_BUTTON_Group2,	 0,	 0
	DIN_ENTRY	SEQ_BUTTON_Group3,	 0,	 0
	DIN_ENTRY	SEQ_BUTTON_Group4,	 0,	 0

	DIN_ENTRY_EOT


; ==========================================================================
;  Following statements are used to assign LED functions to DOUT pins
;
;  To enable a LED function, specify the shift register number SR (1-16),
;  and the pin number (0-7).
;  Note that Pin 0 is D7 of the DOUT register, Pin 1 is D6, ... Pin 7 is D0
;
;  With SR value = 0, the LED function will be disabled
; ==========================================================================

;;                         SR    ignore    Pin
LED_SECTION_A	EQU	((( 2   -1)<<3)+    6)
LED_SECTION_B	EQU	((( 2   -1)<<3)+    5)
LED_SECTION_C	EQU	((( 2   -1)<<3)+    3)
LED_SECTION_D	EQU	((( 2   -1)<<3)+    1)

;;                         SR    ignore    Pin
LED_LAYER_GATE	EQU	((( 2   -1)<<3)+    7)
LED_LAYER_AUX	EQU	((( 2   -1)<<3)+    0)

;;                         SR    ignore    Pin
LED_EDIT	EQU	((( 7   -1)<<3)+    3)
LED_MUTE	EQU	((( 6   -1)<<3)+    7)
LED_PATTERN	EQU	((( 7   -1)<<3)+    1)
LED_SONG	EQU	((( 7   -1)<<3)+    2)

;;                         SR    ignore    Pin
LED_SOLO	EQU	((( 0   -1)<<3)+    0)
LED_FAST	EQU	((( 0   -1)<<3)+    0)
LED_ALL		EQU	((( 0   -1)<<3)+    0)

;;                         SR    ignore    Pin
LED_GROUP1	EQU	((( 0   -1)<<3)+    0)
LED_GROUP2	EQU	((( 0   -1)<<3)+    0)
LED_GROUP3	EQU	((( 0   -1)<<3)+    0)
LED_GROUP4	EQU	((( 0   -1)<<3)+    0)

;;                         SR    ignore    Pin
LED_SHIFT	EQU	((( 6   -1)<<3)+    2)
LED_ALT		EQU	((( 6   -1)<<3)+    1)

;;                         SR    ignore    Pin
LED_RECORD	EQU	((( 7   -1)<<3)+    4)
LED_XXX		EQU	((( 7   -1)<<3)+    5)

;;                         SR    ignore    Pin
LED_PLAY	EQU	((( 7   -1)<<3)+    0)
LED_STOP	EQU	((( 6   -1)<<3)+    6)
LED_PAUSE	EQU	((( 0   -1)<<3)+    0)
LED_FWD		EQU	((( 6   -1)<<3)+    5)
LED_REW		EQU	((( 6   -1)<<3)+    4)
LED_LIVE	EQU	((( 6   -1)<<3)+    3)

;;                         SR    ignore    Pin
LED_BEAT	EQU	((( 0   -1)<<3)+   0)


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
	ENC_ENTRY  6,  2,  MIOS_ENC_MODE_DETENTED2	; Tempo Encoder
	ENC_ENTRY  1,  6,  MIOS_ENC_MODE_DETENTED2	; Instrument Encoder

	;; don't remove this "end-of-table" entry!
	ENC_EOT			


;; include the rest of the application
#include "main.inc"
