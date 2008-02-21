; $Id$
;
; SEQ LCD functions, mainly used by cs_m_display.inc
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function clears both LCDs
;;  IN:  -
;;  OUT: -
;; --------------------------------------------------------------------------
SEQ_LCD_Clear
	movlw	0x80
	call	MIOS_LCD_CursorSet
	call	MIOS_LCD_Clear

	movlw	0x00
	call	MIOS_LCD_CursorSet
	goto	MIOS_LCD_Clear


;; --------------------------------------------------------------------------
;;  This functions sets the Y offsets for a menu page
;;  IN:  -
;;  OUT: -
;; --------------------------------------------------------------------------
SEQ_LCD_SetPageOffsets
	;; init offsets for two * 2x40 LCDs
	;; also valid for a single 2x16 LCD (cs_m_* menu pages select a special layout depending on DEFAULT_LCD_WIDTH)
	movlw	0x00
#if DEFAULT_LCD_WIDTH == 20
	addlw	2
#endif
	movwf	MIOS_PARAMETER1
	movlw	0x40
#if DEFAULT_LCD_WIDTH == 20
	addlw	2
#endif
	movwf	MIOS_PARAMETER2
	movlw	0x80
#if DEFAULT_LCD_WIDTH == 20
	addlw	2
#endif
	movwf	MIOS_PARAMETER3
	movlw	0xc0
#if DEFAULT_LCD_WIDTH == 20
	addlw	2
#endif
	goto	MIOS_LCD_YAddressSet


;; ==========================================================================
;; starts a message which notifies, that no pattern is selected
;; ==========================================================================
SEQ_LCD_MSG_TRK_DISABLED STRING 30, 0x00, "*** No pattern selected on ***"
			 STRING 30, 0x40, "***    current Track(s)    ***"
SEQ_LCD_MsgTrkDisabled
	TABLE_ADDR SEQ_LCD_MSG_TRK_DISABLED
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage
	

;; ==========================================================================
;; print selected/muted tracks
;; IN: WREG: group number
;;     SEQ_SELECTED_TRKS: selected tracks
;; USES: BSR, TMP[12]
;; ==========================================================================
SEQ_LCD_PrnSelectedTrks
	SET_BSR	SEQ_BASE

	movwf	TMP1		; temporary save group which should be displayed in TMP1

	;; determine muted tracks
	BRA_IFSET TMP1, 1, ACCESS, SEQ_LCD_PrnSelectedTrks_M23
SEQ_LCD_PrnSelectedTrks_M01
	movf	SEQ_TRKS_MUTED0, W, BANKED	; get muted[3:0]
	btfsc	TMP1, 0
	swapf	SEQ_TRKS_MUTED0, W, BANKED	; get muted[7:4]
	rgoto	SEQ_LCD_PrnSelectedTrks_M_Cont

SEQ_LCD_PrnSelectedTrks_M23
	movf	SEQ_TRKS_MUTED1, W, BANKED	; get muted[11:8]
	btfsc	TMP1, 0
	swapf	SEQ_TRKS_MUTED1, W, BANKED	; get muted[15:12]
	;; 	rgoto	SEQ_LCD_PrnSelectedTrks_M_Cont

SEQ_LCD_PrnSelectedTrks_M_Cont
	movwf	TMP2				; store value in TMP2 (only [3:0] used below)

	;; TODO change for 808
	lfsr	FSR1, SEQ_SELECTED_TRKS_0
	movlw	0
	movf	PLUSW1, W
	andlw	0x0f
	movwf	TMP1				; TMP1 stores the selected tracks of a group
SEQ_LCD_PrnSelectedTrks_Cont

	;; TMP1 holds the selected tracks
	;; TMP2 holds the muted tracks
	
	movlw	'1'
	btfsc	TMP1, 0; (selected track)
	movlw '*'	
	btfsc	TMP2, 0; (muted track)
	movlw '-'	
	call	MIOS_LCD_PrintChar

	movlw	'2'
	btfsc	TMP1, 1; (selected track)
	movlw '*'	
	btfsc	TMP2, 1; (muted track)
	movlw '-'	
	call	MIOS_LCD_PrintChar
	
	movlw	'3'
	btfsc	TMP1, 2; (selected track)
	movlw '*'	
	btfsc	TMP2, 2; (muted track)
	movlw '-'	
	call	MIOS_LCD_PrintChar
	
	movlw	'4'
	btfsc	TMP1, 3; (selected track)
	movlw '*'	
	btfsc	TMP2, 3; (muted track)
	movlw '-'	
	goto	MIOS_LCD_PrintChar

;; ==========================================================================
;;  print vertical bar
;;  IN:  7-bit value in WREG
;;  OUT: 1 character
;;  USES: BSR
;; ==========================================================================
SEQ_LCD_PrnVBar
	swapf	WREG, W
	andlw	0x07
	;; finally print character and exit
	goto	MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------
