; $Id$
;
; MIDIbox64E
; Encoder Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by USER_ENC_NotifyChange when a  encoder has
;;  been moved. Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
MB64E_ENC_Handler
	;; save current enc number in MB64E_CURRENT_ENTRY and MB64E_LAST_ENTRY
	SET_BSR MB64E_BASE
	movwf	MB64E_CURRENT_ENTRY, BANKED
	movwf	MB64E_LAST_ENTRY, BANKED

	;; notify that last controller move was *not* a button
	bcf	MB_STAT, MB_STAT_LAST_WAS_BUTTON

	;; if in "select group/bank" mode, branch to MB64E_ENC_SetGroupBank
	BRA_IFSET MB_STAT, MB_STAT_SEL_GROUP_AND_BANK, ACCESS, MB64E_ENC_SetGroupBank

	;; if in "select parameter" mode, branch to MB64E_ENC_SetMap
	BRA_IFSET MB_STAT, MB_STAT_SEL_PARAMETER, ACCESS, MB64E_ENC_SetMap

	;; if in "morph" mode, branch to MB64E_ENC_Morph
	BRA_IFSET MB_STAT2, MB_STAT2_MORPH_MODE, ACCESS, MB64E_ENC_Morph

	;; special MB64E feature: map encoder to parameter entry
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	rcall	MB64E_ENC_Map
	movwf	MB64E_CURRENT_ENTRY, BANKED
	movwf	MB64E_LAST_ENTRY_MAPPED, BANKED

	;; in MIDI learn mode: branch to CS_M_LEARN_NotifyEncEvent
	GOTO_IFSET MB_STAT, MB_STAT_LEARN_MODE, ACCESS, CS_M_LEARN_NotifyEncEvent

	;; if encoders have been muted, branch to MB64_ENC_Muted
	BRA_IFSET MB_STAT3, MB_STAT3_MUTE_ENCODERS, ACCESS, MB64_ENC_Muted

	;; else branch to common send handler
	movf	MIOS_PARAMETER2, W
	rgoto	MB64E_ENC_Send


;; --------------------------------------------------------------------------
;;  The handler branches to this function when the encoders are muted
;; --------------------------------------------------------------------------
MB64_ENC_Muted
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; and exit
	return


;; --------------------------------------------------------------------------
;;  This function is used to send a enc value
;;  Input:
;;     o Enc number in MB64E_CURRENT_ENTRY
;;     o incrementer in WREG
;; --------------------------------------------------------------------------
MB64E_ENC_Send
	;; store current incrementer value in TMP1
	movwf	TMP1

	;; get encoder mode and store it in MB64E_CURRENT_ENC_MODE
	call	MB64E_ADDR_EncModeEntry
	call	MB64E_BANK_Read
	andlw	0x0f
	SET_BSR	MB64E_BASE
	movwf	MB64E_CURRENT_ENC_MODE, BANKED


	;; if mode != 0 (not ENC_MODE_ABSOLUTE), branch directly to sending routine
	bnz	MB64E_ENC_SendNotAbs

	;; ELSE: on absolute values

	;; get address to Min/Max entry:
	call	MB64E_ADDR_EncEntry
	;; select the 3rd byte
	movlw	3-1
	addwf	MB_ADDRL, F

	;; copy min value to MB64E_ENTRY_MIN_VALUE
	;; copy max value to MB64E_ENTRY_MAX_VALUE
	call	MB64E_BANK_Read
	movff	WREG, MB64E_ENTRY_MIN_VALUE
	call	MB64E_BANK_Read
	movff	WREG, MB64E_ENTRY_MAX_VALUE

	;; now call the MIOS_HLP_16bitAddSaturate function, input parameters:
	
	;; max value = MB64E_ENTRY_MAX_VALUE - MB64E_ENTRY_MIN_VALUE
	SET_BSR	MB64E_BASE
	movf	MB64E_ENTRY_MIN_VALUE, W, BANKED
	subwf	MB64E_ENTRY_MAX_VALUE, W, BANKED
	btfsc	WREG, 7; on overrun: set to 0x7f
	movlw 0x7f		
	movwf	MIOS_PARAMETER1			; low-byte of max value
	clrf	MIOS_PARAMETER2			; high-byte is almost zero

	;; encoder value = (ENC_VALUE_xx & 0x7f) - MB64E_ENTRY_MIN_VALUE
	call	MB64E_FSR_EncValue
	movf	INDF0, W
	andlw	0x7f
	movwf	TMP3
	
	movf	MB64E_ENTRY_MIN_VALUE, W, BANKED
	subwf	TMP3, W
	btfsc	WREG, 7; on overrun: set to 0x00
	movlw 0x00		
	movwf	TMP3				; low byte of encoder value
	clrf	TMP4				; high-byte is almost zero
	lfsr	FSR1, TMP3			; init FSR1 pointer

	;; get incrementer (stored in TMP1)
	movf	TMP1, W

	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = ENC_VALUE_xx contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; exit routine if value has not been changed
	btfss	MIOS_PARAMETER1, 0
	return
	
	;; store result + MB64E_ENTRY_MIN_VALUE in ENC_VALUE_xx
	movf	TMP3, W
	SET_BSR	MB64E_BASE
	addwf	MB64E_ENTRY_MIN_VALUE, W, BANKED
	movwf	INDF0
	movwf	TMP3

	;; store it also in EncLastValue if morph function is enabled
