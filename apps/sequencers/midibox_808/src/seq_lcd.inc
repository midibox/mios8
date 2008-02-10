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
SEQ_LCD_SetMenuOffsets
	;; init offsets for 2 * 2x40 LCDs
	movlw	0x18
	movwf	MIOS_PARAMETER1
	movlw	0x58
	movwf	MIOS_PARAMETER2
	movlw	0x80
	movwf	MIOS_PARAMETER3
	movlw	0xc0
	goto	MIOS_LCD_YAddressSet

;; --------------------------------------------------------------------------
;;  This functions sets the Y offsets for a menu page
;;  IN:  -
;;  OUT: -
;; --------------------------------------------------------------------------
SEQ_LCD_SetMsgOffsets
	movlw	0x00
	movwf	MIOS_PARAMETER1
	movlw	0x40
	movwf	MIOS_PARAMETER2
	movlw	0x80
	movwf	MIOS_PARAMETER3
	movlw	0xc0
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
;;     SEQ_SELECTED_GROUP: selected group
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

	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	movf	TMP1, W
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
;; print selected layers
;; IN: selected tracks in SEQ_SELECTED_LAYERS
;; USES: BSR
;; ==========================================================================
SEQ_LCD_PrnSelectedLayers
	SET_BSR	SEQ_BASE
	movlw	'A'			; selected layers
	btfsc	SEQ_SELECTED_LAYERS, 0, BANKED
	movlw '*'
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	movlw	'B'
	btfsc	SEQ_SELECTED_LAYERS, 1, BANKED
	movlw '*'
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	movlw	'C'
	btfsc	SEQ_SELECTED_LAYERS, 2, BANKED
	movlw '*'
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
;;  This functions prints a marker in the EDIT page
;;  IN: various positions and edit modes
;;  OUT: 1 character
;; --------------------------------------------------------------------------
SEQ_LCD_PrnMarker
	;; print marker to notify current step
	SET_BSR	SEQ_BASE

	;; marker handling is different in utils page
	movf	CS_MENU_POS, W
	xorlw	CS_MENU_PAGE_UTILS
	bnz	SEQ_LCD_PrnMarker_Normal

	movf	CS_IN_MENU_MSG, W
	xorlw	CS_M_UTILS_MSG_COPY
	bz	SEQ_LCD_PrnMarker_Copy
	xorlw	CS_M_UTILS_MSG_PASTE ^ CS_M_UTILS_MSG_COPY
	bz	SEQ_LCD_PrnMarker_Paste
	xorlw	CS_M_UTILS_MSG_SCROLL ^ CS_M_UTILS_MSG_PASTE
	bz	SEQ_LCD_PrnMarker_Scroll
	rgoto	SEQ_LCD_PrnMarker_Normal



SEQ_LCD_PrnMarker_Copy
	incf	SEQ_EVNTS, W, BANKED
	cpfseq	CS_COPYPASTE_BEGIN, ACCESS
	rgoto SEQ_LCD_PrnMarker_Copy_NoM1
SEQ_LCD_PrnMarker_Copy_M1
	movlw	'>'
	goto	MIOS_LCD_PrintChar

SEQ_LCD_PrnMarker_Copy_NoM1
	movf	SEQ_EVNTS, W, BANKED
	cpfseq	CS_COPYPASTE_END, ACCESS
	rgoto SEQ_LCD_PrnMarker_Copy_NoM2
	movlw	'<'
	goto	MIOS_LCD_PrintChar
SEQ_LCD_PrnMarker_Copy_NoM2
	rgoto	SEQ_LCD_PrnMarker_Space


SEQ_LCD_PrnMarker_Paste
	incf	SEQ_EVNTS, W, BANKED
	cpfseq	SEQ_CURRENT_STEP, BANKED
	rgoto SEQ_LCD_PrnMarker_Paste_NoM1
SEQ_LCD_PrnMarker_Paste_M1
	movlw	'>'
	goto	MIOS_LCD_PrintChar

SEQ_LCD_PrnMarker_Paste_NoM1
	movf	CS_COPYPASTE_BEGIN, W	; determine range
	subwf	CS_COPYPASTE_END, W
	addwf	SEQ_CURRENT_STEP, W, BANKED
	movwf	PRODL

	movf	SEQ_EVNTS, W, BANKED
	cpfseq	PRODL, ACCESS
	rgoto SEQ_LCD_PrnMarker_Paste_NoM2
	movlw	'<'
	goto	MIOS_LCD_PrintChar
SEQ_LCD_PrnMarker_Paste_NoM2
	rgoto	SEQ_LCD_PrnMarker_Space


SEQ_LCD_PrnMarker_Scroll
	incf	SEQ_EVNTS, W, BANKED
	cpfseq	SEQ_CURRENT_STEP, BANKED
	rgoto SEQ_LCD_PrnMarker_Scroll_NoM1
SEQ_LCD_PrnMarker_Scroll_M1
	movlw	'>'
	goto	MIOS_LCD_PrintChar

