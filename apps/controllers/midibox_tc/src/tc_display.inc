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

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the display content should be 
;;  initialized. Thats the case during startup and after a temporary message
;;  has been printed on the screen
;; --------------------------------------------------------------------------
USER_DISPLAY_Init
	;; just request a display update
	bsf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ
	;; in the next cycle USER_DISPLAY_Tick will be called which
	;; does the job
	return

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------

USER_DISPLAY_Tick
TC_DISPLAY_Page
	;; check if a display update has been requested (TC_STAT.TC_STAT_DISPLAY_UPDATE_REQ set)
	;; do nothing if flag is zero
	btfss	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ
	return

	;; clear request flag
	bcf	TC_STAT, TC_STAT_DISPLAY_UPDATE_REQ

	;; branch depending on TC_MODE
	movf	TC_MODE, W
	JUMPTABLE_2BYTES 2		; (2 entries)
	rgoto	TC_DISPLAY_Page_Menu	; if TC_MODE_MENU
	rgoto	TC_DISPLAY_Page_Param	; if TC_MODE_PARA

;; --------------------------------------------------------------------------
;;  print the menu page
;; --------------------------------------------------------------------------
MENU_PAGE_0	STRING 20, 0x00, "MIDIbox Tiny Control"

TC_DISPLAY_Page_Menu
	;; print the strings above
	TABLE_ADDR MENU_PAGE_0
	call	MIOS_LCD_PrintString	; first line

	;; set cursor to second line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; check flash flag
	;; which is handled by tc_timer.inc
	;; if flag is set, print bank label, else print spaces
	BRA_IFSET TC_STAT, TC_STAT_CURSOR_FLASH, ACCESS, TC_DISPLAY_Page_Menu_Val
TC_DISPLAY_Page_Menu_NoVal
	;; print 16 spaces
	movlw	16
	rcall	TC_DISPLAY_Hlp_PrintSpaces
	rgoto	TC_DISPLAY_Page_Menu_Cont

TC_DISPLAY_Page_Menu_Val
	;; calculate pointer to the bank label (defined in tc_labels.inc):
	;; TC_BANK_LABELS + 16 * TC_BANK
	TABLE_ADDR TC_BANK_LABELS		; this macro sets the base address
	movf	TC_BANK, W			; get bank number
	mullw	16				; * 16 -> result in PROD[LH]
	movf	PRODL, W			; add offset (bank*16) to TABLPTR
	addwf	TBLPTRL, F			; low-byte
	movf	PRODH, W
	addwfc	TBLPTRH, F			; high-byte (+carry bit on overrun)

	;; print string by using the MIOS_LCD_PrintPreconfString function
	movlw	16		; 16 characters
	call	MIOS_LCD_PrintPreconfString
TC_DISPLAY_Page_Menu_Cont

	;; fill the rest of the page with spaces
	movlw	4
	rcall	TC_DISPLAY_Hlp_PrintSpaces

	;; thats all
	return


;; --------------------------------------------------------------------------
;;  print the parameters
;; --------------------------------------------------------------------------
TC_DISPLAY_Page_Param
	;; check if a "long label" has been requested
	;; thats the case after a select button has been pressed or
	;; the rotary encoder has been moved
	BRA_IFCLR TC_STAT, TC_STAT_DISPLAY_LONGLABEL_REQ, ACCESS, TC_DISPLAY_Page_ParamShort
TC_DISPLAY_Page_ParamLong
	;; clear request
	bcf	TC_STAT, TC_STAT_DISPLAY_LONGLABEL_REQ

	;; calc parameter number: TC_BANK * VALUES_PER_PAGE + TC_PARA
	movf	TC_BANK, W		; get Bank
	mullw	VALUES_PER_PAGE		; * 5 -> result in PRODL (PRODH can be ignored here, since we have 256 parameters max)
	movf	PRODL, W
	addwf	TC_PARA, W
	;; store parameter number in TMP1
	movwf	TMP1

	;; first line: print the long label and a value
	movlw	0x00			; set cursor to first line, first column
	call	MIOS_LCD_CursorSet

	;; print long label
	movf	TMP1, W			; parameter number has been stored in TMP1
	call	TC_DISPLAY_Page_PrintLLabel

	;; print 4 additional spaces
	movlw	4
	rcall	TC_DISPLAY_Hlp_PrintSpaces

	;; second line: print a nice horizontal bar
	movlw	0x40			; set cursor to second line, first column
	call	MIOS_LCD_CursorSet

	;; print horizontal bar
	movf	TMP1, W			; parameter number has been stored in TMP1
	call	TC_DISPLAY_Page_HBar

	;; print 4 additional spaces
	movlw	4
	rcall	TC_DISPLAY_Hlp_PrintSpaces

	;; set the "select counter" which is handled by tc_timer.inc
	;; it will be decremented, when it is zero, the short parameter labels
	;; will be displayed again
	movlw	0x40
	movwf	TC_SELECT_CTR
	
	return
	
	
	
