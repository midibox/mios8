; $Id$
;
; SID Patch routines
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  Init Routine for patch parameters
;;  Note that the MIDI events are initialized seperately, this routine will
;;  be called on power-on and after a new MIDI dump has been loaded
;; --------------------------------------------------------------------------
SID_PATCH_Init
	;; disable SID engine
	bsf	SID_STAT, SID_STAT_ENGINE_DISABLE

	;; clear the whole SID SFR memory space
	movlw	((SIDSW_CLEAR_END-SIDSW_CLEAR_BEGIN)+1) & 0xff
	movwf	TMP1
	lfsr	FSR0, SIDSW_CLEAR_BEGIN
SID_PATCH_Init_ClearLoop
	clrf	POSTINC0
	decfsz	TMP1, F
	rgoto	SID_PATCH_Init_ClearLoop

	;; fetch 128 SID parameters from EEPROM
	clrf	TMP1
	clrf	EEADR
SID_PATCH_InitEELoop
	movf	TMP1, W
	call	SID_SYSEX_TABLE_Get; get CC number
	movwf	TMP2		; temporary store number in TMP2

	movff	TMP1, EEADR	; read parameter from current SID bank
	call	SID_BANK_Read
	movwf	MIOS_PARAMETER1	; store value in MIOS_PARAMETER1

	;; branch depending on TMP2[7..4]
	BRA_IFSET TMP2, 7, ACCESS, SID_PATCH_InitEELoopS
SID_PATCH_InitEELoopN
	;; if TMP2[7] == 0, then set CC parameter
	movf	TMP2, W
	call	SID_CCIN_Set
	rgoto	SID_PATCH_InitEELoopC
SID_PATCH_InitEELoopS
	;; if TMP2[7..4] == 9, then set split point
	movf	TMP2, W
	andlw	0xf0
	xorlw	0x90
	bnz	SID_PATCH_InitEELoopC
	movf	TMP2, W
	rcall	SID_PATCH_GetSplitPointer
	movff	MIOS_PARAMETER1, INDF1
SID_PATCH_InitEELoopC

	incf	TMP1, F		; increment CC counter, loop until 0x80
	BRA_IFCLR TMP1, 7, ACCESS, SID_PATCH_InitEELoop

	;; get and store the MIDI channels
	movlw	EEPROM_CFG_BASE & 0xff
	movwf	EEADR
	call	MIOS_EEPROM_Read
	movff	WREG, SID_V3_BASE + SID_Vx_MIDI_CHANNEL
	call	MIOS_EEPROM_Read
	movff	WREG, SID_V2_BASE + SID_Vx_MIDI_CHANNEL
	call	MIOS_EEPROM_Read
	movff	WREG, SID_V1_BASE + SID_Vx_MIDI_CHANNEL
	
	;; get and store the device ID
	call	MIOS_EEPROM_Read	
#if AUTO_DEVICE_ID
	;; if AUTO_ID enabled, derive the SID device ID from the MIOS device ID instead
	call	MIOS_MIDI_DeviceIDGet
#endif
	movwf	SID_MIDI_DEVICE


	;; store new patch number in SID_WT_PATCH
	movff	SID_PATCH, SID_WT_PATCH

	;; force a refresh of all registers
	bsf	SID_STAT, SID_STAT_FORCE_REFRESH

	;; enable SID engine again
	bcf	SID_STAT, SID_STAT_ENGINE_DISABLE

#if CS_ENABLED == 1
	;; cancel note
	movlw	0x00
	call	CS_MENU_MS_CancelNote

	;; now request a display update so that we see the new parameter on screen
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
#endif

	return



;; --------------------------------------------------------------------------
;;  This help routine returns the pointer to SID_Vx_SPLIT_xxx depending
;;  on WREG
;;  OUT: pointer in FSR1
;; --------------------------------------------------------------------------
SID_PATCH_GetSplitPointer
	lfsr	FSR1, SID_V1_BASE + SID_Vx_SPLIT_LOWER
	btfsc	WREG, 0
	lfsr	FSR1, SID_V1_BASE + SID_Vx_SPLIT_UPPER
	
	rrf	WREG, W
	andlw	0x03
	mullw	SID_Vx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR1L, F
	return
