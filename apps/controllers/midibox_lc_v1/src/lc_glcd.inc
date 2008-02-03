; $Id$
;
; MIDIbox LC Display Driver for Graphical LCDs
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; include the initialization macros used by LC_GLCD_Init_Page*
#include "lc_glcd_macros.inc"

;; --------------------------------------------------------------------------
;;  Initialize the screen depending on selected display page
;; --------------------------------------------------------------------------
LC_GLCD_Init
	;; fetch the display mode settings
	rcall	LC_GLCD_Init_FetchSettings

	;; call all display element handlers to built the screen layout
	rcall	LC_GLCD_Print_STATUS_Digits
	rcall	LC_GLCD_Print_SMPTE_BEATS
	rcall	LC_GLCD_Print_MTC_Digits
	rcall	LC_GLCD_Print_Msg
	rcall	LC_GLCD_Print_Meters
	rcall	LC_GLCD_Print_Knobs
	return

LC_GLCD_Init_FetchSettings
	movf	LC_DISPLAY_PAGE, W
	JUMPTABLE_2BYTES 4	; 4 entries
	rgoto	LC_GLCD_Init_Page0
	rgoto	LC_GLCD_Init_Page1
	rgoto	LC_GLCD_Init_Page2
	rgoto	LC_GLCD_Init_Page3

;; --------------------------------------------------------------------------
;;  Page0: 
;; --------------------------------------------------------------------------
LC_GLCD_Init_Page0
	;; print status digits at position 12/4 (16 pixels between chars)
	SET_GLCD_STATUS GLCD_FONT_BIG, 12, 4, 16
	
	;; print "SMPTE/BEATS" at position: 56/5
	SET_GLCD_SMPTE_BEATS GLCD_FONT_SMALL, 56, 5

	;; print MTC digits at position 70/4 (12 pixels between chars)
	SET_GLCD_MTC GLCD_FONT_BIG, 70, 4, 12

	;; print host messages at position 2/1 (4 pixels between chars)
	SET_GLCD_HOST_MSG GLCD_FONT_SMALL, 2, 1, 4

	;; don't print meters
	SET_GLCD_METER 0, 0, 0, 0
	
	;; don't print knobs
	SET_GLCD_KNOB 0, 0, 0, 0
	
	return


;; --------------------------------------------------------------------------
;;  Page1: 
;; --------------------------------------------------------------------------
LC_GLCD_Init_Page1
	;; print status digits at position 12/4 (16 pixels between chars)
	SET_GLCD_STATUS GLCD_FONT_BIG, 12, 4, 16
	
	;; print "SMPTE/BEATS" at position: 56/5
	SET_GLCD_SMPTE_BEATS GLCD_FONT_SMALL, 56, 5

	;; print MTC digits at position 70/4 (12 pixels between chars)
	SET_GLCD_MTC GLCD_FONT_BIG, 70, 4, 12

	;; print host messages at position 2/1 (4 pixels between chars)
	SET_GLCD_HOST_MSG GLCD_FONT_SMALL, 2, 1, 4

	;; print horizontal meters at position 10/0 (28 pixels between icons)
	SET_GLCD_METER GLCD_FONT_METER_ICONS_H, 10, 0, 28
	
	;; don't print knobs
	SET_GLCD_KNOB 0, 0, 0, 0
	
	return


;; --------------------------------------------------------------------------
;;  Page2: 
;; --------------------------------------------------------------------------
LC_GLCD_Init_Page2
	;; print status digits at position 12/7 (8 pixels between chars)
	SET_GLCD_STATUS MIOS_GLCD_FONT, 12, 7, 8
	
	;; print "SMPTE/BEATS" at position: 92/7
	SET_GLCD_SMPTE_BEATS MIOS_GLCD_FONT, 92, 7

	;; print MTC digits at position 124/7 (8 pixels between chars)
	SET_GLCD_MTC MIOS_GLCD_FONT, 124, 7, 8

	;; print host messages at position 2/1 (4 pixels between chars)
	SET_GLCD_HOST_MSG GLCD_FONT_SMALL, 2, 1, 4

	;; print vertical meters at position 18/3 (28 pixels between icons)
	SET_GLCD_METER GLCD_FONT_METER_ICONS_V, 18, 3, 28
	
	;; don't print knobs
	SET_GLCD_KNOB 0, 0, 0, 0
	
	return


