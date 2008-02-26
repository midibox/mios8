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
	;; if button has been pressed...
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_BUTTON_Handler_NoDCChk
SEQ_BUTTON_Handler_DCChk
	;; clear doubleclick counter if current DIN != last DIN
	movf	CS_MENU_DOUBLECLICK_LAST_DIN, W
	xorwf	MIOS_PARAMETER1, W
	skpz
	clrf	CS_MENU_DOUBLECLICK_CTR

	movff	MIOS_PARAMETER1, CS_MENU_DOUBLECLICK_LAST_DIN
SEQ_BUTTON_Handler_NoDCChk

	;; check for general purpose button
SEQ_BUTTON_Handler_ChkGP1
	movf	MIOS_PARAMETER1, W
	andlw	0xf8
	xorlw	(DEFAULT_GP_DIN_SR_L - 1) << 3
	bnz	SEQ_BUTTON_Handler_ChkGP2
	movlw	0x07
	andwf	MIOS_PARAMETER1, F
	call	SEQ_GP_Button
	rgoto	SEQ_BUTTON_Handler_End

SEQ_BUTTON_Handler_ChkGP2
	movf	MIOS_PARAMETER1, W
	andlw	0xf8
	xorlw	(DEFAULT_GP_DIN_SR_R - 1) << 3
	bnz	CS_MENU_BUTTON_Handler_NoGP
	movlw	0x07
	andwf	MIOS_PARAMETER1, F
	bsf	MIOS_PARAMETER1, 3
	call	SEQ_GP_Button
	rgoto	SEQ_BUTTON_Handler_End

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
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	rcall	SEQ_BUTTON_Handler_Exec
	rgoto	SEQ_BUTTON_Handler_End

	;; jump indirectly to the function (pointer in TMP1/TMP2)
SEQ_BUTTON_Handler_Exec
	IRQ_DISABLE		; (IRQs have to be disabled when TOSL/TOSH is modified)
	push
	movf	TMP1, W
	movwf	TOSL
	movf	TMP2, W
	movwf	TOSH
	IRQ_ENABLE
	return			; this return leads to a jump to the function stored in TOSL/TOSH

SEQ_BUTTON_Handler_End
	;; re-arm doubleclick counter if current DIN != last DIN

	;; button routines can determine a doubleclick by checking if this counter is > 0
	;; it will be decremented within cs_menu_timer.inc
	setf	CS_MENU_DOUBLECLICK_CTR
	return


;; --------------------------------------------------------------------------
;;  Button Matrix: this function gets the DIN values of the selected column
;;  It should be called from USER_SR_Service_Finish
;; --------------------------------------------------------------------------
SEQ_BUTTON_GetRowIRQ
	SET_BSR	SEQ_BASE

	;; since the row line of the buttons is identical to the row line of the LEDs,
	;; we can derive the button row offset from SEQ_BLM_ROW_CTR

	;; store row*2 offset in IRQ_TMP1
	decf	SEQ_BLM_ROW_CTR, W, BANKED
	rlf	WREG, W
	andlw	0x06
	movwf	IRQ_TMP1

	;; check the left half
	movlw	DEFAULT_SRM_DIN_L - 1
	rcall	SEQ_BUTTON_GetRowIRQ_Sub

	;; check the right half
	incf	IRQ_TMP1, F	; row*2+1
	movlw	DEFAULT_SRM_DIN_R - 1
	;; 	rgoto	SEQ_BUTTON_GetRowIRQ_Sub


	;; expecting: DIN SR number in WREG
	;;            row counter in IRQ_TMP1