SEQ_LCD_PrnMarker_Scroll_NoM1
	call	SEQ_FSR_TrkLen
	movf	SEQ_EVNTS, W, BANKED
	cpfseq	INDF1, ACCESS
	rgoto SEQ_LCD_PrnMarker_Scroll_NoM2
	movlw	'<'
	goto	MIOS_LCD_PrintChar
SEQ_LCD_PrnMarker_Scroll_NoM2
	rgoto	SEQ_LCD_PrnMarker_Space



SEQ_LCD_PrnMarker_Normal
	incf	SEQ_EVNTS, W, BANKED
	cpfseq	SEQ_CURRENT_STEP, BANKED
	rgoto SEQ_LCD_PrnMarker_NoM1
SEQ_LCD_PrnMarker_M1
	movlw	'>'
	goto	MIOS_LCD_PrintChar

SEQ_LCD_PrnMarker_NoM1
	movf	SEQ_EVNTS, W, BANKED
	cpfseq	SEQ_CURRENT_STEP, BANKED
	rgoto SEQ_LCD_PrnMarker_NoM2
	movlw	'<'
	goto	MIOS_LCD_PrintChar

SEQ_LCD_PrnMarker_NoM2
SEQ_LCD_PrnMarker_Space
	movlw	1
	rgoto	SEQ_LCD_PrintSpaces


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

;; ==========================================================================
;; this function prints the menu header
;; it contains the application name and the song position
;; ==========================================================================
SEQ_LCD_VERSION_STR_0 MBSEQ_VERSION_STR	; (defined in main.asm)
SEQ_LCD_PrintHeader
	TABLE_ADDR SEQ_LCD_VERSION_STR_0
	goto	MIOS_LCD_PrintString

;; --------------------------------------------------------------------------
;;  this function clears the "previous length" and prepares the H/V bars
;;  for the selected PType
;;  IN: -
;;  OUT: -
;;  USES: BSR
;; --------------------------------------------------------------------------
SEQ_LCD_InitPType
	;; clear "previous length"
	SET_BSR	SEQ_BASE
	clrf	SEQ_LCD_PREVIOUS_EVNTL, BANKED
	return

;; --------------------------------------------------------------------------
;;  this function print the PType name
;;  IN: selected track in EVNTT
;;  OUT: 6 characters
;;  USES: BSR, FSR1, MIOS_PARAMETER[12], PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LCD_PrnPTypeName
	SET_BSR	SEQ_BASE

	;; print "Arp." if we display an arpeggio instead of note
	call	SEQ_FSR_TrkMode
	movf	INDF1, W
	andlw	0x07
	xorlw	SEQ_TRKMODE_ARPEGGIATOR
	bz	SEQ_LCD_PrnPTypeName_Arp

	;; fetch SEQ_EVNT[012L]
	call	CS_M_HLP_GetEvent

	;; print variable type xtra-large!
	call	SEQ_FSR_TrkEvnt
	movff	INDF1, MIOS_PARAMETER1
	call	CS_M_HLP_GetSelectedLayer
	call	SEQ_LAYER_PrnVType_XL
	rgoto	SEQ_LCD_PrnPTypeName_End

SEQ_LCD_PrnPTypeName_Arp_STR
	db	"Arp."
SEQ_LCD_PrnPTypeName_Arp
	TABLE_ADDR SEQ_LCD_PrnPTypeName_Arp_STR
	movlw	4
	call	MIOS_LCD_PrintPreconfString
	movlw	2
	rgoto	SEQ_LCD_PrintSpaces
	;; 	rgoto	SEQ_LCD_PrnPTypeName_Cont

SEQ_LCD_PrnPTypeName_End
	return

;; --------------------------------------------------------------------------
;;  this function prints the PType value
;;  IN: current track in SEQ_EVNTT, current step in SEQ_EVNTS
;;  OUT: 5 characters
;;  USES: MIOS_PARAMETER1, TMP5, BSR
;; --------------------------------------------------------------------------
SEQ_LCD_PrnPTypeVal
	;; fetch SEQ_EVNT[012L]
	call	CS_M_HLP_GetEvent

	;; get layer type and store it in TMP5
	call	CS_M_HLP_GetLayerVType
	movwf	TMP5

	xorlw	SEQ_LAYER_V_LEN				; branch if length parameter
	bz	SEQ_LCD_PrnPTypeVal_L

	movlw	CS_MENU_CHARSET_VBARS	; init charset for vertical bars
	call	CS_SPECIAL_CHARS_Init
	SET_BSR	SEQ_BASE

	
	movf	TMP5, W
	xorlw	SEQ_LAYER_V_CHORD1		; branch if chord #1
	skpnz
	rgoto	SEQ_LCD_PrnPTypeVal_Crd1
	xorlw	SEQ_LAYER_V_CHORD1_VEL ^ SEQ_LAYER_V_CHORD1	; branch if chord #1 vel.
	skpnz
	rgoto	SEQ_LCD_PrnPTypeVal_Crd1
	xorlw	SEQ_LAYER_V_CHORD2 ^ SEQ_LAYER_V_CHORD1_VEL	; branch if chord #2
	skpnz
	rgoto	SEQ_LCD_PrnPTypeVal_Crd2
	xorlw	SEQ_LAYER_V_CHORD2_VEL ^ SEQ_LAYER_V_CHORD2	; branch if chord #2 vel.
	skpnz
	rgoto	SEQ_LCD_PrnPTypeVal_Crd2

	;; print "---- " if event is zero
	movf	SEQ_EVNT0, W, BANKED
	bnz	PrnPTypeVal_NotMuted
