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
	;; 4444444444444444444---------------------5555555555555555-0000-1111-2222-3333----
	;; lower line: print track 1..16

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	skpz
	call	CS_M_HLP_LCD_Tick_Resume ; (will branch to next CS_M_TRG_Tick_x)

        ;; ------------------------------------------------------------------
CS_M_TRG_Tick_0123
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	mullw	5
	movlw	0x80 + 17
	addwf	PRODL, W
	call	MIOS_LCD_CursorSet
	
	movf	CS_MENU_REFRESH_ITEM_CTR, W	; print selected tracks of group
	call	SEQ_LCD_PrnSelectedTrks
	rcall	CS_M_TRG_Tick_Yield		; repeat 4 times
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 2, ACCESS, CS_M_TRG_Tick_0123

        ;; ------------------------------------------------------------------
CS_M_TRG_Tick_4
	call	SEQ_LCD_PrintStringAfterCall
	STRING	19, 0x00 + 0, "Manual Step Trigger"
	rcall	CS_M_TRG_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRG_Tick_5
	call	SEQ_LCD_PrintStringAfterCall
	STRING	16, 0x80 + 0, "Selected Tracks:"
	rcall	CS_M_TRG_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRG_Tick_6
	rcall	CS_M_TRG_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRG_Tick_7
	rcall	CS_M_TRG_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRG_Tick_Lower
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-8
	movwf	TMP1		; (step)
	andlw	0x07
	mullw	5
	movlw	0x40
	btfsc	TMP1, 3
	movlw	0xc0
	addwf	PRODL, W
	addlw	1
	call	MIOS_LCD_CursorSet

	SET_BSR	SEQ_BASE
	incf	TMP1, W
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	addlw	16
	call	MIOS_LCD_PrintBCD2

        movlw   24-1
	cpfslt  CS_MENU_REFRESH_ITEM_CTR, ACCESS
	goto	CS_M_HLP_LCD_Tick_End
	rcall	CS_M_TRG_Tick_Yield
	rgoto	CS_M_TRG_Tick_Lower

        ;; ------------------------------------------------------------------
CS_M_TRG_Tick_End
	goto	CS_M_HLP_LCD_Tick_End

CS_M_TRG_Tick_Yield
	goto	CS_M_HLP_LCD_Tick_Yield

;; ==========================================================================