#if DEFAULT_NUMBER_AIN == 0 && DEFAULT_MORPH_FUNCTION_ENABLED == 1
	call	MB64E_FSR_EncLastValue
	movff	TMP3, INDF0
#endif

	;; send MIDI event
	rgoto	MB64E_ENC_SendIt

	;; ---
MB64E_ENC_SendNotAbs
	;; store incrementer & 0x7f in ENC_VALUE_xx
	call	MB64E_FSR_EncValue
	movf	TMP1, W		; (incrementer stored in TMP1)
	andlw	0x7f
	movwf	INDF0
	;; 	rgoto	MB64E_ENC_SendIt

MB64E_ENC_SendIt
	;; send MIDI value
	call	MB64E_MIDI_SendEncEvent

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; reset the cursor of the CS
	call	CS_MENU_ResetCursor

MB64E_ENC_Send_End
	return


;; --------------------------------------------------------------------------
;;  This function updates the speed modes
;;  IN:  MB_BANK
;;       WREG == 0x00: set encoders to normal speed mode
;;       WREG == 0x01: set encoders to slow speed mode
;;       WREG == 0x02: set encoders to fast speed mode
;;       WREG == 0xff: update depending on predefined value embedded in bank
;;  OUT: updated speeds
;;  USES: TMP1, TMP3, MB64E_CURRENT_ENTRY, BSR
;; --------------------------------------------------------------------------
MB64E_ENC_UpdateSpeed
	;; save mode in TMP3
	movwf	TMP3

	incf	TMP3, W
	bnz	MB64E_ENC_UpdateSpeed_NoPredef
	;; dirty workaround: don't update when encoders are muted
	;; why? since mostly when this flag is set, MB64E_ENC_MODE_BUFFER
	;; is used to store temporary menu data which shouldn't be overwritten
	btfsc	MB_STAT3, MB_STAT3_MUTE_ENCODERS
	return
	;; transfer the speed settings to MB64E_ENC_MODE_BUFFER
	call	MB64E_DUMP_RestoreEncSpeed
MB64E_ENC_UpdateSpeed_NoPredef

	;; process buffer, using MB64E_CURRENT_ENTRY as counter
	SET_BSR	MB64E_BASE
	clrf	MB64E_CURRENT_ENTRY, BANKED
MB64E_ENC_UpdateSpeedLoop
	;; take bank setting if TMP3 == 0xff, else take content of TMP3
	incf	TMP3, W
	bnz	MB64E_ENC_UpdateSpeedLoop_Forced