PrnPTypeVal_Muted
	movlw	'-'
	movwf	PRODL
	movlw	4
	rcall	SEQ_LCD_PrintChars
	rgoto	SEQ_LCD_PrnPTypeVal_End
PrnPTypeVal_NotMuted

	;; should CC value be print?
	movf	TMP5, W
	xorlw	SEQ_LAYER_V_CC
	bz	SEQ_LCD_PrnPTypeVal_CC

	;; print Note
SEQ_LCD_PrnPTypeVal_N
	;; print dot's if note==0 or velocity==0
	movf	SEQ_EVNT1, W, BANKED
	bz	SEQ_LCD_PrnPTypeVal_Dots
	movf	SEQ_EVNT2, W, BANKED
	bz	SEQ_LCD_PrnPTypeVal_Dots

	;; print arp value if we display an arpeggio instead of note
	call	SEQ_FSR_TrkMode
	movf	INDF1, W
	andlw	0x07
	xorlw	SEQ_TRKMODE_ARPEGGIATOR
	bz	SEQ_LCD_PrnPTypeVal_N_Arp
SEQ_LCD_PrnPTypeVal_N_NoArp
	;; print note, a vertical bar (for velocity) and a space
	movf	SEQ_EVNT1, W, BANKED
	rcall	SEQ_LCD_PrnNote
SEQ_LCD_PrnPTypeVal_N_ArpCont
	;; print vertical bar if appr. font is selected
	movlw	CS_MENU_CHARSET_VBARS
	cpfseq	CS_MENU_CHARSET, ACCESS
	rgoto SEQ_LCD_PrnPTypeVal_N_NoVBar
SEQ_LCD_PrnPTypeVal_N_VBar
	movff	SEQ_EVNT2, WREG
	rcall	SEQ_LCD_PrnVBar
	rgoto	SEQ_LCD_PrnPTypeVal_End
SEQ_LCD_PrnPTypeVal_N_NoVBar
	movlw	1
	rcall	SEQ_LCD_PrintSpaces
	rgoto	SEQ_LCD_PrnPTypeVal_End

SEQ_LCD_PrnPTypeVal_N_Arp
	;; print arp number, vertical bar (for velocity) and a space
	movf	SEQ_EVNT1, W, BANKED
	rcall	SEQ_LCD_PrnArpNote
	rgoto	SEQ_LCD_PrnPTypeVal_N_ArpCont


SEQ_LCD_PrnPTypeVal_CC		; CC Values
	;; print "...." if CC#0
	movf	SEQ_EVNT1, W, BANKED
	bz	SEQ_LCD_PrnPTypeVal_Dots

	;; print second event byte as BCD, a V-Bar and a space
	movf	SEQ_EVNT2, W, BANKED
	call	MIOS_LCD_PrintBCD3
	;; print vertical bar if appr. font is selected
	movlw	CS_MENU_CHARSET_VBARS
	cpfseq	CS_MENU_CHARSET, ACCESS
	rgoto SEQ_LCD_PrnPTypeVal_CC_NVBar
SEQ_LCD_PrnPTypeVal_CC_VBar
	movff	SEQ_EVNT2, WREG
	rcall	SEQ_LCD_PrnVBar
	rgoto	SEQ_LCD_PrnPTypeVal_End
SEQ_LCD_PrnPTypeVal_CC_NVBar
	movlw	1
	rcall	SEQ_LCD_PrintSpaces
	rgoto	SEQ_LCD_PrnPTypeVal_End

	;; dots print when CC#0, or velocity=0, or note=0
SEQ_LCD_PrnPTypeVal_Dots
	movlw	'.'
	movwf	PRODL
	movlw	4
	rcall	SEQ_LCD_PrintChars
	rgoto	SEQ_LCD_PrnPTypeVal_End

SEQ_LCD_PrnPTypeVal_L		; Length
	movlw	CS_MENU_CHARSET_HBARS	; init charset for horizontal bars
	call	CS_SPECIAL_CHARS_Init
	SET_BSR	SEQ_BASE

	;; multi note trigger?
	movf	SEQ_EVNTL, W, BANKED
	mullw	8
	movf	PRODH, W
	bnz	SEQ_LCD_PrnPTypeVal_L_Multi
	BRA_IFSET SEQ_LCD_PREVIOUS_EVNTL, 7, BANKED, SEQ_LCD_PrnPTypeVal_L_Multi

	;; muted step?
	movf	SEQ_EVNT0, W, BANKED
	bnz	SEQ_LCD_PrnPTypeVal_L_Bar
SEQ_LCD_PrnPTypeVal_L_ChkSpc
	;; if previous gatelength > 24, print the bar, else spaces
	movlw	24+1
	cpfslt	SEQ_LCD_PREVIOUS_EVNTL, BANKED
	rgoto SEQ_LCD_PrnPTypeVal_L_Bar

