; $Id$
;
; MIDIboxSEQ
; SEQ Dump functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function restores the whole dump data (pot/buttons and global settings)
;;  IN: current pattern in SEQ_PATTERN
;; --------------------------------------------------------------------------
SEQ_DUMP_RestoreAll
	rcall	SEQ_DUMP_RestoreGlobal
	rcall	SEQ_DUMP_RestoreGroups
	rgoto	SEQ_DUMP_RestorePots


;; --------------------------------------------------------------------------
;;  This function stores the whole dump data (pot/button values and global settings)
;;  IN: current pattern in SEQ_PATTERN
;; --------------------------------------------------------------------------
SEQ_DUMP_StoreAll
	rcall	SEQ_DUMP_StoreTrk
	rcall	SEQ_DUMP_StorePots
	rgoto	SEQ_DUMP_StoreGlobal

;; --------------------------------------------------------------------------
;;  Used by the save function to save a partial dump (selected track -> save track)
;;  depending on SEQ_SAVE_MODE
;;  IN: current pattern in SEQ_PATTERN and SEQ_SAVE_MODE
;; --------------------------------------------------------------------------
SEQ_DUMP_StorePartial
	;; if mode == 0, store all tracks
	SET_BSR	SEQ_BASE
	movf	SEQ_SAVE_MODE, W, BANKED
	bz	SEQ_DUMP_StoreAll

	;; else store selected track -> save track (SEQ_SAVE_MODE-1)
	rcall	SEQ_DUMP_StoreTrk_P
	rcall	SEQ_DUMP_StorePots_P
	rgoto	SEQ_DUMP_StoreGlobal

;; --------------------------------------------------------------------------
;;  This function restores the TRK settings of all groups
;;  IN: patterns in SEQ_PATTERNx
;; --------------------------------------------------------------------------
SEQ_DUMP_RestoreGroups
	SET_BSR	SEQ_BASE

	clrf	SEQ_GROUP, BANKED
SEQ_DUMP_RestoreGroups_Loop
	lfsr	FSR0, SEQ_PATTERN0
	movf	SEQ_GROUP, W, BANKED
	movff	PLUSW0, SEQ_PATTERN
	bcf	SEQ_PATTERN, 7	; ensure that 7th bit is cleared
	rcall	SEQ_DUMP_RestoreTrk
	SET_BSR	SEQ_BASE
	incf	SEQ_GROUP, F, BANKED
	BRA_IFCLR SEQ_GROUP, 2, BANKED, SEQ_DUMP_RestoreGroups_Loop

	;; ensure that we are in group0 again!
	clrf	SEQ_GROUP, BANKED
	movff	SEQ_PATTERN0, SEQ_PATTERN

	return


;; --------------------------------------------------------------------------
;;  This function restores the dump data from EEPROM/BankStick
;;  IN: current pattern in SEQ_PATTERN, current group in SEQ_GROUP
;;  NOTE: SEQ_POT_VALUES_A_00-3F will be overwritten by this function, this
;;        means that you have to call SEQ_DUMP_RestorePots after this
;;        function!
;; --------------------------------------------------------------------------
SEQ_DUMP_RestoreTrk
	;; new track values: request a complete display re-initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; see "seq_presets.inc" for the structure
	movlw	SEQ_PRESETS_CFG_Offset & 0xff
	movwf	EEADR

	;; load the whole CFG page at once
	lfsr	FSR1, SEQ_DATA_BUFFER
	call	SEQ_BANK_ReadPage

	;; pattern settings: loop through TRK CC's
	clrf	TMP1
	lfsr	FSR0, SEQ_DATA_BUFFER
SEQ_DUMP_RestoreTrk_Loop
	;; send INDF0[3..0] to CC handler
	movf	INDF0, W
	andlw	0x0f
	movwf	MIOS_PARAMETER1
	movf	TMP1, W
	call	SEQ_CCIN_Set

	;; increment CC
	incf	TMP1, F

	;; send POSTINC0[7..4] to CC handler
	swapf	POSTINC0, W
	andlw	0x0f
	movwf	MIOS_PARAMETER1
	movf	TMP1, W
	call	SEQ_CCIN_Set

	;; increment CC
	incf	TMP1, F

	movlw	SEQ_CC_OFFSET_LAST_SAVED_ENTRY ;; loop until last CC reached
	cpfsgt	TMP1, ACCESS
	rgoto SEQ_DUMP_RestoreTrk_Loop


	;; restore single values which are related to the track settings
	;; currently only morph pattern number
	movlw	SEQ_PRESETS_MORPH_PATTERN & 0xff
	movwf	EEADR
	call	SEQ_BANK_Read
	movwf	TMP1
	lfsr	FSR1, SEQ_MORPH_PATTERN0
	movf	SEQ_GROUP, W
	movff	TMP1, PLUSW1
	
	return