;;  This functions prints a horizontal meter
;;  IN:  7-bit value in WREG
;;  OUT: 5 characters
;;  USES: BSR, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LCD_PrnHBar
	;; calc pointer to table entry: offset = (value/8) * 6 = (value*32/256) * 6
	mullw	32		; *32
	movf	PRODH, W	; (/256)
	mullw	6		; *6
	;; add to offset
	TABLE_ADDR SEQ_LCD_PrnHBar_Table
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F

	;; print preconfigured string
	movlw	5		; (5 chars)
	goto	MIOS_LCD_PrintPreconfString

SEQ_LCD_PrnHBar_Table
	;; special chars which should be print depending on meter value (16 entries, only 14 used)
	;; only 5 are used, the 6th has to be inserted due to a MPASM imperfection
	db	4, 0, 0, 0, 0, 0
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
	db	3, 3, 3, 3, 3, 1


;; ==========================================================================
;; print MIDI channel number (0: disabled)
;; ==========================================================================
SEQ_LCD_PrnChn
	andlw	0xff
	bz	SEQ_LCD_PrnChn_Dis
SEQ_LCD_PrnChn_NotDis
	goto	MIOS_LCD_PrintBCD2
SEQ_LCD_PrnChn_Dis
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	goto	MIOS_LCD_PrintChar

;; ==========================================================================
;; print pattern name
;; ==========================================================================
SEQ_LCD_PrnPattern
	BRA_IFSET WREG, 7, ACCESS, SEQ_LCD_PrnPattern_Dis
	movwf	PRODL
	rlf	WREG, W
	swapf	WREG, W
	andlw	0x07
	btfss	PRODL, 6
	addlw 'A'
	btfsc	PRODL, 6
	addlw 'a'
	call	MIOS_LCD_PrintChar
	movf	PRODL, W
	andlw	0x07
	addlw	'1'
	goto	MIOS_LCD_PrintChar

SEQ_LCD_PrnPattern_Dis
SEQ_LCD_PrnSongPos_Dis		; re-used
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	goto	MIOS_LCD_PrintChar

;; ==========================================================================
;; prints group/track (4 characters)
;; ==========================================================================
SEQ_LCD_PrnGxTy
	movwf	PRODL

	movlw	'G'
	call	MIOS_LCD_PrintChar
	rrf	PRODL, W
	rrf	WREG, W
	andlw	0x03
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	'T'
	call	MIOS_LCD_PrintChar
	movf	PRODL, W
	andlw	0x03
	addlw	1
	goto	MIOS_LCD_PrintBCD1

;; ==========================================================================
;; print song position (uses PRODL)
;; ==========================================================================
SEQ_LCD_PrnSongPos
	BRA_IFSET WREG, 7, ACCESS, SEQ_LCD_PrnSongPos_Dis
	movwf	PRODL
	rlf	WREG, W
	swapf	WREG, W
	andlw	0x0f
	addlw	'A'
	call	MIOS_LCD_PrintChar
	movf	PRODL, W
	andlw	0x07
	addlw	'1'
	goto	MIOS_LCD_PrintChar


;; ==========================================================================
;; print the note (3 characters) (uses MIOS_PARAMETER1 and FSR1)
;; ==========================================================================
SEQ_LCD_PrnNote
	;; size-optimized note print routine

	;; print "---" if note number is 0
	andlw	0x7f
	bnz	SEQ_LCD_PrnNote_NotDis
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	goto	MIOS_LCD_PrintChar

SEQ_LCD_PrnNote_NotDis
	;; determine octave (PRODH) and note number within octave (PRODL)
	movwf	PRODL		; note number
	clrf	PRODH		; octave

SEQ_LCD_PrnNote_Loop
	movlw	12 - 1		; octave found?
	cpfsgt	PRODL, ACCESS
	rgoto SEQ_LCD_PrnNote_LoopBreak
	movlw	-12		; if not, decrement 12 from note number and increment octave
	addwf	PRODL, F
	incf	PRODH, F
	rgoto	SEQ_LCD_PrnNote_Loop

SEQ_LCD_PrnNote_LoopBreak

	;; get note name from table
	rlf	PRODL, W
	andlw	0xfe
	addlw	SEQ_LCD_PrnNote_Tab & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	SEQ_LCD_PrnNote_Tab >> 8
	addwfc	TBLPTRH, F

	;; get first character
	tblrd*+

	;; capital letter if octave > 2
	movlw	2 - 1
	cpfsgt	PRODH, ACCESS
	rgoto SEQ_LCD_PrnNote_NoCap
SEQ_LCD_PrnNote_Cap
	movlw	'A'-'a'
	addwf	TABLAT, F
	movlw	-2
	addwf	PRODH, F
	rgoto	SEQ_LCD_PrnNote_Cont
