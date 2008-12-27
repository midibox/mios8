; $Id$
;
; MIDIbox CV
; Map the internal CV voices to the AOUT channels
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_RequestUpdateAll
;;  DESCRIPTION: requests an update of all CV outputs and gates
;;  OUT:  -
;;  USES: PRODL
;; --------------------------------------------------------------------------
CV_MAP_RequestUpdateAll
	SET_BSR	CV_BASE

	;; update all CV outputs
	lfsr	FSR0, CV_C1_STAT
	clrf	PRODL
CV_MAP_RequestUpdateAll_Loop
	movf	PRODL, W
	bsf	PLUSW0, CV_STAT_CV_UPDATE_REQ
	incf	PRODL, F
	movlw	8 - 1
	cpfsgt	PRODL, ACCESS
	rgoto CV_MAP_RequestUpdateAll_Loop

	;; ensure that gates will be updated
	SET_BSR GATES
	comf	GATES, W, BANKED
	movwf	GATES_LAST, BANKED

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_Update
;;  DESCRIPTION: maps the internal CV voices to the AOUT channels
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
CV_MAP_Update
	SET_BSR	CV_BASE

	clrf	CV_CURRENT_CHANNEL, BANKED
CV_MAP_Update_Loop
	lfsr	FSR0, CV_C1_STAT
	movf	CV_CURRENT_CHANNEL, W, BANKED
CV_MAP_Update_Loop_GateClr
	BRA_IFCLR PLUSW0, CV_STAT_GATE_CLR_REQ, ACCESS, CV_MAP_Update_Loop_NoGateClr
	bcf	PLUSW0, CV_STAT_GATE_CLR_REQ

	movf	CV_CURRENT_CHANNEL, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	SET_BSR	GATES
	andwf	GATES, F, BANKED
	SET_BSR	CV_BASE
	rgoto	CV_MAP_Update_Loop_Gate_End
CV_MAP_Update_Loop_NoGateClr

CV_MAP_Update_Loop_GateSet
	BRA_IFCLR PLUSW0, CV_STAT_GATE_SET_REQ, ACCESS, CV_MAP_Update_Loop_NoGateSet
	bcf	PLUSW0, CV_STAT_GATE_SET_REQ

	movf	CV_CURRENT_CHANNEL, W, BANKED
	call	MIOS_HLP_GetBitORMask
	SET_BSR	GATES
	iorwf	GATES, F, BANKED
	SET_BSR	CV_BASE
	;;	rgoto	CV_MAP_Update_Loop_Gate_End
CV_MAP_Update_Loop_NoGateSet
CV_MAP_Update_Loop_Gate_End

CV_MAP_Update_Loop_CV
	movf	CV_CURRENT_CHANNEL, W, BANKED
	BRA_IFCLR PLUSW0, CV_STAT_CV_UPDATE_REQ, ACCESS, CV_MAP_Update_Loop_CV_End
	bcf	PLUSW0, CV_STAT_CV_UPDATE_REQ

	rcall	CV_MAP_Hlp_GetCV

	;; disable IRQs to ensure consistency
	IRQ_DISABLE

	;; store new AOUT value in patch
	lfsr	FSR0, CV_C1_AOUT_L
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	CV_AOUT_L, PLUSW0
	addlw	8
	movff	CV_AOUT_H, PLUSW0

	;; forward AOUT value to interface if interpolation not enabled for channel
	;; (interpolator is doing this instead in cv_ip.inc)
	lfsr	FSR0, CV_C1_SLEW_RATE
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movf	PLUSW0, W
	bnz	CV_MAP_Update_Loop_NoAOUT
CV_MAP_Update_Loop_AOUT
	swapf	CV_AOUT_H, W, BANKED
	andlw	0xf0
	movwf	MIOS_PARAMETER2
	swapf	CV_AOUT_L, W, BANKED
	andlw	0x0f
	iorwf	MIOS_PARAMETER2, F
	swapf	CV_AOUT_L, W, BANKED
	andlw	0xf0
	movwf	MIOS_PARAMETER1
	movf	CV_CURRENT_CHANNEL, W, BANKED
	call	AOUT_Pin16bitSet
	SET_BSR	CV_BASE
CV_MAP_Update_Loop_NoAOUT

	;; enable IRQs again
	IRQ_ENABLE
CV_MAP_Update_Loop_CV_End
	