;; --------------------------------------------------------------------------
;;  Page3: 
;; --------------------------------------------------------------------------
LC_GLCD_Init_Page3
	;; print status digits at position 12/7 (8 pixels between chars)
	SET_GLCD_STATUS MIOS_GLCD_FONT, 12, 7, 8
	
	;; print "SMPTE/BEATS" at position: 92/7
	SET_GLCD_SMPTE_BEATS MIOS_GLCD_FONT, 92, 7

	;; print MTC digits at position 124/7 (8 pixels between chars)
	SET_GLCD_MTC MIOS_GLCD_FONT, 124, 7, 8

	;; print host messages at position 2/1 (4 pixels between chars)
	SET_GLCD_HOST_MSG GLCD_FONT_SMALL, 2, 1, 4

	;; print horizontal meters at position 10/3 (28 pixels between icons)
	SET_GLCD_METER GLCD_FONT_METER_ICONS_H, 10, 3, 28
	
	;; print knobs at position 8/4 (28 pixels between icons)
	SET_GLCD_KNOB GLCD_FONT_KNOB_ICONS, 8, 4, 28
	
	return


;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------
;;  Help functions used by the different display page handlers
;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  This function is called by lc_display::LC_DISPLAY_LEDStatusUpdate
;;  when any 90 xx xx event has been received
;;  IN: LED ID in WREG
;;      
;; --------------------------------------------------------------------------
LC_GLCD_LEDStatusUpdate
	;; save ID in TMP1
	movwf	TMP1

	;; prepare FSR0
	lfsr	FSR0, LED_STATUS_BEGIN
	addwf	FSR0L, F

	;; skip SMPTE and BEATS check if "90xx00" has been received
	movf	INDF0, W
	bz	LC_GLCD_LEDStatusUpdate_Skip
	
	;; print SMPTE if 90 71 7F has been received
	movf	TMP1, W
	xorlw	0x71
	bz	LC_GLCD_Print_SMPTE

	;; print BEATS if 90 72 7F has been received
	movf	TMP1, W
	xorlw	0x72
	bz	LC_GLCD_Print_BEATS

LC_GLCD_LEDStatusUpdate_Skip
	return



;; --------------------------------------------------------------------------
;;  print SMPTE or BEATS depending on LC_STAT_SMPTE flag which is
;;  directly controlled by the host application (Logic, Cubase, etc...)
;; --------------------------------------------------------------------------
LC_GLCD_Print_SMPTE_BEATS
	BRA_IFSET LC_STAT, LC_STAT_SMPTE, ACCESS, LC_GLCD_Print_SMPTE
	rgoto	LC_GLCD_Print_BEATS

;; --------------------------------------------------------------------------
;;  This function is called by LC_GLCD_LEDStatusUpdate when
;;  "90 71 7F" has been received
;; --------------------------------------------------------------------------
LC_GLCD_Print_SMPTE
	;; force display mode to SMPTE
	bsf	LC_STAT, LC_STAT_SMPTE

	;; don't print if LCD_SMPTE_BEATS_FONT_H is 0
	movf	LCD_SMPTE_BEATS_FONT_H, F
	skpnz
	return

	;; initialize the font
	movff	LCD_SMPTE_BEATS_FONT_L, TBLPTRL
	movff	LCD_SMPTE_BEATS_FONT_H, TBLPTRH
	call	MIOS_GLCD_FontInit

	;; set graphical cursor
	movff	LCD_SMPTE_BEATS_Y, MIOS_PARAMETER1
	movf	LCD_SMPTE_BEATS_X, W
	call	MIOS_GLCD_GCursorSet

	;; print "SMPTE"
	movlw	'S'
	call	MIOS_LCD_PrintChar
	movlw	'M'
	call	MIOS_LCD_PrintChar
	movlw	'P'
	call	MIOS_LCD_PrintChar
	movlw	'T'
	call	MIOS_LCD_PrintChar
	movlw	'E'
	call	MIOS_LCD_PrintChar

	rgoto	LC_GLCD_Print_MTC_Digits

