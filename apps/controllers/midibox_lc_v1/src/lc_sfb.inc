; $Id$
;
; MIDIbox LC Special Function Button Handlers (see lc_io_table.inc)
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by LC_BUTTON_Handler (lc_buttons.inc) when a
;;  button has been pressed which is assigned to a special function
;;     o special function number in WREG
;;     o Button number in MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
LC_SFB_Handler
	;; else branch to the special function
	JUMPTABLE_2BYTES 32	; 32 entries, addressed by LC_MBHP_xxx
	rgoto	LC_SFB_DISPLAY_Page0
	rgoto	LC_SFB_DISPLAY_Page1
	rgoto	LC_SFB_DISPLAY_Page2
	rgoto	LC_SFB_DISPLAY_Page3
	rgoto	LC_SFB_DISPLAY_NextPage
	return
	return
	return

	return
	return
	return
	return
	return
	return
	return
	return

	rgoto	LC_SFB_LAYER_Switch0
	rgoto	LC_SFB_LAYER_Switch1
	rgoto	LC_SFB_LAYER_Toggle
	rgoto	LC_SFB_LAYER_Hold
	
	rgoto	LC_SFB_GPC_Switch0
	rgoto	LC_SFB_GPC_Switch1
	rgoto	LC_SFB_GPC_Toggle
	rgoto	LC_SFB_GPC_Hold

	rgoto	LC_SFB_LEDMETER_Switch0
	rgoto	LC_SFB_LEDMETER_Switch1
	rgoto	LC_SFB_LEDMETER_Toggle
	rgoto	LC_SFB_LEDMETER_Hold
	rgoto	LC_SFB_GPC_Switch0
	rgoto	LC_SFB_CALI_Switch1
	rgoto	LC_SFB_CALI_Toggle
	rgoto	LC_SFB_CALI_Hold


; ==========================================================================

; --------------------------------------------------------------------------
	;; switch to display page 0
LC_SFB_DISPLAY_Page0
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	movlw	0x00
	movwf	LC_DISPLAY_PAGE
	rgoto	LC_SFB_DISPLAY_Cont
; --------------------------------------------------------------------------
	;; switch to display page 1
LC_SFB_DISPLAY_Page1
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	movlw	0x01
	movwf	LC_DISPLAY_PAGE
	rgoto	LC_SFB_DISPLAY_Cont
; --------------------------------------------------------------------------
	;; switch to display page 2
LC_SFB_DISPLAY_Page2
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	movlw	0x02
	movwf	LC_DISPLAY_PAGE
	rgoto	LC_SFB_DISPLAY_Cont
; --------------------------------------------------------------------------
	;; switch to display page 3
LC_SFB_DISPLAY_Page3
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	movlw	0x03
	movwf	LC_DISPLAY_PAGE
	rgoto	LC_SFB_DISPLAY_Cont
; --------------------------------------------------------------------------
	;; switch to next display page
LC_SFB_DISPLAY_NextPage
	;; only turn off LED if button has been depressed
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, LC_SFB_DISPLAY_NextPage_Pre
LC_SFB_DISPLAY_NextPage_Dep
	clrf	MIOS_PARAMETER3		; (parameters: see lc_leds.inc)
	movlw	ID_MBLC_DISPLAY_NEXT
	movwf	MIOS_PARAMETER2
	goto	LC_LED_Set	; and exit

LC_SFB_DISPLAY_NextPage_Pre

	;; update assigned LEDs
	movlw	0x01			; (parameters: see lc_leds.inc)
	movwf	MIOS_PARAMETER3
	movlw	ID_MBLC_DISPLAY_NEXT
	movwf	MIOS_PARAMETER2
	call	LC_LED_Set

	incf	LC_DISPLAY_PAGE, F
	movlw	0x04		; we support page 0-3
	cpfslt	LC_DISPLAY_PAGE, ACCESS
	clrf LC_DISPLAY_PAGE


LC_SFB_DISPLAY_Cont
	;; update LEDs
	rcall	LC_SFB_LED_Update

	;; init display and exit
	goto	USER_DISPLAY_Init



; --------------------------------------------------------------------------
	;; switch to layer 0
