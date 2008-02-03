; $Id$
;
; MIDIO128
; MIDIO Dump functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function restores the whole dump data (global settings)
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MIDIO_DUMP_RestoreAll
	rgoto	MIDIO_DUMP_RestoreGlobal


;; --------------------------------------------------------------------------
;;  This function stores the whole dump data (global settings)
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MIDIO_DUMP_StoreAll
	rgoto	MIDIO_DUMP_StoreGlobal

;; --------------------------------------------------------------------------
;;  This function restores and inits the global data from EEPROM
;; --------------------------------------------------------------------------
MIDIO_DUMP_RestoreGlobal
	;; see "midio_presets.inc" for the structure

	;; store MIDIO_CFG0_INVERSE_DOUT flag in TMP2.0, we need it later to determine if the
	;; DOUT polarity has been changed
	movlw	0x00
	SET_BSR	MIDIO_BASE
	btfsc	MIDIO_CFG0, MIDIO_CFG0_INVERSE_DOUT, BANKED
	movlw 0x01
	movwf	TMP2
	
	;; use TMP1 to transfer the values
	lfsr	FSR0, TMP1
	
	;; Merger setup
	movlw	MIDIO_Presets_CFG0 & 0xff
	movwf	EEADR
	rcall	MIDIO_DUMP_RestoreGlobal_Hlp	; reads low/high nibble
	movf	TMP1, W
	andlw	0x03
	movff	WREG, MIDIO_CFG0

	;; forward debounce counter to MIOS
	rcall	MIDIO_DUMP_RestoreGlobal_Hlp	; reads low/high nibble
	movf	TMP1, W
	call	MIOS_SRIO_DebounceSet

	;; Alternative Program Change Behaviour
	rcall	MIDIO_DUMP_RestoreGlobal_Hlp	; reads low/high nibble
	SET_BSR	MIDIO_BASE
	movf	TMP1, W
	skpz
	bsf	MIDIO_CFG0, MIDIO_CFG0_ALT_PROGCHNG, BANKED

	;; Forward IO
	rcall	MIDIO_DUMP_RestoreGlobal_Hlp	; reads low/high nibble
	SET_BSR	MIDIO_BASE
	movf	TMP1, W
	skpz
	bsf	MIDIO_CFG0, MIDIO_CFG0_FORWARD_IO, BANKED

	;; Inverse DINs
	rcall	MIDIO_DUMP_RestoreGlobal_Hlp	; reads low/high nibble
	SET_BSR	MIDIO_BASE
	movf	TMP1, W
	skpz
	bsf	MIDIO_CFG0, MIDIO_CFG0_INVERSE_DIN, BANKED

	;; Inverse DOUTs
	rcall	MIDIO_DUMP_RestoreGlobal_Hlp	; reads low/high nibble
	SET_BSR	MIDIO_BASE
	movf	TMP1, W
	skpz
	bsf	MIDIO_CFG0, MIDIO_CFG0_INVERSE_DOUT, BANKED

	;; forward TS sensitivity directly to MIOS
	rcall	MIDIO_DUMP_RestoreGlobal_Hlp
	movf	TMP1, W
	skpnz
	movlw	0xff		; min sensitivity if value == zero
	call	MIOS_SRIO_TS_SensitivitySet

	;; init MIDI merger depending on selected CFG
	SET_BSR	MIDIO_BASE
	movlw	0x00
	btfsc	MIDIO_CFG0, MIDIO_CFG0_MERGER, BANKED
	iorlw 0x01
	btfsc	MIDIO_CFG0, MIDIO_CFG0_MBLINK, BANKED
	iorlw 0x02
	call	MIOS_MIDI_MergerSet

	;; now check if the DOUT polarity has been changed
	movlw	0x00
	SET_BSR	MIDIO_BASE
	btfsc	MIDIO_CFG0, MIDIO_CFG0_INVERSE_DOUT, BANKED
	movlw 0x01
	xorwf	TMP2, W
	skpz
	call	MIDIO_DOUT_Invert	; invert all DOUTs

	;; request update of DIN/DOUT registers
	bsf	MB_STAT, MB_STAT_FETCH_DIN_REQ
	
	return

	;; ---
MIDIO_DUMP_RestoreGlobal_Hlp
	call	MIOS_EEPROM_Read
	andlw	0x0f
	movwf	INDF0
	call	MIOS_EEPROM_Read
	swapf	WREG, W
	andlw	0xf0
	iorwf	INDF0, F
	return

;; --------------------------------------------------------------------------
;;  This function saves the global settings in EEPROM
;; --------------------------------------------------------------------------
MIDIO_DUMP_StoreGlobal
	;; see "MIDIO_presets.inc" for the structure

	;; use TMP1 to transfer the values
	lfsr	FSR0, TMP1

	;; Merger setup
	movlw	MIDIO_Presets_CFG0 & 0xff
	movwf	EEADR
	SET_BSR	MIDIO_BASE
	movf	MIDIO_CFG0, W, BANKED
	andlw	0x03
	movwf	TMP1
	rcall	MIDIO_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	;; get debounce counter from MIOS
	call	MIOS_SRIO_DebounceGet
	movwf	TMP1
	rcall	MIDIO_DUMP_StoreGlobal_Hlp

	;; Alternative Program Change Behaviour
	movlw	0x00
	SET_BSR	MIDIO_BASE
	btfsc	MIDIO_CFG0, MIDIO_CFG0_ALT_PROGCHNG, BANKED
	movlw 0x01
	movwf	TMP1
	rcall	MIDIO_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	;; Forward IO
	movlw	0x00
	SET_BSR	MIDIO_BASE
	btfsc	MIDIO_CFG0, MIDIO_CFG0_FORWARD_IO, BANKED
	movlw 0x01
	movwf	TMP1
	rcall	MIDIO_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	;; Inverse DINs
	movlw	0x00
	SET_BSR	MIDIO_BASE
	btfsc	MIDIO_CFG0, MIDIO_CFG0_INVERSE_DIN, BANKED
	movlw 0x01
	movwf	TMP1
	rcall	MIDIO_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	;; Inverse DOUTs
	movlw	0x00
	SET_BSR	MIDIO_BASE
	btfsc	MIDIO_CFG0, MIDIO_CFG0_INVERSE_DOUT, BANKED
	movlw 0x01
	movwf	TMP1
	rcall	MIDIO_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	;; get TS sensitivity from MIOS
	call	MIOS_SRIO_TS_SensitivityGet
	movwf	TMP1
	lfsr	FSR0, TMP1
	rcall	MIDIO_DUMP_StoreGlobal_Hlp

	return

	;; ---
MIDIO_DUMP_StoreGlobal_Hlp
	movf	INDF0, W
	andlw	0x0f
	call	MIOS_EEPROM_Write
	swapf	INDF0, W
	andlw	0x0f
	goto	MIOS_EEPROM_Write


;; --------------------------------------------------------------------------
;;  This function is used in MIDIO_sysex.inc to transfer the internal settings
;;  from BankStick memory to internal EEPROM after a dump upload
;; --------------------------------------------------------------------------
MIDIO_DUMP_TransferInternals
	;; not relevant for MIDIO128
	return
