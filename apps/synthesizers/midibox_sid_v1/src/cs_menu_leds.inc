; $Id$
;
; SID Control Surface LED Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by the menu handler in cs_menu.inc
;;  when all LEDs should be updated
;;  The DOUT pin numbers are defined in cs_menu_io_tables.inc
;; --------------------------------------------------------------------------
CS_MENU_LED_Update
	;; get current CS_MENU_SID and base address
	call	CS_MENU_MS_GetSIDNumber

	;; set LEDs of modulation matrix:
	lfsr	FSR1, CS_MENU_MATRIX_BEGIN	; pointer for LEDs: FSR1
	call	CS_MENU_MS_GetSIDBase		; pointer for SID: FSR0

	clrf	INDF1
	movlw	CS_SID_VOICE1_OFFSET+CS_SID_VOICEx_ENV
	movf	PLUSW0, W	; ENV
	andlw	0x03
	movwf	INDF1
	movlw	CS_SID_VOICE1_OFFSET+CS_SID_VOICEx_LFO_PITCH
	rlf	PLUSW0, W	; LFO << 2
	rlf	WREG, W
	andlw	0xfc
	iorwf	POSTINC1, F
	
	clrf	INDF1
	movlw	CS_SID_VOICE2_OFFSET+CS_SID_VOICEx_ENV
	movf	PLUSW0, W	; ENV
	andlw	0x03
	movwf	INDF1
	movlw	CS_SID_VOICE2_OFFSET+CS_SID_VOICEx_LFO_PITCH
	rlf	PLUSW0, W	; LFO << 2
	rlf	WREG, W
	andlw	0xfc
	iorwf	POSTINC1, F
	
	clrf	INDF1
	movlw	CS_SID_VOICE3_OFFSET+CS_SID_VOICEx_ENV
	movf	PLUSW0, W	; ENV
	andlw	0x03
	movwf	INDF1
	movlw	CS_SID_VOICE3_OFFSET+CS_SID_VOICEx_LFO_PITCH
	rlf	PLUSW0, W	; LFO << 2
	rlf	WREG, W
	andlw	0xfc
	iorwf	POSTINC1, F
	
	clrf	INDF1
	movlw	CS_SID_VOICE1_OFFSET+CS_SID_VOICEx_ENV
	swapf	PLUSW0, W	; ENV
	andlw	0x03
	movwf	INDF1
	movlw	CS_SID_VOICE1_OFFSET+CS_SID_VOICEx_LFO_PW
	rlf	PLUSW0, W	; LFO << 2
	rlf	WREG, W
	andlw	0xfc
	iorwf	POSTINC1, F
	
	clrf	INDF1
	movlw	CS_SID_VOICE2_OFFSET+CS_SID_VOICEx_ENV
	swapf	PLUSW0, W	; ENV
	andlw	0x03
	movwf	INDF1
	movlw	CS_SID_VOICE2_OFFSET+CS_SID_VOICEx_LFO_PW
	rlf	PLUSW0, W	; LFO << 2
	rlf	WREG, W
	andlw	0xfc
	iorwf	POSTINC1, F
	
	clrf	INDF1
	movlw	CS_SID_VOICE3_OFFSET+CS_SID_VOICEx_ENV
	swapf	PLUSW0, W	; ENV
	andlw	0x03
	movwf	INDF1
	movlw	CS_SID_VOICE3_OFFSET+CS_SID_VOICEx_LFO_PW
	rlf	PLUSW0, W	; LFO << 2
	rlf	WREG, W
	andlw	0xfc
	iorwf	POSTINC1, F
	
	clrf	INDF1
	movlw	CS_SID_FILTER_ENV
	movf	PLUSW0, W	; ENV
	movwf	INDF1
	movlw	CS_SID_FILTER_LFO
	rlf	PLUSW0, W	; LFO << 2
	rlf	WREG, W
	andlw	0xfc
	iorwf	POSTINC1, F
	
	

	;; clear temporary registers which hold some LED flags which are not saved anywhere else
	;; (see also CS_MENU_DOUT_TABLE)
	;; this quick & dirty method saves some memory...
	clrf	TMP1
	clrf	TMP2
	clrf	TMP3

	;; TMP1[2..0]: selected OSC layer
	movf	CS_MENU_SELECTED_OSC_CTRL, W
	call	MIOS_HLP_GetBitORMask
	andlw	0x07
	movwf	TMP1

	;; TMP1[7..4]: waveforms
	call	CS_MENU_MS_GetSIDBase
	call	CS_MENU_Hlp_AddVoiceOffset
	movlw	CS_SID_VOICEx_WAVEFORM
	addwf	FSR0L, F
	swapf	INDF0, W
	andlw	0xf0
	btfsc	INDF0, 4; (voice off)
	movlw 0x00
	iorwf	TMP1, F

	;; TMP2[1..0]: sync/ring	
	swapf	INDF0, W
	rrf	WREG, W
	andlw	0x03
	movwf	TMP2

	;; TMP2[4..5]: selected envelope
	movf	CS_MENU_SELECTED_ENV, W
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
	andlw	0x30
	iorwf	TMP2, F

	;; TMP2[6..7]: selected ENV Ctrl
	movf	CS_MENU_SELECTED_ENV_CTRL, W
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, F
	rlf	WREG, F
	rlf	WREG, W
	andlw	0xc0
	iorwf	TMP2, F
	
	;; switch back to SID_BASE, offset 0
	movlw	0x80
	andwf	FSR0L, F

	;; TMP3[3..0]: filter O[123E]
	movlw	CS_SID_FILTER_CHANNELS
	movf	PLUSW0, W
	movwf	TMP3

	;; TMP3[7..4]: filter LP/BP/HP/3O
	movlw	CS_SID_FILTER_MODE
	swapf	PLUSW0, W
	iorwf	TMP3, F

	;; TMP4[5..0]: selected LFO
	movf	CS_MENU_SELECTED_LFO, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP4

	;; TMP5[4..0]: LFO waveform
	call	CS_MENU_MS_GetSIDBase
	call	CS_MENU_Hlp_AddLFOOffset
	swapf	INDF0, W
	andlw	0x07
	rcall	CS_MENU_LED_LFO_Wav_Pattern
	btfss	INDF0, 0
	movlw 0x00
	movwf	TMP5

	;; ----

	;; go through the DOUT map
	TABLE_ADDR CS_MENU_DOUT_TABLE
