; $Id$
;
; MIDIbox64
; Pots handler
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
;;  pot has been moved. Input:
;;     o Pot number in WREG and MIOS_PARAMETER1
;;     o 7-bit value in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MB64_POT_Handler
	;; save current pot number in MB64_CURRENT_POT
	SET_BSR MB64_BASE
	movwf	MB64_CURRENT_POT, BANKED

#if DEFAULT_ENABLE_PAGE_HANDLING
	;;Current_Pot = Current_Pot + (MB64_PAGE << 4)
        swapf    MB64_PAGE, W, BANKED
        andlw    0x30
        addwf    MB64_CURRENT_POT, W, BANKED
        movwf    MB64_CURRENT_POT, BANKED
#endif

	;; save it also in MB64_LAST_POT0
	;; if LAST_POT0 != CURRENT_POT, shift value to LAST_POT1
	xorwf	MB64_LAST_POT0, W, BANKED
	skpz
	movff	MB64_LAST_POT0, MB64_LAST_POT1
	movff	MB64_CURRENT_POT, MB64_LAST_POT0
	;; notify that last controller move was *not* a button
	bcf	MB_STAT, MB_STAT_LAST_WAS_BUTTON

	;; for MIDI learn function: check if pot was turned to 00 or 7F
	movf	MIOS_PARAMETER2, W
	rcall	MB64_POT_Learn

	;; exit here if we are in learn mode (don't send pot value)
	btfsc	MB_STAT, MB_STAT_LEARN_MODE
	return

	;; branch to the morph handler if current pot matches with any MB64_MORPH_POT_xx
	;; set the appr flag in MORPH_GROUP_SELECT
	clrf	MB64_MORPH_GROUP_SELECT, BANKED
	incf	MB64_CURRENT_POT, W, BANKED
MB64_POT_Handler_ChkM1
	cpfseq	MB64_MORPH_POT_GLOBAL, BANKED
	rgoto MB64_POT_Handler_ChkM2
	bsf	MB64_MORPH_GROUP_SELECT, 0, BANKED
	bsf	MB64_MORPH_GROUP_SELECT, 1, BANKED
	bsf	MB64_MORPH_GROUP_SELECT, 2, BANKED
	bsf	MB64_MORPH_GROUP_SELECT, 3, BANKED
MB64_POT_Handler_ChkM2
	cpfseq	MB64_MORPH_POT_G0, BANKED
	rgoto MB64_POT_Handler_ChkM3
	bsf	MB64_MORPH_GROUP_SELECT, 0, BANKED
MB64_POT_Handler_ChkM3
	cpfseq	MB64_MORPH_POT_G1, BANKED
	rgoto MB64_POT_Handler_ChkM4
	bsf	MB64_MORPH_GROUP_SELECT, 1, BANKED
MB64_POT_Handler_ChkM4
	cpfseq	MB64_MORPH_POT_G2, BANKED
	rgoto MB64_POT_Handler_ChkM5
	bsf	MB64_MORPH_GROUP_SELECT, 2, BANKED
MB64_POT_Handler_ChkM5
	cpfseq	MB64_MORPH_POT_G3, BANKED
	rgoto MB64_POT_Handler_ChkMEnd
	bsf	MB64_MORPH_GROUP_SELECT, 3, BANKED
MB64_POT_Handler_ChkMEnd
	movf	MB64_MORPH_GROUP_SELECT, W, BANKED
	skpz
	rgoto	MB64_POT_Morph

	;; else clear morph page request
	call	MB64_MSG_Stop

	;; and branch to common send handler
	movf	MIOS_PARAMETER2, W
	rgoto	MB64_POT_Send


;; --------------------------------------------------------------------------
;;  This function is used to send a pot value
;;  Input:
;;     o Pot number in MB64_CURRENT_POT
;;     o 7-bit value in WREG
;; --------------------------------------------------------------------------
MB64_POT_Send
	;; store current pot value in TMP1
	movwf	TMP1

	;; scale value depending on Min/Max entry
	;; calc address to Min/Max entry:
	call	MB64_ADDR_PotEntry
	;; select the 3rd byte
	movlw	3-1
	addwf	MB_ADDRL, F

	;; scale value:
	;; copy min value to MB64_POT_MIN_VALUE
	;; copy max value to MB64_POT_MAX_VALUE
	;; move pot value to WREG
	call	MB64_BANK_Read
	movff	WREG, MB64_POT_MIN_VALUE
	call	MB64_BANK_Read
	movff	WREG, MB64_POT_MAX_VALUE
	movf	TMP1, W
	call	MB64_POT_ScaleValue
	
	;; store result in MB64_POT_NEW_VALUE
	movff	WREG, MB64_POT_NEW_VALUE

	;; get pointer to POT_LAST_VALUE_xx register
	call	MB64_FSR_PotLastValue

	;; store value in MB64_POT_LAST_VALUE
	movff	INDF0, MB64_POT_LAST_VALUE

	;; copy the new value into this location for the next call
	movff	MB64_POT_NEW_VALUE, INDF0

	;; if new value == old value, branch to the end
	SET_BSR	MB64_BASE
	movf	MB64_POT_NEW_VALUE, W, BANKED
	xorwf	MB64_POT_LAST_VALUE, W, BANKED
	bz	MB64_POT_Send_End

	;; process snap/relative/parallax mode
	call	MB64_POT_Conv
	;; branch to the end if zero bit set
	bz	MB64_POT_Send_End

	;; send MIDI value
	call	MB64_MIDI_SendPotEvent

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; reset the cursor of the CS
	call	CS_MENU_ResetCursor

MB64_POT_Send_End
	return


;; --------------------------------------------------------------------------
;;  learn mode handler for pots
;;  IN: pot number in WREG
;; --------------------------------------------------------------------------
MB64_POT_Learn
	;; if not in learn mode, invalidate variables
	BRA_IFCLR MB_STAT, MB_STAT_LEARN_MODE, ACCESS, MB64_POT_Learn_Invalid

	andlw	0x7f
	bnz	MB64_POT_Learn_NotTurned_00
MB64_POT_Learn_Turned_00
	movff	MB64_CURRENT_POT, MB64_POT_LAST_TURNED_00
	rgoto	MB64_POT_Learn_Check
MB64_POT_Learn_NotTurned_00
	xorlw	0x7f
	bnz	MB64_POT_Learn_NotTurned_7F
MB64_POT_Learn_Turned_7F
	movff	MB64_CURRENT_POT, MB64_POT_LAST_TURNED_7F
	rgoto	MB64_POT_Learn_Check
MB64_POT_Learn_NotTurned_7F
	return			; do nothing if neither 00 nor 7F

MB64_POT_Learn_Check
	;; when pot numbers are equal, trigger learn function
	SET_BSR	MB64_BASE
	movf	MB64_POT_LAST_TURNED_00, W, BANKED
	cpfseq	MB64_POT_LAST_TURNED_7F, BANKED
	return

	;; (pot number already in WREG)
	call	CS_M_LEARN_NotifyPotEvent
	;; 	rgoto	MB64_POT_Learn_Invalid

MB64_POT_Learn_Invalid
	;; invalidate variables
	SET_BSR	MB64_BASE
	setf	MB64_POT_LAST_TURNED_00, BANKED
	setf	MB64_POT_LAST_TURNED_7F, BANKED
	return


;; --------------------------------------------------------------------------
;;  This function scales a 7bit value depending on a min and max value
;;  If the min value is greater than the max value, they will be
;;  automatically converted to realise a reversed scaling
;;  Input:
;;     o 7bit value in WREG
;;     o min value in MB64_POT_MIN_VALUE
;;     o max value in MB64_POT_MAX_VALUE
;;  Output:
;;     o scaled value in WREG and MIOS_PARAMETER1
;;  USES: MIOS_PARAMETER1 and MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MB64_POT_ScaleValue
	;; save pot value in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	SET_BSR	MB64_BASE

	;; send min value if min == max
	movf	MB64_POT_MIN_VALUE, W, BANKED
	cpfseq	MB64_POT_MAX_VALUE, BANKED
	rgoto MB64_POT_ScaleValueDo
	movwf	MIOS_PARAMETER1
	rgoto	MB64_POT_ScaleValue_End

MB64_POT_ScaleValueDo
	;; set MIOS_PARAMETER2[0] if min > max
	bcf	MIOS_PARAMETER2, 0
	movf	MB64_POT_MAX_VALUE, W, BANKED
	cpfsgt	MB64_POT_MIN_VALUE, BANKED
	rgoto MB64_POT_ScaleValue_NoConv
	bsf	MIOS_PARAMETER2, 0
MB64_POT_ScaleValue_NoConv

	;; scaled value-1 = ((current value+1) * (max-min+1)) / 128
	;; swap max/min if MIOS_PARAMETER2[0] set

	;; multiply current value * (max-min+1)
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, MB64_POT_ScaleValue_Inv
MB64_POT_ScaleValue_NoInv
	movf	MB64_POT_MIN_VALUE, W, BANKED
	subwf	MB64_POT_MAX_VALUE, W, BANKED
	rgoto	MB64_POT_ScaleValue_Cont1
MB64_POT_ScaleValue_Inv
	movf	MB64_POT_MAX_VALUE, W, BANKED
	subwf	MB64_POT_MIN_VALUE, W, BANKED
	;; 	rgoto	MB64_POT_ScaleValue_Cont1
MB64_POT_ScaleValue_Cont1
	addlw	1
	mulwf	MIOS_PARAMETER1, ACCESS	; multiply with current value
	;; divide result by 128 (result >> 7)
	;; good trick: just shift the upper bit of the low byte into the high byte
	rlf	PRODL, W
	rlf	PRODH, W
	andlw	0x7f
	;; add min or max value depending on MIOS_PARAMETER2[0]
	btfss	MIOS_PARAMETER2, 0
	addwf	MB64_POT_MIN_VALUE, W, BANKED
	btfsc	MIOS_PARAMETER2, 0
	addwf	MB64_POT_MAX_VALUE, W, BANKED
	;; store result in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1
MB64_POT_ScaleValue_End
	movf	MIOS_PARAMETER1, W

	;; return immediately if inversion bit not set
	btfss	MIOS_PARAMETER2, 0
	return

	;; else inverse the result
	subwf	MB64_POT_MIN_VALUE, W, BANKED
	addwf	MB64_POT_MAX_VALUE, W, BANKED
	movwf	MIOS_PARAMETER1
	return


;; --------------------------------------------------------------------------
;;  This function processes the new pot value
;;  Input:
;;     o number of current pot in MB64_CURRENT_POT
;;     o last value in MB64_POT_LAST_VALUE
;;     o new value in MB64_POT_NEW_VALUE
;;     o min value in MB64_POT_MIN_VALUE
;;     o max value in MB64_POT_MAX_VALUE
;;  Output:
;;     o processed/converted value in INDF0
;;       zero bit cleared when the value should be sent
;; --------------------------------------------------------------------------
MB64_POT_Conv
	;; get pointer to POT_VALUE_xx register (the active value)
	;; (controlled from MB64 if snap bit is set, or via MIDI)
	call	MB64_FSR_PotValue

	;; if soft takeover flag already set, jump to the end
	BRA_IFSET INDF0, 7, ACCESS, MB64_POT_ConvBypass

	;; jump to the end if active value == new value
	movf	INDF0, W
	xorwf	MB64_POT_NEW_VALUE, W, BANKED
	bz	MB64_POT_ConvBypass

	;; else branch depending on selected pot mode
	SET_BSR	MB64_BASE
	BRA_IFSET MB64_CFG0, MB64_CFG0_PARALLAX, BANKED, MB64_POT_ConvParallax
	BRA_IFSET MB64_CFG0, MB64_CFG0_RELATIVE, BANKED, MB64_POT_ConvRelative
	BRA_IFSET MB64_CFG0, MB64_CFG0_SNAP, BANKED, MB64_POT_ConvSnap

	;; ------------------------------------------------------------------
MB64_POT_ConvNormal
	;; "normal" mode selected: copy set snap bit and exit
	rgoto	MB64_POT_ConvBypass

	;; ------------------------------------------------------------------
MB64_POT_ConvSnap
	;; "snap" mode selected: "soft-takeover"

	;; branch depending on clockwise or counter clockwise turn
	;; means: last value <= new value
	movf	MB64_POT_LAST_VALUE, W, BANKED
	cpfsgt	MB64_POT_NEW_VALUE, BANKED
	rgoto MB64_POT_ConvSnap_CClockwise
MB64_POT_ConvSnap_Clockwise
	;; pot has been moved clockwise

	;; exit if if new value <= active value
	movf	INDF0, W
	cpfsgt	MB64_POT_NEW_VALUE, BANKED
	rgoto MB64_POT_Conv_DontSend

	;; exit if active value >= last value
	cpfslt	MB64_POT_LAST_VALUE, BANKED
	rgoto MB64_POT_Conv_DontSend

	;; else continue
	rgoto	MB64_POT_ConvBypass
	
MB64_POT_ConvSnap_CClockwise
	;; pot has been counter clockwise
	;; exit if if new value >= active value
	movf	INDF0, W
	cpfslt	MB64_POT_NEW_VALUE, BANKED
	rgoto MB64_POT_Conv_DontSend

	;; exit if active value <= last value
	cpfsgt	MB64_POT_LAST_VALUE, BANKED
	rgoto MB64_POT_Conv_DontSend

	;; else continue
	rgoto	MB64_POT_ConvBypass


	;; ------------------------------------------------------------------
MB64_POT_ConvRelative
	;; "relative" mode selected:

	;; clear snap bit (will be set independent from last status)
	bcf	INDF0, 7

	;; calculate difference between last and new value
	movf	MB64_POT_LAST_VALUE, W, BANKED
	subwf	MB64_POT_NEW_VALUE, W, BANKED

	;; add difference to active value
	addwf	INDF0, F

	;; saturate:
	BRA_IFSET WREG, 7, ACCESS, MB64_POT_ConvRelative_CClockwise
MB64_POT_ConvRelative_Clockwise
	movf	MB64_POT_MAX_VALUE, W, BANKED
	BRA_IFSET INDF0, 7, ACCESS, MB64_POT_ConvRelative_Max
	cpfsgt	INDF0, ACCESS
	rgoto MB64_POT_ConvRelative_Cont
MB64_POT_ConvRelative_Max
	movwf	INDF0
	rgoto	MB64_POT_ConvRelative_Cont

MB64_POT_ConvRelative_CClockwise
	movf	MB64_POT_MIN_VALUE, W, BANKED
	BRA_IFSET INDF0, 7, ACCESS, MB64_POT_ConvRelative_Min
	cpfslt	INDF0, ACCESS
	rgoto MB64_POT_ConvRelative_Cont
MB64_POT_ConvRelative_Min
	movwf	INDF0
	rgoto	MB64_POT_ConvRelative_Cont

MB64_POT_ConvRelative_Cont
	rgoto	MB64_POT_Conv_Send

	
	;; ------------------------------------------------------------------
MB64_POT_ConvParallax
	;; clear snap bit (will be set independent from last status)
	bcf	INDF0, 7

	;; branch depending on clockwise or counter clockwise turn
	;; means: last value <= new value
	movf	MB64_POT_LAST_VALUE, W, BANKED
	cpfsgt	MB64_POT_NEW_VALUE, BANKED
	rgoto MB64_POT_ConvParallax_CClckwise
MB64_POT_ConvParallax_Clckw
	;; clockwise turn: send value = (127-active value) / (127-current pot value)
	movf	INDF0, W			; number (127-active value) in PROD[LH]
	sublw	0x7f
	movwf	PRODL
	clrf	PRODH
	movf	MB64_POT_NEW_VALUE, W, BANKED	; divisor (127-current pot value) in WREG
	sublw	0x7f
	call	MATH_DIV16_8			; divide
	movf	PRODL, W			; quotient in PRODL
	addwf	INDF0, F			; add to active value
	rgoto	MB64_POT_Conv_Send		; and send
	
MB64_POT_ConvParallax_CClckwise
	;; counter clockwise turn: send value = active value - (active value / current pot value)
	movf	INDF0, W			; number (active value) in PROD[LH]
	movwf	PRODL
	clrf	PRODH
	movf	MB64_POT_NEW_VALUE, W, BANKED	; divisor (current pot value) in WREG
	call	MATH_DIV16_8			; divide
	movf	PRODL, W
	subwf	INDF0, F			; subtract from active value
	rgoto	MB64_POT_Conv_Send		; and send

	;; ------------------------------------------------------------------
MB64_POT_ConvBypass
	;; save new value in active value
	movff	MB64_POT_NEW_VALUE, INDF0

	;; ------------------------------------------------------------------
MB64_POT_Conv_Send
	;; set snap bit
	bsf	INDF0, 7

	iorlw	0xff		; (clear zero bit)
	return

	;; ------------------------------------------------------------------
MB64_POT_Conv_DontSend
	andlw	0x00		; (set zero bit)
	return



;; --------------------------------------------------------------------------
;;  This function is called when the current pot matches with a pot assigned
;;  to the morph function
;;  IN: 7-bit morph value in MIOS_PARAMETER2
;;      selected group in MB64_MORPH_GROUP_SELECT:
;;         bit 0: group0 - pot 0x00-0x0f
;;         bit 1: group1 - pot 0x10-0x1f
;;         bit 2: group2 - pot 0x20-0x2f
;;         bit 3: group3 - pot 0x30-0x3f
;; --------------------------------------------------------------------------
MB64_POT_Morph
	;; save morph value in MB64_MORPH_VALUE
	SET_BSR	MB64_BASE
	movff	MIOS_PARAMETER2, MB64_MORPH_VALUE

	;; using MB64_CURRENT_POT as loop counter
	clrf	MB64_CURRENT_POT, BANKED	
MB64_POT_Morph_Loop
	;; don't send the values of the morph pots
	incf	MB64_CURRENT_POT, W, BANKED
MB64_POT_Morph_LoopChk1
	cpfseq	MB64_MORPH_POT_GLOBAL, BANKED
	rgoto MB64_POT_Morph_LoopChk2
	rgoto	MB64_POT_Morph_LoopNext
MB64_POT_Morph_LoopChk2
	cpfseq	MB64_MORPH_POT_G0, BANKED
	rgoto MB64_POT_Morph_LoopChk3
	rgoto	MB64_POT_Morph_LoopNext
MB64_POT_Morph_LoopChk3
	cpfseq	MB64_MORPH_POT_G1, BANKED
	rgoto MB64_POT_Morph_LoopChk4
	rgoto	MB64_POT_Morph_LoopNext
MB64_POT_Morph_LoopChk4
	cpfseq	MB64_MORPH_POT_G2, BANKED
	rgoto MB64_POT_Morph_LoopChk5
	rgoto	MB64_POT_Morph_LoopNext
MB64_POT_Morph_LoopChk5
	cpfseq	MB64_MORPH_POT_G3, BANKED
	rgoto MB64_POT_Morph_LoopChkR0
	rgoto	MB64_POT_Morph_LoopNext

MB64_POT_Morph_LoopChkR0
	;; check if current group (==MB64E_CURRENT_ENC[5..4]) is selected
	SET_BSR	MB64_BASE
	swapf	MB64_CURRENT_POT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	MB64_MORPH_GROUP_SELECT, W, BANKED
	bz	MB64_POT_Morph_LoopNext
	
MB64_POT_Morph_LoopSend
	;; no morph pot: fade pot value between two scenes
	rcall	MB64_POT_MorphIt
MB64_POT_Morph_LoopNext
	;; loop until last pot reached
	SET_BSR	MB64_BASE
	incf	MB64_CURRENT_POT, F, BANKED
	movlw	DEFAULT_NUMBER_POTS-1
	cpfsgt	MB64_CURRENT_POT, BANKED
	rgoto MB64_POT_Morph_Loop

	;; request morph page and exit
	movlw	MB64_MSG_ID_MORPH
	goto	MB64_MSG_Start


;; --------------------------------------------------------------------------
;;  This function is used by the morph handler to fade a single pot between
;;  two scenes
;;  IN: o pot number in MB64_CURRENT_POT
;;      o morph value in MB64_MORPH_VALUE
;;  USES: TMP1
;; --------------------------------------------------------------------------
MB64_POT_MorphIt
	;; save current value in TMP1
	call	MB64_FSR_PotValue		; get pointer to current value
	movff	INDF0, MB64_POT_MIN_VALUE	; save it in MB64_POT_MIN_VALUE
	bcf	MB64_POT_MIN_VALUE, 7, BANKED	; clear snap bit

	;; save value of target scene in TMP2
#if DEFAULT_PATCHMODE == 0
	movff	MB_BANK, TMP1			; save current bank in TMP1
	movff	MB64_MORPH_BANK, MB_BANK	; switch to MORPH bank
	call	MB64_DUMP_GetPotValue		; get current pot value
	movff	TMP1, MB_BANK			; restore bank number from TMP1
#else
	movff	MB_PATCH, TMP1			; save current patch in TMP1
	movff	MB64_MORPH_BANK, MB_PATCH	; switch to MORPH patch
	call	MB64_DUMP_GetPotValue		; get current pot value
	movff	TMP1, MB_PATCH			; restore patch number from TMP1
#endif

	SET_BSR	MB64_BASE
	movwf	MB64_POT_MAX_VALUE, BANKED	; finally save value in MB64_POT_MAX_VALUE
	bcf	MB64_POT_MAX_VALUE, 7, BANKED	; clear snap bit

	;; extra: if MAX == MIN and morph value == 0x00 or 0x7f, send MIN/MAX directly
	;; otherwise it will never be sent since the resulting value will never change
	movf	MB64_POT_MAX_VALUE, W, BANKED
	cpfseq	MB64_POT_MIN_VALUE, BANKED
	rgoto MB64_POT_MorphItNormal
MB64_POT_MorphItExtra
	;; skip if MB64_MORPH_VALUE neither 0x7f nor 0x00
	movf	MB64_MORPH_VALUE, W, BANKED
	bz	MB64_POT_MorphItExtraCont
	xorlw	0x7f
	bnz	MB64_POT_MorphIt_End
MB64_POT_MorphItExtraCont
	;; just send MAX value (it's the same like the min value)
	movff	MB64_POT_MAX_VALUE, TMP1
	;; save sent value in "POT_LAST_VALUE_xx"
	call	MB64_FSR_PotLastValue
	movff	TMP1, INDF0
	rgoto	MB64_POT_MorphIt_Send

MB64_POT_MorphItNormal
	;; use the scale routine which sweeps between a min and max value
	movf	MB64_MORPH_VALUE, W, BANKED
	rcall	MB64_POT_ScaleValue
	;; save result in TMP1
	movwf	TMP1

	;; compare it with last sent value, do nothing if value is equal
	call	MB64_FSR_PotLastValue
	movf	INDF0, W
	xorwf	TMP1, W
	bz	MB64_POT_MorphIt_End

	;; save new value in register
	movff	TMP1, INDF0
	
MB64_POT_MorphIt_Send
	;; send pot value and exit
	call	MB64_FSR_PotEvnt0
	movff	INDF0, MIDI_EVNT0
	call	MB64_FSR_PotEvnt1
	movff	INDF0, MIDI_EVNT1
	movff	TMP1, MIDI_EVNT_VALUE
	goto	MIDI_EVNT_Send

MB64_POT_MorphIt_End
	return
