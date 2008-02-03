; $Id$
;
; MB64 Menu PotMode Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Exit routine is called when the exec button has been pressed
;; --------------------------------------------------------------------------
CS_M_POTMODE_Exit
	goto	MB64_DUMP_StoreGlobal; (save CFG0)

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_POTMODE_Snapshot
	goto	MB64_BUTTON_Snapshot

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_POTMODE_Init
	movlw	3
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	SET_BSR	MB64_BASE
	movlw	0x00
	btfsc	MB64_CFG0, MB64_CFG0_SNAP, BANKED
	movlw 0x01
	btfsc	MB64_CFG0, MB64_CFG0_RELATIVE, BANKED
	movlw 0x02
	btfsc	MB64_CFG0, MB64_CFG0_PARALLAX, BANKED
	movlw 0x03
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	TABLE_ADDR CS_M_POTMODE_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; print "Pot Behaviour:  " at first line
	TABLE_ADDR CS_M_POTMODE_STR_0
	goto	MIOS_LCD_PrintString

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_POTMODE_ValueChanged
	;; copy menu value to MB64_CFG0
	SET_BSR	MB64_BASE
	bcf	MB64_CFG0, MB64_CFG0_SNAP, BANKED
	bcf	MB64_CFG0, MB64_CFG0_RELATIVE, BANKED
	bcf	MB64_CFG0, MB64_CFG0_PARALLAX, BANKED

	movf	CS_MENU_PARAMETER_L, W
	bz	CS_M_POTMODE_ValueChanged_End

	bsf	MB64_CFG0, MB64_CFG0_SNAP, BANKED
	decf	WREG, W
	bz	CS_M_POTMODE_ValueChanged_End

	bsf	MB64_CFG0, MB64_CFG0_RELATIVE, BANKED
	decf	WREG, W
	bz	CS_M_POTMODE_ValueChanged_End

	bsf	MB64_CFG0, MB64_CFG0_PARALLAX, BANKED

CS_M_POTMODE_ValueChanged_End
	return

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_POTMODE_Tick
	;; print pot mode at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_POTMODE_NAME_TABLE	; initial pointer to table
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x03
	mullw	CS_M_POTMODE_NAME_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_POTMODE_NAME_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; print arrows and exit
	goto	CS_MENU_PrintArrows


;; ==========================================================================

CS_M_POTMODE_STR_0 STRING 16, 0x00, "Pot Behaviour:  "

CS_M_POTMODE_NAME_LEN		EQU	14	; 14 characters per entry
CS_M_POTMODE_NAME_ENTRIES	EQU	4	; 4 entries
CS_M_POTMODE_NAME_TABLE				; all entries are listed here
	db	"Normal        "
	db	"Snap          "
	db	"Relative      "
	db	"Parallax      "
