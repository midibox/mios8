; $Id$
;
; MIDIboxSEQ
; Layer specific functions
;
; This is the only place where we get informations about the layers
;
; If adaptions/extensions in the layers are made, they all have to be done
; here!
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; variable types (each step has a selectable value, embedded in the three available layers)
	;; Bit 3-0 is a unique number (0x0...0xf)
	;; Bit 6-4 select the MIDI event status byte, bit 7 must be 0!
	;;    0x0: not relevant for status byte (e.g. note length)
	;;    0x1: Note
	;;    0x3: CC
SEQ_LAYER_V_NONE	EQU	0x00
SEQ_LAYER_V_NOTE	EQU	0x11
SEQ_LAYER_V_CHORD1	EQU	0x12
SEQ_LAYER_V_CHORD2	EQU	0x13
SEQ_LAYER_V_VEL		EQU	0x14
SEQ_LAYER_V_CHORD1_VEL	EQU	0x15
SEQ_LAYER_V_CHORD2_VEL	EQU	0x16
SEQ_LAYER_V_LEN		EQU	0x07
SEQ_LAYER_V_CC		EQU	0x38

SEQ_LAYER_V_NAMES
	db	"----"		; SEQ_LAYER_V_NONE
	db	"Note"		; SEQ_LAYER_V_NOTE
	db	"Crd1"		; SEQ_LAYER_V_CHORD1
	db	"Crd2"		; SEQ_LAYER_V_CHORD2
	db	"Vel."		; SEQ_LAYER_V_VEL
	db	"Vel."		; SEQ_LAYER_V_CHORD1_VEL
	db	"Vel."		; SEQ_LAYER_V_CHORD2_VEL
	db	"Len."		; SEQ_LAYER_V_LEN
	db	" CC "		; SEQ_LAYER_V_CC

	;; constant types (the whole track only uses the specified value)
	;; Bit 3-0 is a unique number (0x0...0xf)
	;; Bit 6-4 select the MIDI event status byte, bit 7 must be 1!
	;;    0x8: not relevant for status byte (e.g. note length)
	;;    0x9: Note
	;;    0xb: CC
SEQ_LAYER_C_NONE	EQU	0x80
SEQ_LAYER_C_NOTE_A	EQU	0x91
SEQ_LAYER_C_NOTE_B	EQU	0x92
SEQ_LAYER_C_NOTE_C	EQU	0x93
SEQ_LAYER_C_VEL		EQU	0x94
SEQ_LAYER_C_LEN		EQU	0x05
SEQ_LAYER_C_CC_A	EQU	0xb6
SEQ_LAYER_C_CC_B	EQU	0xb7
SEQ_LAYER_C_CC_C	EQU	0xb8
SEQ_LAYER_C_CMEM_T	EQU	0x09

SEQ_LAYER_C_NAMES
	db	"----"		; SEQ_LAYER_C_NONE
	db	"NteA"		; SEQ_LAYER_C_NOTE_A
	db	"NteB"		; SEQ_LAYER_C_NOTE_B
	db	"NteC"		; SEQ_LAYER_C_NOTE_C
	db	"Vel."		; SEQ_LAYER_C_VEL
	db	"Len."		; SEQ_LAYER_C_LEN
	db	"CC#A"		; SEQ_LAYER_C_CC_A
	db	"CC#B"		; SEQ_LAYER_C_CC_B
	db	"CC#C"		; SEQ_LAYER_C_CC_C
	db	"CM-T"		; SEQ_LAYER_C_CMEM_T

	;; the assignments depending on the event mode
	;; note: on changes, adaptions have also to be made in SEQ_LAYER_GetEvnt!
SEQ_LAYER_TABLE
	;;      LayerA            LayerB            LayerC            Const1              Const2              Const3
	db	SEQ_LAYER_V_NOTE, SEQ_LAYER_V_VEL,  SEQ_LAYER_V_LEN,  SEQ_LAYER_C_NONE,   SEQ_LAYER_C_NONE,   SEQ_LAYER_C_NONE	 ; 1
	db	SEQ_LAYER_V_CHORD1,SEQ_LAYER_V_CHORD1_VEL, SEQ_LAYER_V_LEN,  SEQ_LAYER_C_CMEM_T, SEQ_LAYER_C_NONE,   SEQ_LAYER_C_NONE	 ; 2
	db	SEQ_LAYER_V_CHORD2,SEQ_LAYER_V_CHORD2_VEL, SEQ_LAYER_V_LEN,  SEQ_LAYER_C_NONE,  SEQ_LAYER_C_NONE,   SEQ_LAYER_C_NONE	 ; 3
	db	SEQ_LAYER_V_NOTE, SEQ_LAYER_V_NOTE, SEQ_LAYER_V_NOTE, SEQ_LAYER_C_VEL,    SEQ_LAYER_C_LEN,    SEQ_LAYER_C_NONE	 ; 4
	db	SEQ_LAYER_V_VEL,  SEQ_LAYER_V_VEL,  SEQ_LAYER_V_VEL,  SEQ_LAYER_C_NOTE_A, SEQ_LAYER_C_NOTE_B, SEQ_LAYER_C_NOTE_C ; 5
	db	SEQ_LAYER_V_NOTE, SEQ_LAYER_V_VEL,  SEQ_LAYER_V_CC,   SEQ_LAYER_C_LEN,    SEQ_LAYER_C_NONE,   SEQ_LAYER_C_CC_C	 ; 6
	db	SEQ_LAYER_V_NOTE, SEQ_LAYER_V_CC,   SEQ_LAYER_V_LEN,  SEQ_LAYER_C_VEL,    SEQ_LAYER_C_CC_B,   SEQ_LAYER_C_NONE   ; 7
	db	SEQ_LAYER_V_NOTE, SEQ_LAYER_V_CC,   SEQ_LAYER_V_CC,   SEQ_LAYER_C_VEL,    SEQ_LAYER_C_CC_B,   SEQ_LAYER_C_CC_C	 ; 8
	db	SEQ_LAYER_V_VEL,  SEQ_LAYER_V_CC,   SEQ_LAYER_V_CC,   SEQ_LAYER_C_NOTE_A, SEQ_LAYER_C_CC_B,   SEQ_LAYER_C_CC_C	 ; 9
	db	SEQ_LAYER_V_CC,   SEQ_LAYER_V_CC,   SEQ_LAYER_V_LEN,  SEQ_LAYER_C_CC_A,   SEQ_LAYER_C_CC_B,   SEQ_LAYER_C_NONE	 ; 10
	db	SEQ_LAYER_V_CC,   SEQ_LAYER_V_CC,   SEQ_LAYER_V_CC,   SEQ_LAYER_C_CC_A,   SEQ_LAYER_C_CC_B,   SEQ_LAYER_C_CC_C	 ; 11
SEQ_LAYER_TABLE_END

	;; define which event modes provide a special "drum mode" behaviour
	;; Note: if more than 2 modes are used, SEQ_LAYER_CheckDrumMode has to be extented as well
SEQ_LAYER_DRUM_MODE_EM1	EQU	5-1
SEQ_LAYER_DRUM_MODE_EM2	EQU	9-1

	;; this constant contains the number of available event modes
	;; it's derived from the number of entries of SEQ_LAYER_TABLE
SEQ_LAYER_NUMBER_EVNT_MODES	EQU (SEQ_LAYER_TABLE_END-SEQ_LAYER_TABLE)/6

	;; left side: maps layer A/B/C to the appr MIDI event number (0/1/2 - input of SEQ_LAYER_GetEvnt)
	;;            returned by SEQ_LAYER_Map_LtoENum
	;; right side: defines the order the MIDI event numbers are processed by the sequencer
	;;             15: not played, 0/1/2: order
SEQ_LAYER_EVENTNUM_MAP
SEQ_LAYER_EVNTNUM MACRO ma, mb, mc, oa, ob, oc, od
	db	(ma<<0) | (mb<<4), (mc<<0) | (0 << 4), (oa<<0) | (ob<<4), (oc<<0) | (od<<4)
	ENDM

	;;            map A  B  C  prio 1st 2nd 3rd 4th
	SEQ_LAYER_EVNTNUM 0, 0, 0,       0, 15, 15, 15	; 1 - only one Note Event
	SEQ_LAYER_EVNTNUM 0, 0, 0,       0,  1,  2,  3	; 2 - Chord Event with 4 notes maximum
	SEQ_LAYER_EVNTNUM 0, 0, 0,       0,  1,  2,  3	; 2 - Chord Event with 4 notes maximum
	SEQ_LAYER_EVNTNUM 0, 1, 2,       0,  1,  2, 15	; 3 - three Note Events
	SEQ_LAYER_EVNTNUM 0, 1, 2,       0,  1,  2, 15	; 4 - three Note Events
	SEQ_LAYER_EVNTNUM 0, 0, 2,       2,  0, 15, 15	; 5 - one Note, one CC event
	SEQ_LAYER_EVNTNUM 0, 1, 0,       1,  0, 15, 15	; 6 - one Note, one CC event
	SEQ_LAYER_EVNTNUM 0, 1, 2,       1,  2,  0, 15	; 7 - one Note, two CC events
	SEQ_LAYER_EVNTNUM 0, 1, 2,       1,  2,  0, 15	; 8 - one Note, two CC events
	SEQ_LAYER_EVNTNUM 0, 1, 0,       0,  1, 15, 15	; 9 - two CC events
	SEQ_LAYER_EVNTNUM 0, 1, 2,       0,  1,  2, 15	; 10 - three CC events


	;; preset table - each entry contains the initial track configuration and the step settings
	;; EVNT0 and CHN won't be overwritten
	;; for four steps (they are copied to all other 4step groups)
	;; structure of track settings matches with the SEQ_TRKRECORD in app_defines.h

	;; parameters which will be reset to the given values in all event modes
