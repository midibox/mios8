; $Id$
;
; MIOS Application Encoder Driver
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an encoder has been moved
;;  Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
USER_ENC_NotifyChange
TC_ENC_Handler
	;; only one encoder has been connected (see mios_tables::MIOS_ENC_PIN_TABLE)
	;; ignore all other events
	movlw	0x00		; encoder #0
	cpfseq	MIOS_PARAMETER1, ACCESS
	return

	;; branch depending on current mode
	movf	TC_MODE, W
	JUMPTABLE_2BYTES 2	; 2 entries
	rgoto	TC_ENC_Handler_Menu
	rgoto	TC_ENC_Handler_Para
	;; you could add more modes and actions here...

	;; ------------------------------------------------------------------
	;; if TC_MODE == TC_MODE_MENU
TC_ENC_Handler_Menu
	;; branch depending on clockwise/counter clockwise turn
	BRA_IFSET MIOS_PARAMETER2, 7, ACCESS, TC_ENC_Handler_Menu_Dec
TC_ENC_Handler_Menu_Inc
	;; add incrementer to current parameter value
	movf	MIOS_PARAMETER2, W
	addwf	TC_BANK, F
	;; continue if value <= 256/VALUES_PER_PAGE-1 (normaly we have 51 banks: 0, 1, 2, ... 50)
	movlw	256/VALUES_PER_PAGE-1
	cpfsgt	TC_BANK, ACCESS
	rgoto TC_ENC_Handler_Menu_Cont
	;; else set max value
	movlw	256/VALUES_PER_PAGE-1
	movwf	TC_BANK
	rgoto	TC_ENC_Handler_Menu_Cont

TC_ENC_Handler_Menu_Dec
	;; subtract incrementer from current parameter value
	movf	MIOS_PARAMETER2, W	; (it's a negative value...)
	addwf	TC_BANK, F
	;; continue if no overflow
	bc	TC_ENC_Handler_Menu_Cont
	;; else set value to zero
	clrf	TC_BANK
	;; 	rgoto	TC_ENC_Handler_Menu_Cont
TC_ENC_Handler_Menu_Cont
	;; now request a display update so that we see the new bank on screen
	bsf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ	; (see tc_display.inc)

	;; thats all
	return

	
	;; ------------------------------------------------------------------
	;; if TC_MODE == TC_MODE_PARA
TC_ENC_Handler_Para
	;; calculate pointer to parameter: FSR0 = TC_VALUES_BEGIN + VALUES_PER_PAGE*TC_BANK + TC_PARA
	lfsr	FSR0, TC_VALUES_BEGIN	; base address
	movf	TC_BANK, W		; Bank
	mullw	VALUES_PER_PAGE		; * 5 -> result in PRODL (PRODH can be ignored here, since we have 256 parameters max)
	movf	PRODL, W		; add TC_PARA
	addwf	TC_PARA, W
	movwf	FSR0L			; store result in FSR0L (Note: FSR0H already contains
					; high byte of TC_VALUES_BEGIN due to the lfsr instruction)

	;; save current value in TMP1, we want to compare it later with the new value
	movff	INDF0, TMP1

	;; branch depending on clockwise/counter clockwise turn
	BRA_IFSET MIOS_PARAMETER2, 7, ACCESS, TC_ENC_Handler_Para_Dec
TC_ENC_Handler_Para_Inc
	;; add incrementer to current parameter value
	movf	MIOS_PARAMETER2, W
	addwf	INDF0, F
	;; continue if value <= 0x7f
	movlw	0x7f
	cpfsgt	INDF0, ACCESS
	rgoto TC_ENC_Handler_Para_Cont
	;; else set max value
	movlw	0x7f
	movwf	INDF0
	rgoto	TC_ENC_Handler_Para_Cont

TC_ENC_Handler_Para_Dec
	;; subtract incrementer from current parameter value
	movf	MIOS_PARAMETER2, W	; (it's a negative value...)
	addwf	INDF0, F
	;; continue if no overflow
	bc	TC_ENC_Handler_Para_Cont
	;; else set value to zero
	clrf	INDF0
	;; 	rgoto	TC_ENC_Handler_Para_Cont
TC_ENC_Handler_Para_Cont

	;; now compare new with last value
	movf	INDF0, W
	xorwf	TMP1, W			; which has been stored in TMP1
	bz	TC_ENC_DontSendMIDI	; branch to "DontSendMIDI" if values are equal

	;; send MIDI event
	;; calc address to MIDI event:	MIOS_MPROC_EVENT_TABLE + (2*parameter index (FSR0L))
	TABLE_ADDR MIOS_MPROC_EVENT_TABLE	; this macro sets the base address
	movf	FSR0L, W			; parameter index
	mullw	2				; *2, result in PROD[LH]
	movf	PRODL, W			; add offset (index*2) to TABLPTR
	addwf	TBLPTRL, F			; low-byte
	movf	PRODH, W
	addwfc	TBLPTRH, F			; high-byte (+carry bit on overrun)

	;; read first MIDI byte
	tblrd*+

	;; don't send MIDI event if no event has been defined for this parameter (entry = 0xff)
	incf	TABLAT, W
	bz	TC_ENC_DontSendMIDI

TC_ENC_SendMIDI
	;; for MIDIbox Link: notify begin of stream
	call	MIOS_MIDI_BeginStream

	;; send first MIDI byte
	movf	TABLAT, W
	call	MIOS_MIDI_TxBufferPut

	;; read second MIDI byte and send it
	tblrd*+
	movf	TABLAT, W
	call	MIOS_MIDI_TxBufferPut

	;; send third MIDI byte: the paramter value, stored in INDF0
	movf	INDF0, W
	call	MIOS_MIDI_TxBufferPut

	;; for MIDIbox Link: notify end of stream
	call	MIOS_MIDI_EndStream

TC_ENC_DontSendMIDI
	;; now request a display update so that we see the new parameter on screen
	bsf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ	; (see tc_display.inc)

	;; request a temporary "long label" message
	bsf	TC_STAT, TC_STAT_DISPLAY_LONGLABEL_REQ

	;; thats all
	return
