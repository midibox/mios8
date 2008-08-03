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
;;  IN: current pattern in SEQ_PATTERNx/SEQ_PATTERN_BANKx
;; --------------------------------------------------------------------------
SEQ_DUMP_RestoreAll
	rcall	SEQ_DUMP_RestoreGlobal
	rcall	SEQ_DUMP_RestoreGroups
	rcall	SEQ_DUMP_RestorePots

	return


;; --------------------------------------------------------------------------
;;  This function stores the whole dump data (pot/button values and global settings)
;;  IN: group in SEQ_GROUP
;;      target pattern in SEQ_SAVE_PATTERN
;; --------------------------------------------------------------------------
SEQ_DUMP_StoreAll
 	rcall	SEQ_DUMP_StoreTrk
	rcall	SEQ_DUMP_StorePots
	rgoto	SEQ_DUMP_StoreGlobal

;; --------------------------------------------------------------------------
;;  This function restores the TRK and layer settings of all groups
;;  IN: patterns in SEQ_PATTERNx/SEQ_PATTERN_BANKx
;; --------------------------------------------------------------------------
SEQ_DUMP_RestoreGroups
	SET_BSR	SEQ_BASE

	clrf	SEQ_GROUP, BANKED
SEQ_DUMP_RestoreGroups_Loop
	rcall	SEQ_DUMP_RestoreTrk
	rcall	SEQ_DUMP_RestorePots
	SET_BSR	SEQ_BASE
	incf	SEQ_GROUP, F, BANKED
	BRA_IFCLR SEQ_GROUP, 2, BANKED, SEQ_DUMP_RestoreGroups_Loop

	return


;; --------------------------------------------------------------------------
;;  This function restores the dump data from EEPROM/BankStick
;;  IN: current pattern in SEQ_PATTERNx/SEQ_PATTERN_BANKx, current group in SEQ_GROUP
;; --------------------------------------------------------------------------
SEQ_DUMP_RestoreTrk
	SET_BSR	SEQ_BASE

	;; new track values: request a complete display re-initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; initialize SEQ_PATTERN/SEQ_PATTERN_BANK depending on SEQ_GROUP and SEQ_PATTERNx/SEQ_PATTERN_BANKx
	rcall	SEQ_DUMP_Hlp_Init_SEQ_PATTERN

	;; fetch constant track parameters
	;; see "seq_presets.inc" for the structure
	movlw	LOW(SEQ_PRESETS_CFG_Offset)
	movwf	EEADR
	movlw	HIGH(SEQ_PRESETS_CFG_Offset)
	movwf	EEADRH

	;; determine pointer to SEQ_TRKx depending on group
	rcall	SEQ_DUMP_Hlp_FSR1Grp

	;; load 2 pages (0x80 words, 4*SEQ_TRKRECORD_LENGTH)
	call	SEQ_BANK_ReadPage
	rcall	SEQ_DUMP_Hlp_FSR1Inc40
	call	SEQ_BANK_ReadPage

	return

;; --------------------------------------------------------------------------
;;  This function saves the TRK settings
;;  IN: group in SEQ_GROUP, target pattern in SEQ_SAVE_PATTERN/SEQ_SAVE_PATTERN_BANK
;; --------------------------------------------------------------------------
SEQ_DUMP_StoreTrk
	SET_BSR	SEQ_BASE

	;; set pattern number and bank
	movff	SEQ_SAVE_PATTERN, SEQ_PATTERN
	movff	SEQ_SAVE_PATTERN_BANK, SEQ_PATTERN_BANK

	;; see "seq_presets.inc" for the structure
	movlw	LOW(SEQ_PRESETS_CFG_Offset)
	movwf	EEADR
	movlw	HIGH(SEQ_PRESETS_CFG_Offset)
	movwf	EEADRH

	;; determine pointer to SEQ_TRKx depending on group
	rcall	SEQ_DUMP_Hlp_FSR1Grp

	;; store 2 pages (0x80 words, 4*SEQ_TRKRECORD_LENGTH)
	call	SEQ_BANK_WritePage
	rcall	SEQ_DUMP_Hlp_FSR1Inc40
	call	SEQ_BANK_WritePage

	return

