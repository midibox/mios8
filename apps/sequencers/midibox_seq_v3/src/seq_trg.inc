; $Id$
;
; MIDIboxSEQ
; Step Trigger functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

SEQ_TRG_NAMES
	db	"Gate  " ; 0
	db	"Skip  " ; 1
	db	"Acc.  " ; 2
	db	"Glide " ; 3
	db	" Roll " ; 4
	db	" R.G  " ; 5
	db	" R.V  " ; 6
	db	"Spare " ; 7

;; --------------------------------------------------------------------------
;;  This function prints the trigger assignment type assigned to a layer
;;  IN: WREG: 0-7: trigger type
;;  OUT: 5 characters
;;  USES: BSR, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_TRG_PrnType
	;; print string from table
	mullw	6
	movlw	LOW(SEQ_TRG_NAMES)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(SEQ_TRG_NAMES)
	addwfc	PRODH, W
	movwf	TBLPTRH

	;; 5 characters
	movlw	5
	goto	MIOS_LCD_PrintPreconfString

;; --------------------------------------------------------------------------
;;  This function prints the assigned trigger(s) - if multiple triggers are
;;  assigned, it prints "Multi" - if layer is not assigned to any trigger,
;;  it prints "-----"
;;  IN: WREG: 0-2: layer A/B/C
;;      track in SEQ_EVNTT
;;  OUT: 5 characters
;;  USES: BSR, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_TRG_PrnAssigned_MULTI	db "Multi"
SEQ_TRG_PrnAssigned_NONE	db "-----"

SEQ_TRG_PrnAssignedType
	addlw	1
	movwf	TBLPTRH		; save layer A/B/C in TBLPTRH

	call	SEQ_FSR_TrkAssgn0	; get pointer to TRKASSGN0x register

	;; check if the same layer is assigned to multiple triggers
	clrf	PRODL		; using PRODL as loop counter
	clrf	PRODH		; using PRODH as trigger counter
	movlw	0x03
	movwf	TBLPTRL		; using TBLPTRL as mask
SEQ_TRG_PrnAssignedTypeLoop
	movf	POSTINC1, W	; compare with SEQ_TRKASSGN0x
	andwf	TBLPTRL, W
	xorwf	TBLPTRH, W
	bnz	SEQ_TRG_PrnAssignedTypeLoop_N0
SEQ_TRG_PrnAssignedTypeLoop_0
	incf	PRODH, F
	movf	PRODL, W	; save event type number in TABLAT
	movwf	TABLAT
SEQ_TRG_PrnAssignedTypeLoop_N0

	movf	POSTDEC1, W	; compare with SEQ_TRKASSGN1x
	andwf	TBLPTRL, W
	xorwf	TBLPTRH, W
	bnz	SEQ_TRG_PrnAssignedTypeLoop_N1
SEQ_TRG_PrnAssignedTypeLoop_1
	incf	PRODH, F
	movf	PRODL, W	; save event type number in TABLAT
	addlw	4
	movwf	TABLAT
SEQ_TRG_PrnAssignedTypeLoop_N1

	clrc			; leftshift mask two times
	rlf	TBLPTRL, F
	clrc
	rlf	TBLPTRL, F
	clrc			; leftshift compare value two times
	rlf	TBLPTRH, F
	clrc
	rlf	TBLPTRH, F
	incf	PRODL, F	; loop 4 times
	BRA_IFCLR PRODL, 2, ACCESS, SEQ_TRG_PrnAssignedTypeLoop

	;; now check what should be print out
	movf	PRODH, W
	bz	SEQ_TRG_PrnAssignedType_0
	addlw	-1
	bz	SEQ_TRG_PrnAssignedType_1
	rgoto	SEQ_TRG_PrnAssignedType_M

SEQ_TRG_PrnAssignedType_0
	TABLE_ADDR SEQ_TRG_PrnAssigned_NONE
	movlw	5		; print 5 characters
	goto	MIOS_LCD_PrintPreconfString

SEQ_TRG_PrnAssignedType_1
	movf	TABLAT, W	; event type number was saved in TABLAT
	rgoto	SEQ_TRG_PrnType

SEQ_TRG_PrnAssignedType_M
	TABLE_ADDR SEQ_TRG_PrnAssigned_MULTI
	movlw	5		; print 5 characters
	goto	MIOS_LCD_PrintPreconfString


;; --------------------------------------------------------------------------
;;  These "Get" functions return the assigned value (0 or 1) of a given
;;  function for the given step
;;  IN: step number in SEQ_EVNTS (0-31)
;;      track number in SEQ_EVNTT (0-15)
;;      trigger assignments in TRKASSGN[01]x
;;  OUT: 0 or 1
;; --------------------------------------------------------------------------
SEQ_TRG_GateGet
	call	SEQ_FSR_TrkAssgn0	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB10	; determine layer (located in SEQ_TRKASSGN0x[1:0]) 
	bnz	SEQ_TRG_Get_Lx		; if no assignment, gate always on!
	movlw	0x01
	andlw	0x01		; (fix ZERO flag)
	return

