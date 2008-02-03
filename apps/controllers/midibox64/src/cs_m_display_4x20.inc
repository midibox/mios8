; $Id$
;
; MB64 Menu Display Handler for 4x20 displays
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; Display Page 3 Copyright 2003 by Douglas Pederick (dpederick@pacific.net.au)
; some makeup in the display layout by PayC <bechtold@fh-coburg.de>
;

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

	;; print short information about the last sent MIDI event (8 characters)
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page0_Btn1
CS_M_DISPLAY_Page0_Pot1
	movff	MB64_LAST_POT0, WREG
	call	MB64_LCD_PrintShortEvntStd_Pot
	rgoto	CS_M_DISPLAY_Page0_2ndline
CS_M_DISPLAY_Page0_Btn1
	movff	MB64_LAST_BUTTON0, WREG
	call	MB64_LCD_PrintShortEvntStd_Btn

CS_M_DISPLAY_Page0_2ndline
	;; set cursor to third line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print status of current pot or button row
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page0_Btn2
CS_M_DISPLAY_Page0_Pot2
	movff	MB64_LAST_POT0, WREG
	call	MB64_LCD_PrintLabel
	rgoto	CS_M_DISPLAY_Page0_3rdline
CS_M_DISPLAY_Page0_Btn2
	movff	MB64_LAST_BUTTON0, WREG
	call	MB64_LCD_PrintEvntStd_Btn
	
		;; print pot status at second line
CS_M_DISPLAY_Page0_3rdline
	movlw	0x80
	call	MIOS_LCD_CursorSet
	movff	MB64_LAST_POT0, WREG
	call	MB64_LCD_16PotBars

	;; print button status at fourth line
CS_M_DISPLAY_Page0_4thline
	movlw	0xc0
	call	MIOS_LCD_CursorSet
	movff	MB64_LAST_BUTTON0, WREG
	goto	MB64_LCD_16ButtonBars

;; --------------------------------------------------------------------------
;;  --> Display Page 1
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Page1
CS_M_DISPLAY_Page4
CS_M_DISPLAY_Page1_1stline
	;; set cursor to first line, first column
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; print short information about the last sent MIDI event (8 characters)
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page1_Btn1
CS_M_DISPLAY_Page1_Pot1
	movff	MB64_LAST_POT0, WREG
	call	MB64_LCD_PrintEvntStd_Pot
	rgoto	CS_M_DISPLAY_Page1_2ndline
CS_M_DISPLAY_Page1_Btn1
	movff	MB64_LAST_BUTTON0, WREG
	call	MB64_LCD_PrintEvntStd_Btn

CS_M_DISPLAY_Page1_2ndline
	;; set cursor to third line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print status of pot or button row
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page1_Btn2
CS_M_DISPLAY_Page1_Pot2
	movff	MB64_LAST_POT0, WREG
	call	MB64_LCD_16PotBars
	rgoto	CS_M_DISPLAY_Page1_3rdline
CS_M_DISPLAY_Page1_Btn2
	movff	MB64_LAST_BUTTON0, WREG
	call	MB64_LCD_16ButtonBars
	
	; print pot status horizontal at third line
CS_M_DISPLAY_Page1_3rdline
	movlw	0x80
	call	MIOS_LCD_CursorSet
	movff	MB64_POT_NEW_VALUE, WREG
	call	MB64_LCD_PrintHBar
	
	; print MTC or MIDI clock at fourth line
CS_M_DISPLAY_Page1_4thline
	movlw	0xc0
	call	MIOS_LCD_CursorSet
	goto	MB64_LCD_PrintClkOrMTC


;; --------------------------------------------------------------------------
;;  --> Display Page 2
;; --------------------------------------------------------------------------
; display all 64 pots at once
CS_M_DISPLAY_Page2
; clrf CS_STAT_CURSOR_FLASH
	;; print first pot row status at first line
CS_M_DISPLAY_Page2_1stline
	movlw	0x00
	call	MIOS_LCD_CursorSet
	movlw	0x00
	call	MB64_LCD_16PotBars
	;; print second pot row status at second line