LC_SFB_LAYER_Switch0
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	bcf	LC_STAT, LC_STAT_LAYER_SEL; switch to layer 0
	rgoto	LC_SFB_LAYER_Cont
; --------------------------------------------------------------------------
	;; switch to layer 1
LC_SFB_LAYER_Switch1
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	bsf	LC_STAT, LC_STAT_LAYER_SEL; switch to layer 1
	rgoto	LC_SFB_LAYER_Cont

; --------------------------------------------------------------------------
	;; toggle layer
LC_SFB_LAYER_Toggle
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	btg	LC_STAT, LC_STAT_LAYER_SEL; toggle layer
	rgoto	LC_SFB_LAYER_Cont

; --------------------------------------------------------------------------
	;; hold layer
LC_SFB_LAYER_Hold
	;; if button pressed (MIOS_PARAMETER2[0] == 0): layer 1
	;; if button depressed (MIOS_PARAMETER2[0] == 1): layer 0
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, LC_SFB_LAYER_Hold_1
LC_SFB_LAYER_Hold_0
	bcf	LC_STAT, LC_STAT_LAYER_SEL; switch to layer 0
	rgoto	LC_SFB_LAYER_Cont
LC_SFB_LAYER_Hold_1
	bsf	LC_STAT, LC_STAT_LAYER_SEL; switch to layer 1
	;; 	rgoto	LC_SFB_LAYER_Cont
LC_SFB_LAYER_Cont
	;; update ALL leds which have been assigned to a LC function
	call	LC_LED_Refresh

	;; update SFB leds
	rgoto	LC_SFB_LED_Update

; --------------------------------------------------------------------------
	;; switch to logic control emulation mode
LC_SFB_GPC_Switch0
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	bcf	LC_STAT, LC_STAT_GPC_SEL; switch to emulation mode
	bcf	LC_STAT, LC_STAT_CALI_SEL; switch to emulation mode
	rgoto	LC_SFB_GPC_Cont
; --------------------------------------------------------------------------
	;; switch to general purpose controller mode
LC_SFB_GPC_Switch1
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	bsf	LC_STAT, LC_STAT_GPC_SEL; switch to controller mode
	bcf	LC_STAT, LC_STAT_CALI_SEL
	rgoto	LC_SFB_GPC_Cont

; --------------------------------------------------------------------------
	;; toggle GPC mode
LC_SFB_GPC_Toggle
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	btg	LC_STAT, LC_STAT_GPC_SEL; toggle GPC mode
	bcf	LC_STAT, LC_STAT_CALI_SEL
	rgoto	LC_SFB_GPC_Cont

; --------------------------------------------------------------------------
	;; hold GPC mode
LC_SFB_GPC_Hold
	;; if button pressed (MIOS_PARAMETER2[0] == 0): GPC on
	;; if button depressed (MIOS_PARAMETER2[0] == 1): GPC off
	bcf	LC_STAT, LC_STAT_CALI_SEL
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, LC_SFB_GPC_Hold_1
LC_SFB_GPC_Hold_0
	bcf	LC_STAT, LC_STAT_GPC_SEL; switch to logic control mode
	rgoto	LC_SFB_GPC_Cont
LC_SFB_GPC_Hold_1
	bsf	LC_STAT, LC_STAT_GPC_SEL; switch to general purpose controller mode
	;; 	rgoto	LC_SFB_GPC_Cont
LC_SFB_GPC_Cont
	call	LC_GPC_Msg_Update	; update GPC message
	;; (also used by lc_cali.inc)
_LC_SFB_CALI_Cont
	;; request display message update
	bsf	LC_STAT, LC_STAT_MSG_UPDATE_REQ

	;; request update of all LED rings
	setf	LC_LEDRING_UPDATE_REQ

	;; update ALL leds which have been assigned to a LC function
	call	LC_LED_Refresh

	;; update SFB leds
	rcall	LC_SFB_LED_Update

	;; send MIDI event if activated in main.asm