SEQ_TRG_SkipGet
	call	SEQ_FSR_TrkAssgn0	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB32	; determine layer (located in SEQ_TRKASSGN0x[3:2]) 
	rgoto	SEQ_TRG_Get_Lx		; branch to layer handler

SEQ_TRG_AccentGet
	call	SEQ_FSR_TrkAssgn0	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB54	; determine layer (located in SEQ_TRKASSGN0x[5:4]) 
	rgoto	SEQ_TRG_Get_Lx		; branch to layer handler

SEQ_TRG_GlideGet
	call	SEQ_FSR_TrkAssgn0	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB76	; determine layer (located in SEQ_TRKASSGN0x[7:6]) 
	rgoto	SEQ_TRG_Get_Lx		; branch to layer handler

SEQ_TRG_RollGet
	call	SEQ_FSR_TrkAssgn1	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB10	; determine layer (located in SEQ_TRKASSGN1x[1:0]) 
	rgoto	SEQ_TRG_Get_Lx		; branch to layer handler

SEQ_TRG_RandomGateGet
	call	SEQ_FSR_TrkAssgn1	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB32	; determine layer (located in SEQ_TRKASSGN1x[3:2]) 
	rgoto	SEQ_TRG_Get_Lx		; branch to layer handler

SEQ_TRG_RandomValueGet
	call	SEQ_FSR_TrkAssgn1	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB54	; determine layer (located in SEQ_TRKASSGN1x[5:4]) 
	rgoto	SEQ_TRG_Get_Lx		; branch to layer handler

SEQ_TRG_SpareGet		; not used yet, not selectable in Assignments menu...
	call	SEQ_FSR_TrkAssgn1	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB76	; determine layer (located in SEQ_TRKASSGN1x[7:6]) 
	rgoto	SEQ_TRG_Get_Lx		; branch to layer handler

;; help function which returns the assigned bit depending on the step
SEQ_TRG_Get_Lx
	bnz	SEQ_TRG_Get_LABC
SEQ_TRG_Get_L0
	andlw	0x00		; return 0
	return
SEQ_TRG_Get_LABC
	;; get pointer to SEQ_TRKTRG[ABC]_[0123] register
	rcall	SEQ_TRG_GetTPtr

	;; mask value
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNTS, W, BANKED
	andlw	0x07
	call	MIOS_HLP_GetBitORMask
	andwf	INDF1, W

	;; return either 0 or 1
	skpz
	movlw	0x01

	;; fix ZERO flag and exit
	andlw	0x01
	return


;; --------------------------------------------------------------------------
;;  This "Get" function returns a byte which contains the current state of
;;  all triggers
;;  IN: step number in SEQ_EVNTS (0-31)
;;      track number in SEQ_EVNTT (0-15)
;;  OUT: 8 bits in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
SEQ_TRG_AllGet
	clrf	MIOS_PARAMETER1

	rcall	SEQ_TRG_GateGet
	skpz
	bsf	MIOS_PARAMETER1, 0

	rcall	SEQ_TRG_SkipGet
	skpz
	bsf	MIOS_PARAMETER1, 1

	rcall	SEQ_TRG_AccentGet
	skpz
	bsf	MIOS_PARAMETER1, 2

	rcall	SEQ_TRG_GlideGet
	skpz
	bsf	MIOS_PARAMETER1, 3

	rcall	SEQ_TRG_RollGet
	skpz
	bsf	MIOS_PARAMETER1, 4

	rcall	SEQ_TRG_RandomGateGet
	skpz
	bsf	MIOS_PARAMETER1, 5

	rcall	SEQ_TRG_RandomValueGet
	skpz
	bsf	MIOS_PARAMETER1, 6

	rcall	SEQ_TRG_SpareGet
	skpz
	bsf	MIOS_PARAMETER1, 7

	movf	MIOS_PARAMETER1, W
	return

;; --------------------------------------------------------------------------
;;  These "Set" functions change the assigned value (0 or 1) to a given
;;  function for the given step
;;  IN: value in WREG (0 or 1)
;;      step number in SEQ_EVNTS (0-31)
;;      track number in SEQ_EVNTT (0-15)
;;      trigger assignments in TRKASSGN[01]x
;;  OUT: -
;; --------------------------------------------------------------------------
SEQ_TRG_GateSet
	movwf	TABLAT			; temporary store value in TABLAT
	call	SEQ_FSR_TrkAssgn0	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB10	; determine layer (located in SEQ_TRKASSGN0x[1:0]) 
	bnz	SEQ_TRG_Set_Lx		; if no assignment, gate always on!
	movlw	0x01
	andlw	0x01		; (fix ZERO flag)
	return

