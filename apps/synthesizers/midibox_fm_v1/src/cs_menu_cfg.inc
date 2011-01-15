; $Id$
;
; MBFM Control Surface CFG menu
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Prints the upper line of the CFG page
;; --------------------------------------------------------------------------
#if CS_SAMMICH_FM
;;
;; reorder/show the four most useful CFG menu items in the 20 characters available.
;; TODO: support scrolling of the CFG menu for 2x20 char displays
;;
CS_MENU_CFG_STR_0 STRING 40, 0x00,   "SyxD PSav DSav ESav Merger CC MJmp View"
#else
CS_MENU_CFG_STR_0 STRING 40, 0x00,   "Merger CC MJmp SyxD PSav DSav ESav View"
#endif

CS_MENU_CFG_PrintUL
	TABLE_ADDR CS_MENU_CFG_STR_0
	goto	MIOS_LCD_PrintString


;; --------------------------------------------------------------------------
;;  Prints the lower line of the CFG page
;; --------------------------------------------------------------------------
CS_MENU_CFG_STR_SYXD_0	db "Ptch"
CS_MENU_CFG_STR_SYXD_1	db "Drum"
CS_MENU_CFG_STR_SYXD_2	db "Ens."

CS_MENU_CFG_STR_VIEW_0	db "Full"
CS_MENU_CFG_STR_VIEW_1	db "Inst"

CS_MENU_CFG_PrintLL
	movlw	0x40
	call	MIOS_LCD_CursorSet

#if CS_SAMMICH_FM
	goto	CS_MENU_CFG_PrintLL_SyxD
#endif

CS_MENU_CFG_PrintLL_Merger
	;; Merger mode
	movlw	0x00
	btfsc	CS_MENU_MODE, CS_MENU_MODE_MERGER
	movlw 0x01
	rcall	CS_MENU_CFG_PrintLL_OnOff

	;; CC mode
	movlw	0x00
	btfsc	CS_MENU_MODE, CS_MENU_MODE_CC
	movlw 0x01
	rcall	CS_MENU_CFG_PrintLL_OnOff

	;; Menu Jump Mode
	movlw	0x01
	btfsc	CS_MENU_MODE, CS_MENU_MODE_MJMP_N
	movlw 0x00
	rcall	CS_MENU_CFG_PrintLL_OnOff

#if CS_SAMMICH_FM
	goto	CS_MENU_CFG_PrintLL_View
#endif
	
CS_MENU_CFG_PrintLL_SyxD	
	;; SysEx dump
	rcall	CS_MENU_CFG_GetSyxType
	mullw	4
	movlw	CS_MENU_CFG_STR_SYXD_0 & 0xff
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	CS_MENU_CFG_STR_SYXD_0 >> 8
	addwfc	PRODH, W
	movwf	TBLPTRH
	movlw	4
	call	MIOS_LCD_PrintPreconfString
	rcall	CS_MENU_CFG_PrintLL_Space

	;; Patch number
	call	CS_MENU_MS_GetInstrPatch
	call	CS_MENU_PrintBnkAndPatch

	;; drumset number
	call	CS_MENU_PrintBnkAndDrumset
	rcall	CS_MENU_CFG_PrintLL_Space

	;; Ensemble number
	call	CS_MENU_PrintBnkAndEnsemble
 	rcall	CS_MENU_CFG_PrintLL_Space

#if CS_SAMMICH_FM
	goto	CS_MENU_CFG_PrintLL_Merger
#endif

	
CS_MENU_CFG_PrintLL_View
	;; display view mode
	TABLE_ADDR CS_MENU_CFG_STR_VIEW_0
	BRA_IFCLR CS_MENU_MODE, CS_MENU_MODE_VIEW, ACCESS, CS_MENU_CFG_PrintLL_View0
CS_MENU_CFG_PrintLL_View1
	TABLE_ADDR CS_MENU_CFG_STR_VIEW_1
CS_MENU_CFG_PrintLL_View0
	movlw	4
	call	MIOS_LCD_PrintPreconfString

	return

	
	;; help functions
CS_MENU_CFG_PrintLL_Space
	movlw	' '
CS_MENU_CFG_PrintLL_Chr
	goto	MIOS_LCD_PrintChar

CS_MENU_CFG_PrintLL_OnOff
	movwf	TMP1

	rcall	CS_MENU_CFG_PrintLL_Space
	movlw	'o'
	rcall	CS_MENU_CFG_PrintLL_Chr
	movlw	'f'
	btfsc	TMP1, 0
	movlw 'n'
	rcall	CS_MENU_CFG_PrintLL_Chr
	movlw	'f'
	btfsc	TMP1, 0
	movlw ' '
	rcall	CS_MENU_CFG_PrintLL_Chr
	rgoto	CS_MENU_CFG_PrintLL_Space