SEQ_BUTTON_GetRowIRQ_Sub
	;; determine changes and save new value in ROWx_VALUE depending on selected column
	call	MIOS_DIN_SRGet	; result in MIOS_PARAMETER1
	
	;; ---[ BEGIN DIRTY CODE ]---
	movlw	0x10		; clear the MIOS_SR_DIN_CHANGED_x register to avoid, that USER_DIN_NotifyToggle
	addwf	FSR1L, F	; will be called - we have our own toggle handler
	clrf	INDF1		; Direct accesses to MIOS registers are normaly not allowed, I do this here since
				; I know that the register address offset of the DIN_CHANGED_x register will
				; never be changed in future
				; REMINDER for later MIOS versions: provide a function to access the DIN_CHANGED_x register
	;; ---[ END DIRTY CODE ]---

	;; backup old values in IRQ_TMP2
	;; store new values in SEQ_BUTTONM_ROW0L_VALUES + IRQ_TMP1
	lfsr	FSR1, SEQ_BUTTONM_ROW0L_VALUES
	movf	IRQ_TMP1, W
	movff	PLUSW1, IRQ_TMP2
	movff	MIOS_PARAMETER1, PLUSW1

	;; old value in IRQ_TMP2
	;; new value in MIOS_PARAMETER1
	;; select ROWx_CHANGED register
	lfsr	FSR1, SEQ_BUTTONM_ROW0L_CHANGED
	movf	IRQ_TMP1, W
	addwf	FSR1L, F

	;; changes are: IRQ_TMP2 xor MIOS_PARAMETER1
	movf	IRQ_TMP2, W
	xorwf	MIOS_PARAMETER1, W

	;; OR the result to the already notified changes (if they have not been zeroed by SEQ_BUTTON_SRM_Handler)
	iorwf	INDF1, F

	return


;; --------------------------------------------------------------------------
;;  Button Matrix: this function should be called from USER_Tick (in the
;;  main program) to determine for button changes. Events (change from 0->1 or
;;  from 1->0) will be notified to the SEQ_BUTTON_GP function, the track
;;  will be changed according to the selected row
;; --------------------------------------------------------------------------
SEQ_BUTTON_SRM_Handler
	SET_BSR	SEQ_BASE
	clrf	SEQ_BLM_COL_CTR, BANKED
SEQ_BUTTON_SRM_Handler_Loop
	;; get address to ROWx[LR]_CHANGED register
	lfsr	FSR1, SEQ_BUTTONM_ROW0L_CHANGED
	movf	SEQ_BLM_COL_CTR, W, BANKED
	addwf	FSR1L, F

	;; skip individual checks if register is zero
	movf	INDF1, W
	bz	SEQ_BUTTON_SRM_Handler_Loop_Next

	;; check which bits are set
	btfsc	INDF1, 0		; fastest solution...
	rcall	SEQ_BUTTON_SRM_Notify_Pin0
	btfsc	INDF1, 1
	rcall	SEQ_BUTTON_SRM_Notify_Pin1
	btfsc	INDF1, 2
	rcall	SEQ_BUTTON_SRM_Notify_Pin2
	btfsc	INDF1, 3
	rcall	SEQ_BUTTON_SRM_Notify_Pin3
	btfsc	INDF1, 4
	rcall	SEQ_BUTTON_SRM_Notify_Pin4
	btfsc	INDF1, 5
	rcall	SEQ_BUTTON_SRM_Notify_Pin5
	btfsc	INDF1, 6
	rcall	SEQ_BUTTON_SRM_Notify_Pin6
	btfsc	INDF1, 7
	rcall	SEQ_BUTTON_SRM_Notify_Pin7
	
SEQ_BUTTON_SRM_Handler_Loop_Next
	;; increment until last column has been reached
	incf	SEQ_BLM_COL_CTR, F, BANKED
	movlw	0x08-1		; (8 columns)
	cpfsgt	SEQ_BLM_COL_CTR, BANKED
	rgoto SEQ_BUTTON_SRM_Handler_Loop
	return

	;; ---

SEQ_BUTTON_SRM_Notify_Pin0
	bcf	INDF1, 0			; clear "changed" flag
	movlw	0x00
	rgoto	SEQ_BUTTON_SRM_Notify_Pin_Cont
SEQ_BUTTON_SRM_Notify_Pin1
	bcf	INDF1, 1			; clear "changed" flag
	movlw	0x01
	rgoto	SEQ_BUTTON_SRM_Notify_Pin_Cont
SEQ_BUTTON_SRM_Notify_Pin2
	bcf	INDF1, 2			; clear "changed" flag
	movlw	0x02
	rgoto	SEQ_BUTTON_SRM_Notify_Pin_Cont
SEQ_BUTTON_SRM_Notify_Pin3
	bcf	INDF1, 3			; clear "changed" flag
	movlw	0x03
	rgoto	SEQ_BUTTON_SRM_Notify_Pin_Cont
SEQ_BUTTON_SRM_Notify_Pin4
	bcf	INDF1, 4			; clear "changed" flag
	movlw	0x04
	rgoto	SEQ_BUTTON_SRM_Notify_Pin_Cont
