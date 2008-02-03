; $Id$
;
; MIDIbox LC General Purpose Controller Routines
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called from lc_mproc.inc and updates a absolute value of 
;;  a V-Pot parameter (one of 128)
;; 
;;  IN:	
;;     o number of entry in WREG
;;     o first  MIDI event byte in MIOS_PARAMETER1 (not used here)
;;     o second MIDI event byte in MIOS_PARAMETER2 (not used here)
;;     o third  MIDI event byte in MIOS_PARAMETER3 (here: the v-pot value!)
;; --------------------------------------------------------------------------
LC_GPC_AbsValue_Set
	;; calculate pointer to V-Pot entry
	lfsr	FSR0, GPC_ABS_VALUES_BEGIN
	andlw	0x7f
	addwf	FSR0L, F

	;; copy value to register
	movff	MIOS_PARAMETER3, INDF0

	;; request LED ring update
	setf	LC_LEDRING_UPDATE_REQ

	return

;; --------------------------------------------------------------------------
;;  Updates the GPC message screen
;; --------------------------------------------------------------------------
LC_GPC_Msg_Update
	;; calculate address to first table entry (LC_GPC_LABLES + 6*GPC_OFFSET)
	TABLE_ADDR LC_GPC_LABLES
	movf	GPC_OFFSET, W
	mullw	6
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F

	;; copy 8 lables to the message buffer
	lfsr	FSR0, MSG_GPC_BEGIN
	clrf	TMP1
LC_GPC_Msg_Update_OuterLoop
	clrf	TMP2
LC_GPC_Msg_Update_InnerLoop
	tblrd*+
	movff	TABLAT, POSTINC0
	incf	TMP2, F
	movlw	5
	cpfsgt	TMP2, ACCESS
	rgoto LC_GPC_Msg_Update_InnerLoop

	movlw	' '
	movwf	POSTINC0

	incf	TMP1, F
	BRA_IFCLR TMP1, 3, ACCESS, LC_GPC_Msg_Update_OuterLoop

	;; 	rgoto	LC_GPC_Msg_Update_Values

;; --------------------------------------------------------------------------
;;  Updates the values of the GPC message screen
;; --------------------------------------------------------------------------
LC_GPC_Msg_Update_Values
	;; write values into message buffer, second line
	lfsr	FSR0, MSG_GPC_BEGIN+0x40
	lfsr	FSR1, GPC_ABS_VALUES_BEGIN
	movf	GPC_OFFSET, W
	addwf	FSR1L, F
	clrf	TMP1, W
LC_GPC_Msg_Update_ValuesLoop
	rcall	LC_GPC_Copy2Spaces
	movf	POSTINC1, W
	rcall	LC_GPC_CopyHex2	
	rcall	LC_GPC_Copy3Spaces
	incf	TMP1, F
	BRA_IFCLR TMP1, 3, ACCESS, LC_GPC_Msg_Update_ValuesLoop

	return

LC_GPC_CopyHex2
	movwf	TMP5
	swapf	TMP5, W
	rcall	LC_GPC_CopyHex1
	movf	TMP5, W
	;; 	rgoto	LC_GPC_CopyHex1
LC_GPC_CopyHex1
	andlw	0x0f
	addlw	-0x0a
	skpnc
	addlw	0x07
	addlw	0x3a
	movwf	POSTINC0
	return

LC_GPC_Copy2Spaces
	movlw	0x02
	rgoto	LC_GPC_CopySpaces
LC_GPC_Copy3Spaces
	movlw	0x03
	;; 	rgoto	LC_GPC_CopySpaces
LC_GPC_CopySpaces
	movwf	TMP5
	movlw	' '
LC_GPC_CopySpaces_Loop	
	movwf	POSTINC0
	decfsz	TMP5, F
	rgoto	LC_GPC_CopySpaces_Loop
	return