;; --------------------------------------------------------------------------
;;  A soft button has been pressed
;;  IN: button number in WREG
;; --------------------------------------------------------------------------
CS_MENU_CFG_Button
	;; branch depending on button
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
#if CS_SAMMICH_FM
	rgoto	CS_MENU_CFG_Button_SyxD
	rgoto	CS_MENU_CFG_Button_PSav
	rgoto	CS_MENU_CFG_Button_DSav
	rgoto	CS_MENU_CFG_Button_ESav
	rgoto	CS_MENU_CFG_Button_Merg
	rgoto	CS_MENU_CFG_Button_CC
	rgoto	CS_MENU_CFG_Button_MJmp
	rgoto	CS_MENU_CFG_Button_View
#else
	rgoto	CS_MENU_CFG_Button_Merg
	rgoto	CS_MENU_CFG_Button_CC
	rgoto	CS_MENU_CFG_Button_MJmp
	rgoto	CS_MENU_CFG_Button_SyxD
	rgoto	CS_MENU_CFG_Button_PSav
	rgoto	CS_MENU_CFG_Button_DSav
	rgoto	CS_MENU_CFG_Button_ESav
	rgoto	CS_MENU_CFG_Button_View
#endif

CS_MENU_CFG_Button_Merg
	btg	CS_MENU_MODE, CS_MENU_MODE_MERGER
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	goto	CS_MENU_UpdateMergerMode

CS_MENU_CFG_Button_CC
	btg	CS_MENU_MODE, CS_MENU_MODE_CC
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

CS_MENU_CFG_Button_MJmp
	btg	CS_MENU_MODE, CS_MENU_MODE_MJMP_N
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

TEXT_DUMP_SENT	STRING 21, 0x40, "-- SysEx Dump sent --"
CS_MENU_CFG_Button_SyxD
	;; set MBFM_SYSEX_TYPE depending on current context
	rcall	CS_MENU_CFG_GetSyxType

	SET_BSR	MBFM_BASE
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_CFG_Button_SyxD_D
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_CFG_Button_SyxD_E
CS_MENU_CFG_Button_SyxD_I
	call	CS_MENU_MS_GetInstrPatch
	movf	MBFM_CURRENT_INSTRUMENT, W, BANKED
	rgoto	CS_MENU_CFG_Button_SyxD_Cont
CS_MENU_CFG_Button_SyxD_D
	movff	MBFM_DRUMSET, MBFM_PATCH
	movff	MBFM_DBANK, MBFM_BANK
	movlw	0x10
	rgoto	CS_MENU_CFG_Button_SyxD_Cont
CS_MENU_CFG_Button_SyxD_E
	movff	MBFM_ENSEMBLE, MBFM_PATCH
	movff	MBFM_EBANK, MBFM_BANK
	movlw	0x70
	;; 	rgoto	CS_MENU_CFG_Button_SyxD_Cont
CS_MENU_CFG_Button_SyxD_Cont
	movwf	MBFM_SYSEX_TYPE, BANKED

	;; print message and exit
	TABLE_ADDR TEXT_DUMP_SENT
	call	MIOS_LCD_PrintMessage
	movlw	19
	call	CS_MENU_Hlp_PrintSpaces

	;; send sysex dump
	goto	MBFM_SYSEX_SendDump


CS_MENU_CFG_Button_PSav
	call	CS_MENU_MS_GetInstrPatch
	goto	CS_MENU_SaveCurPatch

CS_MENU_CFG_Button_DSav
	goto	CS_MENU_SaveCurDrumset

CS_MENU_CFG_Button_ESav
	goto	CS_MENU_SaveCurEnsemble

CS_MENU_CFG_Button_View
	btg	CS_MENU_MODE, CS_MENU_MODE_VIEW
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
;;  Determines which type of sysex dump should be sent depending on current context
;;  OUT: WREG == 0x00: patch
;;       WREG == 0x01: drumset
;;       WREG == 0x02: ensemble
;; --------------------------------------------------------------------------
CS_MENU_CFG_GetSyxType
	BRA_IFSET CS_MENU, 7, ACCESS, CS_MENU_CFG_GetSyxType_E

	movf	CS_MENU, W
	xorlw	CS_MENU_ENS
	bz	CS_MENU_CFG_GetSyxType_E

	movf	CS_MENU, W
	xorlw	CS_MENU_DRUM
	bz	CS_MENU_CFG_GetSyxType_D

	movf	CS_MENU, W
	xorlw	CS_MENU_USER
	bz	CS_MENU_CFG_GetSyxType_P

	movlw	CS_MENU_BD
	cpfslt	CS_MENU, ACCESS
	rgoto CS_MENU_CFG_GetSyxType_D

CS_MENU_CFG_GetSyxType_P
	retlw	0x00

CS_MENU_CFG_GetSyxType_D
	retlw	0x01

CS_MENU_CFG_GetSyxType_E
	retlw	0x02