SEQ_LCD_PrnPTypeVal_L_Spc
	;; muted and no GL "overrun" on previous step
	clrf	SEQ_LCD_PREVIOUS_EVNTL, BANKED; (clear gatelength)
	movlw	4
	rcall	SEQ_LCD_PrintSpaces
	rgoto	SEQ_LCD_PrnPTypeVal_End_NoMarker

SEQ_LCD_PrnPTypeVal_L_Multi
	;; if previous gatelength > 24, print '!'
	BRA_IFSET SEQ_LCD_PREVIOUS_EVNTL, 7, BANKED, SEQ_LCD_PrnPTypeVal_L_Multi_Con

	;; if event == 0, print spaces (muted step)
	movf	SEQ_EVNT0, W, BANKED
	bz	SEQ_LCD_PrnPTypeVal_L_Spc

SEQ_LCD_PrnPTypeVal_L_Multi_Val
	;; print number of events
	incf	PRODH, W
	call	MIOS_LCD_PrintBCD1

	;; calculate total length of all events
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNTL, W, BANKED
	andlw	0x1f
	mulwf	PRODH, ACCESS
	movff	PRODL, SEQ_LCD_PREVIOUS_EVNTL_CTR

	;; if length >= 24, set bit 7 of PREVIOUS_EVNTL to notify that the next step has to be activated
	movlw	24
	cpfslt	SEQ_LCD_PREVIOUS_EVNTL_CTR, BANKED
	bsf	SEQ_LCD_PREVIOUS_EVNTL, 7, BANKED

	;; this step takes 24 ticks
	movlw	-24
	addwf	SEQ_LCD_PREVIOUS_EVNTL_CTR, F, BANKED

	;; print delay between events
	movlw	'x'
	call	MIOS_LCD_PrintChar
	movff	SEQ_EVNTL, WREG
	andlw	0x1f
	call	MIOS_LCD_PrintBCD2
	rgoto	SEQ_LCD_PrnPTypeVal_End_NoMarker

SEQ_LCD_PrnPTypeVal_L_Multi_Con
	;; continued multi note event
	;; if length still >= 24, next step will also be taken
	bcf	SEQ_LCD_PREVIOUS_EVNTL, 7, BANKED
	movlw	-24
	addwf	SEQ_LCD_PREVIOUS_EVNTL_CTR, F, BANKED
	btfss	SEQ_LCD_PREVIOUS_EVNTL_CTR, 7, BANKED
	bsf	SEQ_LCD_PREVIOUS_EVNTL, 7, BANKED

	movf	SEQ_EVNT0, F, BANKED
	movlw	'!'
	skpz
	movlw	'>'
	movwf	PRODL
	movlw	4
	call	SEQ_LCD_PrintChars
	rgoto	SEQ_LCD_PrnPTypeVal_End_NoMarker
	
SEQ_LCD_PrnPTypeVal_L_Bar
	;; print gatelength in form of a nice horizontal bar
	;; if current gatelength > 24, print full bar
	movlw	24
	cpfsgt	SEQ_EVNTL, BANKED
	rgoto SEQ_LCD_PrnPTypeVal_L_Bar_NF
SEQ_LCD_PrnPTypeVal_L_Bar_F
	movlw	25					; memorize saturated value for next step
	movwf	SEQ_LCD_PREVIOUS_EVNTL, BANKED
	movlw	0x7f
	rgoto	SEQ_LCD_PrnPTypeVal_L_Bar_F_C
SEQ_LCD_PrnPTypeVal_L_Bar_NF
	movff	SEQ_EVNTL, SEQ_LCD_PREVIOUS_EVNTL	; memorize for next step
	rlf	SEQ_EVNTL, W, BANKED
	rlf	WREG, W
	andlw	0x7c
SEQ_LCD_PrnPTypeVal_L_Bar_F_C
	rgoto	SEQ_LCD_PrnHBar

	;; chords
SEQ_LCD_PrnPTypeVal_Crd1
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNT0, W, BANKED
	skpnz
	rgoto	PrnPTypeVal_Muted

	movlw	'S'
	call	MIOS_LCD_PrintChar
	movff	SEQ_EVNT_CHORD_INDEX, WREG
	andlw	0x1f
	addlw	1
	call	MIOS_LCD_PrintBCD2
	movff	SEQ_EVNT2, WREG
	rcall	SEQ_LCD_PrnVBar
	rgoto	SEQ_LCD_PrnPTypeVal_End

SEQ_LCD_PrnPTypeVal_Crd1_Dis
	movlw	'-'
	call	MIOS_LCD_PrintChar
	rgoto	SEQ_LCD_PrnPTypeVal_End