CV_MAP_Update_Loop_Next
	incf	CV_CURRENT_CHANNEL, F, BANKED
	BRA_IFCLR CV_CURRENT_CHANNEL, 3, BANKED, CV_MAP_Update_Loop

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_Hlp_GetCV
;;  DESCRIPTION: help function to get CV value depending on selected mode
;;  IN: voice number (0-7) in CV_CURRENT_CHANNEL
;;  OUT:  value in CV_AOUT_[LH]
;;  USES: TMP[12345], FSR[01]
;; --------------------------------------------------------------------------
CV_MAP_Hlp_GetCV
	;; change CV to special values in calibration mode
	BRA_IFCLR MB_STAT, MB_STAT_CALIBRATION_MODE, ACCESS, CV_MAP_Hlp_GetCV_NoCalib
	movf	CV_CURRENT_CHANNEL, W, BANKED
	cpfseq	CV_SELECTED_OUT, BANKED
	rgoto CV_MAP_Hlp_GetCV_NoCalib
	rgoto	CV_MAP_Hlp_Calib

CV_MAP_Hlp_GetCV_NoCalib
	lfsr	FSR0, CV_C1_MIDI_MODE
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movf	PLUSW0, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CV_MAP_Hlp_Note
	rgoto	CV_MAP_Hlp_Vel
	rgoto	CV_MAP_Hlp_ATch
	rgoto	CV_MAP_Hlp_CC
	rgoto	CV_MAP_Hlp_NRPN
	rgoto	CV_MAP_Hlp_PBend
	rgoto	CV_MAP_Hlp_Note		; dummy
	rgoto	CV_MAP_Hlp_Note		; dummy


;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_Hlp_RestoreVal
;;  DESCRIPTION: help function to restore CV value depending on selected mode
;;  IN: voice number (0-7) in CV_CURRENT_CHANNEL, value in CV_AOUT_[LH]
;;  USES: TMP[12345], FSR[01]
;; --------------------------------------------------------------------------
CV_MAP_Hlp_RestoreVal
	lfsr	FSR0, CV_C1_MIDI_MODE
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movf	PLUSW0, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CV_MAP_Hlp_RestoreNote
	rgoto	CV_MAP_Hlp_RestoreVel
	rgoto	CV_MAP_Hlp_RestoreATch
	rgoto	CV_MAP_Hlp_RestoreCC
	rgoto	CV_MAP_Hlp_RestoreNRPN
	rgoto	CV_MAP_Hlp_RestorePBend
	rgoto	CV_MAP_Hlp_RestoreNote		; dummy
	rgoto	CV_MAP_Hlp_RestoreNote		; dummy

;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_Hlp_Note
;;  DESCRIPTION: help function which handles the note/pitchbender value
;;  IN: voice number (0-7) in CV_CURRENT_CHANNEL
;;  OUT:  value in CV_AOUT_[LH]
;;  USES: TMP[12345], FSR[01]
;; --------------------------------------------------------------------------
CV_MAP_Hlp_Note
	SET_BSR	CV_BASE

	;; get note
	lfsr	FSR0, CV_C1_NOTE
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PLUSW0, TMP1

	;; transpose octave
CV_MAP_Hlp_Note_TOct
	lfsr	FSR0, CV_C1_TRANSP_OCT
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movf	PLUSW0, W
	bz	CV_MAP_Hlp_Note_NoTOct
	andlw	0x0f
	btfsc	WREG, 3
	iorlw 0xf0
	mullw	12		; * octave
	movf	PRODL, W
	addwf	TMP1, F		; add to note
	btfsc	TMP1, 7		; zero note on overrun
	clrf	TMP1
CV_MAP_Hlp_Note_NoTOct

	;; transpose semitones
CV_MAP_Hlp_Note_TSem
	lfsr	FSR0, CV_C1_TRANSP_SEM
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movf	PLUSW0, W
	bz	CV_MAP_Hlp_Note_NoTSem
	andlw	0x0f
	btfsc	WREG, 3
	iorlw 0xf0
	addwf	TMP1, F		; add to note
	btfsc	TMP1, 7		; zero note on overrun
	clrf	TMP1
CV_MAP_Hlp_Note_NoTSem

	;; convert note to 12bit value depending on output curve
	movf	TMP1, W
	call	CV_MAP_Hlp_ConvCurve

	;; add pitchbender
	lfsr	FSR0, CV_C1_PITCHBENDER_H
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movf	PLUSW0, W
	movwf	TMP2
	bz	CV_MAP_Hlp_Note_NoPitchB
CV_MAP_Hlp_Note_PitchB
	movwf	MUL_A_H
	lfsr	FSR0, CV_C1_PITCHBENDER_L
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PLUSW0, MUL_A_L

	BRA_IFCLR TMP2, 7, ACCESS, CV_MAP_Hlp_Note_PitchB_Pos