;; --------------------------------------------------------------------------
;;  This function is called by LC_GLCD_LEDStatusUpdate when
;;  "90 72 7F" has been received
;; --------------------------------------------------------------------------
LC_GLCD_Print_BEATS
	;; force display mode to BEATS
	bcf	LC_STAT, LC_STAT_SMPTE

	;; don't print if LCD_SMPTE_BEATS_FONT_H is 0
	movf	LCD_SMPTE_BEATS_FONT_H, F
	skpnz
	return

	;; initialize the font
	movff	LCD_SMPTE_BEATS_FONT_L, TBLPTRL
	movff	LCD_SMPTE_BEATS_FONT_H, TBLPTRH
	call	MIOS_GLCD_FontInit

	;; set graphical cursor to position 46/05
	movff	LCD_SMPTE_BEATS_Y, MIOS_PARAMETER1
	movf	LCD_SMPTE_BEATS_X, W
	call	MIOS_GLCD_GCursorSet

	;; print "BEATS"
	movlw	'B'
	call	MIOS_LCD_PrintChar
	movlw	'E'
	call	MIOS_LCD_PrintChar
	movlw	'A'
	call	MIOS_LCD_PrintChar
	movlw	'T'
	call	MIOS_LCD_PrintChar
	movlw	'S'
	call	MIOS_LCD_PrintChar

	rgoto	LC_GLCD_Print_MTC_Digits

;; --------------------------------------------------------------------------


;; --------------------------------------------------------------------------
;;  This function is called by LC_GLCD_Print_SMPTE and LC_GLCD_Print_BEATS
;;  when all MTC digits should be updated
;; --------------------------------------------------------------------------
LC_GLCD_Print_MTC_Digits
	;; print the MTC digits and colons
	clrf	TMP5
LC_GLCD_Print_MTC_Digits_Loop
	movff	TMP5, MIOS_PARAMETER2
	lfsr	FSR0, LC_MTC_DIGIT_0
	movf	TMP5, W
	movff	PLUSW0, MIOS_PARAMETER3
	rcall	LC_GLCD_Print_MTC_Digit
	incf	TMP5, F
	movlw	14
	cpfseq	TMP5, ACCESS
	rgoto LC_GLCD_Print_MTC_Digits_Loop

	return

;; --------------------------------------------------------------------------
;;  This function is called by lc_mproc::LC_MPROC_Received_B04x,
;;  lc_mproc::LC_MPROC_Action_WriteMTC* and some functions of
;;  this file when a MTC digit should be print
;;  IN: digit position in MIOS_PARAMETER2
;;      digit value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
LC_GLCD_Print_MTC_Digit
	;; don't print if LCD_MTC_FONT_H is 0
	movf	LCD_MTC_FONT_H, F
	skpnz
	return

	;; initialize the font
	movff	LCD_MTC_FONT_L, TBLPTRL
	movff	LCD_MTC_FONT_H, TBLPTRH
	call	MIOS_GLCD_FontInit

	;; set graphical cursor depending on digit position
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	call	LC_HLP_MTCPositionGet
	BRA_IFSET WREG, 7, ACCESS, LC_GLCD_Print_MTC_Digit_End; don't print position if bit 7 is set
	;; multiply with LCD_MTC_D
	mulwf	LCD_MTC_D, ACCESS
	movf	PRODL, W
	addwf	LCD_MTC_X, W	; add x offset
	movff	LCD_MTC_Y, MIOS_PARAMETER1
	call	MIOS_GLCD_GCursorSet

	;; print character and leave routine
	movf	MIOS_PARAMETER3, W
	andlw	0x3f
	xorlw	0x20		; (special character LC coding)
	addlw	0x20
	call	MIOS_LCD_PrintChar
LC_GLCD_Print_MTC_Digit_End
	return

;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  This function is called by LC_GLCD_Print_SMPTE and LC_GLCD_Print_BEATS
;;  when the two status digits should be displayed
;; --------------------------------------------------------------------------
LC_GLCD_Print_STATUS_Digits
	;; print the two status digits
	movlw	0x00
	movwf	MIOS_PARAMETER2
	SET_BSR	LC_STATUS_DIGIT_A
	movf	LC_STATUS_DIGIT_A, W, BANKED
	movwf	MIOS_PARAMETER3
	rcall	LC_GLCD_Print_STATUS_Digit

	movlw	0x01
	movwf	MIOS_PARAMETER2
	SET_BSR	LC_STATUS_DIGIT_B
	movf	LC_STATUS_DIGIT_B, W, BANKED
	movwf	MIOS_PARAMETER3
	rgoto	LC_GLCD_Print_STATUS_Digit

