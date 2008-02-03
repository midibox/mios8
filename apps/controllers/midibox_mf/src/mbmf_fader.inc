; $Id$
;
; MIDIbox MF
; Motorfader Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by USER_AIN_NotifyChange in main.inc when a 
;;  fader has been moved. Input:
;;     o Fader number in WREG and MIOS_PARAMETER1
;;     o 7-bit value in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MBMF_FADER_Handler
	;; save current fader number in MBMF_CURRENT_FADER
	SET_BSR MBMF_BASE
	movwf	MBMF_CURRENT_FADER, BANKED

	;; save it also in MBMF_LAST_FADER0
	;; if LAST_FADER0 != CURRENT_FADER, shift value to LAST_FADER1
	xorwf	MBMF_LAST_FADER0, W, BANKED
	skpz
	movff	MBMF_LAST_FADER0, MBMF_LAST_FADER1
	movff	MBMF_CURRENT_FADER, MBMF_LAST_FADER0
	;; notify that last controller move was *not* a button
	bcf	MB_STAT, MB_STAT_LAST_WAS_BUTTON

	;; if in "select group/bank" mode, branch to MBMF_FADER_SetGroupBank
	BRA_IFSET MB_STAT, MB_STAT_SEL_GROUP_AND_BANK, ACCESS, MBMF_FADER_SetGroupBank

	;; if in "morph" mode, branch to MBMF_FADER_Morph
	BRA_IFSET MB_STAT2, MB_STAT2_MORPH_MODE, ACCESS, MBMF_FADER_Morph

	;; if in calibration mode, branch to CS_M_CALI_Handler
	GOTO_IFSET MB_STAT2, MB_STAT2_CALI_MODE, ACCESS, CS_M_CALI_Handler

	;; map encoder to parameter entry
	movf	MBMF_CURRENT_FADER, W, BANKED
	rcall	MBMF_FADER_Map
	movwf	MBMF_CURRENT_FADER, BANKED

	;; for MIDI learn function: check if fader was turned to 00 or 7F
	movf	MIOS_PARAMETER2, W
	rcall	MBMF_FADER_Learn

	;; exit here if we are in learn mode (don't send fader value)
	btfsc	MB_STAT, MB_STAT_LEARN_MODE
	return

	;; if faders have been muted, branch to MBMF_FADER_Muted
	BRA_IFSET MB_STAT2, MB_STAT2_MUTE_FADERS, ACCESS, MBMF_FADER_Muted

#if DEFAULT_ENABLE_MOTORDRIVER
	;; if TS mode == 2: don't send if touch sensor not pressed
	BRA_IFCLR MBMF_CFG0, MBMF_CFG0_TSMODE1, BANKED, MBMF_FADER_Handler_TS_NotM2
	movf	MBMF_CURRENT_FADER, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	MBMF_TS_STATUS, W, BANKED
	skpnz
	return
	SET_BSR	MBMF_BASE
MBMF_FADER_Handler_TS_NotM2
#endif

	;; else branch to common send handler
	movf	MIOS_PARAMETER2, W
	rgoto	MBMF_FADER_Send


;; --------------------------------------------------------------------------
;;  The handler branches to this function when the faders are muted
;; --------------------------------------------------------------------------
MBMF_FADER_Muted
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; and exit
	return


;; --------------------------------------------------------------------------
;;  This function is used to send a fader value
;;  Input:
;;     o fader number in MBMF_CURRENT_FADER
;;     o absolute position in WREG
;; --------------------------------------------------------------------------
MBMF_FADER_Send
	;; store current position in TMP1
	movwf	TMP1

	;; scale value depending on Min/Max entry
	;; calc address to Min/Max entry:
	call	MBMF_ADDR_FaderEntry
	;; select the 3rd byte
	movlw	3-1
	addwf	MB_ADDRL, F

	;; scale value:
	;; copy min value to MBMF_FADER_MIN_VALUE
	;; copy max value to MBMF_FADER_MAX_VALUE
	;; move fader value to WREG
	call	MBMF_BANK_Read
	movff	WREG, MBMF_FADER_MIN_VALUE
	call	MBMF_BANK_Read
	movff	WREG, MBMF_FADER_MAX_VALUE
	movf	TMP1, W
	call	MBMF_FADER_ScaleValue
	
	;; store result in MBMF_FADER_NEW_VALUE
	movff	WREG, MBMF_FADER_NEW_VALUE

	;; get pointer to FADER_LAST_VALUE_xx register
	call	MBMF_FSR_FaderLastValue

	;; store value in MBMF_FADER_LAST_VALUE
	movff	INDF0, MBMF_FADER_LAST_VALUE

	;; copy the new value into this location for the next call
	movff	MBMF_FADER_NEW_VALUE, INDF0

	;; if new value == old value, branch to the end
	SET_BSR	MBMF_BASE
	movf	MBMF_FADER_NEW_VALUE, W, BANKED
	xorwf	MBMF_FADER_LAST_VALUE, W, BANKED
	bz	MBMF_FADER_Send_End

	;; store new value
	call	MBMF_FSR_FaderValue
	movff	MBMF_FADER_NEW_VALUE, INDF0

	;; send MIDI value
	call	MBMF_MIDI_SendFaderEvent

	;; clear request fader pos update
	bcf	MB_STAT2, MB_STAT2_FADER_UPDATE_REQ
	bcf	MB_STAT2, MB_STAT2_FADER_SOFT_UPDATE_REQ

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; reset the cursor of the CS
	call	CS_MENU_ResetCursor

MBMF_FADER_Send_End
	return


;; --------------------------------------------------------------------------
;;  This function checks if the faders should be repositioned to the
;;  current group values
;;  IN:  MB_STAT2, MB_STAT2_FADER_UPDATE_REQ
;;  OUT: -
;; --------------------------------------------------------------------------
MBMF_FADER_CheckUpdate
	;; don't check when faders have been muted
	btfsc	MB_STAT2, MB_STAT2_MUTE_FADERS
	return

	;; update if request flag set
	btfss	MB_STAT2, MB_STAT2_FADER_UPDATE_REQ
	return
	
	movlw	0xff
	rcall	MBMF_FADER_UpdatePos
	bcf	MB_STAT2, MB_STAT2_FADER_UPDATE_REQ
	bcf	MB_STAT2, MB_STAT2_FADER_SOFT_UPDATE_REQ

	return


;; --------------------------------------------------------------------------
;;  This function updates all fader positions
;;  IN:  MBMF_GROUP_OFFSET
;;       WREG == 0x00: move faders to the group/bank/patch value
;;       WREG == 0x01: move faders to the morph values
;;       WREG == 0xff: update depending on predefined value embedded in bank
;;  OUT: repositioned faders
;;  USES: TMP1, TMP3, MBMF_CURRENT_FADER, BSR
;; --------------------------------------------------------------------------
MBMF_FADER_UpdatePos
	;; save mode in TMP3
	movwf	TMP3

	;; process all faders
	SET_BSR	MBMF_BASE
	clrf	MBMF_CURRENT_FADER, BANKED
MBMF_FADER_UpdatePosLoop
	SET_BSR	MBMF_BASE
	;; take bank setting if TMP3 == 0xff, else take content of TMP3
	incf	TMP3, W
	bnz	MBMF_FADER_UpdatePosLoop_Forced
MBMF_FADER_UpdatePosLoop_Predef
	;; check soft update (set on updates via MIDI) - in this case don't reset software touch detection
	;; means: fader won't be repositioned when it's currently moved manually
	BRA_IFSET MB_STAT2, MB_STAT2_FADER_SOFT_UPDATE_REQ, ACCESS, MBMF_FADER_UpdatePosLoop_PredefS
	;; reset touch detection of current fader
	movf	MBMF_CURRENT_FADER, W, BANKED
#if DEFAULT_ENABLE_MOTORDRIVER
	call	MIOS_MF_TouchDetectionReset
#endif
	SET_BSR	MBMF_BASE
MBMF_FADER_UpdatePosLoop_PredefS

	movf	MBMF_CURRENT_FADER, W, BANKED
	rcall	MBMF_FADER_Map
	lfsr	FSR0, MBMF_FADER_VALUES_00
	movf	PLUSW0, W
	rgoto	MBMF_FADER_UpdatePosLoopCont

MBMF_FADER_UpdatePosLoop_Forced
	;; reset touch detection of current fader
	movf	MBMF_CURRENT_FADER, W, BANKED
#if DEFAULT_ENABLE_MOTORDRIVER
	call	MIOS_MF_TouchDetectionReset
#endif
	SET_BSR	MBMF_BASE

	movf	TMP3, W
	bz	MBMF_FADER_UpdatePosLoop_00
	BRA_IFSET WREG, 0, ACCESS, MBMF_FADER_UpdatePosLoop_01
	rgoto	MBMF_FADER_UpdatePosLoopNext


MBMF_FADER_UpdatePosLoop_00	; group/bank/patch position
	movlw	2+1		; (only the first three faders)
	cpfslt	MBMF_CURRENT_FADER, BANKED
	rgoto MBMF_FADER_UpdatePosLoopNext

	BRA_IFSET MBMF_CURRENT_FADER, 0, BANKED, MBMF_FADER_UpdatePosLoop_00B
	BRA_IFSET MBMF_CURRENT_FADER, 1, BANKED, MBMF_FADER_UpdatePosLoop_00P

MBMF_FADER_UpdatePosLoop_00G
	;; group value
	movf	MBMF_GROUP_OFFSET, W, BANKED
	rgoto	MBMF_FADER_UpdatePosLoopCont

MBMF_FADER_UpdatePosLoop_00B
	;; bank value
#if DEFAULT_PATCHMODE == 0
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	swapf	MBMF_SELECTED_BANK, W, BANKED
	andlw	0x70
#else
	rlf	MBMF_SELECTED_BANK, W, BANKED
	andlw	0x7e
#endif
#else
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	movlw	0x00
#else
	swapf	MBMF_SELECTED_BANK, W, BANKED
	andlw	0x70
#endif
#endif
	rgoto	MBMF_FADER_UpdatePosLoopCont

MBMF_FADER_UpdatePosLoop_00P
	movf	MB_PATCH, W
	rgoto	MBMF_FADER_UpdatePosLoopCont


MBMF_FADER_UpdatePosLoop_01	; morph position
	lfsr	FSR0, MBMF_MORPH_VALUES_G0
	movf	MBMF_CURRENT_FADER, W, BANKED
	andlw	0x07
	movf	PLUSW0, W
	rgoto	MBMF_FADER_UpdatePosLoopCont

MBMF_FADER_UpdatePosLoopCont
	rcall	MBMF_FADER_Move
MBMF_FADER_UpdatePosLoopNext
	SET_BSR	MBMF_BASE
	incf	MBMF_CURRENT_FADER, F, BANKED
	movlw	DEFAULT_NUMBER_FADERS-1
	cpfsgt	MBMF_CURRENT_FADER, BANKED
	rgoto MBMF_FADER_UpdatePosLoop

	;; exit
	return


;; --------------------------------------------------------------------------
;;  This function moves a single fader to the specified position
;;  IN:  WREG: position (7-bit value)
;;	 MBMF_CURRENT_FADER: fader number
;;  OUT: repositioned fader
;;  USES: BSR
;; --------------------------------------------------------------------------
MBMF_FADER_Move
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
	movff	MBMF_CURRENT_FADER, WREG
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
MBMF_FADER_Map
	SET_BSR	MBMF_BASE
	addwf	MBMF_GROUP_OFFSET, W, BANKED
	andlw	0x7f		; wrap at 128
	return


;; --------------------------------------------------------------------------
;;  This function is by the fader handler when MB_STAT_SEL_GROUP_AND_BANK is set
;;  Input:
;;     o Fader number in MIOS_PARAMETER1
;;     o absolute value in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MBMF_FADER_SetGroupBank
	SET_BSR	MBMF_BASE

	;; save fader number in TMP1
	movf	MIOS_PARAMETER1, W
	movwf	TMP1

	;; do nothing if fader ID >= 2+1
	movlw	2+1
	cpfslt	MBMF_CURRENT_FADER, BANKED
	return

	;; modify bank or group or patch value
	BRA_IFSET TMP1, 0, ACCESS, MBMF_FADER_SetGroupBank_B
	BRA_IFSET TMP1, 1, ACCESS, MBMF_FADER_SetGroupBank_P

MBMF_FADER_SetGroupBank_G	; modify group
	;; store absolute value & 0x70 -> group offset
	;; FIXME for different DEFAULT_FADER_GROUP_WIDTHs!
	movf	MIOS_PARAMETER2, W
	andlw	0x70
	movff	WREG, MBMF_GROUP_OFFSET
	rgoto	MBMF_FADER_SetGroupBank_End

MBMF_FADER_SetGroupBank_B	; modify bank
#if DEFAULT_PATCHMODE == 0
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	;; store absolute value / 16
	swapf	MIOS_PARAMETER2, W
	andlw	0x07
#else
	;; store absolute value / 2
	rrf	MIOS_PARAMETER2, W
	andlw	0x3f
#endif
#else
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	;; store always 0
	andlw	0x00
#else
	;; store absolute value / 16
	swapf	MIOS_PARAMETER2, W
	andlw	0x07
#endif
#endif
	movff	WREG, MBMF_SELECTED_BANK
	rgoto	MBMF_FADER_SetGroupBank_End

MBMF_FADER_SetGroupBank_P	; modify patch
#if DEFAULT_PATCHMODE == 0
	;; store always 0
	andlw	0x00
#else
	;; store absolute value
	movf	MIOS_PARAMETER2, W
	andlw	0x7f
#endif
	movff	WREG, MBMF_SELECTED_PATCH
	;; 	rgoto	MBMF_FADER_SetGroupBank_End

MBMF_FADER_SetGroupBank_End
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; and exit
	return


;; --------------------------------------------------------------------------
;;  learn mode handler for faders
;;  IN: fader number in WREG
;; --------------------------------------------------------------------------
MBMF_FADER_Learn
	;; if not in learn mode, invalidate variables
	BRA_IFCLR MB_STAT, MB_STAT_LEARN_MODE, ACCESS, MBMF_FADER_Learn_Invalid

	andlw	0x7f
	bnz	MBMF_FADER_Learn_NotTurned_00
MBMF_FADER_Learn_Turned_00
	movff	MBMF_CURRENT_FADER, MBMF_FADER_LAST_TURNED_00
	rgoto	MBMF_FADER_Learn_Check
MBMF_FADER_Learn_NotTurned_00
	xorlw	0x7f
	bnz	MBMF_FADER_Learn_NotTurned_7F
MBMF_FADER_Learn_Turned_7F
	movff	MBMF_CURRENT_FADER, MBMF_FADER_LAST_TURNED_7F
	rgoto	MBMF_FADER_Learn_Check
MBMF_FADER_Learn_NotTurned_7F
	return			; do nothing if neither 00 nor 7F

MBMF_FADER_Learn_Check
	;; when fader numbers are equal, trigger learn function
	SET_BSR	MBMF_BASE
	movf	MBMF_FADER_LAST_TURNED_00, W, BANKED
	cpfseq	MBMF_FADER_LAST_TURNED_7F, BANKED
	return

	;; (fader number already in WREG)
	call	CS_M_LEARN_NotifyFaderEvent
	;; 	rgoto	MBMF_FADER_Learn_Invalid

MBMF_FADER_Learn_Invalid
	;; invalidate variables
	SET_BSR	MBMF_BASE
	setf	MBMF_FADER_LAST_TURNED_00, BANKED
	setf	MBMF_FADER_LAST_TURNED_7F, BANKED
	return


;; --------------------------------------------------------------------------
;;  This function scales a 7bit value depending on a min and max value
;;  If the min value is greater than the max value, they will be
;;  automatically converted to realise a reversed scaling
;;  Input:
;;     o 7bit value in WREG
;;     o min value in MBMF_FADER_MIN_VALUE
;;     o max value in MBMF_FADER_MAX_VALUE
;;  Output:
;;     o scaled value in WREG and MIOS_PARAMETER1
;;  USES: MIOS_PARAMETER1 and MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MBMF_FADER_ScaleValue
	;; save fader value in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	SET_BSR	MBMF_BASE

	;; send min value if min == max
	movf	MBMF_FADER_MIN_VALUE, W, BANKED
	cpfseq	MBMF_FADER_MAX_VALUE, BANKED
	rgoto MBMF_FADER_ScaleValueDo
	movwf	MIOS_PARAMETER1
	rgoto	MBMF_FADER_ScaleValue_End

MBMF_FADER_ScaleValueDo
	;; set MIOS_PARAMETER2[0] if min > max
	bcf	MIOS_PARAMETER2, 0
	movf	MBMF_FADER_MAX_VALUE, W, BANKED
	cpfsgt	MBMF_FADER_MIN_VALUE, BANKED
	rgoto MBMF_FADER_ScaleValue_NoConv
	bsf	MIOS_PARAMETER2, 0
MBMF_FADER_ScaleValue_NoConv

	;; scaled value-1 = ((current value+1) * (max-min+1)) / 128
	;; swap max/min if MIOS_PARAMETER2[0] set

	;; multiply current value * (max-min+1)
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, MBMF_FADER_ScaleValue_Inv
MBMF_FADER_ScaleValue_NoInv
	movf	MBMF_FADER_MIN_VALUE, W, BANKED
	subwf	MBMF_FADER_MAX_VALUE, W, BANKED
	rgoto	MBMF_FADER_ScaleValue_Cont1
MBMF_FADER_ScaleValue_Inv
	movf	MBMF_FADER_MAX_VALUE, W, BANKED
	subwf	MBMF_FADER_MIN_VALUE, W, BANKED
	;; 	rgoto	MBMF_FADER_ScaleValue_Cont1
MBMF_FADER_ScaleValue_Cont1
	addlw	1
	mulwf	MIOS_PARAMETER1, ACCESS	; multiply with current value
	;; divide result by 128 (result >> 7)
	;; good trick: just shift the upper bit of the low byte into the high byte
	rlf	PRODL, W
	rlf	PRODH, W
	andlw	0x7f
	;; add min or max value depending on MIOS_PARAMETER2[0]
	btfss	MIOS_PARAMETER2, 0
	addwf	MBMF_FADER_MIN_VALUE, W, BANKED
	btfsc	MIOS_PARAMETER2, 0
	addwf	MBMF_FADER_MAX_VALUE, W, BANKED
	;; store result in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1
MBMF_FADER_ScaleValue_End
	movf	MIOS_PARAMETER1, W

	;; return immediately if inversion bit not set
	btfss	MIOS_PARAMETER2, 0
	return

	;; else inverse the result
	subwf	MBMF_FADER_MIN_VALUE, W, BANKED
	addwf	MBMF_FADER_MAX_VALUE, W, BANKED
	movwf	MIOS_PARAMETER1
	return



;; --------------------------------------------------------------------------
;;  This function is called when the morphing mode is active
;;  IN: fader number in MBMF_CURRENT_FADER
;;      absolute value in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MBMF_FADER_Morph
	SET_BSR	MBMF_BASE

	;; 9 faders are handled here:
	;; the 8 first faders to morph the group
	;; the 9th fader to morph all groups (ok, it doesn't work with MBMF)

	;; do nothing if fader ID >= 8+1
	movlw	8+1
	cpfslt	MBMF_CURRENT_FADER, BANKED
	return

	;; save position in TMP5
	movff	MIOS_PARAMETER2, TMP5

	;; set group select flags depending on fader
	BRA_IFSET MBMF_CURRENT_FADER, 3, BANKED, MBMF_FADER_Morph_All
MBMF_FADER_Morph_Single
	;; decode the group flag from fader number
	movf	MBMF_CURRENT_FADER, W, BANKED
	call	MIOS_HLP_GetBitORMask
	rgoto	MBMF_FADER_Morph_Cont
MBMF_FADER_Morph_All
	;; select all groups
	movlw	0xff
MBMF_FADER_Morph_Cont
	movwf	MBMF_MORPH_GROUP_SELECT, BANKED

	
	;; copy absolute value (stored in TMP5) to all selected morph groups
	movwf	TMP1				; TMP1 saves the group flags, it will be shifted with every iteration
	lfsr	FSR0, MBMF_MORPH_VALUES_G0	; pointer to the morph values
	clrf	TMP2				; TMP2 notifies if any value has been changed
MBMF_FADER_Morph_AddLoop
	BRA_IFCLR TMP1, 0, ACCESS, MBMF_FADER_Morph_AddLoop_Next

	movff	TMP5, INDF0
	bsf	TMP2, 0

MBMF_FADER_Morph_AddLoop_Next
	rrf	TMP1, F		; shift the select flags to the right
	movf	POSTINC0, W	; increment pointer to group values
	movf	FSR0L, W	; until all 8 groups processed
	andlw	0x07
	bnz	MBMF_FADER_Morph_AddLoop

	;; exit if no value has been changed
	btfss	TMP2, 0
	return

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; now loop through all fader entries and send the selected groups

	;; using MBMF_CURRENT_FADER as loop counter
	SET_BSR	MBMF_BASE
	clrf	MBMF_CURRENT_FADER, BANKED	
MBMF_FADER_Morhp_Loop
	;; check if current group (==MBMF_CURRENT_FADER[6..4]) is selected
	SET_BSR	MBMF_BASE
	swapf	MBMF_CURRENT_FADER, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	MBMF_MORPH_GROUP_SELECT, W, BANKED
	bz	MBMF_FADER_Morhp_LoopNext

MBMF_FADER_Morhp_LoopSend
	;; get morph value of appr. group
	lfsr	FSR0, MBMF_MORPH_VALUES_G0
	swapf	MBMF_CURRENT_FADER, W, BANKED
	andlw	0x07
	movff	PLUSW0, MBMF_MORPH_VALUE
	;; fade fader value between two scenes
	rcall	MBMF_FADER_MorphIt
MBMF_FADER_Morhp_LoopNext
	;; loop until last fader reached
	SET_BSR	MBMF_BASE
	incf	MBMF_CURRENT_FADER, F, BANKED
	BRA_IFCLR MBMF_CURRENT_FADER, 7, BANKED, MBMF_FADER_Morhp_Loop

	;; thats all
	return


;; --------------------------------------------------------------------------
;;  This function is used by the morph handler to fade a single fader between
;;  two scenes
;;  IN: o enc number in MBMF_CURRENT_FADER
;;      o morph value in MBMF_MORPH_VALUE
;;  USES: TMP1
;; --------------------------------------------------------------------------
MBMF_FADER_MorphIt
	;; save current value in TMP1
	call	MBMF_FSR_FaderValue		; get pointer to current value
	movff	INDF0, MBMF_FADER_MIN_VALUE	; save it in MBMF_FADER_MIN_VALUE
	bcf	MBMF_FADER_MIN_VALUE, 7, BANKED	; clear snap bit

	;; save value of target scene in TMP2
#if DEFAULT_PATCHMODE == 0
	movff	MB_BANK, TMP1			; save current bank in TMP1
	movff	MBMF_MORPH_BANK, MB_BANK	; switch to MORPH bank
	call	MBMF_DUMP_GetFaderValue		; get current fader value
	movff	TMP1, MB_BANK			; restore bank number from TMP1
#else
	movff	MB_PATCH, TMP1			; save current patch in TMP1
	movff	MBMF_MORPH_BANK, MB_PATCH	; switch to MORPH patch
	call	MBMF_DUMP_GetFaderValue		; get current fader value
	movff	TMP1, MB_PATCH			; restore patch number from TMP1
#endif

	SET_BSR	MBMF_BASE
	movwf	MBMF_FADER_MAX_VALUE, BANKED	; finally save value in MBMF_FADER_MAX_VALUE
	bcf	MBMF_FADER_MAX_VALUE, 7, BANKED	; clear snap bit

	;; extra: if MAX == MIN and morph value == 0x00 or 0x7f, send MIN/MAX directly
	;; otherwise it will never be sent since the resulting value will never change
	movf	MBMF_FADER_MAX_VALUE, W, BANKED
	cpfseq	MBMF_FADER_MIN_VALUE, BANKED
	rgoto MBMF_FADER_MorphItNormal
MBMF_FADER_MorphItExtra
	;; skip if MBMF_MORPH_VALUE neither 0x7f nor 0x00
	movf	MBMF_MORPH_VALUE, W, BANKED
	bz	MBMF_FADER_MorphItExtraCont
	xorlw	0x7f
	bnz	MBMF_FADER_MorphIt_End
MBMF_FADER_MorphItExtraCont
	;; just send MAX value (it's the same like the min value)
	movff	MBMF_FADER_MAX_VALUE, TMP1
	;; save sent value in "FADER_LAST_VALUE_xx"
	call	MBMF_FSR_FaderLastValue
	movff	TMP1, INDF0
	rgoto	MBMF_FADER_MorphIt_Send

MBMF_FADER_MorphItNormal
	;; use the scale routine which sweeps between a min and max value
	movf	MBMF_MORPH_VALUE, W, BANKED
	rcall	MBMF_FADER_ScaleValue
	;; save result in TMP1
	movwf	TMP1

	;; compare it with last sent value, do nothing if value is equal
	call	MBMF_FSR_FaderLastValue
	movf	INDF0, W
	xorwf	TMP1, W
	bz	MBMF_FADER_MorphIt_End

	;; save new value in register
	movff	TMP1, INDF0
	
MBMF_FADER_MorphIt_Send
	;; send fader value and exit
	call	MBMF_FSR_FaderEvnt0
	movff	INDF0, MIDI_EVNT0
	call	MBMF_FSR_FaderEvnt1
	movff	INDF0, MIDI_EVNT1
	movff	TMP1, MIDI_EVNT_VALUE
	goto	MIDI_EVNT_Send

MBMF_FADER_MorphIt_End
	return