CV_MAP_Hlp_Note_PitchB_Neg
	comf	MUL_A_L, F
	comf	MUL_A_H, F
	bcf	MUL_A_H, 7
CV_MAP_Hlp_Note_PitchB_Pos

	;; determine pitchrange
	lfsr	FSR0, CV_C1_PITCHRANGE
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movf	PLUSW0, W
	bz	CV_MAP_Hlp_Note_NoPitchB
	mullw	64
	movff	PRODL, MUL_B_L
	movff	PRODH, MUL_B_H

	;; do a 16x16 bit multiplication
	call	MATH_MUL16_16
	;; result in MUL_R3:MUL_R0

	;; add/subtract result
	SET_BSR	CV_BASE
	BRA_IFSET TMP2, 7, ACCESS, CV_MAP_Hlp_Note_Pitch_Dec
CV_MAP_Hlp_Note_Pitch_Inc
	movf	MUL_R_2, W
	addwf	CV_AOUT_L, F, BANKED
	movf	MUL_R_3, W
	addwfc	CV_AOUT_H, F, BANKED
	rgoto	CV_MAP_Hlp_Note_Pitch_Cont
CV_MAP_Hlp_Note_Pitch_Dec
	movf	MUL_R_2, W
	subwf	CV_AOUT_L, F, BANKED
	movf	MUL_R_3, W
	subwfb	CV_AOUT_H, F, BANKED
	;; 	rgoto	CV_MAP_Hlp_Note_Pitch_Cont
CV_MAP_Hlp_Note_Pitch_Cont
	
CV_MAP_Hlp_Note_NoPitchB
	return


;; --------------------------------------------------------------------------
;; Restore Function
CV_MAP_Hlp_RestoreNote
	lfsr	FSR0, CV_C1_NOTE
	rgoto	CV_MAP_Hlp_Restore_7bit


;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_Hlp_Vel
;;  DESCRIPTION: help function which returns the velocity
;;  IN: voice number (0-7) in CV_CURRENT_CHANNEL
;;  OUT:  value in CV_AOUT_[LH]
;;  USES: TMP[12345], FSR[01]
;; --------------------------------------------------------------------------
CV_MAP_Hlp_Vel
	;; convert 7bit velocity value to 12bit
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_VELOCITY
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movf	PLUSW0, W
	rgoto	CV_MAP_Hlp_ConvCurve

;; --------------------------------------------------------------------------
;; Restore Function
CV_MAP_Hlp_RestoreVel
	lfsr	FSR0, CV_C1_VELOCITY
	rgoto	CV_MAP_Hlp_Restore_7bit


;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_Hlp_ATch
;;  DESCRIPTION: help function which returns the aftertouch value
;;  IN: voice number (0-7) in CV_CURRENT_CHANNEL
;;  OUT:  value in CV_AOUT_[LH]
;;  USES: TMP[12345], FSR[01]
;; --------------------------------------------------------------------------
CV_MAP_Hlp_ATch
	;; convert 7bit aftertouch value to 12bit
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_AFTERTOUCH
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movf	PLUSW0, W
	rgoto	CV_MAP_Hlp_ConvCurve

;; --------------------------------------------------------------------------
;; Restore Function
CV_MAP_Hlp_RestoreATch
	lfsr	FSR0, CV_C1_AFTERTOUCH
	rgoto	CV_MAP_Hlp_Restore_7bit

;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_Hlp_CC
;;  DESCRIPTION: help function which returns the CC value
;;  IN: voice number (0-7) in CV_CURRENT_CHANNEL
;;  OUT:  value in CV_AOUT_[LH]
;;  USES: TMP[12345], FSR[01]
;; --------------------------------------------------------------------------
CV_MAP_Hlp_CC
	;; convert 7bit CC value to 12bit
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_CC_H
	movf	CV_CURRENT_CHANNEL, W, BANKED
	rrf	PLUSW0, W
	andlw	0x7f
	rgoto	CV_MAP_Hlp_ConvCurve

;; --------------------------------------------------------------------------
;; Restore Function
CV_MAP_Hlp_RestoreCC
	SET_BSR	CV_BASE

	lfsr	FSR0, CV_C1_CC_H
	swapf	CV_AOUT_H, W, BANKED
	andlw	0xf0
	movwf	PRODH
	swapf	CV_AOUT_L, W, BANKED
	andlw	0x0f
	iorwf	PRODH, F
	swapf	CV_AOUT_L, W, BANKED
	andlw	0xf0
	movwf	PRODL

	lfsr	FSR0, CV_C1_CC_L
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PRODL, PLUSW0
	lfsr	FSR0, CV_C1_CC_H
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PRODH, PLUSW0
	return

	