SEQ_LCD_PrnNote_NoCap
	movlw	2
	btfsc	PRODH, 0
	movlw 1
	movwf	PRODH
	;; 	rgoto	SEQ_LCD_PrnNote_Cont

SEQ_LCD_PrnNote_Cont
	
	;; print first char
	movf	TABLAT, W
	call	MIOS_LCD_PrintChar

	;; print second char
	tblrd*+
	movf	TABLAT, W
	call	MIOS_LCD_PrintChar

	;; print octave
	movf	PRODH, W
	goto	MIOS_LCD_PrintBCD1
	
SEQ_LCD_PrnNote_Tab
	db	"c-", "c#", "d-", "d#", "e-", "f-", "f#", "g-", "g#", "a-", "a#", "b-"


;; ==========================================================================
;; prints the length (4 characters)
;; ==========================================================================
SEQ_LCD_PrnLen
	movwf	TABLAT
	andlw	0xe0
	bnz	SEQ_LCD_PrnLen_Multi	; multi trigger

SEQ_LCD_PrnLen_Normal
	movlw	24
	cpfslt	TABLAT, ACCESS
	rgoto SEQ_LCD_PrnLen_NormalQuer
	movf	TABLAT, W
SEQ_LCD_PrnLen_NormalQuer
	mullw	4
	movlw	LOW(SEQ_LCD_PrnLen_TAB)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(SEQ_LCD_PrnLen_TAB)
	addwfc	PRODH, W
	movwf	TBLPTRH
	movlw	4
	goto	MIOS_LCD_PrintPreconfString

SEQ_LCD_PrnLen_Multi
	movf	TABLAT, W
	mullw	8
	movf	PRODH, W
	call	MIOS_LCD_PrintBCD1
	movlw	'x'
	call	MIOS_LCD_PrintChar
	movf	TABLAT, W
	andlw	0x1f
	goto	MIOS_LCD_PrintBCD2


SEQ_LCD_PrnLen_TAB
	db	"  4%"	; 0
	db	"  8%"	; 1
	db	" 13%"	; 2
	db	" 17%"	; 3
	db	" 21%"	; 4
	db	" 25%"	; 5
	db	" 29%"	; 6
	db	" 33%"	; 7
	db	" 38%"	; 8
	db	" 42%"	; 9
	db	" 46%"	; 10
	db	" 50%"	; 11
	db	" 54%"	; 12
	db	" 58%"	; 13
	db	" 52%"	; 14
	db	" 67%"	; 15
	db	" 71%"	; 16
	db	" 75%"	; 17
	db	" 79%"	; 18
	db	" 83%"	; 19
	db	" 88%"	; 20
	db	" 92%"	; 21
	db	" 96%"	; 22
	db	"100%"	; 23
	db	"Gld."	; 24


;; ==========================================================================
;; print an arp parameter (uses MIOS_PARAMETER1)
;; ==========================================================================
SEQ_LCD_PrnArpNote
	andlw	0xff		; print "---" if zero
	skpnz
	rgoto	SEQ_LCD_PrnNote
	movwf	MIOS_PARAMETER1

	;; print '*' if bit [7:4] of arp parameter < 2
	swapf	WREG, W
	andlw	0x0f
	addlw	-2
	bnc	SEQ_LCD_PrnArpNote_Multi

	;; print key number (1-4), located in bit #3-2
	rrf	MIOS_PARAMETER1, W
	rrf	WREG, W
	andlw	0x03
	addlw	1
	call	MIOS_LCD_PrintHex1

	;; print signed value, located in bit #6-4
	swapf	MIOS_PARAMETER1, W
SEQ_LCD_PrnArpNote_Multi_Cont
	addlw	-4
	andlw	0x07
	btfsc	WREG, 2
	iorlw 0x08
	rgoto	SEQ_LCD_Prn_PMDec1


SEQ_LCD_PrnArpNote_Multi
	movlw	'*'
	call	MIOS_LCD_PrintChar
	rrf	MIOS_PARAMETER1, W
	rrf	WREG, W
	rgoto	SEQ_LCD_PrnArpNote_Multi_Cont

;; ==========================================================================
;; print dec value -64..+63
;; USES: MIOS_PARAMETER1
;; ==========================================================================
SEQ_LCD_Prn_PMDec2
	movwf	MIOS_PARAMETER1
	movlw	'+'		; '+' or '-'?
	btfsc	MIOS_PARAMETER1, 7;
	movlw '-'
	call	MIOS_LCD_PrintChar

	movf	MIOS_PARAMETER1, W
	btfsc	WREG, 7			; negate value if negative to get absolute value
	negf	WREG, ACCESS
	goto	MIOS_LCD_PrintBCD2	; and print it

