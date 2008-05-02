; $Id$
;
; MIDIboxSEQ
; Groove functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; constant value to improve the oversight: GVAR means: variable will be inserted
	;; Pxxx means: percentage (works only for velocity)
#define D__0    0
#define D__1    1
#define D__2    2
#define D__3    3
#define D__4    4
#define D__5    5
#define D__6    6
#define D__7    7
#define D__8    8
#define D__9    9
#define D_10   10
#define D_11   11
#define D_12   12
#define D_13   13
#define D_14   14
#define DVAR   15

#define V020    0
#define V030    1
#define V040    2
#define V050    3
#define V060    4
#define V070    5
#define V080    6
#define V090    7
#define V100    8
#define V110    9
#define V120   10
#define V130   11
#define V140   12
#define V150   13
#define V160   14
#define VVAR   15

#define L020    0
#define L040    1
#define L060    2
#define L080    3
#define L100    4
#define L120    5
#define L140    6
#define L160    7
#define LC25    8
#define LC50    9
#define LC75   10
#define LC__   11 ; full
#define LC2x   12
#define LC3x   13
#define LC4x   14
#define LVAR   15

GRV_VELOCITY MACRO s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16
	db	s1|(s2<<4),s3|(s4<<4),s5|(s6<<4),s7|(s8<<4),s9|(s10<<4),s11|(s12<<4),s13|(s14<<4),s15|(s16<<4)
	ENDM
GRV_LENGTH MACRO s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16
	db	s1|(s2<<4),s3|(s4<<4),s5|(s6<<4),s7|(s8<<4),s9|(s10<<4),s11|(s12<<4),s13|(s14<<4),s15|(s16<<4)
	ENDM
GRV_DELAY MACRO s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16
	db	s1|(s2<<4),s3|(s4<<4),s5|(s6<<4),s7|(s8<<4),s9|(s10<<4),s11|(s12<<4),s13|(s14<<4),s15|(s16<<4)
	ENDM

SEQ_GROOVE_TABLE
  db		" -- off --  "	; note: all values of this entry are ignored!
  GRV_DELAY	D__0, D__0, D__0, D__0, D__0, D__0, D__0, D__0, D__0, D__0, D__0, D__0, D__0, D__0, D__0, D__0
  GRV_LENGTH	L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100
  GRV_VELOCITY	V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100
	;;        1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16

  db		"  Shuffle   "
  GRV_DELAY	D__0, DVAR, D__0, DVAR, D__0, DVAR, D__0, DVAR, D__0, DVAR, D__0, DVAR, D__0, DVAR, D__0, DVAR
  GRV_LENGTH	L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100
  GRV_VELOCITY	V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100
	;;        1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16

  db		"Inv. Shuffle"
  GRV_DELAY	DVAR, D__0, DVAR, D__0, DVAR, D__0, DVAR, D__0, DVAR, D__0, DVAR, D__0, DVAR, D__0, DVAR, D__0
  GRV_LENGTH	L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100
  GRV_VELOCITY	V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100
	;;        1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16

  db	"  Shuffle2  "
  GRV_DELAY	D__0, D__8, D__0, DVAR, D__0, D__8, D__0, DVAR, D__0, D__8, D__0, DVAR, D__0, D__8, D__0, DVAR
  GRV_LENGTH	L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100
  GRV_VELOCITY	V100, V120, V100, V080, V100, V120, V100, V080, V100, V120, V100, V080, V100, V120, V100, V080
	;;        1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16

  db		"  Shuffle3  "
  GRV_DELAY	D__0, D__8, D__4, DVAR, D__0, D__8, D__4, DVAR, D__0, D__8, D__4, DVAR, D__0, D__8, D__4, DVAR
  GRV_LENGTH	L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100
  GRV_VELOCITY	V100, V080, V060, V080, V100, V080, V060, V080, V100, V080, V060, V080, V100, V080, V060, V080
	;;        1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16

  db		"  Shuffle4  "
  GRV_DELAY	D__0, D__8, D__0, D__8, D__0, D__8, D__0, D__8, D__0, D__8, D__0, D__8, D__0, D__8, D__0, D__8
  GRV_LENGTH	L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100, L100
  GRV_VELOCITY	V100, DVAR, V100, DVAR, V100, DVAR, V100, DVAR, V100, DVAR, V100, DVAR, V100, DVAR, V100, DVAR
	;;        1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16

  db		"  Shuffle5  "
  GRV_DELAY	D__0, D__8, D__0, D__8, D__0, D__8, D__0, D__8, D__0, D__8, D__0, D__8, D__0, D__8, D__0, D__8
  GRV_LENGTH	DVAR, L100, DVAR, L100, DVAR, L100, DVAR, L100, DVAR, L100, DVAR, L100, DVAR, L100, DVAR, L100
  GRV_VELOCITY	V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100, V100
	;;        1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16

