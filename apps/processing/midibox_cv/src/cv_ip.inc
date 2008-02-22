; $Id$
;
; MIDIbox CV
; Interpolation Handler (for smoothing CV outputs)
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;; Called from USER_Timer to interpolate CV outputs
;; --------------------------------------------------------------------------
CV_IP_Handler
	;; run loop
	clrf	IRQ_TMP5
	lfsr	FSR1, CVIP1_BASE
CV_IP_Handler_Loop
	;; skip if slew rate is 0
	lfsr	FSR2, CV_C1_SLEW_RATE
	movf	IRQ_TMP5, W
	movf	PLUSW2, W
	bz	CV_IP_Handler_Loop_Next
	movwf	IRQ_TMP3		; slew rate -> IRQ_TMP3

	;; get target values
	lfsr	FSR2, CV_C1_AOUT_L
	movf	IRQ_TMP5, W
	movff	PLUSW2, IRQ_TMP1	; CV_Cx_AOUT_L -> IRQ_TMP1
	addlw	8
	movff	PLUSW2, IRQ_TMP2	; CV_Cx_AOUT_H -> IRQ_TMP2

	;; do interpolation
 	rcall	CV_IP_Sub

	;; check if value has changed
	clrc
	rlf	IRQ_TMP5, W
	movwf	TABLAT

	lfsr	FSR2, AOUT0_L
	movf	TABLAT, W
	movf	PLUSW2, W
	xorwf	IRQ_TMP1, W
	bnz	CV_IP_Handler_Loop_Change
	incf	TABLAT, W
	movf	PLUSW2, W
	andlw	0x0f
	xorwf	IRQ_TMP2, W
	bz	CV_IP_Handler_Loop_NoChange
CV_IP_Handler_Loop_Change
	;; copy to AOUTx_[LH] register
	movf	TABLAT, W
	movff	IRQ_TMP1, PLUSW2
	incf	TABLAT, W
	movff	IRQ_TMP2, PLUSW2
CV_IP_Handler_Loop_NoChange

CV_IP_Handler_Loop_Next
	movlw	CVIP2_BASE-CVIP1_BASE
	addwf	FSR1L, F
	incf	IRQ_TMP5, F
	BRA_IFCLR IRQ_TMP5, 3, ACCESS, CV_IP_Handler_Loop

	return


;; --------------------------------------------------------------------------
;; Help Function which interpolates a 12bit value
;; IN:  base address of parameter set (CVIPx_BASE) in FSR1
;;      target value in IRQ_TMP[12]
;;      slew rate (1..7) in IRQ_TMP3
;; OUT:	interpolated value in IRQ_TMP[12]
;; USES: PROD[LH]
;; --------------------------------------------------------------------------
CV_IP_Sub
	;; check for new target value
	movlw	CVIPx_TARGET_VALUE_L
	movf	PLUSW1, W
	cpfseq	IRQ_TMP1, ACCESS
	rgoto CV_IP_Change
	movlw	CVIPx_TARGET_VALUE_H
	movf	PLUSW1, W
	cpfseq	IRQ_TMP2, ACCESS
	rgoto CV_IP_Change
	rgoto	CV_IP_Cont
CV_IP_Change
	;; copy new target value
	movlw	CVIPx_TARGET_VALUE_L
	movff	IRQ_TMP1, PLUSW1
	movlw	CVIPx_TARGET_VALUE_H
	movff	IRQ_TMP2, PLUSW1

	;; calculate difference between current value and new target value
	movlw	CVIPx_VALUE_L
	movf	PLUSW1, W
	subwf	IRQ_TMP1, W
	movwf	IRQ_TMP1
	movlw	CVIPx_VALUE_H
	movf	PLUSW1, W
	subwfb	IRQ_TMP2, W
	movwf	IRQ_TMP2

	;; branch depending on slew rate (fast, unrolled version)
	BRA_IFSET IRQ_TMP3, 2, ACCESS, CV_IP_Change_SlewRate4567
CV_IP_Change_SlewRate0123
	BRA_IFSET IRQ_TMP3, 1, ACCESS, CV_IP_Change_SlewRate23
CV_IP_Change_SlewRate01
	;; value which will be added on each step: difference / 2
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F

	;; extend to signed 16bit value
	movf	IRQ_TMP2, W
	andlw	0x7f
	btfsc	WREG, 6
	iorlw 0x80
	movwf	IRQ_TMP2
	rgoto	CV_IP_Change_SlewRate_Cont

CV_IP_Change_SlewRate23
	BRA_IFSET IRQ_TMP3, 0, ACCESS, CV_IP_Change_SlewRate3
CV_IP_Change_SlewRate2
	;; value which will be added on each step: difference / 4
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F

	;; extend to signed 16bit value
	movf	IRQ_TMP2, W
	andlw	0x3f
	btfsc	WREG, 5
	iorlw 0xc0
	movwf	IRQ_TMP2
	rgoto	CV_IP_Change_SlewRate_Cont

