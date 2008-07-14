; $Id$
;
; SID Control Surface Menu Execute Functions
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


; ==========================================================================
; ==========================================================================
;  refered by cs_menu_tables.inc
; ==========================================================================
; ==========================================================================

; ==========================================================================
;  All supported EXEC_* indices
; ==========================================================================
EXEC_NOP	EQU	0x00
EXEC_MENU	EQU	0x01
EXEC_MENU_SAVE	EQU	0x02
EXEC_MENU_SAVE_ENS EQU	0x03
EXEC_SELPAR	EQU	0x04
EXEC_SELPAR_ASG	EQU	0x05
EXEC_SELPAR_BANK EQU	0x06
EXEC_SELPAR_PATCH EQU	0x07
EXEC_TOGPAR	EQU	0x08
EXEC_RND	EQU	0x09
EXEC_DRM_RND	EQU	0x0a
EXEC_SAVE_PATCH	EQU	0x0b
EXEC_SAVE_ENS	EQU	0x0c
EXEC_EDIT_SEQ	EQU	0x0d
EXEC_SEQ_CPY	EQU	0x0e
EXEC_SEQ_PST	EQU	0x0f
EXEC_SEQ_CLR	EQU	0x10


; ==========================================================================
;  The EXEC handler
;; IN: EXEC_xxx ID in WREG
; ==========================================================================
CS_MENU_EXEC_Handler
	JUMPTABLE_2BYTES (0x10+1)	; entries
	rgoto	CS_MENU_EXEC_NOP
	rgoto	CS_MENU_EXEC_Menu
	rgoto	CS_MENU_EXEC_Menu_Save
	rgoto	CS_MENU_EXEC_Menu_SaveEns
	rgoto	CS_MENU_EXEC_SelPar
	rgoto	CS_MENU_EXEC_SelPar_Asg
	rgoto	CS_MENU_EXEC_SelPar_Bank
	rgoto	CS_MENU_EXEC_SelPar_Patch
	rgoto	CS_MENU_EXEC_TogPar
	rgoto	CS_MENU_EXEC_Rnd
	rgoto	CS_MENU_EXEC_DrmRnd
	rgoto	CS_MENU_EXEC_SavePatch
	rgoto	CS_MENU_EXEC_SaveEns
	rgoto	CS_MENU_EXEC_EditSeq
	rgoto	CS_MENU_EXEC_SEQ_CPY
	rgoto	CS_MENU_EXEC_SEQ_PST
	rgoto	CS_MENU_EXEC_SEQ_CLR


; ==========================================================================
;  Dummy function, can be called if nothing has to be done
; ==========================================================================
CS_MENU_EXEC_NOP
	return

; ==========================================================================
;  Use this function to change to a menu
; ==========================================================================
CS_MENU_EXEC_Menu
	;; was previousliy located in CS_MENU_Page_Init
	;; now done here, so that the cursor doesn't change when we are within a menu page and LCD initialisation is requested
	btfss	CS_STAT, CS_STAT_MODIFY_PARAMETER
	clrf CS_MENU_CURSOR_POS
	btfss	CS_STAT, CS_STAT_MODIFY_PARAMETER
	clrf CS_MENU_PAGE_OFFSET

	movf	FSR0L, W
	andlw	0x7f
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu


; ==========================================================================
;  Dummy function, can be called if nothing has to be done
; ==========================================================================
CS_MENU_EXEC_Nothing
	return

; ==========================================================================
;  Use this function to change to the save patch menu
; ==========================================================================
CS_MENU_EXEC_Menu_Save
	;; was previousliy located in CS_MENU_Page_Init
	;; now done here, so that the cursor doesn't change when we are within a menu page and LCD initialisation is requested
	btfss	CS_STAT, CS_STAT_MODIFY_PARAMETER
	clrf CS_MENU_CURSOR_POS
	btfss	CS_STAT, CS_STAT_MODIFY_PARAMETER
	clrf CS_MENU_PAGE_OFFSET

	;; copy current patch/bank number to SAVE_PATCH and SAVE_BANK and change menu
	call	CS_MENU_MS_GetSIDNumber
	call	CS_MENU_MS_GetSIDPatch; sets FSR1 to patch register
	movf	INDF1, W
	andlw	0x7f
	movwf	CS_MENU_SAVE_PATCH
	call	CS_MENU_MS_GetSIDBank; sets FSR1 to bank register
	movf	INDF1, W
	andlw	0x07
	movwf	CS_MENU_SAVE_BANK

	call	CS_MENU_GetMenuID_SAV
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu


; ==========================================================================
;  Use this function to change to the save ensemble menu
; ==========================================================================
CS_MENU_EXEC_Menu_SaveEns
	;; was previousliy located in CS_MENU_Page_Init
	;; now done here, so that the cursor doesn't change when we are within a menu page and LCD initialisation is requested
	btfss	CS_STAT, CS_STAT_MODIFY_PARAMETER
	clrf CS_MENU_CURSOR_POS
	btfss	CS_STAT, CS_STAT_MODIFY_PARAMETER
	clrf CS_MENU_PAGE_OFFSET

	;; copy current ensemble number to SAVE_ENS and change menu
	movff	SID_ENSEMBLE, CS_MENU_SAVE_ENS

	movlw	CS_MENU_E_SAV
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu


; ==========================================================================
;  Sequencer Copy Function
; ==========================================================================
TEXT_SEQ_CPY_OK    STRING 16, 0x40, "----- COPY -----"

CS_MENU_EXEC_SEQ_CPY
	;; notify that buffer filled
	bsf	CS_STAT2, CS_STAT2_COPY_BUFFER_FILLED

	;; branch depending on bassline/drum engine
	;; function only used by these engines, therefore this can be simplified:
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x02
	bz	CS_MENU_EXEC_SEQ_CPY_D

CS_MENU_EXEC_SEQ_CPY_B
	;; copy 2*16 bytes
	lfsr	FSR1, CS_COPY_PASTE_BUFFER_BEGIN
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES
	swapf	CS_MENU_WT_SEQ, W
	andlw	0x70
	iorwf	FSR0L, F

	clrf	CS_MENU_PARAMETER_IX		; using *_IX as loop counter
CS_MENU_EXEC_SEQ_CPY_B_Loop
	movf	CS_MENU_PARAMETER_IX, W
	movff	PLUSW0, PLUSW1		; note

	;; switch to parameter bank
	movlw	0x10
	addwf	FSR1L, F
	movlw	0x80
	addwf	FSR0L, F

	movf	CS_MENU_PARAMETER_IX, W
	movff	PLUSW0, PLUSW1		; parameter

	;; switch back to note bank
	movlw	-0x10
	addwf	FSR1L, F
	movlw	-0x80
	addwf	FSR0L, F

	incf	CS_MENU_PARAMETER_IX, F
	BRA_IFCLR CS_MENU_PARAMETER_IX, 4, ACCESS, CS_MENU_EXEC_SEQ_CPY_B_Loop

	rgoto	CS_MENU_EXEC_SEQ_CPY_Cont


CS_MENU_EXEC_SEQ_CPY_D
	;; copy 2*16 bytes
	lfsr	FSR1, CS_COPY_PASTE_BUFFER_BEGIN
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_D_SEQUENCES
	movf	CS_MENU_WT_SEQ, W
	mullw	0x20
	movf	PRODL, W
	addwf	FSR0L, F

	clrf	CS_MENU_PARAMETER_IX	; using *_IX as loop counter
CS_MENU_EXEC_SEQ_CPY_D_Loop
	movff	POSTINC0, POSTINC1
	incf	CS_MENU_PARAMETER_IX, F
	BRA_IFCLR CS_MENU_PARAMETER_IX, 5, ACCESS, CS_MENU_EXEC_SEQ_CPY_D_Loop

	;; 	rgoto	CS_MENU_EXEC_SEQ_CPY_Cont

CS_MENU_EXEC_SEQ_CPY_Cont

	;; print message
	TABLE_ADDR TEXT_SEQ_CPY_OK
	rgoto	CS_MENU_EXEC_CpyPstClr_Msg
	
	return

; ==========================================================================
;  Sequencer Paste Function
; ==========================================================================
TEXT_SEQ_PST_EMPTY STRING 16, 0x40, "- Buffer Empty -"
TEXT_SEQ_PST_OK    STRING 16, 0x40, "---- PASTE ---- "

CS_MENU_EXEC_SEQ_PST
	;; print message if copy buffer not filled
	BRA_IFCLR CS_STAT2, CS_STAT2_COPY_BUFFER_FILLED, ACCESS, CS_MENU_EXEC_SEQ_PST_Empty

	;; branch depending on bassline/drum engine
	;; function only used by these engines, therefore this can be simplified:
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x02
	bz	CS_MENU_EXEC_SEQ_PST_D

