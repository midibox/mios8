; $Id$
;
; MBSEQ Menu "BPM"
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This table contains the edit values
;; --------------------------------------------------------------------------
CS_M_BPM_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					255-48+1, 0x47, 5,	CS_M_BPM_ValueChanged


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
CS_M_BPM_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; set external clock divider
	movff	MIOS_PARAMETER1, SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD

	;; update the LEDs and exit
	;; 	rgoto	CS_M_BPM_UpdateGPLEDs

CS_M_BPM_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; set LED depending on clock divider value
	SET_BSR	SEQ_BASE
	movf	SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD, W, BANKED
	call	MIOS_HLP_GetBitORMask
	btfss	SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD, 3, BANKED
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	btfsc	SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD, 3, BANKED
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_BPM_Init
	;; install parameter table
	TABLE_ADDR CS_M_BPM_PARAMETERS		; pointer to table
	movlw	1				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy BPM value to menu parameter
	call	SEQ_BPM_Get
	addlw	-48+1
	skpc
	movlw	0x00
	movwf	CS_MENU_PARAMETER_L

	;; install callback function for general purpose buttons, used as ext clock divider buttons here
	TABLE_ADDR CS_M_BPM_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rcall	CS_M_BPM_UpdateGPLEDs

	TABLE_ADDR CS_M_BPM_STR0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_BPM_STR0 STRING 16, 0x00, "Beats per Minute"
CS_M_BPM_STR1 STRING  5, 0x40, "---->"

CS_M_BPM_STR_S STRING 5, 0x47, "Slave"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_BPM_Tick
	;; branch depending on BPM setting
	call	SEQ_BPM_Get
	bnz	CS_M_BPM_Tick_Master

CS_M_BPM_Tick_Slave
	TABLE_ADDR CS_M_BPM_STR_S
	call	MIOS_LCD_PrintString
	rgoto	CS_M_BPM_Tick_Cont

CS_M_BPM_Tick_Master
	movwf	TMP1

	movlw	0x47
	call	MIOS_LCD_CursorSet
	movf	TMP1, W
	call	MIOS_LCD_PrintBCD3
	movlw	2
	call	SEQ_LCD_PrintSpaces
CS_M_BPM_Tick_Cont

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_BPM_ValueChanged
	;; set new BPM
	movf	CS_MENU_PARAMETER_L, W
	skpz
	addlw	48-1
	goto	SEQ_BPM_Set

;; ==========================================================================
