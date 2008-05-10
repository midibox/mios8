; $Id$
;
; MIOS Application DIN Driver
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an button has been toggled
;;  Input:
;;     o Button number in WREG and MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
USER_DIN_NotifyToggle
	movwf	TMP1			; save number of button in TMP1
	movff	MIOS_PARAMETER2, TMP2	; save status in TMP2

	;; set LED depending on value
	CALL_IFCLR TMP2, 0, ACCESS, MIOS_DOUT_PinSet1	; if button pressed: switch-on LED
	CALL_IFSET TMP2, 0, ACCESS, MIOS_DOUT_PinSet0	; if button depressed: switch-off LED

	;; dont continue if button has been depressed
	btfsc	TMP2, 0
	return

	;; branch depending on pressed button
	movf	TMP1, W
	JUMPTABLE_2BYTES 30	; max. 30 entries
	rgoto	MMD_BUTTON_Delay0Down
	rgoto	MMD_BUTTON_Delay0Up
	rgoto	MMD_BUTTON_Delay1Down
	rgoto	MMD_BUTTON_Delay1Up
	rgoto	MMD_BUTTON_Delay2Down
	rgoto	MMD_BUTTON_Delay2Up
	rgoto	MMD_BUTTON_Delay3Down
	rgoto	MMD_BUTTON_Delay3Up
	rgoto	MMD_BUTTON_Transp0Down
	rgoto	MMD_BUTTON_Transp0Up
	rgoto	MMD_BUTTON_Transp1Down
	rgoto	MMD_BUTTON_Transp1Up
	rgoto	MMD_BUTTON_Transp2Down
	rgoto	MMD_BUTTON_Transp2Up
	rgoto	MMD_BUTTON_Transp3Down
	rgoto	MMD_BUTTON_Transp3Up
	rgoto	MMD_BUTTON_Chn0Down
	rgoto	MMD_BUTTON_Chn0Up
	rgoto	MMD_BUTTON_Chn1Down
	rgoto	MMD_BUTTON_Chn1Up
	rgoto	MMD_BUTTON_Chn2Down
	rgoto	MMD_BUTTON_Chn2Up
	rgoto	MMD_BUTTON_Chn3Down
	rgoto	MMD_BUTTON_Chn3Up
	rgoto	MMD_BUTTON_ChnInDown
	rgoto	MMD_BUTTON_ChnInUp
	rgoto	MMD_BUTTON_ChnCfgDown
	rgoto	MMD_BUTTON_ChnCfgUp
	rgoto	MMD_BUTTON_BPMDown
	rgoto	MMD_BUTTON_BPMUp


;; --------------------------------------------------------------------------
;;  Branches to the Button Handlers
;; --------------------------------------------------------------------------
MMD_BUTTON_Delay0Down
	movlw	MMD_QUEUE0_DLY		; register which should be modified
	rgoto	MMD_BUTTON_DelayDownHandler ; jump to the handler
MMD_BUTTON_Delay0Up
	movlw	MMD_QUEUE0_DLY		; register which should be modified
	rgoto	MMD_BUTTON_DelayUpHandler ; jump to the handler
;; ---
MMD_BUTTON_Delay1Down
	movlw	MMD_QUEUE1_DLY		; register which should be modified
	rgoto	MMD_BUTTON_DelayDownHandler ; jump to the handler
MMD_BUTTON_Delay1Up
	movlw	MMD_QUEUE1_DLY		; register which should be modified
	rgoto	MMD_BUTTON_DelayUpHandler ; jump to the handler
;; ---
MMD_BUTTON_Delay2Down
	movlw	MMD_QUEUE2_DLY		; register which should be modified
	rgoto	MMD_BUTTON_DelayDownHandler ; jump to the handler
MMD_BUTTON_Delay2Up
	movlw	MMD_QUEUE2_DLY		; register which should be modified
	rgoto	MMD_BUTTON_DelayUpHandler ; jump to the handler