CS_MENU_LED_Update_Loop
	tblrd*+				; get low-byte of register address
	movf	TABLAT, W		; load it in WREG
	movwf	FSR0L			; and store it in FSR0L
	tblrd*+				; get high byte
	movff	TABLAT, FSR0H		; store it in FSR0H without touching WREG
	iorwf	TABLAT, W		; or the byte with WREG --- if both bytes are zero, the result will be zero
	bz	CS_MENU_LED_Update_End	; in this case jump to the end

	tblrd*+				; get flag-number
	movf	TABLAT, W
	call	MIOS_HLP_GetBitORMask	; get appr. OR mask (1 of 8)
	andwf	INDF0, W		; AND this with the register specified in the table
	skpz				; LED value: 0 if result zero, 1 if result != zero
	movlw	0x01
	movwf	MIOS_PARAMETER1		; store result in MIOS_PARAMETER1
	tblrd*+				; read pin number from table
	movf	TABLAT, W		; get pin number
	CALL_IFCLR TABLAT, 7, ACCESS, MIOS_DOUT_PinSet	; and finally set the pin of SR if bit #7 cleared
	CALL_IFSET TABLAT, 7, ACCESS, J5_DOUT_PinSet		; or pin of J5 if bit #7 set (SR #0 in cs_menu_io_tables.inc)
	rgoto	CS_MENU_LED_Update_Loop	; next entry

CS_MENU_LED_Update_End
	return


;; --------------------------------------------------------------------------
;; we have 5 LFO LEDs
;; this function maps the LFO waveform number to these LEDs
;; --------------------------------------------------------------------------
CS_MENU_LED_LFO_Wav_Pattern
	andlw	0x07		; 8 entries
	JUMPTABLE_2BYTES_UNSECURE
	retlw	b'00000001'	; Sine
	retlw	b'00000010'	; Triangle
	retlw	b'00000100'	; Saw
	retlw	b'00001000'	; Pulse
	retlw	b'00010000'	; Random
	retlw	b'00010001'	; AIN
	retlw	b'00010010'	; unused
	retlw	b'00010100'	; unused
	
