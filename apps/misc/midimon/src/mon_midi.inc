; $Id$
;
; MIDImon Display functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function is called by MB_MPROC when a complete MIDI event has been
;;  received
;;  also used by MON_MIDI_NotifyReceivedByte when a system event has
;;  been regognized
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
MON_MIDI_NotifyReceivedEvent
	;; increment trace memory pointer, wrap at DEFAULT_TRACE_MEM_SIZE
	incf	MON_TRACE_PTR, F
#if DEFAULT_TRACE_MEM_ENTRIES < 256
	movlw	DEFAULT_TRACE_MEM_ENTRIES
	cpfslt	MON_TRACE_PTR, ACCESS
	clrf MON_TRACE_PTR
#endif

	;; store MIDI event in trace memory
	movf	MON_TRACE_PTR, W
	movwf	MON_CURRENT_ENTRY_PTR
	call	MON_FSR_TraceEntry

	movff	MIOS_PARAMETER1, POSTINC0
	movff	MIOS_PARAMETER2, POSTINC0
	movff	MIOS_PARAMETER3, POSTINC0

	;; clear offset
	clrf	MON_DISPLAY_OFFSET

	;; request display update
	bsf	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ

	;; and exit
	return

;; --------------------------------------------------------------------------
;;  This function is called by MB_MPROC when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MON_MIDI_NotifyReceivedByte
	;; NOTE: common MIDI events are saved by MON_MIDI_NotifyReceivedEvent
	;; this function saves system events only

	;; if MIDI clock filter enabled, branch to the end on 0xf8 and 0xfe
	BRA_IFCLR MB_STAT, MB_STAT_FILTER_CLOCK, ACCESS, MON_MIDI_NotifyReceivedByte_NoF
MON_MIDI_NotifyReceivedByte_F
	movf	MIOS_PARAMETER1, W
	xorlw	0xf8
	bz	MON_MIDI_NotifyReceivedByte_End
	movf	MIOS_PARAMETER1, W
	xorlw	0xfe
	bz	MON_MIDI_NotifyReceivedByte_End	
MON_MIDI_NotifyReceivedByte_NoF
	
	;; don't touch running status on realtime events - save 1-byte event
	movlw	0xf8
	cpfslt	MIOS_PARAMETER1, ACCESS
	rgoto MON_MIDI_NotifyReceivedByte_1B

	;; if bit #7 set, change running status and clear byte counter
	BRA_IFCLR MIOS_PARAMETER1, 7, ACCESS, MON_MIDI_NotifyReceivedByte_NoRe
MON_MIDI_NotifyReceivedByte_Re	; P.S.:	i really HATE the label size limitation of MPASM!!!
	movff	MIOS_PARAMETER1, MON_RUNNING_STATUS
	setf	MON_MIDI_BYTE_CTR	; set it to 0xff, so that the next increment results to 0x00
MON_MIDI_NotifyReceivedByte_NoRe

	;; increment byte counter, ensure that it never exceeds 2
	incf	MON_MIDI_BYTE_CTR, F
	movlw	0x02
	cpfsgt	MON_MIDI_BYTE_CTR, ACCESS
	rgoto MON_MIDI_NotifyReceivedByte_NoSa
MON_MIDI_NotifyReceivedByte_Sa
	movwf	MON_MIDI_BYTE_CTR
