; $Id$
;
; MAX7219/MAX7221 driver (used by Stribe Project)
; 
; ==========================================================================
;
;  Copyright 1998-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
;
; Number of connected MAX72xx chips
#ifndef MAX72XX_CHAINLENGTH
#define MAX72XX_CHAINLENGTH 16
#endif
;
; Pin Configuration
; Can be overloaded from external
#ifndef MAX72XX_LAT_CS
#define MAX72XX_LAT_CS		LATC	; The chip select pin
#endif
#ifndef MAX72XX_TRIS_CS
#define MAX72XX_TRIS_CS		TRISC	; is connected to Port C.0
#endif
#ifndef MAX72XX_PIN_CS
#define MAX72XX_PIN_CS		0	; (CANNOT be shared with other outputs!)
#endif
;
#ifndef MAX72XX_LAT_SCLK
#define MAX72XX_LAT_SCLK	LATC	; The shift clock input pin SCLK
#endif
#ifndef MAX72XX_TRIS_SCLK
#define MAX72XX_TRIS_SCLK	TRISC	; is connected to Port C.1
#endif
#ifndef MAX72XX_PIN_SCLK
#define MAX72XX_PIN_SCLK	1	; (can be shared with other outputs)
#endif
;
#ifndef MAX72XX_LAT_DIN
#define MAX72XX_LAT_DIN		LATC	; The data input pin DIN
#endif
#ifndef MAX72XX_TRIS_DIN
#define MAX72XX_TRIS_DIN	TRISC	; is connected to Port C.2
#endif
#ifndef MAX72XX_PIN_DIN
#define MAX72XX_PIN_DIN		2	; (can be shared with other outputs)
#endif
;

;; --------------------------------------------------------------------------
;;  MAX72XX initialisation
;;  IN: -
;;  USES: MIOS_PARAMETER[123], PROD[LH] and TABLAT
;; --------------------------------------------------------------------------
_MAX72XX_Init ; (for C)
MAX72XX_Init
	;; ensure that clock starts at low level
        bcf	MAX72XX_LAT_SCLK, MAX72XX_PIN_SCLK

	;; activate chip select
	bcf	MAX72XX_LAT_CS, MAX72XX_PIN_CS

	;; enable pin drivers
	bcf	MAX72XX_TRIS_CS, MAX72XX_PIN_CS
	bcf	MAX72XX_TRIS_DIN, MAX72XX_PIN_DIN
	bcf	MAX72XX_TRIS_SCLK, MAX72XX_PIN_SCLK

	;; clear all digits
	movlw	MAX72XX_CHAINLENGTH*8
	movwf	PRODL
	lfsr	FSR1, _max72xx_digits
MAX72XX_Init_Loop
	clrf	POSTINC1
	decfsz	PRODL, F
	rgoto	MAX72XX_Init_Loop

	;; enter normal operation mode
	movlw	0x01
	movwf	MIOS_PARAMETER1
	movlw	0x0c		; shutdown register
	rcall	MAX72XX_WriteRegs

	;; set decode mode to 0 (no decoding)
	movlw	0x00
	movwf	MIOS_PARAMETER1
	movlw	0x09		; decode mode register
	rcall	MAX72XX_WriteRegs

	;; set maximum intensity
	movlw	0x0f
	movwf	MIOS_PARAMETER1
	movlw	0x0a		; intensity register
	rcall	MAX72XX_WriteRegs

	;; scan all digits
	movlw	0x07
	movwf	MIOS_PARAMETER1
	movlw	0x0b		; scan limit register
	rcall	MAX72XX_WriteRegs

	;; ensure that display test mode disabled
	movlw	0x00
	movwf	MIOS_PARAMETER1
	movlw	0x0f		; display test register
	rcall	MAX72XX_WriteRegs

	;; update the digits
 	rcall	MAX72XX_UpdateAllDigits

	return