TC_DISPLAY_Page_ParamShort
	
	;; first line: print the parameter labels
	movlw	0x00			; set cursor to first line, first column
	call	MIOS_LCD_CursorSet

	;; TMP1 contains the parameter number, 
	;; it's: TC_BANK * VALUES_PER_PAGE + (0, 1, ... VALUES_PER_PAGE)
	movf	TC_BANK, W		; get Bank
	mullw	VALUES_PER_PAGE		; * 5 -> result in PRODL (PRODH can be ignored here, since we have 256 parameters max)
	movff	PRODL, TMP1		; save in TMP1
	;; TMP2 used as loop counter
	clrf	TMP2
TC_DISPLAY_Page_ParamLabel_Loop
	;; check if TMP2 is equal to TC_PARA
	movf	TMP2, W
	cpfseq	TC_PARA, ACCESS
	rgoto TC_DISPLAY_Page_ParamLabel_NSel
	;; yes: print the value depending on the flash flag
	;; which is handled by tc_timer.inc
	;; if flag is set, print label, else print spaces
	BRA_IFSET TC_STAT, TC_STAT_CURSOR_FLASH, ACCESS, TC_DISPLAY_Page_ParamLabel_NSel
TC_DISPLAY_Page_ParamLabel_Sel
	;; else print 4 spaces
	movlw	4
	rcall	TC_DISPLAY_Hlp_PrintSpaces
	rgoto	TC_DISPLAY_Page_ParamLabel_Next

TC_DISPLAY_Page_ParamLabel_NSel
	movf	TMP1, W			; print short label of parameter in TMP1
	rcall	TC_DISPLAY_Page_PrintSLabel
TC_DISPLAY_Page_ParamLabel_Next
	incf	TMP1, F			; increment parameter number
	incf	TMP2, F			; increment loop counter
	movlw	VALUES_PER_PAGE		; loop until VALUES_PER_PAGE has been reached
	cpfseq	TMP2, ACCESS
	rgoto TC_DISPLAY_Page_ParamLabel_Loop

	;; second line: print the values
	movlw	0x40			; set cursor to second line, first column
	call	MIOS_LCD_CursorSet

	;; TMP1 contains the parameter number, 
	;; it's: TC_BANK * VALUES_PER_PAGE + (0, 1, ... VALUES_PER_PAGE)
	movf	TC_BANK, W		; get Bank
	mullw	VALUES_PER_PAGE		; * 5 -> result in PRODL (PRODH can be ignored here, since we have 256 parameters max)
	movff	PRODL, TMP1		; save in TMP1
	;; TMP2 used as loop counter, we want to print VALUES_PER_PAGE labels
	movlw	VALUES_PER_PAGE
	movwf	TMP2
TC_DISPLAY_Page_ParamVal_Loop
	movlw	' '			; print space
	call	MIOS_LCD_PrintChar

	movf	TMP1, W			; print value of parameter in TMP1
	rcall	TC_DISPLAY_Page_PrintVal
	incf	TMP1, F			; increment parameter number
	decfsz	TMP2, F			; decrement loop counter until zero
	rgoto	TC_DISPLAY_Page_ParamVal_Loop

	;; thats all
	return


;; --------------------------------------------------------------------------
;;  print the long parameter label and value
;;  IN: number of parameter (0x00-0xff) in WREG
;; --------------------------------------------------------------------------
TC_DISPLAY_Page_PrintLLabel
	;; temporary save number of parameter in FSR0L
	movwf	FSR0L

	;; this help function calculates the pointer to the long label
	rcall	TC_DISPLAY_Hlp_CalcPtr_LLabel

	;; print string by using the MIOS_LCD_PrintPreconfString function
	movlw	13		; 13 characters
	call	MIOS_LCD_PrintPreconfString

	;; print value and exit
	movf	FSR0L, W	; parameter number has been saved in FSR0L
	goto	TC_DISPLAY_Page_PrintVal


;; --------------------------------------------------------------------------
;;  print a parameter label
;;  IN: number of parameter (0x00-0xff) in WREG
;; --------------------------------------------------------------------------
TC_DISPLAY_Page_PrintSLabel
	;; this help function calculates the pointer to the short label
	rcall	TC_DISPLAY_Hlp_CalcPtr_SLabel

	;; print string by using the MIOS_LCD_PrintPreconfString function
	movlw	4		; 4 characters
	call	MIOS_LCD_PrintPreconfString

	return