SEQ_LCD_PrnPTypeVal_Crd2
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNT0, W, BANKED
	skpnz
	rgoto	PrnPTypeVal_Muted

	movf	SEQ_EVNT_CHORD_INDEX, W, BANKED
	andlw	0x0f
	call	MIOS_LCD_PrintHex1

	movlw	'/'
	call	MIOS_LCD_PrintChar

	SET_BSR	SEQ_BASE
	swapf	SEQ_EVNT_CHORD_INDEX, W, BANKED
	andlw	0x07
	call	MIOS_LCD_PrintBCD1

	movff	SEQ_EVNT2, WREG
	rcall	SEQ_LCD_PrnVBar

	;; 	rgoto	SEQ_LCD_PrnPTypeVal_End

SEQ_LCD_PrnPTypeVal_End
	rgoto	SEQ_LCD_PrnMarker

SEQ_LCD_PrnPTypeVal_End_NoMarker
	movlw	1
	goto	SEQ_LCD_PrintSpaces


;; --------------------------------------------------------------------------
;;  this function prints the upper info line
;;  IN: current track in SEQ_EVNTT, current step in SEQ_EVNTS
;;  OUT: variable number of characters (!)
;;  USES: MIOS_PARAMETER1, BSR
;; --------------------------------------------------------------------------
SEQ_LCD_PrnMsgItemU
	SET_BSR	SEQ_BASE

	;; with MIDIbox SEQ V3, we don't display the same note type for each step
	;; anymore, ("Note Note Note Note ...") but we display different useful
	;; informations instead. The SEQ_EVNTS (step number) is used as counter
	;; here - with each invokation of this function, we print a small part
	;; of the info line

	;; layout of info line:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; P1T1  A:Note   Chn 1/Def. B:Gate  S 1-16Step xx/xx:C-3  Vel:xxx Len:xxx% GSAGRGV
	;; P1T1  A:CC# 16 Chn 1/Def. B:Gate  S 1-16Step xx/xx:xxx          Len:xxx% GSAGRGV 
	;; 0000001111111112222233333344444445555555666667777778888899999999AAAAAAAAABBBBBBB

	;; thanks to the huge memory of PIC18F4620, we can program this on an easy readable way :)

	movf	SEQ_EVNTS, W, BANKED
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_LCD_PrnMsgItemU_0
	rgoto	SEQ_LCD_PrnMsgItemU_1
	rgoto	SEQ_LCD_PrnMsgItemU_2
	rgoto	SEQ_LCD_PrnMsgItemU_3
	rgoto	SEQ_LCD_PrnMsgItemU_4
	rgoto	SEQ_LCD_PrnMsgItemU_5
	rgoto	SEQ_LCD_PrnMsgItemU_6
	rgoto	SEQ_LCD_PrnMsgItemU_7
	rgoto	SEQ_LCD_PrnMsgItemU_8
	rgoto	SEQ_LCD_PrnMsgItemU_9
	rgoto	SEQ_LCD_PrnMsgItemU_A
	rgoto	SEQ_LCD_PrnMsgItemU_B
	rgoto	SEQ_LCD_PrnMsgItemU_C
	rgoto	SEQ_LCD_PrnMsgItemU_D
	rgoto	SEQ_LCD_PrnMsgItemU_E
	rgoto	SEQ_LCD_PrnMsgItemU_F

SEQ_LCD_PrnMsgItemU_0	
	movlw	0x00 + 0	; track number
	call	MIOS_LCD_CursorSet

	;; special message if we are in utility menu!
	movf	CS_MENU_POS, W
	xorlw	CS_MENU_PAGE_UTILS
	bnz	SEQ_LCD_PrnMsgItemU_0_NoUtil
SEQ_LCD_PrnMsgItemU_0_Util
	call	CS_M_UTILS_PrnEditMsg
	
SEQ_LCD_PrnMsgItemU_0_NoUtil
	goto	CS_M_HLP_PrnGxTy_NoCursor