CS_MENU_EXEC_SEQ_PST_B
	;; paste 2*16 bytes
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES
	swapf	CS_MENU_WT_SEQ, W
	andlw	0x70
	iorwf	FSR0L, F

	clrf	CS_MENU_PARAMETER_IX	; using *_IX as loop counter
CS_MENU_EXEC_SEQ_PST_B_Loop
	lfsr	FSR1, CS_COPY_PASTE_BUFFER_BEGIN + 0x00
	movf	CS_MENU_PARAMETER_IX, W
	movff	PLUSW1, INDF0
	call	CS_MENU_MS_SendParameter

	movlw	0x80		; switch to parameter bank
	addwf	FSR0L, F

	lfsr	FSR1, CS_COPY_PASTE_BUFFER_BEGIN + 0x10
	movf	CS_MENU_PARAMETER_IX, W
	movff	PLUSW1, INDF0
	call	CS_MENU_MS_SendParameter

	movlw	-0x80+1		; switch back to note bank and increment
	addwf	FSR0L, F

	incf	CS_MENU_PARAMETER_IX, F
	BRA_IFCLR CS_MENU_PARAMETER_IX, 4, ACCESS, CS_MENU_EXEC_SEQ_PST_B_Loop

	rgoto	CS_MENU_EXEC_SEQ_PST_Cont


CS_MENU_EXEC_SEQ_PST_D
	;; paste 2*16 bytes
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_D_SEQUENCES
	movf	CS_MENU_WT_SEQ, W
	mullw	0x20
	movf	PRODL, W
	addwf	FSR0L, F

	clrf	CS_MENU_PARAMETER_IX	; using *_IX as loop counter
CS_MENU_EXEC_SEQ_PST_D_Loop
	lfsr	FSR1, CS_COPY_PASTE_BUFFER_BEGIN
	movf	CS_MENU_PARAMETER_IX, W
	movff	PLUSW1, INDF0
	call	CS_MENU_MS_SendParameter
	movf	POSTINC0, W	; (increment)
	incf	CS_MENU_PARAMETER_IX, F
	BRA_IFCLR CS_MENU_PARAMETER_IX, 5, ACCESS, CS_MENU_EXEC_SEQ_PST_D_Loop

	;; 	rgoto	CS_MENU_EXEC_SEQ_PST_Cont

CS_MENU_EXEC_SEQ_PST_Cont
	;; print message
	TABLE_ADDR TEXT_SEQ_PST_OK
	rgoto	CS_MENU_EXEC_CpyPstClr_Msg
		

CS_MENU_EXEC_SEQ_PST_Empty
	TABLE_ADDR TEXT_SEQ_PST_EMPTY
	rgoto	CS_MENU_EXEC_CpyPstClr_Msg


; ==========================================================================
;  Sequencer Clear Function
; ==========================================================================
TEXT_SEQ_CLR_OK    STRING 16, 0x40, "---- Clear ---- "

CS_MENU_EXEC_SEQ_CLR
	;; branch depending on bassline/drum engine
	;; function only used by these engines, therefore this can be simplified:
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x02
	bz	CS_MENU_EXEC_SEQ_CLR_D

CS_MENU_EXEC_SEQ_CLR_B
	;; clear 2*16 bytes
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES
	swapf	CS_MENU_WT_SEQ, W
	andlw	0x70
	iorwf	FSR0L, F

	clrf	CS_MENU_PARAMETER_IX	; using *_IX as loop counter
CS_MENU_EXEC_SEQ_CLR_B_Loop
	movlw	0x80
	movwf	INDF0
	call	CS_MENU_MS_SendParameter

	movlw	0x80		; switch to parameter bank
	addwf	FSR0L, F

	movlw	0x40
	movwf	INDF0
	call	CS_MENU_MS_SendParameter

	movlw	-0x80+1		; switch back to note bank and increment
	addwf	FSR0L, F

	incf	CS_MENU_PARAMETER_IX, F
	BRA_IFCLR CS_MENU_PARAMETER_IX, 4, ACCESS, CS_MENU_EXEC_SEQ_CLR_B_Loop

	rgoto	CS_MENU_EXEC_SEQ_CLR_Cont