;; ---
MMD_BUTTON_Delay3Down
	movlw	MMD_QUEUE3_DLY		; register which should be modified
	rgoto	MMD_BUTTON_DelayDownHandler ; jump to the handler
MMD_BUTTON_Delay3Up
	movlw	MMD_QUEUE3_DLY		; register which should be modified
	rgoto	MMD_BUTTON_DelayUpHandler ; jump to the handler
;; ---
MMD_BUTTON_Transp0Down
	movlw	MMD_QUEUE0_TRANSP	; register which should be modified
	rgoto	MMD_BUTTON_TranspDownHandler ; jump to the handler
MMD_BUTTON_Transp0Up
	movlw	MMD_QUEUE0_TRANSP	; register which should be modified
	rgoto	MMD_BUTTON_TranspUpHandler ; jump to the handler
;; ---
MMD_BUTTON_Transp1Down
	movlw	MMD_QUEUE1_TRANSP	; register which should be modified
	rgoto	MMD_BUTTON_TranspDownHandler ; jump to the handler
MMD_BUTTON_Transp1Up
	movlw	MMD_QUEUE1_TRANSP	; register which should be modified
	rgoto	MMD_BUTTON_TranspUpHandler ; jump to the handler
;; ---
MMD_BUTTON_Transp2Down
	movlw	MMD_QUEUE2_TRANSP	; register which should be modified
	rgoto	MMD_BUTTON_TranspDownHandler ; jump to the handler
MMD_BUTTON_Transp2Up
	movlw	MMD_QUEUE2_TRANSP	; register which should be modified
	rgoto	MMD_BUTTON_TranspUpHandler ; jump to the handler
;; ---
MMD_BUTTON_Transp3Down
	movlw	MMD_QUEUE3_TRANSP	; register which should be modified
	rgoto	MMD_BUTTON_TranspDownHandler ; jump to the handler
MMD_BUTTON_Transp3Up
	movlw	MMD_QUEUE3_TRANSP	; register which should be modified
	rgoto	MMD_BUTTON_TranspUpHandler ; jump to the handler
;; ---
MMD_BUTTON_Chn0Down
	movlw	MMD_QUEUE0_CHN		; register which should be modified
	rgoto	MMD_BUTTON_ChnDownHandler ; jump to the handler
MMD_BUTTON_Chn0Up
	movlw	MMD_QUEUE0_CHN		; register which should be modified
	rgoto	MMD_BUTTON_ChnUpHandler ; jump to the handler
;; ---
MMD_BUTTON_Chn1Down
	movlw	MMD_QUEUE1_CHN		; register which should be modified
	rgoto	MMD_BUTTON_ChnDownHandler ; jump to the handler
MMD_BUTTON_Chn1Up
	movlw	MMD_QUEUE1_CHN		; register which should be modified
	rgoto	MMD_BUTTON_ChnUpHandler ; jump to the handler
;; ---
MMD_BUTTON_Chn2Down
	movlw	MMD_QUEUE2_CHN		; register which should be modified
	rgoto	MMD_BUTTON_ChnDownHandler ; jump to the handler
MMD_BUTTON_Chn2Up
	movlw	MMD_QUEUE2_CHN		; register which should be modified
	rgoto	MMD_BUTTON_ChnUpHandler ; jump to the handler
;; ---
MMD_BUTTON_Chn3Down
	movlw	MMD_QUEUE3_CHN		; register which should be modified
	rgoto	MMD_BUTTON_ChnDownHandler ; jump to the handler
MMD_BUTTON_Chn3Up
	movlw	MMD_QUEUE3_CHN		; register which should be modified
	rgoto	MMD_BUTTON_ChnUpHandler ; jump to the handler
;; ---
MMD_BUTTON_ChnInDown
	movlw	MMD_MIDI_IN_CHN		; register which should be modified
	rgoto	MMD_BUTTON_CfgDownHandler; jump to the handler
