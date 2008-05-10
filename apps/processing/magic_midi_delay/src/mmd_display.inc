; $Id$
;
; MIOS Application Display Driver
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; messages can be requested whenever a value has been changed
;; via MIDI/Encoders/Pots/Buttons

MMD_DISPLAY_REQUEST_DLY		EQU	0x01
MMD_DISPLAY_REQUEST_TRANSP	EQU	0x02
MMD_DISPLAY_REQUEST_CHN		EQU	0x03
MMD_DISPLAY_REQUEST_CFG		EQU	0x04

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the display content should be 
;;  initialized. Thats the case during startup and after a temporary message
;;  has been printed on the screen
;; --------------------------------------------------------------------------
USER_DISPLAY_Init
	goto	MMD_DISPLAY_DlyMessage	; print delay values by default

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	;; check if a message has been requested
	movf	MMD_DISPLAY_REQUEST, W
	skpnz
	return

	;; call routine which displays a message
	call	USER_DISPLAY_Tick_Request

	;; and clear request
	clrf	MMD_DISPLAY_REQUEST
	
	return


;; --------------------------------------------------------------------------
;;  This function branches to a routine which displays a message
;;  depending on MMD_DISPLAY_REQUEST
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick_Request
	decf	MMD_DISPLAY_REQUEST, W		; get request ID - 1
	JUMPTABLE_2BYTES 4			; 4 supported messages
	rgoto	MMD_DISPLAY_DlyMessage	
	rgoto	MMD_DISPLAY_TranspMessage	
	rgoto	MMD_DISPLAY_ChnMessage	
	rgoto	MMD_DISPLAY_CfgMessage	
	

;; --------------------------------------------------------------------------
;;  This function is used to print a message with the transpose values
;; --------------------------------------------------------------------------
TEXT_DLY_0	STRING 16, 0x00, "DY0 DY1 DY2 DY3 "

MMD_DISPLAY_DlyMessage	
	TABLE_ADDR TEXT_DLY_0
	call	MIOS_LCD_PrintString	; print first line

	;; initial cursor pos: second line (0x40+), first column (0)
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print delay of queue 0 as BCD (decimal) value
	movf	MMD_QUEUE0_DLY, W
	call	MIOS_LCD_PrintBCD3

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print delay of queue 1 as BCD (decimal) value
	movf	MMD_QUEUE1_DLY, W
	call	MIOS_LCD_PrintBCD3

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print delay of queue 2 as BCD (decimal) value
	movf	MMD_QUEUE2_DLY, W
	call	MIOS_LCD_PrintBCD3

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print delay of queue 3 as BCD (decimal) value
	movf	MMD_QUEUE3_DLY, W
	call	MIOS_LCD_PrintBCD3

	return


;; --------------------------------------------------------------------------
;;  This function is used to print a message with the transpose values
;; --------------------------------------------------------------------------
TEXT_TRANSP_0	STRING 16, 0x00, "Tr0 Tr1 Tr2 Tr3 "

MMD_DISPLAY_TranspMessage	
	TABLE_ADDR TEXT_TRANSP_0
	call	MIOS_LCD_PrintString

	;; initial cursor pos: second line (0x40+), first column (0)
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print transpose of queue 0 as signed decimal  value
	movf	MMD_QUEUE0_TRANSP, W
	call	MMD_DISPLAY_SignedBCD3

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print transpose of queue 1 as signed decimal value
	movf	MMD_QUEUE1_TRANSP, W
	call	MMD_DISPLAY_SignedBCD3

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print transpose of queue 2 as signed decimal value
	movf	MMD_QUEUE2_TRANSP, W
	call	MMD_DISPLAY_SignedBCD3

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print transpose of queue 3 as signed decimal value
	movf	MMD_QUEUE3_TRANSP, W
	call	MMD_DISPLAY_SignedBCD3

	return


;; --------------------------------------------------------------------------
;;  This help function displays a signed BCD value (-64...+63)
;; --------------------------------------------------------------------------
MMD_DISPLAY_SignedBCD3
	movwf	TMP1			; temporary save value in TMP1
	movlw	' '		; space or "-"?
	btfss	TMP1, 6; (if value <= 0x3f (6th bit cleared), print "-"
	movlw '-'
	call	MIOS_LCD_PrintChar

	movf	TMP1, W			; calc: 0x40-value
	sublw	0x40
	btfsc	WREG, 7			; negate value if negative to get absolute value
	negf	WREG, ACCESS
	goto	MIOS_LCD_PrintBCD2
	

;; --------------------------------------------------------------------------
;;  This function is used to print a message with the channel settings
;; --------------------------------------------------------------------------
TEXT_CHN_0	STRING 16, 0x00, "Ch0 Ch1 Ch2 Ch3 "

MMD_DISPLAY_ChnMessage	
	TABLE_ADDR TEXT_CHN_0
	call	MIOS_LCD_PrintString

	;; initial cursor pos: second line (0x40+), first column (0)
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print channel of queue 0 as BCD (decimal) value
	movf	MMD_QUEUE0_CHN, W
	rcall	MMD_DISPLAY_PrintChn

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print channel of queue 1 as BCD (decimal) value
	movf	MMD_QUEUE1_CHN, W
	rcall	MMD_DISPLAY_PrintChn

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print channel of queue 2 as BCD (decimal) value
	movf	MMD_QUEUE2_CHN, W
	rcall	MMD_DISPLAY_PrintChn

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print channel of queue 3 as BCD (decimal) value
	movf	MMD_QUEUE3_CHN, W
	rcall	MMD_DISPLAY_PrintChn

	return


;; --------------------------------------------------------------------------
;;  This help function displays the channel number if value > 0, else " --"
;; --------------------------------------------------------------------------
MMD_DISPLAY_PrintChn
	bz	MMD_DISPLAY_PrintChn_Dis
	goto	MIOS_LCD_PrintBCD3

MMD_DISPLAY_PrintChn_Dis
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	goto	MIOS_LCD_PrintChar


;; --------------------------------------------------------------------------
;;  This function is used to print a message with the config settings
;; --------------------------------------------------------------------------
TEXT_CFG_0	STRING 16, 0x00, "InC Cfg     BPM "

MMD_DISPLAY_CfgMessage	
	TABLE_ADDR TEXT_CFG_0
	call	MIOS_LCD_PrintString

	;; initial cursor pos: second line (0x40+), first column (0)
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print MIDI in channel as BCD (decimal) value
	movf	MMD_MIDI_IN_CHN, W
	call	MMD_DISPLAY_PrintChn

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print config channel as BCD (decimal) value
	movf	MMD_MIDI_IN_CFG_CHN, W
	call	MMD_DISPLAY_PrintChn

	;; print spaces
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print BPM as BCD (decimal) value
	movf	MMD_BPM, W
	call	MIOS_LCD_PrintBCD3

	return