CS_M_DISPLAY_Page2_2ndline
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movlw	0x10
	call	MB64_LCD_16PotBars
	;; print third pot row status at third line
CS_M_DISPLAY_Page2_3rdline
	movlw	0x80
	call	MIOS_LCD_CursorSet
	movlw	0x20
	call	MB64_LCD_16PotBars
	;; print fourth pot row status at fourth line
CS_M_DISPLAY_Page2_4thline
	movlw	0xc0
	call	MIOS_LCD_CursorSet
	movlw	0x30
	goto	MB64_LCD_16PotBars

;; --------------------------------------------------------------------------
;;  --> Display Page 3, written by Douglas Pederick <dpederick@pacific.net.au>
;; --------------------------------------------------------------------------
CS_M_DISPLAY_P3_CPosTop ;Cursor Position Top part of field
	JUMPTABLE_2BYTES 8 ; entries 
	retlw 0x0 
	retlw 0x05 
	retlw 0x0a 
	retlw 0x0f 
	retlw 0x15
	retlw 0x1a 
	retlw 0x1f 
	retlw 0x24 

CS_M_DISPLAY_P3_CPosBot ;Cursor Position Bottom part of field
	JUMPTABLE_2BYTES 8 ; entries 
	retlw 0x40 
	retlw 0x45 
	retlw 0x4a 
	retlw 0x4f 
	retlw 0x55 
	retlw 0x5a 
	retlw 0x5f
	retlw 0x64 

CS_M_DISPLAY_P3_POFF ; potnumber offsets
	JUMPTABLE_2BYTES 8 ; entries 
	retlw 0x0 
	retlw 0x01 
	retlw 0x02 
	retlw 0x03 
	retlw 0x10 
	retlw 0x11 
	retlw 0x12 
	retlw 0x13 

;;Algorithm
;; MB64pot array divided into 8*zones
;; 0-1-2-3
;; 4-5-6-7
;;each zone divided into 8*fields
;; A-B-C-D
;; E-F-G-H
;; When a pot change happens need to
;; calc which zone? zone[zrow,zcol]

;; zrow = pot div 32 
;; zcol = (pot mod 16) div 4
;; ZoneIndex=zrow*4+zcol

;; now have co-ords of zone 0..7 from last changed pot
;; need to know which pot's data to print in each field
;; POTBase=zcol*4+zrow*32
;; A=POTbase+0
;; B=POTbase+1
;; C=POTbase+2
;; D=POTbase+3
;; E=POTbase+16 
;; E=POTbase+17
;; G=POTbase+18
;; H=POTbase+19

;;----permanent storage 
;;CSMD_POT previous pot index
;;CSMD_ZONE previous ZoneIndex
;;CSMD_FIELD previous field
;;---temp storage
;;CSMD_TMP1
;;CSMD_TMP2
;;CSMD_TMP3
;;CSMD_TMP4
;;CSMD_POTBASE holds addr of top left pot in zone
;;CSMD_LOOP counter for printing all fields 
;;CSMD_X

CS_M_DISPLAY_Page3
 	lfsr 	FSR0,MB64_POT_VALUES_00		;;set pointer to pot array 
	SET_BSR MB64_LAST_POT0
	movf 	MB64_LAST_POT0,W		;;get val
	SET_BSR CSMD_BASE
	cpfseq 	CSMD_POT			;;compare to last pot
	rgoto	CS_M_Different_POT 
CS_M_SamePot
	movwf 	CSMD_TMP3			;;hold the pot 
	movf  	CSMD_FIELD,W			;;
	rgoto 	CS_M_PrintPOT			;;print field and EXIT 
CS_M_Different_POT  
	movwf	CSMD_POT			;;store latest POT
;;compute zone index belonging to current pot
	clrf  	CSMD_TMP2			;;ZoneIndex 
	movwf 	CSMD_TMP1			;to temp		
	btfsc 	CSMD_TMP1,5			;div 32
	bsf  	CSMD_TMP2,2			;have zone row
	rrncf 	CSMD_TMP1			;div 2
	rrncf 	CSMD_TMP1,W			;div 2
	andlw 	0x03				;have Zone Col
	movwf 	CSMD_TMP1			;hold Zone Col 
  	addwf 	CSMD_TMP2,F			;have Zone index				