MMD_BUTTON_ChnInUp
	movlw	MMD_MIDI_IN_CHN		; register which should be modified
	rgoto	MMD_BUTTON_CfgUpHandler; jump to the handler
;; ---
MMD_BUTTON_ChnCfgDown
	movlw	MMD_MIDI_IN_CFG_CHN	; register which should be modified
	rgoto	MMD_BUTTON_CfgDownHandler; jump to the handler
MMD_BUTTON_ChnCfgUp
	movlw	MMD_MIDI_IN_CFG_CHN	; register which should be modified
	rgoto	MMD_BUTTON_CfgUpHandler; jump to the handler
;; ---
MMD_BUTTON_BPMDown
	movlw	MMD_BPM			; register which should be modified
	rgoto	MMD_BUTTON_BPMDownHandler; jump to the handler
MMD_BUTTON_BPMUp
	movlw	MMD_BPM			; register which should be modified
	rgoto	MMD_BUTTON_BPMUpHandler; jump to the handler
;; ---


;; --------------------------------------------------------------------------
;;  The Button Handlers
;; --------------------------------------------------------------------------
MMD_BUTTON_DelayDownHandler
	lfsr	FSR0, MMD_QUEUE0_DLY	; prepare FSR0 (high byte)
	movwf	FSR0L			; store WREG in low byte of address
	;; do nothing if delay is already zero
	movlw	0x00
	cpfsgt	INDF0, ACCESS
	rgoto MMD_BUTTON_DelayDownMsg
	;; else decrement value
	decf	INDF0, F
	;; print message
	rgoto	MMD_BUTTON_DelayDownMsg
;; ---
MMD_BUTTON_DelayUpHandler
	lfsr	FSR0, MMD_QUEUE0_DLY	; prepare FSR0 (high byte)
	movwf	FSR0L			; store WREG in low byte of address
	;; do nothing if max value reached
	movlw	0x7f			; (max value is 0x7f)
	cpfslt	INDF0, ACCESS
	rgoto MMD_BUTTON_DelayUpMsg
	;; else decrement value
	incf	INDF0, F
	;; print message
MMD_BUTTON_DelayDownMsg
MMD_BUTTON_DelayUpMsg
	movlw	MMD_DISPLAY_REQUEST_DLY		; request a message
	movwf	MMD_DISPLAY_REQUEST		; (will be handled by mmd_display.inc)
	return

;; --------------------------------------------------------------------------
MMD_BUTTON_TranspDownHandler
	lfsr	FSR0, MMD_QUEUE0_TRANSP	; prepare FSR0 (high byte)
	movwf	FSR0L			; store WREG in low byte of address
	;; do nothing if transpose value is already 0x00
	movlw	0x00
	cpfsgt	INDF0, ACCESS
	rgoto MMD_BUTTON_TranspDownMsg
	;; else decrement value
	decf	INDF0, F
	;; print message
	rgoto	MMD_BUTTON_TranspDownMsg
;; ---
MMD_BUTTON_TranspUpHandler
	lfsr	FSR0, MMD_QUEUE0_TRANSP	; prepare FSR0 (high byte)
	movwf	FSR0L			; store WREG in low byte of address
	;; do nothing if max value reached
	movlw	0x7f			; (max value is 0x7f)
	cpfslt	INDF0, ACCESS
	rgoto MMD_BUTTON_TranspUpMsg
	;; else decrement value
	incf	INDF0, F
	;; print message
MMD_BUTTON_TranspUpMsg
MMD_BUTTON_TranspDownMsg
	movlw	MMD_DISPLAY_REQUEST_TRANSP	; request a message
	movwf	MMD_DISPLAY_REQUEST		; (will be handled by mmd_display.inc)
	return

