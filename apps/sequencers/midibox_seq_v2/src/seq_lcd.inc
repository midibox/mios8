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
#if DEFAULT_LCD_SIZE == 3
	movlw	0x80
	call	MIOS_LCD_CursorSet
	call	MIOS_LCD_Clear
#endif
	movlw	0x00
	call	MIOS_LCD_CursorSet
	goto	MIOS_LCD_Clear

;; --------------------------------------------------------------------------
;;  This functions sets the Y offsets for a menu page
;;  IN:  -
;;  OUT: -
;; --------------------------------------------------------------------------
SEQ_LCD_SetMenuOffsets
	;; init offsets depending on LCD type

#if DEFAULT_LCD_SIZE < 0 || DEFAULT_LCD_SIZE > 3
	ERROR "selected display size not supported!"
#endif

#if DEFAULT_LCD_SIZE == 0 ; 2x16
	movlw	0x00
	movwf	MIOS_PARAMETER1
	movlw	0x40
	movwf	MIOS_PARAMETER2
	movlw	0x14
	movwf	MIOS_PARAMETER3
	movlw	0x54
#endif
#if DEFAULT_LCD_SIZE == 1 ; 2x20
	movlw	0x02
	movwf	MIOS_PARAMETER1
	movlw	0x42
	movwf	MIOS_PARAMETER2
	movlw	0x16
	movwf	MIOS_PARAMETER3
	movlw	0x56
#endif
#if DEFAULT_LCD_SIZE == 2 ; 2x40
	movlw	0x18
	movwf	MIOS_PARAMETER1
	movlw	0x58
	movwf	MIOS_PARAMETER2
	movlw	0x00
	movwf	MIOS_PARAMETER3
	movlw	0x40
#endif
#if DEFAULT_LCD_SIZE == 3 ; 2 * 2x40
	movlw	0x18
	movwf	MIOS_PARAMETER1
	movlw	0x58
	movwf	MIOS_PARAMETER2
	movlw	0x80
	movwf	MIOS_PARAMETER3
	movlw	0xc0
#endif
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
;; print selected tracks
;; IN: selected tracks in SEQ_SELECTED_TRKS
;; USES: SEQ_EVNTT, TMP1
;; ==========================================================================
SEQ_LCD_PrnSelectedTrks
	SET_BSR	SEQ_BASE
	clrf	SEQ_EVNTT, BANKED
SEQ_LCD_PrnSelectedTrksLoop
	movf	SEQ_EVNTT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_SELECTED_TRKS, W, BANKED
	andlw	0x0f
	bz	SEQ_LCD_PrnSelectedTrksLoop0
SEQ_LCD_PrnSelectedTrksLoop1
	movf	SEQ_EVNTT, W, BANKED
	addlw	'1'
	rgoto	SEQ_LCD_PrnSelectedTrksLoopC
SEQ_LCD_PrnSelectedTrksLoop0
	movlw	'-'
	;; 	rgoto	SEQ_LCD_PrnSelectedTrksLoopC
SEQ_LCD_PrnSelectedTrksLoopC
	call	MIOS_LCD_PrintChar

	SET_BSR	SEQ_BASE
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 2, BANKED, SEQ_LCD_PrnSelectedTrksLoop

	return

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
;; print pattern name (uses TMP1)
;; ==========================================================================
SEQ_LCD_PrnPattern
	BRA_IFSET WREG, 7, ACCESS, SEQ_LCD_PrnPattern_Dis
	movwf	TMP1
	rlf	WREG, W
	swapf	WREG, W
	andlw	0x07
	btfss	TMP1, 6
	addlw 'A'
	btfsc	TMP1, 6
	addlw 'a'
	call	MIOS_LCD_PrintChar
	movf	TMP1, W
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
;; print song position (uses TMP1)
;; ==========================================================================
SEQ_LCD_PrnSongPos
	BRA_IFSET WREG, 7, ACCESS, SEQ_LCD_PrnSongPos_Dis
	movwf	TMP1
	rlf	WREG, W
	swapf	WREG, W
	andlw	0x0f
	addlw	'A'
	call	MIOS_LCD_PrintChar
	movf	TMP1, W
	andlw	0x07
	addlw	'1'
	goto	MIOS_LCD_PrintChar