;; --------------------------------------------------------------------------
;;  print a parameter value
;;  IN: number of parameter (0x00-0xff) in WREG
;; --------------------------------------------------------------------------
TC_DISPLAY_Page_PrintVal
	;; calculate pointer to first parameter: FSR0 = TC_VALUES_BEGIN, low byte=WREG
	lfsr	FSR0, TC_VALUES_BEGIN	; base address
	movwf	FSR0L			; write WREG to FSR0L

	;; this help function calculates the pointer to the parameter value mode
	rcall	TC_DISPLAY_Hlp_CalcPtr_VMode

	;; branch depending on selected mode
	tblrd*			; read table entry
	movf	TABLAT, W	; get content
	JUMPTABLE_2BYTES 4	; 4 entries (defined in app_defines.h)
	rgoto	TC_DISPLAY_Print_Hex	; if PARA_VALUE_HEX
	rgoto	TC_DISPLAY_Print_PmHex	; if PARA_VALUE_PM_HEX
	rgoto	TC_DISPLAY_Print_Dec	; if PARA_VALUE_DEC
	rgoto	TC_DISPLAY_Print_PmDec	; if PARA_VALUE_PM_DEC

	;; print hex value 0x00..0x7f
TC_DISPLAY_Print_Hex
	movlw	' '		; fill with space
	call	MIOS_LCD_PrintChar
	movf	INDF0, W
	goto	MIOS_LCD_PrintHex2

	;; print hex value -0x40..0x3f