SEQ_LAYER_PRESET_TABLE_STATIC
	;;      parameter               value
	db	SEQ_TRKMODEx,		0x11
	db	SEQ_TRKDIR1x,		0x00
	db	SEQ_TRKDIR2x,		0x00
	db	SEQ_TRKDIVx,		0x03
	db	SEQ_TRKLENx,		0x0f
	db	SEQ_TRKLOOPx,		0x00
	db	SEQ_TRKTRANSPx,		0x00
	db	SEQ_TRKGROOVEx,		0x00
	db	SEQ_TRKMORPHx,		0x00
	db	SEQ_TRKHUMANIZEx,	0x00
	db	0xff ; end marker

	;; parameters which will be selected depending on the event mode
	;; note: number of bytes of this table entry must be even, otherwise MPASM will add an additional zero byte (padding)
SEQ_LAYER_PRESET_TABLE_MTABWIDTH	EQU	12 ; address + 10 event modes + 1 dummy
SEQ_LAYER_PRESET_TABLE_MODEx
	;;      parameter                 1     2     3     4     5     6     7     8     9    10    11
	db	SEQ_TRKEVNTCONST1x,	0x00, 0x00, 0x00, 0x64, 0x24, 0x11, 0x64, 0x64, 0x3c, 0x01, 0x01
	db	SEQ_TRKEVNTCONST2x,	0x00, 0x00, 0x00, 0x11, 0x26, 0x00, 0x01, 0x01, 0x01, 0x10, 0x10
	db	SEQ_TRKEVNTCONST3x,	0x00, 0x00, 0x00, 0x00, 0x2c, 0x01, 0x00, 0x10, 0x10, 0x00, 0x11
	db	SEQ_TRKASSGN0x,		0x21, 0x21, 0x21, 0x21, 0x20, 0x21, 0x21, 0x21, 0x20, 0x21, 0x21
	db	SEQ_TRKASSGN1x,		0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
	db	0x80,			0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 ; (SEQ_TRKTRGA_[0-3]x)
	db	0x81,			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ; (SEQ_TRKTRGB_[0-3]x)
	db	0x82,			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ; (SEQ_TRKTRGC_[0-3]x)
	db	0xff ; end marker

	;; layer values - 4 entries per layer, they will be duplicated for all 32 steps per layer
	;; each mode has a package of 3*4 bytes
SEQ_LAYER_PRESET_TABLE_LAYERx
	;;      Layer A                  Layer B                   Layer C
	db	0x3c, 0x3c, 0x3c, 0x3c,  0x64, 0x64, 0x64, 0x64,   0x11, 0x11, 0x11, 0x11  ; 1
	db	0x40, 0x40, 0x40, 0x40,  0x64, 0x64, 0x64, 0x64,   0x11, 0x11, 0x11, 0x11  ; 2
	db	0x40, 0x40, 0x40, 0x40,  0x64, 0x64, 0x64, 0x64,   0x11, 0x11, 0x11, 0x11  ; 3
	db	0x3c, 0x3c, 0x3c, 0x3c,  0x3e, 0x3e, 0x3e, 0x3e,   0x40, 0x40, 0x40, 0x40  ; 4
	db	0x64, 0x00, 0x00, 0x00,  0x64, 0x00, 0x00, 0x00,   0x64, 0x00, 0x00, 0x00  ; 5
	db	0x3c, 0x3c, 0x3c, 0x3c,  0x64, 0x64, 0x64, 0x64,   0x40, 0x40, 0x40, 0x40  ; 6
	db	0x3c, 0x3c, 0x3c, 0x3c,  0x40, 0x40, 0x40, 0x40,   0x11, 0x11, 0x11, 0x11  ; 7
	db	0x3c, 0x3c, 0x3c, 0x3c,  0x40, 0x40, 0x40, 0x40,   0x40, 0x40, 0x40, 0x40  ; 8
	db	0x64, 0x00, 0x00, 0x00,  0x40, 0x40, 0x40, 0x40,   0x40, 0x40, 0x40, 0x40  ; 9
	db	0x40, 0x40, 0x40, 0x40,  0x40, 0x40, 0x40, 0x40,   0x11, 0x11, 0x11, 0x11  ; 10
	db	0x40, 0x40, 0x40, 0x40,  0x40, 0x40, 0x40, 0x40,   0x40, 0x40, 0x40, 0x40  ; 11


;; --------------------------------------------------------------------------
;;  This function returns the variable type which is assigned to a 
;;  layer depending on the event mode (normaly stored in SEQ_TRKEVNT0x)
;;  IN: MIOS_PARAMETER1: event mode
;;      WREG: 0-2: layer number A/B/C
;;  OUT: SEQ_LAYER_V_* type in WREG
;;  USES: TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_GetVType
	addlw	LOW(SEQ_LAYER_TABLE)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SEQ_LAYER_TABLE)
	addwfc	TBLPTRH, F

	movf	MIOS_PARAMETER1, W ; add mode offset to table pointer
	mullw	6
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F

	;; return type
	tblrd*+
	movf	TABLAT, W
	return

;; --------------------------------------------------------------------------
;;  This function returns 0 (ZERO flag set) if the track is in "drum mode"
;;  this results into a partly different CS behaviour
;;  IN: MIOS_PARAMETER1: event mode
;;  OUT: 0 if drum track (ZERO flag set)
;;  USES: -
;; --------------------------------------------------------------------------
SEQ_LAYER_CheckDrumMode
	movf	MIOS_PARAMETER1, W
	andlw	0x0f

	xorlw	SEQ_LAYER_DRUM_MODE_EM1
	skpnz
	return
	xorlw	SEQ_LAYER_DRUM_MODE_EM1
	xorlw	SEQ_LAYER_DRUM_MODE_EM2
	return


;; --------------------------------------------------------------------------
;;  This function returns the constant type which is assigned to a 
;;  layer depending on the event mode (normaly stored in SEQ_TRKEVNT0x)
;;  IN: MIOS_PARAMETER1: event mode
;;      WREG: 0-2: const parameter number 1/2/3
;;  OUT: SEQ_LAYER_C_* type in WREG
;;  USES: TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_GetCType
	addlw	3
	rgoto	SEQ_LAYER_GetVType


;; --------------------------------------------------------------------------
;;  This function prints the variable type assigned to a layer
;;  IN: MIOS_PARAMETER1: event mode
;;      WREG: 0-2: layer A/B/C
;;  OUT: 4 characters
;;  USES: BSR, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_PrnVType
	rcall	SEQ_LAYER_GetVType
	andlw	0x0f		; mask out bit #7-4
SEQ_LAYER_PrnVType_XL_Cont
	mullw	4
	movlw	LOW(SEQ_LAYER_V_NAMES)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(SEQ_LAYER_V_NAMES)
	addwfc	PRODH, W
	movwf	TBLPTRH

	;; print 4 characters
	movlw	4
	goto	MIOS_LCD_PrintPreconfString


;; --------------------------------------------------------------------------
;;  Like SEQ_LAYER_PrnVType, but with 6 characters
;;  IN: MIOS_PARAMETER1: event mode
;;      WREG: 0-2: layer A/B/C
;;      SEQ_EVNT[012L] event
;;  OUT: 6 characters
;;  USES: BSR, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_PrnVType_XL
	SET_BSR	SEQ_BASE

	rcall	SEQ_LAYER_GetVType
	andlw	0x0f		; mask out bit #7-4
	movwf	PRODL
	xorlw	SEQ_LAYER_V_CC & 0x0f
	bnz	SEQ_LAYER_PrnVType_XL_Normal
SEQ_LAYER_PrnVType_XL_CC
	;; special treatment for CCs: print "CC#xxx" (xxx is assigned CC number), or print "Disab." if CC#0
	movf	SEQ_EVNT1, W, BANKED
	bz	SEQ_LAYER_PrnVType_XL_Disabled

	movlw	'C'
	call	MIOS_LCD_PrintChar
	movlw	'C'
	call	MIOS_LCD_PrintChar
	movlw	'#'
	call	MIOS_LCD_PrintChar
	movff	SEQ_EVNT1, WREG
	goto	MIOS_LCD_PrintBCD3

SEQ_LAYER_PrnVType_XL_Normal
#if 0
	;; TODO: requires unmuted step, otherwise SEQ_EVNT1 and SEQ_EVNT2 are 0 regardless a constant assigned note/vel!
	;; print "Disabled" if Note Number and/or Velocity == 0
	movf	SEQ_EVNT1, W, BANKED
	bz	SEQ_LAYER_PrnVType_XL_Disabled
	movf	SEQ_EVNT2, W, BANKED
	bz	SEQ_LAYER_PrnVType_XL_Disabled
#endif

	movf	PRODL, W	; normal event - print short name + 2 spaces
	rcall	SEQ_LAYER_PrnVType_XL_Cont
	movlw	2
	goto	SEQ_LCD_PrintSpaces

SEQ_LAYER_PrnVType_XL_STR db "Disab."
SEQ_LAYER_PrnVType_XL_Disabled
	TABLE_ADDR SEQ_LAYER_PrnVType_XL_STR
	movlw	6
	goto	MIOS_LCD_PrintPreconfString

;; --------------------------------------------------------------------------
;;  This function prints the constant types
;;  IN: MIOS_PARAMETER1: event mode
;;      WREG: 0-2: const parameter number 1/2/3
;;  OUT: 4 characters
;;  USES: BSR, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_PrnCType
	rcall	SEQ_LAYER_GetCType
	andlw	0x0f		; mask out bit #7-4
	mullw	4
	movlw	LOW(SEQ_LAYER_C_NAMES)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(SEQ_LAYER_C_NAMES)
	addwfc	PRODH, W
	movwf	TBLPTRH

	;; print 4 characters
	movlw	4
	goto	MIOS_LCD_PrintPreconfString

