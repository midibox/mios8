; $Id$
;
; MBSEQ Menu "Manual Trigger"
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
CS_M_TRG_GP_Callback
	;; by default turn off the LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; GP LEDs: show triggered sequencer pos
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1

	btfsc	MIOS_PARAMETER1, 3
	movlw 0x00
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	movf	TMP1, W
	btfss	MIOS_PARAMETER1, 3
	movlw 0x00
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; forward manual trigger to SEQ_CORE
	movff	MIOS_PARAMETER1, SEQ_CURRENT_STEP
	call	SEQ_CORE_ManualTrigger

	;; trigger pot display page and exit
	goto	SEQ_MSG_Start

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRG_Init
	;; no parameter table!

	;; install callback function for general purpose buttons, used to trigger the steps here
	TABLE_ADDR CS_M_TRG_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; by default turn off the LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; don't flash LEDs
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; print static string
	TABLE_ADDR CS_M_TRG_STR0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_TRG_STR0 STRING 14, 0x01, "Manual Trigger"
CS_M_TRG_STR1 STRING  5, 0x43, "Track"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRG_Tick
	;; print selected tracks at second line
	movlw	0x49
	call	MIOS_LCD_CursorSet
	call	SEQ_LCD_PrnSelectedTrks

	;; and exit
	return

;; ==========================================================================

