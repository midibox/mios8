; $Id: $
;
; "Dummy" MIDI Trigger Configuaration Table
; 
; Should be included by the application, if the MIDI processor feature
; is not used in order to pre-initialize the table area with EOT's
;
; Note: due to the inflexible concept (e.g. handling of Note On/Off events),
; the usage of this function is not recommented for new designs - please parse 
; MIDI events locally within the application.
;
;
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


_MIOS_MPROC_EVENT_TABLE
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