;; --------------------------------------------------------------------------
;;  This function prints the value of a constant type
;;  IN: MIOS_PARAMETER1: event mode
;;      MIOS_PARAMETER2: constant value
;;      WREG: 0-2: const parameter number 1/2/3
;;  OUT: 4 characters
;;  USES: BSR, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_PrnCVal
	rcall	SEQ_LAYER_GetCType
	andlw	0x0f		; mask out bit #7-4
	JUMPTABLE_2BYTES 10 ; entries
	rgoto	SEQ_LAYER_PrnCVal_0
	rgoto	SEQ_LAYER_PrnCVal_1
	rgoto	SEQ_LAYER_PrnCVal_2
	rgoto	SEQ_LAYER_PrnCVal_3
	rgoto	SEQ_LAYER_PrnCVal_4
	rgoto	SEQ_LAYER_PrnCVal_5
	rgoto	SEQ_LAYER_PrnCVal_6
	rgoto	SEQ_LAYER_PrnCVal_7
	rgoto	SEQ_LAYER_PrnCVal_8
	rgoto	SEQ_LAYER_PrnCVal_9


SEQ_LAYER_PrnCVal_0 ; SEQ_LAYER_C_NONE
	movlw	4
	goto	SEQ_LCD_PrintSpaces

SEQ_LAYER_PrnCVal_Disabled
	movlw	'-'
	movwf	PRODL
	movlw	4
	goto	SEQ_LCD_PrintChars

SEQ_LAYER_PrnCVal_1 ; SEQ_LAYER_C_NOTE_A
SEQ_LAYER_PrnCVal_2 ; SEQ_LAYER_C_NOTE_B
SEQ_LAYER_PrnCVal_3 ; SEQ_LAYER_C_NOTE_C
	movf	MIOS_PARAMETER2, W	; print "----" if value is zero
	bz	SEQ_LAYER_PrnCVal_Disabled
	
	movlw	' '
	call	MIOS_LCD_PrintChar
	movf	MIOS_PARAMETER2, W
	goto	SEQ_LCD_PrnNote

SEQ_LAYER_PrnCVal_6 ; SEQ_LAYER_C_CC_A
SEQ_LAYER_PrnCVal_7 ; SEQ_LAYER_C_CC_B
SEQ_LAYER_PrnCVal_8 ; SEQ_LAYER_C_CC_C
	movf	MIOS_PARAMETER2, W	; print "----" if value is zero
	bz	SEQ_LAYER_PrnCVal_Disabled
	
SEQ_LAYER_PrnCVal_4 ; SEQ_LAYER_C_VEL
	movlw	' '
	call	MIOS_LCD_PrintChar
	movf	MIOS_PARAMETER2, W
	goto	MIOS_LCD_PrintBCD3

SEQ_LAYER_PrnCVal_5 ; SEQ_LAYER_C_LEN
	movf	MIOS_PARAMETER2, W
	goto	SEQ_LCD_PrnLen

SEQ_LAYER_PrnCVal_9 ; SEQ_LAYER_C_CMEM_T
	movf	MIOS_PARAMETER2, W
	goto	SEQ_LCD_PrnGxTy


;; --------------------------------------------------------------------------
;;  This is the only function which should be used to fetch a variable layer value
;;  It checks if bit 7 of the value is set - if not, we assume that it
;;  already has been fetched from EEPROM, and return it.
;;  If bit 7 is set, we load the value from EEPROM, copy it into RAM and
;;  clear bit 7 (it's like a cache)
;;  IN: WREG: Layer (0-2)
;;      SEQ_EVNTT: track number (0-15)
;;      SEQ_EVNTS: step number (0-31)
;;      SEQ_PATTERN: selected pattern (each pattern holds 4 tracks)
;;      SEQ_PATTERN_BANK: selected bank
;;  USES: FSR1, EEADR, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_GetVValue
	movwf	EEADR		; temporary store layer value

	;; get pointer to layer
	rcall	SEQ_LAYER_Hlp_LayerFSR

	;; fetch if read from EEPROM was forced (used by SEQ_MORPH)
	BRA_IFSET SEQ_STATE, SEQ_STATE_FORCE_EEPROM, BANKED, SEQ_LAYER_GetVValue_Fetch

	;; fetch value if bit 7 set (value not cached, see SEQ_DUMP_RestorePots)
	BRA_IFCLR INDF1, 7, ACCESS, SEQ_LAYER_GetVValue_Cached
SEQ_LAYER_GetVValue_Fetch
	;; fetch value from EEPROM
	movf	EEADR, W
	andlw	0x03
	mullw	0x80
	movff	PRODL, EEADR
	movff	PRODH, EEADRH
	swapf	SEQ_EVNTT, W, BANKED	; add track offset
	rlf	WREG, W
	andlw	0x60
	addwf	EEADR, F
	movf	SEQ_EVNTS, W, BANKED	; add step offset
	andlw	0x1f
	addwf	EEADR, F
	call	SEQ_BANK_Read
	clrf	EEADRH
	andlw	0x7f

	;; don't store value in cache
	btfsc	SEQ_STATE, SEQ_STATE_FORCE_EEPROM, BANKED
	return

	;; store in SEQ_LAYER_VALUES*, bit 7 must be cleared now!
	movwf	INDF1

SEQ_LAYER_GetVValue_Cached
	movf	INDF1, W
	andlw	0x7f
	return

;; --------------------------------------------------------------------------
;;  ClrVValue and SetVValue arethe only functions which should be used to store a variable layer value
;;  IN: WREG: Layer (0-2)
;;      MIOS_PARAMETER1: value (only relevant for SetVValue)
;;      SEQ_EVNTT: track number (0-3)
;;      SEQ_EVNTS: step number (0-31)
;;  USES: FSR1, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_ClrVValue
	rcall	SEQ_LAYER_Hlp_LayerFSR
	clrf	INDF1
	return

SEQ_LAYER_SetVValue
	rcall	SEQ_LAYER_Hlp_LayerFSR
	movf	MIOS_PARAMETER1, W	; ensure that bit 7 not set!
	andlw	0x7f
	movwf	INDF1
	return

;; help function for SEQ_LAYER_GetVValue and Set_LayerValue
;; returns pointer to SEQ_LAYER_VALUES_* in FSR1 depending on WREG/SEQ_EVNTT/SEQ_EVNTS
SEQ_LAYER_Hlp_LayerFSR
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_A		; if Layer A selected
	btfsc	WREG, 0
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_B		; if Layer B selected
	btfsc	WREG, 1
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_C		; if Layer C selected

	movf	SEQ_EVNTS, W, BANKED			; add step offset
	andlw	0x1f
	addwf	FSR1L, F
	movf	SEQ_EVNTT, W, BANKED
	mullw	0x20
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwf	FSR1H, F
	return


;; --------------------------------------------------------------------------
;;  Help function for cs_m_hlp.inc - maps a layer (A/B/C) to a MIDI event number (0/1/2)
;;  IN: MIOS_PARAMETER1: event mode
;;      WREG: Number of Layer A/B/C (0-2)
;;  OUT: WREG: Number of MIDI event (0-2) - input for SEQ_LAYER_GetEvnt
;;  USES: TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_Map_LtoENum
	movwf	PRODL		; temporary store layer in PRODL

	rlf	MIOS_PARAMETER1, W	; table contains 4 bytes entries, the first two bytes contain the map
	rlf	WREG, W
	andlw	0xfc
	btfsc	PRODL, 1
	addlw 1
	addlw	LOW(SEQ_LAYER_EVENTNUM_MAP)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SEQ_LAYER_EVENTNUM_MAP)
	addwfc	TBLPTRH, F

	tblrd*+			; get table entry
	;; branch depending on layer number
	BRA_IFSET PRODL, 0, ACCESS, SEQ_LAYER_Map_LtoENum_LBD
SEQ_LAYER_Map_LtoENum_LAC
	movf	TABLAT, W
	andlw	0x0f
	return

SEQ_LAYER_Map_LtoENum_LBD
	swapf	TABLAT, W
	andlw	0x0f
	return

;; --------------------------------------------------------------------------
;;  This function returns the SEQ_EVNT_NUMBER for a given counter value 0-3
;;  It is used by the sequencer core to handle the MIDI event priorities
;;  within a track: CCs should be played before a Note event
;;  IN: counter value (0..2) in WREG
;;      SEQ_EVNTT: track number (0-3)
;;  OUT: WREG: 0-3 or 15: SEQ_EVNT_NUMBER, 15: don't play event
;;  USES: TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_GetEvntPrio
	movwf	TABLAT		; temporary store event number in TABLAT

	call	SEQ_FSR_TrkEvnt	; get event mode
	rlf	INDF1, W	; table contains 4 bytes entries, the last two bytes contain the map
	rlf	WREG, W
	andlw	0xfc
	addlw	2
	btfsc	TABLAT, 1
	addlw 1
	addlw	LOW(SEQ_LAYER_EVENTNUM_MAP)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SEQ_LAYER_EVENTNUM_MAP)
	addwfc	TBLPTRH, F

	movff	TABLAT, PRODL	; now store counter value in PRODL

	tblrd*+			; get table entry (overwrites TABLAT)
	;; branch depending on layer number
	BRA_IFSET PRODL, 0, ACCESS, SEQ_LAYER_GetEvntPrio_13
SEQ_LAYER_GetEvntPrio_02
	movf	TABLAT, W
	andlw	0x0f
	return

SEQ_LAYER_GetEvntPrio_13
	swapf	TABLAT, W
	andlw	0x0f
	return

;; --------------------------------------------------------------------------
;;  This function returns the complete MIDI event of the given track, step and layer
;;  IN: SEQ_EVNT_NUMBER: Number of MIDI event of the track (0-2)
;;      SEQ_EVNTT: track number (0-3)
;;      SEQ_EVNTS: step number (0-31)
;;  OUT: the MIDI event in SEQ_EVNT[012]
;;       the gatelength in SEQ_EVNTL
;;       on chords: the index (original Layer A value)
;;  USES: TBLPTR[LH], TABLAT, EEADR, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_GetEvnt
	;; select SEQ_PATTERN/BANK depending on track
	rcall	SEQ_LAYER_Hlp_SetPattern

