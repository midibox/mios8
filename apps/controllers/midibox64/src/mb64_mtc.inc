; $Id$
;
; MIDIbox64 MTC handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MB_MPROC when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MB64_MTC_NotifyReceivedByte
	;; call MTC handler
	movf	MIOS_PARAMETER1, W
	call	MTC_Handler
	;; returns 0x01 when the complete MTC has been received, else 0x00
	;; if return status != 0, convert the MTC to digits
	bz	MB64_MTC_NotifyReceivedByte_End

	;; convert "hour"
	SET_BSR	MTC_HR
	movf	MTC_HR, W, BANKED
	andlw	0x1f
	call	CONVERT_Bin2BCD	; result in MSD and LSD
	bsf	LSD, 7		; (set dot)
	movff	MSD, LEDDIGITS_7
	movff	LSD, LEDDIGITS_6
	;; convert "minutes"
	movf	MTC_MN, W, BANKED
	call	CONVERT_Bin2BCD	; result in MSD and LSD
	bsf	LSD, 7		; (set dot)
	movff	MSD, LEDDIGITS_5
	movff	LSD, LEDDIGITS_4
	;; convert "seconds"
	movf	MTC_SC, W, BANKED
	call	CONVERT_Bin2BCD	; result in MSD and LSD
	bsf	LSD, 7		; (set dot)
	movff	MSD, LEDDIGITS_3
	movff	LSD, LEDDIGITS_2
	;; convert "frame"
	movf	MTC_FR, W, BANKED
	call	CONVERT_Bin2BCD	; result in MSD and LSD
	movff	MSD, LEDDIGITS_1
	movff	LSD, LEDDIGITS_0

	;; display MTC at first line
	bsf	MB_STAT, MB_STAT_SHOW_MTC
	bcf	MB_STAT, MB_STAT_SHOW_MIDICLK

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

MB64_MTC_NotifyReceivedByte_End
	return