MB64E_ENC_UpdateSpeedLoop_Predef
	lfsr	FSR0, MB64E_ENC_MODE_BUFFER
	SET_BSR	MB64E_BASE
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	rcall	MB64E_ENC_Map
	movwf	TMP1
	rrf	TMP1, W
	andlw	0x3f
	btfss	TMP1, 0
	movf	PLUSW0, W
	btfsc	TMP1, 0
	swapf	PLUSW0, W
	rgoto	MB64E_ENC_UpdateSpeedLoop_Cont

MB64E_ENC_UpdateSpeedLoop_Forced
	movf	TMP3, W

MB64E_ENC_UpdateSpeedLoop_Cont
	BRA_IFSET WREG, 0, ACCESS, MB64E_ENC_UpdateSpeedLoop_1
	BRA_IFSET WREG, 1, ACCESS, MB64E_ENC_UpdateSpeedLoop_2
MB64E_ENC_UpdateSpeedLoop_0
	movlw	DEFAULT_ENC_SPEED_NORMAL_DIV
	movwf	MIOS_PARAMETER2
	movlw	DEFAULT_ENC_SPEED_NORMAL_MODE
	rgoto	MB64E_ENC_UpdateSpeedLoopSet
MB64E_ENC_UpdateSpeedLoop_1
	movlw	DEFAULT_ENC_SPEED_SLOW_DIV
	movwf	MIOS_PARAMETER2
	movlw	DEFAULT_ENC_SPEED_SLOW_MODE
	rgoto	MB64E_ENC_UpdateSpeedLoopSet
MB64E_ENC_UpdateSpeedLoop_2
	movlw	DEFAULT_ENC_SPEED_FAST_DIV
	movwf	MIOS_PARAMETER2
	movlw	DEFAULT_ENC_SPEED_FAST_MODE
	;; 	rgoto	MB64E_ENC_UpdateSpeedLoopSet
MB64E_ENC_UpdateSpeedLoopSet
	movwf	MIOS_PARAMETER1
	SET_BSR	MB64E_BASE
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	call	MIOS_ENC_SpeedSet

MB64E_ENC_UpdateSpeedLoopNext
	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_ENTRY, F, BANKED
	movlw	DEFAULT_NUMBER_ENCS-1
	cpfsgt	MB64E_CURRENT_ENTRY, BANKED
	rgoto MB64E_ENC_UpdateSpeedLoop

	;; if encoders not muted: restore ledring pattern settings
	CALL_IFCLR MB_STAT3, MB_STAT3_MUTE_ENCODERS, ACCESS, MB64E_DUMP_RestoreLEDRP

	;; exit
	return

	
;; --------------------------------------------------------------------------
;;  This function initializes the encoder map depending on MB64E_GROUP_OFFSET
;;  The map can be modified online with the "parameter select" button to 
;;  select parameters outside of the current group
;;  IN:  MB64E_GROUP_OFFSET
;;  OUT: MB64E_ENC_MAP[ 0] == MB64E_GROUP_OFFSET
;;       MB64E_ENC_MAP[ 1] == MB64E_GROUP_OFFSET+1
;;       MB64E_ENC_MAP[ 2] == MB64E_GROUP_OFFSET+2
;;       ...
;;       MB64E_ENC_MAP[63] == MB64E_GROUP_OFFSET+63
;;  USES: MB64E_CURRENT_ENTRY, BSR
;; --------------------------------------------------------------------------
MB64E_ENC_UpdateMap
	;; MB64E_CURRENT_ENTRY used as counter
	SET_BSR	MB64E_BASE
	clrf	MB64E_CURRENT_ENTRY, BANKED
	