SEQ_LAYER_GetEvnt_AltPattern	; direct entry point for SEQ_MORPH: alternative pattern defined in SEQ_PATTERN/SEQ_PATTERN_BANK

	;; (only used on chords)
	clrf	SEQ_EVNT_CHORD_INDEX, BANKED

	call	SEQ_FSR_TrkEvnt		; get event mode
	movff	INDF1, MIOS_PARAMETER1	; -> MIOS_PARAMETER1

	;; whole pattern disabled (muted) if SEQ_PATTERNx[7] set
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_PATTERN, 7, BANKED, SEQ_LAYER_GetEvnt_Clr

	;; thanks to the special coding of SEQ_LAYER_V_* definitions, determing the status byte is easy:
	movf	SEQ_EVNT_NUMBER, W, BANKED
	rcall	SEQ_LAYER_GetVType	; derive MIDI status byte from V code
	iorlw	0x8f
	andlw	0xf0
	movwf	SEQ_EVNT0, BANKED

	call	SEQ_FSR_TrkChn		; add channel
	movf	INDF1, W
	andlw	0x0f
	iorwf	SEQ_EVNT0, F, BANKED

	;; branch depending on mode
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_LAYER_GetEvnt_1	; see SEQ_LAYER_TABLE for purpose
	rgoto	SEQ_LAYER_GetEvnt_2
	rgoto	SEQ_LAYER_GetEvnt_3
	rgoto	SEQ_LAYER_GetEvnt_4
	rgoto	SEQ_LAYER_GetEvnt_5
	rgoto	SEQ_LAYER_GetEvnt_6
	rgoto	SEQ_LAYER_GetEvnt_7
	rgoto	SEQ_LAYER_GetEvnt_8
	rgoto	SEQ_LAYER_GetEvnt_9
	rgoto	SEQ_LAYER_GetEvnt_10
	rgoto	SEQ_LAYER_GetEvnt_11
	rgoto	SEQ_LAYER_GetEvnt_12
	rgoto	SEQ_LAYER_GetEvnt_13
	rgoto	SEQ_LAYER_GetEvnt_14
	rgoto	SEQ_LAYER_GetEvnt_15
	rgoto	SEQ_LAYER_GetEvnt_16

SEQ_LAYER_GetEvnt_1		; Layer: Note/Vel/Len, Constants: -/-/-
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only one event
	skpz
	rgoto	SEQ_LAYER_GetEvnt_Clr
	call	SEQ_TRG_GateGet		; don't play note if step is muted
	skpnz
	rgoto	SEQ_LAYER_GetEvnt_Clr

	movlw	0		; Note Number stored in Layer A
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT1, BANKED
	movlw	1		; Note Velocity stored in Layer B
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED

	rgoto	SEQ_LAYER_GetEvntL_1
	

SEQ_LAYER_GetEvnt_2		; Layer: Chord/Vel/Len, Constants: -/-/-
	;; plays up to 4 notes
	call	SEQ_TRG_GateGet		; don't play note if step is muted
	skpnz
	rgoto	SEQ_LAYER_GetEvnt_Clr

	movf	SEQ_EVNT0, W, BANKED	; ensure that note will be played
	andlw	0x0f			; (required due to SEQ_LAYER_TABLE based initialisation above)
	iorlw	0x90			; SEQ_EVNT0 will contain 0x00 (or 0x80) on the third note
	movwf	SEQ_EVNT0, BANKED	; otherwise

	movlw	0		; Chord Index stored in Layer A
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT_CHORD_INDEX, BANKED	; must be stored for display function

	;; chord memory access
	movff	SEQ_EVNTT, SEQ_LAYER_STORED_EVNTT	; store values which are temporary overwritten
	movff	SEQ_EVNTS, SEQ_LAYER_STORED_EVNTS
	movff	SEQ_PATTERN, SEQ_LAYER_STORED_PATTERN
	movff	SEQ_PATTERN_BANK, SEQ_LAYER_STORED_PATTERN_BANK

	call	SEQ_FSR_TrkEvntConst1			; CMEM track is constant value #1
	movf	INDF1, W
	andlw	0x0f					; (0-15)
	movwf	SEQ_EVNTT, BANKED			; -> SEQ_EVNTT

	movf	SEQ_EVNT_CHORD_INDEX, W, BANKED		; chord index & 0x1f -> SEQ_EVNTS
	andlw	0x1f
	movwf	SEQ_EVNTS, BANKED

	rcall	SEQ_LAYER_Hlp_SetPattern		; set SEQ_PATTERN/SEQ_PATTERN_BANK

	movf	SEQ_EVNT_NUMBER, W, BANKED		; fetch note from chord memory
	call	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT1, BANKED
	
	movff	SEQ_LAYER_STORED_EVNTT, SEQ_EVNTT	; restore values which have been temporary overwritten
	movff	SEQ_LAYER_STORED_EVNTS, SEQ_EVNTS
	movff	SEQ_LAYER_STORED_PATTERN, SEQ_PATTERN
	movff	SEQ_LAYER_STORED_PATTERN_BANK, SEQ_PATTERN_BANK

	movlw	1		; Note Velocity stored in Layer B
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED

	rgoto	SEQ_LAYER_GetEvntL_2


SEQ_LAYER_GetEvnt_3		; Layer: Chord/Vel/Len, Constants: -/-/-
	;; plays up to 4 notes
	call	SEQ_TRG_GateGet		; don't play note if step is muted
	skpnz
	rgoto	SEQ_LAYER_GetEvnt_Clr

	movf	SEQ_EVNT0, W, BANKED	; ensure that note will be played
	andlw	0x0f			; (required due to SEQ_LAYER_TABLE based initialisation above)
	iorlw	0x90			; SEQ_EVNT0 will contain 0x00 (or 0x80) on the third note
	movwf	SEQ_EVNT0, BANKED	; otherwise

	movlw	0		; Chord Index stored in Layer A
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT_CHORD_INDEX, BANKED	; must be stored for display function
	movff	SEQ_EVNT_NUMBER, MIOS_PARAMETER1
	call	SEQ_CHORD_Get
	skpnz
	rgoto	SEQ_LAYER_GetEvnt_Clr
	movwf	SEQ_EVNT1, BANKED

	movlw	1		; Note Velocity stored in Layer B
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED

	rgoto	SEQ_LAYER_GetEvntL_3


SEQ_LAYER_GetEvnt_4		; Layer: Note/Note/Note, Constants: Vel/Len/-
	call	SEQ_TRG_GateGet		; don't play note if step is muted
	skpnz
	rgoto	SEQ_LAYER_GetEvnt_Clr

	movf	SEQ_EVNT_NUMBER, W, BANKED	; note number stored in Layer A/B/C
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT1, BANKED
	call	SEQ_FSR_TrkEvntConst1	; velocity is constant value #1
	movff	INDF1, SEQ_EVNT2

	rgoto	SEQ_LAYER_GetEvntL_4


SEQ_LAYER_GetEvnt_5		; Layer: Vel/Vel/Vel, Constants: NoteA/NoteB/NoteC
	call	SEQ_TRG_GateGet		; don't play note if step is muted
	skpnz
	rgoto	SEQ_LAYER_GetEvnt_Clr

	movf	SEQ_EVNT_NUMBER, W, BANKED	; velocity stored in Layer A/B/C
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED
	call	SEQ_FSR_TrkEvntConst1	; note number is constant value #1/2/3
	movf	SEQ_EVNT_NUMBER, W, BANKED
	movff	PLUSW1, SEQ_EVNT1

	rgoto	SEQ_LAYER_GetEvntL_5


SEQ_LAYER_GetEvnt_6		; Layer: Note/Vel/CC, Constants: LenA/-/CC_C
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only two events
	bz	SEQ_LAYER_GetEvnt_6_0
	addlw	-2
	bz	SEQ_LAYER_GetEvnt_6_2
	rgoto	SEQ_LAYER_GetEvnt_Clr

SEQ_LAYER_GetEvnt_6_0		; Note
	call	SEQ_TRG_GateGet		; don't play notes if step is muted
	skpnz
	rgoto	SEQ_LAYER_GetEvnt_Clr

	movlw	0		; Note Number stored in Layer A
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT1, BANKED
	movlw	1		; Note Velocity stored in Layer B
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED
	rgoto	SEQ_LAYER_GetEvntL_6_0

SEQ_LAYER_GetEvnt_6_2
	call	SEQ_FSR_TrkEvntConst1	; CC number is constant value #3
	movlw	2
	movff	PLUSW1, SEQ_EVNT1
	movlw	2		; CC value stored in Layer C
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED
	rgoto	SEQ_LAYER_GetEvntL_6_2


SEQ_LAYER_GetEvnt_7		; Layer: Note/Len/CC, Constants: VelA/-/CC_C
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only two events
	bz	SEQ_LAYER_GetEvnt_7_0
	addlw	-1
	bz	SEQ_LAYER_GetEvnt_7_1
	rgoto	SEQ_LAYER_GetEvnt_Clr

SEQ_LAYER_GetEvnt_7_0		; Note
	call	SEQ_TRG_GateGet		; don't play notes if step is muted
	skpnz
	rgoto	SEQ_LAYER_GetEvnt_Clr

	movlw	0		; Note Number stored in Layer A
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT1, BANKED
	call	SEQ_FSR_TrkEvntConst1	; velocity is constant value #1
	movff	INDF1, SEQ_EVNT2
	rgoto	SEQ_LAYER_GetEvntL_7_0

SEQ_LAYER_GetEvnt_7_1
	call	SEQ_FSR_TrkEvntConst1	; CC number is constant value #2
	movlw	1
	movff	PLUSW1, SEQ_EVNT1
	movlw	1		; CC value stored in Layer B
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED
	rgoto	SEQ_LAYER_GetEvntL_7_1


SEQ_LAYER_GetEvnt_8		; Layer: Note/CC/CC, Constants: VelA/CC_B/CC_C
	movf	SEQ_EVNT_NUMBER, W, BANKED
	bz	SEQ_LAYER_GetEvnt_8_0
	addlw	-1
	bz	SEQ_LAYER_GetEvnt_8_1
	rgoto	SEQ_LAYER_GetEvnt_8_2