CS_MENU_EXEC_SEQ_CLR_D
	;; clear 2*16 bytes
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_D_SEQUENCES
	movf	CS_MENU_WT_SEQ, W
	mullw	0x20
	movf	PRODL, W
	addwf	FSR0L, F

	clrf	CS_MENU_PARAMETER_IX	; using *_IX as loop counter
CS_MENU_EXEC_SEQ_CLR_D_Loop
	clrf	INDF0
	call	CS_MENU_MS_SendParameter
	movf	POSTINC0, W	; (increment)
	incf	CS_MENU_PARAMETER_IX, F
	BRA_IFCLR CS_MENU_PARAMETER_IX, 5, ACCESS, CS_MENU_EXEC_SEQ_CLR_D_Loop

	;; 	rgoto	CS_MENU_EXEC_SEQ_CLR_Cont

CS_MENU_EXEC_SEQ_CLR_Cont
	;; print message
	TABLE_ADDR TEXT_SEQ_CLR_OK
	rgoto	CS_MENU_EXEC_CpyPstClr_Msg


; ==========================================================================
;  Similar to SelPar, but prints parameter assignment as big screen message
; ==========================================================================
CS_MENU_EXEC_SelPar_Asg
	;; handle parameter selection
	rcall	CS_MENU_EXEC_SelPar

	;; ignore if no menu item
	movf	CS_MENU_ENTRIES, W
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	return

	;; print big message
	goto	CS_MENU_Page_BigMsg_ParName


; ==========================================================================
;  Similar to SelPar, but prints bank as big screen message
; ==========================================================================
CS_MENU_EXEC_SelPar_Bank
	;; handle parameter selection
	rcall	CS_MENU_EXEC_SelPar

	;; ignore if no menu item
	movf	CS_MENU_ENTRIES, W
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	return

	;; print big message
	goto	CS_MENU_Page_BigMsg_Bank


; ==========================================================================
;  Similar to SelPar, but prints patch as big screen message
; ==========================================================================
CS_MENU_EXEC_SelPar_Patch
	;; handle parameter selection
	rcall	CS_MENU_EXEC_SelPar

	;; ignore if no menu item
	movf	CS_MENU_ENTRIES, W
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	return

	;; print big message
	goto	CS_MENU_Page_BigMsg_Patch


; ==========================================================================
;  Use this function to select/deselect a parameter
;  (depending on CS_STAT.CS_STAT_MODIFY_PARAMETER)
; ==========================================================================
CS_MENU_EXEC_SelPar
	;; don't select if no menu item
	movf	CS_MENU_ENTRIES, W
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	return

	;; set the "modify parameter" flag
	bsf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	;; clear counter so that cs_menu_timer.inc counts from zero and the menu entry is marked for a short time
	clrf	CS_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH
	;; request a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ	; (see cs_menu.inc)

	;; copy new max value to CS_MENU_PARAMETER_MAX_[LH]
	call	CS_MENU_Hlp_GetCursorPosEntry
	movlw	CS_MENU_ENTRY_MAX_OFFSET
	call	CS_MENU_Hlp_GetPointer

	;; read and copy
	tblrd*+
	movff	TABLAT, CS_MENU_PARAMETER_MAX_L
	tblrd*+
	movff	TABLAT, CS_MENU_PARAMETER_MAX_H

	;; new max value: set encoder speed
	movlw	0
	rcall	CS_MENU_EncSpeedSet

	;; update parameter value
	call	CS_MENU_EncParameterSet

#if DEFAULT_LCD_LINES >= 4
	;; for bar graph at lower line: get scaled value and preload display counter
	call	CS_MENU_GetScaledParam
	goto	CS_MENU_UpdateMeterBar
#else
	return
#endif


; ==========================================================================
;  Use this function to select/deselect a parameter and to toggle it
;  (usefull for on/off switches)
; ==========================================================================
CS_MENU_EXEC_TogPar
	;; don't toggle if no menu item
	movf	CS_MENU_ENTRIES, W
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	return
	
	;; select parameter
	rcall	CS_MENU_EXEC_SelPar

	;; increment it, wrap at max value (we expect values < 0x100)
	incf	CS_MENU_PARAMETER_L, F
	incf	CS_MENU_PARAMETER_MAX_L, W
	cpfslt	CS_MENU_PARAMETER_L, ACCESS
	clrf CS_MENU_PARAMETER_L

	;; transfer to register
	call	CS_MENU_ParameterUpdate

	;; deselect item
	rgoto	CS_MENU_EXEC_Hlp_Deselect


