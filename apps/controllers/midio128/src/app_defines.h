; $Id$
;
; Specific Defines and variables of MIOS Application
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

#define MB_STAT_DISPLAY_UPDATE_REQ	0 ; if set, display will be updated
#define MB_STAT_FETCH_DIN_REQ           1 ; if set, the DIN registers will be copied to the DOUT registers

;; ==========================================================================

MIDIO_CFG0_MERGER		EQU 0	; if set, merger is disabled
MIDIO_CFG0_MBLINK		EQU 1	; if set, the MIDIbox Link is enabled
MIDIO_CFG0_ALT_PROGCHNG		EQU 2	; if set, alternative program change activated
MIDIO_CFG0_FORWARD_IO		EQU 3	; if set, buttons will control the appr. LEDs
MIDIO_CFG0_INVERSE_DIN		EQU 4	; if set, DIN will be inverted
MIDIO_CFG0_INVERSE_DOUT		EQU 5	; if set, DOUT will be inverted

;; ==========================================================================

;; ==========================================================================

	;; free memory for user applications:	
	;; 0x010-0x37f

;; ==========================================================================
;;  General MIDIO Control Variables in ACCESS page
;; ==========================================================================
MB_STAT			EQU	0x010
MB_BANK			EQU	0x011

MB_ADDRL		EQU	0x012
MB_ADDRH		EQU	0x013

MIDI_EVNT0		EQU	0x014
MIDI_EVNT1		EQU	0x015
MIDI_EVNT_VALUE		EQU	0x016

MIDI_RXTX_RX_CTR	EQU	0x017
MIDI_RXTX_TX_CTR	EQU	0x018

;; ==========================================================================

MIDIO_BASE		EQU	0x080
MIDIO_CFG0		EQU	0x080
MIDIO_DEVICE_ID		EQU	0x081

MIDIO_CURRENT_DIN	EQU	0x082
MIDIO_LAST_DIN0		EQU	0x083
MIDIO_LAST_DIN1		EQU	0x084

MIDIO_SYSEX_STATE	EQU	0x085
MIDIO_SYSEX_ACTION	EQU	0x086
MIDIO_SYSEX_IN		EQU	0x087
MIDIO_SYSEX_CHECKSUM	EQU	0x088

MIDIO_LAST_RECEIVED_EVNT0 EQU	0x089
MIDIO_LAST_RECEIVED_EVNT1 EQU	0x08a
MIDIO_LAST_RECEIVED_EVNT2 EQU	0x08b

MIDIO_ALL_NOTES_OFF_CHANNEL EQU	0x08c


	;; button values (packed)
MIDIO_DIN_VALUES_SR0	EQU	0x090
	;; ...
MIDIO_DIN_VALUES_SRF	EQU	0x09f

MIDIO_DATA_BUFFER	EQU	0x0c0	; used for page writes/reads
MIDIO_DATA_BUFFER_END	EQU	0x0ff	; could also be overlayed with restorable registers (like MIDIO_POT_EVNT*)

;; ==========================================================================

;; -----------------------------------
;; 256 bytes which are temporary used for SysEx uploads
;; the content of this address area will be restored after the upload
MIDIO_UPLOAD_BUFFER	EQU	0x100
MIDIO_UPLOAD_BUFFER_END	EQU	0x1ff
;; -----------------------------------