;; --------------------------------------------------------------------------
;;  This function is called by lc_mproc::LC_MPROC_Received_B04x,
;;  lc_mproc::LC_MPROC_Action_WriteMTC* and some functions of
;;  this file when a status digit should be print
;; IN: digit position in MIOS_PARAMETER2
;;     digit value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
LC_GLCD_Print_STATUS_Digit
	;; don't print if LCD_STATUS_FONT_H is 0
	movf	LCD_STATUS_FONT_H, F
	skpnz
	return

	;; initialize the font
	movff	LCD_STATUS_FONT_L, TBLPTRL
	movff	LCD_STATUS_FONT_H, TBLPTRH
	call	MIOS_GLCD_FontInit

	;; set graphical cursor depending on digit position
	movf	MIOS_PARAMETER2, W
	xorlw	0x01
	andlw	0x01
	;; multiply with LCD_STATUS_D
	mulwf	LCD_STATUS_D, ACCESS
	movf	PRODL, W
	addwf	LCD_STATUS_X, W	; add x offset
	movff	LCD_STATUS_Y, MIOS_PARAMETER1
	call	MIOS_GLCD_GCursorSet

	;; print character and leave routine
	movf	MIOS_PARAMETER3, W
	andlw	0x3f
	xorlw	0x20		; (special character LC coding)
	addlw	0x20
	goto	MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  This function is used to print all meters on screen
;; --------------------------------------------------------------------------
LC_GLCD_Print_Meters
	;; just request an update
	;; the handler in lc_meters.inc will call LC_DISPLAY_Print_Meter
	;; for every seperate meter
	movlw	0xff
	movwf	LC_METER_UPDATE_REQ
	return
	

;; --------------------------------------------------------------------------
;;  This function is used to print a meter pattern
;;  IN: meter number in FSR0L & 0x7
;;      pattern index in INDF0
;;  NOTE: don't overwrite TMP1 and TMP2 here!
;; --------------------------------------------------------------------------
LC_GLCD_Print_Meter
	;; don't print if LCD_METER_ICONS_H is 0
	movf	LCD_METER_ICONS_H, F
	skpnz
	return

	;; init icon table
	movff	LCD_METER_ICONS_L, TBLPTRL
	movff	LCD_METER_ICONS_H, TBLPTRH
	call	MIOS_GLCD_FontInit

	;; set graphical cursor
	movff	LCD_METER_Y, MIOS_PARAMETER1
	movf	FSR0L, W	; FSR0L & 0x07 == meter number
	andlw	0x07
	mulwf	LCD_METER_D, ACCESS
	movf	PRODL, W
	addwf	LCD_METER_X, W
	call	MIOS_GLCD_GCursorSet
	
	;; print char depending on level
	movf	INDF0, W	; INDF0 == meter index
	andlw	0x0f
	;; if overflow bit set, add 14 (overflow icons begin at this offset)
	btfsc	INDF0, 7
	addlw 14
	call	MIOS_LCD_PrintChar

	return

;; --------------------------------------------------------------------------
;;  This function is used to print all knobs on screen
;; --------------------------------------------------------------------------
LC_GLCD_Print_Knobs
	;; just request an update
	;; the handler in lc_ledrings.inc will call LC_DISPLAY_Print_Knob
	;; for every seperate meter
	movlw	0xff
	movwf	LC_LEDRING_UPDATE_REQ
	return
	

;; --------------------------------------------------------------------------
;;  This function is used to print a knob pattern
;;  IN: knob number in FSR0L & 0x7
;;      pattern index in INDF0
;; --------------------------------------------------------------------------
LC_GLCD_Print_Knob
	;; don't print if LCD_KNOB_ICONS_H is 0
	movf	LCD_KNOB_ICONS_H, F
	skpnz
	return

	;; init icon table
	movff	LCD_KNOB_ICONS_L, TBLPTRL
	movff	LCD_KNOB_ICONS_H, TBLPTRH
	call	MIOS_GLCD_FontInit

	;; set graphical cursor
	movff	LCD_KNOB_Y, MIOS_PARAMETER1
	movf	FSR0L, W	; FSR0L & 0x07 == knob number
	andlw	0x07
	mulwf	LCD_KNOB_D, ACCESS
	movf	PRODL, W
	addwf	LCD_KNOB_X, W
	call	MIOS_GLCD_GCursorSet
	
	;; print char depending on index
	movf	INDF0, W
	BRA_IFCLR LC_STAT, LC_STAT_GPC_SEL, ACCESS, LC_GLCD_Print_Knob_NoGPC
	;; in GPC mode shift value 3 times to the right
	rrf	WREG, F
	rrf	WREG, F
	rrf	WREG, W
LC_GLCD_Print_Knob_NoGPC
	andlw	0x0f
	call	MIOS_LCD_PrintChar

	return

