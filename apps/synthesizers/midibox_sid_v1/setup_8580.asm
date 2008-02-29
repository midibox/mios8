; $Id$
;
; Prepared Setup File for 8580 SID
;

	;; here you can change the default device ID - if you are using
	;; some cores as slave, set:
	;;    o DEFAULT_DEVICE_ID 0x01	; for the first slave
	;;    o DEFAULT_DEVICE_ID 0x02	; for the second slave
	;;    o DEFAULT_DEVICE_ID 0x03	; for the third slave
#define DEFAULT_DEVICE_ID 0x00

	;; if this option is enabled (1), the DEFAULT_DEVICE_ID won't be used, but
	;; it will be automatically derived from the MIOS Device ID instead
#define AUTO_DEVICE_ID	1
	
	;; this MIDI channel will only be taken when:
	;;    - you are using the MBSID standalone (neither as CS Master, nor as CS Slave)
	;;    - the control surface is diabled and a BankStick is connected
	;; in all other cases, the MIDI channel of the Master or Slave has to be selected
	;; from the Control Surface
#define DEFAULT_MIDI_CHANNEL 1

	;; select the BankStick type you are using here:
	;;    0: "normal" 24LC256 EEPROM (32k), up to 8 EEPROMs can be connected for 8 banks
	;;    1: 24LC512 EEPROM (64k), up to 4 EEPROMs can be connected for 8 banks
	;;       (it is not possible to mix 32k with 64k EEPROMs, more than 4 64k devices not supported)
#define DEFAULT_BS_TYPE		0
	
	;; select the filter type here:
	;;    0: if a 6581 is controlled from the core
	;;    1: if a 8580 is controlled from the core
	;;    2: 6581 + freely definable scaling (define MIN and MAX value below)
	;;    3: 8580 + freely definable scaling (define MIN and MAX value below)
#define DEFAULT_FILTER_TYPE	1
	;; only relevant if DEFAULT_FILTER_TYPE is 2 or 3, values between 0 and 2047 are allowed
#define DEFAULT_FILTER_CALI_MIN 20
#define DEFAULT_FILTER_CALI_MAX 1000

	;; set this if you want to switch between two different types and Min/Max values
#define DEFAULT_FILTER_TYPE_SWITCH 0
	;; and define the used pin here (default: pin RC.3)
#define DEFAULT_FILTER_TYPE_SWITCH_TRIS TRISC
#define DEFAULT_FILTER_TYPE_SWITCH_PORT PORTC
#define DEFAULT_FILTER_TYPE_SWITCH_PIN  3
	;; define the type (0-3) and the Min/Max values (for type 2/3) here:
#define DEFAULT_FILTER_TYPE2     1
#define DEFAULT_FILTER_CALI_MIN2 0
#define DEFAULT_FILTER_CALI_MAX2 2047


	;; set this to '0' to disable the control surface - this is especially usefull for
	;; people who want to program their own CS handler in order to free
	;; some memory
	;; It's also a good idea to disable the CS if the core is running as SID slave
	;; (although this is no requirement!)
#define CS_ENABLED 1

	;; if this option is set to "1", the CS will be enabled if the MIOS device ID is 0x00, and
	;; it will be disabled if the MIOS device ID is != 0x00
	;; in other words: the control surface will only be enabled for the master
	;; note that this mechanism doesn't remove the CS code like CS_ENABLED == 0
#define AUTO_CS_ENABLED	1

	;; number of visible menu items on LCD
	;; use:  4 for 2x16 displays
	;;       5 for 2x20 displays
	;;      10 for 2x40 displays
#define CS_MENU_DISPLAYED_ITEMS 5
	;; NOTE: if CS_MENU_DISPLAYED_ITEMS > 5, you have to adapt the DIN settings
	;; in cs_menu_io_tables.inc

	;; OLD_STYLE == 1: page scrolls with the cursor
	;; OLD_STYLE == 0: page scrolls directly with the datawheel