SEQ_TRG_SkipSet
	movwf	TABLAT			; temporary store value in TABLAT
	call	SEQ_FSR_TrkAssgn0	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB32	; determine layer (located in SEQ_TRKASSGN0x[3:2]) 
	rgoto	SEQ_TRG_Set_Lx		; branch to layer handler

SEQ_TRG_AccentSet
	movwf	TABLAT			; temporary store value in TABLAT
	call	SEQ_FSR_TrkAssgn0	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB54	; determine layer (located in SEQ_TRKASSGN0x[5:4]) 
	rgoto	SEQ_TRG_Set_Lx		; branch to layer handler

SEQ_TRG_GlideSet
	movwf	TABLAT			; temporary store value in TABLAT
	call	SEQ_FSR_TrkAssgn0	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB76	; determine layer (located in SEQ_TRKASSGN0x[7:6]) 
	rgoto	SEQ_TRG_Set_Lx		; branch to layer handler

SEQ_TRG_RollSet
	movwf	TABLAT			; temporary store value in TABLAT
	call	SEQ_FSR_TrkAssgn1	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB10	; determine layer (located in SEQ_TRKASSGN1x[1:0]) 
	rgoto	SEQ_TRG_Set_Lx		; branch to layer handler

SEQ_TRG_RandomGateSet
	movwf	TABLAT			; temporary store value in TABLAT
	call	SEQ_FSR_TrkAssgn1	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB32	; determine layer (located in SEQ_TRKASSGN1x[3:2]) 
	rgoto	SEQ_TRG_Set_Lx		; branch to layer handler

SEQ_TRG_RandomValueSet
	movwf	TABLAT			; temporary store value in TABLAT
	call	SEQ_FSR_TrkAssgn1	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB54	; determine layer (located in SEQ_TRKASSGN1x[5:4]) 
	rgoto	SEQ_TRG_Set_Lx		; branch to layer handler

SEQ_TRG_SpareSet		; not used yet, not selectable in Assignments menu...
	movwf	TABLAT			; temporary store value in TABLAT
	call	SEQ_FSR_TrkAssgn1	; get pointer to assignments register -> FSR1
	rcall	SEQ_TRG_Hlp_GetLB76	; determine layer (located in SEQ_TRKASSGN1x[7:6]) 
	rgoto	SEQ_TRG_Set_Lx		; branch to layer handler

;; --------------------------------------------------------------------------
;;  This help function changes flag of the given trigger layer
;;  IN: value in MIOS_PARAMETER1 (0 or 1)
;;      step number in SEQ_EVNTS (0-31)
;;      track number in SEQ_EVNTT (0-15)
;;      trigger layer (0-2) in WREG
;;  OUT: -
;; --------------------------------------------------------------------------
SEQ_TRG_SetLayer
	movff	MIOS_PARAMETER1, TABLAT
	addlw	1	; 0/1/2 -> A/B/C
	andlw	0x03		; trigger layer - fix ZERO flag
	;; 	rgoto	SEQ_TRG_Set_Lx	


;; help function which sets the assigned bit depending on the step
;; also used by SEQ_TRK_xxxSet functions!
SEQ_TRG_Set_Lx
	bnz	SEQ_TRG_Set_LABC
SEQ_TRG_Set_L0
	return			; ignore
SEQ_TRG_Set_LABC
	;; get pointer to SEQ_TRKTRG[ABC]_[0123] register
	rcall	SEQ_TRG_GetTPtr

	;; set/clear bit depending on TABLAT (temporary store register for trigger)
	SET_BSR	SEQ_BASE
	movf	TABLAT, W
	bnz	SEQ_TRG_Set_L_Set

SEQ_TRG_Set_L_Clr
	movf	SEQ_EVNTS, W, BANKED
	andlw	0x07
	call	MIOS_HLP_GetBitANDMask
	andwf	INDF1, F
	return

SEQ_TRG_Set_L_Set
	movf	SEQ_EVNTS, W, BANKED
	andlw	0x07
	call	MIOS_HLP_GetBitORMask
	iorwf	INDF1, F
	return