TC_DISPLAY_Print_PmHex
	movlw	' '		; space or "-"?
	btfss	INDF0, 6; (if value <= 0x3f (6th bit cleared), print "-"
	movlw '-'
	call	MIOS_LCD_PrintChar

	movf	INDF0, W		; calc: 0x40-value
	sublw	0x40
	btfsc	WREG, 7			; negate value if negative to get absolute value
	negf	WREG, ACCESS
	goto	MIOS_LCD_PrintHex2	; and print it

	;; print dec value 1..128
TC_DISPLAY_Print_Dec
	incf	INDF0, W
	goto	MIOS_LCD_PrintBCD3

	;; print dec value -64..63
TC_DISPLAY_Print_PmDec
	movlw	' '		; space or "-"?
	btfss	INDF0, 6; (if value <= 0x3f (6th bit cleared), print "-"
	movlw '-'
	call	MIOS_LCD_PrintChar

	movf	INDF0, W		; calc: 0x40-value
	sublw	0x40
	btfsc	WREG, 7			; negate value if negative to get absolute value
	negf	WREG, ACCESS
	goto	MIOS_LCD_PrintBCD2	; and print it



;; --------------------------------------------------------------------------
;;  This help function prints n spaces
;;  IN: number of spaces in WREG
;; --------------------------------------------------------------------------
TC_DISPLAY_Hlp_PrintSpaces
	movwf	TMP5		; using TMP5 as loop counter
TC_DISPLAY_Hlp_PrintSpaces_Loop
	movlw	' '
	call	MIOS_LCD_PrintChar
	decfsz	TMP5, F		; decrement TMP5, skip if zero
	rgoto	TC_DISPLAY_Hlp_PrintSpaces_Loop

	return


;; --------------------------------------------------------------------------
;;  This help function prints a horizontal bar
;;  IN: number of parameter (0x00-0xff) in WREG
;;  USES: (!) TMP3, TMP4, TMP5
;; --------------------------------------------------------------------------
TC_DISPLAY_Page_HBar
	;; calculate pointer to first parameter: FSR0 = TC_VALUES_BEGIN, low byte=WREG
	lfsr	FSR0, TC_VALUES_BEGIN	; base address
	movwf	FSR0L			; write WREG to FSR0L

	;; get position of the zero-marker ('|')
	rcall	TC_DISPLAY_Page_HBar_MarkerPos

	;; save position in TMP3
	movwf	TMP3

	;; 16 chars to print
	;; length of bar is: value / 8 (since max value is 127)
	movf	INDF0, W
	rrf	WREG, F		; WREG/2
	rrf	WREG, F		; WREG/2
	rrf	WREG, W		; WREG/2
	andlw	0x0f		; mask out result (0x00-0x0f)

	;; save result in TMP5
	movwf	TMP5

	;; start loop, 16 chars to print, example: #####-----------
	clrf	TMP4
TC_DISPLAY_Page_HBar_Loop
	movf	TMP4, W
	cpfseq	TMP3, ACCESS
	rgoto TC_DISPLAY_Page_HBar_Loop_M12
TC_DISPLAY_Page_HBar_Loop_Mark
	incf	TMP5, F		; only valid for panorama: increment "#" pos
	movlw	'|'		; print vertical bar
	rgoto	TC_DISPLAY_Page_HBar_Loop_Cont

TC_DISPLAY_Page_HBar_Loop_M12
	movf	TMP4, W
	cpfsgt	TMP5, ACCESS
	rgoto TC_DISPLAY_Page_HBar_Loop_M2
TC_DISPLAY_Page_HBar_Loop_M1
	movlw	'#'
	rgoto	TC_DISPLAY_Page_HBar_Loop_Cont
TC_DISPLAY_Page_HBar_Loop_M2
	movlw	'-'
	;; 	rgoto	TC_DISPLAY_Page_HBar_Loop_Cont
TC_DISPLAY_Page_HBar_Loop_Cont
	call	MIOS_LCD_PrintChar

	incf	TMP4, F		; increment
	movlw	16
	cpfseq	TMP4, ACCESS
	rgoto TC_DISPLAY_Page_HBar_Loop

	return


	;; help function: marker pos depends on selected value mode
	;; IN: parameter number in WREG
TC_DISPLAY_Page_HBar_MarkerPos
	rcall	TC_DISPLAY_Hlp_CalcPtr_VMode	; get pointer to value mode entry
	tblrd*					; read table entry
	movf	TABLAT, W			; get table entry and branch
	JUMPTABLE_2BYTES 4	; 4 entries
	retlw	15		; PARA_VALUE_HEX:    return 15 (i.E on volume)
	retlw	8		; PARA_VALUE_PM_HEX: return 8  (i.E on panorama)
	retlw	15		; PARA_VALUE_DEC:    return 15 (i.E on volume)
	retlw	8		; PARA_VALUE_PM_DEC: return 8  (i.E on panorama)

;; --------------------------------------------------------------------------
;;  This help function calculates the pointer to the base offset to
;;  a parameter entry in TC_PARA_LABELS
;;  IN: number of parameter (0x00-0xff) in WREG
;;  OUT: pointer in TBLPTR[LH]
;; --------------------------------------------------------------------------
TC_DISPLAY_Hlp_CalcPtr_BOffset
	;; calculate pointer to the base offset (defined in tc_labels.inc):
	;; TC_PARA_LABELS + 18 * parameter_number
	mullw	18				; WREG * 18, result in PROD[LH]
	movlw	TC_PARA_LABELS & 0xff		; add low/high byte of table, result in TBLPTR
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	TC_PARA_LABELS >> 8
	addwfc	PRODH, W
	movwf	TBLPTRH
	return

;; --------------------------------------------------------------------------
;;  This help function calculates the pointer to the short label
;;  IN: number of parameter (0x00-0xff) in WREG
;;  OUT: pointer in TBLPTR[LH]
;; --------------------------------------------------------------------------
TC_DISPLAY_Hlp_CalcPtr_SLabel	
	;; calculate pointer to the short label (defined in tc_labels.inc):
	;; TC_PARA_LABELS + 18 * parameter_number
	rgoto	TC_DISPLAY_Hlp_CalcPtr_BOffset

;; --------------------------------------------------------------------------
;;  This help function calculates the pointer to the long label
;;  IN: number of parameter (0x00-0xff) in WREG
;;  OUT: pointer in TBLPTR[LH]
;; --------------------------------------------------------------------------
TC_DISPLAY_Hlp_CalcPtr_LLabel	
	;; calculate pointer to the long label (defined in tc_labels.inc):
	;; TC_PARA_LABELS + 18 * parameter_number + 4

	;; calculate TC_PARA_LABELS + 18 * parameter_number
	rcall	TC_DISPLAY_Hlp_CalcPtr_BOffset

	;; add 4 (take the 16 bit result into account!)
	movlw	4
	addwf	TBLPTRL, F
	movlw	0
	addwfc	TBLPTRH, F
	return

;; --------------------------------------------------------------------------
;;  This help function calculates the pointer to the value mode
;;  IN: number of parameter (0x00-0xff) in WREG
;;  OUT: pointer in TBLPTR[LH]
;; --------------------------------------------------------------------------
TC_DISPLAY_Hlp_CalcPtr_VMode
	;; calculate pointer to the long label (defined in tc_labels.inc):
	;; TC_PARA_LABELS + 18 * parameter_number + 17

	;; calculate TC_PARA_LABELS + 18 * parameter_number
	rcall	TC_DISPLAY_Hlp_CalcPtr_BOffset

	;; add 17 (take the 16 bit result into account!)
	movlw	17
	addwf	TBLPTRL, F
	movlw	0
	addwfc	TBLPTRH, F
	return

	