;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_Hlp_NRPN
;;  DESCRIPTION: help function which returns the NRPN value
;;  IN: voice number (0-7) in CV_CURRENT_CHANNEL
;;  OUT:  value in CV_AOUT_[LH]
;;  USES: TMP[12345], FSR[01]
;; --------------------------------------------------------------------------
CV_MAP_Hlp_NRPN
	;; convert 16bit CC value to 12bit
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_CC_L
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PLUSW0, TMP1
	lfsr	FSR0, CV_C1_CC_H
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PLUSW0, TMP2

	swapf	TMP2, W
	andlw	0x0f
	movwf	CV_AOUT_H, BANKED

	swapf	TMP1, W
	andlw	0x0f
	movwf	TMP1
	swapf	TMP2, W
	andlw	0xf0
	iorwf	TMP1, W
	movwf	CV_AOUT_L, BANKED
	return

;; --------------------------------------------------------------------------
;; Restore Function
CV_MAP_Hlp_RestoreNRPN
	rgoto	CV_MAP_Hlp_RestoreCC


;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_Hlp_PBend
;;  DESCRIPTION: help function which returns the pitchbender value
;;  IN: voice number (0-7) in CV_CURRENT_CHANNEL
;;  OUT:  value in CV_AOUT_[LH]
;;  USES: TMP[12345], FSR[01]
;; --------------------------------------------------------------------------
CV_MAP_Hlp_PBend
	;; convert signed 16bit PitchBender value to unsigned 12bit
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_PITCHBENDER_L
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PLUSW0, TMP1
	lfsr	FSR0, CV_C1_PITCHBENDER_H
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PLUSW0, TMP2

	swapf	TMP2, W
	xorlw	0x08
	andlw	0x0f
	movwf	CV_AOUT_H, BANKED

	swapf	TMP1, W
	andlw	0x0f
	movwf	TMP1
	swapf	TMP2, W
	andlw	0xf0
	iorwf	TMP1, W
	movwf	CV_AOUT_L, BANKED
	return

;; --------------------------------------------------------------------------
;; Restore Function
CV_MAP_Hlp_RestorePBend
	;; todo
	clrf	PRODL
	movlw	0x80
	movwf	PRODH

	lfsr	FSR0, CV_C1_PITCHBENDER_L
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PRODL, PLUSW0
	lfsr	FSR0, CV_C1_PITCHBENDER_H
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PRODH, PLUSW0
	return



;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_Hlp_Restore_7bit
;;  DESCRIPTION: help function to restore 7bit value from 12bit value
;;  IN: voice number (0-7) in CV_CURRENT_CHANNEL, pointer to CV_Cx_* in FSR0
;;      12bit value in CV_AOUT_[LH]
;;  USES: FSR0, PRODL
;; --------------------------------------------------------------------------
CV_MAP_Hlp_Restore_7bit
	SET_BSR	CV_BASE

	;; 12bit -> 7bit
	swapf	CV_AOUT_L, W
	rrf	WREG, W
	andlw	0x07
	movwf	PRODL
	swapf	CV_AOUT_H, W
	rrf	WREG, W
	andlw	0x78
	iorwf	PRODL, F

	;; store in array
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PRODL, PLUSW0
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_Hlp_Cali
;;  DESCRIPTION: help function which returns the calibration value
;;  IN: voice number (0-7) in CV_CURRENT_CHANNEL
;;  OUT:  value in CV_AOUT_[LH]
;;  USES: TMP[12345], FSR[01]
;; --------------------------------------------------------------------------
CV_MAP_Hlp_CALIB_TABLE
	; C_0, 1V in V/Oct mode
	; C-1, 2V in V/Oct mode
	; C-3, 4V in V/Oct mode
	; C-7, 8V in V/Oct mode
	db	0x0c, 0x18, 0x30, 0x60

	; C-2, 1V in V/Oct mode
	; C-3, 2V in V/Oct mode
	; C-4, 4V in V/Oct mode
	; C-5, 8V in V/Oct mode
	db	0x24, 0x30, 0x3c, 0x48

CV_MAP_Hlp_Calib
	SET_BSR	CV_BASE
	movf	CV_CALIB_VALUE, W
	bz	CV_MAP_Hlp_Calib_Min
	addlw	-1
	bz	CV_MAP_Hlp_Calib_Mid
	addlw	-1
	bz	CV_MAP_Hlp_Calib_Max
	addlw	-1
	rgoto	CV_MAP_Hlp_Calib_Tab

