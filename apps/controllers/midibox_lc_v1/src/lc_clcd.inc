; $Id$
;
; MIDIbox LC Display Driver for Character LCDs
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; include the initialization macros used by LC_CLCD_Init_Page*
#include "lc_clcd_macros.inc"

;; --------------------------------------------------------------------------
;;  Initialize the screen depending on selected display page
;; --------------------------------------------------------------------------
LC_CLCD_Init
	;; fetch the display mode settings
	rcall	LC_CLCD_Init_FetchSettings

	;; get address to char table depending on meter mode
	;; LCD_METER_ICONS_H[0] == 1: vertical meters
	;; LCD_METER_ICONS_H[1] == 1: horizontal meters
	;; everything else: don't care
	BRA_IFSET LCD_METER_ICONS_H, 1, ACCESS, LC_CLCD_Init_HMeter
LC_CLCD_Init_VMeter
	TABLE_ADDR LC_CLCD_SPECIAL_CHARS_V
	rgoto	LC_CLCD_Init_Cont
LC_CLCD_Init_HMeter
	TABLE_ADDR LC_CLCD_SPECIAL_CHARS_H
	;; 	rgoto	LC_CLCD_Init_Cont
LC_CLCD_Init_Cont

	;; save table address in TMP[12]
	movff	TBLPTRL, TMP1
	movff	TBLPTRH, TMP2

	;; init special chars, first LCD
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	MIOS_CLCD_SpecialCharsInit

#if LCD_USE_SECOND == 1
	movff	TMP1, TBLPTRL
	movff	TMP2, TBLPTRH

	;; init special chars, second LCD
	movlw	0x80
	call	MIOS_LCD_CursorSet
	call	MIOS_CLCD_SpecialCharsInit
#endif

	;; call all display element handlers to built the screen layout
	rcall	LC_CLCD_Print_STATUS_Digits
	rcall	LC_CLCD_Print_SMPTE_BEATS
	rcall	LC_CLCD_Print_MTC_Digits
	rcall	LC_CLCD_Print_Msg
	rcall	LC_CLCD_Print_Meters
	rcall	LC_CLCD_Print_RSM
	rcall	LC_CLCD_Print_SEL
	return

LC_CLCD_Init_FetchSettings
	movf	LC_DISPLAY_PAGE, W
	JUMPTABLE_2BYTES 4	; 4 entries
	rgoto	LC_CLCD_Init_Page0
	rgoto	LC_CLCD_Init_Page1
	rgoto	LC_CLCD_Init_Page2
	rgoto	LC_CLCD_Init_Page3

;; --------------------------------------------------------------------------
;;  Some words to the x/y positions:
;;     - x goes from 0 thru 39 (1st column to 40th column)
;;     - y: when 0: first LCD, upper line
;;     - y: when 1: first LCD, lower line
;;     - y: when 2: second LCD, upper line
;;     - y: when 3: second LCD, lower line
;; --------------------------------------------------------------------------
;;  The first parameter of every macro enables/disables the display element
;;  Example:
;;	SET_CLCD_STATUS 1, 38, 2
;;  prints the status message (2 digits which are transmitted by the host)
;;  at x = 38, y = 2
;;
;;      SET_CLCD_STATUS 0, 0, 0
;;  disables the status message
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  Page0: 
;; --------------------------------------------------------------------------
LC_CLCD_Init_Page0
	;; print status digits at position 38/2
	SET_CLCD_STATUS 1, 38, 2
	
	;; print "SMPTE/BEATS" at position: 28/2
	SET_CLCD_SMPTE_BEATS 1, 28, 2

	;; print MTC digits at position 27/3
	SET_CLCD_MTC 1, 27, 3

	;; print host messages at position 12/0, (0=normal spacing)
	SET_CLCD_HOST_MSG 1, 12, 0, 0

	;; don't print meters
	SET_CLCD_METER 0, 0, 0, 0

	;; print rec/solo/mute status at 0/0
	SET_CLCD_RSM_MSG 1, 0, 0

	;; print select status at 0/1
	SET_CLCD_SEL_MSG 1, 0, 1

	return


