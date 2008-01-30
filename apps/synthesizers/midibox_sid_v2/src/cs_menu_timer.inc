; $Id$
;
; SID Control Surface Menu Timer Handler
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
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
	decfsz	CS_MENU_TIMER_CTR, F; since this routine is called every 819 uS, but the cursor
	return			; should be flashed with a much lower frequency, we are using
				; a 8-bit pre-counter (continue every 122 times = ca. 100 ms)
	movlw	122
	movwf	CS_MENU_TIMER_CTR

	;; decrement CS_SELECT_CTR if content > 0
	;; we are using this counter in cs_menu.inc to print temporary messages
	movf	CS_SELECT_CTR, W
	skpz
	decf	CS_SELECT_CTR, F
	skpz
	return			; if counter > 0, don't handle the flash routine

	incf	CS_CURSOR_CTR, F			; increment postscaler

	;; CS_MENU_M_TX counter is used to notify that a patch can be changed
	movf	CS_MENU_TX_M_CTR, W
	andlw	0x7f		; bit 7 indicates that a new patch should be sent - it has to be masked out here
	skpz
	decf	CS_MENU_TX_M_CTR, F

	;; same for the slaves
	movf	CS_MENU_TX_S1_CTR, W
	andlw	0x7f
	skpz
	decf	CS_MENU_TX_S1_CTR, F

	movf	CS_MENU_TX_S2_CTR, W
	andlw	0x7f
	skpz
	decf	CS_MENU_TX_S2_CTR, F

	movf	CS_MENU_TX_S3_CTR, W
	andlw	0x7f
	skpz
	decf	CS_MENU_TX_S3_CTR, F

	;; reinit counter used to restore the configuration
	movf	CS_MENU_REINIT_CFG_CTR, W
	bz	CS_MENU_TIMER_NoCfgCtrDec
	decf	CS_MENU_REINIT_CFG_CTR, W
	bz	CS_MENU_TIMER_NoCfgCtrDec
	decf	CS_MENU_REINIT_CFG_CTR, F
CS_MENU_TIMER_NoCfgCtrDec

	;; if not in edit mode...
	BRA_IFSET CS_MENU_MODE, CS_MENU_MODE_EDIT, ACCESS, CS_MENU_Timer_InEditMode
	;; do nothing else not in menu (CS_MENU[7] set)
	btfsc	CS_MENU, 7
	return
CS_MENU_Timer_InEditMode

	;; set flashing edit LED
	bcf	CS_MENU_MODE, CS_MENU_MODE_EDIT_FLASHED
	btfsc	CS_MENU_MODE, CS_MENU_MODE_EDIT
	bsf	CS_MENU_MODE, CS_MENU_MODE_EDIT_FLASHED
	
	;; we want to flash the cursor with following "waveform":
	;; 800 ms: cursor on (CS_STAT.CS_STAT_CURSOR_FLASH set)
	;; 200 ms: cursor off (CS_STAT.CS_STAT_CURSOR_FLASH cleared)

	;; means: cursor off for 2 * postscaler_factor cycles
	;;        cursor on for  8 * postscaler_factor cycles
	;; the postcaler_factor is 2

	movf	CS_CURSOR_CTR, W
	xorlw	2*2			; CTR == 2*2 -> Cursor On
	bz	CS_MENU_TIMER_CursorOn

	movf	CS_CURSOR_CTR, W		; CTR == 10*2 -> Cursor Off and clear counter
	xorlw	10*2
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
	;; clear flashing edit LED if in edit mode and something has been changed
	BRA_IFCLR CS_MENU_MODE, CS_MENU_MODE_EDIT, ACCESS, CS_MENU_TIMER_CursorOff_NoEdit
	btfsc	CS_MENU_MODE, CS_MENU_MODE_EDIT_NOTIFIER
	bcf	CS_MENU_MODE, CS_MENU_MODE_EDIT_FLASHED
CS_MENU_TIMER_CursorOff_NoEdit
	;; request a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