SEQ_BUTTON_SRM_Notify_Pin5
	bcf	INDF1, 5			; clear "changed" flag
	movlw	0x05
	rgoto	SEQ_BUTTON_SRM_Notify_Pin_Cont
SEQ_BUTTON_SRM_Notify_Pin6
	bcf	INDF1, 6			; clear "changed" flag
	movlw	0x06
	rgoto	SEQ_BUTTON_SRM_Notify_Pin_Cont
SEQ_BUTTON_SRM_Notify_Pin7
	bcf	INDF1, 7			; clear "changed" flag
	movlw	0x07
	;; 	rgoto	SEQ_BUTTON_SRM_Notify_Pin_Cont
SEQ_BUTTON_SRM_Notify_Pin_Cont
	;; store pin number in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	;; store pin value in MIOS_PARAMETER2
	lfsr	FSR1, SEQ_BUTTONM_ROW0L_VALUES
	movf	SEQ_BLM_COL_CTR, W, BANKED
	addwf	FSR1L, F
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	andwf	INDF1, W
	skpz
	movlw	0x01
	movwf	MIOS_PARAMETER2

	;; switch to 8-15 in MIOS_PARAMETER1 if right half of 4x16 matrix
	btfsc	SEQ_BLM_COL_CTR, 0, BANKED
	bsf	MIOS_PARAMETER1, 3

	;; select track depending on row (one row == two columns)
	;; preserve current track group (TRK[3:2])
	rrf	SEQ_BLM_COL_CTR, W, BANKED
	andlw	0x03
	movwf	PRODL
	movf	SEQ_SELECTED_TRK, W, BANKED
	andlw	0x0c
	iorwf	PRODL, W
	call	CS_M_HLP_SetSelectedTrk

#if DEFAULT_SRM_BUTTONS_NO_UI
	;; always branch to Mode0 button handler, which activates/deactivates the triggers
	call	CS_M_EDIT_GP_Callback
#else
	;; call "normal" GP button handler
	call	SEQ_GP_Button
#endif

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; fix BSR and FSR1, they propably have been overwritten by the GP_BUTTON handler
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_BUTTONM_ROW0L_CHANGED
	movf	SEQ_BLM_COL_CTR, W, BANKED
	addwf	FSR1L, F
	return


;; --------------------------------------------------------------------------
;;  general purpose button function
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
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
SEQ_BUTTON_Left
	goto	CS_MENU_BUTTON_Left
SEQ_BUTTON_LeftRemote
	goto	CS_MENU_BUTTON_LeftRemote
SEQ_BUTTON_Right
	goto	CS_MENU_BUTTON_Right
SEQ_BUTTON_RightRemote
	goto	CS_MENU_BUTTON_RightRemote

;; --------------------------------------------------------------------------
;;  Shift Button
;; --------------------------------------------------------------------------
SEQ_BUTTON_Shift
	;; activate/deactivate shift function
	SET_BSR	SEQ_BASE
	bcf	SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED
	btfss	MIOS_PARAMETER2, 0
	bsf	SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED

	;; request display re-initialisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return

;; --------------------------------------------------------------------------
;;  Alt/Menu button
;; --------------------------------------------------------------------------
SEQ_BUTTON_Alt
	;; toggle ALT button
	SET_BSR	SEQ_BASE
	bcf	SEQ_MODE0, SEQ_MODE0_ALT, BANKED
	btfss	MIOS_PARAMETER2, 0
	bsf	SEQ_MODE0, SEQ_MODE0_ALT, BANKED

	return


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
;;  Rwd button function
;; --------------------------------------------------------------------------
SEQ_BUTTON_Rew
	SET_BSR	SEQ_BASE

	;; clear REW hold flag if if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_BUTTON_Rew_Released
SEQ_BUTTON_Rew_Pressed
	bsf	SEQ_MODE2, SEQ_MODE2_REW_HOLD, BANKED ; set hold flag

	;; if shift button pressed: branch to rotate function
	btfsc	SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED
	goto	CS_M_UTILS_RotateLeft

	;; do nothing if sequencer not running
	btfss	SEQ_STATE, SEQ_STATE_RUN, BANKED
	return

	;; in pattern mode: nudge track backwards
	btfsc	SEQ_SONG, 7, BANKED
	goto	SEQ_CORE_NudgeRew

	;; in song mode: previous song position
	goto	SEQ_SONG_Rew