;; --------------------------------------------------------------------------
;;  Page1: 
;; --------------------------------------------------------------------------
LC_CLCD_Init_Page1
	;; print status digits at position 38/2
	SET_CLCD_STATUS 1, 38, 2
	
	;; print "SMPTE/BEATS" at position: 28/2
	SET_CLCD_SMPTE_BEATS 1, 28, 2

	;; print MTC digits at position 27/3
	SET_CLCD_MTC 1, 27, 3

	;; print host messages at position 12/0, only first line, (0=normal spacing)
	SET_CLCD_HOST_MSG 2, 12, 0, 0

	;; print horizontal meters at position 12/1 (distance between meter chars: 7)
	SET_CLCD_METER 2, 12, 1, 7

	;; print rec/solo/mute status at 0/0
	SET_CLCD_RSM_MSG 1, 0, 0

	;; print select status at 0/1
	SET_CLCD_SEL_MSG 1, 0, 1

	return


;; --------------------------------------------------------------------------
;;  Page2: 
;; --------------------------------------------------------------------------
LC_CLCD_Init_Page2
	;; don't print status digits
	SET_CLCD_STATUS 0, 0, 0
	
	;; don't print "SMPTE/BEATS"
	SET_CLCD_SMPTE_BEATS 0, 0, 0

	;; don't print MTC digits
	SET_CLCD_MTC 0, 0, 0

	;; print host messages at position 12/0, (1=large spacing, x and y ignored)
	SET_CLCD_HOST_MSG 1, 12, 0, 1

	;; don't print meters
	SET_CLCD_METER 0, 0, 0, 0

	;; don't print rec/solo/mute status
	SET_CLCD_RSM_MSG 0, 0, 0

	;; don't print select status
	SET_CLCD_SEL_MSG 0, 0, 0

	return


;; --------------------------------------------------------------------------
;;  Page3: 
;; --------------------------------------------------------------------------
LC_CLCD_Init_Page3
	;; don't print status digits
	SET_CLCD_STATUS 0, 0, 0
	
	;; don't print "SMPTE/BEATS"
	SET_CLCD_SMPTE_BEATS 0, 0, 0

	;; don't print MTC digits
	SET_CLCD_MTC 0, 0, 0

	;; print host messages at position 12/0, (1=large spacing, x and y ignored)
	SET_CLCD_HOST_MSG 2, 12, 0, 1

	;; print horizontal meters at position 7/1 (distance between meter chars: 2)
	;; |4 means: x-2 on second display
	SET_CLCD_METER 2 | 4, 7, 1, 9

	;; don't print rec/solo/mute status
	SET_CLCD_RSM_MSG 0, 0, 0

	;; don't print select status
	SET_CLCD_SEL_MSG 0, 0, 0

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
LC_CLCD_LEDStatusUpdate
	;; save ID in TMP1
	movwf	TMP1

	;; prepare FSR0
	lfsr	FSR0, LED_STATUS_BEGIN
	addwf	FSR0L, F

	;; skip SMPTE and BEATS check if "90xx00" has been received
	movf	INDF0, W
	bz	LC_CLCD_LEDStatusUpdate_Skip
	
	;; print SMPTE if 90 71 7F has been received
	movf	TMP1, W
	xorlw	0x71
	bz	LC_CLCD_Print_SMPTE

	;; print BEATS if 90 72 7F has been received
	movf	TMP1, W
	xorlw	0x72
	bz	LC_CLCD_Print_BEATS