;; --------------------------------------------------------------------------
;;  This function restores the pot data from EEPROM/BankStick
;;  IN: current pattern in SEQ_PATTERN
;; --------------------------------------------------------------------------
SEQ_DUMP_RestorePots
	;; new pot values: request a complete display re-initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; ---[ read pot values ]---
	movlw	SEQ_PRESETS_LAYER_A & 0xff
	movwf	EEADR
	lfsr	FSR1, SEQ_POT_VALUES_A_00
	call	SEQ_BANK_ReadPage
	lfsr	FSR1, SEQ_POT_VALUES_B_00
	call	SEQ_BANK_ReadPage
	lfsr	FSR1, SEQ_POT_VALUES_C_00
	goto	SEQ_BANK_ReadPage
	
;; --------------------------------------------------------------------------
;;  This function saves the TRK settings
;;  IN: current pattern in SEQ_PATTERN, current group in SEQ_GROUP
;; --------------------------------------------------------------------------
SEQ_DUMP_StoreTrk
	;; see "seq_presets.inc" for the structure
	movlw	SEQ_PRESETS_CFG_Offset & 0xff
	movwf	EEADR

	;; pattern settings: loop through TRK CC's
	clrf	TMP1
SEQ_DUMP_StoreTrk_Loop
	;; read first CC and store it in TMP2[3..0]
	movf	TMP1, W
	call	SEQ_CCOUT_Get
	andlw	0x0f
	movwf	TMP2

	;; increment CC
	incf	TMP1, F

	;; read first CC and store it in TMP2[7..4]
	movf	TMP1, W
	call	SEQ_CCOUT_Get
	swapf	WREG, W
	andlw	0xf0
	iorwf	TMP2, F

	;; increment CC
	incf	TMP1, F

	;; write value into EEPROM
	movf	TMP2, W
	call	SEQ_BANK_Write

	movlw	SEQ_CC_OFFSET_LAST_SAVED_ENTRY ;; loop until last CC reached
	cpfsgt	TMP1, ACCESS
	rgoto SEQ_DUMP_StoreTrk_Loop

	;; store single values which are related to the track settings
	;; currently only morph pattern number
	movlw	SEQ_PRESETS_MORPH_PATTERN & 0xff
	movwf	EEADR
	lfsr	FSR1, SEQ_MORPH_PATTERN0
	movf	SEQ_GROUP, W
	movf	PLUSW1, W
	call	SEQ_BANK_Write
	
	return

;; --------------------------------------------------------------------------
;;  This function saves the TRK settings of the selected track
;;  IN: current pattern in SEQ_PATTERN, current group in SEQ_GROUP, selected
;;  track returned by CS_M_HLP_GetSelectedTrk, target track in SEQ_SAVE_MODE-1
;; --------------------------------------------------------------------------
SEQ_DUMP_StoreTrk_P
	;; store number of source track in TMP4
	call	CS_M_HLP_GetSelectedTrk
	movwf	TMP4
	;; store number of target track in TMP5
	SET_BSR	SEQ_BASE
	decf	SEQ_SAVE_MODE, W, BANKED
	andlw	0x03
	movwf	TMP5
	
	;; pattern settings: loop through TRK CC's
	clrf	TMP1
SEQ_DUMP_StoreTrk_P_Loop
	;; select CC depending on source track and save value in TMP2
	movf	TMP1, W
	addwf	TMP4, W
	call	SEQ_CCOUT_Get
	andlw	0x0f
	movwf	TMP2

	;; determine EEPROM address of target track
	rrf	TMP1, W
	andlw	0x3f
	addlw	SEQ_PRESETS_CFG_Offset & 0xff
	movwf	EEADR
	btfsc	TMP5, 1
	incf	EEADR, F
	
	;; read byte and store it in TMP3
	call	SEQ_BANK_Read
	movwf	TMP3

	;; decrement EEADR (for write operation)
	decf	EEADR, F

	;; modify byte depending on target track
	BRA_IFSET TMP5, 0, ACCESS, SEQ_DUMP_StoreTrk_P_H
SEQ_DUMP_StoreTrk_P_L
	movlw	0xf0
	andwf	TMP3, F
	movf	TMP2, W
	rgoto	SEQ_DUMP_StoreTrk_P_C
SEQ_DUMP_StoreTrk_P_H
	movlw	0x0f
	andwf	TMP3, F
	swapf	TMP2, W
SEQ_DUMP_StoreTrk_P_C
	iorwf	TMP3, F

	;; store modified value in EEPROM
	movf	TMP3, W
	call	SEQ_BANK_Write

	;; increment CC by 4
	movlw	0x04
	addwf	TMP1, F

	movlw	SEQ_CC_OFFSET_LAST_SAVED_ENTRY ;; loop until last CC reached
	cpfsgt	TMP1, ACCESS
	rgoto SEQ_DUMP_StoreTrk_P_Loop

	return