;; ==========================================================================
;; print the note (uses MIOS_PARAMETER1 and FSR1)
;; ==========================================================================
SEQ_LCD_PrnNote
	goto	MIDI_EVNT_PrintNote


;; ==========================================================================
;; print an arp parameter (uses MIOS_PARAMETER1)
;; ==========================================================================
SEQ_LCD_PrnArpNote
	andlw	0xff		; print "---" if zero
	bz	SEQ_LCD_PrnNote
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

;; --------------------------------------------------------------------------
;;  This functions prints x chars
;;  IN:  character in PRODL, number of chars in WREG
;;  OUT: x chars
;;  USES: TMP5
;; --------------------------------------------------------------------------
SEQ_LCD_PrintChars
	movwf	TMP5
SEQ_LCD_PrintCharsLoop
	movf	PRODL, W
	call	MIOS_LCD_PrintChar
	decfsz	TMP5, F
	rgoto	SEQ_LCD_PrintCharsLoop
	return

;; --------------------------------------------------------------------------
;;  This functions prints x spaces
;;  IN:  number of spaces in WREG
;;  OUT: x spaces
;;  USES: TMP5, PRODL
;; --------------------------------------------------------------------------
SEQ_LCD_PrintSpaces
	movwf	TMP5
	movlw	' '
	movwf	PRODL
	rgoto	SEQ_LCD_PrintCharsLoop

;; ==========================================================================
;; this function prints the menu header
;; it contains the application name and the song position
;; ==========================================================================
SEQ_LCD_VERSION_STR_0 STRING 16, 0x00, MBSEQ_VERSION_STR	; (defined in main.asm)
SEQ_LCD_PrintHeader
	TABLE_ADDR SEQ_LCD_VERSION_STR_0
	goto	MIOS_LCD_PrintString

;; --------------------------------------------------------------------------
;;  this function clears the "previous length" and prepares the H/V bars
;;  for the selected PType
;;  IN: PType in SEQ_PTYPE
;;  OUT: -
;;  USES: BSR
;; --------------------------------------------------------------------------
SEQ_LCD_InitPType
	;; clear "previous length"
	SET_BSR	SEQ_BASE
	clrf	SEQ_LCD_PREVIOUS_EVNTL, BANKED

	;; prepare H/V bars
	movlw	CS_MENU_CHARSET_VBARS
	btfsc	SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNTL, BANKED
	movlw CS_MENU_CHARSET_HBARS
	goto	CS_SPECIAL_CHARS_Init

;; --------------------------------------------------------------------------
;;  this function print the PType name
;;  IN: PType in SEQ_PTYPE, selected track in EVNTT, current step in SEQ_EVNTS
;;	current drum layer in SEQ_EVNTD (only relevant in drum mode)
;;  OUT: 5 characters
;;  USES: BSR, FSR1, MIOS_PARAMETER[12], PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LCD_PrnPTypeName
	;; special handler for drum mode!
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_DRUMS, BANKED, SEQ_LCD_PrnPTypeName_Drum

	;; check for event length
	movlw	0x10
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNTL, BANKED, SEQ_LCD_PrnPTypeName_Cont ; -> EVNTL

	;; check for ARP_MODIF_VALUE
	movlw	0x11
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_ARP_MODIF_VALUE, BANKED, SEQ_LCD_PrnPTypeName_Cont ; -> ARP

	;; check for threshold value
	movlw	0x12
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_THRESHOLD_VALUE, BANKED, SEQ_LCD_PrnPTypeName_Cont ; -> MORPH THRS

SEQ_LCD_PrnPTypeNameEvnt0
	call	SEQ_FSR_TrkEvnt0
	swapf	INDF1, W
	andlw	0x07
	btfsc	SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNT2, BANKED
	addlw 8

SEQ_LCD_PrnPTypeName_Cont
	mullw	4		; every string consists of 4 characters

	movlw	SEQ_LCD_PrnPTypeNameTab & 0xff
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	(SEQ_LCD_PrnPTypeNameTab >> 8) & 0xff
	addwfc	PRODH, W
	movwf	TBLPTRH

	movlw	0x04
	call	MIOS_LCD_PrintPreconfString

	movlw	1
	rgoto	SEQ_LCD_PrintSpaces