CV_MAP_Hlp_Calib_Min
	clrf	CV_AOUT_L, BANKED
	clrf	CV_AOUT_H, BANKED
	return	

CV_MAP_Hlp_Calib_Mid
	clrf	CV_AOUT_L, BANKED
	movlw	0x08
	movwf	CV_AOUT_H, BANKED
	return	

CV_MAP_Hlp_Calib_Max
	movlw	0xff
	movwf	CV_AOUT_L, BANKED
	movlw	0x0f
	movwf	CV_AOUT_H, BANKED
	return	

CV_MAP_Hlp_Calib_Tab
	;; get note number from table, convert it to AOUT value
	movwf	PRODL
	TABLE_ADDR CV_MAP_Hlp_CALIB_TABLE

	;; if selected curve == 1, switch to alternative note numbers
	lfsr	FSR1, CV_C1_CURVE
	movf	CV_CURRENT_CHANNEL, W
	decf	PLUSW1, W
	skpnz
	bsf	PRODL, 2

	movf	PRODL, W
	TABLE_ADD_W
	tblrd*+
	movf	TABLAT, W
	rgoto	CV_MAP_Hlp_ConvCurve


;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_Hlp_ConvCurve
;;  DESCRIPTION: help function to convert a 7bit value to 12 bit, takes the
;;  selected output curve into account!
;;  IN: voice number (0-7) in CV_CURRENT_CHANNEL, value in WREG
;;  OUT:  value in CV_AOUT_[LH]
;;  USES: FSR1, TBLPTR[LH], TABLAT
;; --------------------------------------------------------------------------
CV_MAP_Hlp_ConvCurve
	movwf	PRODL		; temporary save 7bit value in PRODL

	lfsr	FSR1, CV_C1_CURVE
	movf	CV_CURRENT_CHANNEL, W
	movf	PLUSW1, W
	bz	CV_MAP_Hlp_ConvCurve_0
	addlw	-1
	bz	CV_MAP_Hlp_ConvCurve_1
	addlw	-1
	bz	CV_MAP_Hlp_ConvCurve_2
	addlw	-1
	bz	CV_MAP_Hlp_ConvCurve_3
	;; else use curve_0

	;; linear curve, just calculate the output value: 7bit value << 5
CV_MAP_Hlp_ConvCurve_0
	movf	PRODL, W
	mullw	32
	movff	PRODL, CV_AOUT_L
	movff	PRODH, CV_AOUT_H
	return

	;; take output value from table 1
CV_MAP_Hlp_ConvCurve_1
	TABLE_ADDR CV_CURVE1_TABLE
	rgoto	CV_MAP_Hlp_ConvCurve_Cont

	;; take output value from table 2
CV_MAP_Hlp_ConvCurve_2
	TABLE_ADDR CV_CURVE2_TABLE
	rgoto	CV_MAP_Hlp_ConvCurve_Cont

	;; take output value from table 3
CV_MAP_Hlp_ConvCurve_3
	TABLE_ADDR CV_CURVE3_TABLE
	rgoto	CV_MAP_Hlp_ConvCurve_Cont

CV_MAP_Hlp_ConvCurve_Cont
	clrc
	rlf	PRODL, W
	addwf	TBLPTRL, F
	movlw	0x00
	addwfc	TBLPTRH, F
	tblrd*+
	movff	TABLAT, CV_AOUT_L
	tblrd*+
	movff	TABLAT, CV_AOUT_H
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: CV_MAP_Refresh
;;  DESCRIPTION: maps the stored AOUT value from patch structure to CV structure
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
CV_MAP_Refresh
	;; clear gates
	SET_BSR	GATES
	clrf	GATES, BANKED

	SET_BSR	CV_BASE

	;; restore note/velocity/CC/NRPN/PitchWheel values from patch
	clrf	CV_CURRENT_CHANNEL, BANKED
CV_MAP_Refresh_Loop
	;; get AOUT value from patch
	lfsr	FSR0, CV_C1_AOUT_L
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PLUSW0, CV_AOUT_L
	addlw	8
	movff	PLUSW0, CV_AOUT_H

	;; restore depending on mode
	rcall	CV_MAP_Hlp_RestoreVal

	;; for animated meters
	lfsr	FSR0, CS_MENU_METER_CTR_BEGIN
	movf	CV_CURRENT_CHANNEL, W, BANKED
	setf	PLUSW0

	;; loop 8 times
	incf	CV_CURRENT_CHANNEL, F, BANKED
	BRA_IFCLR CV_CURRENT_CHANNEL, 3, BANKED, CV_MAP_Refresh_Loop

	;; refresh AOUTs
	rgoto	CV_MAP_RequestUpdateAll