MB64E_ENC_UpdateMapLoop
	call	MB64E_FSR_EncMap
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	addwf	MB64E_GROUP_OFFSET, W, BANKED
	andlw	0x7f		; wrap at 128
	movwf	INDF0

	incf	MB64E_CURRENT_ENTRY, F, BANKED
	BRA_IFCLR MB64E_CURRENT_ENTRY, 6, BANKED, MB64E_ENC_UpdateMapLoop

	;; update also the (new mapped) speed settings
	movlw	0xff
	goto	MB64E_ENC_UpdateSpeed


;; --------------------------------------------------------------------------
;;  Returns mapped encoder entry
;;  IN:  encoder number in WREG (0x00-0x3f)
;;  OUT: encoder entry in WREG (0x00-0x7f)
;;  USES: BSR, FSR1
;; --------------------------------------------------------------------------
MB64E_ENC_Map
	lfsr	FSR1, MB64E_ENC_MAP_00
	movf	PLUSW1, W
	return

;; --------------------------------------------------------------------------
;;  This function is by the encoder handler when MB_STAT_SEL_PARAMETER is set
;;  Input:
;;     o Encoder number in MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
MB64E_ENC_SetMap
	;; save encoder number in TMP1
	movff	MIOS_PARAMETER1, TMP1
	;; save incrementer in TMP2
	movff	MIOS_PARAMETER2, TMP2

	;; modify map value

	;; set maximum value
	movlw	0x7f
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2

	;; set current map value -> TMP[34] (clear TMP4)
	movf	TMP1, W
	rcall	MB64E_ENC_Map
	movwf	TMP3
	clrf	TMP4

	;; init pointer to map value
	lfsr	FSR1, TMP3

	;; get incrementer and call saturate function
	movf	TMP2, W		; incrementer stored in TMP2
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = TMP3 contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; exit routine if value has not been changed
	btfss	MIOS_PARAMETER1, 0
	return

	;; else store result
	movf	TMP1, W		; (encoder number)
	lfsr	FSR1, MB64E_ENC_MAP_00
	movff	TMP3, PLUSW1	; TMP3 -> MB64E_ENC_MAP_00[TMP1]

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; and exit
	return


;; --------------------------------------------------------------------------
;;  This function is by the encoder handler when MB_STAT_SEL_GROUP_AND_BANK is set
;;  Input:
;;     o Encoder number in MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
MB64E_ENC_SetGroupBank
	SET_BSR	MB64E_BASE

	;; save encoder number in TMP1
	movf	MIOS_PARAMETER1, W
	movwf	TMP1

	;; do nothing if encoder ID >= 2+1
	movlw	2+1
	cpfslt	MB64E_CURRENT_ENTRY, BANKED
	return

	;; save incrementer in TMP2
	movff	MIOS_PARAMETER2, TMP2

	;; modify bank or group or patch value
	BRA_IFSET TMP1, 0, ACCESS, MB64E_ENC_SetGroupBank_B
	BRA_IFSET TMP1, 1, ACCESS, MB64E_ENC_SetGroupBank_P

MB64E_ENC_SetGroupBank_G	; modify group
	;; set maximum value
	movlw	0x7f - DEFAULT_NUMBER_ENCS + 1
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2

	;; set current group value
	movf	MB64E_GROUP_OFFSET, W, BANKED
	movwf	TMP3
	clrf	TMP4

	;; init pointer to map value
	lfsr	FSR1, TMP3

	;; determine incremnter
	;; when positive: DEFAULT_ENCODER_GROUP_WIDTH
	;; when negative: -DEFAULT_ENCODER_GROUP_WIDTH
	movlw	DEFAULT_ENCODER_GROUP_WIDTH
	btfsc	TMP2, 7
	movlw -DEFAULT_ENCODER_GROUP_WIDTH

	;; call saturate function
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = TMP3 contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; exit routine if value has not been changed
	btfss	MIOS_PARAMETER1, 0
	return

	;; else store result
	movff	TMP3, MB64E_GROUP_OFFSET

	rgoto	MB64E_ENC_SetGroupBank_End

MB64E_ENC_SetGroupBank_B	; modify bank
	;; set maximum value
