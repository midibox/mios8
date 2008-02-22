; $Id$
;
; MIDImon
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ==========================================================================

#define MB_STAT_DISPLAY_UPDATE_REQ	0	; if set, display will be updated
#define MB_STAT_DISPLAY_UPDATE_1L_REQ	1	; if set, only the first line will be updated (when allocated by MTC and MIDIclock)
#define MB_STAT_DISPLAY_GLCD		2	; if set, we assume that a graphical LCD is connected
#define MB_STAT_PRINT_CC_NAMES		3	; if set, CC names will be print
#define MB_STAT_FILTER_CLOCK		4	; if set, F8 and F1 will be filtered
#define MB_STAT_SHOW_MIDICLK		5	; if set, MIDIclock will be displayed at first line
#define MB_STAT_SHOW_MTC		6	; if set, MTC will be displayed at first line

;; ==========================================================================

	;; free memory for user applications:	
	;; 0x010-0x37f

MB_STAT			EQU	0x010

;; used by midi_evnt.inc
MIDI_EVNT0		EQU	0x011
MIDI_EVNT1		EQU	0x012
MIDI_EVNT_VALUE		EQU	0x013

;; used by midi_rxtx.inc
MIDI_RXTX_RX_CTR	EQU	0x014
MIDI_RXTX_TX_CTR	EQU	0x015

;; used by j5_din.inc
J5_DIN_STATUS		EQU	0x018	; used by J5_DIN_Update to store the current status
J5_DIN_STATUS_CHANGED	EQU	0x019	; used by J5_DIN_Update to notify that an input has been toggled

;; used by mtc.inc
MTC_STATE		EQU	0x020	; state of received MTC code
MTC_HR			EQU     0x021	; current "hours"
MTC_MN			EQU     0x022	; current "minutes"
MTC_SC			EQU     0x023	; current "seconds"
MTC_FR			EQU     0x024	; current "frames"
MTC_HR_NEW		EQU	0x025	; received "hours"
MTC_MN_NEW		EQU	0x026	; received "minutes"
MTC_SC_NEW		EQU	0x027	; received "seconds"
MTC_FR_NEW		EQU	0x028	; received "frames"

;; used by midiclk.inc
MIDICLK_STATE		EQU	0x030	; state of MIDI clock handler
MIDICLK_BEAT_MSB	EQU     0x031	; current "beat" (most significant bits))
MIDICLK_BEAT_LSB	EQU     0x032	; current "beat" (least significant bits))
MIDICLK_QN		EQU     0x033	; current "quarter note"
MIDICLK_CTR		EQU     0x034	; current "clock counter"
MIDICLK_POS_LSB		EQU	0x035	; temporary used to receive the song position
MIDICLK_POS_MSB		EQU	0x036	; temporary used to receive the song position

;; used by bpm.inc
BPM_CTR			EQU	0x038	; running F8 clock counter
BPM_CTR_REG      	EQU     0x039	; registered counter value
BPM_TIMER_CTR_L 	EQU     0x03a	; used as predivider for a 2-second-timer
BPM_TIMER_CTR_H		EQU     0x03b	; dito, high byte

;; used by leddigits.inc
LEDDIGITS_CTR		EQU	0x040	; led digits counter, used by leddigits.inc
LEDDIGITS_0		EQU	0x041	; define registers for all available digits
LEDDIGITS_1		EQU	0x042
LEDDIGITS_2		EQU	0x043
LEDDIGITS_3		EQU	0x044
LEDDIGITS_4		EQU	0x045
LEDDIGITS_5		EQU	0x046
LEDDIGITS_6		EQU	0x047
LEDDIGITS_7		EQU	0x048
;; define the number of available digits here - it can be derived from the address
;; this number is only used by the initialization routine and not by the update handler itself
LEDDIGITS_NUMBER	EQU	(LEDDIGITS_7-LEDDIGITS_0) + 1

;; used by MIDImon itself
MON_TRACE_PTR		EQU	0x050		; this variable points to the last received entry in the TRACE_MEM array
MON_RUNNING_STATUS	EQU	0x051		; saves the current status (used to indicate system events)
MON_MIDI_BYTE_CTR	EQU	0x052		; number of received bytes (used for events F0 >= x <= F7)
MON_SYSEX_PTR		EQU	0x053		; used for SysEx events: points to the position in the SysEx buffer
MON_SYSEX_BUFFER_BEGIN	EQU	0x054		; here begins the circular SysEx buffer (16 bytes max)
MON_SYSEX_BUFFER_END	EQU	0x063
MON_DISPLAY_LOOP_CTR	EQU	0x064		; loop counter for display handler
MON_CURRENT_ENTRY_PTR	EQU	0x065		; trace entry which will currently be processed
MON_DISPLAY_OFFSET	EQU	0x066		; offset controlled by the up/down buttons
MON_DISPLAY_CURSOR_CTR	EQU	0x067		; incremented by USER_Timer to flash the offset position indicator

;; the huge trace memory can save up to 256 MIDI event entries (3*0x100 bytes)
MON_TRACE_MEM_BEGIN	EQU	0x080
MON_TRACE_MEM_END	EQU	(DEFAULT_TRACE_MEM_ENTRIES)*3	; (when DEFAULT_TRACE_MEM_ENTRIES == 256 -> 0x37f !!!)