LC_CLCD_LEDStatusUpdate_Skip
	;; handle with Mute/Solo/Rec/Select

	;; prepare FSR0 again (now we need the base pointer!)
	lfsr	FSR0, LED_STATUS_BEGIN

	;; if 90 00 - 90 07 has been received: update Rec
	movlw	0x07
	cpfsgt	TMP1, ACCESS
	rgoto LC_CLCD_LEDStatusUpdate_Rec

	;; if 90 08 - 90 0F has been received: update Solo
	movlw	0x0f
	cpfsgt	TMP1, ACCESS
	rgoto LC_CLCD_LEDStatusUpdate_Solo

	;; if 90 10 - 90 17 has been received: update Mute
	movlw	0x17
	cpfsgt	TMP1, ACCESS
	rgoto LC_CLCD_LEDStatusUpdate_Mute

	;; if 90 18 - 90 1F has been received: update Select
	movlw	0x1f
	cpfsgt	TMP1, ACCESS
	rgoto LC_CLCD_LEDStatusUpdate_Select

	;; else do nothing
	rgoto	LC_CLCD_LEDStatusUpdate_End

LC_CLCD_LEDStatusUpdate_Rec
	;; print the solo status if value == 0, else a "R"
	movf	TMP1, W
	andlw	0x07
	addlw	0x00		; rec == 90 0[0-7]
	movf	PLUSW0, W
	bz	LC_CLCD_LEDStatusUpdate_Solo

LC_CLCD_LEDStatusUpdate_Rec1
	;; print R
	movlw	'R'
	rgoto	LC_CLCD_LEDStatusUpdate_RSM_Save
	
LC_CLCD_LEDStatusUpdate_Solo
	;; print the mute status if value == 0, else a "S"
	movf	TMP1, W
	andlw	0x07
	addlw	0x08		; solo == 90 0[8-F]
	movf	PLUSW0, W
	bz	LC_CLCD_LEDStatusUpdate_Mute

LC_CLCD_LEDStatusUpdate_Solo1
	;; print S
	movlw	'S'
	rgoto	LC_CLCD_LEDStatusUpdate_RSM_Save
	
LC_CLCD_LEDStatusUpdate_Mute
	;; print space or "S" if value == 0
	movf	TMP1, W
	andlw	0x07
	addlw	0x10		; mute == 90 1[0-7]
	movf	PLUSW0, W
	bz	LC_CLCD_LEDStatusUpdate_Mute0
LC_CLCD_LEDStatusUpdate_Mute1
	;; else print M
	movlw	'M'
	rgoto	LC_CLCD_LEDStatusUpdate_RSM_Save
LC_CLCD_LEDStatusUpdate_Mute0
	;; print 'R' if rec active, 'S' if solo active, else space
	clrf	TMP2		; TMP2.0 notifies that rec active, TMP2.1 that solo active

	movf	TMP1, W
	andlw	0x07
	addlw	0x00		; rec == 90 0[0-7]
	movf	PLUSW0, W
	skpz
	bsf	TMP2, 0

	movf	TMP1, W
	andlw	0x07
	addlw	0x08		; solo == 90 0[8-F]
	movf	PLUSW0, W
	skpz
	bsf	TMP2, 1

	;; print "R", "S" or ' ' (in this priority)
	movlw	' '
	btfsc	TMP2, 1
	movlw 'S'
	btfsc	TMP2, 0
	movlw 'R'
	;; 	rgoto	LC_CLCD_LEDStatusUpdate_RSM_Save
LC_CLCD_LEDStatusUpdate_RSM_Save
	;; save character in RSM_MSG array
	movwf	TMP2		; temp. save it in TMP2
	;; prepare pointer to RSM_MSG array
	lfsr	FSR1, RSM_MSG_BEGIN
	movf	TMP1, W
	andlw	0x07
	addwf	FSR1L, F
	movff	TMP2, INDF1	; transfer character to array

	;; print message
	call	LC_CLCD_Print_RSM
	;; and exit
	rgoto	LC_CLCD_LEDStatusUpdate_End

	;; ---
	
LC_CLCD_LEDStatusUpdate_Select
	;; print space if value == 0
	movf	TMP1, W
	andlw	0x07
	addlw	0x18		; select == 90 1[8-F]
	movf	PLUSW0, W
	bz	LC_CLCD_LEDStatusUpdate_Select0
LC_CLCD_LEDStatusUpdate_Select1
	;; else print number 1-8
	movf	TMP1, W
	andlw	0x07
	addlw	'1'
	rgoto	LC_CLCD_LEDStatusUpdate_SEL_Save
