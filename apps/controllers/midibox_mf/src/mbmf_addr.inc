; $Id$
;
; MIDIbox MF
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
;;  This function returns a MB_ADDR[LH] pointer to the fader entry, 
;;  selected by MBMF_CURRENT_FADER
;; --------------------------------------------------------------------------
MBMF_ADDR_FaderEntry
	SET_BSR	MBMF_BASE
	;; low address of fader event
	rlf	MBMF_CURRENT_FADER, W, BANKED
	rlf	WREG, W
	andlw	0xfc
	addlw	(MBMF_Presets_OffsetButtons) & 0xff
	movwf	MB_ADDRL
	;; high address of fader event
	movlw	0x00
	btfsc	MBMF_CURRENT_FADER, 6, BANKED
	movlw 0x01
	movwf	MB_ADDRH
	movlw	((MBMF_Presets_OffsetFaders-MBMF_Presets_FLASH_Lower) >> 8) & 0x07
	addwfc	MB_ADDRH, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a MB_ADDR[LH] pointer to the fader value, 
;;  selected by MBMF_CURRENT_FADER
;; --------------------------------------------------------------------------
MBMF_ADDR_FaderValue
	SET_BSR	MBMF_BASE
	;; low address of fader value
	movf	MBMF_CURRENT_FADER, W, BANKED
	addlw	MBMF_Presets_OffsetFaderValues & 0xff
	movwf	MB_ADDRL
	;; high address of fader value
	movlw	0x06
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns a MB_ADDR[LH] pointer to the button entry, 
;;  selected by MBMF_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MBMF_ADDR_ButtonEntry
	SET_BSR	MBMF_BASE
	;; low address of button event
	rlf	MBMF_CURRENT_BUTTON, W, BANKED
	rlf	WREG, W
	andlw	0xfc
	addlw	(MBMF_Presets_OffsetButtons) & 0xff
	movwf	MB_ADDRL	
	;; high address of button event
	clrf	MB_ADDRH
	movlw	((MBMF_Presets_OffsetButtons-MBMF_Presets_FLASH_Lower) >> 8) & 0x07
	addwfc	MB_ADDRH, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a MB_ADDR[LH] pointer to the button SR value, 
;;  selected by MBMF_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MBMF_ADDR_ButtonValue
	SET_BSR	MBMF_BASE
	;; low address of button value
	rrf	MBMF_CURRENT_BUTTON, W, BANKED
	rrf	WREG, W
	andlw	0x7 << 1
	addlw	MBMF_Presets_OffsetButtonValues & 0xff
	movwf	MB_ADDRL
	;; high address of button value
	movlw	0x06
	movwf	MB_ADDRH
	return


;; --------------------------------------------------------------------------
;;  This function calculates the MB_ADDR[LH] pointer to the label
;;  selected by MBMF_CURRENT_FADER
;; --------------------------------------------------------------------------
MBMF_ADDR_LabelEntry
	movff	MBMF_CURRENT_FADER, WREG
	andlw	0x7f
	mullw	16
	movlw	MBMF_Presets_OffsetLabels & 0xff
	addwf	PRODL, W
	movwf	MB_ADDRL
	movlw	((MBMF_Presets_OffsetLabels-MBMF_Presets_FLASH_Upper) >> 8) + 0x08
	addwfc	PRODH, W
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the global settings page
;; --------------------------------------------------------------------------
MBMF_ADDR_Global
	movlw	MBMF_Presets_OffsetGlobal & 0xff
	movwf	MB_ADDRL
	movlw	0x06
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the bankname
;; --------------------------------------------------------------------------
MBMF_ADDR_Bankname
	movlw	MBMF_Presets_BANKNAME & 0xff
	movwf	MB_ADDRL
	movlw	0x06
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the LED map
;; --------------------------------------------------------------------------
MBMF_ADDR_LED_Map
	movlw	MBMF_Presets_LED_MAP & 0xff
	movwf	MB_ADDRL
	movlw	0x06
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the Magic Numbers
;; --------------------------------------------------------------------------
MBMF_ADDR_MagicNumbers
	movlw	MBMF_Presets_OffsetMagicNumbers & 0xff
	movwf	MB_ADDRL
	movlw	0x06
	movwf	MB_ADDRH
	return



;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the fader section of a patch
;; --------------------------------------------------------------------------
MBMF_ADDR_PATCH_Fader
	SET_BSR	MBMF_BASE
	;; low address of enc value
	movf	MBMF_CURRENT_FADER, W, BANKED
	;; 	addlw	0x00
	movwf	MB_ADDRL
	clrf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the button section of a patch
;;  selected by MBMF_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MBMF_ADDR_PATCH_Button
	SET_BSR	MBMF_BASE
	;; low address of button value
	rrf	MBMF_CURRENT_BUTTON, W, BANKED
	rrf	WREG, W
	andlw	0x7 << 1
	addlw	0x80
	movwf	MB_ADDRL
	clrf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the name of a patch
;; --------------------------------------------------------------------------
MBMF_ADDR_PATCH_Name
	movlw	0x90
	movwf	MB_ADDRL
	clrf	MB_ADDRH
	return

