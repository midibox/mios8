; $Id$
;
; MBSEQ Menu "Mute Tracks"
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This is the menu specific GP callback function called by cs_menu_button.inc 
;;  when the GP hook has been installed
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1 (0x00-0x0f)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;;  Output:
;;     should return the GP LED values in
;;     CS_MENU_GP_OVERLAY_DOUT_SR[01]
;; --------------------------------------------------------------------------
CS_M_MUTE_GP_Callback
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; toggle mute flag
	lfsr	FSR1, SEQ_TRKS_MUTED0
	btfsc	MIOS_PARAMETER1, 3
	movf	POSTINC1, W	; increment pointer if track > 8
	
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	xorwf	INDF1, F

	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_MUTE_Init
	;; no parameter table!

	;; install callback function for general purpose buttons, used to mute/unmute the tracks here
	TABLE_ADDR CS_M_MUTE_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; don't flash LEDs
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; print static string
	TABLE_ADDR CS_M_MUTE_STR0
	goto	MIOS_LCD_PrintString

CS_M_MUTE_STR0 STRING 13, 0x00, "Muted Tracks:"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_MUTE_Tick
	;; print muted tracks at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet

	movff	SEQ_TRKS_MUTED0, TMP1
	movff	SEQ_TRKS_MUTED1, TMP2
	movlw	16
	movwf	TMP3
CS_M_MUTE_Tick_Loop
	movlw	'o'
	btfsc	TMP1, 0
	movlw '*'
	call	MIOS_LCD_PrintChar
	rrf	TMP2, F
	rrf	TMP1, F
	decfsz	TMP3, F
	rgoto	CS_M_MUTE_Tick_Loop

	;; transfer track mute status (16 tracks) to CS_MENU_GP_OVERLAY_DOUT_SR[01]
	movff	SEQ_TRKS_MUTED0, CS_MENU_GP_OVERLAY_DOUT_SR0
	movff	SEQ_TRKS_MUTED1, CS_MENU_GP_OVERLAY_DOUT_SR1

	;; and exit
	return

;; ==========================================================================

