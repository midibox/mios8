; $Id$
;
; MBFM Control Surface Menu Timer Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
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
CS_MENU_TIMER
	decfsz	CS_MENU_TIMER_CTR, F; since this routine is called every 1 ms, but the cursor
	return			; should be flashed with a much lower frequency, we are using
				; a 8-bit pre-counter (continue every 100 times = 100 ms)
	movlw	100
	movwf	CS_MENU_TIMER_CTR

	;; decrement CS_SELECT_CTR if content > 0
	;; we are using this counter in cs_menu.inc to print temporary messages
	movf	CS_SELECT_CTR, W
	skpz
	decf	CS_SELECT_CTR, F
	skpz
	return			; if counter > 0, don't handle the flash routine

	incf	CS_CURSOR_CTR, F			; increment postscaler

	;; do nothing else not in menu (CS_MENU[7] set)
	btfsc	CS_MENU, 7
	return

	;; we want to flash the cursor with following "waveform":
	;; 400 ms: cursor on (CS_STAT.CS_STAT_CURSOR_FLASH set)
	;; 100 ms: cursor off (CS_STAT.CS_STAT_CURSOR_FLASH cleared)

	;; means: cursor off for 1 * postscaler_factor cycles
	;;        cursor on for  4 * postscaler_factor cycles
	;; the postcaler_factor is 2

	movf	CS_CURSOR_CTR, W
	xorlw	1*2			; CTR == 1*2 -> Cursor On
	bz	CS_MENU_TIMER_CursorOn

	movf	CS_CURSOR_CTR, W	; CTR == 5*2 -> Cursor Off and clear counter
	xorlw	5*2
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
	clrf	CS_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH
	;; request a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; if WT write has been requested, grant it now
	btfsc	CS_EEPROM_WR, CS_EEPROM_WR_WT_REQ
	bsf	CS_EEPROM_WR, CS_EEPROM_WR_WT_GNT
	return