SEQ_BUTTON_Rew_Released
	bcf	SEQ_MODE2, SEQ_MODE2_REW_HOLD, BANKED ; clear hold flag
	return

;; --------------------------------------------------------------------------
;;  Fwd button function
;; --------------------------------------------------------------------------
SEQ_BUTTON_Fwd
	SET_BSR	SEQ_BASE

	;; clear FWD hold flag if if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_BUTTON_Fwd_Released
SEQ_BUTTON_Fwd_Pressed
	bsf	SEQ_MODE2, SEQ_MODE2_FWD_HOLD, BANKED ; set hold flag

	;; if shift button pressed: branch to rotate function
	btfsc	SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED
	goto	CS_M_UTILS_RotateRight

	;; do nothing if sequencer not running
	btfss	SEQ_STATE, SEQ_STATE_RUN, BANKED
	return

	;; in pattern mode: nudge track forward
	btfsc	SEQ_SONG, 7, BANKED
	goto	SEQ_CORE_NudgeFwd

	;; next song position
	goto	SEQ_SONG_Fwd

SEQ_BUTTON_Fwd_Released
	bcf	SEQ_MODE2, SEQ_MODE2_FWD_HOLD, BANKED ; clear hold flag
	return


;; --------------------------------------------------------------------------
;;  Play button function
;; --------------------------------------------------------------------------
SEQ_BUTTON_Play
	SET_BSR	SEQ_BASE

	;; if SHIFT button pressed: branch to tap tempo function
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, SEQ_BUTTON_TapTempo

	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

SEQ_BUTTON_Play_From_Rec
	;; restart sequencer
	movlw	(1 << SEQ_REQ_START)
	movwf	SEQ_REQ, BANKED

	;; in Auto Slave Mode: check if we are already in master mode, if not, request it now!
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_BPM_CLK_AUTO, BANKED, SEQ_BUTTON_Play_NoAuto
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED, SEQ_BUTTON_Play_NoAuto
SEQ_BUTTON_Play_NoAuto
	bcf	SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED
	bsf	SEQ_REQ, SEQ_REQ_START_AUTO_SLAVE_REQ, BANKED
SEQ_BUTTON_Play_Auto
	return

;; --------------------------------------------------------------------------
;;  Stop button function
;; --------------------------------------------------------------------------
SEQ_BUTTON_Stop
	;; if shift button pressed: start/stop pause mode
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, SEQ_BUTTON_Pause

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
;;  Record button function
;; --------------------------------------------------------------------------
SEQ_BUTTON_Record
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; toggle record mode
	SET_BSR	SEQ_BASE
	btg	SEQ_MODE1, SEQ_MODE1_RECORD, BANKED

	;; if record has been activated, and sequencer not playing: play now
	BRA_IFCLR SEQ_MODE1, SEQ_MODE1_RECORD, BANKED, SEQ_BUTTON_Record_End
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_BUTTON_Play_From_Rec
SEQ_BUTTON_Record_End
	return


;; --------------------------------------------------------------------------
;;  Tap Tempo Button
;; --------------------------------------------------------------------------
SEQ_BUTTON_TapTempo
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; call TAP function
	call	SEQ_BPM_TapTempo

	;; print tap message
	call	SEQ_LCD_PrnTapTempo

	movlw	0x80		; short display time
	call	MIOS_LCD_MessageStart

	;; if tap button has been pressed >= 5 times, start sequencer automatically if not already running
	SET_BSR	SEQ_BASE
	movf	SEQ_TAPTEMPO_BEAT_CTR, W, BANKED
	andlw	0xfc
	bz	SEQ_BUTTON_TapTempo_NoAutoStart
SEQ_BUTTON_TapTempo_AutoStart
	movlw	(1 << SEQ_REQ_START)
	btfss	SEQ_STATE, SEQ_STATE_RUN, BANKED
	movwf	SEQ_REQ, BANKED
SEQ_BUTTON_TapTempo_NoAutoStart

	;; and exit
	return


;; --------------------------------------------------------------------------
;;  enter edit mode
;; --------------------------------------------------------------------------
SEQ_BUTTON_Edit
	SET_BSR	SEQ_BASE

	;; if shift button pressed: activate All function
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, SEQ_BUTTON_All

	;; clear edit hold flag if if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_BUTTON_Edit_Released
SEQ_BUTTON_Edit_Pressed
	bsf	SEQ_MODE1, SEQ_MODE1_EDIT_HOLD, BANKED	; set hold flag

	;; branch to edit page
	movlw	CS_MENU_PAGE_EDIT
	goto	CS_M_HLP_ChangePage