;;compute POTbase (pot number of top left field)
	clrf 	CSMD_POTBASE
	btfsc 	CSMD_TMP2,2      
	bsf 	CSMD_POTBASE,5			;Zrow*32
	rlncf 	CSMD_TMP1
	rlncf 	CSMD_TMP1,W			;Zcol*4
	addwf 	CSMD_POTBASE,F			;;Zcol*4+Zrow*32
;;calc field index and cursorpos
	movf	CSMD_POTBASE,W
	subwf	CSMD_POT,W 			;;thisPOT-POTbase
	movwf	CSMD_FIELD			;;hold interim result 
	movlw	3	
	cpfsgt  CSMD_FIELD
	rgoto	CS_M_topline			;;CSMD_FIELD is fieldindex		
CS_M_bottomline
	movlw	12
	subwf   CSMD_FIELD,F			;;have field index
CS_M_topline 
	movf 	CSMD_ZONE,W			;;get previous zone
	cpfseq 	CSMD_TMP2			;; compare with current zone	
	rgoto	CS_M_Different_Zone ;; 
CS_M_Same_Zone
	movf 	CSMD_POT,W
	movwf 	CSMD_TMP3
	movf 	CSMD_FIELD,W			;;load arg	
	goto 	CS_M_PrintPOT			;;print field and EXIT
CS_M_Different_Zone ;; 
	movff 	CSMD_TMP2,CSMD_ZONE		;;store the zone number	
	movlw 	8
	movwf 	CSMD_LOOP  ;;
	clrf 	CSMD_X
CS_M_DISPLAY_Page3loop:	
;calc pot address--------------
	movf 	CSMD_X,W
	rcall 	CS_M_DISPLAY_P3_POFF		;;potnumber offsets
	addwf 	CSMD_POTBASE,W			;;add offset 
	movwf 	CSMD_TMP3 			;;this is POTnumber
	movf 	CSMD_X,W			;;load field 
	rcall	CS_M_PrintPOT 
	SET_BSR CSMD_BASE
	incf 	CSMD_X,F
	decfsz 	CSMD_LOOP			;;
	rgoto 	CS_M_DISPLAY_Page3loop
	return

CS_M_PrintPOT ;;W has field index
	;;POT=CSMD_TMP3
	movwf	CSMD_TMP4			;save field
	rcall 	CS_M_DISPLAY_P3_CPosBot		;Cursor Position Bottom part of field	
	call 	MIOS_LCD_CursorSet		;;set cursor
	SET_BSR CSMD_BASE
	movff 	CSMD_TMP3,MB64_CURRENT_POT
	call	MB64_ADDR_LabelEntry		; get address to label
	call	MB64_BANK_Read			; read mode byte
	movwf	TMP1				; save mode in TMP1
	bnz	CS_M_LCD_PrintLabel_Ok
	;; no label defined, print standard strin
CS_M_LCD_PrintLabel_Std
	movff	MB64_CURRENT_POT,WREG
	goto	MB64_LCD_PrintEvntStd_Pot
CS_M_LCD_PrintLabel_Ok
	;; print string
	movlw	4
	;;IFSET	TMP1, 5, movlw 5
	movwf	TMP2
CS_M_LCD_PrintLabel_Loop
	call	MB64_BANK_Read
	call	MIOS_LCD_PrintChar
	decfsz	TMP2, F
	rgoto	CS_M_LCD_PrintLabel_Loop
	;--top of field--value 
	SET_BSR CSMD_BASE
	movf 	CSMD_TMP4,W
	rcall 	CS_M_DISPLAY_P3_CPosTop		;Cursor Position Top part of field	
	call	MIOS_LCD_CursorSet		;;set cursor
	call	MB64_FSR_PotValue
	;; continue depending on value mode
	movf	TMP1, W
	call	MB64_LCD_PrintValue
	CALL_IFSET TMP1, 5, ACCESS, MB64_LCD_PrnVBar
	return

