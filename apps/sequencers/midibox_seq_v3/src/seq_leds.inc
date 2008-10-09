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

SEQ_LED_UPDATE_DOUT_INV MACRO register, flag, bank, dout
	setf	MIOS_PARAMETER1
	btfsc	register, flag, bank
	clrf	MIOS_PARAMETER1
	movlw	dout
	rcall	SEQ_LED_Update_PinSet
	ENDM

SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 MACRO dout		; note: this method consumes 4 bytes per LED assignment 
	movlw	dout					; thats the same amount like consumed by the previous table based approach
	rcall	SEQ_LED_Update_PinSet_ShiftTMP1
	ENDM

;; --------------------------------------------------------------------------
;;  this function is called by cs_menu.inc on every "display" update
;; --------------------------------------------------------------------------
SEQ_LED_Update
	;; update GP LEDs
	call	SEQ_GP_LED_Update

	SET_BSR	SEQ_BASE

	;; TMP[6:4] == A/B/C layers
	swapf	SEQ_SELECTED_LAYERS, W, BANKED
	andlw	0x70
	movwf	TMP1

	;; TMP1[3..0] == 1/2/3/4 LEDs
	;; branch depending on mode
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_MUTE, BANKED, SEQ_LED_Update_Mute
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED, SEQ_LED_Update_NoEdit
SEQ_LED_Update_Mute
SEQ_LED_Update_Edit
	;; 1/2/3/4 showing selected tracks
	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	movf	SEQ_SELECTED_GROUP, W, BANKED
	movf	PLUSW1, W
	andlw	0x0f
	iorwf	TMP1, F
	rgoto	SEQ_LED_Update_Edit_Cont

SEQ_LED_Update_NoEdit
SEQ_LED_Update_PatternSong
	;; in pattern/song mode: show cursor position
	movf	CS_MENU_CURSOR_POS, W
	btfsc	SEQ_MODE0, SEQ_MODE0_SONG, BANKED
	addlw	-3
	call	MIOS_HLP_GetBitORMask
	andlw	0x0f
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH	; (flash LED - makes the different LED/button assignment more induitive)
	movlw	0x00
	iorwf	TMP1, F

	;; in Song Mode: also overlay layer LEDs
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_SONG, BANKED, SEQ_LED_Update_Edit_Cont
	movf	CS_MENU_CURSOR_POS, W
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
	andlw	0x70
	btfsc	CS_STAT, CS_STAT_CURSOR_FLASH	; (flash LED - makes the different LED/button assignment more induitive)
	iorwf	TMP1, F
SEQ_LED_Update_Edit_Cont

	;; stream Tracks/Layers in TMP1[6:0]
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_TRACK1
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_TRACK2
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_TRACK3
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_TRACK4

	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_LAYER_A
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_LAYER_B
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_LAYER_C


	;; display flags of MODE0 register
	movff	SEQ_MODE0, TMP1

	;; extra: the song LED flashes if no song selected, and it is permanent on if song mode active
	btfsc	SEQ_STATE, SEQ_STATE_SONG, BANKED
	bsf	TMP1, SEQ_MODE0_SONG
	BRA_IFSET SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_LED_Update_SongLED_NoFl
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH
	bcf	TMP1, SEQ_MODE0_SONG
SEQ_LED_Update_SongLED_NoFl


	;; stream MODE0 flags in TMP1[7:0]
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_EDIT
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_MUTE
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_PATTERN
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_SONG
	rrf	TMP1, F		; (no LED for menu button... yet)
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_FAST
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_ALL
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_SOLO
	

	;; stream selected group 1-4 and trigger layer in TMP1[6:0]
	movf	SEQ_SELECTED_GROUP, W, BANKED
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1
	swapf	SEQ_SELECTED_TRG_LAYERS, W, BANKED
	andlw	0x70
	iorwf	TMP1, F
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_GROUP1
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_GROUP2
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_GROUP3
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_GROUP4
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_TRG_LAYER_A
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_TRG_LAYER_B
	SEQ_LED_UPDATE_DOUT_SHIFT_TMP1 LED_TRG_LAYER_C


	;; additional LED functions, which are not streamed via TMP1
 IF LED_PLAY < 192
	SEQ_LED_UPDATE_DOUT SEQ_STATE, SEQ_STATE_RUN, BANKED, LED_PLAY
 ENDIF
 IF LED_STOP < 192
	SEQ_LED_UPDATE_DOUT_INV SEQ_STATE, SEQ_STATE_RUN, BANKED, LED_STOP
 ENDIF
 IF LED_PAUSE < 192
	SEQ_LED_UPDATE_DOUT SEQ_STATE, SEQ_STATE_PAUSE, BANKED, LED_PAUSE
 ENDIF

 IF LED_STEP_1_16 < 192
	SEQ_LED_UPDATE_DOUT_INV SEQ_SELECTED_STEPS, 0, BANKED, LED_STEP_1_16
 ENDIF
 IF LED_STEP_17_32 < 192
	SEQ_LED_UPDATE_DOUT SEQ_SELECTED_STEPS, 0, BANKED, LED_STEP_17_32
 ENDIF

	return


	;; help function (rgoto only requires 2 bytes instead of 4 bytes)
	;; ignore shift register assignments with WREG==128
SEQ_LED_Update_PinSet
	;; ignore if WREG >= 192
	movwf	MIOS_PARAMETER2	; temporary store value in MIOS_PARAMETER2
	andlw	0xc0		; check for LED number >= 0xc0
	xorlw	0xc0		; ignore these numbers
	skpnz
	return

	;; if WREG >= 128, call BLM LED function
	movf	MIOS_PARAMETER2, W
	BRA_IFSET WREG, 7, ACCESS, SEQ_LED_BLM_PinSet

	;; otherwise call MIOS function
	goto	MIOS_DOUT_PinSet


	;; special variant which displays TMP1,0 and right-shifts TMP1 thereafter
SEQ_LED_Update_PinSet_ShiftTMP1
	clrf	MIOS_PARAMETER1
	btfsc	TMP1, 0
	setf	MIOS_PARAMETER1
	rrf	TMP1, F		; (right-shift for next execution of SEQ_LED_Update_PinSet_ShiftTMP1)
	rgoto	SEQ_LED_Update_PinSet


;; --------------------------------------------------------------------------
;;  LED Matrix for misc. buttons: 
;;  set a pin - parameter interface like for MIOS_DOUT_PinSet
;;     - WREG contains the LED number (0-63, 64-127 will be mirrored!)
;;     - MIOS_PARAMETER1 contains the LED value (0 or 1)
;; --------------------------------------------------------------------------
SEQ_LED_BLM_PinSet
        ;; branch depending on LED value
        lfsr    FSR1, SEQ_LEDM_MISC0
        BRA_IFCLR MIOS_PARAMETER1, 0, ACCESS, SEQ_LED_BLM_PinSet0

SEQ_LED_BLM_PinSet1
        movwf   MIOS_PARAMETER1         ; temporary store value in MIOS_PARAMETER1
        rlf     WREG, F
        swapf   WREG, W
        andlw   0x07                    ; 8 SRs maximum, >=8 will be mirrored
        addwf   FSR1L, F

        movf    MIOS_PARAMETER1, W
        call    MIOS_HLP_GetBitORMask
        iorwf   INDF1, F
        return

SEQ_LED_BLM_PinSet0
        movwf   MIOS_PARAMETER1         ; temporary store value in MIOS_PARAMETER1
        rlf     WREG, F
        swapf   WREG, W
        andlw   0x07                    ; 8 SRs maximum, >=8 will be mirrored
        addwf   FSR1L, F

        movf    MIOS_PARAMETER1, W
        call    MIOS_HLP_GetBitANDMask
        andwf   INDF1, F
        return


;; --------------------------------------------------------------------------
;;  this function is called by USER_SR_ServicePrepare before the DOUT chains
;;  will be loaded
;; --------------------------------------------------------------------------
SEQ_LED_UpdateIRQ
	;; we are using multiplications within a IRQ service handler - store PROD[LH]
	movff	PRODL, SEQ_IRQ_SAVED_PRODL
	movff	PRODH, SEQ_IRQ_SAVED_PRODH

	;; ---[ turn on the beat indicator LED on every beat ]---
	SET_BSR	SEQ_BASE
	clrf	MIOS_PARAMETER1
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_LED_UpdateIRQ_NoBeat
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	andlw	0x03
	bnz	SEQ_LED_UpdateIRQ_NoBeat
	movf	SEQ_CLK_SUBSTEP_CTR, W, BANKED
	andlw	0x03		; lit for 3/4 of each quarter note
	xorlw	0x03
	bz	SEQ_LED_UpdateIRQ_NoBeat
SEQ_LED_UpdateIRQ_Beat
	setf	MIOS_PARAMETER1
SEQ_LED_UpdateIRQ_NoBeat
	movlw	LED_BEAT
	rcall	SEQ_LED_Update_PinSet	

	;; ---[ increment the BLM row counter (used when DEFAULT_SRM_ENABLED or DEFAULT_BPM_DIGITS_ENABLED set ]---
	incf	SEQ_BLM_ROW_CTR, F, BANKED
	bcf	SEQ_BLM_ROW_CTR, 3, BANKED	; wrap at 8

	;; ---[ update the GP LEDs ]---

	;; store selected track in IRQ_TMP5[3:0]
	call	CS_M_HLP_GetSelectedTrk
	movwf	IRQ_TMP5

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

	;; don't print if played step not within view (1-16/17-31)
	BRA_IFSET SEQ_SELECTED_STEPS, 0, BANKED, SEQ_LED_UpdateIRQ_SeqOn_GPLED_S1
SEQ_LED_UpdateIRQ_SeqOn_GPLED_S0
	BRA_IFSET IRQ_TMP3, 4, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_GPLED_NM
	rgoto	SEQ_LED_UpdateIRQ_SeqOn_GPLED_C

SEQ_LED_UpdateIRQ_SeqOn_GPLED_S1
	BRA_IFCLR IRQ_TMP3, 4, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_GPLED_NM
	;; 	rgoto	SEQ_LED_UpdateIRQ_SeqOn_GPLED_C

SEQ_LED_UpdateIRQ_SeqOn_GPLED_C
	;; check if handshake disable set (which means: track not played due to handshaking)
	movlw	SEQ_TRKVARSTATE2x
	BRA_IFSET PLUSW1, SEQ_TRKVARSTATE2_HNDSHK_DIS, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_GPLED_NM

	;; mark position
	bsf	IRQ_TMP5, 4
SEQ_LED_UpdateIRQ_SeqOn_GPLED_NM


#if DEFAULT_SRM_ENABLED
	;; check for matrix LEDs
SEQ_LED_UpdateIRQ_SeqOn_LM
	;; get matrix seq pos, store it in IRQ_TMP4
	lfsr	FSR1, SEQ_TRKVAR0
	movf	SEQ_BLM_ROW_CTR, W, BANKED
	andlw	0x03
	movwf	PRODL
	movf	IRQ_TMP5, W	; (selected track group = IRQ_TMP5[3:2])
	andlw	0x0c
	addwf	PRODL, W
	mullw	SEQ_TRKVARRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	movlw	SEQ_TRKVARSTEPx
	movff	PLUSW1, IRQ_TMP4

	;; don't print if played step not within view (1-16/17-31)
	BRA_IFSET SEQ_SELECTED_STEPS, 0, BANKED, SEQ_LED_UpdateIRQ_SeqOn_LM_S1
SEQ_LED_UpdateIRQ_SeqOn_LM_S0
	BRA_IFSET IRQ_TMP4, 4, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_LM_NM
	rgoto	SEQ_LED_UpdateIRQ_SeqOn_LM_C

SEQ_LED_UpdateIRQ_SeqOn_LM_S1
	BRA_IFCLR IRQ_TMP4, 4, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_LM_NM
	;; 	rgoto	SEQ_LED_UpdateIRQ_SeqOn_LM_C

SEQ_LED_UpdateIRQ_SeqOn_LM_C
	;; check if handshake disable set (which means: track not played due to handshaking)
	movlw	SEQ_TRKVARSTATE2x
	BRA_IFSET PLUSW1, SEQ_TRKVARSTATE2_HNDSHK_DIS, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_LM_NM

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
	andlw	0x06
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


#if DEFAULT_SRM_DOUT_M
	;; misc. LED functions
	lfsr	FSR1, SEQ_LEDM_MISC0
	movf	SEQ_BLM_ROW_CTR, W, BANKED
	movff	PLUSW1, MIOS_PARAMETER1
	movlw	DEFAULT_SRM_DOUT_M - 1
	call	MIOS_DOUT_SRSet
#endif


#if DEFAULT_SRM_DOUT_DUOCOLOUR
	;; drive the second colour LEDs
	lfsr	FSR1, SEQ_LEDM_TRK0_B_L
	rlf	SEQ_BLM_ROW_CTR, W, BANKED
	andlw	0x06
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

	;; set cathode line for GP Button/LED matrix
	;; if track active: cathode signal must be 0
	;; if track not active: cathode signal must be 1
	;; this can be optionally inverted if sink drivers are used
	SET_BSR	SEQ_BASE
	movf	SEQ_BLM_ROW_CTR, W, BANKED
	andlw	0x03
	call	MIOS_HLP_GetBitANDMask
	andlw	0x0f
	movwf	MIOS_PARAMETER1

	;; duplicate for button matrix
	swapf	WREG, W
	iorwf	MIOS_PARAMETER1, W

        ;; finally apply inversion mask (required when sink drivers are connected to cathode lines)
        xorlw   DEFAULT_SRM_CATHODES_INV_MASK
        movwf   MIOS_PARAMETER1

#if DEFAULT_SRM_DOUT_CATHODES1
	movlw	DEFAULT_SRM_DOUT_CATHODES1 - 1
	call	MIOS_DOUT_SRSet
#endif
#if DEFAULT_SRM_DOUT_CATHODES2
	movlw	DEFAULT_SRM_DOUT_CATHODES2 - 1
	call	MIOS_DOUT_SRSet
#endif


#if DEFAULT_SRM_DOUT_CATHODESM
	;; set cathode line for misc. button/LED functions
	movf	SEQ_BLM_ROW_CTR, W, BANKED
	call	MIOS_HLP_GetBitANDMask

        ;; apply inversion mask (required when sink drivers are connected to cathode lines)
        xorlw   DEFAULT_SRM_CATHODES_INV_MASK_M
        movwf   MIOS_PARAMETER1

	movlw	DEFAULT_SRM_DOUT_CATHODESM - 1
	call	MIOS_DOUT_SRSet
#endif

#endif


	;; ---[ update the BPM digits ]---
#if DEFAULT_BPM_DIGITS_ENABLED
	;; select common pin
	SET_BSR	SEQ_BASE
	movf	SEQ_BLM_ROW_CTR, W, BANKED
	andlw	0x03
	xorlw	0x03			; (reverse digit select lines)
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
	andlw	0x03
	movff	PLUSW1, MIOS_PARAMETER1
#if DEFAULT_BPM_DIGITS_ENABLED == 2
	comf	MIOS_PARAMETER1, F	; invert for common anode
#endif

#if DEFAULT_BPM_DIGITS_SEGMENTS
	movlw	DEFAULT_BPM_DIGITS_SEGMENTS - 1
	call	MIOS_DOUT_SRSet
#endif
	
#endif

SEQ_LED_UpdateIRQ_End
	;; restore PROD[LH]
	movff	SEQ_IRQ_SAVED_PRODL, PRODL
	movff	SEQ_IRQ_SAVED_PRODH, PRODH
	return