;; ==========================================================================
;; print dec value -8..+7
;; USES: MIOS_PARAMETER1
;; ==========================================================================
SEQ_LCD_Prn_PMDec1
	movwf	MIOS_PARAMETER1
	movlw	'+'		; '+' or '-'?
	btfsc	MIOS_PARAMETER1, 3;
	movlw '-'
	call	MIOS_LCD_PrintChar

	movf	MIOS_PARAMETER1, W
	btfsc	WREG, 3			; negate value if negative to get absolute value
	negf	WREG, ACCESS
	goto	MIOS_LCD_PrintHex1	; and print it

;; ==========================================================================
;;  print on/off
;;  IN:  value 0 or != 0 in WREG
;;  OUT: 3 characters
;;  USES: PRODL
;; ==========================================================================
SEQ_LCD_PrnOnOff
	movwf	PRODL
	movlw	'o'
	call	MIOS_LCD_PrintChar
	
	movf	PRODL, W
	movlw	'n'
	skpnz
	movlw	'f'
	call	MIOS_LCD_PrintChar
	
	movf	PRODL, W
	movlw	' '
	skpnz
	movlw	'f'
	goto	MIOS_LCD_PrintChar
	
	
;; --------------------------------------------------------------------------
;;  This function prints the MIDI port
;;  IN: WREG: midi port number
;;  OUT: 4 characters
;;  USES: BSR, FSR1, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LCD_PrnPort
	TABLE_ADDR_MUL_W SEQ_LCD_PrnPort_TABLE, 4

	;; print 4 characters
	movlw	4
	goto	MIOS_LCD_PrintPreconfString

SEQ_LCD_PrnPort_TABLE
	db	"Def.", "Int.", "IIC1", "IIC2", "IIC3", "IIC4", "Lbck", "AOUT"
	

;; --------------------------------------------------------------------------
;;  This functions prints x chars
;;  IN:  character in PRODL, number of chars in WREG
;;  OUT: x chars
;;  USES: PRODH
;; --------------------------------------------------------------------------
SEQ_LCD_PrintChars
	movwf	PRODH
SEQ_LCD_PrintCharsLoop
	movf	PRODL, W
	call	MIOS_LCD_PrintChar
	decfsz	PRODH, F
	rgoto	SEQ_LCD_PrintCharsLoop
	return

;; --------------------------------------------------------------------------
;;  This functions prints x spaces
;;  IN:  number of spaces in WREG
;;  OUT: x spaces
;;  USES: PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LCD_PrintSpaces
	movwf	PRODH
	movlw	' '
	movwf	PRODL
	rgoto	SEQ_LCD_PrintCharsLoop


;; --------------------------------------------------------------------------
;;  This functions prints the tap tempo message
;;  OUT: 2x20 chars at display offset 0x00 + 0 / 0x40 + 0
;;  USES: PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LCD_PrnTapTempo_STR
	STRING	13, 0x00 + 0, "* Tap Tempo: "
SEQ_LCD_PrnTapTempo
	TABLE_ADDR SEQ_LCD_PrnTapTempo_STR
	call	MIOS_LCD_PrintMessage

	call	SEQ_BPM_Get
	call	MIOS_LCD_PrintBCD3

	movlw	4
	rcall	SEQ_LCD_PrintSpaces

	movlw	4
	rcall	SEQ_LCD_PrintSpaces

	;; print <beat> * ">>>>"
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet

	clrf	TABLAT
SEQ_LCD_PrnTapTempoLoop
	SET_BSR	SEQ_BASE
	movf	SEQ_TAPTEMPO_BEAT_CTR, W, BANKED
	andlw	0x03
	addlw	1
	cpfslt	TABLAT
	rgoto	SEQ_LCD_PrnTapTempoLoop_GET
SEQ_LCD_PrnTapTempoLoop_LT
	movlw	'>'
	rgoto	SEQ_LCD_PrnTapTempoLoop_Cont
SEQ_LCD_PrnTapTempoLoop_GET
	movlw	' '
	;;	rgoto	SEQ_LCD_PrnTapTempoLoop_Cont
SEQ_LCD_PrnTapTempoLoop_Cont
	movwf	PRODL
	movlw	4
	rcall	SEQ_LCD_PrintChars

	incf	TABLAT, F
	BRA_IFCLR TABLAT, 2, ACCESS, SEQ_LCD_PrnTapTempoLoop

	return
	


;; ==========================================================================
;; this function prints the menu header
;; it contains the application name and the song position
;; ==========================================================================
SEQ_LCD_VERSION_STR_0 MBSEQ_VERSION_STR	; (defined in main.asm)
SEQ_LCD_PrintHeader
	TABLE_ADDR SEQ_LCD_VERSION_STR_0
	goto	MIOS_LCD_PrintString
