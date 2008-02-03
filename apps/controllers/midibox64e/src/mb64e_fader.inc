; $Id$
;
; MIDIbox64E
; Motorfader/Pot Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by USER_AIN_NotifyChange in main.asm when a 
;;  fader has been moved. Input:
;;     o Fader number in WREG and MIOS_PARAMETER1
;;     o 7-bit value in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MB64E_FADER_Handler
	;; don't process faders if encoders have been muted
	btfsc	MB_STAT3, MB_STAT3_MUTE_ENCODERS
	return

	;; save current fader number in MB64E_CURRENT_ENTRY
	SET_BSR MB64E_BASE
	movwf	MB64E_CURRENT_ENTRY, BANKED
	;; extra for faders: add 0x40, wrap at 0x80, save value in MB64E_LAST_ENTRY
	addlw	0x40
	andlw	0x7f
	movwf	MB64E_LAST_ENTRY, BANKED
	;; notify that last controller move was *not* a button
	bcf	MB_STAT, MB_STAT_LAST_WAS_BUTTON

	;; if in calibration mode, branch to CS_M_CALI_Handler
	GOTO_IFSET MB_STAT3, MB_STAT3_CALI_MODE, ACCESS, CS_M_CALI_Handler

	;; map encoder to parameter entry
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	rcall	MB64E_FADER_Map
	movwf	MB64E_CURRENT_ENTRY, BANKED
	movwf	MB64E_LAST_ENTRY_MAPPED, BANKED

	;; for MIDI learn function: check if fader was turned to 00 or 7F
	movf	MIOS_PARAMETER2, W
	rcall	MB64E_FADER_Learn

	;; exit here if we are in learn mode (don't send fader value)
	btfsc	MB_STAT, MB_STAT_LEARN_MODE
	return

	;; if faders have been muted, branch to MB64E_FADER_Muted
	BRA_IFSET MB_STAT3, MB_STAT3_MUTE_FADERS, ACCESS, MB64E_FADER_Muted

#if DEFAULT_ENABLE_MOTORDRIVER
	;; if TS mode == 2: don't send if touch sensor not pressed
	BRA_IFCLR MB64E_CFG0, MB64E_CFG0_TSMODE1, BANKED, MB64E_FADER_Handler_TS_NotM2
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	MB64E_TS_STATUS, W, BANKED
	skpnz
	return
	SET_BSR	MB64E_BASE
MB64E_FADER_Handler_TS_NotM2
#endif

	;; else branch to common send handler
	movf	MIOS_PARAMETER2, W
	rgoto	MB64E_FADER_Send


;; --------------------------------------------------------------------------
;;  The handler branches to this function when the faders are muted
;; --------------------------------------------------------------------------
MB64E_FADER_Muted
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; and exit
	return


;; --------------------------------------------------------------------------
;;  This function is used to send a fader value
;;  Input:
;;     o fader number in MB64E_CURRENT_ENTRY
;;     o absolute position in WREG
;; --------------------------------------------------------------------------
MB64E_FADER_Send
	;; store current position in TMP1
	movwf	TMP1

	;; scale value depending on Min/Max entry
	;; calc address to Min/Max entry:
	call	MB64E_ADDR_FaderEntry
	;; select the 3rd byte
	movlw	3-1
	addwf	MB_ADDRL, F

	;; scale value:
	;; copy min value to MB64E_ENTRY_MIN_VALUE
	;; copy max value to MB64E_ENTRY_MAX_VALUE
	;; move fader value to WREG
	call	MB64E_BANK_Read
	movff	WREG, MB64E_ENTRY_MIN_VALUE
	call	MB64E_BANK_Read
	movff	WREG, MB64E_ENTRY_MAX_VALUE
	movf	TMP1, W
	call	MB64E_FADER_ScaleValue

	;; store result in MB64E_FADER_NEW_VALUE
	movff	WREG, MB64E_FADER_NEW_VALUE

	;; get pointer to FADER_VALUE_xx register
	call	MB64E_FSR_FaderValue

	;; store value in MB64E_FADER_VALUE
	movff	INDF0, MB64E_FADER_LAST_VALUE

	;; copy the new value into this location for the next call
	movff	MB64E_FADER_NEW_VALUE, INDF0

	;; if new value == old value, branch to the end
	SET_BSR	MB64E_BASE
	movf	MB64E_FADER_NEW_VALUE, W, BANKED
	xorwf	MB64E_FADER_LAST_VALUE, W, BANKED
	bz	MB64E_FADER_Send_End

	;; store new value
	call	MB64E_FSR_FaderValue
	movff	MB64E_FADER_NEW_VALUE, INDF0

	;; send MIDI value
	call	MB64E_MIDI_SendFaderEvent

	;; clear request fader pos update
	bcf	MB_STAT3, MB_STAT3_FADER_UPDATE_REQ
	bcf	MB_STAT3, MB_STAT3_FADER_SOFT_UPDATE_REQ

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; reset the cursor of the CS
	call	CS_MENU_ResetCursor

MB64E_FADER_Send_End
	return


;; --------------------------------------------------------------------------
;;  This function checks if the faders should be repositioned to the
;;  current group values
;;  IN:  MB_STAT3, MB_STAT3_FADER_UPDATE_REQ
;;  OUT: -
;; --------------------------------------------------------------------------
MB64E_FADER_CheckUpdate
	;; don't check when faders have been muted
	btfsc	MB_STAT3, MB_STAT3_MUTE_FADERS
	return

	;; update if request flag set
	btfss	MB_STAT3, MB_STAT3_FADER_UPDATE_REQ
	return
	
	movlw	0xff
	rcall	MB64E_FADER_UpdatePos
	bcf	MB_STAT3, MB_STAT3_FADER_UPDATE_REQ
	bcf	MB_STAT3, MB_STAT3_FADER_SOFT_UPDATE_REQ

	return


;; --------------------------------------------------------------------------
;;  This function updates all fader positions
;;  IN:  MB64E_GROUP_OFFSET
;;  OUT: repositioned faders
;;  USES: TMP1, TMP3, MB64E_CURRENT_ENTRY, BSR
;; --------------------------------------------------------------------------
MB64E_FADER_UpdatePos
	;; save mode in TMP3
	movwf	TMP3

	;; process all faders
	SET_BSR	MB64E_BASE
	clrf	MB64E_CURRENT_ENTRY, BANKED
MB64E_FADER_UpdatePosLoop
	;; check soft update (set on updates via MIDI) - in this case don't reset software touch detection
	;; means: fader won't be repositioned when it's currently moved manually
	BRA_IFSET MB_STAT3, MB_STAT3_FADER_SOFT_UPDATE_REQ, ACCESS, MB64E_FADER_UpdatePosLoop_Predef
	;; reset touch detection of current fader
	movf	MB64E_CURRENT_ENTRY, W, BANKED
#if DEFAULT_ENABLE_MOTORDRIVER
	call	MIOS_MF_TouchDetectionReset
#endif
	SET_BSR	MB64E_BASE
MB64E_FADER_UpdatePosLoop_Predef

	;; move fader
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	rcall	MB64E_FADER_Map
	lfsr	FSR0, MB64E_ENC_VALUES_00
	movf	PLUSW0, W
	rcall	MB64E_FADER_Move
MB64E_FADER_UpdatePosLoopNext
	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_ENTRY, F, BANKED
	movlw	DEFAULT_NUMBER_AIN-1
	cpfsgt	MB64E_CURRENT_ENTRY, BANKED
	rgoto MB64E_FADER_UpdatePosLoop

	;; exit
	return


;; --------------------------------------------------------------------------
;;  This function moves a single fader to the specified position
;;  IN:  WREG: position (7-bit value)
;;	 MB64E_CURRENT_ENTRY: fader number
;;  OUT: repositioned fader
;;  USES: BSR
;; --------------------------------------------------------------------------
MB64E_FADER_Move
#if DEFAULT_ENABLE_MOTORDRIVER
	;; derive 10-bit value from 7-bit value
	andlw	0x7f
	mullw	8
	movff	PRODL, MIOS_PARAMETER1
	movff	PRODH, MIOS_PARAMETER2

#if DEFAULT_TOUCH_SENSOR_MODE >= 1
	;; if touch sensor mode >= 1, reset touch detection
	call	MIOS_MF_TouchDetectionReset
#endif
	;; and move fader
	movff	MB64E_CURRENT_ENTRY, WREG
	andlw	0x07
	call	MIOS_MF_FaderMove
#endif
	return


;; --------------------------------------------------------------------------
;;  Returns mapped fader entry
;;  IN:  fader number in WREG (0x00-0x3f)
;;  OUT: fader entry in WREG (0x00-0x7f)
;;  USES: BSR, FSR1
;; --------------------------------------------------------------------------
MB64E_FADER_Map
	lfsr	FSR1, MB64E_ENC_MAP_00
	movf	PLUSW1, W

	;; EXTRA FOR MB64E: add 0x40 to the entry, wrap at 0x80
	;; by doing so, the faders are mapped to the upper 64 entries, the
	;; encoders to the lower 64 entries (and visa versa)
	addlw	0x40
	andlw	0x7f

	return

;; --------------------------------------------------------------------------
;;  learn mode handler for faders
;;  IN: fader number in WREG
;; --------------------------------------------------------------------------
MB64E_FADER_Learn
	;; if not in learn mode, invalidate variables
	BRA_IFCLR MB_STAT, MB_STAT_LEARN_MODE, ACCESS, MB64E_FADER_Learn_Invalid

	andlw	0x7f
	bnz	MB64E_FADER_Learn_NotTurned_00
MB64E_FADER_Learn_Turned_00
	movff	MB64E_CURRENT_ENTRY, MB64E_FADER_LAST_TURNED_00
	rgoto	MB64E_FADER_Learn_Check
MB64E_FADER_Learn_NotTurned_00
	xorlw	0x7f
	bnz	MB64E_FADER_Learn_NotTurned_7F
MB64E_FADER_Learn_Turned_7F
	movff	MB64E_CURRENT_ENTRY, MB64E_FADER_LAST_TURNED_7F
	rgoto	MB64E_FADER_Learn_Check
MB64E_FADER_Learn_NotTurned_7F
	return			; do nothing if neither 00 nor 7F

MB64E_FADER_Learn_Check
	;; when fader numbers are equal, trigger learn function
	SET_BSR	MB64E_BASE
	movf	MB64E_FADER_LAST_TURNED_00, W, BANKED
	cpfseq	MB64E_FADER_LAST_TURNED_7F, BANKED
	return

	;; (fader number already in WREG)
	call	CS_M_LEARN_NotifyFaderEvent
	;; 	rgoto	MB64E_FADER_Learn_Invalid

MB64E_FADER_Learn_Invalid
	;; invalidate variables
	SET_BSR	MB64E_BASE
	setf	MB64E_FADER_LAST_TURNED_00, BANKED
	setf	MB64E_FADER_LAST_TURNED_7F, BANKED
	return


;; --------------------------------------------------------------------------
;;  This function scales a 7bit value depending on a min and max value
;;  If the min value is greater than the max value, they will be
;;  automatically converted to realise a reversed scaling
;;  Input:
;;     o 7bit value in WREG
;;     o min value in MB64E_ENTRY_MIN_VALUE
;;     o max value in MB64E_ENTRY_MAX_VALUE
;;  Output:
;;     o scaled value in WREG and MIOS_PARAMETER1
;;  USES: MIOS_PARAMETER1 and MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MB64E_FADER_ScaleValue
	;; save fader value in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	SET_BSR	MB64E_BASE

	;; send min value if min == max
	movf	MB64E_ENTRY_MIN_VALUE, W, BANKED
	cpfseq	MB64E_ENTRY_MAX_VALUE, BANKED
	rgoto MB64E_FADER_ScaleValueDo
	movwf	MIOS_PARAMETER1
	rgoto	MB64E_FADER_ScaleValue_End

MB64E_FADER_ScaleValueDo
	;; set MIOS_PARAMETER2[0] if min > max
	bcf	MIOS_PARAMETER2, 0
	movf	MB64E_ENTRY_MAX_VALUE, W, BANKED
	cpfsgt	MB64E_ENTRY_MIN_VALUE, BANKED
	rgoto MB64E_FADER_ScaleValue_NoConv
	bsf	MIOS_PARAMETER2, 0
MB64E_FADER_ScaleValue_NoConv

	;; scaled value-1 = ((current value+1) * (max-min+1)) / 128
	;; swap max/min if MIOS_PARAMETER2[0] set

	;; multiply current value * (max-min+1)
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, MB64E_FADER_ScaleValue_Inv
MB64E_FADER_ScaleValue_NoInv
	movf	MB64E_ENTRY_MIN_VALUE, W, BANKED
	subwf	MB64E_ENTRY_MAX_VALUE, W, BANKED
	rgoto	MB64E_FADER_ScaleValue_Cont1
MB64E_FADER_ScaleValue_Inv
	movf	MB64E_ENTRY_MAX_VALUE, W, BANKED
	subwf	MB64E_ENTRY_MIN_VALUE, W, BANKED
	;; 	rgoto	MB64E_FADER_ScaleValue_Cont1
MB64E_FADER_ScaleValue_Cont1
	addlw	1
	mulwf	MIOS_PARAMETER1, ACCESS	; multiply with current value
	;; divide result by 128 (result >> 7)
	;; good trick: just shift the upper bit of the low byte into the high byte
	rlf	PRODL, W
	rlf	PRODH, W
	andlw	0x7f
	;; add min or max value depending on MIOS_PARAMETER2[0]
	btfss	MIOS_PARAMETER2, 0
	addwf	MB64E_ENTRY_MIN_VALUE, W, BANKED
	btfsc	MIOS_PARAMETER2, 0
	addwf	MB64E_ENTRY_MAX_VALUE, W, BANKED
	;; store result in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1
MB64E_FADER_ScaleValue_End
	movf	MIOS_PARAMETER1, W

	;; return immediately if inversion bit not set
	btfss	MIOS_PARAMETER2, 0
	return

	;; else inverse the result
	subwf	MB64E_ENTRY_MIN_VALUE, W, BANKED
	addwf	MB64E_ENTRY_MAX_VALUE, W, BANKED
	return