SEQ_LCD_PrnPTypeNameTab
	db	"Note"
	db	"Note"
	db	"ATch"
	db	"CC# "
	db	"Prg."
	db	"CPs."
	db	"Pit."
	db	"CV-N"

	db	"Vel."		; 0x08
	db	"Vel."
	db	"ATV."
	db	"CCV."
	db	"----"
	db	"CPV."
	db	"----"
	db	"CV-V"

	db	"Len."		; 0x10
	db	"Arp."		; 0x11
	db	"Thrs"		; 0x12

	db	"Drum"		; 0x13
	db	"Chn."		; 0x14
	db	"Acc."		; 0x15


SEQ_LCD_PrnPTypeName_Drum
	movf	SEQ_EVNTS, W, BANKED
	bnz	SEQ_LCD_PrnPTypeName_Drum_1

SEQ_LCD_PrnPTypeName_Drum_0
	movlw	0x13
	rgoto	SEQ_LCD_PrnPTypeName_Cont ; -> "Drum"
	
SEQ_LCD_PrnPTypeName_Drum_1
	addlw	-1
	bnz	SEQ_LCD_PrnPTypeName_Drum_2
	incf	SEQ_EVNTT, W, BANKED
	call	MIOS_LCD_PrintBCD2
	SET_BSR	SEQ_BASE
	movlw	'A'
	btfsc	SEQ_SELECTED_LAYERS, 1, BANKED
	movlw 'B'
	btfsc	SEQ_SELECTED_LAYERS, 2, BANKED
	movlw 'C'
	call	MIOS_LCD_PrintChar
	movlw	2
	rgoto	SEQ_LCD_PrintSpaces	

SEQ_LCD_PrnPTypeName_Drum_2
	addlw	-1
	bnz	SEQ_LCD_PrnPTypeName_Drum_3
	rgoto	SEQ_LCD_PrnPTypeNameEvnt0

SEQ_LCD_PrnPTypeName_Drum_3
	addlw	-1
	bnz	SEQ_LCD_PrnPTypeName_Drum_4
	call	CS_M_Hlp_Get_D_TRK_x_y
	movlw	SEQ_D_TRKEVNT1
	addwf	FSR1L, F
	movf	INDF1, W
	RCALL_IFSET SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED, SEQ_LCD_PrnNote
	SET_BSR	SEQ_BASE
	CALL_IFCLR SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED, MIOS_LCD_PrintBCD3
	movlw	1
	rgoto	SEQ_LCD_PrintSpaces
	
SEQ_LCD_PrnPTypeName_Drum_4
	addlw	-1
	bnz	SEQ_LCD_PrnPTypeName_Drum_5
	movlw	0x14
	rgoto	SEQ_LCD_PrnPTypeName_Cont ; -> "Chn."

SEQ_LCD_PrnPTypeName_Drum_5
	addlw	-1
	bnz	SEQ_LCD_PrnPTypeName_Drum_6
	call	SEQ_FSR_TrkEvnt0
	movf	INDF1, W
	andlw	0x0f
	addlw	1
	call	MIOS_LCD_PrintBCD2
	movlw	3
	rgoto	SEQ_LCD_PrintSpaces
	
SEQ_LCD_PrnPTypeName_Drum_6
	addlw	-1
	bnz	SEQ_LCD_PrnPTypeName_Drum_7
	movlw	0x10
	rgoto	SEQ_LCD_PrnPTypeName_Cont ; -> "Len."

SEQ_LCD_PrnPTypeName_Drum_7
	addlw	-1
	bnz	SEQ_LCD_PrnPTypeName_Drum_8
	call	CS_M_Hlp_Get_D_TRK_x_y
	movlw	SEQ_D_TRKEVNT_L
	addwf	FSR1L, F
	movf	INDF1, W
	andlw	0x1f
	call	MIOS_LCD_PrintBCD3
	movlw	2
	rgoto	SEQ_LCD_PrintSpaces