SEQ_LAYER_GetEvnt_8_0
	call	SEQ_TRG_GateGet		; don't play notes if step is muted
	skpnz
	rgoto	SEQ_LAYER_GetEvnt_Clr

	movlw	0			; Note Number stored in Layer A
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT1, BANKED
	call	SEQ_FSR_TrkEvntConst1	; Note Velocity is constant value #1
	movlw	0
	movff	PLUSW1, SEQ_EVNT2
	rgoto	SEQ_LAYER_GetEvntL_8_0

SEQ_LAYER_GetEvnt_8_1
	call	SEQ_FSR_TrkEvntConst1	; CC number is constant value #2
	movlw	1
	movff	PLUSW1, SEQ_EVNT1
	movlw	1		; CC value stored in Layer B
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED
	rgoto	SEQ_LAYER_GetEvntL_8_1

SEQ_LAYER_GetEvnt_8_2
	call	SEQ_FSR_TrkEvntConst1	; CC number is constant value #3
	movlw	2
	movff	PLUSW1, SEQ_EVNT1
	movlw	2		; CC value stored in Layer C
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED
	rgoto	SEQ_LAYER_GetEvntL_8_2


SEQ_LAYER_GetEvnt_9		; Layer: Vel/CC/CC, Constants: NoteA/CC_B/CC_C
	movf	SEQ_EVNT_NUMBER, W, BANKED
	bz	SEQ_LAYER_GetEvnt_9_0
	addlw	-1
	bz	SEQ_LAYER_GetEvnt_9_1
	rgoto	SEQ_LAYER_GetEvnt_9_2

SEQ_LAYER_GetEvnt_9_0
	call	SEQ_TRG_GateGet		; don't play notes if step is muted
	skpnz
	rgoto	SEQ_LAYER_GetEvnt_Clr

	call	SEQ_FSR_TrkEvntConst1	; note number is constant value #1
	movff	INDF1, SEQ_EVNT1
	movlw	0		; Velocity stored in Layer A
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED
	rgoto	SEQ_LAYER_GetEvntL_9_0

SEQ_LAYER_GetEvnt_9_1
	call	SEQ_FSR_TrkEvntConst1	; CC number is constant value #2
	movlw	1
	movff	PLUSW1, SEQ_EVNT1
	movlw	1		; CC value stored in Layer B
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED
	rgoto	SEQ_LAYER_GetEvntL_9_1

SEQ_LAYER_GetEvnt_9_2
	call	SEQ_FSR_TrkEvntConst1	; CC number is constant value #3
	movlw	2
	movff	PLUSW1, SEQ_EVNT1
	movlw	2		; CC value stored in Layer C
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED
	rgoto	SEQ_LAYER_GetEvntL_9_2


SEQ_LAYER_GetEvnt_10		; Layer: CC/CC/Len, Constants: CC_A/CC_B/-
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only two events
	bz	SEQ_LAYER_GetEvnt_10_0
	addlw	-1
	bz	SEQ_LAYER_GetEvnt_10_1
	rgoto	SEQ_LAYER_GetEvnt_Clr

SEQ_LAYER_GetEvnt_10_0
	call	SEQ_FSR_TrkEvntConst1	; CC number is constant value #1
	movlw	0
	movff	PLUSW1, SEQ_EVNT1
	movlw	0		; CC value stored in Layer A
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED
	rgoto	SEQ_LAYER_GetEvntL_10

SEQ_LAYER_GetEvnt_10_1
	call	SEQ_FSR_TrkEvntConst1	; CC number is constant value #2
	movlw	1
	movff	PLUSW1, SEQ_EVNT1
	movlw	1		; CC value stored in Layer B
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED
	rgoto	SEQ_LAYER_GetEvntL_10


SEQ_LAYER_GetEvnt_11		; Layer: CC/CC/CC, Constants: CC_A/CC_B/CC_C
	call	SEQ_FSR_TrkEvntConst1	; CC number is constant value #1/2/3
	movf	SEQ_EVNT_NUMBER, W, BANKED
	movff	PLUSW1, SEQ_EVNT1
	movf	SEQ_EVNT_NUMBER, W, BANKED	; CC value stored in Layer A/B/C
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNT2, BANKED
	rgoto	SEQ_LAYER_GetEvntL_11

SEQ_LAYER_GetEvnt_12		; not used (yet)
SEQ_LAYER_GetEvnt_13		; not used (yet)
SEQ_LAYER_GetEvnt_14		; not used (yet)
SEQ_LAYER_GetEvnt_15		; not used (yet)
SEQ_LAYER_GetEvnt_16		; not used (yet)

SEQ_LAYER_GetEvnt_Clr		; used when a certain layer doesn't contain an additional event
	clrf	SEQ_EVNT0, BANKED
	clrf	SEQ_EVNT1, BANKED
	clrf	SEQ_EVNT2, BANKED

	;; if pattern disabled: set length to 0 as well and exit
	BRA_IFCLR SEQ_PATTERN, 7, BANKED, SEQ_LAYER_GetEvntL
SEQ_LAYER_GetEvnt_PtnDisabled
	clrf	SEQ_EVNTL, BANKED
	return

SEQ_LAYER_GetEvntL
	call	SEQ_FSR_TrkEvnt		; get event mode
	movff	INDF1, MIOS_PARAMETER1	; -> MIOS_PARAMETER1

	;; set length depending on mode
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_LAYER_GetEvntL_1	; see SEQ_LAYER_TABLE for purpose
	rgoto	SEQ_LAYER_GetEvntL_2
	rgoto	SEQ_LAYER_GetEvntL_3
	rgoto	SEQ_LAYER_GetEvntL_4
	rgoto	SEQ_LAYER_GetEvntL_5
	rgoto	SEQ_LAYER_GetEvntL_6
	rgoto	SEQ_LAYER_GetEvntL_7
	rgoto	SEQ_LAYER_GetEvntL_8
	rgoto	SEQ_LAYER_GetEvntL_9
	rgoto	SEQ_LAYER_GetEvntL_10
	rgoto	SEQ_LAYER_GetEvntL_11
	rgoto	SEQ_LAYER_GetEvntL_12
	rgoto	SEQ_LAYER_GetEvntL_13
	rgoto	SEQ_LAYER_GetEvntL_14
	rgoto	SEQ_LAYER_GetEvntL_15
	rgoto	SEQ_LAYER_GetEvntL_16

SEQ_LAYER_GetEvntL_1		; (also used as direct jump target of SEQ_LAYER_GetEvnt_1)
	movlw	2		; Length stored in Layer C
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNTL, BANKED
	rgoto	SEQ_LAYER_GetEvntL_End

SEQ_LAYER_GetEvntL_2		; (also used as direct jump target of SEQ_LAYER_GetEvnt_2)
	movlw	2		; Length stored in Layer C
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNTL, BANKED
	rgoto	SEQ_LAYER_GetEvntL_End

SEQ_LAYER_GetEvntL_3		; (also used as direct jump target of SEQ_LAYER_GetEvnt_3)
	movlw	2		; Length stored in Layer C
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNTL, BANKED
	rgoto	SEQ_LAYER_GetEvntL_End

SEQ_LAYER_GetEvntL_4		; (also used as direct jump target of SEQ_LAYER_GetEvnt_4)
	call	SEQ_FSR_TrkEvntConst1	; length is constant value #2
	movff	PREINC1, SEQ_EVNTL
	rgoto	SEQ_LAYER_GetEvntL_End

SEQ_LAYER_GetEvntL_5		; (also used as direct jump target of SEQ_LAYER_GetEvnt_5)
	movlw	0x10			; fixed length
	movwf	SEQ_EVNTL, BANKED
	rgoto	SEQ_LAYER_GetEvntL_End


SEQ_LAYER_GetEvntL_6
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only two events
	bz	SEQ_LAYER_GetEvntL_6_0
	addlw	-2
	bz	SEQ_LAYER_GetEvntL_6_2
	rgoto	SEQ_LAYER_GetEvntL_End

SEQ_LAYER_GetEvntL_6_0		; (also used as direct jump target of SEQ_LAYER_GetEvnt_6_0)
	call	SEQ_FSR_TrkEvntConst1	; length is constant value #1
	movff	INDF1, SEQ_EVNTL
	rgoto	SEQ_LAYER_GetEvntL_End

SEQ_LAYER_GetEvntL_6_2		; (also used as direct jump target of SEQ_LAYER_GetEvnt_6_2)
	movlw	0x1f		; fixed length (maximum)
	movwf	SEQ_EVNTL, BANKED
	rgoto	SEQ_LAYER_GetEvntL_End


SEQ_LAYER_GetEvntL_7
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only two events
	bz	SEQ_LAYER_GetEvntL_7_0
	addlw	-1
	bz	SEQ_LAYER_GetEvntL_7_1
	rgoto	SEQ_LAYER_GetEvntL_End

SEQ_LAYER_GetEvntL_7_0		; (also used as direct jump target of SEQ_LAYER_GetEvnt_7_0)
	movlw	2		; Note Length stored in Layer C
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNTL, BANKED
	rgoto	SEQ_LAYER_GetEvntL_End

SEQ_LAYER_GetEvntL_7_1		; (also used as direct jump target of SEQ_LAYER_GetEvnt_7_1)
	movlw	0x1f		; fixed length (maximum)
	movwf	SEQ_EVNTL, BANKED
	rgoto	SEQ_LAYER_GetEvntL_End


SEQ_LAYER_GetEvntL_8
SEQ_LAYER_GetEvntL_9
	movf	SEQ_EVNT_NUMBER, W, BANKED
	bz	SEQ_LAYER_GetEvntL_9_0
	addlw	-1
	bz	SEQ_LAYER_GetEvntL_9_1
	rgoto	SEQ_LAYER_GetEvntL_9_2