SEQ_BUTTON_Edit_Released
	bcf	SEQ_MODE1, SEQ_MODE1_EDIT_HOLD, BANKED	; clear hold flag
	bcf	SEQ_MODE1, SEQ_MODE1_EDIT_HOLD_MULTISEL, BANKED	; de-arm multiselection flag
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return


;; --------------------------------------------------------------------------
;;  enter mute page
;; --------------------------------------------------------------------------
SEQ_BUTTON_Mute
	SET_BSR	SEQ_BASE

	;; if shift button pressed: activate solo function
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, SEQ_BUTTON_Solo

	;; clear mute hold flag if if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_BUTTON_Mute_Released

SEQ_BUTTON_Mute_Pressed
	bsf	SEQ_MODE1, SEQ_MODE1_MUTE_HOLD, BANKED	; set hold flag

	;; branch to mute menu
	movlw	CS_MENU_PAGE_MUTE
	goto	CS_M_HLP_ChangePage

SEQ_BUTTON_Mute_Released
	bcf	SEQ_MODE1, SEQ_MODE1_MUTE_HOLD, BANKED	; clear hold flag
	bcf	SEQ_MODE1, SEQ_MODE1_MUTE_HOLD_MULTISEL, BANKED	; de-arm multiselection flag
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return


;; --------------------------------------------------------------------------
;;  enter pattern mode
;; --------------------------------------------------------------------------
SEQ_BUTTON_Pattern
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; if SHIFT button pressed: branch to bank menu
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, SEQ_BUTTON_Pattern_Bank

	;; if ALT button pressed: branch to save menu
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_ALT, BANKED, SEQ_BUTTON_Pattern_Save

	;; clear state of right select buttons
	clrf	CS_PATTERN_GP_STATE_R

	;; branch to normal pattern menu
	movlw	CS_MENU_PAGE_PATTERN
	goto	CS_M_HLP_ChangePage

SEQ_BUTTON_Pattern_Bank
	movlw	CS_MENU_PAGE_PATTERN_B
	goto	CS_M_HLP_ChangePage
	
SEQ_BUTTON_Pattern_Save
	movlw	CS_MENU_PAGE_PATTERN_S
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

	;; branch to song menu
	movlw	CS_MENU_PAGE_SONG
	goto	CS_M_HLP_ChangePage


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
	;; Toggle Mode (default)

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

	;; request display re-initisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
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

	;; set speed depending on SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS
	goto	SEQ_ENC_ConfigSpeed



;; --------------------------------------------------------------------------
;;  Select Section A/B/C/D and Step View
;; --------------------------------------------------------------------------
SEQ_BUTTON_SectionA
	movlw	0
	rgoto	SEQ_BUTTON_Sectionx_Cont
SEQ_BUTTON_SectionB
	movlw	1
	rgoto	SEQ_BUTTON_Sectionx_Cont
SEQ_BUTTON_SectionC
	movlw	2
	rgoto	SEQ_BUTTON_Sectionx_Cont
SEQ_BUTTON_SectionD
	movlw	3
	;; 	rgoto	SEQ_BUTTON_Sectionx_Cont

SEQ_BUTTON_Sectionx_Cont
	movwf	MIOS_PARAMETER1

	;; if shift button pressed: only change step view, don't change section selection
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, SEQ_BUTTON_Sectionx_NoShift
SEQ_BUTTON_Sectionx_Shift
	;; ignore if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; select new step view depending on last pressed button
	movff	MIOS_PARAMETER1, SEQ_SELECTED_STEP_VIEW

	;; request display reinitialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return


SEQ_BUTTON_Sectionx_NoShift	

	;; only save button state if button has been depressed and exit
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_BUTTON_SectionX_Hlp_Sav

	;; select new step view depending on last pressed button
	movff	MIOS_PARAMETER1, SEQ_SELECTED_STEP_VIEW

	;; branch depending on state of both select buttons:
	;; if only one select button pressed: radio-button function (1 of 2)
	;; if more than one select button pressed: toggle function (2 of 2)	
	movf	SEQ_SELECTED_ABCD, W, BANKED
	andlw	0xf0
	xorlw	0xf0
	bz	SEQ_BUTTON_SectionX_Radio