SEQ_GROOVE_TABLE_End

	;; entry offsets
SEQ_GROOVE_TABLE_OFFSET_STR	EQU	0
SEQ_GROOVE_TABLE_OFFSET_D	EQU	12
SEQ_GROOVE_TABLE_OFFSET_L	EQU	12 + 8
SEQ_GROOVE_TABLE_OFFSET_V	EQU	12 + 8 + 8
SEQ_GROOVE_TABLE_ENTRY_LEN	EQU	12 + 8 + 8 + 8

	;; number of grooves: used in cs_m_trkgrv.inc
SEQ_GROOVE_NUMBER	EQU	(SEQ_GROOVE_TABLE_End-SEQ_GROOVE_TABLE) / SEQ_GROOVE_TABLE_ENTRY_LEN


;; --------------------------------------------------------------------------
;;  this function is called from SEQ_CORE if some groove should be added to
;;  the event. Note that SEQ_GROOVE_Get has already been called much earlier
;;  to determine the delay of a step. Now we have to process the SEQ_TRKVARGRVSCALEx
;;  value
;;  IN: SEQ_EVNTS: step number (0-15 for MIDIboxSEQ)
;;      SEQ_EVNTT: track number (0-15 for MIDIboxSEQ)
;;      FSR0: pointer to track record
;;      FSR2: pointer to track variables
;;      SEQ_EVNT[012L]: the MIDI event
;;      SEQ_EVNTL: the gatelength
;;      SEQ_TRKGROOVEx: groove mode and intensity
;;  OUT: "groovified" values in SEQ_EVNT[12L]
;; --------------------------------------------------------------------------
SEQ_GROOVE_Event
	;; MEMO:
	;;    SEQ_TRKGROOVEx[3..0]: groove intensity
	;;    SEQ_TRKGROOVEx[7..4]: groove style

	;; exit if groove style == 0 (disabled)
	movlw	SEQ_TRKGROOVEx
	movf	PLUSW0, W
	andlw	0xf0
	bz	SEQ_GROOVE_Event_End

	;; calculate pointer to table (style*SEQ_GROOVE_TABLE_ENTRY_LEN + SEQ_GROOVE_TABLE_L_OFFSET + step/2)
	movlw	SEQ_GROOVE_TABLE_OFFSET_L
	call	SEQ_GROOVE_GetTablePtr

	;; get length entry
	rcall	SEQ_GROOVE_GetEntry

	;; scale length based on the entry
	movf	SEQ_EVNTL, W, BANKED
	rcall	SEQ_GROOVE_ScaleLen
	movwf	SEQ_EVNTL, BANKED

	;; add offset to velocity entry
	movlw	SEQ_GROOVE_TABLE_OFFSET_V - SEQ_GROOVE_TABLE_OFFSET_L
	addwf	TBLPTRL, F
	movlw	0
	addwfc	TBLPTRH, F

	;; get velocity entry
	rcall	SEQ_GROOVE_GetEntry

	;; scale velocity based on the entry
	movf	SEQ_EVNT2, W, BANKED
	rcall	SEQ_GROOVE_ScaleVel
	movwf	SEQ_EVNT2, BANKED
	
SEQ_GROOVE_Event_End
	return

;; --------------------------------------------------------------------------
;;  This function returns a name of a groove style
;;  IN: Style Number in WREG
;;  OUT: 12 characters
;; --------------------------------------------------------------------------
SEQ_GROOVE_GetName
	mullw	SEQ_GROOVE_TABLE_ENTRY_LEN
	movlw	LOW(SEQ_GROOVE_TABLE)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(SEQ_GROOVE_TABLE)
	addwfc	PRODH, W
	movwf	TBLPTRH

	movlw	12
	goto	MIOS_LCD_PrintPreconfString


