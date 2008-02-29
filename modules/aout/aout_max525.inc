; $Id$
;
; AOUT driver for two daisychained MAX525 (-> MBHP_AOUT module)
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
	;; deactivate chip select
	bsf	AOUT_LAT_CS, AOUT_PIN_CS

	;; enable pin drivers
	bcf	AOUT_TRIS_CS, AOUT_PIN_CS
	bcf	AOUT_TRIS_DIN, AOUT_PIN_DIN
	bcf	AOUT_TRIS_SCLK, AOUT_PIN_SCLK

	return


;; --------------------------------------------------------------------------
;;  Sets the digital pins (see aout.inc for details)
;; --------------------------------------------------------------------------
AOUT_DigitalPinsSet
	;; temporary store gate flags in PRODL
	movwf	PRODL

	;; commands to set the UPO:
	;; UPO=low:  A1=0, A0=0, C1=1, C0=0
	;; UPO=high: A1=0, A0=1, C1=1, C0=0

	;; ensure that clock starts at low level
        bcf	AOUT_LAT_SCLK, AOUT_PIN_SCLK

	;; activate chip select
	bcf	AOUT_LAT_CS, AOUT_PIN_CS

	;; update last MAX525 in the chain
	clrf	MIOS_PARAMETER1
	movlw	0x20
	btfsc	PRODL, 1	; bit #1
	iorlw	0x40
	movwf	MIOS_PARAMETER2
	rcall	AOUT_LoadWord

	;; update first MAX525 in the chain
	clrf	MIOS_PARAMETER1
	movlw	0x20
	btfsc	PRODL, 0	; bit #0
	iorlw	0x40
	movwf	MIOS_PARAMETER2
	rcall	AOUT_LoadWord

	;; deactivate chip select
	bsf	AOUT_LAT_CS, AOUT_PIN_CS

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
	movlw	0x11			; AND mask (running ones)
	movwf	PRODH
AOUT_Update_Loop
	;; check if one of the two channels has been updated
	swapf	PRODH, W
	iorwf	PRODH, W
	andwf	AOUT_UPDATE_REQ, W, BANKED
	bz	AOUT_Update_Loop_Next

	;; activate chip select
	bcf	AOUT_LAT_CS, AOUT_PIN_CS

	;; update last MAX525 in the chain
	movlw	4*2		; switch to appr. array position
	addwf	FSR1L, F
	movlw	4
	addwf	PRODL, F
	swapf	PRODH, F	; (running one now located at high-nibble)
	rcall	AOUT_Update_Hlp

	;; update first MAX525 in the chain
	movlw	-4*2		; switch back to initial array position
	addwf	FSR1L, F
	movlw	-4
	addwf	PRODL, F
	swapf	PRODH, F	; (running one now located at low-nibble)	
	rcall	AOUT_Update_Hlp

	bsf	AOUT_LAT_CS, AOUT_PIN_CS	; deactivate chip select

AOUT_Update_Loop_Next
	;; increment loop counter, and exit if all 8 channels processed
	incf	PRODL, F
	BRA_IFSET PRODL, 2, ACCESS, AOUT_Update_End	; (each iteration services 2 channels)
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



;; help function used in update loop
AOUT_Update_Hlp
	;; commands to load and update the DAC register:
	;; A1=channel number[1], A0=channel number[0], C1=1, C0=1
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
	bz	AOUT_Update_Hlp_NotInverted
AOUT_Update_Hlp_Inverted
	comf	MIOS_PARAMETER1, F
	movlw	0x0f
	xorwf	MIOS_PARAMETER2, F
AOUT_Update_Hlp_NotInverted

	;; insert channel number (== loop counter)
	swapf	PRODL, W	; located at [7:6]
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xc0
	iorlw	0x30		; [5:4] = 3
	iorwf	MIOS_PARAMETER2, F

	;; transfer word to chip
	rgoto	AOUT_LoadWord


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