SEQ_LCD_PrnPTypeName_Drum_8
	addlw	-1
	bnz	SEQ_LCD_PrnPTypeName_Drum_9
	movlw	0x08
	rgoto	SEQ_LCD_PrnPTypeName_Cont ; -> "Vel."

SEQ_LCD_PrnPTypeName_Drum_9
	addlw	-1
	bnz	SEQ_LCD_PrnPTypeName_Drum_10
	call	CS_M_Hlp_Get_D_TRK_x_y
	movlw	SEQ_D_TRKEVNT2_N
	addwf	FSR1L, F
	movf	INDF1, W
	call	MIOS_LCD_PrintBCD3
	movlw	2
	rgoto	SEQ_LCD_PrintSpaces

SEQ_LCD_PrnPTypeName_Drum_10
	addlw	-1
	bnz	SEQ_LCD_PrnPTypeName_Drum_11
	movlw	0x15
	rgoto	SEQ_LCD_PrnPTypeName_Cont ; -> "Acc."

SEQ_LCD_PrnPTypeName_Drum_11
	addlw	-1
	bnz	SEQ_LCD_PrnPTypeName_Drum_12
	call	CS_M_Hlp_Get_D_TRK_x_y
	movlw	SEQ_D_TRKEVNT2_A
	addwf	FSR1L, F
	movf	INDF1, W
	call	MIOS_LCD_PrintBCD3
	movlw	2
	rgoto	SEQ_LCD_PrintSpaces

SEQ_LCD_PrnPTypeName_Drum_12
SEQ_LCD_PrnPTypeName_Drum_Spc
	movlw	5
	call	SEQ_LCD_PrintSpaces
	return

;; --------------------------------------------------------------------------
;;  this function prints the PType value
;;  IN: PType in SEQ_PTYPE, current track in SEQ_EVNTT, current step in SEQ_EVNTS,
;;	current drum layer in SEQ_EVNTD (only relevant in drum mode)
;;  OUT: 5 characters
;;  USES: MIOS_PARAMETER1, TMP5, BSR
;; --------------------------------------------------------------------------
SEQ_LCD_PrnPTypeVal
	call	SEQ_EVNT_Get

	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_DRUMS, BANKED, SEQ_LCD_PrnPTypeVal_Drums

	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_THRESHOLD_VALUE, BANKED, SEQ_LCD_PrnPTypeVal_V_Thrs

	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNTL, BANKED, SEQ_LCD_PrnPTypeVal_L

	;; print "---- " if event is zero
	movf	SEQ_EVNT0, W, BANKED
	bnz	SEQ_LCD_PrnPTypeVal_NotMuted
SEQ_LCD_PrnPTypeVal_Muted
	movlw	'-'
	movwf	PRODL
	movlw	4
	rcall	SEQ_LCD_PrintChars
	movlw	1
	rgoto	SEQ_LCD_PrintSpaces


SEQ_LCD_PrnPTypeVal_NotMuted	
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_ARP, BANKED, SEQ_LCD_PrnPTypeVal_A

	;; branch depending on SEQ_PTYPE_NOTELIKE_EVENT
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED, SEQ_LCD_PrnPTypeVal_Note
SEQ_LCD_PrnPTypeVal_NoNote
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNT2, BANKED, SEQ_LCD_PrnPTypeVal_V_L2
SEQ_LCD_PrnPTypeVal_V_L1
	;; print first event byte as BCD and two spaces
	movf	SEQ_EVNT1, W, BANKED
	call	MIOS_LCD_PrintBCD3
	movlw	2
	rgoto	SEQ_LCD_PrintSpaces
	
SEQ_LCD_PrnPTypeVal_V_L2
	;; print second event byte as BCD, a V-Bar and a space
	movf	SEQ_EVNT2, W, BANKED
	call	MIOS_LCD_PrintBCD3
	movff	SEQ_EVNT2, WREG
	rcall	SEQ_LCD_PrnVBar
	movlw	1
	rgoto	SEQ_LCD_PrintSpaces
	
SEQ_LCD_PrnPTypeVal_V_Thrs
	;; print threshold value
	movf	SEQ_EVNTTHRS, W, BANKED
	call	MIOS_LCD_PrintBCD3
	movff	SEQ_EVNTTHRS, WREG
	rcall	SEQ_LCD_PrnVBar
	movlw	1
	rgoto	SEQ_LCD_PrintSpaces

