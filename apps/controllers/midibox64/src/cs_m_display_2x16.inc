; $Id$
;
; MB64 Menu Display Handler for 2x16 displays
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Display Page 3 Copyright 2003 by Douglas Pederick (dpederick@pacific.net.au)
;  2x20 Variations Copyright 2004 by Matthias Schreiber <scriptor909@gmx.de>
;  some makeup in the display layout by PayC <bechtold@fh-coburg.de>
; 
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Exit routine is called when the exec button has been pressed
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Exit
	;; set the default LCD offsets (required if we were in CSMD page)
	call	MB64_LCD_SetDefaultOffsets
	;; save page number and exit
	goto	MB64_DUMP_StoreGlobal

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Snapshot
	goto	MB64_BUTTON_Snapshot

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Init
	;; set max number of display pages
	movlw	4
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H
	;; get pointer to MB64_DISPLAY_PAGE and copy value
	;; to CS_MENU_PARAMETER_[LH]
	rcall	CS_M_DISPLAY_Hlp_GetFSR
	movf	INDF0, W
	andlw	0x07
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H
	;; install hook which notifies value changes
	TABLE_ADDR CS_M_DISPLAY_ValueChanged
	call	CS_MENU_SetMenuParameterHook
	;; exception: since this function does some additional display
	;; initialization, we call it also from here to ensure that the
	;; screen has been initialized properly
	call	CS_M_DISPLAY_ValueChanged
	;; init special characters for vertical bars
	goto	MB64_LCD_InitVBars


;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_DISPLAY_ValueChanged
	;; copy menu value to MB64_DISPLAY_PAGE
	rcall	CS_M_DISPLAY_Hlp_GetFSR
	movff	CS_MENU_PARAMETER_L, INDF0

	;; if we are in page 3, set the CSMD Y offsets , else the default Y offsets
	call	MB64_LCD_SetCSMDOffsets
	movlw	0x03
	cpfseq	INDF0, ACCESS
	call MB64_LCD_SetDefaultOffsets

	;; some additional initialization, should
	;; be called after every page change
	call	MIOS_LCD_Clear
	SET_BSR CSMD_BASE	; select RAM page
	movlw 	0xff
	movwf 	CSMD_ZONE	; for page3: force complete display refresh
	movwf	CSMD_POT	;

	return


;; ==========================================================================
	;; returns pointer to MB64_DISPLAY_PAGE in FSR0
CS_M_DISPLAY_Hlp_GetFSR
	lfsr	FSR0, MB64_DISPLAY_PAGE
	return
	
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Tick
	;; branch depending on selected page
	SET_BSR	MB64_BASE
	movf	MB64_DISPLAY_PAGE, W, BANKED
	JUMPTABLE_2BYTES 5	; 5 entries
	rgoto	CS_M_DISPLAY_Page0
	rgoto	CS_M_DISPLAY_Page1
	rgoto	CS_M_DISPLAY_Page2
	rgoto	CS_M_DISPLAY_Page3
	rgoto	CS_M_DISPLAY_Page4


;; --------------------------------------------------------------------------
;;  --> Display Page 0
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Page0
CS_M_DISPLAY_Page0_1stline
	;; set cursor to first line, first column
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; print bank information and pot mode (8 characters)
	call	MB64_LCD_PrintBank

	;; print 8 bars of the current pot or button row
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page0_Btn1
CS_M_DISPLAY_Page0_Pot1
	movff	MB64_LAST_POT0, WREG
	call	MB64_LCD_8PotBars
	rgoto	CS_M_DISPLAY_Page0_2ndline
CS_M_DISPLAY_Page0_Btn1
	movff	MB64_LAST_BUTTON0, WREG
	call	MB64_LCD_8ButtonBars
	;; 	rgoto	CS_M_DISPLAY_Page0_2ndline

CS_M_DISPLAY_Page0_2ndline
	;; set cursor to second line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print status of current pot or button row
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page0_Btn2
CS_M_DISPLAY_Page0_Pot2
	movff	MB64_LAST_POT0, WREG
	goto	MB64_LCD_PrintLabel
CS_M_DISPLAY_Page0_Btn2
	movff	MB64_LAST_BUTTON0, WREG
	goto	MB64_LCD_PrintEvntStd_Btn

;; --------------------------------------------------------------------------
;;  --> Display Page 1
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Page1
	;; print pot status at first line
CS_M_DISPLAY_Page1_1stline
	movlw	0x00
	call	MIOS_LCD_CursorSet
	movff	MB64_LAST_POT0, WREG
	call	MB64_LCD_16PotBars

	;; print button status at second line
CS_M_DISPLAY_Page1_2ndline
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movff	MB64_LAST_BUTTON0, WREG
	goto	MB64_LCD_16ButtonBars