#define CS_MENU_OLD_STYLE 0

	;; Inc/Dec buttons or rotary encoder of data entry?
	;; use:  0 if rotary encoder should be used 
	;;         (pins of encoder are defined in mios_tables.inc)
	;;       1 if buttons should be used instead
#define CS_MENU_USE_INCDEC_BUTTONS 0

	;; use:  0 if a non-detented encoder is used to select the menu items
	;;       1 if a detented encoder is used to select the menu items
	;; the CS_MENU_EncSpeedSet function in "cs_menu.inc" sets the speed
	;; depending on this value
#define CS_MENU_USE_DETENTED_ENCODER 1

	;; the speed value for the additional encoders (#2-#15) is defined here
	;; Hint: for non-detented encoders try the following: in mios_tables.inc
	;; set the type of all encoders to "MIOS_ENC_MODE_DETENTED2" in order to
	;; reduce the resolution (e.g. 24 ticks per revolution for a ALPS STEC16B),
	;; set the speed value to 3 --- result: so long as the encoder is moved
	;; slow, the parameter can be changed very precisely. By moving the encoder
	;; faster, it is possible to sweep over the whole value range with a single turn
	;; ("Progressive Mode")
#define CS_MENU_ENC_SPEED_VALUE 3

	;; set this define to 1 of the link to slave SIDs should be enabled by default
#define CS_MENU_DEFAULT_LINK 0

	;; if 0: OSC button selects also combined modes (7 states, combined OSCs)
	;; if 1: OSC button selects only one of three OSCs (faster handling)
#define CS_MENU_OSC_BUTTON_BEHAVIOUR 1

	;; use PORTA and PORTE (J5 of the core module) as output
	;; you can address these pins in cs_menu_io_tables.inc as "shift register" #0
#define ENABLE_J5 0

	;; set this to '1' if the modulation matrix (8x7 LEDs) is connected to the DOUT chain
#define MOD_MATRIX_ENABLE	1

	;; define the shift registers (note: HERE the shift register begin with 0: 1st SR is 0, 2nd is 1, 3rd is 2, ...)
#define MOD_MATRIX_ANODES	6	; shift register with anodes (HERE: 7th shift register in the chain)
#define MOD_MATRIX_CATHODES	7	; shift register with cathodes (HERE: 8th shift register in the chain)

; For MIDI activity monitor: define the DOUT pins for the Rx and Tx LED:
#define MIDI_RXTX_USE_J5       0	; if 0: MIDI Rx/Tx LEDs are assigned to the DOUT chain, pin 0x00-0x7f
					; if 1: MIDI Rx/Tx LEDs are assigned to J5 DOUT, pin 0x00-0x07
					;       (in this case ENABLE_J5 has to be set to 1!)

#define DEFAULT_MIDI_MONITOR_ENABLED 1  ; if 1, the Tx/Rx LEDs are enabled
#define DEFAULT_MIDI_RX_LED 0x41	; DOUT SR#9, pin D1
#define DEFAULT_MIDI_TX_LED 0x42	; DOUT SR#9, pin D2

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
	;; all other values invalid!
#define AOUT_LC_RESOLUTION_OPTION_M1 0
#define AOUT_LC_RESOLUTION_OPTION_M2 0
#define AOUT_LC_RESOLUTION_OPTION_M3 0
#define AOUT_LC_RESOLUTION_OPTION_M4 0

	;; enable the 6th LFO waveform (analog inputs)
#define ENABLE_AIN_LFO_WAVEFORM 1

	;; sets the default display mode for the LED matrix
	;;    0: shows modulation assignments after power-on
	;;    1: shows modulation meters after power-on
	;; the mode can be toggled by pressing two mod target buttons at the same time (e.g. O1 and O2 Pitch)
#define DEFAULT_LEDMATRIX_MODE 0

#include "src/main.inc"
