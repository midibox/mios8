; $Id$
;
; MIDIbox CV
; Data Dump functions
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function restores the whole SysEx dump
;;  Patch number in CV_PATCH
;; --------------------------------------------------------------------------
CV_DUMP_RestoreAll
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; restore EEPROM 0x00-0x3f -> 0x280-0x2bf
	clrf	EEADR
	lfsr	FSR1, CV_CFG0
	call	CV_BANK_ReadPage

	;; restore EEPROM 0x40-0x7f -> 0x2c0-0x2ff
	movlw	0x40
	addwf	FSR1L, F
	call	CV_BANK_ReadPage

	;; restore EEPROM 0x80-0xbf -> 0x300-0x34f
	movlw	0x40
	addwf	FSR1L, F
	incf	FSR1H, F
	call	CV_BANK_ReadPage

	;; restore EEPROM 0xc0-0xff -> 0x340-0x37f
	movlw	0x40
	addwf	FSR1L, F
	call	CV_BANK_ReadPage

	;; refresh values
	goto	CV_MAP_Refresh


;; --------------------------------------------------------------------------
;;  This function stores the whole SysEx dump in EEPROM or BankStick
;;  Patch number in CV_PATCH
;; --------------------------------------------------------------------------
CV_DUMP_StoreAll
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; store 0x280-0x2bf -> EEPROM 0x00-0x3f
	clrf	EEADR
	lfsr	FSR1, CV_CFG0
	call	CV_BANK_WritePage

	;; store 0x2c0-0x2ff -> EEPROM 0x40-0x7f
	movlw	0x40
	addwf	FSR1L, F
	call	CV_BANK_WritePage

	;; store 0x300-0x33f -> EEPROM 0x80-0xbf
	movlw	0x40
	addwf	FSR1L, F
	incf	FSR1H, F
	call	CV_BANK_WritePage

	;; store 0x340-0x37f -> EEPROM 0xc0-0xff
	movlw	0x40
	addwf	FSR1L, F
	goto	CV_BANK_WritePage