#if DEFAULT_PATCHMODE == 0
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	;; 8 banks maximum
	movlw	8-1
#else
	;; 64 banks maximum
	movlw	64-1
#endif
#else
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	;; 1 bank maximum
	movlw	1-1
#else
	;; 8 banks maximum
	movlw	8-1
#endif
#endif
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2

	;; set selected bank value
	movff	MB64E_SELECTED_BANK, TMP3
	clrf	TMP4

	;; init pointer to map value
	lfsr	FSR1, TMP3

	;; get incrementer and call saturate function
	movf	TMP2, W
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = TMP3 contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; exit routine if value has not been changed
	btfss	MIOS_PARAMETER1, 0
	return

	;; else change selected bank
	movff	TMP3, MB64E_SELECTED_BANK

	rgoto	MB64E_ENC_SetGroupBank_End

MB64E_ENC_SetGroupBank_P	; modify patch
	;; set maximum value
#if DEFAULT_PATCHMODE == 0
	;; only one patch
	movlw	0
#else
	;; 128 patches
	movlw	128-1
#endif
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2

	;; set selected patch value
	movff	MB64E_SELECTED_PATCH, TMP3
	clrf	TMP4

	;; init pointer to map value
	lfsr	FSR1, TMP3

	;; get incrementer and call saturate function
	movf	TMP2, W
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = TMP3 contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; exit routine if value has not been changed
	btfss	MIOS_PARAMETER1, 0
	return

	;; else change selected patch
	movff	TMP3, MB64E_SELECTED_PATCH

	;; 	rgoto	MB64E_ENC_SetGroupBank_End


MB64E_ENC_SetGroupBank_End
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; and exit
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
MB64E_ENC_ScaleValue
	;; save enc value in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	SET_BSR	MB64E_BASE

	;; send min value if min == max
	movf	MB64E_ENTRY_MIN_VALUE, W, BANKED
	cpfseq	MB64E_ENTRY_MAX_VALUE, BANKED
	rgoto MB64E_ENC_ScaleValueDo
	movwf	MIOS_PARAMETER1
	rgoto	MB64E_ENC_ScaleValue_End

MB64E_ENC_ScaleValueDo
	;; set MIOS_PARAMETER2[0] if min > max
	bcf	MIOS_PARAMETER2, 0
	movf	MB64E_ENTRY_MAX_VALUE, W, BANKED
	cpfsgt	MB64E_ENTRY_MIN_VALUE, BANKED
	rgoto MB64E_ENC_ScaleValue_NoConv
	bsf	MIOS_PARAMETER2, 0
MB64E_ENC_ScaleValue_NoConv

	;; scaled value-1 = ((current value+1) * (max-min+1)) / 128
	;; swap max/min if MIOS_PARAMETER2[0] set

	;; multiply current value * (max-min+1)
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, MB64E_ENC_ScaleValue_Inv
MB64E_ENC_ScaleValue_NoInv
	movf	MB64E_ENTRY_MIN_VALUE, W, BANKED
	subwf	MB64E_ENTRY_MAX_VALUE, W, BANKED
	rgoto	MB64E_ENC_ScaleValue_Cont1
MB64E_ENC_ScaleValue_Inv
	movf	MB64E_ENTRY_MAX_VALUE, W, BANKED
	subwf	MB64E_ENTRY_MIN_VALUE, W, BANKED
	;; 	rgoto	MB64E_ENC_ScaleValue_Cont1
MB64E_ENC_ScaleValue_Cont1
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
MB64E_ENC_ScaleValue_End
	movf	MIOS_PARAMETER1, W

	;; return immediately if inversion bit not set
	btfss	MIOS_PARAMETER2, 0
	return

	;; else inverse the result
	subwf	MB64E_ENTRY_MIN_VALUE, W, BANKED
	addwf	MB64E_ENTRY_MAX_VALUE, W, BANKED
	movwf	MIOS_PARAMETER1
	return



