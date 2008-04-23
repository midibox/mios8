; $Id$
	LIST R=DEC
;
; Prepared Setup File for TK's MIDIbox FM setup
; In difference to the setup_mbfm_v1.asm configuration it uses a different 
; rotary encoder type and an enabled AOUT interface
;

; Hardware related setup:

	;; set this to '0' to disable the control surface - this is especially usefull for
	;; people who want to program their own CS handler in order to free
	;; some memory
#define CS_ENABLED 1

	;; number of visible menu items on LCD
	;; use:  3 for 2x16 displays
	;;       4 for 2x20 displays
	;;       8 for 2x40 displays
#define CS_MENU_DISPLAYED_ITEMS 8
	;; NOTE: if CS_MENU_DISPLAYED_ITEMS < 8, you have to adapt the DIN settings
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

	;; the speed value for the additional encoders (#2-#5) is defined here
	;; Hint: for non-detented encoders try the following: in mios_tables.inc
	;; set the type of all encoders to "MIOS_ENC_MODE_DETENTED2" in order to
	;; reduce the resolution (e.g. 24 ticks per revolution for a ALPS STEC16B),
	;; set the speed value to 3 --- result: so long as the encoder is moved
	;; slow, the parameter can be changed very precisely. By moving the encoder
	;; faster, it is possible to sweep over the whole value range with a single turn
	;; ("Progressive Mode")
#define CS_MENU_ENC_SPEED_VALUE 1

	;; select the default mode of the main display after startup:
	;; 0: show all 4 instruments, names are cropped to 9 characters
	;; 1: show only the selected instrument, full name is displayed
#define CS_MENU_DEFAULT_VIEW_MODE 1

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

	;; only for debugging: prints the MBFM load at the right upper edge of the 2x40 LCD
#define DISPLAY_PERFORMANCE 0

	;; experimental superpoly mode (currently hard-coded to save programming effort)
#define SUPERPOLY_EXPERIMENT 0



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
	;; encoders 1-16
	;;        SR  Pin  Mode
#if CS_MENU_USE_INCDEC_BUTTONS
	ENC_EOT
#else
	ENC_ENTRY  1,  0,  MIOS_ENC_MODE_DETENTED2	; menu encoder
#endif

	;; additional CS encoders
	;;        SR  Pin  Mode
	ENC_ENTRY  3,  2,  MIOS_ENC_MODE_DETENTED2	; Multipurpose Enc #1
	ENC_ENTRY  3,  4,  MIOS_ENC_MODE_DETENTED2	; Multipurpose Enc #2
	ENC_ENTRY  3,  6,  MIOS_ENC_MODE_DETENTED2	; Multipurpose Enc #3
	ENC_ENTRY  4,  0,  MIOS_ENC_MODE_DETENTED2	; Multipurpose Enc #4

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


#include "src/main.inc"