;; --------------------------------------------------------------------------
;;  --> Display Page 2
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Page2
CS_M_DISPLAY_Page2_1stline
	;; set cursor to first line, first column
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; print bank information and pot mode (8 characters)
	call	MB64_LCD_PrintBank

	;; print short information about the last sent MIDI event (8 characters)
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page2_Btn1
CS_M_DISPLAY_Page2_Pot1
	movff	MB64_LAST_POT0, WREG
	call	MB64_LCD_PrintShortEvntStd_Pot
	rgoto	CS_M_DISPLAY_Page2_2ndline
CS_M_DISPLAY_Page2_Btn1
	movff	MB64_LAST_BUTTON0, WREG
	call	MB64_LCD_PrintShortEvntStd_Btn

CS_M_DISPLAY_Page2_2ndline
	;; set cursor to second line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print status of pot or button row
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page2_Btn2
CS_M_DISPLAY_Page2_Pot2
	movff	MB64_LAST_POT0, WREG
	goto	MB64_LCD_16PotBars
CS_M_DISPLAY_Page2_Btn2
	movff	MB64_LAST_BUTTON0, WREG
	goto	MB64_LCD_16ButtonBars


;; --------------------------------------------------------------------------
;;  --> Display Page 3, written by Douglas Pederick <dpederick@pacific.net.au>
;;  modified for 2x20 displays by Matthias Schreiber <scriptor909@gmx.de>
;; --------------------------------------------------------------------------
CS_M_DISPLAY_P3_CPosTop ; Cursor Position Top Part of field (PotValue)
	JUMPTABLE_2BYTES 4
	retlw 0x0
	retlw 0x4
	retlw 0x8
	retlw 0xC

CS_M_DISPLAY_P3_CPosBot ; Cursor Position Bottom part of filed (PotLabel)
	JUMPTABLE_2BYTES 4
	retlw 0x40
	retlw 0x44
	retlw 0x48
	retlw 0x4C	

CS_M_DISPLAY_P3_POFF ;potnumber offsets
	JUMPTABLE_2BYTES 4
	retlw 0x0
	retlw 0x1
	retlw 0x2
	retlw 0x3

;;-----+------------+------------+------------+------------+
;;Zone |     0	    |      1     |      2     |     3      |
;;Pot  | 0  1  2  3 | 4  5  6  7 | 8  9 10 11 |12 13 14 15 |
;;-----+------------+------------+------------+------------+
;;Zone |     4	    |      5     |      6     |     7      |
;;Pot  |16 17 18 19 |20 21 22 23 |24 25 26 27 |28 29 30 31 |
;;-----+------------+------------+------------+------------+
;;Zone |     8	    |      9     |      10    |     11     |
;;Pot  |32 33 34 35 |36 37 38 39 |40 41 42 43 |44 45 46 47 |
;;-----+------------+------------+------------+------------+
;;Zone |     12	    |     13     |     14     |     15     |
;;Pot  |48 49 50 51 |52 53 54 55 |56 57 58 59 |60 61 62 63 |
;;-----+------------+------------+------------+------------+

;;ZoneIndex = (LAST_POT0 / 4) & 0xF
;;PotBase = ZoneIndex * 0x4
;;Field = LAST_POT0 - PotBase

;;----permantent storage
;;CSMD_POT - previous pot index
;;CSMD_ZONE - previous ZoneIndex
;;CSMD_FIELD - previous field
;;----temp storage
;;CSMD_TMP1 - not used
;;CSMD_TMP2 - Zone
;;CSMD_TMP3 - actuell Pot
;;CSMD_TMP4 - actuell Field to print
;;CSMD_POTBASE - address of top left pot in zone (POT0, 4, 8, ...)
;;CSMD_LOOP - counter for printing all fields
;;CSMD_X - field counter for printing
;;TMP1 - Display Mode (HEX, DEZ, +/- HEX, ...)
;;TMP2 - Counter for Character print

CS_M_DISPLAY_Page3
	lfsr	FSR0,MB64_POT_VALUES_00		;;set pointer to pot array
	SET_BSR	MB64_LAST_POT0
	movf	MB64_LAST_POT0,W		;;get val
	SET_BSR	CSMD_BASE
	cpfseq	CSMD_POT			;;compare to last pot
	rgoto	CS_M_Different_POT
CS_M_SamePot
	movwf	CSMD_TMP3			;;hold the pot
	movf	CSMD_FIELD,W
	rgoto	CS_M_PrintPOT
CS_M_Different_POT
	movwf	CSMD_POT			;;store latest POT

