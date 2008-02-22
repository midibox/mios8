; $Id$
;
; MBCV Menu "Display"
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This table contains the edit values
;; --------------------------------------------------------------------------
CS_M_DISPLAY_PARAMETERS		; for patch changes
 ;;			 address / signed flag			max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x7f,   0x08, 3,	CS_M_DISPLAY_PatchChanged


;; --------------------------------------------------------------------------
;;  Menu Exit routine is called when the exit button is pressed
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Exit
	;; disabled - inconsistent handling
	;; user has to step through all 16 characters to store patch!
	;; EXIT button allows to terminate the process
#if 0
	;; if save step 2 reached: store patch
	RCALL_IFSET CS_STAT2, CS_STAT2_SAVE_STEP2, ACCESS, CS_M_DISPLAY_StorePatch
#else
	;; restore original patch number and name instead
	BRA_IFCLR CS_STAT2, CS_STAT2_SAVE_STEP1, ACCESS, CS_M_DISPLAY_Exit_End

	movff	CS_MENU_SAVE_ORIGINAL_PATCH, CV_PATCH

	movlw	0x10
	movwf	PRODL
	lfsr	FSR0, CS_MENU_SAVE_ORIGINAL_PATCH_NAME
	lfsr	FSR1, CV_PATCH_NAME0
CS_M_DISPLAY_Exit_CopyLoop
	movff	POSTINC0, POSTINC1
	decfsz	PRODL, F
	rgoto	CS_M_DISPLAY_Exit_CopyLoop
CS_M_DISPLAY_Exit_End
#endif
	return

;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Select
	;; clear callback if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_M_BANK_Select_Depressed

	;; branch depending on save step
	BRA_IFSET CS_STAT2, CS_STAT2_SAVE_STEP2, ACCESS, CS_M_DISPLAY_Select_Step2
	BRA_IFSET CS_STAT2, CS_STAT2_SAVE_STEP1, ACCESS, CS_M_DISPLAY_Select_Step1

CS_M_DISPLAY_Select_Step0
	;; install delayed callback hook, used to trigger the format function after 2 seconds
	TABLE_ADDR CS_M_BANK_Select_SaveStart
	movlw	0x08
	call	CS_MENU_SetDelayedCallbackHook

	;; print message
	TABLE_ADDR CS_M_DISPLAY_SAVE_STR_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage
CS_M_DISPLAY_SAVE_STR_0	STRING 16, 0x00, " Hold 2 seconds "
CS_M_DISPLAY_SAVE_STR_1	STRING 16, 0x40, " to store Patch "

CS_M_BANK_Select_SaveStart
	;; store original patch number and name
	rcall	CS_M_BANK_SaveOrig
	;; start with first step
	bsf	CS_STAT2, CS_STAT2_SAVE_STEP1
	bcf	CS_STAT2, CS_STAT2_SAVE_STEP2
	;; re-init screen
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return
	

CS_M_DISPLAY_Select_Step1
	;; select second step
	bsf	CS_STAT2, CS_STAT2_SAVE_STEP2
	;; ensure that cursor position is cleared
	clrf	CS_MENU_CURSOR_POS
	;; re-init screen
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return

CS_M_DISPLAY_Select_Step2
	;; increment cursor
	incf	CS_MENU_CURSOR_POS, F
	;; update menu parameter depending on cursor pos
	rcall	CS_M_DISPLAY_UpdateChr_MenuParam
	;; nothing else to do so long cursor pos < 16
	BRA_IFCLR CS_MENU_CURSOR_POS, 4, ACCESS, CS_M_DISPLAY_Select_Step2_End
	;; store patch
	rcall	CS_M_DISPLAY_StorePatch
CS_M_DISPLAY_Select_Step2_End
	;; re-init screen
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return


CS_M_BANK_Select_Depressed
	;; deinstall hook and stop message
	movlw	0x00
	call	CS_MENU_SetDelayedCallbackHook
	goto	MIOS_LCD_MessageStop


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Init
	;; branch depending on save step
	BRA_IFSET CS_STAT2, CS_STAT2_SAVE_STEP2, ACCESS, CS_M_DISPLAY_Init_Step2
	BRA_IFSET CS_STAT2, CS_STAT2_SAVE_STEP1, ACCESS, CS_M_DISPLAY_Init_Step1

