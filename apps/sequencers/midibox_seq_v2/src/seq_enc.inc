; $Id$
;
; MIDIboxSEQ
; Encoder handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by USER_ENC_NotifyChange when the datawheel has
;;  been moved. Input:
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
SEQ_ENC_Datawheel
	SET_BSR	SEQ_BASE
	;; branch in scrub mode
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_SCRUB, BANKED, SEQ_ENC_Datawheel_Scrub

	;; if we are in page #0, use the data wheel to modify the "active step"
	BRA_IFCLR CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, SEQ_ENC_Datawheel_NotP0
	movf	CS_MENU_POS, W
	bz	SEQ_ENC_Datawheel_P0
SEQ_ENC_Datawheel_NotP0

	;; else branch to the common CS function for the datawheel
	movf	MIOS_PARAMETER2, W
	goto	CS_MENU_Enc

	;; ---
SEQ_ENC_Datawheel_Scrub
	;; in scrub mode: enable sequencer if not already running
	BRA_IFSET SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_ENC_Datawheel_ScrubRun
	movlw	(1 << SEQ_REQ_START)
	movwf	SEQ_REQ, BANKED	
SEQ_ENC_Datawheel_ScrubRun
	;; do nothing on decrements (handling for reverse scrub would be too difficult)
	btfsc	MIOS_PARAMETER2, 7
	return
	;; release pause if set
	bcf	SEQ_STATE, SEQ_STATE_PAUSE, BANKED
	;; add some ticks to running clock
	movf	MIOS_PARAMETER2, W
	mullw	4*6 / 2		; (1/2 step)
	movf	PRODL, W
	addwf	SEQ_CLK_REQ_CTR, F, BANKED
	return

	;; ---
SEQ_ENC_Datawheel_P0
	;; if we are in page #0, use the data wheel to modify the "active step"
	movff	SEQ_CURRENT_STEP, WREG
	goto	SEQ_ENC_Handler

;; --------------------------------------------------------------------------
;;  This function is called by USER_ENC_NotifyChange when an encoder has
;;  been moved. Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
SEQ_ENC_Handler

	;; save encoder number in SEQ_CURRENT_STEP and SEQ_EVNTS
	SET_BSR SEQ_BASE
	movwf	SEQ_CURRENT_STEP, BANKED
	movwf	SEQ_EVNTS, BANKED

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; save incrementer in SEQ_ENC_INC
	movff	MIOS_PARAMETER2, SEQ_ENC_INC

	;; loop through all selected tracks
	clrf	SEQ_EVNTT, BANKED
SEQ_ENC_Handler_Loop
	;; check if track selected
	movf	SEQ_EVNTT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_SELECTED_TRKS, W, BANKED
	andlw	0x0f
	bz	SEQ_ENC_Handler_Loop_Next

	;; get pointer to pot value register depending on selected layer and save value in TMP[12]
	call	SEQ_FSR_PotValue
	movff	INDF0, TMP1	; low-byte
	clrf	TMP2		; high-byte

	;; get parameter type
	call	SEQ_PTYPE_Get

	;; temporary save incrementer in TMP3
	movff	SEQ_ENC_INC, TMP3

#if 0
	;; multiply incrementer by 4 if LAYER_EVNTL flag set
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNTL, BANKED, SEQ_ENC_Handler_NoLenModif
SEQ_ENC_Handler_LenModif
	movf	TMP3, W
	mullw	4
	movff	PRODL, TMP3
SEQ_ENC_Handler_NoLenModif
#endif

	;; multiply incrementer by 4 if ARP_MODIF_VALUE flag set
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_ARP_MODIF_VALUE, BANKED, SEQ_ENC_Handler_NoArpModif
SEQ_ENC_Handler_ArpModif
	movf	TMP3, W
	mullw	4
	movff	PRODL, TMP3	
SEQ_ENC_Handler_NoArpModif

	;; inc/dec value by using the 16 bit add w/ saturation routine
	;; set pointer to TMP1 (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, TMP1
	;; set max value
	movlw	0x7f
	movwf	MIOS_PARAMETER1	; low-byte of max value
	clrf	MIOS_PARAMETER2	; high-byte of max value
	;; move incrementer (temporary stored in TMP3) to WREG
	movf	TMP3, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = TMP[12] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; do nothing if flag not set
	btfss	MIOS_PARAMETER1, 0
	return

	;; send value
	call	SEQ_POTENC_Send

SEQ_ENC_Handler_Loop_Next
	SET_BSR	SEQ_BASE
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 2, BANKED, SEQ_ENC_Handler_Loop

	;; trigger pot display page
	call	SEQ_MSG_Start

	return