;; --------------------------------------------------------------------------
MMD_BUTTON_ChnDownHandler
	lfsr	FSR0, MMD_MIDI_IN_CHN	; prepare FSR0 (high byte)
	movwf	FSR0L			; store WREG in low byte of address
	;; do nothing if channel value is already 0x00
	movlw	0x00
	cpfsgt	INDF0, ACCESS
	rgoto MMD_BUTTON_ChnDownMsg
	;; else decrement value
	decf	INDF0, F
	;; print message
	rgoto	MMD_BUTTON_ChnDownMsg
;; ---
MMD_BUTTON_ChnUpHandler
	lfsr	FSR0, MMD_MIDI_IN_CHN	; prepare FSR0 (high byte)
	movwf	FSR0L			; store WREG in low byte of address
	;; do nothing if max value reached
	movlw	0x10			; (max value is 0x10)
	cpfslt	INDF0, ACCESS
	rgoto MMD_BUTTON_ChnUpMsg
	;; else decrement value
	incf	INDF0, F
	;; print message
MMD_BUTTON_ChnDownMsg
MMD_BUTTON_ChnUpMsg
	movlw	MMD_DISPLAY_REQUEST_CHN		; request a message
	movwf	MMD_DISPLAY_REQUEST		; (will be handled by mmd_display.inc)
	return

;; --------------------------------------------------------------------------
MMD_BUTTON_CfgDownHandler
	lfsr	FSR0, MMD_QUEUE0_CHN	; prepare FSR0 (high byte)
	movwf	FSR0L			; store WREG in low byte of address
	;; do nothing if config value is already 0x00
	movlw	0x00
	cpfsgt	INDF0, ACCESS
	rgoto MMD_BUTTON_CfgDownMsg
	;; else decrement value
	decf	INDF0, F
	;; print message
	rgoto	MMD_BUTTON_CfgDownMsg
;; ---
MMD_BUTTON_CfgUpHandler
	lfsr	FSR0, MMD_QUEUE0_CHN	; prepare FSR0 (high byte)
	movwf	FSR0L			; store WREG in low byte of address
	;; do nothing if max value reached
	movlw	0x10			; (max value is 0x0f)
	cpfslt	INDF0, ACCESS
	rgoto MMD_BUTTON_CfgUpMsg
	;; else decrement value
	incf	INDF0, F
	;; print message
MMD_BUTTON_CfgDownMsg
MMD_BUTTON_CfgUpMsg
	movlw	MMD_DISPLAY_REQUEST_CFG		; request a message
	movwf	MMD_DISPLAY_REQUEST		; (will be handled by mmd_display.inc)
	return

;; --------------------------------------------------------------------------
MMD_BUTTON_BPMDownHandler
	lfsr	FSR0, MMD_QUEUE0_CHN	; prepare FSR0 (high byte)
	movwf	FSR0L			; store WREG in low byte of address
	;; do nothing if config value is already 0x00
	movlw	0x00
	cpfsgt	INDF0, ACCESS
	rgoto MMD_BUTTON_BPMDownMsg
	;; else decrement value
	decf	INDF0, F
	;; important: reconfigure timer for new BPM setting
	movf	MMD_BPM, W
	call	MMD_Timer_BPMSet
	;; print message
	rgoto	MMD_BUTTON_BPMDownMsg
;; ---
MMD_BUTTON_BPMUpHandler
	lfsr	FSR0, MMD_QUEUE0_CHN	; prepare FSR0 (high byte)
	movwf	FSR0L			; store WREG in low byte of address
	;; do nothing if max value reached
	movlw	0xff			; (max value is 0xff)
	cpfslt	INDF0, ACCESS
	rgoto MMD_BUTTON_BPMUpMsg
	;; else decrement value
	incf	INDF0, F
	;; important: reconfigure timer for new BPM setting
	movf	MMD_BPM, W
	call	MMD_Timer_BPMSet
	;; print message
MMD_BUTTON_BPMDownMsg
MMD_BUTTON_BPMUpMsg
	movlw	MMD_DISPLAY_REQUEST_CFG		; request a message
	movwf	MMD_DISPLAY_REQUEST		; (will be handled by mmd_display.inc)
	return
