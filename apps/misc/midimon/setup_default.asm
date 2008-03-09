; $Id$
	LIST R=DEC
;
; Setup File of MIDImon
;

; number of entries in trace memory --- note that this value affects the allocated SRAM
; see also app_defines.h
; maximum setting: 256
#define DEFAULT_TRACE_MEM_ENTRIES	256	; we want it all :-)
;
; if 0: buttons/jumper are connected to the DIN chain
; if 1: buttons/jumper are connected to J5 of the core module (default)
#define DEFAULT_USE_J5			1
;
; The button assignments to DIN or J5 pins
#define DEFAULT_BUTTON_UP		0x00	; to shift the trace window up
#define DEFAULT_BUTTON_DOWN		0x01	; to shift the trace window down
#define DEFAULT_BUTTON_FILTER		0x02	; filters MTC/Clk/Running Status - should be a switch or jumper
#define DEFAULT_BUTTON_CC_NAMES		0x03	; displays CC with standard names - should be a switch or jumper
;
; debounce counter (see the function description of MIOS_SRIO_DebounceSet)
; Use 0 for high-quality buttons, use higher values for low-quality buttons
; works only with DIN (and not J5) inputs
#define DEFAULT_SRIO_DEBOUNCE_CTR 32
;
; For MIDI activity monitor: define the DOUT pins for the Rx and Tx LED
#define DEFAULT_MIDI_MONITOR_ENABLED	1	; if 1, the Tx/Rx LEDs are enabled
#define DEFAULT_MIDI_RX_LED		0x18	; DOUT SR#3, pin D0
#define DEFAULT_MIDI_TX_LED		0x19	; DOUT SR#3, pin D1
;
; configure the leddigits driver
#define DEFAULT_LEDDIGITS_ENABLED	1	; if 1, leddigits are enabled
#define DEFAULT_LEDDIGITS_SR_SEGMENTS_2 2	; shift register which drives the segments of digit 7-4 (left side)
#define DEFAULT_LEDDIGITS_SR_SEGMENTS_1 0	; shift register which drives the segments of digit 3-0 (right side)
#define DEFAULT_LEDDIGITS_SR_SELECT_21	1	; shift register which selects the digits 7-0
;
; Number of displayed lines
; character LCD: up to 4
; graphical LCD: depends on display, in horizontal mode 8, in vertical mode even more
#define DEFAULT_NUMBER_OF_LINES		4
;
; Display Offsets -- only relevant if character LCD is connected
					; recommented values:
					; 2x16 | 2x20 | 4x20 | Comments
					; -----+------+------+----------
#define DEFAULT_YOFFSET_LINE0	0x00	; 0x00 | 0x00 | 0x00 | cursor pos: 0x00-0x13
#define DEFAULT_YOFFSET_LINE1	0x40	; 0x40 | 0x40 | 0x40 | cursor pos: 0x40-0x53
#define DEFAULT_YOFFSET_LINE2	0x14	; 0x14 | 0x14 | 0x14 | cursor pos: 0x80-0x73 (not used yet)
#define DEFAULT_YOFFSET_LINE3	0x54	; 0x54 | 0x53 | 0x54 | cursor pos: 0xc0-0xd3 (not used yet)
;
; Offset of "shift indicator" (visible when Up/Down button is pressed)
; use 14 on LCDs with 20 or more characters per line
; use 0 on LCDs with only 16 characters per line
#define DEFAULT_OFFSET_SHIFT_INDICATOR	14


#include "src/main.inc"
