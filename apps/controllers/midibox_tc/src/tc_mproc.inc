; $Id$
;
; MIOS Application MIDI Processor
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedEvent
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI event has been received
;;  which has been specified in the MIOS_MPROC_EVENT_TABLE table
;;  Input:
;;     o number of entry in WREG
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyFoundEvent

	;; feedback can be so easy... :-)
	
	;; since WREG corresponds to the parameter number in MIOS_MPROC_EVENT_TABLE
	;; we just only have to copy the value in MIOS_PARAMETER3 to the
	;; parameter array

	;; calculate pointer to first parameter: FSR0 = TC_VALUES_BEGIN, low byte=WREG
	lfsr	FSR0, TC_VALUES_BEGIN	; base address
	movwf	FSR0L			; write WREG to FSR0L

	;; copy MIOS_PARAMETER3 to parameter value
	movff	MIOS_PARAMETER3, INDF0

	;; now request a display update so that we see the new parameter on screen
	;; if it is in the current page
	bsf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ	; (see tc_display.inc)

	;; thats all
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI event has not been completly
;;  received within 2 seconds
;; --------------------------------------------------------------------------
USER_MPROC_NotifyTimeout
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	return