SEQ_LCD_PrnPTypeVal_Drums
	;; for drums: we always have to print the velocity level
	movf	SEQ_EVNT0, W, BANKED
	bz	SEQ_LCD_PrnPTypeVal_Muted
	rgoto	SEQ_LCD_PrnPTypeVal_V_L2


SEQ_LCD_PrnPTypeVal_Note
SEQ_LCD_PrnPTypeVal_N_L12
	movf	SEQ_EVNT1, W, BANKED
	rcall	SEQ_LCD_PrnNote
SEQ_LCD_PrnPTypeVal_A_Cont
	movff	SEQ_EVNT2, WREG
	rcall	SEQ_LCD_PrnVBar
	movlw	1
	rgoto	SEQ_LCD_PrintSpaces

SEQ_LCD_PrnPTypeVal_A
	movf	SEQ_EVNT1, W, BANKED
	rcall	SEQ_LCD_PrnArpNote
	rgoto	SEQ_LCD_PrnPTypeVal_A_Cont

SEQ_LCD_PrnPTypeVal_L
	;; multi note trigger?
	movf	SEQ_EVNTTHRS, W, BANKED
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
	movlw	5
	rgoto	SEQ_LCD_PrintSpaces

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
	movf	PRODH, W
	mulwf	SEQ_EVNTL, BANKED
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
	movlw	1
	rgoto	SEQ_LCD_PrintSpaces

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
	movlw	1
	rgoto	SEQ_LCD_PrintSpaces
	
SEQ_LCD_PrnPTypeVal_L_Bar
	;; print gatelength in form of a nice horizontal bar
	;; if current gatelength > 24, print full bar
	movlw	24
	cpfsgt	SEQ_EVNTL, BANKED
	rgoto SEQ_LCD_PrnPTypeVal_L_Bar_NF
SEQ_LCD_PrnPTypeVal_L_Bar_F
	movlw	0x7f
	rgoto	SEQ_LCD_PrnPTypeVal_L_Bar_F_C
SEQ_LCD_PrnPTypeVal_L_Bar_NF
	rlf	SEQ_EVNTL, W, BANKED
	rlf	WREG, W
	andlw	0x7c
SEQ_LCD_PrnPTypeVal_L_Bar_F_C
	rcall	SEQ_LCD_PrnHBar
	;; memorize the gatelength
	movff	SEQ_EVNTL, SEQ_LCD_PREVIOUS_EVNTL
	return


;; --------------------------------------------------------------------------
;;  this function prints a PType item at the correct position depending on SEQ_EVNTS
;;  (upper line)
;;  IN: PType in SEQ_PTYPE, current track in SEQ_EVNTT, current step in SEQ_EVNTS
;;	current drum layer in SEQ_EVNTD (only relevant in drum mode)
;;  OUT: 5 characters
;;  USES: MIOS_PARAMETER1, BSR
;; --------------------------------------------------------------------------
SEQ_LCD_PrnMsgItemU
	;; set cursor depending on SEQ_EVNTS
	rcall	SEQ_LCD_PrnMsgItem_Hlp
	call	MIOS_LCD_CursorSet
	;; print PType name
	rgoto	SEQ_LCD_PrnPTypeName

;; --------------------------------------------------------------------------
;;  this function prints a PType item at the correct position depending on SEQ_EVNTS
;;  (lower line)
;;  IN: PType in SEQ_PTYPE, current track in SEQ_EVNTT, current step in SEQ_EVNTS
;;	current drum layer in SEQ_EVNTD (only relevant in drum mode)
;;  OUT: 5 characters
;;  USES: MIOS_PARAMETER1, BSR
;; --------------------------------------------------------------------------
SEQ_LCD_PrnMsgItemL
	;; set cursor depending on SEQ_EVNTS
	rcall	SEQ_LCD_PrnMsgItem_Hlp
	addlw	0x40
	call	MIOS_LCD_CursorSet
	;; print PType value
	rgoto	SEQ_LCD_PrnPTypeVal

	;; ---
