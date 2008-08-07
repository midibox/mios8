; $Id$
;
; MIDIboxSEQ
; LED handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; help macro to make LED assignments easier
SEQ_LED_UPDATE_DOUT MACRO register, flag, bank, dout
	clrf	MIOS_PARAMETER1
	btfsc	register, flag, bank
	setf	MIOS_PARAMETER1
	movlw	dout
	rcall	SEQ_LED_Update_PinSet
	ENDM

SEQ_LED_UPDATE_DOUT_W_SHIFT MACRO register, flag, bank, dout
	LOCAL	no_shift
	BRA_IFCLR SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, no_shift
	clrf	MIOS_PARAMETER1
	btfsc	register, flag, bank
	setf	MIOS_PARAMETER1
	movlw	dout
	rcall	SEQ_LED_Update_PinSet
no_shift:
	ENDM

SEQ_LED_UPDATE_DOUT_INV MACRO register, flag, bank, dout
	setf	MIOS_PARAMETER1
	btfsc	register, flag, bank
	clrf	MIOS_PARAMETER1
	movlw	dout
	rcall	SEQ_LED_Update_PinSet
	ENDM

;; --------------------------------------------------------------------------
;;  this function is called by cs_menu.inc on every "display" update
;; --------------------------------------------------------------------------
SEQ_LED_Update
	;; update GP LEDs
	call	SEQ_GP_LED_Update

	;; disable IRQs to avoid inconsistencies (or flickering LEDs while alternate SHIFT assignments are made)
	IRQ_DISABLE

	SET_BSR	SEQ_BASE

 IF LED_LAYER_GATE < 128
	SEQ_LED_UPDATE_DOUT SEQ_SELECTED_LAYERS, 0, BANKED, LED_LAYER_GATE
 ENDIF
 IF LED_LAYER_AUX < 128
	SEQ_LED_UPDATE_DOUT SEQ_SELECTED_LAYERS, 1, BANKED, LED_LAYER_AUX
 ENDIF


 IF LED_EDIT < 128
	SEQ_LED_UPDATE_DOUT SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED, LED_EDIT
 ENDIF
 IF LED_MUTE < 128
	SEQ_LED_UPDATE_DOUT SEQ_MODE0, SEQ_MODE0_MUTE, BANKED, LED_MUTE
 ENDIF

	;; extra: the pattern LED flashes if a pattern should be stored
 IF LED_PATTERN < 128
SEQ_LED_Update_PatternLED
	clrf	MIOS_PARAMETER1
	btfsc	SEQ_MODE0, SEQ_MODE0_PATTERN, BANKED
	setf	MIOS_PARAMETER1

	movlw	CS_MENU_PAGE_PATTERN_S
	xorwf	CS_MENU_POS, W
	bnz	SEQ_LED_Update_PatternLED_NoFl
SEQ_LED_Update_PatternLED_Fl
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH
	clrf	MIOS_PARAMETER1
SEQ_LED_Update_PatternLED_NoFl
	movlw	LED_PATTERN
	rcall	SEQ_LED_Update_PinSet
 ENDIF

	;; extra: the song LED flashes if chain should be stored
 IF LED_SONG < 128
SEQ_LED_Update_SongLED
	clrf	MIOS_PARAMETER1
	btfsc	SEQ_MODE0, SEQ_MODE0_SONG, BANKED
	setf	MIOS_PARAMETER1

	movlw	CS_MENU_PAGE_SONG_S
	xorwf	CS_MENU_POS, W
	bnz	SEQ_LED_Update_SongLED_NoFl
SEQ_LED_Update_SongLED_Fl
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH
	clrf	MIOS_PARAMETER1
SEQ_LED_Update_SongLED_NoFl
	movlw	LED_SONG
	rcall	SEQ_LED_Update_PinSet
 ENDIF



 IF LED_SOLO < 128
	SEQ_LED_UPDATE_DOUT SEQ_MODE0, SEQ_MODE0_SOLO, BANKED, LED_SOLO
 ENDIF
 IF LED_FAST < 128
	SEQ_LED_UPDATE_DOUT SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED, LED_FAST
 ENDIF
 IF LED_ALL < 128
	SEQ_LED_UPDATE_DOUT SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED, LED_ALL
 ENDIF

		
	;; track group 1-4
	rrf	SEQ_SELECTED_TRK, W, BANKED
	rrf	WREG, W
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	movwf	PRODL

 IF LED_GROUP1 < 128
	SEQ_LED_UPDATE_DOUT PRODL, 0, ACCESS, LED_GROUP1
 ENDIF
 IF LED_GROUP2 < 128
	SEQ_LED_UPDATE_DOUT PRODL, 1, ACCESS, LED_GROUP2
 ENDIF
 IF LED_GROUP3 < 128
	SEQ_LED_UPDATE_DOUT PRODL, 2, ACCESS, LED_GROUP3
 ENDIF
 IF LED_GROUP4 < 128
	SEQ_LED_UPDATE_DOUT PRODL, 3, ACCESS, LED_GROUP4
 ENDIF


 IF LED_SHIFT < 128
	SEQ_LED_UPDATE_DOUT SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, LED_SHIFT
 ENDIF
 IF LED_ALT < 128
	SEQ_LED_UPDATE_DOUT SEQ_MODE0, SEQ_MODE0_ALT, BANKED, LED_ALT
 ENDIF


 IF LED_RECORD < 128
	SEQ_LED_UPDATE_DOUT SEQ_MODE1, SEQ_MODE1_RECORD, BANKED, LED_RECORD
 ENDIF


 IF LED_PLAY < 128
	SEQ_LED_UPDATE_DOUT SEQ_STATE, SEQ_STATE_RUN, BANKED, LED_PLAY
 ENDIF
 IF LED_STOP < 128
	SEQ_LED_UPDATE_DOUT_INV SEQ_STATE, SEQ_STATE_RUN, BANKED, LED_STOP
 ENDIF
 IF LED_PAUSE < 128
	SEQ_LED_UPDATE_DOUT SEQ_STATE, SEQ_STATE_PAUSE, BANKED, LED_PAUSE
 ENDIF
 IF LED_FWD < 128
	SEQ_LED_UPDATE_DOUT SEQ_MODE2, SEQ_MODE2_FWD_HOLD, BANKED, LED_FWD
 ENDIF
 IF LED_REW < 128
	SEQ_LED_UPDATE_DOUT SEQ_MODE2, SEQ_MODE2_REW_HOLD, BANKED, LED_REW
 ENDIF
 IF LED_LIVE < 128
	SEQ_LED_UPDATE_DOUT SEQ_MODE2, SEQ_MODE2_LIVE_MODE, BANKED, LED_LIVE
 ENDIF

#if 1
	;; extra for shared button in MB808 frontpanel (can be made more generic later)
 IF LED_LIVE < 128
	SEQ_LED_UPDATE_DOUT_W_SHIFT SEQ_MODE1, SEQ_MODE1_RECORD, BANKED, LED_LIVE
 ENDIF
 IF LED_STOP < 128
	SEQ_LED_UPDATE_DOUT_W_SHIFT SEQ_STATE, SEQ_STATE_PAUSE, BANKED, LED_STOP
 ENDIF
 IF LED_MUTE < 128
	SEQ_LED_UPDATE_DOUT_W_SHIFT SEQ_MODE0, SEQ_MODE0_SOLO, BANKED, LED_MUTE
 ENDIF
 IF LED_EDIT < 128
	SEQ_LED_UPDATE_DOUT_W_SHIFT SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED, LED_EDIT
 ENDIF
#endif

	IRQ_ENABLE


	return


	;; help function (rgoto only requires 2 bytes instead of 4 bytes)
SEQ_LED_Update_PinSet
	goto	MIOS_DOUT_PinSet


;; --------------------------------------------------------------------------
;;  this function is called by USER_SR_ServicePrepare before the DOUT chains
;;  will be loaded
;; --------------------------------------------------------------------------
SEQ_LED_UpdateIRQ
	;; we are using multiplications within a IRQ service handler - store PROD[LH]
	movff	PRODL, SEQ_IRQ_SAVED_PRODL
	movff	PRODH, SEQ_IRQ_SAVED_PRODH

	;; ---[ increment the BLM row counter (used when DEFAULT_SRM_ENABLED or DEFAULT_BPM_DIGITS_ENABLED set ]---
	SET_BSR	SEQ_BASE
	incf	SEQ_BLM_ROW_CTR, F, BANKED
	bcf	SEQ_BLM_ROW_CTR, 2, BANKED	; wrap at 4

	;; ---[ update the GP LEDs ]---

	;; store selected track in IRQ_TMP5[3:0]
	call	CS_M_HLP_GetSelectedTrk
	movwf	IRQ_TMP5

	;; get current seq pos, store it in IRQ_TMP3
	lfsr	FSR1, SEQ_TRKVAR0
	movf	IRQ_TMP5, W	; (selected track = IRQ_TMP5[3:0])
	andlw	0x0f
	mullw	SEQ_TRKVARRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	movlw	SEQ_TRKVARSTEPx
	movff	PLUSW1, IRQ_TMP3

	;; display marker edit page, don't display it in song, pattern and mute page
	;; marker stored in IRQ_TMP5, [3:0] holds the GP LED number, flag [4] and [5] enable it
	clrf	IRQ_TMP5
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED, SEQ_LED_UpdateIRQ_NoMarker
SEQ_LED_UpdateIRQ_MakeMarker
	;; check if sequencer is running, and the current step position should be marked (either via XOR or second colour of DUO LED)
	;; notified via IRQ_TMP5[4] for GPLEDs
	;; notified via IRQ_TMP5[5] for matrix LEDs
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_LED_UpdateIRQ_SeqOff
SEQ_LED_UpdateIRQ_SeqOn
	;; if pause: flash step LED
	BRA_IFCLR SEQ_STATE, SEQ_STATE_PAUSE, BANKED, SEQ_LED_UpdateIRQ_SeqOn_NoPause
	BRA_IFCLR CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, SEQ_LED_UpdateIRQ_SeqIgnore
SEQ_LED_UpdateIRQ_SeqOn_NoPause

	;; check for GPLEDs
SEQ_LED_UpdateIRQ_SeqOn_GPLED
	;; (current position in IRQ_TMP3)
	;; don't print if played step not within view (1-16/17-31)
	swapf	SEQ_SELECTED_STEP_VIEW, W, BANKED
	xorwf	IRQ_TMP3, W
	andlw	0x30
	bnz	SEQ_LED_UpdateIRQ_SeqOn_GPLED_NM
SEQ_LED_UpdateIRQ_SeqOn_GPLED_M
	;; mark position
	bsf	IRQ_TMP5, 4
SEQ_LED_UpdateIRQ_SeqOn_GPLED_NM
SEQ_LED_UpdateIRQ_NoMarker


#if DEFAULT_SRM_ENABLED
	;; check for matrix LEDs
SEQ_LED_UpdateIRQ_SeqOn_LM
	;; get matrix seq pos, store it in IRQ_TMP4
	lfsr	FSR1, SEQ_TRKVAR0
	movf	IRQ_TMP5, W	; (selected track group = IRQ_TMP5[3:2])
	andlw	0x0c
	addwf	SEQ_BLM_ROW_CTR, W, BANKED
	mullw	SEQ_TRKVARRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	movlw	SEQ_TRKVARSTEPx
	movff	PLUSW1, IRQ_TMP4

	;; don't print if played step not within view (1-16/17-31)
	BRA_IFSET SEQ_SELECTED_STEP_VIEW, 0, BANKED, SEQ_LED_UpdateIRQ_SeqOn_LM_S1
SEQ_LED_UpdateIRQ_SeqOn_LM_S0
	BRA_IFSET IRQ_TMP4, 4, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_LM_NM
	rgoto	SEQ_LED_UpdateIRQ_SeqOn_LM_C

SEQ_LED_UpdateIRQ_SeqOn_LM_S1
	BRA_IFCLR IRQ_TMP4, 4, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_LM_NM
	;; 	rgoto	SEQ_LED_UpdateIRQ_SeqOn_LM_C

SEQ_LED_UpdateIRQ_SeqOn_LM_C
	;; mark position
	bsf	IRQ_TMP5, 5
SEQ_LED_UpdateIRQ_SeqOn_LM_NM
#endif

SEQ_LED_UpdateIRQ_SeqIgnore
SEQ_LED_UpdateIRQ_SeqOff


#if DEFAULT_GP_DOUT_SR_L != DEFAULT_SRM_DOUT_L1
	;; ---[ update primary GP LEDs of the control surface ]---
	;; only used when Shift Registers of the primary GP LEDs are not equal to the multiplexed LED SRs
SEQ_LED_UpdateIRQ_GPLED
	;; transfer current LED pattern to IRQ_TMP[12]
	movff	SEQ_GP_LED_L, IRQ_TMP1
	movff	SEQ_GP_LED_R, IRQ_TMP2

	;; flickering feature for INST_OVERLAY
	;; clear LEDs whenever INST_OVERLAY_CTR[5] set
	;; in addition, marker disabled on instrument selection
	movf	CS_MENU_INST_OVERLAY_CTR, W
	bz	SEQ_LED_UpdateIRQ_GPLED_NoInstOv
SEQ_LED_UpdateIRQ_GPLED_InstOv
	btfsc	CS_MENU_INST_OVERLAY_CTR, 5
	clrf	IRQ_TMP1
	btfsc	CS_MENU_INST_OVERLAY_CTR, 5
	clrf	IRQ_TMP2
	rgoto	SEQ_LED_UpdateIRQ_GPLED_NoMarker

SEQ_LED_UpdateIRQ_GPLED_NoInstOv

	;; live meters active?
	;; TODO: find new mode - currently controlled with "loop" button
	BRA_IFCLR SEQ_MODE2, SEQ_MODE2_LIVEPLAY, BANKED, SEQ_LED_UpdateIRQ_GPLED_NoLivePM
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_ALT, BANKED, SEQ_LED_UpdateIRQ_GPLED_NoLivePM
SEQ_LED_UpdateIRQ_GPLED_LivePM
	movff	SEQ_LIVEPLAY_METERS0, IRQ_TMP1
	movff	SEQ_LIVEPLAY_METERS1, IRQ_TMP2
	rgoto	SEQ_LED_UpdateIRQ_GPLED_NoMarker
SEQ_LED_UpdateIRQ_GPLED_NoLivePM

	;; check if a position marker should be XORed
	BRA_IFCLR IRQ_TMP5, 4, ACCESS, SEQ_LED_UpdateIRQ_GPLED_NoMarker
SEQ_LED_UpdateIRQ_GPLED_Marker
	;; get xor mask and operate with IRQ_TMP[12] (pos[3] selects the register)
	movf	IRQ_TMP3, W
	call	MIOS_HLP_GetBitORMask

	btfss	IRQ_TMP3, 3
	xorwf	IRQ_TMP1, F
	btfsc	IRQ_TMP3, 3
	xorwf	IRQ_TMP2, F
SEQ_LED_UpdateIRQ_GPLED_NoMarker

	;; transfer LED value to DOUT registers
#if DEFAULT_GP_DOUT_SR_L
	movff	IRQ_TMP1, MIOS_PARAMETER1
	movlw	DEFAULT_GP_DOUT_SR_L - 1
	call	MIOS_DOUT_SRSet
#endif

#if DEFAULT_GP_DOUT_SR_R
	movff	IRQ_TMP2, MIOS_PARAMETER1
	movlw	DEFAULT_GP_DOUT_SR_R - 1
	call	MIOS_DOUT_SRSet
#endif
#endif



#if DEFAULT_SRM_ENABLED
	;; ---[ update LED matrix ]---
SEQ_LED_UpdateIRQ_LM
	;; transfer current LED pattern to IRQ_TMP[12]
	;; Track LED Row 1-4 - values stored in separate array
	lfsr	FSR1, SEQ_LEDM_TRK0_A_L
	rlf	SEQ_BLM_ROW_CTR, W, BANKED
	andlw	0xfe
	movff	PLUSW1, IRQ_TMP1
	addlw	1
	movff	PLUSW1, IRQ_TMP2

#if DEFAULT_SRM_DOUT_DUOCOLOUR == 0
	;; check if a position marker should be XORed
	BRA_IFCLR IRQ_TMP5, 5, ACCESS, SEQ_LED_UpdateIRQ_LM_NoMarker
SEQ_LED_UpdateIRQ_LM_Marker
	;; get xor mask and operate with IRQ_TMP[12] (pos[3] selects the register)
	movf	IRQ_TMP4, W
	call	MIOS_HLP_GetBitORMask

	btfss	IRQ_TMP4, 3
	xorwf	IRQ_TMP1, F
	btfsc	IRQ_TMP4, 3
	xorwf	IRQ_TMP2, F
SEQ_LED_UpdateIRQ_LM_NoMarker
#endif

	;; transfer LED value to DOUT registers
#if DEFAULT_SRM_DOUT_L1
	movff	IRQ_TMP1, MIOS_PARAMETER1
	movlw	DEFAULT_SRM_DOUT_L1 - 1
	call	MIOS_DOUT_SRSet
#endif
	
#if DEFAULT_SRM_DOUT_L1
	movff	IRQ_TMP2, MIOS_PARAMETER1
	movlw	DEFAULT_SRM_DOUT_R1 - 1
	call	MIOS_DOUT_SRSet
#endif

#if DEFAULT_SRM_DOUT_DUOCOLOUR
	;; drive the second colour LEDs
	lfsr	FSR1, SEQ_LEDM_TRK0_B_L
	rlf	SEQ_BLM_ROW_CTR, W, BANKED
	andlw	0xfe
	movff	PLUSW1, IRQ_TMP1
	addlw	1
	movff	PLUSW1, IRQ_TMP2

	;; if marker is requested (song position), invert the second colour LED
	BRA_IFCLR IRQ_TMP5, 5, ACCESS, SEQ_LED_UpdateIRQ_LM_NoMarker
SEQ_LED_UpdateIRQ_LM_Marker
	movf	IRQ_TMP4, W
	call	MIOS_HLP_GetBitORMask

	btfss	IRQ_TMP4, 3
	xorwf	IRQ_TMP1, F
	btfsc	IRQ_TMP4, 3
	xorwf	IRQ_TMP2, F
SEQ_LED_UpdateIRQ_LM_NoMarker

#if DEFAULT_SRM_DOUT_L2
	movff	IRQ_TMP1, MIOS_PARAMETER1
	movlw	DEFAULT_SRM_DOUT_L2 - 1
	call	MIOS_DOUT_SRSet
#endif

#if DEFAULT_SRM_DOUT_R2
	movff	IRQ_TMP2, MIOS_PARAMETER1
	movlw	DEFAULT_SRM_DOUT_R2 - 1
	call	MIOS_DOUT_SRSet
#endif
#endif

	;; set cathode line of current track
	;; if track active: cathode signal must be 0
	;; if track not active: cathode signal must be 1
	SET_BSR	SEQ_BASE
	movf	SEQ_BLM_ROW_CTR, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andlw	0x0f
	movwf	MIOS_PARAMETER1

	;; duplicate for button matrix
	swapf	WREG, W
	iorwf	MIOS_PARAMETER1, F

#if DEFAULT_SRM_DOUT_CATHODES1
	movlw	DEFAULT_SRM_DOUT_CATHODES1 - 1
	call	MIOS_DOUT_SRSet
#endif
#if DEFAULT_SRM_DOUT_CATHODES2
	movlw	DEFAULT_SRM_DOUT_CATHODES2 - 1
	call	MIOS_DOUT_SRSet
#endif

#endif


	;; ---[ update the BPM digits ]---
#if DEFAULT_BPM_DIGITS_ENABLED
	;; select common pin
	SET_BSR	SEQ_BASE
	movf	SEQ_BLM_ROW_CTR, W, BANKED
	xorlw	3			; (reverse digit select lines)
#if DEFAULT_BPM_DIGITS_ENABLED == 1
	call	MIOS_HLP_GetBitANDMask	; common cathode
#else
	call	MIOS_HLP_GetBitORMask	; common anode
#endif
	andlw	0x0f
	movwf	MIOS_PARAMETER1

	;; duplicate (just because the remaining pins are not used for any other purpose)
	swapf	WREG, W
	iorwf	MIOS_PARAMETER1, F

#if DEFAULT_BPM_DIGITS_COMMON
	movlw	DEFAULT_BPM_DIGITS_COMMON - 1
	call	MIOS_DOUT_SRSet
#endif

	;; set digit
	lfsr	FSR1, BPM_DIGIT_0
	movf	SEQ_BLM_ROW_CTR, W, BANKED
	movff	PLUSW1, MIOS_PARAMETER1
#if DEFAULT_BPM_DIGITS_ENABLED == 2
	comf	MIOS_PARAMETER1, F	; invert for common anode
#endif

#if DEFAULT_BPM_DIGITS_SEGMENTS
	movlw	DEFAULT_BPM_DIGITS_SEGMENTS - 1
	call	MIOS_DOUT_SRSet
#endif
	
#endif

	;; ---[ turn on the beat and step indicator LEDs on every beat ]---
	SET_BSR	SEQ_BASE
	clrf	IRQ_TMP1	; temporary used to indicate on/off state of beat
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_LED_UpdateIRQ_NoBeat
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	andlw	0x03
	bnz	SEQ_LED_UpdateIRQ_NoBeat
	movf	SEQ_CLK_SUBSTEP_CTR, W, BANKED
	andlw	0x03		; lit for 3/4 of each quarter note
	xorlw	0x03
	bz	SEQ_LED_UpdateIRQ_NoBeat
SEQ_LED_UpdateIRQ_Beat
	setf	IRQ_TMP1
SEQ_LED_UpdateIRQ_NoBeat
 IF LED_BEAT < 128
	;; forward to BEAT LED
	movff	IRQ_TMP1, MIOS_PARAMETER1
	movlw	LED_BEAT
	call	MIOS_DOUT_PinSet
 ENDIF

#if 1
	;; section selection LEDS
	movf	SEQ_SELECTED_ABCD, W, BANKED
#else
	;; section selection LEDS
	movf	SEQ_SELECTED_STEP_VIEW, W
	call	MIOS_HLP_GetBitORMask
#endif
	movwf	IRQ_TMP2

	movf	IRQ_TMP1, W
	bz	SEQ_LED_UpdateIRQ_NoStepBeat
SEQ_LED_UpdateIRQ_StepBeat
	;; current position still in IRQ_TMP3
	swapf	IRQ_TMP3, W
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	xorwf	IRQ_TMP2, F
SEQ_LED_UpdateIRQ_NoStepBeat

	;; section A LED
 IF LED_SECTION_A < 128
	clrf	MIOS_PARAMETER1
	btfsc	IRQ_TMP2, 0
	setf	MIOS_PARAMETER1
	movlw	LED_SECTION_A
	call	MIOS_DOUT_PinSet
 ENDIF

	;; section B LED
 IF LED_SECTION_B < 128
	clrf	MIOS_PARAMETER1
	btfsc	IRQ_TMP2, 1
	setf	MIOS_PARAMETER1
	movlw	LED_SECTION_B
	call	MIOS_DOUT_PinSet
 ENDIF

	;; section C LED
 IF LED_SECTION_C < 128
	clrf	MIOS_PARAMETER1
	btfsc	IRQ_TMP2, 2
	setf	MIOS_PARAMETER1
	movlw	LED_SECTION_C
	call	MIOS_DOUT_PinSet
 ENDIF

	;; section D LED
 IF LED_SECTION_D < 128
	clrf	MIOS_PARAMETER1
	btfsc	IRQ_TMP2, 3
	setf	MIOS_PARAMETER1
	movlw	LED_SECTION_D
	call	MIOS_DOUT_PinSet
 ENDIF

SEQ_LED_UpdateIRQ_End
	;; restore PROD[LH]
	movff	SEQ_IRQ_SAVED_PRODL, PRODL
	movff	SEQ_IRQ_SAVED_PRODH, PRODH
	return