SEQ_BUTTON_SectionX_Toggle
	;; toggle section A/B flag
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	xorwf	SEQ_SELECTED_ABCD, F, BANKED
 	rgoto	SEQ_BUTTON_SectionX_End
SEQ_BUTTON_SectionX_Radio
	;; select 1 of 2
	movlw	0xf0
	andwf	SEQ_SELECTED_ABCD, F, BANKED
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	iorwf	SEQ_SELECTED_ABCD, F, BANKED

	;; 	rgoto	SEQ_BUTTON_SectionX_End
	
SEQ_BUTTON_SectionX_End
	;; save button state
	rcall	SEQ_BUTTON_SectionX_Hlp_Sav

	;; request display reinitialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return

;; ---
SEQ_BUTTON_SectionX_Hlp_Sav
	;; mask out old button status
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitANDMask
	swapf	WREG, W
	andwf	SEQ_SELECTED_ABCD, F, BANKED

	;; store new button status (pressed/depressed) in SEQ_SELECTED_TRKS_Gx[7..4]
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
	btfsc	MIOS_PARAMETER2, 0
	iorwf	SEQ_SELECTED_ABCD, F, BANKED
	return


;; --------------------------------------------------------------------------
;;  Select Gate Layer
;; --------------------------------------------------------------------------
SEQ_BUTTON_LayerGate
	SET_BSR	SEQ_BASE

	;; do nothing when button has been released
	btfsc	MIOS_PARAMETER2, 0
	return

	;; select gate layer
	bcf	SEQ_MODE2, SEQ_MODE2_AUX_SELECTED, BANKED

	;; request display re-initialisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return


;; --------------------------------------------------------------------------
;;  Select Auxiliary Layer
;; --------------------------------------------------------------------------
SEQ_BUTTON_LayerAux
	SET_BSR	SEQ_BASE

	;; do nothing when button has been released
	btfsc	MIOS_PARAMETER2, 0
	return

	;; select aux layer
	bsf	SEQ_MODE2, SEQ_MODE2_AUX_SELECTED, BANKED

	;; request display re-initialisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return


;; --------------------------------------------------------------------------
;;  Select Group 1/2/3/4
;; --------------------------------------------------------------------------
SEQ_BUTTON_Group1
	movlw	0 << 2
	rgoto	SEQ_BUTTON_Groupx_Cont
SEQ_BUTTON_Group2
	movlw	1 << 2
	rgoto	SEQ_BUTTON_Groupx_Cont
SEQ_BUTTON_Group3
	movlw	2 << 2
	rgoto	SEQ_BUTTON_Groupx_Cont
SEQ_BUTTON_Group4
	movlw	3 << 2
	;; 	rgoto	SEQ_BUTTON_Groupx_Cont

SEQ_BUTTON_Groupx_Cont
	movwf	MIOS_PARAMETER1

	;; do nothing when button has been released
	btfsc	MIOS_PARAMETER2, 0
	return

	;; store new track offset, preserve TRK[1:0]
	SET_BSR	SEQ_BASE
	movf	SEQ_SELECTED_TRK, W, BANKED
	andlw	0x03
	iorwf	MIOS_PARAMETER1, W
	call	CS_M_HLP_SetSelectedTrk

	;; request display re-initialisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return


;; --------------------------------------------------------------------------
;;  Toggle Live Mode
;; --------------------------------------------------------------------------
SEQ_BUTTON_Live
	SET_BSR	SEQ_BASE

	;; if shift button pressed: start/stop record mode
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, SEQ_BUTTON_Record

	;; do nothing when button has been released
	btfsc	MIOS_PARAMETER2, 0
	return

	;; toggle mode flag
	btg	SEQ_MODE2, SEQ_MODE2_LIVE_MODE, BANKED

	return


;; --------------------------------------------------------------------------
;;  F1 button --- implement your favourite function here!
;; --------------------------------------------------------------------------
SEQ_BUTTON_F1
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	return

;; --------------------------------------------------------------------------
;;  F2 button --- implement your favourite function here!
;; --------------------------------------------------------------------------
SEQ_BUTTON_F2
	return

;; --------------------------------------------------------------------------
;;  F3 button --- implement your favourite function here!
;; --------------------------------------------------------------------------
SEQ_BUTTON_F3
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	return

;; --------------------------------------------------------------------------
;;  F4 button --- implement your favourite function here!
;; --------------------------------------------------------------------------
SEQ_BUTTON_F4
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	return