;; --------------------------------------------------------------------------
;;  This function is called when the morphing mode is active
;;  IN: encoder number in MB64E_CURRENT_ENTRY
;;      incrementer in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MB64E_ENC_Morph
	SET_BSR	MB64E_BASE

	;; 9 encoders are handled here:
	;; the 8 first encoders to morph the group
	;; the 9th encoder to morph all groups

	;; do nothing if encoder ID >= 8+1
	movlw	8+1
	cpfslt	MB64E_CURRENT_ENTRY, BANKED
	return

	;; save incrementer in TMP5
	movff	MIOS_PARAMETER2, TMP5

	;; set group select flags depending on encoder
	BRA_IFSET MB64E_CURRENT_ENTRY, 3, BANKED, MB64E_ENC_Morph_All
MB64E_ENC_Morph_Single
	;; decode the group flag from encoder number
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	call	MIOS_HLP_GetBitORMask
	rgoto	MB64E_ENC_Morph_Cont
MB64E_ENC_Morph_All
	;; select all groups
	movlw	0xff
MB64E_ENC_Morph_Cont
	movwf	MB64E_MORPH_GROUP_SELECT, BANKED

	
	;; now add incrementer (stored in TMP5) to all selected morph groups
	movwf	TMP1				; TMP1 saves the group flags, it will be shifted with every iteration
	lfsr	FSR0, MB64E_MORPH_VALUES_G0	; pointer to the morph values
	clrf	TMP2				; TMP2 notifies if any value has been changed
MB64E_ENC_Morph_AddLoop
	BRA_IFCLR TMP1, 0, ACCESS, MB64E_ENC_Morph_AddLoop_Next

	;; move group value to TMP[34], init FSR1 pointer
	movff	INDF0, TMP3
	clrf	TMP4
	lfsr	FSR1, TMP3

	;; set maximum value
	movlw	0x7f
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2

	;; get incrementer and call saturate function
	movf	TMP5, W		; incrementer stored in TMP5
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = TMP3 contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; next iteration if value has not been changed
	BRA_IFCLR MIOS_PARAMETER1, 0, ACCESS, MB64E_ENC_Morph_AddLoop_Next

	;; notify the change
	bsf	TMP2, 0

	;; store new value
	movff	TMP3, INDF0

MB64E_ENC_Morph_AddLoop_Next
	rrf	TMP1, F		; shift the select flags to the right
	movf	POSTINC0, W	; increment pointer to group values
	movf	FSR0L, W	; until all 8 groups processed
	andlw	0x07
	bnz	MB64E_ENC_Morph_AddLoop

	;; exit if no value has been changed
	btfss	TMP2, 0
	return

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; now loop through all encoder entries and send the selected groups

	;; using MB64E_CURRENT_ENTRY as loop counter
	SET_BSR	MB64E_BASE
	clrf	MB64E_CURRENT_ENTRY, BANKED	
MB64E_ENC_Morhp_Loop
	;; check if current group (==MB64E_CURRENT_ENTRY[6..4]) is selected
	SET_BSR	MB64E_BASE
	swapf	MB64E_CURRENT_ENTRY, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	MB64E_MORPH_GROUP_SELECT, W, BANKED
	bz	MB64E_ENC_Morhp_LoopNext

MB64E_ENC_Morhp_LoopSend
	;; get morph value of appr. group
	lfsr	FSR0, MB64E_MORPH_VALUES_G0
	swapf	MB64E_CURRENT_ENTRY, W, BANKED
	andlw	0x07
	movff	PLUSW0, MB64E_MORPH_VALUE
	;; fade enc value between two scenes
	rcall	MB64E_ENC_MorphIt
MB64E_ENC_Morhp_LoopNext
	;; loop until last enc reached
	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_ENTRY, F, BANKED
	BRA_IFCLR MB64E_CURRENT_ENTRY, 7, BANKED, MB64E_ENC_Morhp_Loop

	;; thats all
	return