#if GPC_BUTTON_SENDS_MIDI_EVENT != 0
	call	MIOS_MIDI_BeginStream
	movlw	GPC_BUTTON_OFF_EVNT0
	btfsc	LC_STAT, LC_STAT_GPC_SEL
	movlw GPC_BUTTON_ON_EVNT0
	call	MIOS_MIDI_TxBufferPut
	movlw	GPC_BUTTON_OFF_EVNT1
	btfsc	LC_STAT, LC_STAT_GPC_SEL
	movlw GPC_BUTTON_ON_EVNT1
	call	MIOS_MIDI_TxBufferPut
	movlw	GPC_BUTTON_OFF_EVNT2
	btfsc	LC_STAT, LC_STAT_GPC_SEL
	movlw GPC_BUTTON_ON_EVNT2
	call	MIOS_MIDI_TxBufferPut
	call	MIOS_MIDI_EndStream
#endif
	return

; --------------------------------------------------------------------------
	;; switch to motorfader calibration mode
LC_SFB_CALI_Switch1
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	bcf	LC_STAT, LC_STAT_GPC_SEL; switch to calibration mode
	bsf	LC_STAT, LC_STAT_CALI_SEL
	rgoto	LC_SFB_CALI_Cont

; --------------------------------------------------------------------------
	;; toggle CALI mode
LC_SFB_CALI_Toggle
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	bcf	LC_STAT, LC_STAT_GPC_SEL; toggle CALI mode
	btg	LC_STAT, LC_STAT_CALI_SEL
	rgoto	LC_SFB_CALI_Cont

; --------------------------------------------------------------------------
	;; hold CALI mode
LC_SFB_CALI_Hold
	;; if button pressed (MIOS_PARAMETER2[0] == 0): GPC on
	;; if button depressed (MIOS_PARAMETER2[0] == 1): GPC off
	bcf	LC_STAT, LC_STAT_GPC_SEL
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, LC_SFB_CALI_Hold_1
LC_SFB_CALI_Hold_0
	bcf	LC_STAT, LC_STAT_CALI_SEL; switch to logic control mode
	rgoto	LC_SFB_CALI_Cont
LC_SFB_CALI_Hold_1
	bsf	LC_STAT, LC_STAT_CALI_SEL; switch to motorfader calibration mode
	;; 	rgoto	LC_SFB_CALI_Cont
LC_SFB_CALI_Cont
	rcall	LC_CALI_Msg_Update	; update GPC message
	rcall	_LC_SFB_CALI_Cont	; set LEDs
	;; clear the first 16 DOUT LEDs
	clrf	TMP1
LC_SFB_CALI_Cont_Loop
	movf	TMP1, W
	call	MIOS_DOUT_PinSet0
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, LC_SFB_CALI_Cont_Loop
	return

; --------------------------------------------------------------------------
	;; disable ledmeters
LC_SFB_LEDMETER_Switch0
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	bcf	LC_STAT, LC_STAT_LEDMETERS; disable ledmeters
	rgoto	LC_SFB_LEDMETER_Cont
; --------------------------------------------------------------------------
	;; enable ledmeters
LC_SFB_LEDMETER_Switch1
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	bsf	LC_STAT, LC_STAT_LEDMETERS; enable ledmeters
	rgoto	LC_SFB_LEDMETER_Cont

; --------------------------------------------------------------------------
	;; toggle ledmeter mode
LC_SFB_LEDMETER_Toggle
	;; don't continue if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	btg	LC_STAT, LC_STAT_LEDMETERS; toggle ledmeter mode
	rgoto	LC_SFB_LEDMETER_Cont

; --------------------------------------------------------------------------
	;; hold ledmeter mode
LC_SFB_LEDMETER_Hold
	;; if button pressed (MIOS_PARAMETER2[0] == 0): enable ledmeters
	;; if button depressed (MIOS_PARAMETER2[0] == 1): disable ledmeters
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, LC_SFB_LEDMETER_Hold_1
LC_SFB_LEDMETER_Hold_0
	bcf	LC_STAT, LC_STAT_LEDMETERS; switch to layer 0
	rgoto	LC_SFB_LEDMETER_Cont
LC_SFB_LEDMETER_Hold_1
	bsf	LC_STAT, LC_STAT_LEDMETERS; switch to layer 1
	;; 	rgoto	LC_SFB_LEDMETER_Cont
LC_SFB_LEDMETER_Cont
	;; request update of all LEDrings
	movlw	0xff
	movwf	LC_LEDRING_UPDATE_REQ

	;; update LEDs
	rgoto	LC_SFB_LED_Update


