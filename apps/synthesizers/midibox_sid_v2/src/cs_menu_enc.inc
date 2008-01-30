; $Id$
;
; Control Surface Encoder Handler
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  CS Encoder Handler
;;  IN: incrementer of menu encoder in WREG
;; --------------------------------------------------------------------------
CS_MENU_ENC_Handler
	;; store incrementer in CS_MENU_ENC_INCREMENTER
	movwf	CS_MENU_ENC_INCREMENTER

	;; exit with message if SID player mode enabled and SID1 selected
	GOTO_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, SIDPLAY_NoCSMessage

	;; if page message is displayed (page up/down button pressed),
	;; allow to use main encoder to change page as well
	GOTO_IFSET CS_STAT, CS_STAT_PAGE_MSG, ACCESS, CS_MENU_PageUpDown_NoWrap

	;; due to double function of SHIFT button (faster encoders + display CFG page):
	;; disable CFG page if encoder is turned
	btfsc	CS_STAT3, CS_STAT3_CFG_PAGE
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ	; LCD has to be re-initialized
	bcf	CS_STAT3, CS_STAT3_CFG_PAGE

	;; special measure for main page (where the encoder selects a patch/ensemble)
	;; determine CS_MENU_PARAMETER_[LH] value depending on MENU button state
	;; MENU button depressed: modify patch
	;; MENU button pressed: modify ensemble
	CALL_IFSET CS_MENU, 7, ACCESS, CS_MENU_ParameterSet_Main

	;; inc/dec value by using the 16 bit add w/ saturation routine
	;; set pointer to CS_MENU_PARAMETER_L (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, CS_MENU_PARAMETER_L
	;; set max value
	movff	CS_MENU_PARAMETER_MAX_L, MIOS_PARAMETER1	; low-byte of max value
	movff	CS_MENU_PARAMETER_MAX_H, MIOS_PARAMETER2	; high-byte of max value
	movf	CS_MENU_ENC_INCREMENTER, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = CS_MENU_PARAMETER_[LH] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; do nothing if value has not been changed
	btfss	MIOS_PARAMETER1, 0
	return

	;; if main page (CS_MENU[7] set): update patch/ensemble and init sound
	BRA_IFCLR CS_MENU, 7, ACCESS, CS_MENU_ENC_Handler_NotMainPage2
CS_MENU_ENC_Handler_MainPage2
	GOTO_IFCLR CS_MENU_MODE, CS_MENU_MODE_MENU_PRESSED, ACCESS, CS_MENU_PatchUpdate
	movff	CS_MENU_PARAMETER_L, SID_ENSEMBLE
	goto	CS_MENU_EnsUpdate
CS_MENU_ENC_Handler_NotMainPage2

	;; if not in selection mode: update cursor pos
	GOTO_IFCLR CS_STAT, CS_STAT_MODIFY_PARAMETER, ACCESS, CS_MENU_MenuPosUpdate

	;; else do a parameter update
	goto	CS_MENU_ParameterUpdate


;; --------------------------------------------------------------------------
;;  CS Encoder Handler
;;  IN: encoder number (-1) in MIOS_PARAMETER1
;;      incrementer in MIOS_PARAMETER2
;;  NOTE: the menu encoder will not be counted as encoder #0 here to avoid
;;        problems with the INCDEC_BUTTONS option
;; --------------------------------------------------------------------------
CS_MENU_ENC_CS_Handler
	;; store incrementer in CS_MENU_ENC_INCREMENTER
	movf	MIOS_PARAMETER2, W
	movwf	CS_MENU_ENC_INCREMENTER
	;; ...and in MIOS_PARAMETER3 (could be changed in future, CS_MENU_ENC_INCREMENTER to be used by subfunctions)
	movwf	MIOS_PARAMETER3

	;; exit with message if SID player mode enabled and SID1 selected
	GOTO_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, SIDPLAY_NoCSMessage

	;; due to double function of SHIFT button (faster encoders + display CFG page):
	;; disable CFG page if encoder is turned
	btfsc	CS_STAT3, CS_STAT3_CFG_PAGE
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ	; LCD has to be re-initialized
	bcf	CS_STAT3, CS_STAT3_CFG_PAGE

	;; copy SID number to CS_MENU_SID
	call	CS_MENU_MS_GetSIDNumber

	;; determine offset (quick & dirty, should be programmed on a more elegant way later)
	movlw	0x04
	cpfsgt	MIOS_PARAMETER1, ACCESS
	rgoto CS_MENU_ENC_CS_Osc
	movlw	0x06
	cpfsgt	MIOS_PARAMETER1, ACCESS
	rgoto CS_MENU_ENC_CS_LFO
	movlw	0x08
	cpfsgt	MIOS_PARAMETER1, ACCESS
	rgoto CS_MENU_ENC_CS_Filter
	rgoto	CS_MENU_ENC_CS_Env
;; ---
CS_MENU_ENC_CS_Osc
	movf	MIOS_PARAMETER1, W
	btfsc	CS_MENU_SELECTED_OSC_CTRL, 0; Misc Layer
	addlw 0x05   
	btfsc	CS_MENU_SELECTED_OSC_CTRL, 1; Assign Layer
	addlw 0x0a   
	rgoto	CS_MENU_ENC_CS_Cont
CS_MENU_ENC_CS_LFO
	movf	MIOS_PARAMETER1, W
	addlw	0x0f-0x05
	rgoto	CS_MENU_ENC_CS_Cont
CS_MENU_ENC_CS_Filter
	movf	MIOS_PARAMETER1, W
	addlw	0x11-0x07
	rgoto	CS_MENU_ENC_CS_Cont
CS_MENU_ENC_CS_Env
	movf	MIOS_PARAMETER1, W
	addlw	0x13-0x09
	btfsc	CS_MENU_SELECTED_ENV_CTRL, 0; Assign Layer
	addlw 0x05   
#if DEFAULT_MB6582_CS
	btfsc	CS_MENU_SELECTED_ENV_CTRL, 1; Misc Layer
	addlw 0x0a   
#endif
	;; 	rgoto	CS_MENU_ENC_CS_Cont
;; ---
CS_MENU_ENC_CS_Cont
	;; store table position in TABLAT
	movwf	TABLAT

	;; add table offset depending on selected engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	TABLE_ADDR_MUL_W CS_MENU_ENC_TABLE_L, CS_MENU_ENC_TABLE_LBDM_OFFSET

	;; add entry offset
	rlf	TABLAT, W
	rlf	WREG, W
	andlw	0xfc
	TABLE_ADD_W

	;; get menu number and store it in TMP1
	tblrd*+
	movf	TABLAT, W
	movwf	TMP1

	;; skip if disabled
	bz	CS_MENU_ENC_CS_Change_Menu_End	

	;; get cursor pos and store it in MIOS_PARAMETER2
	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2

	;; get menu offset and store it in MIOS_PARAMETER1
	tblrd*+
	movff	TABLAT, MIOS_PARAMETER1

	;; branch if menu == 0xff (change currently visible parameters)
	incf	TMP1, W
	bz	CS_MENU_ENC_CS_Change_Item

CS_MENU_ENC_CS_Change_Par
	movf	TMP1, W		; menu number
	goto	CS_MENU_ENC_CSInc

CS_MENU_ENC_CS_Change_Item
	;; ignore if in main or menu page
	movf	CS_MENU, W
	andlw	0x7f
	bz	CS_MENU_ENC_CS_Change_Menu_End
	;; else modify value of menu depending on given offset in MIOS_PARAMETER2
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	addwf	CS_MENU_PAGE_OFFSET, W
        movwf   MIOS_PARAMETER1
        ;; ignore if parameter pos >= number of entries
        movf    CS_MENU_ENTRIES, W
	cpfslt	MIOS_PARAMETER1, ACCESS
	rgoto CS_MENU_ENC_CS_Change_Menu_End
        ;; else modify parameter
        movff   CS_MENU_PAGE_OFFSET, MIOS_PARAMETER2
	movf	CS_MENU, W
	goto	CS_MENU_ENC_CSInc
CS_MENU_ENC_CS_Change_Menu_End	
	return