;; --------------------------------------------------------------------------
;;  This function invalidates all layer values, so that they will be re-fetched
;;  into the cache by SEQ_EVNT_GetLayerValue (see comments there)
;;  IN: current group in SEQ_GROUP
;; --------------------------------------------------------------------------
SEQ_DUMP_RestorePots
	;; new pot values: request a complete display re-initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; invalidate the cache of the selected group

	;; Layer A
	movlw	0
	rcall	SEQ_DUMP_Hlp_FSR1LayerOffset
	rcall	SEQ_DUMP_RestorePots_Hlp

	;; Layer B
	movlw	1
	rcall	SEQ_DUMP_Hlp_FSR1LayerOffset
	rcall	SEQ_DUMP_RestorePots_Hlp

	;; Layer C
	movlw	2
	rcall	SEQ_DUMP_Hlp_FSR1LayerOffset
	rcall	SEQ_DUMP_RestorePots_Hlp

	return


	;; invalidate a layer of 4 tracks
SEQ_DUMP_RestorePots_Hlp
	movlw	4 * 32
	movwf	PRODL
SEQ_DUMP_RestorePots_HlpLoop
	movlw	0x80
	movwf	POSTINC1
	decfsz	PRODL, F
	rgoto	SEQ_DUMP_RestorePots_HlpLoop
	return
	
;; --------------------------------------------------------------------------
;;  This function saves the pot values
;;  IN: group in SEQ_GROUP, target pattern in SEQ_SAVE_PATTERN
;; --------------------------------------------------------------------------
SEQ_DUMP_StorePots
	SET_BSR	SEQ_BASE

	;; ---[ fill the whole cache ]---
	rcall	SEQ_DUMP_FillCache

	;; set pattern number and bank
	movff	SEQ_SAVE_PATTERN, SEQ_PATTERN
	movff	SEQ_SAVE_PATTERN_BANK, SEQ_PATTERN_BANK

	;; ---[ store pot values ]---
	movlw	LOW(SEQ_PRESETS_LAYER)
	movwf	EEADR
	movlw	HIGH(SEQ_PRESETS_LAYER)
	movwf	EEADRH

	;; Layer A
	movlw	0
	rcall	SEQ_DUMP_Hlp_FSR1LayerOffset
	call	SEQ_BANK_WritePage	; store 0x80 bytes
	rcall	SEQ_DUMP_Hlp_FSR1Inc40
	call	SEQ_BANK_WritePage

	;; Layer B
	movlw	1
	rcall	SEQ_DUMP_Hlp_FSR1LayerOffset
	call	SEQ_BANK_WritePage	; store 0x80 bytes
	rcall	SEQ_DUMP_Hlp_FSR1Inc40
	call	SEQ_BANK_WritePage

	;; Layer C
	movlw	2
	rcall	SEQ_DUMP_Hlp_FSR1LayerOffset
	call	SEQ_BANK_WritePage	; store 0x80 bytes
	rcall	SEQ_DUMP_Hlp_FSR1Inc40
	call	SEQ_BANK_WritePage

	return


;; --------------------------------------------------------------------------
;;  This function fills the whole cache
;;  This must be done for all tracks before a pattern is saved to avoid
;;  inconsistencies when multiple tracks are using the same pattern)
;; --------------------------------------------------------------------------
SEQ_DUMP_FillCache
	;; Layer A
	movlw	0
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_A
	rcall	SEQ_DUMP_FillCache_Hlp1
	
	;; Layer B
	movlw	1
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_B
	rcall	SEQ_DUMP_FillCache_Hlp1
	
	;; Layer C
	movlw	2
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_C
	;; 	rgoto	SEQ_DUMP_FillCache_Hlp1

	;; refills the cache of a given layer (pointer in FSR1, layer number in WREG)
SEQ_DUMP_FillCache_Hlp1
	clrwdt			; feed watchdog
	movwf	TMP1

	movff	SEQ_PATTERN_BANK0, SEQ_PATTERN_BANK
	movf	SEQ_PATTERN0, W, BANKED
	rcall	SEQ_DUMP_FillCache_Hlp2
	movff	SEQ_PATTERN_BANK1, SEQ_PATTERN_BANK
	movf	SEQ_PATTERN1, W, BANKED
	rcall	SEQ_DUMP_FillCache_Hlp2
	movff	SEQ_PATTERN_BANK2, SEQ_PATTERN_BANK
	movf	SEQ_PATTERN2, W, BANKED
	rcall	SEQ_DUMP_FillCache_Hlp2
	movff	SEQ_PATTERN_BANK3, SEQ_PATTERN_BANK
	movf	SEQ_PATTERN3, W, BANKED
	;; 	rgoto	SEQ_DUMP_FillCache_Hlp2

	;; refills the cache of a given layer group (pointer in FSR1, layer number in TMP1, pattern in WREG, bank in SEQ_PATTERN_BANK)
