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
; Special Option for AOUT_LC module: select either 12/4 bit (0), or 8/8 bit (1),
; or 12/12/8 (2) configuration
; this has to be done for each module seperately
;
; Note: the 12/12/8 bit selection (2) combines 4 shift registers. The first two SRs
; are working like option 0 (first channel: 12bit, second channel: upper 4bit)
; The third SR outputs the lower bits of second channel, and the fourth SR the
; 8bit value of third channel. The fourth channel is ignored
; (same for channel 4-7... channel 8 ignored)
; For this "combined" option, it is recommented, to set both modes (M1,M2 and/or M3,M4)
; to value 2 for future compatibilty
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
#if AOUT_LC_RESOLUTION_OPTION_M4 == 2
	lfsr	FSR1, AOUT_VALUES + 3*4-2	; pointer to values in FSR1
	swapf	AOUT_INVERTED, W, BANKED	; inversion flags in MIOS_PARAMETER3[1:0]
	rrf	WREG, W
#else
	lfsr	FSR1, AOUT_VALUES + 3*4		; pointer to values in FSR1
	swapf	AOUT_INVERTED, W, BANKED	; inversion flags in MIOS_PARAMETER3[1:0]
	rrf	WREG, W
	rrf	WREG, W
#endif
	movwf	MIOS_PARAMETER3
#if AOUT_LC_RESOLUTION_OPTION_M4 == 0
	rcall	AOUT_Update_Hlp_12_4
#endif
#if AOUT_LC_RESOLUTION_OPTION_M4 == 1
	rcall	AOUT_Update_Hlp_8_8
#endif
#if AOUT_LC_RESOLUTION_OPTION_M4 == 2
	rcall	AOUT_Update_Hlp_12_12_8
#endif

	;; 3rd AOUT_LC module
	lfsr	FSR1, AOUT_VALUES + 2*4		; pointer to values in FSR1
	swapf	AOUT_INVERTED, W, BANKED	; inversion flags in MIOS_PARAMETER3[1:0]
	movwf	MIOS_PARAMETER3
#if AOUT_LC_RESOLUTION_OPTION_M3 == 1
	rcall	AOUT_Update_Hlp_8_8		; used for 8/8 resolution
#else
	rcall	AOUT_Update_Hlp_12_4		; used for 12/4 and 12/12/8 resolution
#endif


	;; 2nd AOUT_LC module
#if AOUT_LC_RESOLUTION_OPTION_M2 == 2
	lfsr	FSR1, AOUT_VALUES + 1*4-2	; pointer to values in FSR1
	rrf	AOUT_INVERTED, W, BANKED	; inversion flags in MIOS_PARAMETER3[1:0]
#else
	lfsr	FSR1, AOUT_VALUES + 1*4		; pointer to values in FSR1
	rrf	AOUT_INVERTED, W, BANKED	; inversion flags in MIOS_PARAMETER3[1:0]
	rrf	WREG, W
#endif
	movwf	MIOS_PARAMETER3
#if AOUT_LC_RESOLUTION_OPTION_M2 == 0
	rcall	AOUT_Update_Hlp_12_4
#endif
#if AOUT_LC_RESOLUTION_OPTION_M2 == 1
	rcall	AOUT_Update_Hlp_8_8
#endif
#if AOUT_LC_RESOLUTION_OPTION_M2 == 2
	rcall	AOUT_Update_Hlp_12_12_8
#endif

	;; 1st AOUT_LC module
	lfsr	FSR1, AOUT_VALUES + 0*4		; pointer to values in FSR1
	movf	AOUT_INVERTED, W, BANKED	; inversion flags in MIOS_PARAMETER3[1:0]
	movwf	MIOS_PARAMETER3
#if AOUT_LC_RESOLUTION_OPTION_M1 == 1
	rcall	AOUT_Update_Hlp_8_8		; used for 8/8 resolution
#else
	rcall	AOUT_Update_Hlp_12_4		; used for 12/4 and 12/12/8 resolution
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
#if AOUT_LC_RESOLUTION_OPTION_M1 != 1 || AOUT_LC_RESOLUTION_OPTION_M2 != 1 || AOUT_LC_RESOLUTION_OPTION_M3 != 1 || AOUT_LC_RESOLUTION_OPTION_M4 != 1
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
#else
	return					; neither 12/4, nor 12/12/4 option used - save memory
#endif



AOUT_Update_Hlp_8_8
#if AOUT_LC_RESOLUTION_OPTION_M1 == 1 || AOUT_LC_RESOLUTION_OPTION_M2 == 1 || AOUT_LC_RESOLUTION_OPTION_M3 == 1 || AOUT_LC_RESOLUTION_OPTION_M4 == 1
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
#else
	return					; 8/8 option not used - save memory
#endif


	
AOUT_Update_Hlp_12_12_8
#if AOUT_LC_RESOLUTION_OPTION_M1 == 2 || AOUT_LC_RESOLUTION_OPTION_M2 == 2 || AOUT_LC_RESOLUTION_OPTION_M3 == 2 || AOUT_LC_RESOLUTION_OPTION_M4 == 2
	;; first channel, and upper bits of second channel already uploaded via 12_4 function
	;; now we have to upload the lower bits of the second channel + 8 bits of the third channel
	;; the fourth channel is ignored
	movlw	1				; second: lower 8bit
	swapf	PLUSW1, W			; [3:0] of high-byte -> MIOS_PARAMETER1[7:4]
	btfsc	MIOS_PARAMETER3, 0		; inversion?
	xorlw	0xf0
	andlw	0xf0
	movwf	MIOS_PARAMETER1
	swapf	INDF1, W			; [7:4] of low-byte -> MIOS_PARAMETER1[3:0]
	andlw	0x0f
	btfsc	MIOS_PARAMETER3, 0		; inversion?
	xorlw	0x0f
	iorwf	MIOS_PARAMETER1, F

	movlw	3				; third channel: 8bit
	movf	PLUSW1, W			; high-byte -> MIOS_PARAMETER2
	btfsc	MIOS_PARAMETER3, 1		; inversion?
	xorlw	0xff
	movwf	MIOS_PARAMETER2
	rgoto	AOUT_LoadWord
#else
	return					; 12/12/8 option not used - save memory
#endif


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