;; --------------------------------------------------------------------------
;;  This function saves the pot values (Group 0 only!)
;;  IN: current pattern in SEQ_PATTERN
;; --------------------------------------------------------------------------
SEQ_DUMP_StorePots
	;; ---[ store pot values ]---
	movlw	SEQ_PRESETS_LAYER_A & 0xff
	movwf	EEADR
	lfsr	FSR1, SEQ_POT_VALUES_A_00
	call	SEQ_BANK_WritePage
	lfsr	FSR1, SEQ_POT_VALUES_B_00
	call	SEQ_BANK_WritePage
	lfsr	FSR1, SEQ_POT_VALUES_C_00
	goto	SEQ_BANK_WritePage

;; --------------------------------------------------------------------------
;;  This function saves the pot values of the selected track (Group 0 only!)
;;  IN: current pattern in SEQ_PATTERN, selected track returned by
;;  CS_M_HLP_GetSelectedTrk, target track in SEQ_SAVE_MODE-1
;; --------------------------------------------------------------------------
SEQ_DUMP_StorePots_P
	;; store offset to source track in TMP4
	call	CS_M_HLP_GetSelectedTrk
	swapf	WREG, W
	movwf	TMP4
	;; store offset to target track in TMP5
	SET_BSR	SEQ_BASE
	decf	SEQ_SAVE_MODE, W, BANKED
	andlw	0x03
	swapf	WREG, W
	movwf	TMP5
	
	;; ---[ store selected pot values to target track ]---
	movlw	SEQ_PRESETS_LAYER_A & 0xff
	movwf	EEADR
	lfsr	FSR0, SEQ_POT_VALUES_A_00
	rcall	SEQ_DUMP_StorePots_P_Hlp
	lfsr	FSR0, SEQ_POT_VALUES_B_00
	rcall	SEQ_DUMP_StorePots_P_Hlp
	lfsr	FSR0, SEQ_POT_VALUES_C_00
	;; 	rgoto	SEQ_DUMP_StorePots_P_Hlp

SEQ_DUMP_StorePots_P_Hlp
	;; load stored values into data buffer
	lfsr	FSR1, SEQ_DATA_BUFFER
	call	SEQ_BANK_ReadPage
	;; fix EEADR
	movlw	-0x40
	addwf	EEADR, F
	;; copy selected values (16 steps) into buffer
	clrf	TMP2
SEQ_DUMP_StorePots_P_Hlp_Loop
	movf	TMP4, W		; (source offset)
	addwf	TMP2, W		; (step offset)
	movff	PLUSW0, TMP3
	movf	TMP5, W		; (target offset)
	addwf	TMP2, W		; (step offset)
	movff	TMP3, PLUSW1
	incf	TMP2, F		; loop 16 times
	BRA_IFCLR TMP2, 4, ACCESS, SEQ_DUMP_StorePots_P_Hlp_Loop

	;; store modified values in EEPROM
	goto	SEQ_BANK_WritePage


;; --------------------------------------------------------------------------
;;  This function restores and inits the global data from EEPROM
;; --------------------------------------------------------------------------
SEQ_DUMP_RestoreGlobal
	;; see "seq_presets.inc" for the structure
	movlw	SEQ_PRESETS_GLOBALS_Offset & 0xff
	movwf	EEADR

	;; ---[ read & init box configuration ]---
	call	MIOS_EEPROM_Read
	SET_BSR	SEQ_BASE
	movwf	SEQ_CFG0, BANKED

	movlw	0x00
	btfss	SEQ_CFG0, SEQ_CFG0_MERGEROFF, BANKED
	iorlw 0x01
	btfsc	SEQ_CFG0, SEQ_CFG0_MBLINK, BANKED
	iorlw 0x02
	call	MIOS_MIDI_MergerSet
	
	;; ---[ read & init BPM ]---
	call	MIOS_EEPROM_Read
	call	SEQ_BPM_Set

	;; ---[ read metronome channel byte ]---
	call	MIOS_EEPROM_Read
	SET_BSR	SEQ_BASE
	movwf	SEQ_METRONOME_CHANNEL, BANKED

	;; ---[ read device ID ]---
	call	MIOS_EEPROM_Read
	movwf	MIDI_DEVICE_ID

	return
	
;; --------------------------------------------------------------------------
;;  This function saves the global settings in EEPROM
;; --------------------------------------------------------------------------
SEQ_DUMP_StoreGlobal
	;; see "seq_presets.inc" for the structure
	movlw	SEQ_PRESETS_GLOBALS_Offset & 0xff
	movwf	EEADR

	;; ---[ store box configuration ]---
	SET_BSR	SEQ_BASE
	movf	SEQ_CFG0, W, BANKED
	call	MIOS_EEPROM_Write
	
	;; ---[ store BPM ]---
	call	SEQ_BPM_Get
	call	MIOS_EEPROM_Write

	;; ---[ store metronome channel ]---
	SET_BSR	SEQ_BASE
	movf	SEQ_METRONOME_CHANNEL, W, BANKED
	call	MIOS_EEPROM_Write

	;; ---[ store MIDI device ID ]---
	movf	MIDI_DEVICE_ID, W
	call	MIOS_EEPROM_Write

	return