SEQ_LCD_PrnMsgItemU_1
	;; (don't print something here if we are in utility menu)
	movf	CS_MENU_POS, W
	xorlw	CS_MENU_PAGE_UTILS
	skpnz
	return

	movlw	0x00 + 6		; parameter type
	call	MIOS_LCD_CursorSet
	call	CS_M_HLP_GetSelectedLayer
	addlw	'A'
	call	MIOS_LCD_PrintChar
	movlw	':'
	call	MIOS_LCD_PrintChar
	rcall	SEQ_LCD_PrnPTypeName	; (6 characters)
	movlw	1
	rgoto	SEQ_LCD_PrintSpaces

SEQ_LCD_PrnMsgItemU_2
	movlw	0x00 + 15		; channel
	call	MIOS_LCD_CursorSet
	movlw	'C'
	call	MIOS_LCD_PrintChar
	movlw	'h'
	call	MIOS_LCD_PrintChar
	movlw	'n'
	call	MIOS_LCD_PrintChar
	call	SEQ_FSR_TrkChn
	movf	INDF1, W
	andlw	0x0f
	addlw	1
	goto	MIOS_LCD_PrintBCD2

SEQ_LCD_PrnMsgItemU_3
	movlw	0x00 + 20		; port
	call	MIOS_LCD_CursorSet
	movlw	'/'
	call	MIOS_LCD_PrintChar
	call	SEQ_FSR_TrkChn
	swapf	INDF1, W
	andlw	0x07
	rcall	SEQ_LCD_PrnPort
	movlw	1
	rgoto	SEQ_LCD_PrintSpaces

SEQ_LCD_PrnMsgItemU_4
	movlw	0x00 + 26		; button function
	call	MIOS_LCD_CursorSet
	call	CS_M_HLP_GetSelectedTrgLayer
	addlw	'A'
	call	MIOS_LCD_PrintChar
	movlw	':'
	call	MIOS_LCD_PrintChar
	call	CS_M_HLP_GetSelectedTrgLayer
	call	SEQ_TRG_PrnAssignedType	; 5 characters
	movlw	1
	rgoto	SEQ_LCD_PrintSpaces

SEQ_LCD_PrnMsgItemU_5_STR_0 STRING 6, 0x00 + 34, "S 1-16"
SEQ_LCD_PrnMsgItemU_5_STR_1 STRING 6, 0x00 + 34, "S17-32"
SEQ_LCD_PrnMsgItemU_5
	TABLE_ADDR SEQ_LCD_PrnMsgItemU_5_STR_0
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_SELECTED_STEPS, 0, BANKED, SEQ_LCD_PrnMsgItemU_5_Stp0
SEQ_LCD_PrnMsgItemU_5_Stp1
	TABLE_ADDR SEQ_LCD_PrnMsgItemU_5_STR_1
SEQ_LCD_PrnMsgItemU_5_Stp0
	goto	MIOS_LCD_PrintString

SEQ_LCD_PrnMsgItemU_6_STR STRING 5, 0x80 + 0, "Step "
SEQ_LCD_PrnMsgItemU_6
	TABLE_ADDR SEQ_LCD_PrnMsgItemU_6_STR
	goto	MIOS_LCD_PrintString

SEQ_LCD_PrnMsgItemU_7
	movlw	0x80 + 5
	call	MIOS_LCD_CursorSet
	SET_BSR	SEQ_BASE
	incf	SEQ_CURRENT_STEP, W, BANKED
	call	MIOS_LCD_PrintBCD2
	movlw	'/'
	call	MIOS_LCD_PrintChar
	call	SEQ_FSR_TrkLen
	incf	INDF1, W
	call	MIOS_LCD_PrintBCD2
	movlw	':'
	goto	MIOS_LCD_PrintChar

SEQ_LCD_PrnMsgItemU_8
	movlw	0x80 + 11
	call	MIOS_LCD_CursorSet

	;; fetch SEQ_EVNT[012L]
	movff	SEQ_CURRENT_STEP, SEQ_EVNTS
	call	CS_M_HLP_GetEvent

	;; chord2? in this case we print a special string
	call	CS_M_HLP_GetLayerVType
	xorlw	SEQ_LAYER_V_CHORD2
	bz	SEQ_LCD_PrnMsgItemU_8_Crd2
	xorlw	SEQ_LAYER_V_CHORD2_VEL ^ SEQ_LAYER_V_CHORD2
	bz	SEQ_LCD_PrnMsgItemU_8_Crd2
	
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0xb0
	bz	SEQ_LCD_PrnMsgItemU_8_CC
SEQ_LCD_PrnMsgItemU_8_Note
	rgoto	SEQ_LCD_PrnPTypeVal_N

SEQ_LCD_PrnMsgItemU_8_Crd2
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNT_CHORD_INDEX, W, BANKED
	goto	SEQ_CHORD_PrintName

SEQ_LCD_PrnMsgItemU_8_CC
	rgoto	SEQ_LCD_PrnPTypeVal_CC

	movf	SEQ_EVNT2, W, BANKED
	call	MIOS_LCD_PrintBCD3
	;; print vertical bar if appr. font is selected
	movlw	CS_MENU_CHARSET_VBARS
	cpfseq	CS_MENU_CHARSET, ACCESS
	rgoto SEQ_LCD_PrnMsgItemU_8_CC_NVBars

	movlw	2		; one space spaces
	rgoto	SEQ_LCD_PrintSpaces

SEQ_LCD_PrnMsgItemU_8_CC_NVBars
	movlw	2		; two spaces
	rgoto	SEQ_LCD_PrintSpaces

SEQ_LCD_PrnMsgItemU_9
	movlw	0x80 + 16
	call	MIOS_LCD_CursorSet

	;; fetch SEQ_EVNT[012L]
	movff	SEQ_CURRENT_STEP, SEQ_EVNTS
	call	CS_M_HLP_GetEvent

	;; chord2? in this case we print a special string
	call	CS_M_HLP_GetLayerVType
	xorlw	SEQ_LAYER_V_CHORD2
	bz	SEQ_LCD_PrnMsgItemU_9_Crd2
	xorlw	SEQ_LAYER_V_CHORD2_VEL ^ SEQ_LAYER_V_CHORD2
	bz	SEQ_LCD_PrnMsgItemU_9_Crd2

	SET_BSR	SEQ_BASE
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0xb0
	bz	SEQ_LCD_PrnMsgItemU_9_CC

SEQ_LCD_PrnMsgItemU_9_Note
	movlw	'V'
	call	MIOS_LCD_PrintChar
	movlw	'e'
	call	MIOS_LCD_PrintChar
	movlw	'l'
	call	MIOS_LCD_PrintChar
	movlw	':'
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNT2, W, BANKED
	call	MIOS_LCD_PrintBCD3
	movlw	1			; one space
	rgoto	SEQ_LCD_PrintSpaces

SEQ_LCD_PrnMsgItemU_9_Crd2
	movlw	0x80 + 19	; (new cursor pos required, we already print 8 characters)
	call	MIOS_LCD_CursorSet
	movlw	1			; one space
	rcall	SEQ_LCD_PrintSpaces
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNT2, W, BANKED
	call	MIOS_LCD_PrintBCD3
	movlw	1			; one space
	rgoto	SEQ_LCD_PrintSpaces
	

SEQ_LCD_PrnMsgItemU_9_CC
	movlw	8			; 8 spaces
	rgoto	SEQ_LCD_PrintSpaces

	
SEQ_LCD_PrnMsgItemU_A
	movlw	0x80 + 24
	call	MIOS_LCD_CursorSet

	;; fetch SEQ_EVNT[012L]
	movff	SEQ_CURRENT_STEP, SEQ_EVNTS
	call	CS_M_HLP_GetEvent

	movlw	'L'
	call	MIOS_LCD_PrintChar
	movlw	'e'
	call	MIOS_LCD_PrintChar
	movlw	'n'
	call	MIOS_LCD_PrintChar
	movlw	':'
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNTL, W, BANKED
	rcall	SEQ_LCD_PrnLen
	movlw	1			; one space
	rgoto	SEQ_LCD_PrintSpaces

SEQ_LCD_PrnMsgItemU_B
	movlw	0x80 + 33
	call	MIOS_LCD_CursorSet

	;; gate
	movlw	'G'
	movwf	TMP1
	call	SEQ_FSR_TrkAssgn0
	movf	INDF1, W
	rcall	SEQ_LCD_PrnMsgItemU_B_Hlp

	;; skip
	movlw	'S'
	movwf	TMP1
	call	SEQ_FSR_TrkAssgn0
	rrf	INDF1, W
	rrf	WREG, W
	rcall	SEQ_LCD_PrnMsgItemU_B_Hlp

	;; accent
	movlw	'A'
	movwf	TMP1
	call	SEQ_FSR_TrkAssgn0
	swapf	INDF1, W
	rcall	SEQ_LCD_PrnMsgItemU_B_Hlp

	;; glide
	movlw	'G'
	movwf	TMP1
	call	SEQ_FSR_TrkAssgn0
	swapf	INDF1, W
	rrf	WREG, W
	rrf	WREG, W
	rcall	SEQ_LCD_PrnMsgItemU_B_Hlp

	;; roll
	movlw	'R'
	movwf	TMP1
	call	SEQ_FSR_TrkAssgn1
	movf	INDF1, W
	rcall	SEQ_LCD_PrnMsgItemU_B_Hlp

	;; random gate
	movlw	'G'
	movwf	TMP1
	call	SEQ_FSR_TrkAssgn1
	rrf	INDF1, W
	rrf	WREG, W
	rcall	SEQ_LCD_PrnMsgItemU_B_Hlp

	;; random value
	movlw	'V'
	movwf	TMP1
	call	SEQ_FSR_TrkAssgn1
	swapf	INDF1, W
	rcall	SEQ_LCD_PrnMsgItemU_B_Hlp

	return


SEQ_LCD_PrnMsgItemU_B_Hlp
	SET_BSR	SEQ_BASE
	andlw	0x03
	bz	SEQ_LCD_PrnMsgItemU_B_Hlp_0	; trigger not assigned
	addlw	-1
	movwf	TMP2		; store layer assignment in TMP2

	;; determine pointer to step control register
	call	SEQ_FSR_TrkTrgA_0
	rlf	TMP2, W
	rlf	WREG, W
	andlw	0x0c
	addwf	FSR1L, F

	rlf	SEQ_CURRENT_STEP, W, BANKED
	swapf	WREG, W
	andlw	0x03
	addwf	FSR1L, F

	;; lower character if step is not triggered
	movf	SEQ_CURRENT_STEP, W, BANKED
	andlw	0x07
	call	MIOS_HLP_GetBitORMask
	andwf	INDF1, W
	bz	SEQ_LCD_PrnMsgItemU_B_Hlp_L0
SEQ_LCD_PrnMsgItemU_B_Hlp_L1
	movf	TMP1, W		; step is triggered
	goto	MIOS_LCD_PrintChar
	
SEQ_LCD_PrnMsgItemU_B_Hlp_L0
	movf	TMP1, W		; step is not triggered
	addlw	'a'-'A'
	goto	MIOS_LCD_PrintChar

SEQ_LCD_PrnMsgItemU_B_Hlp_0	; not assigned
	movlw	'-'
	goto	MIOS_LCD_PrintChar
	

	;; dummies
SEQ_LCD_PrnMsgItemU_C
SEQ_LCD_PrnMsgItemU_D
SEQ_LCD_PrnMsgItemU_E
SEQ_LCD_PrnMsgItemU_F
	return

;; --------------------------------------------------------------------------
;;  this function prints a PType item at the correct position depending on SEQ_EVNTS
;;  (lower line)
;;  IN: current track in SEQ_EVNTT, current step in SEQ_EVNTS
;;  OUT: 5 characters
;;  USES: MIOS_PARAMETER1, BSR
;; --------------------------------------------------------------------------
SEQ_LCD_PrnMsgItemL
	;; set cursor depending on SEQ_EVNTS
	rcall	SEQ_LCD_PrnMsgItem_Hlp
	addlw	0x40
	call	MIOS_LCD_CursorSet

	;; ignore if flash flag set
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, SEQ_LCD_PrnMsgItemL_Normal

	;; if we are in utils page, and copy/paste is selected: flash selected range
	movf	CS_MENU_POS, W
	xorlw	CS_MENU_PAGE_UTILS
	bnz	SEQ_LCD_PrnMsgItemL_Normal

	movf	CS_IN_MENU_MSG, W
	xorlw	CS_M_UTILS_MSG_COPY
	bz	SEQ_LCD_PrnMsgItemL_FlCopy
	xorlw	CS_M_UTILS_MSG_PASTE ^ CS_M_UTILS_MSG_COPY
	bz	SEQ_LCD_PrnMsgItemL_FlPaste
	xorlw	CS_M_UTILS_MSG_SCROLL ^ CS_M_UTILS_MSG_PASTE
	bz	SEQ_LCD_PrnMsgItemL_FlScrl
	rgoto	SEQ_LCD_PrnMsgItemL_Normal

SEQ_LCD_PrnMsgItemL_FlCopy
	;; check if SEQ_EVNTS is within the range
	SET_BSR	SEQ_BASE
	movf	CS_COPYPASTE_BEGIN, W
	bz	SEQ_LCD_PrnMsgItemL_FlCopyEChk
	decf	CS_COPYPASTE_BEGIN, W
	cpfsgt	SEQ_EVNTS, BANKED
	rgoto SEQ_LCD_PrnMsgItemL_Normal

SEQ_LCD_PrnMsgItemL_FlCopyEChk
	incf	CS_COPYPASTE_END, W
	cpfslt	SEQ_EVNTS, BANKED
	rgoto SEQ_LCD_PrnMsgItemL_Normal
	rgoto	SEQ_LCD_PrnMsgItemL_Flash

SEQ_LCD_PrnMsgItemL_FlPaste
	;; check if SEQ_EVNTS is within the range
	SET_BSR	SEQ_BASE
	movf	SEQ_CURRENT_STEP, W, BANKED
	bz	SEQ_LCD_PrnMsgItemL_FlPasteEChk
	decf	SEQ_CURRENT_STEP, W, BANKED
	cpfsgt	SEQ_EVNTS, BANKED
	rgoto SEQ_LCD_PrnMsgItemL_Normal

SEQ_LCD_PrnMsgItemL_FlPasteEChk
	movf	CS_COPYPASTE_BEGIN, W	; determine range
	subwf	CS_COPYPASTE_END, W
	addwf	SEQ_CURRENT_STEP, W, BANKED
	addlw	1
	cpfslt	SEQ_EVNTS, BANKED
	rgoto SEQ_LCD_PrnMsgItemL_Normal
	rgoto	SEQ_LCD_PrnMsgItemL_Flash

SEQ_LCD_PrnMsgItemL_FlScrl
	;; check if SEQ_EVNTS is within the range
	SET_BSR	SEQ_BASE
	movf	SEQ_CURRENT_STEP, W, BANKED
	bz	SEQ_LCD_PrnMsgItemL_FlScrlEChk
	decf	SEQ_CURRENT_STEP, W, BANKED
	cpfsgt	SEQ_EVNTS, BANKED
	rgoto SEQ_LCD_PrnMsgItemL_Normal

SEQ_LCD_PrnMsgItemL_FlScrlEChk
	call	SEQ_FSR_TrkLen
	incf	INDF1, W
	cpfslt	SEQ_EVNTS, BANKED
	rgoto SEQ_LCD_PrnMsgItemL_Normal
	rgoto	SEQ_LCD_PrnMsgItemL_Flash


SEQ_LCD_PrnMsgItemL_Normal
	;; print PType value
	rgoto	SEQ_LCD_PrnPTypeVal

SEQ_LCD_PrnMsgItemL_Flash
	;; print 5 spaces to flash the current step
	movlw	5
	goto	SEQ_LCD_PrintSpaces

	;; ---
SEQ_LCD_PrnMsgItem_Hlp
	;; returns cursor X position depending on SEQ_EVNTS
	movff	SEQ_EVNTS, WREG

	andlw	0x0f
	mullw	5
	movlw	40-1
	cpfsgt	PRODL, ACCESS
	rgoto SEQ_LCD_PrnMsgItem_Hlp_Cont
	movlw	0x80 - 40
	addwf	PRODL, F
SEQ_LCD_PrnMsgItem_Hlp_Cont

	movf	PRODL, W
	return
