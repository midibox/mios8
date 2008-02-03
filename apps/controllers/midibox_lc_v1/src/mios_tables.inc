; $Id$
;
; Configuration Tables for MIOS Application
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
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


	;; the MIDI processor is used for the general purpose controller mode

	;; logic control button functions have to be defined in lc_io_table.inc
MIOS_MPROC_EVENT_TABLE

	;; these entries are used by the "GPC" feature of MIDIbox LC which
	;; can be activated with the GPC button
	;; Up to 128 MIDI events can be defined here which are sent (and received) by the V-Pots in GPC mode
	;; The labels for these entries are defined in lc_gpc_lables.inc
	;; entry 0x00-0x0f
	MT_ENTRY	0xbf, 0x00
	MT_ENTRY	0xbf, 0x01
	MT_ENTRY	0xbf, 0x02
	MT_ENTRY	0xbf, 0x03
	MT_ENTRY	0xbf, 0x04
	MT_ENTRY	0xbf, 0x05
	MT_ENTRY	0xbf, 0x06
	MT_ENTRY	0xbf, 0x07
	MT_ENTRY	0xbf, 0x08
	MT_ENTRY	0xbf, 0x09
	MT_ENTRY	0xbf, 0x0a
	MT_ENTRY	0xbf, 0x0b
	MT_ENTRY	0xbf, 0x0c
	MT_ENTRY	0xbf, 0x0d
	MT_ENTRY	0xbf, 0x0e
	MT_ENTRY	0xbf, 0x0f

	;; entry 0x10-0x1f	
	MT_ENTRY	0xbf, 0x10
	MT_ENTRY	0xbf, 0x11
	MT_ENTRY	0xbf, 0x12
	MT_ENTRY	0xbf, 0x13
	MT_ENTRY	0xbf, 0x14
	MT_ENTRY	0xbf, 0x15
	MT_ENTRY	0xbf, 0x16
	MT_ENTRY	0xbf, 0x17
	MT_ENTRY	0xbf, 0x18
	MT_ENTRY	0xbf, 0x19
	MT_ENTRY	0xbf, 0x1a
	MT_ENTRY	0xbf, 0x1b
	MT_ENTRY	0xbf, 0x1c
	MT_ENTRY	0xbf, 0x1d
	MT_ENTRY	0xbf, 0x1e
	MT_ENTRY	0xbf, 0x1f

	;; entry 0x20-0x2f
	MT_ENTRY	0xbf, 0x20
	MT_ENTRY	0xbf, 0x21
	MT_ENTRY	0xbf, 0x22
	MT_ENTRY	0xbf, 0x23
	MT_ENTRY	0xbf, 0x24
	MT_ENTRY	0xbf, 0x25
	MT_ENTRY	0xbf, 0x26
	MT_ENTRY	0xbf, 0x27
	MT_ENTRY	0xbf, 0x28
	MT_ENTRY	0xbf, 0x29
	MT_ENTRY	0xbf, 0x2a
	MT_ENTRY	0xbf, 0x2b
	MT_ENTRY	0xbf, 0x2c
	MT_ENTRY	0xbf, 0x2d
	MT_ENTRY	0xbf, 0x2e
	MT_ENTRY	0xbf, 0x2f
	
	;; entry 0x30-0x3f
	MT_ENTRY	0xbf, 0x30
	MT_ENTRY	0xbf, 0x31
	MT_ENTRY	0xbf, 0x32
	MT_ENTRY	0xbf, 0x33
	MT_ENTRY	0xbf, 0x34
	MT_ENTRY	0xbf, 0x35
	MT_ENTRY	0xbf, 0x36
	MT_ENTRY	0xbf, 0x37
	MT_ENTRY	0xbf, 0x38
	MT_ENTRY	0xbf, 0x39
	MT_ENTRY	0xbf, 0x3a
	MT_ENTRY	0xbf, 0x3b
	MT_ENTRY	0xbf, 0x3c
	MT_ENTRY	0xbf, 0x3d
	MT_ENTRY	0xbf, 0x3e
	MT_ENTRY	0xbf, 0x3f
	
	;; entry 0x40-0x4f
	MT_ENTRY	0xbf, 0x40
	MT_ENTRY	0xbf, 0x41
	MT_ENTRY	0xbf, 0x42
	MT_ENTRY	0xbf, 0x43
	MT_ENTRY	0xbf, 0x44
	MT_ENTRY	0xbf, 0x45
	MT_ENTRY	0xbf, 0x46
	MT_ENTRY	0xbf, 0x47
	MT_ENTRY	0xbf, 0x48
	MT_ENTRY	0xbf, 0x49
	MT_ENTRY	0xbf, 0x4a
	MT_ENTRY	0xbf, 0x4b
	MT_ENTRY	0xbf, 0x4c
	MT_ENTRY	0xbf, 0x4d
	MT_ENTRY	0xbf, 0x4e
	MT_ENTRY	0xbf, 0x4f
	
	;; entry 0x50-0x5f
	MT_ENTRY	0xbf, 0x50
	MT_ENTRY	0xbf, 0x51
	MT_ENTRY	0xbf, 0x52
	MT_ENTRY	0xbf, 0x53
	MT_ENTRY	0xbf, 0x54
	MT_ENTRY	0xbf, 0x55
	MT_ENTRY	0xbf, 0x56
	MT_ENTRY	0xbf, 0x57
	MT_ENTRY	0xbf, 0x58
	MT_ENTRY	0xbf, 0x59
	MT_ENTRY	0xbf, 0x5a
	MT_ENTRY	0xbf, 0x5b
	MT_ENTRY	0xbf, 0x5c
	MT_ENTRY	0xbf, 0x5d
	MT_ENTRY	0xbf, 0x5e
	MT_ENTRY	0xbf, 0x5f
		
	;; entry 0x60-0x6f
	MT_ENTRY	0xbf, 0x60
	MT_ENTRY	0xbf, 0x61
	MT_ENTRY	0xbf, 0x62
	MT_ENTRY	0xbf, 0x63
	MT_ENTRY	0xbf, 0x64
	MT_ENTRY	0xbf, 0x65
	MT_ENTRY	0xbf, 0x66
	MT_ENTRY	0xbf, 0x67
	MT_ENTRY	0xbf, 0x68
	MT_ENTRY	0xbf, 0x69
	MT_ENTRY	0xbf, 0x6a
	MT_ENTRY	0xbf, 0x6b
	MT_ENTRY	0xbf, 0x6c
	MT_ENTRY	0xbf, 0x6d
	MT_ENTRY	0xbf, 0x6e
	MT_ENTRY	0xbf, 0x6f

	;; entry 0x70-0x7f
	MT_ENTRY	0xbf, 0x70
	MT_ENTRY	0xbf, 0x71
	MT_ENTRY	0xbf, 0x72
	MT_ENTRY	0xbf, 0x73
	MT_ENTRY	0xbf, 0x74
	MT_ENTRY	0xbf, 0x75
	MT_ENTRY	0xbf, 0x76
	MT_ENTRY	0xbf, 0x77
	MT_ENTRY	0xbf, 0x78
	MT_ENTRY	0xbf, 0x79
	MT_ENTRY	0xbf, 0x7a
	MT_ENTRY	0xbf, 0x7b
	MT_ENTRY	0xbf, 0x7c
	MT_ENTRY	0xbf, 0x7d
	MT_ENTRY	0xbf, 0x7e
	MT_ENTRY	0xbf, 0x7f

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

;  Encoder pins are defined in setup_*.asm

; ==========================================================================

	org	0x3300		; never change the origin!
