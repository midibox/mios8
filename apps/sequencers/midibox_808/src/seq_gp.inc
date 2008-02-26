; $Id$
;
; MIDIboxSEQ
; handler for general purpose LEDs and general purpose buttons
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  general purpose button function
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
SEQ_GP_Button
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; the ALT (menu select) function overlays all GP buttons
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_ALT, BANKED, SEQ_GP_Mode4_Button

	;; if liveplay mode active: branch to MIDI function
	BRA_IFSET SEQ_MODE2, SEQ_MODE2_LIVEPLAY, BANKED, SEQ_GP_Button_LivePlay

	;; following code is normaly located in cs_menu_buttons.inc
	;; check for GP buttons if hook has been installed
	call	CS_MENU_CheckMenuGPCallbackHook
	bz	SEQ_GP_Button_NoCS
SEQ_GP_Button_CS
	;; call the GP hook. 
	;; Parameter:
	;;    GP button number in MIOS_PARAMETER1: 0x00-0x0f
	;;    button value in MIOS_PARAMETER2 (forwarded)
	;;    encoder incrementer CS_MENU_ENC_INCREMENTER: always 0
	clrf	CS_MENU_ENC_INCREMENTER
	goto	CS_MENU_ExecMenuGPCallbackHook
SEQ_GP_Button_NoCS

	;; nothing to do...
	return


;; --------------------------------------------------------------------------
;;  this function is called from SEQ_LED_Update to update the LEDs
;;  IN: a lot of different sequencer settings
;;  OUT: the status of the 16 GP LEDs in SEQ_GP_LED_[LR]
;; --------------------------------------------------------------------------
SEQ_GP_LED_Update
	;; clear the 16 GP LEDs
	SET_BSR	SEQ_BASE
	clrf	SEQ_GP_LED_L, BANKED
	clrf	SEQ_GP_LED_R, BANKED

	;; ALT (menu select) overlays everything
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_ALT, BANKED, SEQ_GP_Mode4_LED

	;; if INST_OVERLAY_CTR > 0x00, display currently selected instrument
	;; (controlled by SEQ_ENC_Inst, counter decremented in cs_menu_timer.inc)
	;; additionally, the LED will be quickly flashed (flickerin) in SEQ_LED_UpdateIRQ_GPLED
	movf	CS_MENU_INST_OVERLAY_CTR, W
	bz	SEQ_GP_LED_Update_NoInst
SEQ_GP_LED_Update_Inst
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED, SEQ_GP_LED_Update_Inst_NotAll
SEQ_GP_LED_Update_Inst_All
	setf	SEQ_GP_LED_L, BANKED
	setf	SEQ_GP_LED_R, BANKED
	rgoto	SEQ_GP_LED_Update_Inst_Cont
SEQ_GP_LED_Update_Inst_NotAll

	movff	CS_MENU_INST_OVERLAY_DOUT_SR0, SEQ_GP_LED_L
	movff	CS_MENU_INST_OVERLAY_DOUT_SR1, SEQ_GP_LED_R
	rgoto	SEQ_GP_LED_Update_Inst_Cont
SEQ_GP_LED_Update_NoInst

	;; SEQ_GP_LED_[LR] hold the status of the GP buttons
	;; it will be transfered to the appr. DOUT registers on every SEQ_LED_UpdateIRQ cycle
	;; if MenuGPCallbackHook installed: show the GP status
	;; if MenuGPCallbackHook not installed: show play/song status depending on SEQ_MODE0
	call	CS_MENU_CheckMenuGPCallbackHook
	bz	SEQ_GP_LED_Update_NoHook

SEQ_GP_LED_Update_Hook
	SET_BSR	SEQ_BASE

	movf	CS_MENU_GP_OVERLAY_DOUT_SR0, W
	BRA_IFSET MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT, ACCESS, SEQ_GP_LED_Update_NoF1L
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH
	movlw	0x00
SEQ_GP_LED_Update_NoF1L
	movwf	SEQ_GP_LED_L, BANKED

	movf	CS_MENU_GP_OVERLAY_DOUT_SR1, W
	BRA_IFSET MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT, ACCESS, SEQ_GP_LED_Update_NoF1R
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH
	movlw	0x00
SEQ_GP_LED_Update_NoF1R
	movwf	SEQ_GP_LED_R, BANKED

SEQ_GP_LED_Update_Inst_Cont
#if DEFAULT_SRM_ENABLED
#if DEFAULT_GP_DOUT_SR_L == DEFAULT_SRM_DOUT_L1
	rcall	SEQ_GP_SRM_OverlayCopy
#else
	rcall	SEQ_GP_SRM_NormalCopy
#endif
#endif
	return

	;; ------------------------------------------------------------------
SEQ_GP_LED_Update_NoHook

#if DEFAULT_SRM_ENABLED
	;; update 4x16 LED matrix
	rcall	SEQ_GP_SRM_NormalCopy
#endif

	return


; ==========================================================================
; ==========================================================================
; ==========================================================================
; ==========================================================================


;; --------------------------------------------------------------------------
;;  LED/Button functions for Mode 4: SEQ_MODE0_MENU
;; --------------------------------------------------------------------------
SEQ_GP_Mode4_Button
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; clear screen(s)
	call	SEQ_LCD_Clear
	;; branch to the change menu function of the control surface
	movf	MIOS_PARAMETER1, W
	call	CS_MENU_TABLE_GPMapGetPos
	goto	CS_MENU_ChangePage

	;; ------------------------------------------------------------------
SEQ_GP_Mode4_LED
	;; transfer menu pos to SEQ_GP_LED_[LR]
	call	CS_MENU_TABLE_GPMapGetLED
	movff	MIOS_PARAMETER1, SEQ_GP_LED_L
	movff	MIOS_PARAMETER2, SEQ_GP_LED_R

#if DEFAULT_SRM_ENABLED
#if DEFAULT_GP_DOUT_SR_L == DEFAULT_SRM_DOUT_L1
	rcall	SEQ_GP_SRM_OverlayCopy
#else
	rcall	SEQ_GP_SRM_NormalCopy
#endif
#endif

	return


;; ---------------------------------------------------------------------------
;;  Help Function for SRM: copy SEQ_GP_LED_[LR] into TRK0_A_[LR], clear the rest
SEQ_GP_SRM_OverlayCopy
	;; clear LED arrays
	lfsr	FSR0, SEQ_LEDM_TRK0_A_L
	lfsr	FSR2, SEQ_LEDM_TRK0_B_L
	movlw	8
	movwf	PRODL
SEQ_GP_SRM_OverlayCopy_Loop
	clrf	POSTINC0
	clrf	POSTINC2
	decfsz	PRODL, F
	rgoto	SEQ_GP_SRM_OverlayCopy_Loop

	;; copy SEQ_GP_LED_[LR] to the first row
	lfsr	FSR0, SEQ_LEDM_TRK0_A_L
	movff	SEQ_GP_LED_L, POSTINC0
	movff	SEQ_GP_LED_R, POSTINC0
	return

;; ---------------------------------------------------------------------------
;;  Help Function for SRM: copy the trigger flags to the TRK0_A_[LR] array
SEQ_GP_SRM_NormalCopy
	;; Matrix LEDs (prepared for 4x16 LEDs)
	SET_BSR	SEQ_BASE

	;; align track offset to 2bit
	movf	SEQ_SELECTED_TRK, W, BANKED
	andlw	0x0c
	movwf	SEQ_EVNTT, BANKED

	;; get pointer to button trigger register depending on selected trigger layer
	call	CS_M_HLP_GetSelectedTrgLayerPtr	;  -> FSR1

	;; add step offset
	movf	SEQ_SELECTED_STEP_VIEW, W, BANKED
	rlf	WREG, W
	andlw	0x06
	addwf	FSR1L, F

	;; store pointer in TMP[34]
	movff	FSR1L, TMP3
	movff	FSR1H, TMP4

	;; using TMP2 as loop counter
	movlw	4
	movwf	TMP2

	;; prepare pointer to A/B colour
	lfsr	FSR0, SEQ_LEDM_TRK0_A_L
	lfsr	FSR2, SEQ_LEDM_TRK0_B_L

SEQ_GP_SRM_NormalCopy_Loop
	movff	TMP3, FSR1L
	movff	TMP4, FSR1H

	;; first colour
	movff	POSTINC1, POSTINC0
	movff	POSTDEC1, POSTINC0

#if 0
	;; disabled - doesn't look so cool
	;; second colour: inverted _A
	movf	POSTDEC0, W	; (decrements)
	movf	POSTDEC0, W

	comf	POSTINC0, W
	movwf	POSTINC2
	comf	POSTINC0, W
	movwf	POSTINC2
#endif

SEQ_GP_SRM_NormalCopy_Loop_Next
	;; switch to trigger layer of next track
	movlw	SEQ_TRGRECORD_LENGTH
	addwf	TMP3, F
	incf	SEQ_EVNTT, F, BANKED

	decfsz	TMP2, F
	rgoto	SEQ_GP_SRM_NormalCopy_Loop

	return


;; --------------------------------------------------------------------------
;;  for playing drums live with GP buttons
;;     o GP Button number in MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
SEQ_GP_Button_LivePlay
	SET_BSR	SEQ_BASE

	;; set new track depending on button number
	movf	MIOS_PARAMETER1, W
	call	CS_M_HLP_SetSelectedTrk

	;; deactivate "all" function (to avoid inconsistencies)
	bcf	SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED

	;; set also to SEQ_EVNTT
	movff	MIOS_PARAMETER1, SEQ_EVNTT

	;; prepare MIDI event
	call	SEQ_FSR_TrkChn			; MIDI channel -> MIDI_EVNT0
	movf	INDF1, W
	andlw	0x0f
	iorlw	0x90
	movwf	MIDI_EVNT0
	
	call	SEQ_FSR_TrkNote			; note number -> MIDI_EVNT1
	movff	INDF1, MIDI_EVNT1

	;; button depressed? only forward to record function (if record mode enabled)
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, SEQ_GP_Button_LivePlay_NoteOn

SEQ_GP_Button_LivePlay_NoteOff
	clrf	MIDI_EVNT_VALUE			; clear velocity value
	rgoto	SEQ_GP_Button_LivePlay_NoteOff_C; and continue here

SEQ_GP_Button_LivePlay_NoteOn
	;; set velocity to maximum
	movlw	0x7f
	movwf	MIDI_EVNT_VALUE

	movf	SEQ_EVNTT, W, BANKED		; request trigger
	call	SEQ_TRK_ReqTrigger
	call	SEQ_TRK_SetTriggers		; update all triggers
	call	SEQ_CORE_UpdateMeter		; update meter as well

SEQ_GP_Button_LivePlay_NoteOff_C

	;; play MIDI event if port enabled
	movf	SEQ_EVNTT, W, BANKED		; check if MIDI port enabled
	call	SEQ_TRK_GetMIDIPort
	addlw	-1
	movwf	MIDI_EVNT_PORT
	addlw	1
	bz	SEQ_GP_Button_LivePlay_NoMIDI
SEQ_GP_Button_LivePlay_MIDI
	call	MIDI_EVNT_Send
SEQ_GP_Button_LivePlay_NoMIDI
	
	;; forward to record function if enabled
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_MODE1, SEQ_MODE1_RECORD, BANKED, SEQ_GP_Button_LivePlay_End

	movff	MIDI_EVNT0, MIOS_PARAMETER1
	movff	MIDI_EVNT1, MIOS_PARAMETER2
	movff	MIDI_EVNT_VALUE, MIOS_PARAMETER3
	call	SEQ_RECORD_NotifyEvent
SEQ_GP_Button_LivePlay_End
	return
