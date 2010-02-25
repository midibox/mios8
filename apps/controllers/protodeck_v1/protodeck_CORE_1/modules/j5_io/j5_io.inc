; $Id: j5_io.inc 426 2008-08-05 23:12:19Z tk $
;
; This driver provides access to MBHP_CORE:J5 as digital IO port
; 
; This include file provides following functions:
;    o J5_IO_Init:      initializes the J5 port
;    o J5_IO_Set:       user function to set all output pins at J5 at once
;    o J5_IO_PinSet:    user function to set a single pin of J5
;    o J5_IO_Get:       user function to get the state of all input pins of J5 at once
;    o J5_IO_PinGet:    user function to get the state of a single input pin of J5
;
; Optional defines:
; J5_IO_DONT_USE_INPUT_FUNCTIONS: if set, input functions won't be provided by driver
; J5_IO_DONT_USE_OUTPUT_FUNCTIONS: if set, output functions won't be provided by driver
;
; Both defines are provided for saving code size
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

#ifndef J5_IO_DONT_USE_INPUT_FUNCTIONS
#define J5_IO_DONT_USE_INPUT_FUNCTIONS 0
#endif

#ifndef J5_IO_DONT_USE_OUTPUT_FUNCTIONS
#define J5_IO_DONT_USE_OUTPUT_FUNCTIONS 0
#endif
	
;; --------------------------------------------------------------------------
;;  FUNCTION: J5_IO_Init
;;  DESCRIPTION: This function initializes J5 as digital IO port.
;;  The "TRIS" (tristate) flags are expected as parameter.
;;  - if 1: pin used as input
;;  - if 0: pin used as output
;;
;;  Pin Mapping:
;;    Pin #0: RA.0
;;    Pin #1: RA.1
;;    Pin #2: RA.2
;;    Pin #3: RA.3
;;    Pin #4: RA.5
;;    Pin #5: RE.0
;;    Pin #6: RE.1
;;    Pin #7: RE.2
;;  IN:   tristate flags in WREG
;;  OUT:  -
;;  USES: MIOS_PARAMETER1
;; --------------------------------------------------------------------------
_J5_IO_Init			; (for C)
J5_IO_Init
	;; temporary store values in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	;; disable the ADC which allocates the analog pins
#if PIC_DERIVATIVE_NEW_ADC == 0 ; PIC18F452
	movlw	0x07
#else
	movlw	0x0f		; all newer PIC18F derivatives (like PIC18F4685)
#endif
	movwf	ADCON1

	;; copy TRIS flags into SFRs

	;; MIOS_PARAMETER1[3..0] -> TRISA[3..0]
	;; MIOS_PARAMETER1[4] -> TRISA[5]
	movlw	0xd0
	andwf	TRISA, F
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	TRISA, F
	btfsc	MIOS_PARAMETER1, 4
	bsf	TRISA, 5

	;; MIOS_PARAMETER1[7..5] -> TRISE[2..0]
	movlw	0xf8
	andwf	TRISE, F
	swapf	MIOS_PARAMETER1, W
	rrf	WREG, W
	andlw	0x07
	iorwf	TRISE, F

	return


#if J5_IO_DONT_USE_OUTPUT_FUNCTIONS == 0
;; --------------------------------------------------------------------------
;;  FUNCTION: J5_IO_Set
;;  DESCRIPTION: This function sets all pins of J5 at once
;;  IN:   pin values in WREG
;;  OUT:  -
;;  USES: MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
_J5_IO_Set			; (for C)
J5_IO_Set
	;; temporary store values in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	;; copy values into SFRs

	;; MIOS_PARAMETER1[3..0] -> LATA[3..0]
	;; MIOS_PARAMETER1[4] -> LATA[5]
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

	;; MIOS_PARAMETER1[7..5] -> LATE[2..0] (make it clean, avoid spikes!)
	movf	LATE, W
	andlw	0xf8
	movwf	MIOS_PARAMETER2
	swapf	MIOS_PARAMETER1, W
	rrf	WREG, W
	andlw	0x07
	iorwf	MIOS_PARAMETER2, W
	movwf	LATE
	
	return
#endif


#if J5_IO_DONT_USE_OUTPUT_FUNCTIONS == 0
;; --------------------------------------------------------------------------
;;  FUNCTION: J5_IO_PinSet
;;  DESCRIPTION: This function sets a single pin of J5
;;  IN:   pin number in WREG, pin value in MIOS_PARAMETER1
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
J5_IO_PinSet
	;; make it superfast: branch depending on WREG
	BRA_IFSET WREG, 2, ACCESS, J5_IO_PinSet_4567