;; --------------------------------------------------------------------------
;;  This function is used by the morph handler to fade a single enc between
;;  two scenes
;;  IN: o enc number in MB64E_CURRENT_ENTRY
;;      o morph value in MB64E_MORPH_VALUE
;;  USES: TMP1
;; --------------------------------------------------------------------------
MB64E_ENC_MorphIt
#if DEFAULT_NUMBER_AIN == 0 && DEFAULT_MORPH_FUNCTION_ENABLED == 1
	;; save current value in TMP1
	call	MB64E_FSR_EncValue		; get pointer to current value
	movff	INDF0, MB64E_ENTRY_MIN_VALUE	; save it in MB64E_ENTRY_MIN_VALUE
	bcf	MB64E_ENTRY_MIN_VALUE, 7, BANKED	; clear snap bit

	;; save value of target scene in TMP2
#if DEFAULT_PATCHMODE == 0
	movff	MB_BANK, TMP1			; save current bank in TMP1
	movff	MB64E_MORPH_BANK, MB_BANK	; switch to MORPH bank
	call	MB64E_DUMP_GetEncValue		; get current enc value
	movff	TMP1, MB_BANK			; restore bank number from TMP1
#else
	movff	MB_PATCH, TMP1			; save current patch in TMP1
	movff	MB64E_MORPH_BANK, MB_PATCH	; switch to MORPH patch
	call	MB64E_DUMP_GetEncValue		; get current enc value
	movff	TMP1, MB_PATCH			; restore patch number from TMP1
#endif

	SET_BSR	MB64E_BASE
	movwf	MB64E_ENTRY_MAX_VALUE, BANKED	; finally save value in MB64E_ENTRY_MAX_VALUE
	bcf	MB64E_ENTRY_MAX_VALUE, 7, BANKED	; clear snap bit

	;; extra: if MAX == MIN and morph value == 0x00 or 0x7f, send MIN/MAX directly
	;; otherwise it will never be sent since the resulting value will never change
	movf	MB64E_ENTRY_MAX_VALUE, W, BANKED
	cpfseq	MB64E_ENTRY_MIN_VALUE, BANKED
	rgoto MB64E_ENC_MorphItNormal
MB64E_ENC_MorphItExtra
	;; skip if MB64E_MORPH_VALUE neither 0x7f nor 0x00
	movf	MB64E_MORPH_VALUE, W, BANKED
	bz	MB64E_ENC_MorphItExtraCont
	xorlw	0x7f
	bnz	MB64E_ENC_MorphIt_End
MB64E_ENC_MorphItExtraCont
	;; just send MAX value (it's the same like the min value)
	movff	MB64E_ENTRY_MAX_VALUE, TMP1
	;; save sent value in "ENC_LAST_VALUE_xx"
	call	MB64E_FSR_EncLastValue
	movff	TMP1, INDF0
	rgoto	MB64E_ENC_MorphIt_Send

MB64E_ENC_MorphItNormal
	;; use the scale routine which sweeps between a min and max value
	movf	MB64E_MORPH_VALUE, W, BANKED
	rcall	MB64E_ENC_ScaleValue
	;; save result in TMP1
	movwf	TMP1

	;; compare it with last sent value, do nothing if value is equal
	call	MB64E_FSR_EncLastValue
	movf	INDF0, W
	xorwf	TMP1, W
	bz	MB64E_ENC_MorphIt_End

	;; save new value in register
	movff	TMP1, INDF0
	
MB64E_ENC_MorphIt_Send
	;; send enc value and exit
	call	MB64E_FSR_EncEvnt0
	movff	INDF0, MIDI_EVNT0
	call	MB64E_FSR_EncEvnt1
	movff	INDF0, MIDI_EVNT1
	movff	TMP1, MIDI_EVNT_VALUE
	goto	MIDI_EVNT_Send

#endif	; DEFAULT_NUMBER_AIN == 0 && DEFAULT_MORPH_FUNCTION_ENABLED == 1
MB64E_ENC_MorphIt_End
	return