SEQ_LCD_PrnMsgItem_Hlp
	;; returns cursor X position depending on SEQ_EVNTS
	movff	SEQ_EVNTS, WREG

#if DEFAULT_LCD_SIZE == 0
	andlw	0x03
	mullw	4
#endif

#if DEFAULT_LCD_SIZE == 1
	andlw	0x03
	mullw	5
#endif

#if DEFAULT_LCD_SIZE == 2
	andlw	0x07
	mullw	5
#endif

#if DEFAULT_LCD_SIZE == 3
	mullw	5
	movlw	40-1
	cpfsgt	PRODL, ACCESS
	rgoto SEQ_LCD_PrnMsgItem_Hlp_Cont
	movlw	0x80 - 40
	addwf	PRODL, F
SEQ_LCD_PrnMsgItem_Hlp_Cont
#endif

	movf	PRODL, W
	return


;; --------------------------------------------------------------------------
;;  this function prints an info screen
;;  it can be accessed by the user in EDIT mode (press the select button)
;;  it is also displayed in song mode if a large display (2*2x40) is used
;; --------------------------------------------------------------------------
SEQ_LCD_PrintInfo
	;; set flag so that we can decide between normal and song page
	bcf	MB_STAT, MB_STAT_INFO_IN_SONG_PAGE
	movlw	CS_MENU_PAGE_SONG
	cpfseq	CS_MENU_POS, ACCESS
	rgoto SEQ_LCD_PrintInfo_Normal

#if DEFAULT_LCD_SIZE != 3
	return			; print nothing in song mode
#endif
	bsf	MB_STAT, MB_STAT_INFO_IN_SONG_PAGE

SEQ_LCD_PrintInfo_Normal
	movlw	0x40
	btfsc	MB_STAT, MB_STAT_INFO_IN_SONG_PAGE
	iorlw 0x80
	call	MIOS_LCD_CursorSet

#if 0
	;; for hold stack debugging
	movff	SEQ_NOTE_STACK_0_HOLD, WREG
	rcall	SEQ_LCD_PrnNote
	movlw	' '
	call	MIOS_LCD_PrintChar
	movff	SEQ_NOTE_STACK_1_HOLD, WREG
	rcall	SEQ_LCD_PrnNote
	movlw	' '
	call	MIOS_LCD_PrintChar
	movff	SEQ_NOTE_STACK_2_HOLD, WREG
	rcall	SEQ_LCD_PrnNote
	movlw	' '
	call	MIOS_LCD_PrintChar
	movff	SEQ_NOTE_STACK_3_HOLD, WREG
	rcall	SEQ_LCD_PrnNote
	movlw	' '
	call	MIOS_LCD_PrintChar
	return
#endif

	;; print layer name (A/B/C)
	SET_BSR	SEQ_BASE
	movlw	'A'
	btfsc	SEQ_SELECTED_LAYERS, 1, BANKED
	movlw 'B'
	btfsc	SEQ_SELECTED_LAYERS, 2, BANKED
	movlw 'C'
	call	MIOS_LCD_PrintChar

	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print selected/muted tracks
	movlw	'1'
	SET_BSR	SEQ_BASE
	btfsc	SEQ_SELECTED_TRKS, 0, BANKED
	movlw '*'
	btfsc	SEQ_TRKS_MUTED0, 0, BANKED
	movlw '-'
	call	MIOS_LCD_PrintChar

	movlw	'2'
	SET_BSR	SEQ_BASE
	btfsc	SEQ_SELECTED_TRKS, 1, BANKED
	movlw '*'
	btfsc	SEQ_TRKS_MUTED0, 1, BANKED
	movlw '-'
	call	MIOS_LCD_PrintChar
	
	movlw	'3'
	SET_BSR	SEQ_BASE
	btfsc	SEQ_SELECTED_TRKS, 2, BANKED
	movlw '*'
	btfsc	SEQ_TRKS_MUTED0, 2, BANKED
	movlw '-'
	call	MIOS_LCD_PrintChar
	
	movlw	'4'
	SET_BSR	SEQ_BASE
	btfsc	SEQ_SELECTED_TRKS, 3, BANKED
	movlw '*'
	btfsc	SEQ_TRKS_MUTED0, 3, BANKED
	movlw '-'
	call	MIOS_LCD_PrintChar

	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print pattern number
	movff	SEQ_PATTERN0, WREG
	rcall	SEQ_LCD_PrnPattern

	;; in song mode: print song position at upper line
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_LCD_PrintInfo_NoSong
SEQ_LCD_PrintInfo_Song
	movlw	0x0a
	btfsc	MB_STAT, MB_STAT_INFO_IN_SONG_PAGE
	iorlw 0x80
	call	MIOS_LCD_CursorSet

	movlw	'>'
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	movf	SEQ_SONG_POS, W, BANKED
	call	SEQ_LCD_PrnSongPos

	movlw	'.'
	call	MIOS_LCD_PrintChar

	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_LCD_PrintInfo_SongNR