J5_IO_PinSet_0123
	BRA_IFSET WREG, 1, ACCESS, J5_IO_PinSet_23
J5_IO_PinSet_01
	BRA_IFSET WREG, 0, ACCESS, J5_IO_PinSet_1
J5_IO_PinSet_0
	btfsc	MIOS_PARAMETER1, 0		; set A.0
	bsf	LATA, 0
	btfss	MIOS_PARAMETER1, 0
	bcf	LATA, 0
	return
	
J5_IO_PinSet_1
	btfsc	MIOS_PARAMETER1, 0		; set A.1
	bsf	LATA, 1
	btfss	MIOS_PARAMETER1, 0
	bcf	LATA, 1
	return
	
J5_IO_PinSet_23
	BRA_IFSET WREG, 0, ACCESS, J5_IO_PinSet_3
J5_IO_PinSet_2
	btfsc	MIOS_PARAMETER1, 0		; set A.2
	bsf	LATA, 2
	btfss	MIOS_PARAMETER1, 0
	bcf	LATA, 2
	return

J5_IO_PinSet_3
	btfsc	MIOS_PARAMETER1, 0		; set A.3
	bsf	LATA, 3
	btfss	MIOS_PARAMETER1, 0
	bcf	LATA, 3
	return

J5_IO_PinSet_4567
	BRA_IFSET WREG, 1, ACCESS, J5_IO_PinSet_67
J5_IO_PinSet_45
	BRA_IFSET WREG, 0, ACCESS, J5_IO_PinSet_5
J5_IO_PinSet_4
	btfsc	MIOS_PARAMETER1, 0		; set A.5
	bsf	LATA, 5
	btfss	MIOS_PARAMETER1, 0
	bcf	LATA, 5
	return

J5_IO_PinSet_5
	btfsc	MIOS_PARAMETER1, 0		; set E.0
	bsf	LATE, 0
	btfss	MIOS_PARAMETER1, 0
	bcf	LATE, 0
	return

J5_IO_PinSet_67
	BRA_IFSET WREG, 0, ACCESS, J5_IO_PinSet_7
J5_IO_PinSet_6
	btfsc	MIOS_PARAMETER1, 0		; set E.1
	bsf	LATE, 1
	btfss	MIOS_PARAMETER1, 0
	bcf	LATE, 1
	return

J5_IO_PinSet_7
	btfsc	MIOS_PARAMETER1, 0		; set E.2
	bsf	LATE, 2
	btfss	MIOS_PARAMETER1, 0
	bcf	LATE, 2
	return
#endif


#if J5_IO_DONT_USE_INPUT_FUNCTIONS == 0
;; --------------------------------------------------------------------------
;;  FUNCTION: J5_IO_Get
;;  DESCRIPTION: This function returns the state of all J5 pins
;;  IN:   -
;;  OUT:  pin values in WREG
;;  USES: MIOS_PARAMETER1
;; --------------------------------------------------------------------------
_J5_IO_Get			; (for C)
J5_IO_Get
	;; using MIOS_PARAMETER1 as temporary register

	;; MIOS_PARAMETER1[3..0] == PORTA[3..0]
	movf	PORTA, W
	andlw	0x0f

	;; MIOS_PARAMETER1[4] == PORTA[5]
	btfsc	PORTA, 5
	iorlw	0x10
	movwf	MIOS_PARAMETER1
	
	;; MIOS_PARAMETER1[7..6] == PORTE[2..0]
	swapf	PORTE, W
	rlf	WREG, W
	andlw	0xe0
	iorwf	MIOS_PARAMETER1, W

	;; pin status now in WREG
	return
#endif


#if J5_IO_DONT_USE_INPUT_FUNCTIONS == 0
;; --------------------------------------------------------------------------
;;  FUNCTION: J5_IO_PinGet
;;  DESCRIPTION: This function returns the state of a single pin at J5
;;  IN:   J5 pin number (0-7) in WREG
;;  OUT:  pin value (0 or 1) in WREG
;;  USES: MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
_J5_IO_PinGet			; (for C)
J5_IO_PinGet
	;; temporary store pin mask in MIOS_PARAMETER2
	call	MIOS_HLP_GetBitORMask
	movwf	MIOS_PARAMETER2

	;; get current state of J5 inputs
	rcall	J5_IO_Get

	;; and check if pin set/cleared by applying mask
	andwf	MIOS_PARAMETER2, W

	;; return either 0 or 1
	skpz
	movlw	0x01

	;; fix ZERO flag (because movlw instruction doesn't touch it)
	andlw	0xff
	return
#endif