CS_M_DISPLAY_Init_Step0
	;; allow fast display update
	bsf	CS_STAT2, CS_STAT2_ALLOW_FAST_DISP_UPDATE

	;; install parameter table for patch change
	TABLE_ADDR CS_M_DISPLAY_PARAMETERS	; pointer to table
	movlw	1				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; always CV_PATCH in CS_MENU_PARAMETER_L
	movff	CV_PATCH, CS_MENU_PARAMETER_L

	;; print patch number at upper line
	movlw	0x00
	call	MIOS_LCD_CursorSet
	movlw	'P'
	call	MIOS_LCD_PrintChar
	call	CV_LCD_PrintPatchNumber

	;; 4 spaces
	movlw	4
	call	CV_LCD_PrintSpaces

	;; print the 8 special chars (used for animated meters)
	movlw	0x00
	movwf	PRODL
CS_M_DISPLAY_Init_Step0Loop
	movf	PRODL, W
	call	MIOS_LCD_PrintChar
	incf	PRODL, F
	BRA_IFCLR PRODL, 3, ACCESS, CS_M_DISPLAY_Init_Step0Loop

	;; invalidate charset number (will be created on-the-fly)
	setf	CS_MENU_CHARSET

	;; print patch name at lower line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	goto	CV_LCD_PrintPatch


CS_M_DISPLAY_Init_Step1
	;; install parameter table for patch change
	TABLE_ADDR CS_M_DISPLAY_PARAMETERS	; pointer to table
	movlw	1				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; always CV_PATCH in CS_MENU_PARAMETER_L
	movff	CV_PATCH, CS_MENU_PARAMETER_L

	;; print patch number at upper line
	TABLE_ADDR CS_M_DISPLAY_STR_STEP1
	call	MIOS_LCD_PrintString
	call	CV_LCD_PrintPatchNumber

	;; print patch name of destination patch at lower line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	goto	CV_LCD_PrintPatchEE	; taken from EEPROM

CS_M_DISPLAY_STR_STEP1 STRING 13, 0x00, "Save Dest.: P"


CS_M_DISPLAY_Init_Step2
	;; 0x60 characters are available
	movlw	0x60-1
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; update menu parameter depending on cursor pos
	rcall	CS_M_DISPLAY_UpdateChr_MenuParam

	;; install "changed" hook
	TABLE_ADDR CS_M_DISPLAY_ChrChanged
	call	CS_MENU_SetMenuParameterHook

	;; print patch number at upper line
	TABLE_ADDR CS_M_DISPLAY_STR_STEP2
	call	MIOS_LCD_PrintString
	call	CV_LCD_PrintPatchNumber

	;; patch name is print in DISPLAY_Tick
	return

CS_M_DISPLAY_STR_STEP2 STRING 13, 0x00, "Enter Name: P"


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Tick

	;; branch depending on save step
	BRA_IFSET CS_STAT2, CS_STAT2_SAVE_STEP2, ACCESS, CS_M_DISPLAY_Tick_Step2
	BRA_IFSET CS_STAT2, CS_STAT2_SAVE_STEP1, ACCESS, CS_M_DISPLAY_Tick_Step1

CS_M_DISPLAY_Tick_Step0
	;; skip if graphical LCD is used
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD
	return

	;; print meter (counter is decremented by cs_menu_timer.inc, and set within cv_midi.inc)
	movlw	0x08
	addwf	CS_MENU_REFRESH_ITEM_CTR, W
	call	MIOS_LCD_CursorSet

	;; special bar algorithm which allows to display bar value and trigger the same time
	;; (would look better with graphical display...)

	;; determine character RAM address
	swapf	CS_MENU_REFRESH_ITEM_CTR, W
	rrf	WREG, W
	andlw	0x38
	iorlw	0x40
	call	MIOS_LCD_Cmd

	;; meter value -> PRODL
	lfsr	FSR1, CS_MENU_METER_CTR_BEGIN
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	swapf	PLUSW1, W
	rrf	WREG, W
	andlw	0x07
	movwf	PRODL

	;; 3 MSBs of CV value -> PRODH
	lfsr	FSR1, CV_C1_AOUT_H
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	rrf	PLUSW1, W
	andlw	0x07
	movwf	PRODH

	;; create char
	movlw	7
	movwf	TMP1		; used as counter
