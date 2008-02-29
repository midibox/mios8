; $Id$
;
; AOUT driver for TLV5630 (-> MBHP_AOUT_NG module)
; 
; See aout.inc for description of the available functions
;
; ==========================================================================
;
;  Copyright 1998-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Hardware specific initialisation (see aout.inc for details)
;; --------------------------------------------------------------------------
AOUT_InitHW
	;; ensure that clock starts at low level
        bcf	AOUT_LAT_SCLK, AOUT_PIN_SCLK

	;; deactivate chip select
	bsf	AOUT_LAT_CS, AOUT_PIN_CS

	;; enable pin drivers
	bcf	AOUT_TRIS_CS, AOUT_PIN_CS
	bcf	AOUT_TRIS_DIN, AOUT_PIN_DIN
	bcf	AOUT_TRIS_SCLK, AOUT_PIN_SCLK

	;; initialize CTRL0
	movlw	(1 << 3) | (3 << 1)		; DO=1 (DOUT Enable), R=3 (internal reference, 2V)
	movwf	MIOS_PARAMETER1
	movlw	0x8 << 4
	movwf	MIOS_PARAMETER2
	bcf	AOUT_LAT_CS, AOUT_PIN_CS	; activate chip select
	rcall	AOUT_LoadWord
	bsf	AOUT_LAT_CS, AOUT_PIN_CS	; deactivate chip select
	
	;; initialize CTRL1
	movlw	0x00
	movwf	MIOS_PARAMETER1
	movlw	0x9 << 4
	movwf	MIOS_PARAMETER2
	bcf	AOUT_LAT_CS, AOUT_PIN_CS	; activate chip select
	rcall	AOUT_LoadWord
	bsf	AOUT_LAT_CS, AOUT_PIN_CS	; deactivate chip select

	return


;; --------------------------------------------------------------------------
;;  Sets the digital pins (see aout.inc for details)
;; --------------------------------------------------------------------------
AOUT_DigitalPinsSet
	;; not supported by TLV5630
	return

	
;; --------------------------------------------------------------------------
;;  Update function (see aout.inc for details)
;; --------------------------------------------------------------------------
AOUT_Update
	;; skip if no update requested
	SET_BSR	AOUT_UPDATE_REQ
	movf	AOUT_UPDATE_REQ, W, BANKED
	bz	AOUT_Update_End

        bcf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; ensure that clock starts at low level

	;; check which channels should be updated
	lfsr	FSR1, AOUT_VALUES	; base pointer to 16bit value array
	clrf	PRODL			; loop counter
	movlw	0x01			; AND mask (running one)
	movwf	PRODH
AOUT_Update_Loop
	;; check if channel has been updated
	movf	PRODH, W
	andwf	AOUT_UPDATE_REQ, W, BANKED
	bz	AOUT_Update_Loop_Next

	;; commands to load and update the DAC register:
	;; bit [15] = 0, bit [14:12] channel number, bit [11:0] DAC value
	swapf	POSTINC1, W		; convert 16bit to 12bit value -> MIOS_PARAMETER[12]
	andlw	0x0f
	movwf	MIOS_PARAMETER1

	swapf	INDF1, W
	andlw	0xf0
	iorwf	MIOS_PARAMETER1, F

	swapf	POSTDEC1, W
	andlw	0x0f
	movwf	MIOS_PARAMETER2

	;; invert value if inversion flag set
	movff	AOUT_INVERTED, WREG	; (note: AOUT_INVERTED could be located in another bank...)
	andwf	PRODH, W
	bz	AOUT_Update_Loop_NotInverted
AOUT_Update_Loop_Inverted
	comf	MIOS_PARAMETER1, F
	movlw	0x0f
	xorwf	MIOS_PARAMETER2, F
AOUT_Update_Loop_NotInverted

	;; insert channel number (== loop counter)
	swapf	PRODL, W
	iorwf	MIOS_PARAMETER2, F

	;; transfer word to chip
	bcf	AOUT_LAT_CS, AOUT_PIN_CS	; activate chip select
	rcall	AOUT_LoadWord
	bsf	AOUT_LAT_CS, AOUT_PIN_CS	; deactivate chip select

AOUT_Update_Loop_Next
	;; increment loop counter, and exit if all 8 channels processed
	incf	PRODL, F
	BRA_IFSET PRODL, 3, ACCESS, AOUT_Update_End
	;; else increment pointer to AOUT_VALUESx and check next channel
	movlw	2
	addwf	FSR1L, F			; (assumed that AOUT_VALUES is located within 256 byte range)
	clrc					; shiftleft running-one mask
	rlf	PRODH, F
	rgoto	AOUT_Update_Loop

AOUT_Update_End
	;; clear update flags
	clrf	AOUT_UPDATE_REQ, BANKED

	return


;; --------------------------------------------------------------------------
;;  Internal function to load a 16bit value into TLV5630
;;  IN: word in MIOS_PARAMETER[12]
;;  USES: MIOS_PARAMETER3 as temporary register
;; --------------------------------------------------------------------------
AOUT_LoadWord
	;; NOTE: you will notice that the instructions are sometimes arranged
	;; in a special order to ensure proper output signals - for example:
	;; between a rising and a falling SCLK edge there is at least one
	;; other instruction to ensure that the high pulse of the SCLK
	;; is longer than 100 nS

	movlw	16				; init loop counter
	movwf	MIOS_PARAMETER3
AOUT_LoadWord_Loop
	bcf	AOUT_LAT_DIN, AOUT_PIN_DIN	; set DIN depending on current MSB
	btfsc	MIOS_PARAMETER2, 7
	bsf	AOUT_LAT_DIN, AOUT_PIN_DIN
	rlf	MIOS_PARAMETER1, F		; start to shift the 16-bit value
	bsf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; rising clock edge
	rlf	MIOS_PARAMETER2, F		; second step for the 16-bit shift
	bcf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; falling clock edge
	decfsz	MIOS_PARAMETER3, F		; loop 16 times
	rgoto	AOUT_LoadWord_Loop

	return