;; --------------------------------------------------------------------------
;;  Transfers the segment patterns of the given digit number in WREG (0-7)
;;  IN: digit number (0-7) in WREG
;;  USES: MIOS_PARAMETER[12], PRODL, TABLAT
;; --------------------------------------------------------------------------
_MAX72XX_UpdateDigits ; (for C)
MAX72XX_UpdateDigits
        bcf	MAX72XX_LAT_SCLK, MAX72XX_PIN_SCLK	; ensure that clock starts at low level
	bcf	MAX72XX_LAT_CS, MAX72XX_PIN_CS		; activate chip select line

	movwf	PRODL					; store digit number in PRODL
	movlw	MAX72XX_CHAINLENGTH-1			; loop counter
	movwf	TABLAT
MAX72XX_UpdateDigitsLoop
	lfsr	FSR1, _max72xx_digits			; pointer to value array
	rlf	TABLAT, W				; determine offset
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xf8
	addwf	PRODL, W
	movff	PLUSW1, MIOS_PARAMETER1			; pattern -> MIOS_PARAMETER1
	incf	PRODL, W				; address = digit+1
	movwf	MIOS_PARAMETER2

	rcall	MAX72XX_LoadWord			; transfer 16 bits

	decf	TABLAT, F				; loop until overflow
	bc	MAX72XX_UpdateDigitsLoop

	bsf	MAX72XX_LAT_CS, MAX72XX_PIN_CS		; release chip select (MAX72XX will latch serial data)

	return


;; --------------------------------------------------------------------------
;;  Transfers the segment patterns of all digits
;;  IN: digit number in WREG
;;  USES: MIOS_PARAMETER[12], PROD[LH]
;; --------------------------------------------------------------------------
_MAX72XX_UpdateAllDigits ; (for C)
MAX72XX_UpdateAllDigits
	clrf	PRODH					; using PRODH as loop counter (not used by MAX72XX_UpdateDigits)
MAX72XX_UpdateAllDigitsLoop
	movf	PRODH, W				; update the digits
	rcall	MAX72XX_UpdateDigits
	incf	PRODH, F				; loop 8 times
	BRA_IFCLR PRODH, 3, ACCESS, MAX72XX_UpdateAllDigitsLoop
	return


;; --------------------------------------------------------------------------
;;  Internal function to load a 16bit value into an individual MAX72XX register
;;  IN: MAX72XX number which should be updated in WREG, data byte in MIOS_PARAMETER1, address in MIOS_PARAMETER2
;;  USES: PROD[LH], TABLAT, MIOS_PARAMETER3
;; --------------------------------------------------------------------------
MAX72XX_WriteReg
        bcf	MAX72XX_LAT_SCLK, MAX72XX_PIN_SCLK	; ensure that clock starts at low level
	bcf	MAX72XX_LAT_CS, MAX72XX_PIN_CS		; activate chip select line

	movwf	MIOS_PARAMETER3				; store register which should be modified in MIOS_PARAMETER3

	movff	MIOS_PARAMETER1, PRODL			; backup of data byte
	movff	MIOS_PARAMETER2, PRODH			; backup of address

	movlw	MAX72XX_CHAINLENGTH-1			; loop counter
	movwf	TABLAT
MAX72XX_WriteRegLoop
	movf	TABLAT, W				; check if register selected
	cpfseq	MIOS_PARAMETER3, ACCESS
	rgoto	MAX72XX_WriteRegLoop_NOP
MAX72XX_WriteRegLoop_Load
	movff	PRODL, MIOS_PARAMETER1			; get MIOS_PARAMETER[12] from backup
	movff	PRODH, MIOS_PARAMETER2
	rcall	MAX72XX_LoadWord			; upload word
	rgoto	MAX72XX_WriteRegLoop_Next

MAX72XX_WriteRegLoop_NOP
	clrf	MIOS_PARAMETER1				; upload NOP
	clrf	MIOS_PARAMETER2
	rcall	MAX72XX_LoadWord			; upload word
	;; 	rgoto	MAX72XX_WriteRegLoop_Next

MAX72XX_WriteRegLoop_Next
	decf	TABLAT, F				; loop until overflow
	bc	MAX72XX_WriteRegLoop

	bsf	MAX72XX_LAT_CS, MAX72XX_PIN_CS		; release chip select (MAX72XX will latch serial data)
	return


