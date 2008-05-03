; $Id$
;
; Configuration Tables for MIOS Application
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
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
	;; entry 0x00-0x0f
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT

	;; entry 0x10-0x1f	
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT

	;; entry 0x20-0x2f
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	
	;; entry 0x30-0x3f
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	
	;; entry 0x40-0x4f
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	
	;; entry 0x50-0x5f
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
		
	;; entry 0x60-0x6f
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT

	;; entry 0x70-0x7f
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT

	;; entry 0x80-0x8f
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT

	;; entry 0x90-0x9f
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT

	;; entry 0xa0-0xaf
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT

	;; entry 0xb1-0xbf
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT

	;; entry 0xc0-0xcf
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT

	;; entry 0xd0-0xdf
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT

	;; entry 0xe0-0xef
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT

	;; entry 0xf0-0xff
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT
	MT_EOT



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
;;          or MIOS_ENC_MODE_DETENTED3
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
	ENC_ENTRY  1,  0,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 1
	
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