;; --------------------------------------------------------------------------
;;  V-Pot handler for GPC mode
;; --------------------------------------------------------------------------
LC_GPC_SendVPot
	;; store incrementer in TMP3
	movff	MIOS_PARAMETER2, TMP3	; (MIOS_PARAMETER2 contains the incrementer)

	;; calc address to absolute value
	lfsr	FSR0, GPC_ABS_VALUES_BEGIN
	movf	MIOS_PARAMETER1, W	; (MIOS_PARAMETER1 contains the v-pot index)
	addwf	GPC_OFFSET, W		; add GPC offset
	addwf	FSR0L, F

	;; request LED ring update
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	iorwf	LC_LEDRING_UPDATE_REQ, F

	;; inc/dec value by using the 16 bit add w/ saturation routine
	;; set pointer to TMP1 (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, TMP1
	;; copy current absolute value to this register
	movff	INDF0, TMP1
	clrf	TMP2
	;; set max value
	movlw	0x7f		; low-byte of max value
	movwf	MIOS_PARAMETER1
	movlw	0x00		; high-byte of max value
	movwf	MIOS_PARAMETER2
	;; get incrementer (which has been stored in TMP3)
	movf	TMP3, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = CS_MENU_PARAMETER_[LH] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; do nothing if value has not been changed
	btfss	MIOS_PARAMETER1, 0
	return

	;; else store new value in the GPC array
	;; result in [FSR1]==INDF1, target register in [FSR0]==INDF0
	movff	INDF1, INDF0

	;; now calc address to MIDI event:	MIOS_MPROC_EVENT_TABLE + (parameter number * 2)
	TABLE_ADDR MIOS_MPROC_EVENT_TABLE	; this macro sets the base address
	movf	FSR0L, W			; get vpot-number (0-7)
	andlw	0x7f
	mullw	2				; multiplicate by *2
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F			; low byte
	movf	PRODH, W
	addwfc	TBLPTRH, F			; high-byte (+carry bit on overrun)
	
	;; read first MIDI byte
	tblrd*+

	;; exit routine if no event has been defined for this pot (entry = 0xff)
	incf	TABLAT, W
	skpnz
	return

	;; for MIDIbox Link: notify begin of stream
	call	MIOS_MIDI_BeginStream

	;; send first MIDI byte
	movf	TABLAT, W
	call	MIOS_MIDI_TxBufferPut

	;; read second MIDI byte and send it
	tblrd*+
	movf	TABLAT, W
	call	MIOS_MIDI_TxBufferPut

	;; send third MIDI byte: the absolute value
	;; pointer to value still in [FSR0]=INDF0
	movf	INDF0, W
	andlw	0x7f		; ensure 7-bit value
	call	MIOS_MIDI_TxBufferPut		; and send it

	;; for MIDIbox Link: notify end of stream
	call	MIOS_MIDI_EndStream

	;; request display message update
	bsf	LC_STAT, LC_STAT_MSG_UPDATE_REQ

	;; update values of message screen
	rgoto	LC_GPC_Msg_Update_Values

;; --------------------------------------------------------------------------
;;  Jogwheel handler for GPC mode
;; --------------------------------------------------------------------------
LC_GPC_SendJogWheel
	;; store incrementer in TMP3
	movff	MIOS_PARAMETER2, TMP3	; (MIOS_PARAMETER2 contains the incrementer)

	;; inc/dec value by using the 16 bit add w/ saturation routine
	;; set pointer to TMP1 (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, TMP1
	;; copy current GPC offset to this register
	movff	GPC_OFFSET, TMP1
	clrf	TMP2
	;; set max value
	movlw	0x7f-7		; low-byte of max value
	movwf	MIOS_PARAMETER1
	movlw	0x00		; high-byte of max value
	movwf	MIOS_PARAMETER2
	;; get incrementer (which has been stored in TMP3)
	movf	TMP3, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = CS_MENU_PARAMETER_[LH] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; do nothing if value has not been changed
	btfss	MIOS_PARAMETER1, 0
	return

	;; copy new value to offset
	movff	INDF1, GPC_OFFSET

	;; update the GPC message
	call	LC_GPC_Msg_Update

	;; if not in GPC mode, do nothing else
	btfss	LC_STAT, LC_STAT_GPC_SEL
	return

	;; else request MSG display update
	bsf	LC_STAT, LC_STAT_MSG_UPDATE_REQ

	;; and request LEDring update
	setf	LC_LEDRING_UPDATE_REQ
	
	return
