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
	SET_BSR	SEQ_BASE
	movf	MIOS_PARAMETER1, W
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	addlw	0x10
	movwf	SEQ_CURRENT_STEP, BANKED
	call	SEQ_CORE_ManualTrigger

	;; trigger pot display page and exit
	goto	SEQ_MSG_Start

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRG_Init
	;; no parameter table!

	;; switch to full screen view
	call	SEQ_LCD_SetMsgOffsets

	;; install callback function for general purpose buttons, used to trigger the steps here
	TABLE_ADDR CS_M_TRG_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; by default turn off the LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; don't flash LEDs
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	return

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRG_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Manual Step Trigger                     Selected Tracks: 1234 1234 1234 1234
	;; 00000000000000--------------------------1111111111111111-2222-3333-4444-5555---
	;; lower line: print GxTy and -7...0...7 (0x08..0x17)

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0xf8
	bnz	CS_M_TRG_Tick_Lower

CS_M_TRG_Tick_Upper
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRG_Tick_0
	rgoto	CS_M_TRG_Tick_1
	rgoto	CS_M_TRG_Tick_2
	rgoto	CS_M_TRG_Tick_3
	rgoto	CS_M_TRG_Tick_4
	rgoto	CS_M_TRG_Tick_5
	rgoto	CS_M_TRG_Tick_6
	rgoto	CS_M_TRG_Tick_7

CS_M_TRG_Tick_0_STR	STRING 19, 0x00 + 0, "Manual Step Trigger"
CS_M_TRG_Tick_0
	TABLE_ADDR CS_M_TRG_Tick_0_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRG_Tick_Cont

CS_M_TRG_Tick_1_STR	STRING 16, 0x80 + 0, "Selected Tracks:"
CS_M_TRG_Tick_1
	TABLE_ADDR CS_M_TRG_Tick_1_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRG_Tick_Cont

CS_M_TRG_Tick_2
CS_M_TRG_Tick_3
CS_M_TRG_Tick_4
CS_M_TRG_Tick_5
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-2
	movwf	TMP1		; (group)
	mullw	5
	movlw	0x80 + 17
	addwf	PRODL, W
	call	MIOS_LCD_CursorSet
	
	movf	TMP1, W		; print selected tracks of group
	call	SEQ_LCD_PrnSelectedTrks
	rgoto	CS_M_TRG_Tick_Cont

CS_M_TRG_Tick_6
CS_M_TRG_Tick_7
	;; unused
	rgoto	CS_M_TRG_Tick_Cont

CS_M_TRG_Tick_Lower
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-8
	movwf	TMP1		; (step)
	andlw	0x07
	mullw	5
	movlw	0x40
	btfsc	TMP1, 3
	movlw 0xc0
	addwf	PRODL, W
	addlw	1
	call	MIOS_LCD_CursorSet

	SET_BSR	SEQ_BASE
	incf	TMP1, W
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	addlw 16
	call	MIOS_LCD_PrintBCD2
	;; 	rgoto	CS_M_TRG_Tick_Cont		

CS_M_TRG_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movlw	24-1
	cpfsgt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_M_TRG_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_MUTE_Tick_End

CS_M_TRG_Tick_End
	return

;; ==========================================================================