MON_MIDI_NotifyReceivedByte_NoSa

	;; branch to the end if status < 0xf0
	;; such events are stored by MON_MIDI_NotifyReceivedEvent
	movlw	0xf0-1
	cpfsgt	MON_RUNNING_STATUS, ACCESS
	rgoto MON_MIDI_NotifyReceivedByte_End

	;; don't record MTC events if filter enabled
	movf	MON_RUNNING_STATUS, W
	xorlw	0xf1
	btfsc	MB_STAT, MB_STAT_FILTER_CLOCK
	bz MON_MIDI_NotifyReceivedByte_End

	;; now we are sure that F0 >= MON_RUNNING_STATUS <= F7

	;; increment SysEx pointer, wrap at 16
	incf	MON_SYSEX_PTR, W
	andlw	0x0f
	movwf	MON_SYSEX_PTR

	;; store received byte in SysEx buffer
	lfsr	FSR0, MON_SYSEX_BUFFER_BEGIN
	movff	MIOS_PARAMETER1, PLUSW0

	;; branch depending on status byte
	movf	MON_RUNNING_STATUS, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MON_MIDI_NotifyReceivedByte_F0
	rgoto	MON_MIDI_NotifyReceivedByte_F1
	rgoto	MON_MIDI_NotifyReceivedByte_F2
	rgoto	MON_MIDI_NotifyReceivedByte_F3
	rgoto	MON_MIDI_NotifyReceivedByte_F4
	rgoto	MON_MIDI_NotifyReceivedByte_F5
	rgoto	MON_MIDI_NotifyReceivedByte_F6
	rgoto	MON_MIDI_NotifyReceivedByte_F7
	
MON_MIDI_NotifyReceivedByte_F0
	;; nothing else to do - branch to the end
	rgoto	MON_MIDI_NotifyReceivedByte_End

MON_MIDI_NotifyReceivedByte_F1
	;; MTC event: save 2 bytes when event complete
	rgoto	MON_MIDI_NotifyReceivedByte_2B

MON_MIDI_NotifyReceivedByte_F2
	;; Song position counter: save 3 bytes when event complete
	rgoto	MON_MIDI_NotifyReceivedByte_3B

MON_MIDI_NotifyReceivedByte_F3
	;; Song select: save 2 bytes when event complete
	rgoto	MON_MIDI_NotifyReceivedByte_2B

MON_MIDI_NotifyReceivedByte_F4
	;; undefined - just save byte
	rgoto	MON_MIDI_NotifyReceivedByte_1B

MON_MIDI_NotifyReceivedByte_F5
	;; undefined - just save byte
	rgoto	MON_MIDI_NotifyReceivedByte_1B

MON_MIDI_NotifyReceivedByte_F6
	;; tune request - just save byte
	rgoto	MON_MIDI_NotifyReceivedByte_1B

MON_MIDI_NotifyReceivedByte_F7
	;; SysEx end: save byte to notify that SysEx is complete
	;; 	rgoto	MON_MIDI_NotifyReceivedByte_1B

;; --- called on a 1-byte event
MON_MIDI_NotifyReceivedByte_1B
	;; received byte already in MIOS_PARAMETER1 !!!
	clrf	MIOS_PARAMETER2
	clrf	MIOS_PARAMETER3
	clrf	MON_MIDI_BYTE_CTR	; reset counter to consider running status
	rgoto	MON_MIDI_NotifyReceivedEvent

;; --- called on a 2-byte event
MON_MIDI_NotifyReceivedByte_2B
	decf	MON_MIDI_BYTE_CTR, W
	bnz	MON_MIDI_NotifyReceivedByte_End
	movff	MON_RUNNING_STATUS, MIOS_PARAMETER1
	lfsr	FSR0, MON_SYSEX_BUFFER_BEGIN
	movf	MON_SYSEX_PTR, W
	movf	PLUSW0, W
	movwf	MIOS_PARAMETER2
	movwf	MIOS_PARAMETER3
	clrf	MON_MIDI_BYTE_CTR	; reset counter to consider running status
	rgoto	MON_MIDI_NotifyReceivedEvent

;; --- called on a 3-byte event
MON_MIDI_NotifyReceivedByte_3B
	movf	MON_MIDI_BYTE_CTR, W
	addlw	-2
	bnz	MON_MIDI_NotifyReceivedByte_End
	movff	MON_RUNNING_STATUS, MIOS_PARAMETER1
	lfsr	FSR0, MON_SYSEX_BUFFER_BEGIN
	decf	MON_SYSEX_PTR, W
	andlw	0x0f
	movff	PLUSW0, MIOS_PARAMETER2
	addlw	1
	movff	PLUSW0, MIOS_PARAMETER3
	clrf	MON_MIDI_BYTE_CTR	; reset counter to consider running status
	rgoto	MON_MIDI_NotifyReceivedEvent

;; --- the end
MON_MIDI_NotifyReceivedByte_End
	return