; ==========================================================================
;  Use this function to trigger the random generator
; ==========================================================================
CS_MENU_EXEC_Rnd
	;; generate depending on cursor position
	movf	CS_MENU_PARAMETER_IX, W
	call	SID_RND_GeneratePart
	;; transfer to SID and exit
	goto	CS_MENU_MS_Send_EditBuffer

; ==========================================================================
;  Use this function to trigger the random generator
; ==========================================================================
CS_MENU_EXEC_DrmRnd
	;; generate depending on current instrument
	movf	CS_MENU_SELECTED_INS, W
	call	SID_RND_GenerateOsc_D_Gen

	;; transfer specific part to SID
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_D_I1_BASE
	movf	CS_MENU_SELECTED_INS, W
	mullw	SID_Ix_D_I2_BASE-SID_Ix_D_I1_BASE
	movf	PRODL, W
	addwf	FSR0L, F
	movlw	10		; 10 bytes to send
	movwf	TMP1
CS_MENU_EXEC_DrmRnd_SendLoop
	call	CS_MENU_MS_SendParameter
	movf	POSTINC0, W	; (increment)
	decfsz	TMP1, F
	rgoto	CS_MENU_EXEC_DrmRnd_SendLoop	
	return

	
; ==========================================================================
;  Use this function to save the current patch to CS_MENU_SAVE_PATCH
; ==========================================================================
CS_MENU_EXEC_SavePatch
	;; check if saving allowed
	movff	CS_MENU_SAVE_PATCH, SID_PATCH
	movff	CS_MENU_SAVE_BANK, SID_BANK

	;; patch 0, Bank 0 always allowed
	movf	SID_BANK, W
	iorwf	SID_PATCH, W
	bz	CS_MENU_EXEC_SavePatch_Ok
	
	call	SID_PBANK_CheckPatchValid
	bz	CS_MENU_EXEC_SavePatch_Ok
CS_MENU_EXEC_SavePatch_Inv
CS_MENU_EXEC_SaveEns_Inv	; re-used for invalid ensembles
	TABLE_ADDR TEXT_SAVE_PATCH_INV
	rgoto	CS_MENU_EXEC_SavePatch_Msg

CS_MENU_EXEC_SavePatch_Ok

	;; NEW in MBSID V2: go into name editor
	;; patch can be stored from there!
	clrf	CS_MENU_NAME_POS
	;; enable "edit name" mode
	bsf	CS_STAT, CS_STAT_MODIFY_NAME
	;; enter parameter mode
	goto	CS_MENU_EXEC_SelPar


	;; patch editor will jump back to this label
CS_MENU_EXEC_SavePatch_Final
	;; print "save patch" at the second line
	TABLE_ADDR TEXT_SAVE_PATCH_OK
	rcall	CS_MENU_EXEC_SavePatch_Msg
	;; save patch
	call	CS_MENU_SavePatch
	;; reset menu
	bcf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	rgoto	CS_MENU_EXEC_Menu_Save
	
TEXT_SAVE_PATCH_OK STRING 16, 0x40, "-- Save Patch --"
TEXT_SAVE_PATCH_INV STRING 16, 0x40, "- not possible -"


CS_MENU_EXEC_SavePatch_Msg
CS_MENU_EXEC_CpyPstClr_Msg
	call	MIOS_LCD_PrintMessage
	;; fill with spaces
	movlw	40-16
	call	SID_LCD_PrintSpaces
	movlw	64		; short message
	goto	MIOS_LCD_MessageStart


; ==========================================================================
;  Use this function to save the current ensemble
; ==========================================================================
CS_MENU_EXEC_SaveEns
	;; check if saving allowed
	;; ensemble 0 always alowed
	movf	CS_MENU_SAVE_ENS, W
	bz	CS_MENU_EXEC_SaveEns_Ok
	
	call	SID_EBANK_CheckValid
	bz	CS_MENU_EXEC_SaveEns_Ok
	rgoto	CS_MENU_EXEC_SaveEns_Inv	; (ok - reuse!)