;; --------------------------------------------------------------------------
;;  This function is used to reprint the host message lines (2x55 chars) on screen
;; --------------------------------------------------------------------------
LC_GLCD_Print_Msg
	;; don't print if LCD_MSG_FONT_H is 0
	movf	LCD_MSG_FONT_H, F
	skpnz
	return

	;; save current LC_CURSOR in TMP5
	movff	LC_CURSOR, TMP5

	;; set LC_CURSOR to position 0x00
	clrf	LC_CURSOR

	;; update cursor position
	rcall	LC_GLCD_Msg_CursorUpdate

	;; init first line from column 0 to 55
	lfsr	FSR0, MSG_HOST_BEGIN
	btfsc	LC_STAT, LC_STAT_GPC_SEL
	lfsr	FSR0, MSG_GPC_BEGIN
	btfsc	LC_STAT, LC_STAT_CALI_SEL
	lfsr	FSR0, MSG_GPC_BEGIN

	clrf	TMP4		; TMP4 used as loop counter
LC_GLCD_Print_Msg_Loop1
	movf	POSTINC0, W
	rcall	LC_GLCD_Msg_PrintChar
	incf	TMP4, F
	movlw	55
	cpfsgt	TMP4, ACCESS
	rgoto LC_GLCD_Print_Msg_Loop1
	
	;; set LC_CURSOR to position 0x40
	movlw	0x40
	movwf	LC_CURSOR

	;; update cursor position
	rcall	LC_GLCD_Msg_CursorUpdate

	;; init second line from column 0 to 55
	lfsr	FSR0, MSG_HOST_BEGIN+0x40
	btfsc	LC_STAT, LC_STAT_GPC_SEL
	lfsr	FSR0, MSG_GPC_BEGIN+0x40
	btfsc	LC_STAT, LC_STAT_CALI_SEL
	lfsr	FSR0, MSG_GPC_BEGIN+0x40

	clrf	TMP4		; TMP4 used as loop counter
LC_GLCD_Print_Msg_Loop2
	movf	POSTINC0, W
	rcall	LC_GLCD_Msg_PrintChar
	incf	TMP4, F
	movlw	55
	cpfsgt	TMP4, ACCESS
	rgoto LC_GLCD_Print_Msg_Loop2
	
	;; resture LC_CURSOR from TMP5
	movff	TMP5, LC_CURSOR

	return
	
;; --------------------------------------------------------------------------
;;  This function is called by lc_mproc::LC_MPROC_Action_WriteLCD
;;  when a character should be print to the host message section
;;  IN: character in WREG
;;      cursor position in LC_CURSOR
;; --------------------------------------------------------------------------
LC_GLCD_Msg_PrintChar
	;; don't print if LCD_MSG_FONT_H is 0
	movf	LCD_MSG_FONT_H, F
	skpnz
	return

	;; save character in LCD_MSG_CHR
	movwf	LCD_MSG_CHR

	;; don't print host messages if LCD_MSG_Y is 0xff!
	incf	LCD_MSG_Y, W
	skpnz
	return

	;; print character
	movf	LCD_MSG_CHR, W
	call	MIOS_LCD_PrintChar

	;; increment cursor
	incf	LC_CURSOR, F

	;; fix the cursor if second line has been reached
	BRA_IFSET LC_CURSOR, 6, ACCESS, LC_GLCD_Msg_PrintChar_Skip
	movlw	0x38
	cpfseq	LC_CURSOR, 0
	rgoto LC_GLCD_Msg_PrintChar_Skip
	;; set new cursor position (second line)
	movlw	0x40
	movwf	LC_CURSOR
LC_GLCD_Msg_PrintChar_Skip

	;; update the cursor position
	rgoto	LC_GLCD_Msg_CursorUpdate
	
;; --------------------------------------------------------------------------
;;  This function sets the cursor on the host message screen
;;  IN: cursor position in LC_CURSOR
;; --------------------------------------------------------------------------
LC_GLCD_Msg_CursorUpdate
	;; initialize the font
	movff	LCD_MSG_FONT_L, TBLPTRL
	movff	LCD_MSG_FONT_H, TBLPTRH
	call	MIOS_GLCD_FontInit

	;; calc graphical cursor position depending on received byte
	movf	LCD_MSG_Y, W
	btfsc	LC_CURSOR, 6
	addlw 1
	movwf	MIOS_PARAMETER1 ; Y-position -> MIOS_PARAMETER1

	movf	LC_CURSOR, W
	andlw	0x3f
	addlw	2
	mulwf	LCD_MSG_D, ACCESS
	movf	PRODL, W
	addwf	LCD_MSG_X, W	; X-position -> WREG

	;; now set the graphical cursor
	call	MIOS_GLCD_GCursorSet

	return