LC_CLCD_LEDStatusUpdate_Select0
	movlw	' '
	;; 	rgoto	LC_CLCD_LEDStatusUpdate_SEL_Save
LC_CLCD_LEDStatusUpdate_SEL_Save
	;; save character in SEL_MSG array
	movwf	TMP2		; temp. save it in TMP2
	;; prepare pointer to SEL_MSG array
	lfsr	FSR1, SEL_MSG_BEGIN
	movf	TMP1, W
	andlw	0x07
	addwf	FSR1L, F
	movff	TMP2, INDF1	; transfer character to array

	;; print message
	call	LC_CLCD_Print_SEL
	;; and exit
	rgoto	LC_CLCD_LEDStatusUpdate_End

	;; ---
LC_CLCD_LEDStatusUpdate_End
	return

;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  print SMPTE or BEATS depending on LC_STAT_SMPTE flag which is
;;  directly controlled by the host application (Logic, Cubase, etc...)
;; --------------------------------------------------------------------------
LC_CLCD_Print_SMPTE_BEATS
	BRA_IFSET LC_STAT, LC_STAT_SMPTE, ACCESS, LC_CLCD_Print_SMPTE
	rgoto	LC_CLCD_Print_BEATS

;; --------------------------------------------------------------------------
;;  This function is called by LC_CLCD_LEDStatusUpdate when
;;  "90 71 7F" has been received
;; --------------------------------------------------------------------------
LC_CLCD_Print_SMPTE
	;; force display mode to SMPTE
	bsf	LC_STAT, LC_STAT_SMPTE

	;; don't print if LCD_SMPTE_BEATS_FONT_H is 0
	movf	LCD_SMPTE_BEATS_FONT_H, F
	skpnz
	return

	;; set cursor
	movf	LCD_SMPTE_BEATS_X, W
	iorwf	LCD_SMPTE_BEATS_Y, W
	call	MIOS_LCD_CursorSet

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

	rgoto	LC_CLCD_Print_MTC_Digits

;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  This function is called by LC_CLCD_LEDStatusUpdate when
;;  "90 72 7F" has been received
;; --------------------------------------------------------------------------
LC_CLCD_Print_BEATS
	;; force display mode to BEATS
	bcf	LC_STAT, LC_STAT_SMPTE

	;; don't print if LCD_SMPTE_BEATS_FONT_H is 0
	movf	LCD_SMPTE_BEATS_FONT_H, F
	skpnz
	return

	;; set cursor
	movf	LCD_SMPTE_BEATS_X, W
	iorwf	LCD_SMPTE_BEATS_Y, W
	call	MIOS_LCD_CursorSet

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

	rgoto	LC_CLCD_Print_MTC_Digits

;; --------------------------------------------------------------------------


;; --------------------------------------------------------------------------
;;  This function is called by LC_CLCD_Print_SMPTE and LC_CLCD_Print_BEATS
;;  when all MTC digits should be updated
;; --------------------------------------------------------------------------
LC_CLCD_Print_MTC_Digits
	;; print the MTC digits and colons
	clrf	TMP5
LC_CLCD_Print_MTC_Digits_Loop
	movff	TMP5, MIOS_PARAMETER2
	lfsr	FSR0, LC_MTC_DIGIT_0
	movf	TMP5, W
	movff	PLUSW0, MIOS_PARAMETER3
	rcall	LC_CLCD_Print_MTC_Digit
	incf	TMP5, F
	movlw	14
	cpfseq	TMP5, ACCESS
	rgoto LC_CLCD_Print_MTC_Digits_Loop

	return

