; $Id$
	LIST R=DEC
;
; Setup File of MIDIbox CV
; In this setup, J5 (-> 8 gates at MBHP_CORE::J5) is enabled
;
;
; Datawheel for menu navigation/data entry connected or not?
; A datawheel can replace the left/right buttons!
; if -1: datawheel not connected
; if >= 0: encoder number assigned to datawheel function
; don't forget to check the pins of the datawheel in MIOS_ENC_PIN_TABLE (-> mios_tables.inc or setup_*.asm)
; it has to be connected to pin 4 and 5 by default
#define DEFAULT_ENC_DATAWHEEL   -1
;
; DIN pin numbers of menu buttons
#if DEFAULT_ENC_DATAWHEEL == -1
  #define DEFAULT_DIN_MENU_EXEC		7	; menu exec button assigned to DIN pin #7
  #define DEFAULT_DIN_MENU_RIGHT	6	; menu right button assigned to DIN pin #6
  #define DEFAULT_DIN_MENU_LEFT		5	; menu left button assigned to DIN pin #5
  #define DEFAULT_DIN_MENU_SELECT	4	; menu select button assigned to DIN pin #4
#else
  #define DEFAULT_DIN_MENU_EXEC		7	; menu exec button assigned to DIN pin #7
  #define DEFAULT_DIN_MENU_SELECT	6	; menu select button assigned to DIN pin #4
  #define DEFAULT_DIN_MENU_RIGHT	5	; NOT USED - overlayed by datawheel
  #define DEFAULT_DIN_MENU_LEFT		4	; NOT USED - overlayed by datawheel
#endif
;
; debounce counter (see the function description of MIOS_SRIO_DebounceSet)
; Use 0 for high-quality buttons, use higher values for low-quality buttons
#define DEFAULT_SRIO_DEBOUNCE_CTR 32
;
; not used by MIDIbox CV -- ignore it!
#define DEFAULT_GP_DIN_SR0	2	; first GP DIN shift register assigned to SR#2
#define DEFAULT_GP_DIN_SR1	3	; second GP DIN shift register assigned to SR#3
#define DEFAULT_GP_DOUT_SR0	2	; first GP DOUT shift register assigned to SR#1
#define DEFAULT_GP_DOUT_SR1	3	; second GP DOUT shift register assigned to SR#2
;
; use PORTA and PORTE (J5 of the core module) for 8 additional gate outputs
#define ENABLE_J5 1
;
;
; define the AOUT interface which is used here:
;   1: one MBHP_AOUT module
;   2: up to 4 (chained) MBHP_AOUT_LC modules
;   3: one MBHP_AOUT_NG module
; all other values invalid!
#define AOUT_INTERFACE_TYPE 1
;
;
; only relevant if one or more AOUT_LC modules are used:
; define the resolution configuration here
;   0: first channel 12bit, second channel 4bit
;   1: first channel 8bit, second channel 8bit
; all other values invalid!
#define AOUT_LC_RESOLUTION_OPTION_M1 0
#define AOUT_LC_RESOLUTION_OPTION_M2 0
#define AOUT_LC_RESOLUTION_OPTION_M3 0
#define AOUT_LC_RESOLUTION_OPTION_M4 0
;
;
; define the pin which should be used as external clock output here
; DEFAULT_EXT_CLK_LAT can be LATC (Pin 0, 1, 2, 4, 5) or LATD (Pin 4)
; The external pin is disabled with DEFAULT_EXT_CLK_LAT == 0
#define DEFAULT_EXT_CLK_LAT	LATC
#define DEFAULT_EXT_CLK_PIN	0

;; define the pin which should be used as external Start/Stop output here
;; default setting: RC1 (available at J6:SC of the core module)
#define DEFAULT_EXT_START_LAT	LATC
#define DEFAULT_EXT_START_PIN	1
;
; desired pulsewidth of clock output in pieces of 500 uS
; 0 -> 500 uS
; 1 -> 1   mS
; 2 -> 1.5 mS
; etc...
#define DEFAULT_EXT_CLK_PULSEWIDTH 1
;
;
; define the Chip Select number of the BankStick which is used by MBCV
; 0: disables BankStick, 1..8: selects CS#0..CS#7
; If you are unsure about the right value, use 1 (for CS#0, all CS inputs
; tied to ground), even when no BankStick is connected
; ("hotplug", will be detected automatically)
#define DEFAULT_BANKSTICK_NUMBER 1
;
;
; selects MIDI channel for patch changes
; 0: patches can only be changed from CS, 1-16: MIDI channel for program changes
#define DEFAULT_PATCH_CHANGE_CHANNEL 0
;


#include "src/main.inc"
