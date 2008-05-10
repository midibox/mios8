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
	;; cut the MIDI channel from the status byte and save value in TMP1
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	movwf	TMP1

	;; check if a note off event has been received
	xorlw	0x80
	skpnz
	rgoto	MMD_CORE_NoteOff	; -> Note Off branch

	;; check if a note on event has been received
	movf	TMP1, W
	xorlw	0x90
	skpnz
	rgoto	MMD_CORE_NoteOn		; -> Note On branch

	;; check if a controller on the control channel has been received
	;; (if feature has not been disabled)
	movf	MMD_MIDI_IN_CFG_CHN, W
	bz	MMD_MPROC_ReceivedEvent_CfgDis
	decf	MMD_MIDI_IN_CFG_CHN, W
	andlw	0x0f		; ensure that channel is with 0x00 and 0x0f (+1)
	iorlw	0xb0		; we are checking "B<channel>"
	xorwf	MIOS_PARAMETER1, W
	skpnz
	rgoto	MMD_MPROC_CC_Cfg	; -> CC Cfg branch
MMD_MPROC_ReceivedEvent_CfgDis

	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI event has been received
;;  which has been specified in the CONFIG_MIDI_IN table
;;  Input:
;;     o number of entry in WREG
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyFoundEvent
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
	;; if MMD_BPM is not zero (not in MIDI slave mode), ignore incoming clock
	movf	MMD_BPM, W
	skpz
	return

	;; check if MIDI Clock, jump to MIDI clock handler if true
	movf	MIOS_PARAMETER1, W
	xorlw	0xf8
	skpnz
	call	MMD_CORE_ClockHandler

	return


;; --------------------------------------------------------------------------
;;  This function is called by USER_MPROC_NotifyReceivedEvent
;;  when a CC event over the configuration channel has been received
;;  Input:
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
MMD_MPROC_CC_Cfg
	;; do nothing if CC is <= 0
	movlw	0x00
	cpfsgt	MIOS_PARAMETER2, ACCESS
	return

	;; do nothing if CC is >= 14
	movlw	0x14
	cpfslt	MIOS_PARAMETER2, ACCESS
	return

	movlw	0xfa
	call	MIOS_MIDI_TxBufferPut

	;; branch depending on CC
	movf	MIOS_PARAMETER2, W
	addlw	-1
	JUMPTABLE_2BYTES 13	; 13 entries
	rgoto	MMD_MPROC_CC_Dly0
	rgoto	MMD_MPROC_CC_Dly1
	rgoto	MMD_MPROC_CC_Dly2
	rgoto	MMD_MPROC_CC_Dly3
	rgoto	MMD_MPROC_CC_Transp0
	rgoto	MMD_MPROC_CC_Transp1
	rgoto	MMD_MPROC_CC_Transp2
	rgoto	MMD_MPROC_CC_Transp3
	rgoto	MMD_MPROC_CC_Chn0
	rgoto	MMD_MPROC_CC_Chn1
	rgoto	MMD_MPROC_CC_Chn2
	rgoto	MMD_MPROC_CC_Chn3
	rgoto	MMD_MPROC_CC_ChnIn
;; ---
MMD_MPROC_CC_Dly0
	movff	MIOS_PARAMETER3, MMD_QUEUE0_DLY	; copy parameter into register
MMD_MPROC_CC_DlyMsg
	movlw	MMD_DISPLAY_REQUEST_DLY		; request a message
	movwf	MMD_DISPLAY_REQUEST		; (will be handled by mmd_display.inc)
	return
;; ---
MMD_MPROC_CC_Dly1
	movff	MIOS_PARAMETER3, MMD_QUEUE1_DLY	; copy parameter into register
	rgoto	MMD_MPROC_CC_DlyMsg	; request dly message
;; ---
MMD_MPROC_CC_Dly2
	movff	MIOS_PARAMETER3, MMD_QUEUE2_DLY	; copy parameter into register
	rgoto	MMD_MPROC_CC_DlyMsg	; request dly message
;; ---
MMD_MPROC_CC_Dly3
	movff	MIOS_PARAMETER3, MMD_QUEUE3_DLY	; copy parameter into register
	rgoto	MMD_MPROC_CC_DlyMsg	; request dly message
;; ---
MMD_MPROC_CC_Transp0
	movff	MIOS_PARAMETER3, MMD_QUEUE0_TRANSP; copy parameter into register
MMD_MPROC_CC_TranspMsg
	movlw	MMD_DISPLAY_REQUEST_TRANSP	; request a message
	movwf	MMD_DISPLAY_REQUEST		; (will be handled by mmd_display.inc)
	return
;; ---
MMD_MPROC_CC_Transp1
	movff	MIOS_PARAMETER3, MMD_QUEUE1_TRANSP; copy parameter into register
	rgoto	MMD_MPROC_CC_TranspMsg	; request transp message
;; ---
MMD_MPROC_CC_Transp2
	movff	MIOS_PARAMETER3, MMD_QUEUE2_TRANSP; copy parameter into register
	rgoto	MMD_MPROC_CC_TranspMsg	; request transp message
;; ---
MMD_MPROC_CC_Transp3
	movff	MIOS_PARAMETER3, MMD_QUEUE3_TRANSP; copy parameter into register
	rgoto	MMD_MPROC_CC_TranspMsg	; request transp message
;; ---
MMD_MPROC_CC_Chn0
	movlw	0x11			; ensure that value is <= 0x10
	cpfslt	MIOS_PARAMETER3, ACCESS
	return
	movff	MIOS_PARAMETER3, MMD_QUEUE0_CHN ; copy parameter into register
MMD_MPROC_CC_ChnMsg
	movlw	MMD_DISPLAY_REQUEST_CHN		; request a message
	movwf	MMD_DISPLAY_REQUEST		; (will be handled by mmd_display.inc)
	return
MMD_MPROC_CC_Chn1
	movlw	0x11			; ensure that value is <= 0x10
	cpfslt	MIOS_PARAMETER3, ACCESS
	return
	movff	MIOS_PARAMETER3, MMD_QUEUE1_CHN ; copy parameter into register
	rgoto	MMD_MPROC_CC_ChnMsg	; request chn message
MMD_MPROC_CC_Chn2
	movlw	0x11			; ensure that value is <= 0x10
	cpfslt	MIOS_PARAMETER3, ACCESS
	return
	movff	MIOS_PARAMETER3, MMD_QUEUE2_CHN ; copy parameter into register
	rgoto	MMD_MPROC_CC_ChnMsg	; request chn message
MMD_MPROC_CC_Chn3
	movlw	0x11			; ensure that value is <= 0x10
	cpfslt	MIOS_PARAMETER3, ACCESS
	return
	movff	MIOS_PARAMETER3, MMD_QUEUE3_CHN ; copy parameter into register
	rgoto	MMD_MPROC_CC_ChnMsg	; request chn message
;; ---
MMD_MPROC_CC_ChnIn
	movlw	0x11			; ensure that value is <= 0x10
	cpfslt	MIOS_PARAMETER3, ACCESS
	return
	movff	MIOS_PARAMETER3, MMD_MIDI_IN_CHN ; copy parameter into register
MMD_MPROC_CC_CfgMsg
	movlw	MMD_DISPLAY_REQUEST_CFG		; request a message
	movwf	MMD_DISPLAY_REQUEST		; (will be handled by mmd_display.inc)
	return