SEQ_LAYER_GetEvntL_8_0		; (also used as direct jump target of SEQ_LAYER_GetEvnt_8_0)
SEQ_LAYER_GetEvntL_9_0		; (also used as direct jump target of SEQ_LAYER_GetEvnt_9_0)
	movlw	0x10		; fixed length
	movwf	SEQ_EVNTL, BANKED
	rgoto	SEQ_LAYER_GetEvntL_End

SEQ_LAYER_GetEvntL_8_1		; (also used as direct jump target of SEQ_LAYER_GetEvnt_8_1)
SEQ_LAYER_GetEvntL_9_1		; (also used as direct jump target of SEQ_LAYER_GetEvnt_9_1)
	movlw	0x1f		; fixed length (maximum)
	movwf	SEQ_EVNTL, BANKED
	rgoto	SEQ_LAYER_GetEvntL_End

SEQ_LAYER_GetEvntL_8_2		; (also used as direct jump target of SEQ_LAYER_GetEvnt_8_2)
SEQ_LAYER_GetEvntL_9_2		; (also used as direct jump target of SEQ_LAYER_GetEvnt_9_2)
	movlw	0x1f		; fixed length (maximum)
	movwf	SEQ_EVNTL, BANKED
	rgoto	SEQ_LAYER_GetEvntL_End


SEQ_LAYER_GetEvntL_10
	movlw	2		; length stored in Layer C
	rcall	SEQ_LAYER_GetVValue
	movwf	SEQ_EVNTL, BANKED
	rgoto	SEQ_LAYER_GetEvntL_End

SEQ_LAYER_GetEvntL_11
	movlw	0x1f			; fixed length
	movwf	SEQ_EVNTL, BANKED
	rgoto	SEQ_LAYER_GetEvntL_End

SEQ_LAYER_GetEvntL_12		; not used (yet)
SEQ_LAYER_GetEvntL_13		; not used (yet)
SEQ_LAYER_GetEvntL_14		; not used (yet)
SEQ_LAYER_GetEvntL_15		; not used (yet)
SEQ_LAYER_GetEvntL_16		; not used (yet)
	clrf	SEQ_EVNTL, BANKED
	;; 	rgoto	SEQ_LAYER_GetEvntL_End


SEQ_LAYER_GetEvntL_End
	;; exit if event is 0
	movf	SEQ_EVNT0, W, BANKED
	skpnz
	return

	;; force velocity to 0x7f if accent flag set
	call	SEQ_TRG_AccentGet
	movlw	0x7f
	skpz
	movwf	SEQ_EVNT2, BANKED
	
	;; force length to 0x2c (2x12) of roll flag is set
	call	SEQ_TRG_RollGet
	movlw	0x2c
	skpz
	movwf	SEQ_EVNTL, BANKED

	;; force length to 0x1f if glide flag is set
	call	SEQ_TRG_GlideGet
	movlw	0x1f
	skpz
	movwf	SEQ_EVNTL, BANKED

	return


;; --------------------------------------------------------------------------
;;  This help function sets SEQ_PATTERN/SEQ_PATTERN_BANK depending on
;;  the selected track
;;  IN: SEQ_EVNTT: track number (0-3)
;;  OUT: SEQ_PATTERN and SEQ_PATTERN_BANK
;;  USES: FSR1
;; --------------------------------------------------------------------------
SEQ_LAYER_Hlp_SetPattern
	lfsr	FSR1, SEQ_PATTERN0
	rrf	SEQ_EVNTT, W, BANKED
	rrf	WREG, W
	andlw	0x03
	movff	PLUSW1, SEQ_PATTERN
	lfsr	FSR1, SEQ_PATTERN_BANK0
	movff	PLUSW1, SEQ_PATTERN_BANK
	return



;; --------------------------------------------------------------------------
;;  This function saves a MIDI event to the given track and step
;;  IN: SEQ_EVNT_NUMBER: Number of MIDI event of the track (0-2)
;;      SEQ_EVNTT: track number (0-3)
;;      SEQ_EVNTS: step number (0-31)
;;      SEQ_EVNT[12] and SEQ_EVNTL: the event/length which should be saved
;;      SEQ_RECORD_MODE: various control flags
;;  USES: TBLPTR[LH], TABLAT, EEADR, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_SaveEvnt
	call	SEQ_FSR_TrkEvnt		; get event mode
	movff	INDF1, MIOS_PARAMETER1	; -> MIOS_PARAMETER1

	;; whole pattern disabled (muted) if SEQ_PATTERNx[7] set
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_PATTERN, 7, BANKED, SEQ_LAYER_SaveEvnt_End

	;; branch depending on mode
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_LAYER_SaveEvnt_1	; see SEQ_LAYER_TABLE for purpose
	rgoto	SEQ_LAYER_SaveEvnt_2
	rgoto	SEQ_LAYER_SaveEvnt_3
	rgoto	SEQ_LAYER_SaveEvnt_4
	rgoto	SEQ_LAYER_SaveEvnt_5
	rgoto	SEQ_LAYER_SaveEvnt_6
	rgoto	SEQ_LAYER_SaveEvnt_7
	rgoto	SEQ_LAYER_SaveEvnt_8
	rgoto	SEQ_LAYER_SaveEvnt_9
	rgoto	SEQ_LAYER_SaveEvnt_10
	rgoto	SEQ_LAYER_SaveEvnt_11
	rgoto	SEQ_LAYER_SaveEvnt_12
	rgoto	SEQ_LAYER_SaveEvnt_13
	rgoto	SEQ_LAYER_SaveEvnt_14
	rgoto	SEQ_LAYER_SaveEvnt_15
	rgoto	SEQ_LAYER_SaveEvnt_16

SEQ_LAYER_SaveEvnt_1		; Layer: Note/Vel/Len, Constants: -/-/-
SEQ_LAYER_SaveEvnt_2		; Layer: Chord/Vel/Len, Constants: -/-/-
SEQ_LAYER_SaveEvnt_3		; Layer: Chord/Vel/Len, Constants: -/-/-
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only one event
	skpz
	rgoto	SEQ_LAYER_SaveEvnt_End

	;; skip if CC event
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	;; always set gate
	movlw	0x01
	call	SEQ_TRG_GateSet

	movff	SEQ_EVNT1, MIOS_PARAMETER1	; Note Number stored in Layer A
	movlw	0
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED, SEQ_LAYER_SetVValue

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; Note Velcity stored in Layer B
	movlw	1
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_1		; branch to save length routine
	

SEQ_LAYER_SaveEvnt_4		; Layer: Note/Note/Note, Constants: Vel/Len/-
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only one event
	skpz
	rgoto	SEQ_LAYER_SaveEvnt_End

	;; skip if CC event
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	;; set gate if velocity > 0
	movf	SEQ_EVNT2, W, BANKED
	movlw	0x01
	skpnz
	movlw	0x00
	call	SEQ_TRG_GateSet

	;; if bit #7 of poly counter not set: clear all steps (first time we record this step)
	BRA_IFSET SEQ_RECORD_POLY_CTR, 7, BANKED, SEQ_LAYER_SaveEvnt_4_Next
SEQ_LAYER_SaveEvnt_4_First
	;; set "steps cleared" notifier
	bsf	SEQ_RECORD_POLY_CTR, 7, BANKED
	movlw	0		; Layer A
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED, SEQ_LAYER_ClrVValue
	movlw	1		; Layer B
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED, SEQ_LAYER_ClrVValue
	movlw	2		; Layer C
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_C, BANKED, SEQ_LAYER_ClrVValue
SEQ_LAYER_SaveEvnt_4_Next

	;; skip if note is already stored in layer A, B or C
	clrf	TABLAT		; used as loop counter
SEQ_LAYER_SaveEvnt_4_ChkLoop
	movf	TABLAT, W
	rcall	SEQ_LAYER_GetVValue
	xorwf	SEQ_EVNT1, W, BANKED
	skpnz
	rgoto	SEQ_LAYER_SaveEvnt_End
	incf	TABLAT, F
	movlw	3-1
	cpfsgt	TABLAT, ACCESS
	rgoto	SEQ_LAYER_SaveEvnt_4_ChkLoop

	;; branch depending on poly counter
	movf	SEQ_RECORD_POLY_CTR, W, BANKED
	andlw	0x7f
	bz	SEQ_LAYER_SaveEvnt_4_A
	addlw	-1
	bz	SEQ_LAYER_SaveEvnt_4_B
	rgoto	SEQ_LAYER_SaveEvnt_4_C

SEQ_LAYER_SaveEvnt_4_A
	;; skip if layer not selected for recording
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED, SEQ_LAYER_SaveEvnt_4_B
	movff	SEQ_EVNT1, MIOS_PARAMETER1	; Note Value stored in Layer A
	movlw	0
	rcall	SEQ_LAYER_SetVValue
	movlw	0x01		; next poly counter (select layer B)
	rgoto	SEQ_LAYER_SaveEvnt_4_Cont

SEQ_LAYER_SaveEvnt_4_B
	;; skip if layer not selected for recording
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED, SEQ_LAYER_SaveEvnt_4_C
	movff	SEQ_EVNT1, MIOS_PARAMETER1	; Note Value stored in Layer B
	movlw	1
	rcall	SEQ_LAYER_SetVValue
	movlw	0x02		; next poly counter (select layer C)
	rgoto	SEQ_LAYER_SaveEvnt_4_Cont

SEQ_LAYER_SaveEvnt_4_C
	;; skip if layer not selected for recording
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_C, BANKED, SEQ_LAYER_SaveEvnt_4_None
	movff	SEQ_EVNT1, MIOS_PARAMETER1	; Note Value stored in Layer C
	movlw	2
	rcall	SEQ_LAYER_SetVValue
SEQ_LAYER_SaveEvnt_4_None
	movlw	0x00		; start at Layer A again with next step
	;; 	rgoto	SEQ_LAYER_SaveEvnt_4_Cont

