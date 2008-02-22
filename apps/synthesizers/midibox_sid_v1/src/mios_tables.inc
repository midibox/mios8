; $Id$
;
; Configuration Tables for MIOS Application
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	org	0x3080		; never change the origin!

;; --------------------------------------------------------------------------
;; The MIDI Trigger table maps incoming MIDI events to control elements
;; (pots/faders, encoders, buttons, LEDs, ...)
;; 
;; After the MIOS MIDI processor (MPROC) has received a complete event, it
;; searches in this table for a matching entry and calls USER_MPROC_NotifyFoundEvent
;; with the table index in WREG and the MIDI event in MIOS_PARAMETER[123]
;; 
;; Note also that the MIDI processor stops scanning the table after
;; the first MT_EOT entry
;; 
;; Note2: never change the origin (memory location) of this table!
;; 
;; Optionally this table can also be used to define events, which should be
;; transmitted (Example: see example_ain64_din128_dout128), so that incoming
;; and outgoing events are located at one place
;; 
;; The table *must* contain 256 entries. Unused entries should be filled with
;; MT_EOT.
;; --------------------------------------------------------------------------

	;; MIDI Trigger entry structure
MT_ENTRY MACRO event_0, event_1
	dw	(event_1 << 8) | event_0
	ENDM

MT_EOT	MACRO
	dw	0xffff
	ENDM


MIOS_MPROC_EVENT_TABLE
	;; this "end of table" entry ensures that MIOS will stop searching here
	MT_EOT


	;; dirty but allowed so long as MT_EOT can be found at 0x3080:
	;; since the MIDIbox SID application doesn't get use of the MPROC_EVENT_TABLE,
	;; we are using the address range from 0x3082 to 0x327f for other
	;; application specific tables

#include "sid_frq_table.inc"
#include "sid_sin_table.inc"
#include "sid_depth_table.inc"

; ==========================================================================

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
	;;        SR  Pin  Mode
#if CS_MENU_USE_INCDEC_BUTTONS
	ENC_EOT
#else
	ENC_ENTRY  1,  0,  MIOS_ENC_MODE_DETENTED2	; menu encoder
#endif

	;; additional CS encoders
	;;        SR  Pin  Mode
	ENC_ENTRY  3,  0,  MIOS_ENC_MODE_DETENTED2	; Osc delay/transpose/assign #1
	ENC_ENTRY  3,  2,  MIOS_ENC_MODE_DETENTED2	; Osc attack/finetune/assign #2
	ENC_ENTRY  3,  4,  MIOS_ENC_MODE_DETENTED2	; Osc decay/portamento/assign #3
	ENC_ENTRY  3,  6,  MIOS_ENC_MODE_DETENTED2	; Osc sustain/release/assign #4
	ENC_ENTRY  3,  8,  MIOS_ENC_MODE_DETENTED2	; Osc release/pulsewidth/assign #5

	ENC_ENTRY  4,  6,  MIOS_ENC_MODE_DETENTED2	; LFO rate
	ENC_ENTRY  5,  0,  MIOS_ENC_MODE_DETENTED2	; LFO depth

	ENC_ENTRY  5,  4,  MIOS_ENC_MODE_DETENTED2	; Filter CutOff
	ENC_ENTRY  5,  6,  MIOS_ENC_MODE_DETENTED2	; Filter Resonance

	ENC_ENTRY  6,  0,  MIOS_ENC_MODE_DETENTED2	; Env depth/assign #1
	ENC_ENTRY  6,  2,  MIOS_ENC_MODE_DETENTED2	; Env attack/assign #2
	ENC_ENTRY  6,  4,  MIOS_ENC_MODE_DETENTED2	; Env decay/assign #3
	ENC_ENTRY  6,  6,  MIOS_ENC_MODE_DETENTED2	; Env sustain/assign #4
	ENC_ENTRY  7,  0,  MIOS_ENC_MODE_DETENTED2	; Env release/assign #5

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


	org	0x3300		; never change the origin!
