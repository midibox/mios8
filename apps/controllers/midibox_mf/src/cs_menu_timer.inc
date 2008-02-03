; $Id$
;
; Control Surface Menu Timer Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency is given
;;  by the SRIO update period (usually 1mS)
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
CS_MENU_TIMER

	;; we want to flash the cursor with following "waveform":
	;; 400 ms: cursor on (CS_STAT.CS_STAT_CURSOR_FLASH set)
	;; 100 ms: cursor off (CS_STAT.CS_STAT_CURSOR_FLASH cleared)

	;; means: cursor off for 1 * postscaler_factor cycles
	;;        cursor on for  4 * postscaler_factor cycles
	;; the postcaler_factor is 100 (since this routine is called every mS)

	decfsz	CS_MENU_TIMER_CTR, F; since this routine is called every UPDATE_FREQUENCY*1 mS, but the cursor
	return			; should be flashed with a much lower frequency, we are using
				; a 8-bit pre-counter (continue every 100 times = 100 mS)
	movlw	100 / 1 ; DEFAULT_SRIO_UPDATE_FREQUENCY
	movwf	CS_MENU_TIMER_CTR

	;; decrement delayed callback counter if != 0
	movf	CS_MENU_DEL_CALLBACK_CTR, W
	skpz
	decf	CS_MENU_DEL_CALLBACK_CTR, F

	;; decrement message counter if counter > 1
	decf	CS_MENU_MESSAGE_CTR, W
	bz	CS_MENU_TIMER_NoMsgDec
	movf	CS_MENU_MESSAGE_CTR, W
	bz	CS_MENU_TIMER_NoMsgDec
	decf	CS_MENU_MESSAGE_CTR, F
	decf	CS_MENU_MESSAGE_CTR, W
	skpnz
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
CS_MENU_TIMER_NoMsgDec

	;; handle with cursor
	incf	CS_MENU_CURSOR_CTR, F

	movf	CS_MENU_CURSOR_CTR, W
	xorlw	1				; CTR == 1 -> Cursor On
	bz	CS_MENU_TIMER_CursorOn

	movf	CS_MENU_CURSOR_CTR, W		; CTR == 5 -> Cursor Off and clear counter
	xorlw	5
	bz	CS_MENU_TIMER_CursorOff

	return				; else do nothing (return)

CS_MENU_TIMER_CursorOn
	;; set "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bsf	CS_STAT, CS_STAT_CURSOR_FLASH
	;; request a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

CS_MENU_TIMER_CursorOff
	;; clear counter
	clrf	CS_MENU_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH
	;; request a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return