;; --------------------------------------------------------------------------
;;  This function is called by lc_mproc::LC_MPROC_Received_B04x,
;;  lc_mproc::LC_MPROC_Action_WriteMTC* and some functions of
;;  this file when a MTC digit should be print
;;  IN: digit position in MIOS_PARAMETER2
;;      digit value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
LC_CLCD_Print_MTC_Digit
	;; don't print if LCD_MTC_FONT_H is 0
	movf	LCD_MTC_FONT_H, F
	skpnz
	return

	;; set cursor depending on digit position
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	call	LC_HLP_MTCPositionGet
	BRA_IFSET WREG, 7, ACCESS, LC_CLCD_Print_MTC_Digit_End; don't print position if bit 7 is set
	addwf	LCD_MTC_X, W	; add x offset
	iorwf	LCD_MTC_Y, W	; add y offset
	call	MIOS_LCD_CursorSet

	;; print character and leave routine
	movf	MIOS_PARAMETER3, W
	andlw	0x3f
	xorlw	0x20		; (special character LC coding)
	addlw	0x20
	call	MIOS_LCD_PrintChar
LC_CLCD_Print_MTC_Digit_End
	return

;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  This function is called by LC_CLCD_Print_SMPTE and LC_CLCD_Print_BEATS
;;  when the two status digits should be displayed
;; --------------------------------------------------------------------------
LC_CLCD_Print_STATUS_Digits
	;; print the two status digits
	movlw	0x00
	movwf	MIOS_PARAMETER2
	SET_BSR	LC_STATUS_DIGIT_A
	movf	LC_STATUS_DIGIT_A, W, BANKED
	movwf	MIOS_PARAMETER3
	rcall	LC_CLCD_Print_STATUS_Digit

	movlw	0x01
	movwf	MIOS_PARAMETER2
	SET_BSR	LC_STATUS_DIGIT_B
	movf	LC_STATUS_DIGIT_B, W, BANKED
	movwf	MIOS_PARAMETER3
	rgoto	LC_CLCD_Print_STATUS_Digit

;; --------------------------------------------------------------------------
;;  This function is called by lc_mproc::LC_MPROC_Received_B04x,
;;  lc_mproc::LC_MPROC_Action_WriteMTC* and some functions of
;;  this file when a status digit should be print
;; IN: digit position in MIOS_PARAMETER2
;;     digit value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
LC_CLCD_Print_STATUS_Digit
	;; don't print if LCD_STATUS_FONT_H is 0
	movf	LCD_STATUS_FONT_H, F
	skpnz
	return

	;; set cursor depending on digit position
	movf	MIOS_PARAMETER2, W
	xorlw	0x01
	andlw	0x01
	addwf	LCD_STATUS_X, W	; add x offset
	iorwf	LCD_STATUS_Y, W	; add y offset
	call	MIOS_LCD_CursorSet

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
LC_CLCD_Print_Meters
	;; just request an update
	;; the handler in lc_meters.inc will call LC_DISPLAY_Print_Meter
	;; for every seperate meter
	movlw	0xff
	movwf	LC_METER_UPDATE_REQ
	return

;; --------------------------------------------------------------------------
;;  This function is used to set a meter pattern
;;  IN: meter number in FSR0L & 0x7
;;      pattern index in INDF0
;;  NOTE: don't overwrite TMP1 and TMP2 here!
;; --------------------------------------------------------------------------
LC_CLCD_Print_Meter
	;; don't print if LCD_METER_ICONS_H is 0
	movf	LCD_METER_ICONS_H, F
	skpnz
	return

	;; determine cursor pos
	movf	FSR0L, W
	andlw	0x07	; FSR0L & 0x07 == meter number
	mulwf	LCD_METER_D, ACCESS
	movf	PRODL, W
	addwf	LCD_METER_X, W
	movwf	TMP5
	movlw	40-1
	cpfsgt	TMP5, ACCESS
	rgoto LC_CLCD_Print_Meter_C1
LC_CLCD_Print_Meter_C2
	movlw	0x80-40
	btfsc	LCD_METER_ICONS_H, 2; (special option for "large spacing" mode)
	addlw -2
	addwf	TMP5, F
LC_CLCD_Print_Meter_C1
	movf	TMP5, W

	;; branch depending on meter mode (horizontal/vertical)
	BRA_IFSET LCD_METER_ICONS_H, 1, ACCESS, LC_CLCD_Print_Meter_H