SEQ_DUMP_FillCache_Hlp2
	;; save pattern and bank number
	andlw	0x7f
	movwf	SEQ_PATTERN, BANKED

	;; determine EEPROM offset (layer number in TMP1)
	movf	TMP1, W
	mullw	0x80
	movff	PRODL, EEADR
	movff	PRODH, EEADRH

	;; fetch all invalid entries
	;; loop counter -> PRODL
	movlw	0x80
	movwf	PRODL	
SEQ_DUMP_FillCache_Hlp2_Loop
	BRA_IFSET INDF1, 7, ACCESS, SEQ_DUMP_FillCache_Hlp2_LoopFtch
	movf	POSTINC1, W	; increment pointer
	incf	EEADR, F	; increment eeprom address
	skpnz
	incf	EEADRH, F
	rgoto	SEQ_DUMP_FillCache_Hlp2_LoopNext
SEQ_DUMP_FillCache_Hlp2_LoopFtch
	call	SEQ_BANK_Read	; read value
	movwf	POSTINC1	; store in cache
SEQ_DUMP_FillCache_Hlp2_LoopNext
	decfsz	PRODL, F
	rgoto	SEQ_DUMP_FillCache_Hlp2_Loop

	return
	
;; --------------------------------------------------------------------------
;;  This function restores and inits the global data from EEPROM
;; --------------------------------------------------------------------------
SEQ_DUMP_RestoreGlobal
	;; see "seq_presets.inc" for the structure
	movlw	LOW(SEQ_PRESETS_GLOBALS_Offset)
	movwf	EEADR
	movlw	HIGH(SEQ_PRESETS_GLOBALS_Offset)
	movwf	EEADRH

	;; ---[ restore & init box configuration ]---
	call	MIOS_EEPROM_Read
	SET_BSR	SEQ_BASE
	movwf	SEQ_CFG0, BANKED

	movlw	MIOS_MIDI_MERGER_DISABLED
	btfsc	SEQ_CFG0, SEQ_CFG0_MERGER, BANKED
	movlw MIOS_MIDI_MERGER_ENABLED
	call	MIOS_MIDI_MergerSet
	
	;; ---[ restore & init BPM ]---
	call	MIOS_EEPROM_Read
	call	SEQ_BPM_Set

	;; ---[ restore dividers ]---
	call	MIOS_EEPROM_Read
	movff	WREG, SEQ_CORE_INT_CLK_DIVIDER_PRELOAD
	call	MIOS_EEPROM_Read
	movff	WREG, SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD

	;; ---[ restore sequencer channel and split point ]---
	call	MIOS_EEPROM_Read
	movff	WREG, SEQ_MIDI_CHANNEL
	call	MIOS_EEPROM_Read
	movff	WREG, SEQ_MIDI_SPLIT_NOTE

	;; ---[ restore morph channel/CC ]---
	call	MIOS_EEPROM_Read
	movff	WREG, SEQ_MORPH_CHANNEL
	call	MIOS_EEPROM_Read
	movff	WREG, SEQ_MORPH_CC

	;; ---[ restore metronome channel byte ]---
	call	MIOS_EEPROM_Read
	movff	WREG, SEQ_METRONOME_CHANNEL

	;; ---[ dummy ]---
	call	MIOS_EEPROM_Read

	;; ---[ restore metronome measure/beat note ]---
	call	MIOS_EEPROM_Read
	movff	WREG, SEQ_METRONOME_NOTE_MEASURE
	call	MIOS_EEPROM_Read
	movff	WREG, SEQ_METRONOME_NOTE_BEAT

	;; ---[ restore scale ]---
	call	MIOS_EEPROM_Read
	movff	WREG, SEQ_GLOBAL_SCALE
	call	MIOS_EEPROM_Read
	movff	WREG, SEQ_GLOBAL_SCALE_CTRL

	;; ---[ restore MIDI router settings ]---
	call	MIOS_EEPROM_Read
	movff	WREG, SEQ_ROUTER_MCLK_ENABLED
	incf	EEADR, F	; dummy increment

	lfsr	FSR0, ROUTER_IN0_SRC0_CHN
	lfsr	FSR2, ROUTER_IN0_DST0_CHN
	movlw	8		; restore IN0
	movwf	TMP1
