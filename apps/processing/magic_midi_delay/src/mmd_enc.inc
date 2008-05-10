; $Id$
;
; MIOS Application Encoder Driver
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an encoder has been moved
;;  Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
USER_ENC_NotifyChange
	;; branch depending on encoder number
	JUMPTABLE_2BYTES 9	; (9 entries)
	rgoto	MMD_ENC_Dly0	; v-pot #1
	rgoto	MMD_ENC_Dly1	; v-pot #2
	rgoto	MMD_ENC_Dly2	; v-pot #3
	rgoto	MMD_ENC_Dly3	; v-pot #4
	rgoto	MMD_ENC_Transp0	; v-pot #5
	rgoto	MMD_ENC_Transp1	; v-pot #6
	rgoto	MMD_ENC_Transp2	; v-pot #7
	rgoto	MMD_ENC_Transp3	; v-pot #8
	rgoto	MMD_ENC_BPM	; jog-wheel

;; --------------------------------------------------------------------------
;;  Branches to the Encoder Handlers
;; --------------------------------------------------------------------------
MMD_ENC_Dly0
	movlw	MMD_QUEUE0_DLY		; register which should be modified
	rgoto	MMD_ENC_DlyHandler	; jump to the handler
;; ---
MMD_ENC_Dly1
	movlw	MMD_QUEUE1_DLY		; register which should be modified
	rgoto	MMD_ENC_DlyHandler	; jump to the handler
;; ---
MMD_ENC_Dly2
	movlw	MMD_QUEUE2_DLY		; register which should be modified
	rgoto	MMD_ENC_DlyHandler	; jump to the handler
;; ---
MMD_ENC_Dly3
	movlw	MMD_QUEUE3_DLY		; register which should be modified
	rgoto	MMD_ENC_DlyHandler	; jump to the handler
;; ---
MMD_ENC_Transp0
	movlw	MMD_QUEUE0_TRANSP		; register which should be modified
	rgoto	MMD_ENC_TranspHandler	; jump to the handler
;; ---
MMD_ENC_Transp1
	movlw	MMD_QUEUE1_TRANSP		; register which should be modified
	rgoto	MMD_ENC_TranspHandler	; jump to the handler
;; ---
MMD_ENC_Transp2
	movlw	MMD_QUEUE2_TRANSP		; register which should be modified
	rgoto	MMD_ENC_TranspHandler	; jump to the handler
;; ---
MMD_ENC_Transp3
	movlw	MMD_QUEUE3_TRANSP		; register which should be modified
	rgoto	MMD_ENC_TranspHandler	; jump to the handler
;; ---
MMD_ENC_BPM
	movlw	MMD_BPM			; register which should be modified
	rgoto	MMD_ENC_BPMHandler	; jump to the handler

;; --------------------------------------------------------------------------
;;  The Encoder Handlers
;; --------------------------------------------------------------------------
MMD_ENC_DlyHandler
	lfsr	FSR0, MMD_QUEUE0_DLY	; prepare FSR0 (high byte)
	movwf	FSR0L			; store WREG in low byte of address

	;; branch depending on clockwise/counter clockwise turn
	BRA_IFSET MIOS_PARAMETER2, 7, ACCESS, MMD_ENC_DlyHandler_Dec
MMD_ENC_DlyHandler_Inc
	;; add incrementer to current delay
	movf	MIOS_PARAMETER2, W
	addwf	INDF0, F
	;; continue if max value has not been reached
	movlw	0x7f
	cpfsgt	INDF0, ACCESS
	rgoto MMD_ENC_DlyHandler_Cont
	;; else set max value
	movlw	0x7f
	movwf	INDF0
	rgoto	MMD_ENC_DlyHandler_Cont

MMD_ENC_DlyHandler_Dec
	;; subtract incrementer from current delay
	movf	MIOS_PARAMETER2, W	; (it's a negative value...)
	addwf	INDF0, F
	;; continue if no overflow
	bc	MMD_ENC_DlyHandler_Cont
	;; else set value to zero
	clrf	INDF0
	;; 	rgoto	MMD_ENC_DlyHandler_Cont
MMD_ENC_DlyHandler_Cont
	movlw	MMD_DISPLAY_REQUEST_DLY		; request a message
	movwf	MMD_DISPLAY_REQUEST		; (will be handled by mmd_display.inc)
	return

;; --------------------------------------------------------------------------
MMD_ENC_TranspHandler
	lfsr	FSR0, MMD_QUEUE0_TRANSP	; prepare FSR0 (high byte)
	movwf	FSR0L			; store WREG in low byte of address

	;; branch depending on clockwise/counter clockwise turn
	BRA_IFSET MIOS_PARAMETER2, 7, ACCESS, MMD_ENC_TranspHandler_Dec
MMD_ENC_TranspHandler_Inc
	;; add incrementer to current delay
	movf	MIOS_PARAMETER2, W
	addwf	INDF0, F
	;; continue if max value has not been reached
	movlw	0x7f
	cpfsgt	INDF0, ACCESS
	rgoto MMD_ENC_TranspHandler_Cont
	;; else set max value
	movlw	0x7f
	movwf	INDF0
	rgoto	MMD_ENC_TranspHandler_Cont

MMD_ENC_TranspHandler_Dec
	;; subtract incrementer from current delay
	movf	MIOS_PARAMETER2, W	; (it's a negative value...)
	addwf	INDF0, F
	;; continue if no overflow
	bc	MMD_ENC_TranspHandler_Cont
	;; else set value to zero
	clrf	INDF0
	;; 	rgoto	MMD_ENC_TranspHandler_Cont
MMD_ENC_TranspHandler_Cont
	movlw	MMD_DISPLAY_REQUEST_TRANSP	; request a message
	movwf	MMD_DISPLAY_REQUEST		; (will be handled by mmd_display.inc)
	return

;; --------------------------------------------------------------------------
MMD_ENC_BPMHandler
	lfsr	FSR0, MMD_BPM	; prepare FSR0 (high byte)
	movwf	FSR0L			; store WREG in low byte of address

	;; branch depending on clockwise/counter clockwise turn
	BRA_IFSET MIOS_PARAMETER2, 7, ACCESS, MMD_ENC_BPMHandler_Dec
MMD_ENC_BPMHandler_Inc
	;; add incrementer to current delay
	movf	MIOS_PARAMETER2, W
	addwf	INDF0, F
	;; continue if no overflow
	bnc	MMD_ENC_BPMHandler_Cont
	;; else set max value
	movlw	0xff
	movwf	INDF0
	rgoto	MMD_ENC_BPMHandler_Cont

MMD_ENC_BPMHandler_Dec
	;; subtract incrementer from current delay
	movf	MIOS_PARAMETER2, W	; (it's a negative value...)
	addwf	INDF0, F
	;; continue if no overflow
	bc	MMD_ENC_BPMHandler_Cont
	;; else set value to zero
	clrf	INDF0
	;; 	rgoto	MMD_ENC_BPMHandler_Cont
MMD_ENC_BPMHandler_Cont

	;; important: reconfigure timer for new BPM setting
	movf	MMD_BPM, W
	call	MMD_Timer_BPMSet

	movlw	MMD_DISPLAY_REQUEST_CFG		; request a message
	movwf	MMD_DISPLAY_REQUEST		; (will be handled by mmd_display.inc)
	return

;; --------------------------------------------------------------------------