LC_CLCD_Print_Meter_V
	;; set cursor to first line
	call	MIOS_LCD_CursorSet

	;; print upper char depending on level
	movf	INDF0, W	; INDF0 == meter index
	andlw	0x07
	btfss	INDF0, 3
	movlw ' '
	btfsc	INDF0, 7; (overload flag)
	movlw 'o'
	call	MIOS_LCD_PrintChar

	;; set cursor to second line
	movf	TMP5, W
	addlw	0x40
	call	MIOS_LCD_CursorSet

	;; print lower char depending on level
	movf	INDF0, W	; INDF0 == meter index
	btfsc	INDF0, 3
	movlw 0x07
	andlw	0x07
	goto	MIOS_LCD_PrintChar

LC_CLCD_Print_Meter_H
	;; add Y offset and set cursor
	addwf	LCD_METER_Y, W
	call	MIOS_LCD_CursorSet

	;; calc pointer to table entry
	TABLE_ADDR LC_CLCD_Print_Meter_H_Table
	movf	INDF0, W
	andlw	0x0f
	mullw	6		; 6 chars per entry
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F

	;; print preconfigured string
	movlw	5		; (5 chars)
	call	MIOS_LCD_PrintPreconfString

	;; print overrun flag if INDF0[7] set, else space 
	movlw	' '
	btfsc	INDF0, 7
	movlw 4
	goto	MIOS_LCD_PrintChar; ...and exit

LC_CLCD_Print_Meter_H_Table
	;; special chars which should be print depending on meter value (16 entries, only 14 used)
	;; only 5 are used, the 6th has to be inserted due to a MPASM imperfection
	db	1, 0, 0, 0, 0, 0
	db	2, 0, 0, 0, 0, 0
	db	3, 0, 0, 0, 0, 0
	db	3, 1, 0, 0, 0, 0
	db	3, 2, 0, 0, 0, 0
	db	3, 3, 0, 0, 0, 0
	db	3, 3, 1, 0, 0, 0
	db	3, 3, 2, 0, 0, 0
	db	3, 3, 3, 0, 0, 0
	db	3, 3, 3, 1, 0, 0
	db	3, 3, 3, 2, 0, 0
	db	3, 3, 3, 3, 1, 0
	db	3, 3, 3, 3, 2, 0
	db	3, 3, 3, 3, 3, 0
	db	3, 3, 3, 3, 3, 0	; (not used)
	db	3, 3, 3, 3, 3, 0	; (not used)

;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  This function is used to print a knob pattern
;;  IN: knob number in FSR0L & 0x7
;;      pattern index in INDF0
;; --------------------------------------------------------------------------
LC_CLCD_Print_Knob
	;; knobs icons not supported by CLCD
	return


;; --------------------------------------------------------------------------
;;  This function is used to reprint the host message lines (2x55 chars) on screen
;; --------------------------------------------------------------------------
LC_CLCD_Print_Msg
	;; don't print if LCD_MSG_FONT_H is 0
	movf	LCD_MSG_FONT_H, F
	skpnz
	return

	;; save current LC_CURSOR in TMP5
	movff	LC_CURSOR, TMP5

	;; set LC_CURSOR to position 0x00
	clrf	LC_CURSOR

	;; update cursor position
	rcall	LC_CLCD_Msg_CursorUpdate

	;; init first line from column 0 to 55
	lfsr	FSR0, MSG_HOST_BEGIN
	btfsc	LC_STAT, LC_STAT_GPC_SEL
	lfsr	FSR0, MSG_GPC_BEGIN
	btfsc	LC_STAT, LC_STAT_CALI_SEL
	lfsr	FSR0, MSG_GPC_BEGIN
	
	clrf	TMP4		; TMP4 used as loop counter