;; --------------------------------------------------------------------------
;;  This function is used to update the LEDs which have been assigned
;;  to MBLC specific special functions
;; --------------------------------------------------------------------------
LC_SFB_LED_Update	; (also used by lc_init.inc!!!)
	;; clear all LEDs which have been assigned to ID_MBLC_DISPLAY_PAGEx
	movlw	ID_MBLC_DISPLAY_PAGE0
	movwf	TMP1
LC_SFB_LED_Update_Loop
	movff	TMP1, MIOS_PARAMETER2	; (parameters: see lc_leds.inc)
	clrf	MIOS_PARAMETER3
	call	LC_LED_Set
	incf	TMP1, F
	movlw	ID_MBLC_DISPLAY_PAGE3
	cpfsgt	TMP1, ACCESS
	rgoto LC_SFB_LED_Update_Loop

	;; set LEDs which have been assigned to the current page
	movf	LC_DISPLAY_PAGE, W
	addlw	ID_MBLC_DISPLAY_PAGE0
	movwf	MIOS_PARAMETER2
	movlw	0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; ---

	;; set SWITCH_LAYER0 led depending on status
	movlw	ID_MBLC_SWITCH_LAYER0
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfss	LC_STAT, LC_STAT_LAYER_SEL
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; set SWITCH_LAYER1 led depending on status
	movlw	ID_MBLC_SWITCH_LAYER1
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfsc	LC_STAT, LC_STAT_LAYER_SEL
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; set TOGGLE_LAYER led depending on status
	movlw	ID_MBLC_TOGGLE_LAYER
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfsc	LC_STAT, LC_STAT_LAYER_SEL
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; set HOLD_LAYER led depending on status
	movlw	ID_MBLC_HOLD_LAYER
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfsc	LC_STAT, LC_STAT_LAYER_SEL
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; ---

	;; set SWITCH_LC led depending on status
	movlw	ID_MBLC_SWITCH_LC
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfss	LC_STAT, LC_STAT_GPC_SEL
	movlw 0x01
	btfsc	LC_STAT, LC_STAT_CALI_SEL
	movlw 0x00
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; set SWITCH_GP led depending on status
	movlw	ID_MBLC_SWITCH_GPC
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfsc	LC_STAT, LC_STAT_GPC_SEL
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; set TOGGLE_GPC led depending on status
	movlw	ID_MBLC_TOGGLE_GPC
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfsc	LC_STAT, LC_STAT_GPC_SEL
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; set HOLD_GPC led depending on status
	movlw	ID_MBLC_HOLD_GPC
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfsc	LC_STAT, LC_STAT_GPC_SEL
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; ---

	;; set SWITCH_CALI led depending on status
	movlw	ID_MBLC_SWITCH_CALI
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfsc	LC_STAT, LC_STAT_CALI_SEL
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; set TOGGLE_CALI led depending on status
	movlw	ID_MBLC_TOGGLE_CALI
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfsc	LC_STAT, LC_STAT_CALI_SEL
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; set HOLD_CALI led depending on status
	movlw	ID_MBLC_HOLD_CALI
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfsc	LC_STAT, LC_STAT_CALI_SEL
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; ---

	;; set SWITCH_LEDMETER0 led depending on status
	movlw	ID_MBLC_SWITCH_LEDMETER0
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfss	LC_STAT, LC_STAT_LEDMETERS
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; set SWITCH_LEDMETER1 led depending on status
	movlw	ID_MBLC_SWITCH_LEDMETER1
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfsc	LC_STAT, LC_STAT_LEDMETERS
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; set TOGGLE_LEDMETER led depending on status
	movlw	ID_MBLC_TOGGLE_LEDMETER
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfsc	LC_STAT, LC_STAT_LEDMETERS
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	;; set HOLD_LEDMETER led depending on status
	movlw	ID_MBLC_HOLD_LEDMETER
	movwf	MIOS_PARAMETER2
	movlw	0x00
	btfsc	LC_STAT, LC_STAT_LEDMETERS
	movlw 0x01
	movwf	MIOS_PARAMETER3
	call	LC_LED_Set

	return