;;compute ZoneIndex belonging to current pot
	clrf	CSMD_TMP2			;;CSMD_TMP2 = 0
	movwf	CSMD_TMP1
	rrncf	CSMD_TMP1, F			;;MB64_LAST_POT0 div 2
	rrncf	CSMD_TMP1,W			;;MB64_LAST_POT0 div 2
	andlw	0xF
	movwf	CSMD_TMP2			;;have ZoneIndex

;;compute POTbase (pot number of top left field)
	clrf	CSMD_POTBASE			;;CSMD_POTBASE = 0
	movf	CSMD_TMP2,W
	movwf	CSMD_TMP1
	rlncf	CSMD_TMP1, F			;;ZoneIndex * 2
	rlncf	CSMD_TMP1,W			;;ZoneIndex * 2
	movwf	CSMD_POTBASE

;;calc field index and cursorposition
	movf	CSMD_POTBASE,W
	subwf	CSMD_POT,W			;;latestPot - POTbase
	movwf	CSMD_FIELD			;;have FieldIndex

	movf	CSMD_ZONE,W			;;get previous Zone
	cpfseq	CSMD_TMP2			;;compare with current zone
	rgoto	CS_M_Different_Zone

CS_M_Same_Zone
	movf	CSMD_POT,W
	movwf	CSMD_TMP3			;;CSMD_TMP3 = CSMD_POT
	movf	CSMD_FIELD,W			;;W = CSMD_FIELD
	goto	CS_M_PrintPOT
CS_M_Different_Zone
	movff	CSMD_TMP2,CSMD_ZONE		;;CSMD_ZONE = CSMD_TMP2
	movlw	4
	movwf	CSMD_LOOP			;;CSMD_LOOP = 4
	clrf	CSMD_X

CS_M_DISPLAY_Page3loop

;;calc pot address
	movf	CSMD_X,W
	rcall	CS_M_DISPLAY_P3_POFF		;;get potnumber-offset
	addwf	CSMD_POTBASE,W			;;add offset
	movwf	CSMD_TMP3			;;this is potnumber		
	movf	CSMD_X,W			;;load field to print
	rcall	CS_M_PrintPOT
	SET_BSR	CSMD_BASE
	incf	CSMD_X,F
	decfsz	CSMD_LOOP, F
	rgoto	CS_M_DISPLAY_Page3loop
	return

CS_M_PrintPOT	;;W = field index
		;;CSMD_TMP3 = pot to print
	movwf	CSMD_TMP4
	rcall	CS_M_DISPLAY_P3_CPosBot		;;Cursor Position PotLabel (2nd line)
	call	MIOS_LCD_CursorSet
	SET_BSR	CSMD_BASE
	movff	CSMD_TMP3,MB64_CURRENT_POT
	call	MB64_ADDR_LabelEntry		;;get address to PotLabel
	call	MB64_BANK_Read			;;read mode byte (HEX; DEZ; +/- HEX)
	movwf	TMP1				;;TMP1 = Mode
	bnz	CS_M_LCD_PrintLabel_Ok

	;;no label defined, print standard string
CS_M_LCD_PrintLabel_Std
	movff	MB64_CURRENT_POT,WREG
	goto	MB64_LCD_PrintEvntStd_Pot

CS_M_LCD_PrintLabel_Ok
	;; print string
	movlw	4
	movwf	TMP2				;;TMP2 = 4 (4 characters to print)
CS_M_LCD_PrintLabel_Loop
	call	MB64_BANK_Read
	call	MIOS_LCD_PrintChar
	decfsz	TMP2,F
	rgoto	CS_M_LCD_PrintLabel_Loop

;;print 1st line (PotValue)
	SET_BSR	CSMD_BASE
	movf	CSMD_TMP4,W
	rcall	CS_M_DISPLAY_P3_CPosTop		;;cursor position of POTValue (1st line)
	call	MIOS_LCD_CursorSet		;;set cursor
	call	MB64_FSR_PotValue
	movf	TMP1,W				;;W = Mode
	call	MB64_LCD_PrintValue
	CALL_IFSET TMP1, 5, ACCESS, MB64_LCD_PrnVBar
	return

;; --------------------------------------------------------------------------
;;  --> Display Page 4
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Page4
CS_M_DISPLAY_Page4_1stline
	;; print MTC or MIDI clock at first line
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	MB64_LCD_PrintClkOrMTC

CS_M_DISPLAY_Page4_2ndline
	;; set cursor to second line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print status of current pot or button row
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page4_Btn2
CS_M_DISPLAY_Page4_Pot2
	movff	MB64_LAST_POT0, WREG
	goto	MB64_LCD_PrintLabel
CS_M_DISPLAY_Page4_Btn2
	movff	MB64_LAST_BUTTON0, WREG
	goto	MB64_LCD_PrintEvntStd_Btn