CV_IP_Change_SlewRate3
	;; value which will be added on each step: difference / 8
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F

	;; extend to signed 16bit value
	movf	IRQ_TMP2, W
	andlw	0x1f
	btfsc	WREG, 4
	iorlw 0xe0
	movwf	IRQ_TMP2
	rgoto	CV_IP_Change_SlewRate_Cont

CV_IP_Change_SlewRate4567
	BRA_IFSET IRQ_TMP3, 1, ACCESS, CV_IP_Change_SlewRate67
CV_IP_Change_SlewRate45
	BRA_IFSET IRQ_TMP3, 0, ACCESS, CV_IP_Change_SlewRate5
CV_IP_Change_SlewRate4
	;; value which will be added on each step: difference / 16
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F

	;; extend to signed 16bit value
	movf	IRQ_TMP2, W
	andlw	0x0f
	btfsc	WREG, 3
	iorlw 0xf0
	movwf	IRQ_TMP2
	rgoto	CV_IP_Change_SlewRate_Cont

CV_IP_Change_SlewRate5
	;; value which will be added on each step: difference / 32
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F

	;; extend to signed 16bit value
	movf	IRQ_TMP2, W
	andlw	0x07
	btfsc	WREG, 2
	iorlw 0xf8
	movwf	IRQ_TMP2
	rgoto	CV_IP_Change_SlewRate_Cont

CV_IP_Change_SlewRate67
	BRA_IFSET IRQ_TMP3, 0, ACCESS, CV_IP_Change_SlewRate7
CV_IP_Change_SlewRate6
	;; value which will be added on each step: difference / 64
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F

	;; extend to signed 16bit value
	movf	IRQ_TMP2, W
	andlw	0x03
	btfsc	WREG, 1
	iorlw 0xfc
	movwf	IRQ_TMP2
	rgoto	CV_IP_Change_SlewRate_Cont

CV_IP_Change_SlewRate7
	;; value which will be added on each step: difference / 128
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F

	;; extend to signed 16bit value
	movf	IRQ_TMP2, W
	andlw	0x01
	btfsc	WREG, 0
	iorlw 0xfe
	movwf	IRQ_TMP2
	;; 	rgoto	CV_IP_Change_SlewRate_Cont

CV_IP_Change_SlewRate_Cont

	;; should be at least 1 (!)
	movf	IRQ_TMP1, W
	iorwf	IRQ_TMP2, W
	skpnz
	incf	IRQ_TMP1, F

	;; store divided value
	movlw	CVIPx_DIV_VALUE_L
	movff	IRQ_TMP1, PLUSW1
	movlw	CVIPx_DIV_VALUE_H
	movff	IRQ_TMP2, PLUSW1

CV_IP_Cont

	;; step handler

	;; copy current filter value into IRQ_TMP[12]
	movlw	CVIPx_VALUE_L
	movff	PLUSW1, IRQ_TMP1
	movlw	CVIPx_VALUE_H
	movff	PLUSW1, IRQ_TMP2
	
	;; do nothing if target value already reached
	movlw	CVIPx_TARGET_VALUE_L
	movf	PLUSW1, W
	cpfseq	IRQ_TMP1, ACCESS
	rgoto CV_IP_Cont_Do
	movlw	CVIPx_TARGET_VALUE_H
	movf	PLUSW1, W
	cpfseq	IRQ_TMP2, ACCESS
	rgoto CV_IP_Cont_Do
	rgoto	CV_IP_End
CV_IP_Cont_Do

	;; add div value to current value
	movlw	CVIPx_DIV_VALUE_L
	movf	PLUSW1, W
	addwf	IRQ_TMP1, F
	movlw	CVIPx_DIV_VALUE_H
	movf	PLUSW1, W
	addwfc	IRQ_TMP2, F

	;; end reached on 12bit overflow
	BRA_IFSET IRQ_TMP2, 4, ACCESS, CV_IP_TargetReached

	;; end reached if value >= target value
	movlw	CVIPx_TARGET_VALUE_L
	movf	PLUSW1, W
	subwf	IRQ_TMP1, W
	movlw	CVIPx_TARGET_VALUE_H
	movf	PLUSW1, W
	subwfb	IRQ_TMP2, W

	;; branch depending on increment/decrement
	movlw	CVIPx_DIV_VALUE_H
	BRA_IFSET PLUSW1, 7, ACCESS, CV_IP_Dec
CV_IP_Inc
	bnc	CV_IP_End
	;; 	rgoto	CV_IP_TargetReached

CV_IP_TargetReached
	movlw	CVIPx_TARGET_VALUE_L
	movff	PLUSW1, IRQ_TMP1
	movlw	CVIPx_TARGET_VALUE_H
	movff	PLUSW1, IRQ_TMP2
	rgoto	CV_IP_End
	
CV_IP_Dec
	bc	CV_IP_End
	rgoto	CV_IP_TargetReached

CV_IP_End
	;; copy back IRQ_TMP[12] to CVIPx_VALUE_[LH]
	movlw	CVIPx_VALUE_L
	movff	IRQ_TMP1, PLUSW1
	movlw	CVIPx_VALUE_H
	movff	IRQ_TMP2, PLUSW1

	return