SEQ_LCD_PrintInfo_SongR
	incf	SEQ_CLK_STEP_CTR, W, BANKED
	call	MIOS_LCD_PrintBCD2
	rgoto	SEQ_LCD_PrintInfo_SongC
SEQ_LCD_PrintInfo_SongNR
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar

SEQ_LCD_PrintInfo_SongC
SEQ_LCD_PrintInfo_NoSong

	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED

	;; print position of current track
	;; in song mode: print loop counter instead
	movlw	0x4b
	btfsc	MB_STAT, MB_STAT_INFO_IN_SONG_PAGE
	iorlw 0x80
	call	MIOS_LCD_CursorSet

	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_LCD_PrintInfo_Song2
SEQ_LCD_PrintInfo_SongN2
	call	SEQ_FSR_TrkPos
	SET_BSR	SEQ_BASE
	movf	INDF1, W
	btfss	SEQ_STATE, SEQ_STATE_RUN, BANKED
	movlw 0x00
	rgoto	SEQ_LCD_PrintInfo_SongC2
SEQ_LCD_PrintInfo_Song2
	movf	SEQ_SONG_LOOP, W, BANKED
	subwf	SEQ_SONG_LOOP_MAX, W, BANKED
SEQ_LCD_PrintInfo_SongC2

	addlw	1
	call	MIOS_LCD_PrintBCD2
	movlw	'/'
	call	MIOS_LCD_PrintChar

	;; print max steps
	;; in song mode: print max loop counter
	call	SEQ_FSR_TrkDivLen
	swapf	INDF1, W
	andlw	0x0f
	SET_BSR	SEQ_BASE
	btfsc	SEQ_STATE, SEQ_STATE_SONG, BANKED
	movf	SEQ_SONG_LOOP_MAX, W, BANKED

	addlw	1
	call	MIOS_LCD_PrintBCD2

	;; reset cursor to first LCD
	movlw	0x00
	goto	MIOS_LCD_CursorSet


;; --------------------------------------------------------------------------
;;  this function prints the static part of the info screen
;;  it can be accessed by the user in EDIT mode (press the select button)
;;  it is also displayed in song mode if a large display (2x40 or 2*2x40) is used
;; --------------------------------------------------------------------------
SEQ_LCD_PRINTINFOSTATIC_STR db "L Trck P1    Pos"
SEQ_LCD_PrintInfoStatic
	;; set flag so that we can decide between normal and song page
	bcf	MB_STAT, MB_STAT_INFO_IN_SONG_PAGE
	movlw	CS_MENU_PAGE_SONG
	cpfseq	CS_MENU_POS, ACCESS
	rgoto SEQ_LCD_PrintInfoStatic_Normal

#if DEFAULT_LCD_SIZE != 3
	return			; print nothing in song mode
#endif
	bsf	MB_STAT, MB_STAT_INFO_IN_SONG_PAGE

SEQ_LCD_PrintInfoStatic_Normal

	movlw	0x00
	btfsc	MB_STAT, MB_STAT_INFO_IN_SONG_PAGE
	iorlw 0x80
	call	MIOS_LCD_CursorSet

	;; print string
	TABLE_ADDR SEQ_LCD_PRINTINFOSTATIC_STR
	movlw	16
	call	MIOS_LCD_PrintPreconfString

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; set step display update flag (update on every step change)
	bsf	CS_STAT, CS_STAT_STEP_DISPLAY_UPDATE
	return