;; --------------------------------------------------------------------------
;;  This function sets the number of clock ticks which should be played per step
;;  IN: step number in SEQ_TRKVARSTEPx
;;      pointer to SEQ_TRKx in FSR0,
;;      pointer to SEQ_TRKVARx in FSR2
;;  OUT: tick number in SEQ_TRKVARPLYTICKx
;; --------------------------------------------------------------------------
SEQ_GROOVE_GetDelay
	;; check if delay trigger is activated
	call	SEQ_TRG_GetAuxDelay
	bz	SEQ_GROOVE_GetDelay_NoAux
SEQ_GROOVE_GetDelay_Aux
	movwf	TABLAT
	rgoto	SEQ_GROOVE_GetDelay_Aux_Cont

SEQ_GROOVE_GetDelay_NoAux
	;; calculate pointer to table (style*SEQ_GROOVE_TABLE_ENTRY_LEN + SEQ_GROOVE_TABLE_D_OFFSET + step/2)
	movlw	SEQ_GROOVE_TABLE_OFFSET_D
	call	SEQ_GROOVE_GetTablePtr

	rcall	SEQ_GROOVE_GetEntry	; returns delay value in TABLAT

SEQ_GROOVE_GetDelay_Aux_Cont
	movlw	SEQ_TRKVARPLYTICKx
	movff	TABLAT, PLUSW2

	return


;; --------------------------------------------------------------------------
;;  This function returns the table pointer depending on the selected groove
;;  style and step number
;;  IN: table offset (SEQ_GROOVE_TABLE_OFFSET_*) in WREG
;;      groove style in SEQ_TRKGROOVEx[7:4]
;;      step number in SEQ_TRKVARSTEPx
;;  OUT: new value in TABLAT
;; --------------------------------------------------------------------------
SEQ_GROOVE_GetTablePtr
	movwf	TABLAT		; temporary store offset in TABLAT

	;; get style offset
	movlw	SEQ_TRKGROOVEx	; style located in SEQ_TRKGROOVEx[7:4]
	swapf	PLUSW0, W
	andlw	0x0f
	mullw	SEQ_GROOVE_TABLE_ENTRY_LEN

	movlw	LOW(SEQ_GROOVE_TABLE)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(SEQ_GROOVE_TABLE)
	addwfc	PRODH, W
	movwf	TBLPTRH

	movlw	SEQ_TRKVARSTEPx	; get step number (mask 16 steps, rightshift due to compact 4bit format)
	rrf	PLUSW2, W
	andlw	0x07
	addwf	TABLAT, W	; add entry offset
	addwf	TBLPTRL, F	; add to pointer
	movlw	0
	addwfc	TBLPTRH, F

	return

;; --------------------------------------------------------------------------
;;  This function reads a nibble from the groove table and evtl. replaces
;;  it by the "groove intensity" parameter if it is 15 (DVAR, VVAR, LVAR)
;;  otherwise the original value will be returned
;;  IN: step number in TRKVARSTEPx
;;      FSR0: pointer to track record
;;      FSR2: pointer to track variables
;;  OUT: new value in TABLAT
;; --------------------------------------------------------------------------
SEQ_GROOVE_GetEntry
	;; MEMO:
	;;    SEQ_TRKGROOVEx[3..0]: groove intensity
	;;    SEQ_TRKGROOVEx[7..4]: groove style

	;; read table
	tblrd*

	;; swap byte of step number is odd
	movlw	SEQ_TRKVARSTEPx
	btfsc	PLUSW2, 0
	swapf	TABLAT, F

	;; mask out 4bit value
	movlw	0x0f
	andwf	TABLAT, F

	;; if TABLAT == 15, copy selected "groove intensity" into TABLAT
	movlw	15
	cpfseq	TABLAT, ACCESS
	return

	movlw	SEQ_TRKGROOVEx	; groove intensity located in TRKGROOVEx[3:0]
	movf	PLUSW0, W
	andlw	0x0f
	movwf	TABLAT
	return