CS_M_DISPLAY_Tick_Step0_Loop
	movf	TMP1, W
	cpfslt	PRODH, ACCESS
	rgoto CS_M_DISPLAY_Tick_Step0_Loop_1
	rgoto	CS_M_DISPLAY_Tick_Step0_Loop_0
CS_M_DISPLAY_Tick_Step0_Loop_1
	movlw	0x1f
	rgoto	CS_M_DISPLAY_Tick_Step0_Loop_C
CS_M_DISPLAY_Tick_Step0_Loop_0
	movlw	0x00
	;; 	rgoto	CS_M_DISPLAY_Tick_Step0_Loop_C
CS_M_DISPLAY_Tick_Step0_Loop_C
	movwf	TMP2

	;; trigger displayed inline
	movf	PRODL, W
	bz	CS_M_DISPLAY_Tick_Step0_Loop_C0
	movf	TMP1, W
	cpfslt	PRODL, ACCESS
	rgoto CS_M_DISPLAY_Tick_Step0_Loop_C1
	rgoto	CS_M_DISPLAY_Tick_Step0_Loop_C0
CS_M_DISPLAY_Tick_Step0_Loop_C1
	movlw	0x04
	rgoto	CS_M_DISPLAY_Tick_Step0_Loop_CC
CS_M_DISPLAY_Tick_Step0_Loop_C0
	movlw	0x00
	;; 	rgoto	CS_M_DISPLAY_Tick_Step0_Loop_CC
CS_M_DISPLAY_Tick_Step0_Loop_CC
	xorwf	TMP2, W
	call	MIOS_LCD_Data

CS_M_DISPLAY_Tick_Step0_LoopNext
	decf	TMP1, F
	BRA_IFCLR TMP1, 7, ACCESS, CS_M_DISPLAY_Tick_Step0_Loop


	;; increment refresh counter until last meter reached
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	btfss	CS_MENU_REFRESH_ITEM_CTR, 3
	return
	;; stop refresh
	clrf	CS_MENU_REFRESH_ITEM_CTR
	return


CS_M_DISPLAY_Tick_Step1
	;; nothing to do here...
	return


CS_M_DISPLAY_Tick_Step2
	;; print patch name at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	call	CV_LCD_PrintPatch

	;; print cursor (flashing)
	;; w/o arrows, therefore CS_MENU_CursorHandler not used!
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_DISPLAY_Tick_Step2_NoSpc
CS_M_DISPLAY_Tick_Step2_Spc
	movlw	0x40
	addwf	CS_MENU_CURSOR_POS, W
	call	MIOS_LCD_CursorSet
	movlw	' '
	call	MIOS_LCD_PrintChar
CS_M_DISPLAY_Tick_Step2_NoSpc

	;; enable blinking CLCD cursor and exit
	movlw	0x40
	addwf	CS_MENU_CURSOR_POS, W
	call	MIOS_LCD_CursorSet
	movlw	0x0e
	goto	MIOS_LCD_Cmd

	return


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_DISPLAY_STR_NO_BS_0 STRING 16, 0x00, "** No BankStick "
CS_M_DISPLAY_STR_NO_BS_1 STRING 16, 0x40, "** connected :-("

CS_M_DISPLAY_PatchChanged
	;; change patch
	movf	CS_MENU_PARAMETER_L, W
	movwf	CV_PATCH

	;; if patch != 0 and no BankStick connected: print message
	bz	CS_M_DISPLAY_PatchChanged_Ok
CS_M_DISPLAY_PatchChanged_Ext
	call	CV_BANK_CheckPatchValid
	bz	CS_M_DISPLAY_PatchChanged_Ok

CS_M_DISPLAY_PatchChanged_Err
	;; switch back to first patch and print error message
	clrf	CV_PATCH

	TABLE_ADDR CS_M_DISPLAY_STR_NO_BS_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage
CS_M_DISPLAY_PatchChanged_Ok

	;; load new patch if not in step 1
	CALL_IFCLR CS_STAT2, CS_STAT2_SAVE_STEP1, ACCESS, CV_DUMP_RestoreAll

	;; re-init screen
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return	

;; ==========================================================================
;; used during patch name edit

	;; updates the menu parameter depending on menu position
CS_M_DISPLAY_UpdateChr_MenuParam
	lfsr	FSR0, CV_PATCH_NAME0
	movf	CS_MENU_CURSOR_POS, W
	movf	PLUSW0, W
	addlw	-0x20			; (char 0x00..0x1f not selectable)
	btfsc	WREG, 7; (if char was < 0x20)
	movlw 0x00	
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H
	return


CS_M_DISPLAY_ChrChanged
	;; determine pointer to character -> FSR0
	lfsr	FSR0, CV_PATCH_NAME0
	movf	CS_MENU_CURSOR_POS, W
	addwf	FSR0L, F

	;; store character
	movf	CS_MENU_PARAMETER_L, W
	addlw	0x20
	movwf	INDF0

	;; extra: if the name only consists of characters <= 0x20, zero the bankname
	;; if any character > 0x20, ensure that all characters are >= 0x20
	movlw	0x10
	movwf	TMP1
	clrf	TMP2
	lfsr	FSR0, CV_PATCH_NAME0
CS_M_DISPLAY_ChrChangedChkLoop
	movlw	0x20
	cpfsgt	POSTINC0, ACCESS
	incf	TMP2, F
	decfsz	TMP1, F
	rgoto	CS_M_DISPLAY_ChrChangedChkLoop

	movlw	0x10
	cpfseq	TMP2, ACCESS
	rgoto CS_M_DISPLAY_ChrChangedSet20

	lfsr	FSR0, CV_PATCH_NAME0
CS_M_DISPLAY_ChrChangedClrLoop
	clrf	POSTINC0
	decfsz	TMP2, F
	rgoto	CS_M_DISPLAY_ChrChangedClrLoop

	return

CS_M_DISPLAY_ChrChangedSet20
	lfsr	FSR0, CV_PATCH_NAME0
	movlw	0x10
	movwf	TMP1
CS_M_DISPLAY_ChrChangedSet20Loop
	movlw	0x20
	cpfslt	INDF0, ACCESS
	rgoto CS_M_DISPLAY_ChrChangedSet20LpNx
	movwf	INDF0
CS_M_DISPLAY_ChrChangedSet20LpNx
	movf	POSTINC0, F	; increment pointer
	decfsz	TMP1, F		; loop 16 times
	rgoto	CS_M_DISPLAY_ChrChangedSet20Loop
	return


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Help function which stores the patch
;; --------------------------------------------------------------------------
CS_M_DISPLAY_STORED_STR_0 STRING 16, 0x00, "* Patch Stored *"
CS_M_DISPLAY_StorePatch
	;; print message
	TABLE_ADDR CS_M_DISPLAY_STORED_STR_0
	call	MIOS_LCD_PrintMessage
	;; store patch
	call	CV_DUMP_StoreAll
	;; update orig patch/name
	rcall	CS_M_BANK_SaveOrig
	;; select initial step
	bcf	CS_STAT2, CS_STAT2_SAVE_STEP1
	bcf	CS_STAT2, CS_STAT2_SAVE_STEP2
	;; ensure that cursor position is cleared again
	clrf	CS_MENU_CURSOR_POS
	return


;; --------------------------------------------------------------------------
;;  Help function which stores the current patch and name to a temporary location
;; --------------------------------------------------------------------------
CS_M_BANK_SaveOrig
	movff	CV_PATCH, CS_MENU_SAVE_ORIGINAL_PATCH
	;; store original patch name
	movlw	0x10
	movwf	PRODL
	lfsr	FSR0, CV_PATCH_NAME0
	lfsr	FSR1, CS_MENU_SAVE_ORIGINAL_PATCH_NAME
CS_M_BANK_SaveOrigLoop
	movff	POSTINC0, POSTINC1
	decfsz	PRODL, F
	rgoto	CS_M_BANK_SaveOrigLoop
	return