LC_CLCD_Print_Msg_Loop1
	movf	POSTINC0, W
	rcall	LC_CLCD_Msg_PrintChar
	incf	TMP4, F
	movlw	55
	cpfsgt	TMP4, ACCESS
	rgoto LC_CLCD_Print_Msg_Loop1
	
	;; set LC_CURSOR to position 0x40
	movlw	0x40
	movwf	LC_CURSOR

	;; update cursor position
	rcall	LC_CLCD_Msg_CursorUpdate

	;; init second line from column 0 to 55
	lfsr	FSR0, MSG_HOST_BEGIN+0x40
	btfsc	LC_STAT, LC_STAT_GPC_SEL
	lfsr	FSR0, MSG_GPC_BEGIN+0x40
	btfsc	LC_STAT, LC_STAT_CALI_SEL
	lfsr	FSR0, MSG_GPC_BEGIN+0x40

	clrf	TMP4		; TMP4 used as loop counter
LC_CLCD_Print_Msg_Loop2
	movf	POSTINC0, W
	rcall	LC_CLCD_Msg_PrintChar
	incf	TMP4, F
	movlw	55
	cpfsgt	TMP4, ACCESS
	rgoto LC_CLCD_Print_Msg_Loop2
	
	;; restore LC_CURSOR from TMP5
	movff	TMP5, LC_CURSOR

	return

;; --------------------------------------------------------------------------
;;  This function is called by lc_mproc::LC_MPROC_Action_WriteLCD
;;  when a character should be print to the host message section
;;  IN: character in WREG
;;      cursor position in LC_CURSOR
;; --------------------------------------------------------------------------
LC_CLCD_Msg_PrintChar
	;; don't print if LCD_MSG_FONT_H is 0
	movf	LCD_MSG_FONT_H, F
	skpnz
	return

	;; save character in LCD_MSG_CHR
	movwf	LCD_MSG_CHR

	;; print character if X-position less than LC_EMU_COL
	movf	LC_CURSOR, W
	andlw	0x3f
	sublw	LCD_EMU_COL-1
	bnc	LC_CLCD_Msg_PrintChar_Skip1
	movf	LCD_MSG_CHR, W
	;; if LCD_MSG_FONT_H[1] set and second line: skip
	BRA_IFCLR LCD_MSG_FONT_H, 1, ACCESS, LC_CLCD_Msg_PrintChar_Now
	BRA_IFSET LC_CURSOR, 6, ACCESS, LC_CLCD_Msg_PrintChar_Skip1
LC_CLCD_Msg_PrintChar_Now
	call	MIOS_LCD_PrintChar
LC_CLCD_Msg_PrintChar_Skip1

	;; increment cursor
	incf	LC_CURSOR, F

	;; fix the cursor if second line has been reached
	BRA_IFSET LC_CURSOR, 6, ACCESS, LC_CLCD_Msg_PrintChar_Skip2
	movlw	0x38
	cpfseq	LC_CURSOR, 0
	rgoto LC_CLCD_Msg_PrintChar_Skip2
	;; set new cursor position (second line)
	movlw	0x40
	movwf	LC_CURSOR
LC_CLCD_Msg_PrintChar_Skip2

	;; update the cursor position
	rgoto	LC_CLCD_Msg_CursorUpdate
	
;; --------------------------------------------------------------------------
;;  This function sets the cursor on the host message screen
;;  IN: cursor position in LC_CURSOR
;; --------------------------------------------------------------------------
LC_CLCD_Msg_CursorUpdate
	;; if LCD_MSG_D[0] set, use extra large spacing
	BRA_IFSET LCD_MSG_D, 0, ACCESS, LC_CLCD_Msg_CursorUpdate_XL
LC_CLCD_Msg_CursorUpdate_N
	movf	LC_CURSOR, W
	andlw	0x3f
	addwf	LCD_MSG_X, W
	movwf	TMP1
	movlw	40-1
	cpfsgt	TMP1, ACCESS
	rgoto LC_CLCD_Msg_CursorUpdate_C1
LC_CLCD_Msg_CursorUpdate_C2
	movlw	0x80-40
	addwf	TMP1, F
LC_CLCD_Msg_CursorUpdate_C1
	movf	TMP1, W
	rgoto	LC_CLCD_MSG_CursorUpdate_Cont