;; --------------------------------------------------------------------------
;;  This function returns a scaled velocity value
;;  IN: value in WREG (7bit max)
;;      scale value in TABLAT
;;      pointer to SEQ_TRKx in FSR0,
;;      pointer to SEQ_TRKVARx in FSR2
;;  OUT: scaled velocity value in WREG (7bit max)
;; --------------------------------------------------------------------------
SEQ_GROOVE_ScaleVel
	;; temporary store in PRODL
	movwf	PRODL

	;; get scale value
	;; 8 == P100 == 100%
	movf	TABLAT, W
	xorlw	8
	bz	SEQ_GROOVE_ScaleVel_Skip

	;; multiply with input value
	movf	TABLAT, W
	mulwf	PRODL, ACCESS

	;; output value is PROD[HL] >> 8
	rrf	PRODH, F
	rrf	PRODL, F
	rrf	PRODH, F
	rrf	PRODL, F
	rrf	PRODH, F
	rrf	PRODL, W

	btfsc	WREG, 7; saturate
	movlw 0x7f	
	andlw	0x7f
	
	return

SEQ_GROOVE_ScaleVel_Skip
	;; with scale value == P100 we don't need to scale
	movf	PRODL, W
	return


;; --------------------------------------------------------------------------
;;  This function returns a scaled length value
;;  IN: value in WREG (7bit max)
;;      scale value in TABLAT
;;      pointer to SEQ_TRKx in FSR0,
;;      pointer to SEQ_TRKVARx in FSR2
;;  OUT: scaled length value in WREG (7bit max)
;; --------------------------------------------------------------------------
SEQ_GROOVE_ScaleLen
	;; temporary store in PRODL
	movwf	PRODL

	;; get scale value
	;; 4 == P100 == 100%
	movf	TABLAT, W
	xorlw	4
	bz	SEQ_GROOVE_ScaleLen_Skip

	;; branch if value >= 8 (value range 8..15)
	BRA_IFSET TABLAT, 3, ACCESS, SEQ_GROOVE_ScaleLenTab

SEQ_GROOVE_ScaleLenVal
	;; multiply with input value
	movf	TABLAT, W
	mulwf	PRODL, ACCESS

	;; output value is PROD[HL] >> 4
	rrf	PRODH, F
	rrf	PRODL, F
	rrf	PRODH, F
	rrf	PRODL, W
	movwf	TABLAT

	movlw	23
	cpfsgt	TABLAT, ACCESS
	rgoto SEQ_GROOVE_ScaleLenValNoSat
SEQ_GROOVE_ScaleLenValSat
	movlw	23
	return
SEQ_GROOVE_ScaleLenValNoSat
	movf	TABLAT, W
	return

SEQ_GROOVE_ScaleLen_Skip
	;; with scale value == P100 we don't need to scale
	movf	PRODL, W
	return

SEQ_GROOVE_ScaleLenTab
	;; get "table" value without touching TBLPTR[LH]
	BRA_IFSET TABLAT, 2, ACCESS, SEQ_GROOVE_ScaleLenTab_4567
SEQ_GROOVE_ScaleLenTab_0123
	BRA_IFSET TABLAT, 1, ACCESS, SEQ_GROOVE_ScaleLenTab_23
SEQ_GROOVE_ScaleLenTab_01
	movlw	0x05			; 0: LC25
	btfsc	TABLAT, 0; 1: LC50
	movlw 0x0b	
	rgoto	SEQ_GROOVE_ScaleLenTab_End

SEQ_GROOVE_ScaleLenTab_23
	movlw	0x11			; 2: LC75
	btfsc	TABLAT, 0; 3: LC__ (full)
	movlw 0x17	
	rgoto	SEQ_GROOVE_ScaleLenTab_End

SEQ_GROOVE_ScaleLenTab_4567
	BRA_IFSET TABLAT, 1, ACCESS, SEQ_GROOVE_ScaleLenTab_67
SEQ_GROOVE_ScaleLenTab_45
	movlw	0x2c			; 4: LC2x - 2x12
	btfsc	TABLAT, 0; 5: LC3x - 3x6
	movlw 0x46	
	rgoto	SEQ_GROOVE_ScaleLenTab_End

SEQ_GROOVE_ScaleLenTab_67
	movlw	0x63			; 6: LC4x - 4x3
					; 7: LVAR (already replaced - we return LC4x)
	;; 	rgoto	SEQ_GROOVE_ScaleLenTab_End

SEQ_GROOVE_ScaleLenTab_End
	return