;; --------------------------------------------------------------------------
;;  This "Set" function copies the 8 bits of a byte into the trigger arrays
;;  IN: step number in SEQ_EVNTS (0-31)
;;      track number in SEQ_EVNTT (0-15)
;;      byte in WREG
;;  OUT: -
;; --------------------------------------------------------------------------
SEQ_TRG_AllSet
	movwf	MIOS_PARAMETER1	; temporary store byte in MIOS_PARAMETER1

	movlw	0
	btfsc	MIOS_PARAMETER1, 0
	movlw 1
	rcall	SEQ_TRG_GateSet

	movlw	0
	btfsc	MIOS_PARAMETER1, 1
	movlw 1
	rcall	SEQ_TRG_SkipSet

	movlw	0
	btfsc	MIOS_PARAMETER1, 2
	movlw 1
	rcall	SEQ_TRG_AccentSet

	movlw	0
	btfsc	MIOS_PARAMETER1, 3
	movlw 1
	rcall	SEQ_TRG_GlideSet

	movlw	0
	btfsc	MIOS_PARAMETER1, 4
	movlw 1
	rcall	SEQ_TRG_RollSet

	movlw	0
	btfsc	MIOS_PARAMETER1, 5
	movlw 1
	rcall	SEQ_TRG_RandomGateSet

	movlw	0
	btfsc	MIOS_PARAMETER1, 6
	movlw 1
	rcall	SEQ_TRG_RandomValueSet

	movlw	0
	btfsc	MIOS_PARAMETER1, 7
	movlw 1
	rgoto	SEQ_TRG_SpareSet


;; --------------------------------------------------------------------------
;;  Help functions which return a bit field from INDF1
;;  IN: pointer to register in FSR1
;;  OUT: 2 bits, copied to WREG[1:0]
;; --------------------------------------------------------------------------
SEQ_TRG_Hlp_GetLB10
	movf	INDF1, W
	andlw	0x03
	return

SEQ_TRG_Hlp_GetLB32
	rrf	INDF1, W
	rrf	WREG, W
	andlw	0x03
	return

SEQ_TRG_Hlp_GetLB54
	swapf	INDF1, W
	andlw	0x03
	return

SEQ_TRG_Hlp_GetLB76
	swapf	INDF1, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x03
	return


;; --------------------------------------------------------------------------
;;  This help function returns the pointer to SEQ_TRKTRG[ABC]_[0123]x
;;  IN: Layer 1-3 (A-C) in WREG
;;      step number (0-31) in SEQ_EVNTS
;;  OUT: pointer to trigger register in FSR1
;; --------------------------------------------------------------------------
SEQ_TRG_GetTPtr
	addlw	-1
	bz	SEQ_TRG_GetTPtr_A
	addlw	-1
	bz	SEQ_TRG_GetTPtr_B
	rgoto	SEQ_TRG_GetTPtr_C
	
SEQ_TRG_GetTPtr_A
	call	SEQ_FSR_TrkTrgA_0	; pointer to layer A
	rgoto	SEQ_TRG_GetTPtr_Cont
SEQ_TRG_GetTPtr_B
	call	SEQ_FSR_TrkTrgB_0	; pointer to layer B
	rgoto	SEQ_TRG_GetTPtr_Cont
SEQ_TRG_GetTPtr_C
	call	SEQ_FSR_TrkTrgC_0	; pointer to layer C
	;; 	rgoto	SEQ_TRG_GetTPtr_Cont
SEQ_TRG_GetTPtr_Cont
	;; add offset depending on step number
	SET_BSR	SEQ_BASE
	rlf	SEQ_EVNTS, W, BANKED
	swapf	WREG, W
	andlw	0x03
	addwf	FSR1L, F
	return


;; --------------------------------------------------------------------------
;;  This function returns != 0 if a trigger function is assigned to the layer,
;;  otherwise it returns 0
;;  IN: track in SEQ_EVNTT
;;      trigger layer in WREG (0-2)
;;  OUT: 0 (ZERO flag set) if no function assigned to trigger layer
;;  USES: FSR1[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_TRG_GetTAssigned
	;; determine compare mask
	addlw	1
	movwf	TABLAT

	;; check the 4 fields of ASSGN0
	call	SEQ_FSR_TrkAssgn0
	rcall	SEQ_TRG_GetTAssigned_Hlp
	skpz
	return			; result already != 0

	;; check the 4 fields of ASSGN1
	call	SEQ_FSR_TrkAssgn1
	;; 	rgoto	SEQ_TRG_GetTAssigned_Hlp


SEQ_TRG_GetTAssigned_Hlp
	clrf	PRODL		; loop 4 times
	movf	INDF1, W	; value to be shifted
	movwf	PRODH
SEQ_TRG_GetTAssigned_HlpLoop
	movf	PRODH, W
	xorwf	TABLAT, W
	andlw	0x03
	bz	SEQ_TRG_GetTAssigned_Hlp_Match

	rrf	PRODH, F	; leftshift 2 times
	rrf	PRODH, F

	incf	PRODL, F
	BRA_IFCLR PRODL, 2, ACCESS, SEQ_TRG_GetTAssigned_HlpLoop

	;; not found
	andlw	0x00
	return

SEQ_TRG_GetTAssigned_Hlp_Match
	;; found
	iorlw	0xff
	return