SEQ_DUMP_RestoreGlobalRLoop1
	call	MIOS_EEPROM_Read
	movwf	POSTINC0
	call	MIOS_EEPROM_Read
	movwf	POSTINC2
	decfsz	TMP1, F
	rgoto	SEQ_DUMP_RestoreGlobalRLoop1

	lfsr	FSR0, ROUTER_IN1_SRC0_CHN
	lfsr	FSR2, ROUTER_IN1_DST0_CHN
	movlw	8		; restore IN1
	movwf	TMP1
SEQ_DUMP_RestoreGlobalRLoop2
	call	MIOS_EEPROM_Read
	movwf	POSTINC0
	call	MIOS_EEPROM_Read
	movwf	POSTINC2
	decfsz	TMP1, F
	rgoto	SEQ_DUMP_RestoreGlobalRLoop2

	;; ---[ restore steps per measure value ]---
	call	MIOS_EEPROM_Read
	movff	WREG, SEQ_STEPS_PER_MEASURE

	SET_BSR	SEQ_BASE
	return
	
;; --------------------------------------------------------------------------
;;  This function saves the global settings in EEPROM
;; --------------------------------------------------------------------------
SEQ_DUMP_StoreGlobal
	;; see "seq_presets.inc" for the structure
	movlw	LOW(SEQ_PRESETS_GLOBALS_Offset)
	movwf	EEADR
	movlw	HIGH(SEQ_PRESETS_GLOBALS_Offset)
	movwf	EEADRH

	;; ---[ store box configuration ]---
	movff	SEQ_CFG0, WREG
	call	MIOS_EEPROM_Write
	
	;; ---[ store BPM ]---
	call	SEQ_BPM_Get
	call	MIOS_EEPROM_Write

	;; ---[ store dividers ]---
	movff	SEQ_CORE_INT_CLK_DIVIDER_PRELOAD, WREG
	call	MIOS_EEPROM_Write
	movff	SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD, WREG
	call	MIOS_EEPROM_Write

	;; ---[ store sequencer channel and split point ]---
	movff	SEQ_MIDI_CHANNEL, WREG
	call	MIOS_EEPROM_Write
	movff	SEQ_MIDI_SPLIT_NOTE, WREG
	call	MIOS_EEPROM_Write

	;; ---[ store morph channel/CC ]---
	movff	SEQ_MORPH_CHANNEL, WREG
	call	MIOS_EEPROM_Write
	movff	SEQ_MORPH_CC, WREG
	call	MIOS_EEPROM_Write

	;; ---[ store metronome channel ]---
	movff	SEQ_METRONOME_CHANNEL, WREG
	call	MIOS_EEPROM_Write

	;; ---[ dummy ]---
	movlw	0x00
	call	MIOS_EEPROM_Write

	;; ---[ store metronome measure/beat note ]---
	movff	SEQ_METRONOME_NOTE_MEASURE, WREG
	call	MIOS_EEPROM_Write
	movff	SEQ_METRONOME_NOTE_BEAT, WREG
	call	MIOS_EEPROM_Write

	;; ---[ store global scale ]---
	movff	SEQ_GLOBAL_SCALE, WREG
	call	MIOS_EEPROM_Write
	movff	SEQ_GLOBAL_SCALE_CTRL, WREG
	call	MIOS_EEPROM_Write

	;; ---[ store MIDI router settings ]---
	movff	SEQ_ROUTER_MCLK_ENABLED, WREG
	call	MIOS_EEPROM_Write

	;; ---[ dummy ]---
	movlw	0x00
	call	MIOS_EEPROM_Write

	lfsr	FSR0, ROUTER_IN0_SRC0_CHN
	lfsr	FSR2, ROUTER_IN0_DST0_CHN
	movlw	8		; store IN0
	movwf	TMP1