CS_MENU_EXEC_SaveEns_Ok
	;; print "save ensemble" at the second line
	TABLE_ADDR TEXT_SAVE_ENS_OK
	rcall	CS_MENU_EXEC_SaveEns_Msg
	;; select new one
	movff	CS_MENU_SAVE_ENS, SID_ENSEMBLE
	;; save ensemble
	call	CS_MENU_SaveEns
	;; reset menu
	rgoto	CS_MENU_EXEC_Menu_SaveEns
	
TEXT_SAVE_ENS_OK STRING 19, 0x40, "-- Save Ensemble --"


CS_MENU_EXEC_SaveEns_Msg
	call	MIOS_LCD_PrintMessage
	;; fill with spaces
	movlw	40-19
	call	SID_LCD_PrintSpaces
	movlw	64		; short message
	goto	MIOS_LCD_MessageStart


; ==========================================================================
;  Enter the "edit sequence" function
; ==========================================================================
CS_MENU_EXEC_EditSeq
	;; clear name position
	clrf	CS_MENU_NAME_POS
	;; enable "edit name" mode, start with gate layer
	bsf	CS_STAT3, CS_STAT3_MODIFY_SEQ
	;; disable rotate function by default
	bcf	CS_STAT3, CS_STAT3_MODIFY_SEQ_ROTATE
	;; enter parameter mode
	goto	CS_MENU_EXEC_SelPar

; ==========================================================================
;  This help function is used to change to another menu if
;  "parameter modify" mode not active
;  if currently a parameter is modified, change to parameter select
;  mode instead
;  IN: new menu ID in WREG
; ==========================================================================
CS_MENU_EXEC_Hlp_ChangeMenu
	BRA_IFCLR CS_STAT, CS_STAT_MODIFY_PARAMETER, ACCESS, CS_MENU_EXEC_Hlp_ChangeMenu_Now

CS_MENU_EXEC_Hlp_Deselect	; used by CS_MENU_EXEC_TogPar and CS_MENU_Select_NameFunc_Save
	;; we are in "modify parameter" mode, clear flag and update screen
	;; the user has to press the exec button once more to leave the menu
	bcf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	bcf	CS_STAT, CS_STAT_MODIFY_NAME
	bcf	CS_STAT3, CS_STAT3_MODIFY_SEQ
	bcf	CS_STAT, CS_STAT_BIG_MSG
	;; request update of display
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; clear counter so that cs_menu_timer.inc counts from zero and the menu entry is marked for a short time
	clrf	CS_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH
	;; set encoder speed
	movlw	0
	rcall	CS_MENU_EncSpeedSet
	;; update parameter
	rcall	CS_MENU_EncParameterSet
	;; update parameter max value and exit
	rgoto	CS_MENU_EncMaxSet

CS_MENU_EXEC_Hlp_ChangeMenu_Now
	movwf	CS_MENU
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; set encoder speed and exit
	movlw	0
	rgoto	CS_MENU_EncSpeedSet


; ==========================================================================
;  directly called from cs_menu_tables.inc
;  Use this function to change to the main page
; ==========================================================================
CS_MENU_EXEC_GoToMain
	movlw	CS_MENU_MAIN
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu

; ==========================================================================
;  directly called from cs_menu.inc and cs_menu_tables.inc
;  Use this function to change to the root menu
; ==========================================================================
CS_MENU_EXEC_GoToRoot
	;; was previousliy located in CS_MENU_Page_Init
	;; now done here, so that the cursor doesn't change when we are within a menu page and LCD initialisation is requested
	btfss	CS_STAT, CS_STAT_MODIFY_PARAMETER
	clrf CS_MENU_CURSOR_POS
	btfss	CS_STAT, CS_STAT_MODIFY_PARAMETER
	clrf CS_MENU_PAGE_OFFSET

	movlw	CS_MENU_ROOT
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu

; ==========================================================================
;  directly called from cs_menu.inc
;  Use this function to change to the ensemble root menu
; ==========================================================================
CS_MENU_EXEC_GoToEnsRoot
	;; was previousliy located in CS_MENU_Page_Init
	;; now done here, so that the cursor doesn't change when we are within a menu page and LCD initialisation is requested
	btfss	CS_STAT, CS_STAT_MODIFY_PARAMETER
	clrf CS_MENU_CURSOR_POS
	btfss	CS_STAT, CS_STAT_MODIFY_PARAMETER
	clrf CS_MENU_PAGE_OFFSET

	movlw	CS_MENU_E_ROOT
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu

