; $Id$
;
; MIDIboxSEQ
; Button handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This handler has to be called by USER_DIN_NotifyToggle when a button
;;  has been pressed/depressed
;;  It goes through the SEQ_IO_TABLE_DIN in "mios_tables.inc",
;;  searches for a matching DIN pin and calls the appr. function
;; 
;;  Input:
;;     o Button number in MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
SEQ_BUTTON_Handler

	;; check for general purpose button
SEQ_BUTTON_Handler_ChkGP1
	movf	MIOS_PARAMETER1, W
	andlw	0xf8
	xorlw	(DEFAULT_GP_DIN_SR0 - 1) << 3
	bnz	SEQ_BUTTON_Handler_ChkGP2
	movlw	0x07
	andwf	MIOS_PARAMETER1, F
	rgoto	SEQ_BUTTON_GP

SEQ_BUTTON_Handler_ChkGP2
	movf	MIOS_PARAMETER1, W
	andlw	0xf8
	xorlw	(DEFAULT_GP_DIN_SR1 - 1) << 3
	bnz	CS_MENU_BUTTON_Handler_NoGP
	movlw	0x07
	andwf	MIOS_PARAMETER1, F
	bsf	MIOS_PARAMETER1, 3
	rgoto	SEQ_BUTTON_GP
CS_MENU_BUTTON_Handler_NoGP


	;; go through the DIN map
	TABLE_ADDR SEQ_IO_TABLE_DIN
SEQ_BUTTON_Handler_Loop
	tblrd*+				; get low-byte of function pointer
	movf	TABLAT, W		; load it in WREG
	movwf	TMP1			; and store it in TMP1
	tblrd*+				; get high byte
	movff	TABLAT, TMP2		; store it in TMP2 without touching WREG
	iorwf	TABLAT, W		; or the byte with WREG --- if both bytes are zero, the result will be zero
	bz	SEQ_BUTTON_Handler_End	; in this case jump to the end

	tblrd*+				; get pin-number
	movf	TABLAT, W
	tblrd*+				; and read dummy entry
	;; pin number still in WREG, compare it with MIOS_PARAMETER1
	xorwf	MIOS_PARAMETER1, W
	bnz	SEQ_BUTTON_Handler_Loop	; next loop if not equal

	;; else: matching pin has been found
	;; stop messages if button pressed
	CALL_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_MENU_StopMessage
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; jump indirectly to the function (pointer in TMP1/TMP2)
	IRQ_DISABLE		; (IRQs have to be disabled when TOSL/TOSH is modified)
	push
	movf	TMP1, W
	movwf	TOSL
	movf	TMP2, W
	movwf	TOSH
	IRQ_ENABLE
	return			; this return leads to a jump to the function stored in TOSL/TOSH

SEQ_BUTTON_Handler_End
	;; no matching pin found, exit handler
	return


;; --------------------------------------------------------------------------
;;  general purpose button function
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
SEQ_BUTTON_GP
	;; branch to the appr. SEQ_GP_Button function
	goto	SEQ_GP_Button

	;; used by seq_remote.inc
SEQ_BUTTON_GP1
	movlw	0x00
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP2
	movlw	0x01
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP3
	movlw	0x02
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP4
	movlw	0x03
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP5
	movlw	0x04
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP6
	movlw	0x05
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP7
	movlw	0x06
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP8
	movlw	0x07
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP9
	movlw	0x08
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP10
	movlw	0x09
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP11
	movlw	0x0a
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP12
	movlw	0x0b
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP13
	movlw	0x0c
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP14
	movlw	0x0d
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP15
	movlw	0x0e
	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP16
	movlw	0x0f
	;; 	rgoto	SEQ_BUTTON_GP_Cont

SEQ_BUTTON_GP_Cont
	movwf	MIOS_PARAMETER1
	goto	SEQ_GP_Button

;; --------------------------------------------------------------------------
;;  The CS menu functions
;; --------------------------------------------------------------------------
SEQ_BUTTON_Select
	goto	CS_MENU_BUTTON_Select
SEQ_BUTTON_Exit
	goto	CS_MENU_BUTTON_Exit
SEQ_BUTTON_Left
	goto	CS_MENU_BUTTON_Left
SEQ_BUTTON_LeftRemote
	goto	CS_MENU_BUTTON_LeftRemote
SEQ_BUTTON_Right
	goto	CS_MENU_BUTTON_Right
SEQ_BUTTON_RightRemote
	goto	CS_MENU_BUTTON_RightRemote

;; --------------------------------------------------------------------------
;;  Start/Stop button function
;; --------------------------------------------------------------------------
SEQ_BUTTON_StartStop
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_BUTTON_StartStop_Stop
BUTTON_Control4SetStart
	movlw	(1 << SEQ_REQ_START)
	movwf	SEQ_REQ, BANKED
	return
SEQ_BUTTON_StartStop_Stop
	movlw	(1 << SEQ_REQ_STOP)
	movwf	SEQ_REQ, BANKED
	return

;; --------------------------------------------------------------------------
;;  Pause button function
;; --------------------------------------------------------------------------
SEQ_BUTTON_Pause
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; toggle pause flag
	goto	SEQ_CORE_Pause
	

;; --------------------------------------------------------------------------
;;  Rew button function
;; --------------------------------------------------------------------------
SEQ_BUTTON_Rew
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; special feature: branch to "decrement value" function if not in song mode
	;; this allows to use this button for modifying the menu value
	;; alternatively to the rotary encoder
	SET_BSR	SEQ_BASE
	GOTO_IFCLR SEQ_STATE, SEQ_STATE_SONG, BANKED, CS_MENU_BUTTON_Left

	;; in songmode: do nothing if sequencer not running
	btfss	SEQ_STATE, SEQ_STATE_RUN, BANKED
	return

	;; previous song position
	goto	SEQ_SONG_Rew

;; --------------------------------------------------------------------------
;;  Fwd button function
;; --------------------------------------------------------------------------
SEQ_BUTTON_Fwd
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; special feature: branch to "increment value" function if not in song mode
	;; this allows to use this button for modifying the menu value
	;; alternatively to the rotary encoder
	SET_BSR	SEQ_BASE
	GOTO_IFCLR SEQ_STATE, SEQ_STATE_SONG, BANKED, CS_MENU_BUTTON_Right

	;; in songmode: do nothing if sequencer not running
	btfss	SEQ_STATE, SEQ_STATE_RUN, BANKED
	return

	;; next song position
	goto	SEQ_SONG_Fwd


;; --------------------------------------------------------------------------
;;  Play button function
;; --------------------------------------------------------------------------
SEQ_BUTTON_Play
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; restart sequencer
	SET_BSR	SEQ_BASE
	movlw	(1 << SEQ_REQ_START)
	movwf	SEQ_REQ, BANKED
	return

;; --------------------------------------------------------------------------
;;  Stop button function
;; --------------------------------------------------------------------------
SEQ_BUTTON_Stop
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; if sequencer already stopped, reset song position
	SET_BSR	SEQ_BASE
	CALL_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Reset

	;; request stop (again)
	SET_BSR	SEQ_BASE
	movlw	(1 << SEQ_REQ_STOP)
	movwf	SEQ_REQ, BANKED
	return

;; --------------------------------------------------------------------------
;;  Continue button function
;; --------------------------------------------------------------------------
SEQ_BUTTON_Continue
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	SET_BSR	SEQ_BASE
	movlw	(1 << SEQ_REQ_CONT)
	movwf	SEQ_REQ, BANKED
	return

;; --------------------------------------------------------------------------
;;  enter edit mode
;; --------------------------------------------------------------------------
SEQ_BUTTON_Edit
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch to view menu
	movlw	CS_MENU_PAGE_VIEW
	goto	CS_M_HLP_ChangePage

;; --------------------------------------------------------------------------
;;  enter mute page
;; --------------------------------------------------------------------------
SEQ_BUTTON_Mute
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch to mute menu
	movlw	CS_MENU_PAGE_MUTE
	goto	CS_M_HLP_ChangePage

;; --------------------------------------------------------------------------
;;  enter pattern mode
;; --------------------------------------------------------------------------
SEQ_BUTTON_Pattern
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch to mute menu
	movlw	CS_MENU_PAGE_PATTERN
	goto	CS_M_HLP_ChangePage

;; --------------------------------------------------------------------------
;;  toggle song mode
;; --------------------------------------------------------------------------
SEQ_BUTTON_Song
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; toggle song mode if we are already in SONG menu
	movlw	CS_MENU_PAGE_SONG
	cpfseq	CS_MENU_POS, ACCESS
	rgoto SEQ_BUTTON_Song_NoToggle
SEQ_BUTTON_Song_Toggle
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_SONG, 7, BANKED, SEQ_BUTTON_Song_Toggle_0

SEQ_BUTTON_Song_Toggle_1
	bsf	SEQ_SONG, 7, BANKED
	call	SEQ_SONG_ExitSongMode
	rgoto	SEQ_BUTTON_Song_Toggle_Cont

SEQ_BUTTON_Song_Toggle_0
	bcf	SEQ_SONG, 7, BANKED
	call	SEQ_SONG_EnterSongMode
	;; 	rgoto	SEQ_BUTTON_Song_Toggle_Cont

SEQ_BUTTON_Song_Toggle_Cont
SEQ_BUTTON_Song_NoToggle

	;; branch to mute menu
	movlw	CS_MENU_PAGE_SONG
	goto	CS_M_HLP_ChangePage

;; --------------------------------------------------------------------------
;;  set menu mode with button
;; --------------------------------------------------------------------------
SEQ_BUTTON_Menu
	;; enter menu mode
	SET_BSR	SEQ_BASE
	bcf	SEQ_MODE0, SEQ_MODE0_MENU, BANKED
	btfss	MIOS_PARAMETER2, 0
	bsf	SEQ_MODE0, SEQ_MODE0_MENU, BANKED

	return


;; --------------------------------------------------------------------------
;;  Enable/Disable Metronome
;; --------------------------------------------------------------------------
SEQ_BUTTON_Metronome
	SET_BSR	SEQ_BASE
#if DEFAULT_BEHAVIOUR_BUTTON_METRON
	;; Toggle Mode (default):
	;; exit if button depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else toggle status
	btg	SEQ_MODE1, SEQ_MODE1_METRONOME, BANKED
#else
	;; On/Off Mode:
	;; button status controls mode directly
	bcf	SEQ_MODE1, SEQ_MODE1_METRONOME, BANKED
	btfss	MIOS_PARAMETER2, 0
	bsf	SEQ_MODE1, SEQ_MODE1_METRONOME, BANKED
#endif
	return

;; --------------------------------------------------------------------------
;;  Enable/Disable Scrub Mode
;; --------------------------------------------------------------------------
SEQ_BUTTON_Scrub
	SET_BSR	SEQ_BASE
#if DEFAULT_BEHAVIOUR_BUTTON_SCRUB
	;; Toggle Mode (default):
	;; exit if button depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else toggle status
	btg	SEQ_MODE1, SEQ_MODE1_SCRUB, BANKED
#else
	;; On/Off Mode:
	;; button status controls mode directly
	bcf	SEQ_MODE1, SEQ_MODE1_SCRUB, BANKED
	btfss	MIOS_PARAMETER2, 0
	bsf	SEQ_MODE1, SEQ_MODE1_SCRUB, BANKED
#endif
	return

;; --------------------------------------------------------------------------
;;  Play current track solo
;; --------------------------------------------------------------------------
SEQ_BUTTON_Solo
	SET_BSR	SEQ_BASE
#if DEFAULT_BEHAVIOUR_BUTTON_SOLO
	;; Toggle Mode (default):
	;; exit if button depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else toggle status
	btg	SEQ_MODE0, SEQ_MODE0_SOLO, BANKED
#else
	;; On/Off Mode:
	;; button status controls mode directly
	bcf	SEQ_MODE0, SEQ_MODE0_SOLO, BANKED
	btfss	MIOS_PARAMETER2, 0
	bsf	SEQ_MODE0, SEQ_MODE0_SOLO, BANKED
#endif
	return

;; --------------------------------------------------------------------------
;;  Enable to modify all selected steps
;; --------------------------------------------------------------------------
SEQ_BUTTON_All
	SET_BSR	SEQ_BASE
#if DEFAULT_BEHAVIOUR_BUTTON_ALL
	;; Toggle Mode (default):
	;; exit if button depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else toggle status
	btg	SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED
#else
	;; On/Off Mode:
	;; button status controls mode directly
	bcf	SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED
	btfss	MIOS_PARAMETER2, 0
	bsf	SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED
#endif
	return


;; --------------------------------------------------------------------------
;;  Enc Fast Speed Button
;;  USES: TMP[12]
;; --------------------------------------------------------------------------
SEQ_BUTTON_Fast
	SET_BSR	SEQ_BASE
#if DEFAULT_BEHAVIOUR_BUTTON_FAST
	;; Toggle Mode (default):
	;; exit if button depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else toggle status
	btg	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
#else
	;; On/Off Mode:
	;; button status controls fast mode directly
	bcf	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	btfss	MIOS_PARAMETER2, 0
	bsf	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
#endif

	;; data wheel:
#if DEFAULT_ENC_DATAWHEEL >= 0
	;; speed divider
	movlw	DEFAULT_DATAWHEEL_SPEED_VALUE
	btfss	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	movlw 0x00
	movwf	MIOS_PARAMETER2

	;; speed mode
	movlw	MIOS_ENC_SPEED_FAST
	btfss	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	movlw MIOS_ENC_SPEED_NORMAL
	movwf	MIOS_PARAMETER1

	;; encoder number - set speed
	movlw	DEFAULT_ENC_DATAWHEEL
	call	MIOS_ENC_SpeedSet
#endif

	;; the remaining rotary encoders
	movlw	0x01
	movwf	TMP1
SEQ_BUTTON_Fast_Loop
	;; speed divider
	movlw	DEFAULT_ENC_SPEED_VALUE		; button pressed
	btfss	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	movlw 0x00
	movwf	MIOS_PARAMETER2

	;; speed mode
	movlw	MIOS_ENC_SPEED_FAST
	btfss	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	movlw MIOS_ENC_SPEED_NORMAL
	movwf	MIOS_PARAMETER1

	;; encoder number - set speed
	movf	TMP1, W
	call	MIOS_ENC_SpeedSet
	incf	TMP1, F
	movlw	17
	cpfseq	TMP1, ACCESS
	rgoto SEQ_BUTTON_Fast_Loop
	return


;; --------------------------------------------------------------------------
;;  Select Layer A/B/C
;; --------------------------------------------------------------------------
SEQ_BUTTON_LayerA
	movlw	0
	rgoto	SEQ_BUTTON_Layerx

SEQ_BUTTON_LayerB
	movlw	1
	rgoto	SEQ_BUTTON_Layerx

SEQ_BUTTON_LayerC
	movlw	2
	;; 	rgoto	SEQ_BUTTON_Layerx

SEQ_BUTTON_Layerx
	movwf	MIOS_PARAMETER1

	SET_BSR	SEQ_BASE

	;; alternative function if not in edit mode
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED, SEQ_BUTTON_Layerx_Edit
SEQ_BUTTON_Layerx_MPS
	;; do nothing in mute and pattern mode
	btfsc	SEQ_MODE0, SEQ_MODE0_MUTE, BANKED
	return
	btfsc	SEQ_MODE0, SEQ_MODE0_PATTERN, BANKED
	return
	;; do nothing when button has been released
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else (in song mode) set cursor pos
	movf	MIOS_PARAMETER1, W
	andlw	0x03
	movwf	CS_MENU_CURSOR_POS

	;; dirty quickfix: in song mode: save current entry before changing to new cursor pos
	call	CS_M_SONG_WriteValueChk

	movf	CS_MENU_CURSOR_POS, W
	goto	CS_MENU_NewCursorPos
	

SEQ_BUTTON_Layerx_Edit
	;; branch if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_BUTTON_Layer_Depr

	;; decode button number
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	andlw	0x0f

	;; save value in "SEQ_SELECTED_LAYERS"
	movwf	SEQ_SELECTED_LAYERS, BANKED
	;; trigger pot display page and hold it
	goto	SEQ_MSG_Hold

SEQ_BUTTON_Layer_Depr
	;; trigger pot display page
	goto	SEQ_MSG_Start


;; --------------------------------------------------------------------------
;;  Select Track 1/2/3/4
;; --------------------------------------------------------------------------
SEQ_BUTTON_Track1
	movlw	0
	rgoto	SEQ_BUTTON_Trackx_Cont
SEQ_BUTTON_Track2
	movlw	1
	rgoto	SEQ_BUTTON_Trackx_Cont
SEQ_BUTTON_Track3
	movlw	2
	rgoto	SEQ_BUTTON_Trackx_Cont
SEQ_BUTTON_Track4
	movlw	3
	;; 	rgoto	SEQ_BUTTON_Trackx_Cont

SEQ_BUTTON_Trackx_Cont
	movwf	MIOS_PARAMETER1

	SET_BSR	SEQ_BASE

	;; alternative function if not in edit mode
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED, SEQ_BUTTON_Trackx_Edit
SEQ_BUTTON_Trackx_MPS
	;; do nothing in mute mode
	btfsc	SEQ_MODE0, SEQ_MODE0_MUTE, BANKED
	return
	;; do nothing when button has been released
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else set cursor pos

	;; dirty quickfix: in song mode: save current entry before changing to new cursor pos
	CALL_IFSET SEQ_MODE0, SEQ_MODE0_SONG, BANKED, CS_M_SONG_WriteValueChk

	movf	MIOS_PARAMETER1, W
	andlw	0x03
	btfsc	SEQ_MODE0, SEQ_MODE0_SONG, BANKED
	addlw 3
	movwf	CS_MENU_CURSOR_POS
	movf	CS_MENU_CURSOR_POS, W
	goto	CS_MENU_NewCursorPos


SEQ_BUTTON_Trackx_Edit
	;; decode button number
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	andlw	0x0f
	movwf	MIOS_PARAMETER1

	;; only save button state if button has been depressed and exit
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_BUTTON_Trackx_Hlp_Sav

	;; else branch depending on state of all 4 select buttons:
	;; if only one select button pressed: radio-button function (1 of 4)
	;; if more than one select button pressed: toggle function (4 of 4)
	movf	SEQ_SELECTED_TRKS, W, BANKED
	andlw	0xf0
	xorlw	0xf0
	bz	SEQ_BUTTON_Trackx_Radio
SEQ_BUTTON_Trackx_Toggle
	;; toggle track flag
	movf	MIOS_PARAMETER1, W
	xorwf	SEQ_SELECTED_TRKS, F, BANKED
 	rgoto	SEQ_BUTTON_Trackx_End
SEQ_BUTTON_Trackx_Radio
	;; select 1 of 4
	movlw	0xf0
	andwf	SEQ_SELECTED_TRKS, F, BANKED
	movf	MIOS_PARAMETER1, W
	iorwf	SEQ_SELECTED_TRKS, F, BANKED
	;; 	rgoto	CS_MENU_BUTTON_SID_End
	
SEQ_BUTTON_Trackx_End
	;; request display reinitialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; save button state and exit

SEQ_BUTTON_Trackx_Hlp_Sav
	;; mask out old button status
	swapf	MIOS_PARAMETER1, W
	xorlw	0xff
	andwf	SEQ_SELECTED_TRKS, F, BANKED

	;; store new button status (pressed/depressed) in SEQ_SELECTED_TRKS[7..4]
	swapf	MIOS_PARAMETER1, W
	btfsc	MIOS_PARAMETER2, 0
	iorwf	SEQ_SELECTED_TRKS, F, BANKED
	return


;; --------------------------------------------------------------------------
;;  F1 button --- implement your favourite function here!
;; --------------------------------------------------------------------------
SEQ_BUTTON_F1
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch to save menu
	movlw	CS_MENU_PAGE_SAVE
	goto	CS_M_HLP_ChangePage

;; --------------------------------------------------------------------------
;;  F2 button --- implement your favourite function here!
;; --------------------------------------------------------------------------
SEQ_BUTTON_F2
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch to record menu
	movlw	CS_MENU_PAGE_RECORD
	goto	CS_M_HLP_ChangePage

;; --------------------------------------------------------------------------
;;  F3 button --- implement your favourite function here!
;; --------------------------------------------------------------------------
SEQ_BUTTON_F3
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; unmute all tracks
	SET_BSR	SEQ_BASE
	clrf	SEQ_TRKS_MUTED0, BANKED
	clrf	SEQ_TRKS_MUTED1, BANKED

	;; and branch to mute menu
	rgoto	SEQ_BUTTON_Mute

;; --------------------------------------------------------------------------
;;  F4 button --- implement your favourite function here!
;; --------------------------------------------------------------------------
SEQ_BUTTON_F4
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; mute all tracks
	SET_BSR	SEQ_BASE
	setf	SEQ_TRKS_MUTED0, BANKED
	setf	SEQ_TRKS_MUTED1, BANKED

	;; and branch to mute menu
	rgoto	SEQ_BUTTON_Mute