SEQ_LAYER_SaveEvnt_4_Cont
	movwf	PRODL
	movf	SEQ_RECORD_POLY_CTR, W, BANKED
	andlw	0x80		; mask out "steps cleared" step
	iorwf	PRODL, W
	movwf	SEQ_RECORD_POLY_CTR, BANKED

	rgoto	SEQ_LAYER_SaveEvntL_4		; branch to save length routine


SEQ_LAYER_SaveEvnt_5		; Layer: Vel/Vel/Vel, Constants: NoteA/NoteB/NoteC
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only one event
	skpz
	rgoto	SEQ_LAYER_SaveEvnt_End

	;; skip if CC event
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	;; always set gate
	movlw	0x01
	call	SEQ_TRG_GateSet

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; Velocity Value stored in Layer A
	movlw	0
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED, SEQ_LAYER_SetVValue

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; Velocity Value stored in Layer B
	movlw	1
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED, SEQ_LAYER_SetVValue

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; Velocity Value stored in Layer C
	movlw	2
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_C, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_5		; branch to save length routine


SEQ_LAYER_SaveEvnt_6		; Layer: Note/Vel/CC, Constants: LenA/-/CC_C
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only two events
	bz	SEQ_LAYER_SaveEvnt_6_0
	addlw	-2
	bz	SEQ_LAYER_SaveEvnt_6_2
	rgoto	SEQ_LAYER_SaveEvnt_End

SEQ_LAYER_SaveEvnt_6_0		; Note
	;; skip if CC event
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	;; always set gate
	movlw	0x01
	call	SEQ_TRG_GateSet

	movff	SEQ_EVNT1, MIOS_PARAMETER1	; Note Number stored in Layer A
	movlw	0
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED, SEQ_LAYER_SetVValue

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; Note Velcity stored in Layer B
	movlw	1
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_6_0		; branch to save length routine

SEQ_LAYER_SaveEvnt_6_2
	;; skip if no CC event
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; CC Value stored in Layer C
	movlw	2
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_C, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_6_2		; branch to save length routine

SEQ_LAYER_SaveEvnt_7		; Layer: Note/Len/CC, Constants: VelA/-/CC_C
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only two events
	bz	SEQ_LAYER_SaveEvnt_7_0
	addlw	-1
	bz	SEQ_LAYER_SaveEvnt_7_1
	rgoto	SEQ_LAYER_SaveEvnt_End

SEQ_LAYER_SaveEvnt_7_0		; Note
	;; skip if CC event
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	;; always set gate
	movlw	0x01
	call	SEQ_TRG_GateSet

	movff	SEQ_EVNT1, MIOS_PARAMETER1	; Note Number stored in Layer A
	movlw	0
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_7_0		; branch to save length routine

SEQ_LAYER_SaveEvnt_7_1
	;; skip if no CC event
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; CC Value stored in Layer C
	movlw	1
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_7_1		; branch to save length routine


SEQ_LAYER_SaveEvnt_8		; Layer: Note/CC/CC, Constants: Vel/CC_B/CC_C
	movf	SEQ_EVNT_NUMBER, W, BANKED
	bz	SEQ_LAYER_SaveEvnt_8_0
	addlw	-1
	bz	SEQ_LAYER_SaveEvnt_8_1
	rgoto	SEQ_LAYER_SaveEvnt_8_2

SEQ_LAYER_SaveEvnt_8_0
	;; skip if CC event
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	;; always set gate
	movlw	0x01
	call	SEQ_TRG_GateSet

	movff	SEQ_EVNT1, MIOS_PARAMETER1	; Note Number stored in Layer A
	movlw	0
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_8_0		; branch to save length routine

SEQ_LAYER_SaveEvnt_8_1
	;; skip if no CC event
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; CC Value stored in Layer B
	movlw	1
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_8_1		; branch to save length routine

SEQ_LAYER_SaveEvnt_8_2
	;; skip if no CC event
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; CC Value stored in Layer C
	movlw	2
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_8_2		; branch to save length routine


SEQ_LAYER_SaveEvnt_9		; Layer: Vel/CC/CC, Constants: NoteA/CC_B/CC_C
	movf	SEQ_EVNT_NUMBER, W, BANKED
	bz	SEQ_LAYER_SaveEvnt_9_0
	addlw	-1
	bz	SEQ_LAYER_SaveEvnt_9_1
	rgoto	SEQ_LAYER_SaveEvnt_9_2

SEQ_LAYER_SaveEvnt_9_0
	;; skip if CC event
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	;; always set gate
	movlw	0x01
	call	SEQ_TRG_GateSet

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; Note Velcity stored in Layer A
	movlw	0
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_9_0		; branch to save length routine

SEQ_LAYER_SaveEvnt_9_1
	;; skip if no CC event
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; CC Value stored in Layer B
	movlw	1
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_9_1		; branch to save length routine

SEQ_LAYER_SaveEvnt_9_2
	;; skip if no CC event
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; CC Value stored in Layer C
	movlw	2
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_9_2		; branch to save length routine


SEQ_LAYER_SaveEvnt_10		; Layer: CC/CC/Len, Constants: CC_A/CC_B/-
	;; skip if no CC event
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	;; always set gate
	movlw	0x01
	call	SEQ_TRG_GateSet

	movf	SEQ_EVNT_NUMBER, W, BANKED	; only two events
	bz	SEQ_LAYER_SaveEvnt_10_0
	addlw	-1
	bz	SEQ_LAYER_SaveEvnt_10_1
	rgoto	SEQ_LAYER_SaveEvnt_End

SEQ_LAYER_SaveEvnt_10_0
	movff	SEQ_EVNT2, MIOS_PARAMETER1	; CC Value stored in Layer A
	movlw	0
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_10		; branch to save length routine

SEQ_LAYER_SaveEvnt_10_1
	movff	SEQ_EVNT2, MIOS_PARAMETER1	; CC Value stored in Layer B
	movlw	1
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_10		; branch to save length routine

SEQ_LAYER_SaveEvnt_11		; Layer: CC/CC/CC, Constants: CC_A/CC_B/CC_C
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only one event
	skpz
	rgoto	SEQ_LAYER_SaveEvnt_End

	;; skip if no CC event
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_LAYER_SaveEvnt_End

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; CC Value stored in Layer A
	movlw	0
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED, SEQ_LAYER_SetVValue

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; CC Value stored in Layer B
	movlw	1
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED, SEQ_LAYER_SetVValue

	movff	SEQ_EVNT2, MIOS_PARAMETER1	; CC Value stored in Layer C
	movlw	2
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_C, BANKED, SEQ_LAYER_SetVValue

	rgoto	SEQ_LAYER_SaveEvntL_11		; branch to save length routine

SEQ_LAYER_SaveEvnt_12		; not used (yet)
SEQ_LAYER_SaveEvnt_13		; not used (yet)
SEQ_LAYER_SaveEvnt_14		; not used (yet)
SEQ_LAYER_SaveEvnt_15		; not used (yet)
SEQ_LAYER_SaveEvnt_16		; not used (yet)
	rgoto	SEQ_LAYER_SaveEvnt_End


	;; can also be called directly to save the length only
SEQ_LAYER_SaveEvntL
	call	SEQ_FSR_TrkEvnt		; get event mode
	movff	INDF1, MIOS_PARAMETER1	; -> MIOS_PARAMETER1

	;; whole pattern disabled (muted) if SEQ_PATTERNx[7] set
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_PATTERN, 7, BANKED, SEQ_LAYER_SaveEvnt_End

	;; branch depending on mode
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_LAYER_SaveEvntL_1	; see SEQ_LAYER_TABLE for purpose
	rgoto	SEQ_LAYER_SaveEvntL_2
	rgoto	SEQ_LAYER_SaveEvntL_3
	rgoto	SEQ_LAYER_SaveEvntL_4
	rgoto	SEQ_LAYER_SaveEvntL_5
	rgoto	SEQ_LAYER_SaveEvntL_6
	rgoto	SEQ_LAYER_SaveEvntL_7
	rgoto	SEQ_LAYER_SaveEvntL_8
	rgoto	SEQ_LAYER_SaveEvntL_9
	rgoto	SEQ_LAYER_SaveEvntL_10
	rgoto	SEQ_LAYER_SaveEvntL_11
	rgoto	SEQ_LAYER_SaveEvntL_12
	rgoto	SEQ_LAYER_SaveEvntL_13
	rgoto	SEQ_LAYER_SaveEvntL_14
	rgoto	SEQ_LAYER_SaveEvntL_15
	rgoto	SEQ_LAYER_SaveEvntL_16

SEQ_LAYER_SaveEvntL_1		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_1)
SEQ_LAYER_SaveEvntL_2		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_2)
SEQ_LAYER_SaveEvntL_3		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_2)
	movff	SEQ_EVNTL, MIOS_PARAMETER1	; Gate Length stored in Layer C
	movlw	2
	RCALL_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_C, BANKED, SEQ_LAYER_SetVValue
	rgoto	SEQ_LAYER_SaveEvntL_End

SEQ_LAYER_SaveEvntL_4		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_3)
	;; length is constant value - don't store
	rgoto	SEQ_LAYER_SaveEvntL_End

SEQ_LAYER_SaveEvntL_5		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_4)
	;; length is constant value - don't store
	rgoto	SEQ_LAYER_SaveEvntL_End


SEQ_LAYER_SaveEvntL_6
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only two events
	bz	SEQ_LAYER_SaveEvntL_6_0
	addlw	-2
	bz	SEQ_LAYER_SaveEvntL_6_2
	rgoto	SEQ_LAYER_SaveEvntL_End

SEQ_LAYER_SaveEvntL_6_0		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_6_0)
	;; length is constant value - don't store
	rgoto	SEQ_LAYER_SaveEvntL_End

SEQ_LAYER_SaveEvntL_6_2		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_6_2)
	;; length is constant value - don't store
	rgoto	SEQ_LAYER_SaveEvntL_End