SEQ_DUMP_StoreGlobalRLoop1
	movf	POSTINC0, W
	call	MIOS_EEPROM_Write
	movf	POSTINC2, W
	call	MIOS_EEPROM_Write
	decfsz	TMP1, F
	rgoto	SEQ_DUMP_StoreGlobalRLoop1

	lfsr	FSR0, ROUTER_IN1_SRC0_CHN
	lfsr	FSR2, ROUTER_IN1_DST0_CHN
	movlw	8		; store IN1
	movwf	TMP1
SEQ_DUMP_StoreGlobalRLoop2
	movf	POSTINC0, W
	call	MIOS_EEPROM_Write
	movf	POSTINC2, W
	call	MIOS_EEPROM_Write
	decfsz	TMP1, F
	rgoto	SEQ_DUMP_StoreGlobalRLoop2

	;; ---[ store steps per measure value ]---
	movff	SEQ_STEPS_PER_MEASURE, WREG
	call	MIOS_EEPROM_Write

	SET_BSR	SEQ_BASE
	return

;; --------------------------------------------------------------------------
;;  Help function to get base pointer to group
;; --------------------------------------------------------------------------
SEQ_DUMP_Hlp_FSR1Grp
	lfsr	FSR1, SEQ_TRK0
	movf	SEQ_GROUP, W, BANKED
	mullw	4*SEQ_TRKRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	return

;; --------------------------------------------------------------------------
;;  Help function to increment 0x40 to FSR1
;; --------------------------------------------------------------------------
SEQ_DUMP_Hlp_FSR1Inc40
	movlw	0x40
	addwf	FSR1L, F
	movlw	0
	addwfc	FSR1H, F
	return

;; --------------------------------------------------------------------------
;;  Help function which returns the offset to SEQ_LAYER_VALUES_* depending
;;  on SEQ_GROUP and layer (in WREG)
;; --------------------------------------------------------------------------
SEQ_DUMP_Hlp_FSR1LayerOffset
	SET_BSR	SEQ_BASE

	BRA_IFSET SEQ_GROUP, 1, BANKED, SEQ_DUMP_Hlp_FSR1LayerOffset_G23
SEQ_DUMP_Hlp_FSR1LayerOffset_G01
	BRA_IFSET SEQ_GROUP, 0, BANKED, SEQ_DUMP_Hlp_FSR1LayerOffset_G1
SEQ_DUMP_Hlp_FSR1LayerOffset_G0
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_A
	btfsc	WREG, 0
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_B
	btfsc	WREG, 1
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_C
	return

SEQ_DUMP_Hlp_FSR1LayerOffset_G1
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK4_A
	btfsc	WREG, 0
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK4_B
	btfsc	WREG, 1
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK4_C
	return

SEQ_DUMP_Hlp_FSR1LayerOffset_G23
	BRA_IFSET SEQ_GROUP, 0, BANKED, SEQ_DUMP_Hlp_FSR1LayerOffset_G3
SEQ_DUMP_Hlp_FSR1LayerOffset_G2
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK8_A
	btfsc	WREG, 0
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK8_B
	btfsc	WREG, 1
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK8_C
	return

SEQ_DUMP_Hlp_FSR1LayerOffset_G3
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK12_A
	btfsc	WREG, 0
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK12_B
	btfsc	WREG, 1
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK12_C
	return

;; --------------------------------------------------------------------------
;;  This help function copies SEQ_PATTERNx to SEQ_PATTERN and
;;  SEQ_PATTERN_BANKx to SEQ_PATTERN_BANK depending on SEQ_GROUP
;; --------------------------------------------------------------------------
SEQ_DUMP_Hlp_Init_SEQ_PATTERN
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_PATTERN_BANK0
	movf	SEQ_GROUP, W, BANKED
	movf	PLUSW1, W	; ensure that only 3 bits are used
	andlw	0x07
	movwf	SEQ_PATTERN_BANK, BANKED

	lfsr	FSR1, SEQ_PATTERN0
	movf	SEQ_GROUP, W, BANKED
	movf	PLUSW1, W	; ensure that 7th bit is cleared
	andlw	0x7f
	movwf	SEQ_PATTERN, BANKED
	return
