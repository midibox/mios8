; $Id$
;
; AOUT driver for 74HC595 based DAC (-> MBHP_AOUT_LC module)
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
;
; Special Option for AOUT_LC module: select either 12/4 bit (0), or 8/8 bit (1) configuration
; this has to be done for each module seperately
#ifndef AOUT_LC_RESOLUTION_OPTION_M1
#define AOUT_LC_RESOLUTION_OPTION_M1 0
#endif
#ifndef AOUT_LC_RESOLUTION_OPTION_M2
#define AOUT_LC_RESOLUTION_OPTION_M2 0
#endif
#ifndef AOUT_LC_RESOLUTION_OPTION_M3
#define AOUT_LC_RESOLUTION_OPTION_M3 0
#endif
#ifndef AOUT_LC_RESOLUTION_OPTION_M4
#define AOUT_LC_RESOLUTION_OPTION_M4 0
#endif

;; --------------------------------------------------------------------------
;;  Hardware specific initialisation (see aout.inc for details)
;; --------------------------------------------------------------------------
AOUT_InitHW
	;; CS line used as RCLK by AOUT_LC module - should be at low level by default
	bcf	AOUT_LAT_CS, AOUT_PIN_CS

	;; enable pin drivers
	bcf	AOUT_TRIS_CS, AOUT_PIN_CS
	bcf	AOUT_TRIS_DIN, AOUT_PIN_DIN
	bcf	AOUT_TRIS_SCLK, AOUT_PIN_SCLK

	return


;; --------------------------------------------------------------------------
;;  Sets the digital pins (see aout.inc for details)
;; --------------------------------------------------------------------------
_AOUT_DigitalPinsSet		; (for C)
AOUT_DigitalPinsSet
	;; not supported by AOUT_LC module
	return

	
;; --------------------------------------------------------------------------
;;  Update function (see aout.inc for details)
;; --------------------------------------------------------------------------
_AOUT_Update			; (for C)
AOUT_Update
	;; skip if no update requested
	SET_BSR	AOUT_UPDATE_REQ
	movf	AOUT_UPDATE_REQ, W, BANKED
	bz	AOUT_Update_End

        bcf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; ensure that clock starts at low level

	;; update all channels
	SET_BSR	AOUT_INVERTED

	;; 4th AOUT_LC module
	lfsr	FSR1, AOUT_VALUES + 3*4		; pointer to values in FSR1
	swapf	AOUT_INVERTED, W, BANKED	; inversion flags in MIOS_PARAMETER3[1:0]
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x03
	movwf	MIOS_PARAMETER3
#if AOUT_LC_RESOLUTION_OPTION_M4
	rcall	AOUT_Update_Hlp_12_4
#else
	rcall	AOUT_Update_Hlp_8_8
#endif

	;; 3rd AOUT_LC module
	lfsr	FSR1, AOUT_VALUES + 2*4		; pointer to values in FSR1
	swapf	AOUT_INVERTED, W, BANKED	; inversion flags in MIOS_PARAMETER3[1:0]
	andlw	0x03
	movwf	MIOS_PARAMETER3
#if AOUT_LC_RESOLUTION_OPTION_M3
	rcall	AOUT_Update_Hlp_12_4
#else
	rcall	AOUT_Update_Hlp_8_8
#endif

	;; 2nd AOUT_LC module
	lfsr	FSR1, AOUT_VALUES + 1*4		; pointer to values in FSR1
	rrf	AOUT_INVERTED, W, BANKED	; inversion flags in MIOS_PARAMETER3[1:0]
	rrf	WREG, W
	andlw	0x03
	movwf	MIOS_PARAMETER3
#if AOUT_LC_RESOLUTION_OPTION_M2
	rcall	AOUT_Update_Hlp_12_4
#else
	rcall	AOUT_Update_Hlp_8_8
#endif

	;; 1st AOUT_LC module
	lfsr	FSR1, AOUT_VALUES + 0*4		; pointer to values in FSR1
	movf	AOUT_INVERTED, W, BANKED	; inversion flags in MIOS_PARAMETER3[1:0]
	andlw	0x03
	movwf	MIOS_PARAMETER3
#if AOUT_LC_RESOLUTION_OPTION_M1
	rcall	AOUT_Update_Hlp_12_4
#else
	rcall	AOUT_Update_Hlp_8_8
#endif

	;; CS line used as RCLK by AOUT_LC module
	bsf	AOUT_LAT_CS, AOUT_PIN_CS	; trigger RCLK to update chain
	nop	
	bcf	AOUT_LAT_CS, AOUT_PIN_CS

AOUT_Update_End
	;; clear update flags
	SET_BSR	AOUT_UPDATE_REQ
	clrf	AOUT_UPDATE_REQ, BANKED

	return

;; help function to update a single AOUT_LC module
AOUT_Update_Hlp_12_4
	;; map 2 * 16 bit to 12/4 bit
	movlw	1				; first channel: 12bit
	movf	PLUSW1, W			; high-byte -> MIOS_PARAMETER1
	btfsc	MIOS_PARAMETER3, 0		; inversion?
	xorlw	0xff
	movwf	MIOS_PARAMETER1		
	movf	INDF1, W			; [7:4] of low-byte -> MIOS_PARAMETER2[7:4]
	andlw	0xf0
	btfsc	MIOS_PARAMETER3, 0		; inversion?
	xorlw	0xf0
	movwf	MIOS_PARAMETER2

	movlw	3				; second channel: 4bit
	swapf	PLUSW1, W			; [7:4] of high-byte -> MIOS_PARAMETER2[3:0]
	andlw	0x0f
	btfsc	MIOS_PARAMETER3, 1		; inversion?
	xorlw	0x0f
	iorwf	MIOS_PARAMETER2, F
	rgoto	AOUT_LoadWord

AOUT_Update_Hlp_8_8
	;; map 2 * 16 bit to 8/8 bit
	movlw	1				; first channel: 8bit
	movf	PLUSW1, W			; high-byte -> MIOS_PARAMETER1
	btfsc	MIOS_PARAMETER3, 0		; inversion?
	xorlw	0xff
	movwf	MIOS_PARAMETER1

	movlw	3				; second channel: 8bit
	movf	PLUSW1, W			; high-byte -> MIOS_PARAMETER2
	btfsc	MIOS_PARAMETER3, 1		; inversion?
	xorlw	0xff
	movwf	MIOS_PARAMETER2
	rgoto	AOUT_LoadWord



;; --------------------------------------------------------------------------
;;  Internal function to load a 16bit value into two 74HC595
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