SEQ_LAYER_SaveEvntL_7
	movf	SEQ_EVNT_NUMBER, W, BANKED	; only two events
	bz	SEQ_LAYER_SaveEvntL_7_0
	addlw	-1
	bz	SEQ_LAYER_SaveEvntL_7_1
	rgoto	SEQ_LAYER_SaveEvntL_End

SEQ_LAYER_SaveEvntL_7_0		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_7_0)
	movff	SEQ_EVNTL, MIOS_PARAMETER1	; Gate Length stored in Layer C
	movlw	2
	rcall	SEQ_LAYER_SetVValue
	rgoto	SEQ_LAYER_SaveEvntL_End

SEQ_LAYER_SaveEvntL_7_1		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_7_1)
	;; length is constant value - don't store
	rgoto	SEQ_LAYER_SaveEvntL_End


SEQ_LAYER_SaveEvntL_8
SEQ_LAYER_SaveEvntL_9
	movf	SEQ_EVNT_NUMBER, W, BANKED
	bz	SEQ_LAYER_SaveEvntL_9_0
	addlw	-1
	bz	SEQ_LAYER_SaveEvntL_9_1
	rgoto	SEQ_LAYER_SaveEvntL_9_2

SEQ_LAYER_SaveEvntL_8_0		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_8_0)
SEQ_LAYER_SaveEvntL_9_0		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_9_0)
	;; length is constant value - don't store
	rgoto	SEQ_LAYER_SaveEvntL_End

SEQ_LAYER_SaveEvntL_8_1		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_8_1)
SEQ_LAYER_SaveEvntL_9_1		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_9_1)
	;; length is constant value - don't store
	rgoto	SEQ_LAYER_SaveEvntL_End

SEQ_LAYER_SaveEvntL_8_2		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_8_2)
SEQ_LAYER_SaveEvntL_9_2		; (also used as direct jump target of SEQ_LAYER_SaveEvnt_9_2)
	;; length is constant value - don't store
	rgoto	SEQ_LAYER_SaveEvntL_End


SEQ_LAYER_SaveEvntL_10
	movff	SEQ_EVNTL, MIOS_PARAMETER1	; Gate Length stored in Layer C
	movlw	2
	rcall	SEQ_LAYER_SetVValue
	rgoto	SEQ_LAYER_SaveEvntL_End

SEQ_LAYER_SaveEvntL_11
	;; length is constant value - don't store
	rgoto	SEQ_LAYER_SaveEvntL_End

SEQ_LAYER_SaveEvntL_12		; not used (yet)
SEQ_LAYER_SaveEvntL_13		; not used (yet)
SEQ_LAYER_SaveEvntL_14		; not used (yet)
SEQ_LAYER_SaveEvntL_15		; not used (yet)
SEQ_LAYER_SaveEvntL_16		; not used (yet)
	rgoto	SEQ_LAYER_SaveEvntL_End

SEQ_LAYER_SaveEvntL_End
SEQ_LAYER_SaveEvnt_End
	return


;; --------------------------------------------------------------------------
;;  This function fetches a preset and copies it into the memory locations
;;  of the currently selected tracks
;; --------------------------------------------------------------------------
SEQ_LAYER_Init
	SET_BSR	SEQ_BASE

	;; go through all tracks
	clrf	SEQ_EVNTT, BANKED
SEQ_LAYER_InitLoop
	;; skip if track not selected
	call	CS_M_HLP_ChkTrkSelected
	bnz	SEQ_LAYER_InitLoop_Next

	;; init cfg data
	rcall	SEQ_LAYER_InitCfg

	;; initialize layers
	rcall	SEQ_LAYER_InitLayer

SEQ_LAYER_InitLoop_Next
	;; increment until last track is reached
	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_LAYER_InitLoop

	return



;; --------------------------------------------------------------------------
;;  This function only initializes the config area (also used by the CLR function
;;  in cs_m_util.inc)
;;  IN: track number in SEQ_EVNTT
;; --------------------------------------------------------------------------
SEQ_LAYER_InitCfg
	;; determine pointer to SEQ_TRKx
	lfsr	FSR0, SEQ_TRK0
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNTT, W, BANKED
	mullw	SEQ_TRKRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F

	;; go through static settings of preset table
	;; first byte: SEQ_TRKx parameter offset, second byte: value
	;; table terminates with -1
	TABLE_ADDR SEQ_LAYER_PRESET_TABLE_STATIC
SEQ_LAYER_InitCfg_I1
	tblrd*+			; read first byte
	movf	TABLAT, W	; get parameter index
	addlw	1		; check for -1 (first byte)
	bz	SEQ_LAYER_InitCfg_I1_Exit
	addlw	-1
	tblrd*+			; read value	
	movff	TABLAT, PLUSW0	; store value in SEQ_TRKx + index
	rgoto	SEQ_LAYER_InitCfg_I1
SEQ_LAYER_InitCfg_I1_Exit

	;; go through mode dependend settings of preset table
	;; first byte: SEQ_TRKx parameter offset, second+<mode> byte: value
	;; table terminates with -1
	setf	EEADR		; used as table entry counter
SEQ_LAYER_InitCfg_I2
	incf	EEADR, F
	movf	EEADR, W	; determine address offset
	TABLE_ADDR_MUL_W SEQ_LAYER_PRESET_TABLE_MODEx, SEQ_LAYER_PRESET_TABLE_MTABWIDTH

	tblrd*+			; read first byte
	movf	TABLAT, W	; get parameter index
	movwf	PRODL		; temporary store parameter index in PRODL
	addlw	1		; check for -1 (first byte)
	bz	SEQ_LAYER_InitCfg_I2_Exit

	movlw	SEQ_TRKEVNTx	; get event mode
	movf	PLUSW0, W
	andlw	0x0f
	TABLE_ADD_W		; add to table pointer
	
	tblrd*+			; read value
	
	;; branch if bit 7 of parameter index not set
	BRA_IFCLR PRODL, 7, ACCESS, SEQ_LAYER_InitCfg_I2_Norm
SEQ_LAYER_InitCfg_I2_Special
	BRA_IFSET PRODL, 1, ACCESS, SEQ_LAYER_InitCfg_I2_Special_23
SEQ_LAYER_InitCfg_I2_Special_01
	BRA_IFSET PRODL, 0, ACCESS, SEQ_LAYER_InitCfg_I2_Special_1
SEQ_LAYER_InitCfg_I2_Special_0
	;; 0x80: SEQ_TRKTRGA_[0-3]x (duplicate value 4 times)
	movlw	SEQ_TRKTRGA_0x
	rgoto	SEQ_LAYER_InitCfg_I2_Special_C
SEQ_LAYER_InitCfg_I2_Special_1
	;; 0x81: SEQ_TRKTRGB_[0-3]x (duplicate value 4 times)
	movlw	SEQ_TRKTRGB_0x
	rgoto	SEQ_LAYER_InitCfg_I2_Special_C
SEQ_LAYER_InitCfg_I2_Special_23
	;; 0x82: SEQ_TRKTRGC_[0-3]x (duplicate value 4 times)
	movlw	SEQ_TRKTRGC_0x
	;; 	rgoto	SEQ_LAYER_InitCfg_I2_Special_C
SEQ_LAYER_InitCfg_I2_Special_C

	;; store value 4 times
	movff	TABLAT, PLUSW0
	addlw	1
	movff	TABLAT, PLUSW0
	addlw	1
	movff	TABLAT, PLUSW0
	addlw	1
	movff	TABLAT, PLUSW0
	rgoto	SEQ_LAYER_InitCfg_I2
	
SEQ_LAYER_InitCfg_I2_Norm
	movf	PRODL, W
	movff	TABLAT, PLUSW0	; store value in SEQ_TRKx + index
	rgoto	SEQ_LAYER_InitCfg_I2
SEQ_LAYER_InitCfg_I2_Exit
	return

;; --------------------------------------------------------------------------
;;  This function only initializes the layers (also used by the CLR function
;;  in cs_m_util.inc)
;;  IN: track number in SEQ_EVNTT
;; --------------------------------------------------------------------------
SEQ_LAYER_InitLayer
	SET_BSR	SEQ_BASE

SEQ_LAYER_InitLayerLoop
	;; go through layer value table
	;; 4 entries per layer, they will be duplicated for all 32 steps per layer
	;; each mode has a package of 3*4 bytes
	movlw	SEQ_TRKEVNTx	; get event mode
	movf	PLUSW0, W
	andlw	0x0f
	TABLE_ADDR_MUL_W SEQ_LAYER_PRESET_TABLE_LAYERx, 3*4

	;; copy layers
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_A
	rcall	SEQ_LAYER_InitLayer_DupHlp	; add SEQ_EVNTT*0x20 and duplicate 8 times
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_B
	rcall	SEQ_LAYER_InitLayer_DupHlp	; add SEQ_EVNTT*0x20 and duplicate 8 times
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_C
	rcall	SEQ_LAYER_InitLayer_DupHlp	; add SEQ_EVNTT*0x20 and duplicate 8 times

	return



	;; help function which duplicates 4 bytes from a table 8 times into RAM (pointer in FSR1)
SEQ_LAYER_InitLayer_DupHlp
	movf	SEQ_EVNTT, W, BANKED
	mullw	0x20
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F

	clrf	PRODL		; outer loop counter (4 times)
SEQ_LAYER_InitLayer_DupHlpOLoop
	tblrd*+			; read value from table
	clrf	PRODH		; inner loop counter (8 times)
SEQ_LAYER_InitLayer_DupHlpILoop
	movf	PRODH, W
	movff	TABLAT, PLUSW1
	movlw	4		; add offset of 4
	addwf	PRODH, F	; until we reach 0x20
	BRA_IFCLR PRODH, 5, ACCESS, SEQ_LAYER_InitLayer_DupHlpILoop

	movf	POSTINC1, W	; select next byte offset
	incf	PRODL, F	; loop 4 times
	BRA_IFCLR PRODL, 2, ACCESS, SEQ_LAYER_InitLayer_DupHlpOLoop

	return
	
