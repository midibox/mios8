; $Id$
;
; MIDIbox64
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
;;  This function returns a MB_ADDR[LH] pointer to the pot entry, 
;;  selected by MB64_CURRENT_POT
;; --------------------------------------------------------------------------
MB64_ADDR_PotEntry
	SET_BSR	MB64_BASE
	;; low address of pot event
	rlf	MB64_CURRENT_POT, W, BANKED
	rlf	WREG, W
	andlw	0xfc
	addlw	(MB64_Presets_OffsetPots) & 0xff
	movwf	MB_ADDRL
	;; high address of pot event
	clrf	MB_ADDRH
	movlw	((MB64_Presets_OffsetPots-MB64_Presets_FLASH_Lower) >> 8) & 0x01
	addwfc	MB_ADDRH, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a MB_ADDR[LH] pointer to the pot value, 
;;  selected by MB64_CURRENT_POT
;; --------------------------------------------------------------------------
MB64_ADDR_PotValue
	SET_BSR	MB64_BASE
	;; low address of pot value
	movf	MB64_CURRENT_POT, W, BANKED
	addlw	MB64_Presets_OffsetPotValues & 0xff
	movwf	MB_ADDRL
	;; high address of pot value
	movlw	0x02
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns a MB_ADDR[LH] pointer to the button entry, 
;;  selected by MB64_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64_ADDR_ButtonEntry
	SET_BSR	MB64_BASE
	;; low address of button event
	rlf	MB64_CURRENT_BUTTON, W, BANKED
	rlf	WREG, W
	andlw	0xfc
	addlw	(MB64_Presets_OffsetButtons) & 0xff
	movwf	MB_ADDRL	
	;; high address of button event
	clrf	MB_ADDRH
	movlw	((MB64_Presets_OffsetButtons-MB64_Presets_FLASH_Lower) >> 8) & 0x01
	addwfc	MB_ADDRH, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a MB_ADDR[LH] pointer to the button SR value, 
;;  selected by MB64_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64_ADDR_ButtonValue
	SET_BSR	MB64_BASE
	;; low address of button value
	rrf	MB64_CURRENT_BUTTON, W, BANKED
	rrf	WREG, W
	andlw	0x7 << 1
	addlw	MB64_Presets_OffsetButtonValues & 0xff
	movwf	MB_ADDRL
	;; high address of button value
	movlw	0x02
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function calculates the MB_ADDR[LH] pointer to the label
;;  selected by MB64_CURRENT_POT
;; --------------------------------------------------------------------------
MB64_ADDR_LabelEntry
	movff	MB64_CURRENT_POT, WREG
	andlw	0x3f
	mullw	16
	movlw	MB64_Presets_OffsetLabels & 0xff
	addwf	PRODL, W
	movwf	MB_ADDRL
	movlw	((MB64_Presets_OffsetLabels-MB64_Presets_FLASH_Upper) >> 8) + 0x04
	addwfc	PRODH, W
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the global settings page
;; --------------------------------------------------------------------------
MB64_ADDR_Global
	movlw	MB64_Presets_OffsetGlobal & 0xff
	movwf	MB_ADDRL
	movlw	0x02
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the bankname
;; --------------------------------------------------------------------------
MB64_ADDR_Bankname
	movlw	MB64_Presets_BANKNAME & 0xff
	movwf	MB_ADDRL
	movlw	0x02
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the LED map
;; --------------------------------------------------------------------------
MB64_ADDR_LED_Map
	movlw	MB64_Presets_LED_MAP & 0xff
	movwf	MB_ADDRL
	movlw	0x02
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the Magic Numbers
;; --------------------------------------------------------------------------
MB64_ADDR_MagicNumbers
	movlw	MB64_Presets_OffsetMagicNumbers & 0xff
	movwf	MB_ADDRL
	movlw	0x02
	movwf	MB_ADDRH
	return


;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the pot section of a patch
;; --------------------------------------------------------------------------
MB64_ADDR_PATCH_Pot
	SET_BSR	MB64_BASE
	;; low address of pot value
	movf	MB64_CURRENT_POT, W, BANKED
	;; 	addlw	0x00
	movwf	MB_ADDRL
	clrf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the button section of a patch
;;  selected by MB64_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64_ADDR_PATCH_Button
	SET_BSR	MB64_BASE
	;; low address of button value
	rrf	MB64_CURRENT_BUTTON, W, BANKED
	rrf	WREG, W
	andlw	0x7 << 1
	addlw	0x40
	movwf	MB_ADDRL
	clrf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the name of a patch
;; --------------------------------------------------------------------------
MB64_ADDR_PATCH_Name
	movlw	0x50
	movwf	MB_ADDRL
	clrf	MB_ADDRH
	return