;; --------------------------------------------------------------------------
;;  Internal function to load a 16bit value into all MAX72XX registers
;;  IN: address in WREG, data byte in MIOS_PARAMETER1
;;  USES: TABLAT
;; --------------------------------------------------------------------------
MAX72XX_WriteRegs
        bcf	MAX72XX_LAT_SCLK, MAX72XX_PIN_SCLK	; ensure that clock starts at low level
	bcf	MAX72XX_LAT_CS, MAX72XX_PIN_CS		; activate chip select line

	movwf	MIOS_PARAMETER2				; store address in MIOS_PARAMETER2
							; data already in MIOS_PARAMETER1

	movlw	MAX72XX_CHAINLENGTH-1			; loop counter
	movwf	TABLAT
MAX72XX_WriteRegsLoop
	rcall	MAX72XX_LoadWord			; upload word

	decf	TABLAT, F				; loop until overflow
	bc	MAX72XX_WriteRegsLoop

	bsf	MAX72XX_LAT_CS, MAX72XX_PIN_CS		; release chip select (MAX72XX will latch serial data)
	return


;; --------------------------------------------------------------------------
;;  Internal function to load a 16bit value into MAX72XX
;;  IN: word in MIOS_PARAMETER[12]
;;  USES: -
;; --------------------------------------------------------------------------
MAX72XX_LoadWord
	;; NOTE: you will notice that the instructions are sometimes arranged
	;; in a special order to ensure proper output signals - for example:
	;; between a rising and a falling SCLK edge there is at least one
	;; other instruction to ensure that the high pulse of the SCLK
	;; is longer than 100 nS

	;; optimized "unrolled" version
	;; complete update of 1024 LEDs with previous looped version: ca. 2.3 mS
	;; complete update of 1024 LEDs with unrolled version: ca. 1.2 mS!
	;; upload of 16*16 bits (16 MAX72xx) takes ca. 150 uS

	;; other advantages: no additional register used, and MIOS_PARAMETER[12] won't be modified

MAX72XX_LoadWord_HLP MACRO reg, bit
	bcf	MAX72XX_LAT_DIN, MAX72XX_PIN_DIN; set DIN depending on current MSB
	bcf	MAX72XX_LAT_SCLK, MAX72XX_PIN_SCLK	; falling clock edge
	btfsc	reg, bit
	bsf	MAX72XX_LAT_DIN, MAX72XX_PIN_DIN
	bsf	MAX72XX_LAT_SCLK, MAX72XX_PIN_SCLK	; rising clock edge
	ENDM

	MAX72XX_LoadWord_HLP MIOS_PARAMETER2, 7
	MAX72XX_LoadWord_HLP MIOS_PARAMETER2, 6
	MAX72XX_LoadWord_HLP MIOS_PARAMETER2, 5
	MAX72XX_LoadWord_HLP MIOS_PARAMETER2, 4
	MAX72XX_LoadWord_HLP MIOS_PARAMETER2, 3
	MAX72XX_LoadWord_HLP MIOS_PARAMETER2, 2
	MAX72XX_LoadWord_HLP MIOS_PARAMETER2, 1
	MAX72XX_LoadWord_HLP MIOS_PARAMETER2, 0
	MAX72XX_LoadWord_HLP MIOS_PARAMETER1, 7
	MAX72XX_LoadWord_HLP MIOS_PARAMETER1, 6
	MAX72XX_LoadWord_HLP MIOS_PARAMETER1, 5
	MAX72XX_LoadWord_HLP MIOS_PARAMETER1, 4
	MAX72XX_LoadWord_HLP MIOS_PARAMETER1, 3
	MAX72XX_LoadWord_HLP MIOS_PARAMETER1, 2
	MAX72XX_LoadWord_HLP MIOS_PARAMETER1, 1
	MAX72XX_LoadWord_HLP MIOS_PARAMETER1, 0
	nop
	bcf	MAX72XX_LAT_SCLK, MAX72XX_PIN_SCLK	; falling clock edge

	return
