; $Id$
;
; Default Setup File for MIDIO128
;
; here you can change the default device ID
#define DEFAULT_DEVICE_ID	0x00
;
; if this option is enabled (1), the DEFAULT_DEVICE_ID won't be used, but
; it will be automatically derived from the MIOS Device ID instead
#define DEFAULT_AUTO_DEVICE_ID	1
;
; debounce counter (see the function description of MIOS_SRIO_DebounceSet)
; Use 0 for high-quality buttons, use higher values for low-quality buttons
#define DEFAULT_SRIO_DEBOUNCE_CTR 32
;
; Enable Alternative Program Change Behaviour
; If 0: on program change events, the appr. Output pin will
;       just toggle from logic 0 to logic 1 and vice versa
; If 1: on program change events, all output pins of the same
;       channel will be set to logic 0, but the pin which is assigned
;       to the channel and to the program change value will be set
;       to logic 1
#define DEFAULT_ALT_PROGCHNG	0
;
; Forward Input to Output
; If 0: if an inputs gets an raising or falling edge, the appr. output
;       pin will be set to the same new logic level. The output pin
;       can be controlled via MIDI also
; If 1: an output pin can only controlled via MIDI
#define DEFAULT_FORWARD_IO	1
;
; If 0: Inputs are high active
; If 1:	Inputs are low actvive (MIOS default)
#define DEFAULT_INVERSE_DIN	1
;
; Inverse Outputs
; If 0: Outputs are high active, reset value after poweron is "0" (MIOS default)
; If 1:	Outputs are low active, reset value after poweron is "1"
#define DEFAULT_INVERSE_DOUT	0
;
; Default value of touch sensor sensitivity
#define DEFAULT_TS_SENSITIVITY	0x03
;
; For MIDI activity monitor: define the DOUT pins for the Rx and Tx LED
#define DEFAULT_MIDI_MONITOR_ENABLED 0  ; if 1, the Tx/Rx LEDs are enabled
#define DEFAULT_MIDI_RX_LED 0x00	; DOUT SR#1, pin D0
#define DEFAULT_MIDI_TX_LED 0x01	; DOUT SR#1, pin D1
;
; with following settings it is possible to center the screen on 2x20 and 2x40 LCDs
; see also the functional description of MIOS_LCD_YAddressSet
					; recommented values:
					; 2x16 | 2x20 | 4x20 | Comments
					; -----+------+------+----------
#define DEFAULT_YOFFSET_LINE0	0x00	; 0x00 | 0x02 | 0x42 | cursor pos: 0x00-0x0f
#define DEFAULT_YOFFSET_LINE1	0x40	; 0x40 | 0x42 | 0x16 | cursor pos: 0x40-0x0f
#define DEFAULT_YOFFSET_LINE2	0x14	; 0x14 | 0x16 | 0x02 | cursor pos: 0x80-0x8f (not used yet)
#define DEFAULT_YOFFSET_LINE3	0x54	; 0x54 | 0x56 | 0x56 | cursor pos: 0xc0-0xcf (not used yet)


#include "src/main.inc"
