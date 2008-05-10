; $Id$
;
; MIOS Application Timer
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
TC_Timer
	;; timer should be called every 100 ms == 1000000 ticks
	;; since this value is greater than 16 bit (== 65535 max) we just
	;; use a 1:8 prescaler and an additional software-implemented 1:2 postscaler
	;; so the init value must be: 1000000 / (8*2) == 62500
	;; (see also the MIOS_TIMER_Init documentation)

	;; timer has been initialized in tc_init.inc

	;; decrement TC_SELECT_CTR if content > 0
	;; we are using this counter in tc_display.inc to print temporary messages
	movf	TC_SELECT_CTR, W
	skpz
	decf	TC_SELECT_CTR, F
	skpz
	return			; if counter > 0, don't handle the flash routine

	incf	TC_TIMER_CTR, F			; increment postscaler

	;; we want to flash the cursor with following "waveform":
	;; 900 ms: cursor on (TC_STAT.TC_STAT_CURSOR_FLASH set)
	;; 100 ms: cursor off (TC_STAT.TC_STAT_CURSOR_FLASH cleared)

	;; means: cursor off for 2 * postscaler_factor cycles
	;;        cursor on for  8 * postscaler_factor cycles
	;; the postcaler_factor is 2

	movf	TC_TIMER_CTR, W
	xorlw	2*2			; CTR == 2*2 -> Cursor On
	bz	TC_Timer_CursorOn

	movf	TC_TIMER_CTR, W		; CTR == 10*2 -> Cursor Off and clear counter
	xorlw	10*2
	bz	TC_Timer_CursorOff

	return				; else do nothing (return)

TC_Timer_CursorOn
	;; set "TC_STAT_CURSOR_FLASH" bit (see tc_display.inc for the handling)
	bsf	TC_STAT, TC_STAT_CURSOR_FLASH
	;; request a display update
	bsf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ
	return

TC_Timer_CursorOff
	;; clear counter
	clrf	TC_TIMER_CTR
	;; clear "TC_STAT_CURSOR_FLASH" bit (see tc_display.inc for the handling)
	bcf	TC_STAT, TC_STAT_CURSOR_FLASH
	;; request a display update
	bsf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ
	return

