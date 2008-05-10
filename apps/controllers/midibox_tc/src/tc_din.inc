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
TC_DIN_Handler
	;; only handle the first 6 buttons
	;; since the encoder is connected to pin 0 and 1, the buttons begin
	;; at index 2 ... to 7
	;; Note: this routine will not be called on encoder events, so
	;; MIOS_PARAMETER1 will never be 0x00 or 0x01
	;; so just check if number is >= 0x08
	movlw	0x08
	cpfslt	MIOS_PARAMETER1, ACCESS; leave handler in this case
	return	

	;; leave handler also if the button has been depressed (-> MIOS_PARAMETER2.0 == 1)
	btfsc	MIOS_PARAMETER2, 0
	return

	;; button has been pressed
	;; index to button in MIOS_PARAMETER1
	movf	MIOS_PARAMETER1, W
	JUMPTABLE_2BYTES 8	; 8 entries
	rgoto	TC_DIN_Handler_Pin0	; encoder -- will never be executed
	rgoto	TC_DIN_Handler_Pin1
	rgoto	TC_DIN_Handler_Pin2	; button below encoder
	rgoto	TC_DIN_Handler_Pin3	; rightmost select button (#5)
	rgoto	TC_DIN_Handler_Pin4	; select button #4
	rgoto	TC_DIN_Handler_Pin5	; select button #3
	rgoto	TC_DIN_Handler_Pin6	; select button #2
	rgoto	TC_DIN_Handler_Pin7	; select button #1

;; --------------------------------------------------------------------------
TC_DIN_Handler_Pin0
	;; will never be executed, ignore
	return

	;; ------------------------------------------------------------------
TC_DIN_Handler_Pin1
	;; will never be executed, ignore
	return

	;; ------------------------------------------------------------------
TC_DIN_Handler_Pin2
	;; button below encoder, switch between the TC_MODEs
	;; branch depending on current mode
	movf	TC_MODE, W
	JUMPTABLE_2BYTES 2	; 2 entries
	rgoto	TC_DIN_Handler_SwitchToMode1
	rgoto	TC_DIN_Handler_SwitchToMode0
	;; you could add more modes and actions here...

TC_DIN_Handler_SwitchToMode1
	;; switch to mode 1
	movlw	TC_MODE_PARA
	movwf	TC_MODE
	;; encoder #0 in normal mode
	movlw	MIOS_ENC_SPEED_NORMAL
	movwf	MIOS_PARAMETER1
	movlw	0x00
	call	MIOS_ENC_SpeedSet
	;; request display update
	bsf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ	; request display update
	return

TC_DIN_Handler_SwitchToMode0
	;; switch to mode 0
	movlw	TC_MODE_MENU
	movwf	TC_MODE
	;; encoder #0 in slow mode
	movlw	MIOS_ENC_SPEED_SLOW
	movwf	MIOS_PARAMETER1
	movlw	0x00
	call	MIOS_ENC_SpeedSet
	;; request display update
	bsf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ	; request display update
	return

	;; ------------------------------------------------------------------
TC_DIN_Handler_Pin3
	;; select button #5
	movlw	0x04
	movwf	TC_PARA
	bsf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ	; request display update
	bsf	TC_STAT, TC_STAT_DISPLAY_LONGLABEL_REQ	; request a temporary "long label" message
	return

	;; ------------------------------------------------------------------
TC_DIN_Handler_Pin4
	;; select button #4
	movlw	0x03
	movwf	TC_PARA
	bsf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ	; request display update
	bsf	TC_STAT, TC_STAT_DISPLAY_LONGLABEL_REQ	; request a temporary "long label" message
	return

	;; ------------------------------------------------------------------
TC_DIN_Handler_Pin5
	;; select button #3
	movlw	0x02
	movwf	TC_PARA
	bsf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ	; request display update
	bsf	TC_STAT, TC_STAT_DISPLAY_LONGLABEL_REQ	; request a temporary "long label" message
	return

	;; ------------------------------------------------------------------
TC_DIN_Handler_Pin6
	;; select button #2
	movlw	0x01
	movwf	TC_PARA
	bsf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ	; request display update
	bsf	TC_STAT, TC_STAT_DISPLAY_LONGLABEL_REQ	; request a temporary "long label" message
	return

	;; ------------------------------------------------------------------
TC_DIN_Handler_Pin7
	;; select button #1
	movlw	0x00
	movwf	TC_PARA
	bsf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ	; request display update
	bsf	TC_STAT, TC_STAT_DISPLAY_LONGLABEL_REQ	; request a temporary "long label" message
	return
