; $Id$
;
; Using J5 as DOUT
; 
; This include file provides following functions:
;    o J5_DOUT_Init:    initializes the J5 port
;    o J5_DOUT_Set:     user function to set all pins at J5 at once
;    o J5_DOUT_PinSet:  user function to set a single pin of J5
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: J5_DOUT_Init
;;  DESCRIPTION: This function initializes J5 as digital output
;;     Pin #0: RA.0
;;     Pin #1: RA.1
;;     Pin #2: RA.2
;;     Pin #3: RA.3
;;     Pin #4: RA.5
;;     Pin #5: RE.0
;;     Pin #6: RE.1
;;     Pin #7: RE.2
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------

J5_DOUT_Init
	;; disable the ADC which allocates the analog pins
	movlw	0x07
	movwf	ADCON1

	;; turn on the output driver of PORTA and PORTE
	;; (note: if you want to use some of them as input instead, just set the appr. line to "bsf TRISx, x"
	bcf	TRISA, 0	; Pin RA.0 = output
	bcf	TRISA, 1	; Pin RA.1 = output
	bcf	TRISA, 2	; Pin RA.2 = output
	bcf	TRISA, 3	; Pin RA.3 = output
	bcf	TRISA, 5	; Pin RA.5 = output
	bcf	TRISE, 0	; Pin RE.0 = output
	bcf	TRISE, 1	; Pin RE.1 = output
	bcf	TRISE, 2	; Pin RE.2 = output

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: J5_DOUT_Set
;;  DESCRIPTION: This function sets all pins of J5 at once
;;  IN:   pin values in WREG
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
J5_DOUT_Set
	;; temporary store values in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	;; copy values into SFRs

	;; MIOS_PARAMETER1[3..0] -> PORTA[3..0]
	;; MIOS_PARAMETER1[4] -> PORTA[5]
	;; (make it clean, avoid spikes!)
	movf	LATA, W
	andlw	0xd0
	movwf	MIOS_PARAMETER2
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	MIOS_PARAMETER2, W
	btfsc	MIOS_PARAMETER1, 4
	iorlw	0x20
	movwf	LATA

	;; MIOS_PARAMETER1[7..5] -> PORTE[2..0] (make it clean, avoid spikes!)
	movf	LATE, W
	andlw	0xf8
	movwf	MIOS_PARAMETER2
	swapf	MIOS_PARAMETER1, W
	rrf	WREG, W
	andlw	0x07
	iorwf	MIOS_PARAMETER2, W
	movwf	LATE
	
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: J5_DOUT_PinSet
;;  DESCRIPTION: This function sets a single pin of J5
;;  IN:   pin number in WREG, pin value in MIOS_PARAMETER1
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
J5_DOUT_PinSet
	;; make it superfast: branch depending on WREG
	BRA_IFSET WREG, 2, ACCESS, J5_DOUT_PinSet_4567
J5_DOUT_PinSet_0123
	BRA_IFSET WREG, 1, ACCESS, J5_DOUT_PinSet_23
J5_DOUT_PinSet_01
	BRA_IFSET WREG, 0, ACCESS, J5_DOUT_PinSet_1
J5_DOUT_PinSet_0
	btfsc	MIOS_PARAMETER1, 0		; set A.0
	bsf	LATA, 0
	btfss	MIOS_PARAMETER1, 0
	bcf	LATA, 0
	return
	
J5_DOUT_PinSet_1
	btfsc	MIOS_PARAMETER1, 0		; set A.1
	bsf	LATA, 1
	btfss	MIOS_PARAMETER1, 0
	bcf	LATA, 1
	return
	
J5_DOUT_PinSet_23
	BRA_IFSET WREG, 0, ACCESS, J5_DOUT_PinSet_3
J5_DOUT_PinSet_2
	btfsc	MIOS_PARAMETER1, 0		; set A.2
	bsf	LATA, 2
	btfss	MIOS_PARAMETER1, 0
	bcf	LATA, 2
	return

J5_DOUT_PinSet_3
	btfsc	MIOS_PARAMETER1, 0		; set A.3
	bsf	LATA, 3
	btfss	MIOS_PARAMETER1, 0
	bcf	LATA, 3
	return

J5_DOUT_PinSet_4567
	BRA_IFSET WREG, 1, ACCESS, J5_DOUT_PinSet_67
J5_DOUT_PinSet_45
	BRA_IFSET WREG, 0, ACCESS, J5_DOUT_PinSet_5
J5_DOUT_PinSet_4
	btfsc	MIOS_PARAMETER1, 0		; set A.5
	bsf	LATA, 5
	btfss	MIOS_PARAMETER1, 0
	bcf	LATA, 5
	return

J5_DOUT_PinSet_5
	btfsc	MIOS_PARAMETER1, 0		; set E.0
	bsf	LATE, 0
	btfss	MIOS_PARAMETER1, 0
	bcf	LATE, 0
	return

J5_DOUT_PinSet_67
	BRA_IFSET WREG, 0, ACCESS, J5_DOUT_PinSet_7
J5_DOUT_PinSet_6
	btfsc	MIOS_PARAMETER1, 0		; set E.1
	bsf	LATE, 1
	btfss	MIOS_PARAMETER1, 0
	bcf	LATE, 1
	return

J5_DOUT_PinSet_7
	btfsc	MIOS_PARAMETER1, 0		; set E.2
	bsf	LATE, 2
	btfss	MIOS_PARAMETER1, 0
	bcf	LATE, 2
	return

