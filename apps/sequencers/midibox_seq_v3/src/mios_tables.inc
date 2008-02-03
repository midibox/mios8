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


; ==========================================================================

;; encoders are defined in setup_mbseq_*.asm !

; ==========================================================================

	org	0x3300		; never change the origin!
