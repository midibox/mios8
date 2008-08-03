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
	rcall	SEQ_DUMP_RestoreTrks

	return


;; --------------------------------------------------------------------------
;;  This function stores the whole dump data (pot/button values and global settings)
;;  IN: group in SEQ_GROUP
;;      target pattern in SEQ_SAVE_PATTERN/SEQ_SAVE_PATTERN_BANK
;; --------------------------------------------------------------------------
SEQ_DUMP_StoreAll
	rcall	SEQ_DUMP_StoreGlobal
 	rgoto	SEQ_DUMP_StoreTrks

;; --------------------------------------------------------------------------
;;  This function restores the pattern from EEPROM/BankStick
;;  IN: current pattern in SEQ_PATTERNx/SEQ_PATTERN_BANKx
;; --------------------------------------------------------------------------
SEQ_DUMP_RestoreTrks
	SET_BSR	SEQ_BASE

	;; set pattern number and bank
	;; MB808 has only a single group with 16 tracks
	movff	SEQ_PATTERN_BANK0, SEQ_PATTERN_BANK
	movff	SEQ_PATTERN0, SEQ_PATTERN

	;; load 8 pages (0x200 bytes)
	clrf	EEADR
	clrf	EEADRH
	lfsr	FSR1, SEQ_PATTERN_BUFFER
SEQ_DUMP_RestoreTrksLoop
	call	SEQ_BANK_ReadPage
	movlw	0x40
	addwf	FSR1L, F
	movlw	0x00
	addwfc	FSR1H, F
	BRA_IFCLR EEADRH, 1, ACCESS, SEQ_DUMP_RestoreTrksLoop

	;; new track values: request a complete display re-initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return

;; --------------------------------------------------------------------------
;;  This function saves the TRK settings
;;  IN: target pattern in SEQ_SAVE_PATTERN/SEQ_SAVE_PATTERN_BANK
;; --------------------------------------------------------------------------
SEQ_DUMP_StoreTrks
	SET_BSR	SEQ_BASE

	;; set pattern number and bank
	movff	SEQ_SAVE_PATTERN, SEQ_PATTERN
	movff	SEQ_SAVE_PATTERN_BANK, SEQ_PATTERN_BANK

	;; BankStick pages can be stored in background
	movf	SEQ_SAVE_PATTERN, W, BANKED
	bnz	SEQ_DUMP_StoreTrks_BS
SEQ_DUMP_StoreTrks_Internal
	;; store 8 pages (0x200 bytes)
	clrf	EEADR
	clrf	EEADRH
	lfsr	FSR1, SEQ_PATTERN_BUFFER
SEQ_DUMP_StoreTrksLoop
	call	SEQ_BANK_WritePage
	movlw	0x40
	addwf	FSR1L, F
	movlw	0x00
	addwfc	FSR1H, F
	BRA_IFCLR EEADRH, 1, ACCESS, SEQ_DUMP_StoreTrksLoop
	return

SEQ_DUMP_StoreTrks_BS
	;; start non-blocking write operation
	goto	SEQ_BANK_WritePattern_NB


;; --------------------------------------------------------------------------
;;  This function restores a chain from BankStick
;;  IN: current pattern in SEQ_SONG
;; --------------------------------------------------------------------------
SEQ_DUMP_RestoreSong
	SET_BSR	SEQ_BASE

	;; ensure that all chain variables are cleared
	call	SEQ_CHAIN_Clr

	;; load 4 pages (0x100 bytes)
	clrf	EEADR
	lfsr	FSR1, SEQ_PATTERN_CHAIN
SEQ_DUMP_RestoreSongLoop
	call	SEQ_SBANK_ReadPage
	movlw	0x40
	addwf	FSR1L, F
	movlw	0x00
	addwfc	FSR1H, F
	movf	EEADR, W
	bnz	SEQ_DUMP_RestoreSongLoop

	;; find SEQ_CHAIN_TOP
	;; (all allocated entries are marked at bit #7 of bank byte
	SET_BSR	SEQ_BASE
	clrf	SEQ_CHAIN_TOP, BANKED
	lfsr	FSR1, SEQ_PATTERN_CHAIN
SEQ_DUMP_RestoreSongFindTopLoop
	BRA_IFCLR POSTINC1, 7, ACCESS, SEQ_DUMP_RestoreSongFindTopOk
	movf	POSTINC1, W	; (ignore pattern byte of entry)
	incf	SEQ_CHAIN_TOP, F, BANKED	; (maximum 128 entries)
	BRA_IFCLR SEQ_CHAIN_TOP, 7, BANKED, SEQ_DUMP_RestoreSongFindTopLoop
SEQ_DUMP_RestoreSongFindTopOk

	;; notify that change hasn't been synched yet (means: pattern not playing yet)
	bcf	SEQ_STATE, SEQ_STATE_CHAIN_SYNCHED, BANKED
	
	;; new chain: request a complete display re-initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return

;; --------------------------------------------------------------------------
;;  This function saves the Song settings
;;  IN: target song in SEQ_SAVE_SONG
;; --------------------------------------------------------------------------
SEQ_DUMP_StoreSong
	SET_BSR	SEQ_BASE

	movff	SEQ_SAVE_SONG, SEQ_SONG
#if 0
	;; store 4 pages (0x100 bytes)
	clrf	EEADR
	clrf	EEADRH
	lfsr	FSR1, SEQ_PATTERN_CHAIN
SEQ_DUMP_StoreSongLoop
	call	SEQ_SBANK_WritePage
	movlw	0x40
	addwf	FSR1L, F
	movlw	0x00
	addwfc	FSR1H, F
	movf	EEADR, W
	bnz	SEQ_DUMP_StoreSongLoop
#else
	;; start non-blocking write operation
	call	SEQ_SBANK_WriteSong_NB
#endif

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

	;; ---[ restore scale (dummy, not used in MB808 ]---
	call	MIOS_EEPROM_Read
	call	MIOS_EEPROM_Read

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

	;; ---[ store global scale (dummy, not used in MB808) ]---
	movlw	0x00
	call	MIOS_EEPROM_Write
	movlw	0x00
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