LC_CLCD_Msg_CursorUpdate_XL
	movf	LC_CURSOR, W		; cursor positions can be found in lc_hlp.inc
	andlw	0x3f
	call	LC_HLP_MsgCursorPos

LC_CLCD_MSG_CursorUpdate_Cont
	btfsc	LC_CURSOR, 6
	addlw 0x40
	call	MIOS_LCD_CursorSet
	return
	
;; --------------------------------------------------------------------------
;;  This function prints the saved rec/solo/mute status
;; --------------------------------------------------------------------------
LC_CLCD_Print_RSM
	;; don't print if LCD_RSM_FONT_H is 0
	movf	LCD_SEL_FONT_H, F
	skpnz
	return

	;; set cursor
	movf	LCD_RSM_X, W
	iorwf	LCD_RSM_Y, W
	call	MIOS_LCD_CursorSet

	;; init pointer to message
	lfsr	FSR0, RSM_MSG_BEGIN
	;; print 8 chars
	movlw	8
	movwf	TMP1
LC_CLCD_Print_RSM_Loop
	movf	POSTINC0, W
	call	MIOS_LCD_PrintChar
	decfsz	TMP1, F
	rgoto	LC_CLCD_Print_RSM_Loop
	return

;; --------------------------------------------------------------------------
;;  This function prints the saved "select button" status
;; --------------------------------------------------------------------------
LC_CLCD_Print_SEL
	;; don't print if LCD_SEL_FONT_H is 0
	movf	LCD_SEL_FONT_H, F
	skpnz
	return

	;; set cursor
	movf	LCD_SEL_X, W
	iorwf	LCD_SEL_Y, W
	call	MIOS_LCD_CursorSet

	;; init pointer to message
	lfsr	FSR0, SEL_MSG_BEGIN
	;; print 8 chars
	movlw	8
	movwf	TMP1
LC_CLCD_Print_SEL_Loop
	movf	POSTINC0, W
	call	MIOS_LCD_PrintChar
	decfsz	TMP1, F
	rgoto	LC_CLCD_Print_SEL_Loop
	return

;; --------------------------------------------------------------------------
;;  Table of special characters
;; --------------------------------------------------------------------------
	;; for vertical meters
LC_CLCD_SPECIAL_CHARS_V
	;; due to an imperfection in the MPASM we have
	;; to write two bytes in every line :-(
	;; char #1: bar1
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00011111'; 7th and 8th line of special char
	;; char #2: bar2
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #3: bar3
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #4: bar4
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #5: bar5
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #6: bar6
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00011111', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #7: bar7
	db b'00000000', b'00011111'; 1st and 2nd line of special char
	db b'00011111', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #8: bar8
	db b'00011111', b'00011111'; 1st and 2nd line of special char
	db b'00011111', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char

	;; for horizontal meters
LC_CLCD_SPECIAL_CHARS_H
	;; due to an imperfection in the MPASM we have
	;; to write two bytes in every line :-(
	;; char #1: bar1
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #2: bar2
	db b'00000000', b'00010000'; 1st and 2nd line of special char
	db b'00010000', b'00010000'; 3rd and 4th line of special char
	db b'00010000', b'00010000'; 5th and 6th line of special char
	db b'00010000', b'00000000'; 7th and 8th line of special char
	;; char #3: bar3
	db b'00000000', b'00010100'; 1st and 2nd line of special char
	db b'00010100', b'00010100'; 3rd and 4th line of special char
	db b'00010100', b'00010100'; 5th and 6th line of special char
	db b'00010100', b'00000000'; 7th and 8th line of special char
	;; char #4: bar4
	db b'00000000', b'00010101'; 1st and 2nd line of special char
	db b'00010101', b'00010101'; 3rd and 4th line of special char
	db b'00010101', b'00010101'; 5th and 6th line of special char
	db b'00010101', b'00000000'; 7th and 8th line of special char
	;; char #5: bar5
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000100', b'00000110'; 3rd and 4th line of special char
	db b'00000110', b'00000100'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #6: bar6
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #7: bar7
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #8: bar8
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char

