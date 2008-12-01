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
;;  This function is also called from SEQ_BUTTON_SRM_Handler if a button
;;  of the misc. button matrix has been toggled.
;; 
;;  Input:
;;     o Button number in MIOS_PARAMETER1
;;       0-127: common "DIN" button
;;       128-191: button of "misc" button matrix
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
SEQ_BUTTON_Handler
#if 0
	;; only for debugging: send MIDI event
	movlw	0x90		; button 1-127: channel #1
	btfsc	MIOS_PARAMETER1, 7
	addlw	0x01		; button 128-255: channel #2
	call	MIOS_MIDI_TxBufferPut
	
	movf	MIOS_PARAMETER1, W ; button number
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut

	movlw	0x00		; velocity 0 if button depressed
	btfss	MIOS_PARAMETER2, 0
	movlw	0x7f		; velocity 127 if button pressed
	call	MIOS_MIDI_TxBufferPut
#endif

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
;;  Initialises the Button matrix handler (called from USER_Init hook
;; --------------------------------------------------------------------------
SEQ_BUTTON_SRM_Init
        lfsr    FSR0, SEQ_BUTTONM_ROW0L_VALUES   ; will be set to 0xff
        lfsr    FSR1, SEQ_BUTTONM_ROW0L_CHANGED  ; will be set to 0x00
        clrf    PRODL                   ; init loop counter
SEQ_BUTTON_SRM_Init_Loop
        setf    POSTINC0
        clrf    POSTINC1
        incf    PRODL, F
        BRA_IFCLR PRODL, 4, ACCESS, SEQ_BUTTON_SRM_Init_Loop  ; loop 16 times
        
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
#if DEFAULT_SRM_DIN_L
	movlw	DEFAULT_SRM_DIN_L - 1
	rcall	SEQ_BUTTON_GetRowIRQ_Sub
#endif

#if DEFAULT_SRM_DIN_R
	;; check the right half
	incf	IRQ_TMP1, F	; row*2+1
	movlw	DEFAULT_SRM_DIN_R - 1
	rcall	SEQ_BUTTON_GetRowIRQ_Sub
#endif

#if DEFAULT_SRM_DIN_M
	;; check the misc. button SR
	decf	SEQ_BLM_ROW_CTR, W, BANKED
	andlw	0x07
	addlw	LOW(SEQ_BUTTONM_MISC0_VALUES-SEQ_BUTTONM_ROW0L_VALUES)
	movwf	IRQ_TMP1
	movlw	DEFAULT_SRM_DIN_M - 1
	rcall	SEQ_BUTTON_GetRowIRQ_Sub
#endif
	return


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
	;; for second button matrix: SEQ_BUTTONM_MISCx_VALUES (offset selected via IRQ_TMP1)
	lfsr	FSR1, SEQ_BUTTONM_ROW0L_VALUES
	movf	IRQ_TMP1, W
	movff	PLUSW1, IRQ_TMP2
	movff	MIOS_PARAMETER1, PLUSW1

	;; old value in IRQ_TMP2
	;; new value in MIOS_PARAMETER1
	;; select ROWx_CHANGED register
	;; for second button matrix: SEQ_BUTTONM_MISCx_CHANGED (offset selected via IRQ_TMP1)
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
	movlw	0x10-1		; (16 columns, 8 for GP button matrix, 8 for misc. button matrix)
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

	;; branch depending on GP button matrix (calls SEQ_GP_SRM_Button) and misc. button matrix (calls SEQ_BUTTON_Handler)
	BRA_IFSET SEQ_BLM_COL_CTR, 3, BANKED, SEQ_BUTTON_SRM_Notify_Pin_Misc
SEQ_BUTTON_SRM_Notify_Pin_GP
	;; switch to 8-15 in MIOS_PARAMETER1 if right half of 4x16 matrix
	btfsc	SEQ_BLM_COL_CTR, 0, BANKED
	bsf	MIOS_PARAMETER1, 3

	;; call SRM button handler
	call	SEQ_GP_SRM_Button
	rgoto	SEQ_BUTTON_SRM_Notify_Pin_Next

SEQ_BUTTON_SRM_Notify_Pin_Misc
	;; select pin number (128-191)
	swapf	SEQ_BLM_COL_CTR, W, BANKED
	rrf	WREG, W
	andlw	0x38
	iorlw	0x80
	iorwf	MIOS_PARAMETER1, F

	;; call normal button handler
	call	SEQ_BUTTON_Handler

SEQ_BUTTON_SRM_Notify_Pin_Next

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

	;; if MENU button is pressed: use Tap Tempo function
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_MENU, BANKED, SEQ_BUTTON_TapTempo
	
	;; restart sequencer
	SET_BSR	SEQ_BASE
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
;;  "External MIDI Start" Button
;; --------------------------------------------------------------------------
SEQ_BUTTON_ExtMIDIStart
	SET_BSR	SEQ_BASE

	;; clear MIDIstart hold flag if if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_BUTTON_ExtMIDIStart_Released
SEQ_BUTTON_ExtMIDIStart_Pressed
	bsf	SEQ_MODE1, SEQ_MODE1_MIDISTART_HOLD, BANKED

	;; start with first step - can be changed via GP button so long button is held
	clrf	SEQ_EXT_MIDISTART_STEP, BANKED

	;; request to send external MIDI start event once this step is reached
	bsf	SEQ_REQ, SEQ_REQ_EXT_MIDISTART, BANKED

	return

	
SEQ_BUTTON_ExtMIDIStart_Released
	bcf	SEQ_MODE1, SEQ_MODE1_MIDISTART_HOLD, BANKED
	return


;; --------------------------------------------------------------------------
;;  enter edit mode
;; --------------------------------------------------------------------------
SEQ_BUTTON_Edit
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

#if 1
	;; more induitive usage: if edit button pressed
	;; - always switch to step 1-16 view
	SET_BSR	SEQ_BASE
	bcf	SEQ_SELECTED_STEPS, 0, BANKED
	;; - always switch to layer A when edit button pressed
	movlw	0x01
	movwf	SEQ_SELECTED_LAYERS, BANKED
	;; - always switch to trigger layer A when edit button pressed
	movlw	0x01
	movwf	SEQ_SELECTED_TRG_LAYERS, BANKED
	;; - if no track selected in current group: select track 1
	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	movf	SEQ_SELECTED_GROUP, W, BANKED
	addwf	FSR1L, F
	movf	INDF1, W
	andlw	0x0f
	bnz	SEQ_BUTTON_Edit_TrkSelected
SEQ_BUTTON_Edit_NoTrkSelected
	;; clear all selections
	movlw	0xf0
	movwf	SEQ_SELECTED_TRKS_G0, BANKED
	movwf	SEQ_SELECTED_TRKS_G1, BANKED
	movwf	SEQ_SELECTED_TRKS_G2, BANKED
	movwf	SEQ_SELECTED_TRKS_G3, BANKED
	;; select track 1 of current group
	movlw	0xf1
	movwf	INDF1
SEQ_BUTTON_Edit_TrkSelected
#endif

	;; set/clear encoder fast function if required
	call	SEQ_ENC_AutoFast

	;; branch to view menu
	movlw	CS_MENU_PAGE_VIEW
	goto	CS_M_HLP_ChangePage

;; --------------------------------------------------------------------------
;;  enter mute page
;; --------------------------------------------------------------------------
SEQ_BUTTON_Mute
	SET_BSR	SEQ_BASE

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

	;; branch to pattern menu
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

	;; branch to song menu
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

	;; if MENU button is pressed: use External MIDI Start function
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_MENU, BANKED, SEQ_BUTTON_ExtMIDIStart
	;; ensure that hold flag will be cleared - regardless if button is pressed/release
	bcf	SEQ_MODE1, SEQ_MODE1_MIDISTART_HOLD, BANKED

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

	;; holding the button switches the "CHANGE_ALL_STEPS" flag
	bcf	CS_STAT2, CS_STAT2_ALL_SAME_VALUE
	btfss	MIOS_PARAMETER2, 0
	bsf	CS_STAT2, CS_STAT2_ALL_SAME_VALUE

	;; exit if button depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; else toggle status
	btg	SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED
#else
	;; holding the button switches the "CHANGE_ALL_STEPS" flag
	bcf	CS_STAT2, CS_STAT2_ALL_SAME_VALUE
	btfss	MIOS_PARAMETER2, 0
	bsf	CS_STAT2, CS_STAT2_ALL_SAME_VALUE

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

	;; set speed depending on SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS
	goto	SEQ_ENC_ConfigSpeed


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

	;; alternative function if in song mode
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_SONG, BANKED, SEQ_BUTTON_Layerx_NoSong
SEQ_BUTTON_Layerx_Song
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
	

SEQ_BUTTON_Layerx_NoSong
	;; if button has been depressed: release message
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_BUTTON_Layerx_NoSong_Released
SEQ_BUTTON_Layerx_NoSong_Pressed
	;; decode button number
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	andlw	0x0f

	;; save value in "SEQ_SELECTED_LAYERS"
	movwf	SEQ_SELECTED_LAYERS, BANKED

	;; request display reinitialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; set/clear encoder fast function if required
	call	SEQ_ENC_AutoFast

	;; hold message and exit
	goto	SEQ_MSG_Hold

SEQ_BUTTON_Layerx_NoSong_Released
	;; stop message (releases hold)
	goto	SEQ_MSG_Stop


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

	;; alternative function if not in edit or mute mode
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED, SEQ_BUTTON_Trackx_Edit
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_MUTE, BANKED, SEQ_BUTTON_Trackx_Mute
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_PATTERN, BANKED, SEQ_BUTTON_Trackx_Pattern

SEQ_BUTTON_Trackx_Song
	;; do nothing when button has been released
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else set cursor pos

	;; dirty quickfix: in song mode: save current entry before changing to new cursor pos
	CALL_IFSET SEQ_MODE0, SEQ_MODE0_SONG, BANKED, CS_M_SONG_WriteValueChk

	movf	MIOS_PARAMETER1, W
	andlw	0x03
	addlw	3
	movwf	CS_MENU_CURSOR_POS
	goto	CS_MENU_NewCursorPos

SEQ_BUTTON_Trackx_Pattern
	;; do nothing when button has been released
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else change pattern group
	movf	MIOS_PARAMETER1, W
	andlw	0x03
	movwf	SEQ_SELECTED_GROUP, BANKED
	goto	CS_MENU_NewCursorPos	; (updates cursor in pattern menu depending on selected group)

SEQ_BUTTON_Trackx_Edit
SEQ_BUTTON_Trackx_Mute
	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	movf	SEQ_SELECTED_GROUP, W, BANKED
	addwf	FSR1L, F

	;; decode button number
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	andlw	0x0f
	movwf	MIOS_PARAMETER1

	;; only save button state if button has been depressed and exit
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_BUTTON_Trackx_Hlp_Sav

	;; deselect all tracks which are not in the group
	lfsr	FSR0, SEQ_SELECTED_TRKS_G0
	movlw	4
	movwf	TMP1
SEQ_BUTTON_Trackx_DeselLoop
	movf	FSR0L, W
	xorwf	FSR1L, W
	bz	SEQ_BUTTON_Trackx_DeselLoopNext
	movlw	0xf0
	movwf	INDF0
SEQ_BUTTON_Trackx_DeselLoopNext
	incf	FSR0L, F
	decfsz	TMP1, F
	rgoto	SEQ_BUTTON_Trackx_DeselLoop	
	

	;; branch depending on state of all 4 select buttons:
	;; if only one select button pressed: radio-button function (1 of 4)
	;; if more than one select button pressed: toggle function (4 of 4)
	movf	INDF1, W
	andlw	0xf0
	xorlw	0xf0
	bz	SEQ_BUTTON_Trackx_Radio
SEQ_BUTTON_Trackx_Toggle
	;; toggle track flag
	movf	MIOS_PARAMETER1, W
	xorwf	INDF1, F
 	rgoto	SEQ_BUTTON_Trackx_End
SEQ_BUTTON_Trackx_Radio
	;; select 1 of 4
	movlw	0xf0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	iorwf	INDF1, F

	;; toggle between step 1-16/17-32 on doubleclick
	movf	CS_MENU_DOUBLECLICK_CTR, W
	bz	SEQ_BUTTON_Trackx_Radio_NoDC
SEQ_BUTTON_Trackx_Radio_DC
	btg	SEQ_SELECTED_STEPS, 0, BANKED
	setf	CS_MENU_DOUBLECLICK_LAST_DIN	; ensure that we have to doubleclick again for next toggle
SEQ_BUTTON_Trackx_Radio_NoDC


	;; 	rgoto	CS_MENU_BUTTON_SID_End
	
SEQ_BUTTON_Trackx_End
	;; save button state
	rcall	SEQ_BUTTON_Trackx_Hlp_Sav

	;; request display reinitialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; set/clear encoder fast function if required and exit
	goto	SEQ_ENC_AutoFast


SEQ_BUTTON_Trackx_Hlp_Sav
	;; mask out old button status
	swapf	MIOS_PARAMETER1, W
	xorlw	0xff
	andwf	INDF1, F

	;; store new button status (pressed/depressed) in SEQ_SELECTED_TRKS_Gx[7..4]
	swapf	MIOS_PARAMETER1, W
	btfsc	MIOS_PARAMETER2, 0
	iorwf	INDF1, F
	return


;; --------------------------------------------------------------------------
;;  Select Group 1/2/3/4
;; --------------------------------------------------------------------------
SEQ_BUTTON_Group1
	movlw	0
	rgoto	SEQ_BUTTON_Groupx_Cont
SEQ_BUTTON_Group2
	movlw	1
	rgoto	SEQ_BUTTON_Groupx_Cont
SEQ_BUTTON_Group3
	movlw	2
	rgoto	SEQ_BUTTON_Groupx_Cont
SEQ_BUTTON_Group4
	movlw	3
	;; 	rgoto	SEQ_BUTTON_Groupx_Cont

SEQ_BUTTON_Groupx_Cont
	movwf	MIOS_PARAMETER1

	;; do nothing when button has been released
	btfsc	MIOS_PARAMETER2, 0
	return

	;; get selected track(s) of current group
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	movf	SEQ_SELECTED_GROUP, W, BANKED
	movff	PLUSW1, TMP1

	;; clear all selections
	movlw	0xf0
	movwf	SEQ_SELECTED_TRKS_G0, BANKED
	movwf	SEQ_SELECTED_TRKS_G1, BANKED
	movwf	SEQ_SELECTED_TRKS_G2, BANKED
	movwf	SEQ_SELECTED_TRKS_G3, BANKED
	
	;; set new track group
	movff	MIOS_PARAMETER1, SEQ_SELECTED_GROUP

	;; store selected tracks in new groups
	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	movf	SEQ_SELECTED_GROUP, W, BANKED
	movff	TMP1, PLUSW1

	;; request display re-initialisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return


;; --------------------------------------------------------------------------
;;  Select Trigger Layer A/B/C
;; --------------------------------------------------------------------------
SEQ_BUTTON_TrgLayerA
	movlw	0
	rgoto	SEQ_BUTTON_TrgLayerx_Cont
SEQ_BUTTON_TrgLayerB
	movlw	1
	rgoto	SEQ_BUTTON_TrgLayerx_Cont
SEQ_BUTTON_TrgLayerC
	movlw	2
	rgoto	SEQ_BUTTON_TrgLayerx_Cont
	;; 	rgoto	SEQ_BUTTON_TrgLayerx_Cont

SEQ_BUTTON_TrgLayerx_Cont
	movwf	MIOS_PARAMETER1

	;; do nothing when button has been released
	btfsc	MIOS_PARAMETER2, 0
	return

	;; select new trigger layer
	call	MIOS_HLP_GetBitORMask
	SET_BSR	SEQ_BASE
	movwf	SEQ_SELECTED_TRG_LAYERS, BANKED

	;; request display re-initialisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return

;; --------------------------------------------------------------------------
;;  toggle between step 1-16/17-32
;; --------------------------------------------------------------------------
SEQ_BUTTON_ToggleStepView
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; toggle display
	SET_BSR	SEQ_BASE
	btg	SEQ_SELECTED_STEPS, 0, BANKED

	;; request display re-initialisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return

;; --------------------------------------------------------------------------
;;  toggles the gate of the current step
;;  This is especially a nice remote keyboard function
;; --------------------------------------------------------------------------
SEQ_BUTTON_ToggleGate
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; get pointer to gate flag
	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED
	movff	SEQ_CURRENT_STEP, SEQ_EVNTS

	;; toggle gate flag
	call	SEQ_TRG_GateGet
	xorlw	1
	call	SEQ_TRG_GateSet

	;; print sequence on screen
	goto	SEQ_MSG_Start


;; --------------------------------------------------------------------------
;;  decrements the edited/recorded step
;;  This is especially a nice remote keyboard function
;; --------------------------------------------------------------------------
SEQ_BUTTON_StepDec
	;; exit if button depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; decrement current step number, wrap at 32
	SET_BSR	SEQ_BASE
	decf	SEQ_CURRENT_STEP, W, BANKED
	skpc
	movlw	0x1f
	movwf	SEQ_CURRENT_STEP, BANKED

	rgoto	SEQ_BUTTON_StepDec_Cont


;; --------------------------------------------------------------------------
;;  increments the edited/recorded step
;;  This is especially a nice remote keyboard function
;; --------------------------------------------------------------------------
SEQ_BUTTON_StepInc
	;; exit if button depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; increment current step number, wrap at 32
	SET_BSR	SEQ_BASE
	incf	SEQ_CURRENT_STEP, F, BANKED
	bcf	SEQ_CURRENT_STEP, 5, BANKED

SEQ_BUTTON_StepDec_Cont
	;; set also as record step and clear poly counter
	movff	SEQ_CURRENT_STEP, SEQ_RECORD_STEP
	clrf	SEQ_RECORD_POLY_CTR, BANKED
	

	;; fix "SEQ_SELECTED_STEPS" flag (show step 1-16 or 17-32)
	bcf	SEQ_SELECTED_STEPS, 0, BANKED
	btfsc	SEQ_CURRENT_STEP, 4, BANKED
	bsf	SEQ_SELECTED_STEPS, 0, BANKED

	;; print sequence on screen
	goto	SEQ_MSG_Start

;; --------------------------------------------------------------------------
;;  Shortcut button to the Utility menu
;; --------------------------------------------------------------------------
SEQ_BUTTON_Utility
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch to utility menu
	movlw	CS_MENU_PAGE_UTILS
	goto	CS_M_HLP_ChangePage

;; --------------------------------------------------------------------------
;;  Copy button
;; --------------------------------------------------------------------------
SEQ_BUTTON_Copy
	;; branch to copy function of UTIL page
	goto	CS_M_UTILS_CopyViaButton


;; --------------------------------------------------------------------------
;;  Paste button
;; --------------------------------------------------------------------------
SEQ_BUTTON_Paste
	;; branch to paste function of UTIL page
	goto	CS_M_UTILS_PasteViaButton


;; --------------------------------------------------------------------------
;;  Clear button
;; --------------------------------------------------------------------------
SEQ_BUTTON_Clear
	;; branch to clear function of UTIL page
	goto	CS_M_UTILS_ClearViaButton

	
;; --------------------------------------------------------------------------
;;  F1 button --- implement your favourite function here!
;; --------------------------------------------------------------------------
SEQ_BUTTON_F1
	goto	SEQ_BUTTON_Utility

;; --------------------------------------------------------------------------
;;  F2 button --- implement your favourite function here!
;; --------------------------------------------------------------------------
SEQ_BUTTON_F2
	;; toggle between step 1-16/17-32
	;; could also be a dedicated button with appr. LED
	goto	SEQ_BUTTON_ToggleStepView

;; --------------------------------------------------------------------------
;;  F3 button --- implement your favourite function here!
;; --------------------------------------------------------------------------
SEQ_BUTTON_F3
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

#if 1
#if 1
	call	CS_M_HLP_GetSelectedTrgLayer
	addlw	1
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	btfsc	WREG, 3
	movlw 0x01
	SET_BSR	SEQ_BASE
	movwf	SEQ_SELECTED_TRG_LAYERS, BANKED

	;; request display re-initialisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return
#else
	;; unmute all tracks
	SET_BSR	SEQ_BASE
	clrf	SEQ_TRKS_MUTED0, BANKED
	clrf	SEQ_TRKS_MUTED1, BANKED

	;; and branch to mute menu
	rgoto	SEQ_BUTTON_Mute
#endif
#else
	;; invert all gates
	SET_BSR	SEQ_BASE
	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED
	clrf	SEQ_EVNTS, BANKED
SEQ_BUTTON_F3_Loop
	call	SEQ_TRG_GateGet
	xorlw	1
	call	SEQ_TRG_GateSet
	incf	SEQ_EVNTS, F, BANKED
	BRA_IFCLR SEQ_EVNTS, 5, BANKED, SEQ_BUTTON_F3_Loop
	;; request display re-initialisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return
#endif

;; --------------------------------------------------------------------------
;;  F4 button --- implement your favourite function here!
;; --------------------------------------------------------------------------
SEQ_BUTTON_F4
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

#if 0
	;; mute all tracks
	SET_BSR	SEQ_BASE
	setf	SEQ_TRKS_MUTED0, BANKED
	setf	SEQ_TRKS_MUTED1, BANKED

	;; and branch to mute menu
	rgoto	SEQ_BUTTON_Mute
#else
	;; increment track group and branch to Groupx_Cont function
	SET_BSR	SEQ_BASE
	incf	SEQ_SELECTED_GROUP, W, BANKED
	andlw	0x03	
	rgoto	SEQ_BUTTON_Groupx_Cont
#endif
