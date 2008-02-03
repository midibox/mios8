; $Id$
;
; MIDIbox64E
; MB_ADDR pointer return functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function returns a MB_ADDR[LH] pointer to the enc entry, 
;;  selected by MB64E_CURRENT_ENTRY
;; --------------------------------------------------------------------------
MB64E_ADDR_EncEntry
MB64E_ADDR_FaderEntry
	SET_BSR	MB64E_BASE
	;; low address of enc event
	rlf	MB64E_CURRENT_ENTRY, W, BANKED
	rlf	WREG, W
	andlw	0xfc
	addlw	(MB64E_Presets_OffsetButtons) & 0xff
	movwf	MB_ADDRL
	;; high address of enc event
	movlw	0x00
	btfsc	MB64E_CURRENT_ENTRY, 6, BANKED
	movlw 0x01
	movwf	MB_ADDRH
	movlw	((MB64E_Presets_OffsetEncs-MB64E_Presets_FLASH_Lower) >> 8) & 0x07
	addwfc	MB_ADDRH, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a MB_ADDR[LH] pointer to the enc mode entry, 
;;  selected by MB64E_CURRENT_ENTRY
;; --------------------------------------------------------------------------
MB64E_ADDR_EncModeEntry
	SET_BSR	MB64E_BASE
	;; low address of enc event
	rlf	MB64E_CURRENT_ENTRY, W, BANKED
	andlw	0xfe
	addlw	(MB64E_Presets_OffsetEncMode) & 0xff
	movwf	MB_ADDRL
	;; high address of enc mode event
	clrf	MB_ADDRH
	movlw	((MB64E_Presets_OffsetEncMode-MB64E_Presets_FLASH_Lower) >> 8) & 0x07
	addwfc	MB_ADDRH, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a MB_ADDR[LH] pointer to the enc value, 
;;  selected by MB64E_CURRENT_ENTRY
;; --------------------------------------------------------------------------
MB64E_ADDR_EncValue
	SET_BSR	MB64E_BASE
	;; low address of enc value
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	addlw	MB64E_Presets_OffsetEncValues & 0xff
	movwf	MB_ADDRL
	;; high address of enc value
	movlw	0x06
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns a MB_ADDR[LH] pointer to the button entry, 
;;  selected by MB64E_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64E_ADDR_ButtonEntry
	SET_BSR	MB64E_BASE
	;; low address of button event
	rlf	MB64E_CURRENT_BUTTON, W, BANKED
	rlf	WREG, W
	andlw	0xfc
	addlw	(MB64E_Presets_OffsetButtons) & 0xff
	movwf	MB_ADDRL	
	;; high address of button event
	clrf	MB_ADDRH
	movlw	((MB64E_Presets_OffsetButtons-MB64E_Presets_FLASH_Lower) >> 8) & 0x07
	addwfc	MB_ADDRH, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a MB_ADDR[LH] pointer to the button SR value, 
;;  selected by MB64E_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64E_ADDR_ButtonValue
	SET_BSR	MB64E_BASE
	;; low address of button value
	rrf	MB64E_CURRENT_BUTTON, W, BANKED
	rrf	WREG, W
	andlw	0x7 << 1
	addlw	MB64E_Presets_OffsetButtonValues & 0xff
	movwf	MB_ADDRL
	;; high address of button value
	movlw	0x06
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function calculates the MB_ADDR[LH] pointer to the label
;;  selected by MB64E_CURRENT_ENTRY
;; --------------------------------------------------------------------------
MB64E_ADDR_LabelEntry
	movff	MB64E_CURRENT_ENTRY, WREG
	andlw	0x7f
	mullw	16
	movf	PRODL, W
	movwf	MB_ADDRL
	movlw	0x08
	addwfc	PRODH, W
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the global settings page
;; --------------------------------------------------------------------------
MB64E_ADDR_Global
	movlw	MB64E_Presets_OffsetGlobal & 0xff
	movwf	MB_ADDRL
	movlw	0x06
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the bankname
;; --------------------------------------------------------------------------
MB64E_ADDR_Bankname
	movlw	MB64E_Presets_BANKNAME & 0xff
	movwf	MB_ADDRL
	movlw	0x06
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the LED map
;; --------------------------------------------------------------------------
MB64E_ADDR_LED_Map
	movlw	MB64E_Presets_LED_MAP & 0xff
	movwf	MB_ADDRL
	movlw	0x06
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the Magic Numbers
;; --------------------------------------------------------------------------
MB64E_ADDR_MagicNumbers
	movlw	MB64E_Presets_OffsetMagicNumbers & 0xff
	movwf	MB_ADDRL
	movlw	0x06
	movwf	MB_ADDRH
	return



;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the enc section of a patch
;; --------------------------------------------------------------------------
MB64E_ADDR_PATCH_Enc
	SET_BSR	MB64E_BASE
	;; low address of enc value
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	;; 	addlw	0x00
	movwf	MB_ADDRL
	clrf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the button section of a patch
;;  selected by MB64E_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64E_ADDR_PATCH_Button
	SET_BSR	MB64E_BASE
	;; low address of button value
	rrf	MB64E_CURRENT_BUTTON, W, BANKED
	rrf	WREG, W
	andlw	0x7 << 1
	movlw	0x80
	addlw	MB_ADDRL
	clrf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the name of a patch
;; --------------------------------------------------------------------------
MB64E_ADDR_PATCH_Name
	movlw	0x90
	movwf	MB_ADDRL
	clrf	MB_ADDRH
	return

