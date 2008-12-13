; $Id$
;
; MIOS Dotmatrix graphical LCD Driver (1st layer routines)
; for KS0108 and HD61202 compatible displays
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; driver can be optionally disabled
#if MIOS_ENABLE_GLCD0


;; Pins of LC-Display
MIOS_GLCD0_LAT_D	EQU	LATB	; Pin B.7-0
MIOS_GLCD0_PORT_D	EQU	PORTB
MIOS_GLCD0_TRIS_D	EQU	TRISB

MIOS_GLCD0_LAT_E	EQU	LATD	; Pin D.7
MIOS_GLCD0_PIN_E	EQU	7             
MIOS_GLCD0_LAT_RW	EQU	LATD	; Pin D.6
MIOS_GLCD0_PIN_RW	EQU	6
MIOS_GLCD0_LAT_RS	EQU	LATD	; Pin D.5
MIOS_GLCD0_PIN_RS	EQU	5

MIOS_GLCD0_LAT_CS1	EQU	LATC		; Pin C.5
MIOS_GLCD0_PIN_CS1	EQU	5
MIOS_GLCD0_LAT_CS2	EQU	LATC		; Pin C.4
MIOS_GLCD0_PIN_CS2	EQU	4
MIOS_GLCD0_LAT_CS3	EQU	LATD		; Pin D.0
MIOS_GLCD0_PIN_CS3	EQU	0
MIOS_GLCD0_LAT_CS4	EQU	LATC		; Pin C.2
MIOS_GLCD0_PIN_CS4	EQU	2

;; ; CS inversion flag
MIOS_GLCD0_CS_INV	EQU	MIOS_LCD_OPTION1

;; TIMEOUT1 is used to enable/disable the four display segments
MIOS_GLCD0_SEG_DISABLED EQU	MIOS_LCD_TIMEOUT1

;; --------------------------------------------------------------------------
;;  Init Routine for graphic LC Display
;; --------------------------------------------------------------------------
MIOS_GLCD0_Init
	;; notify that a graphical LCD has been connected
	bsf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD

	; (Initialization of Ports: done in Init_Ports)
	SET_BSR	MIOS_GLCD0_SEG_DISABLED
	clrf	MIOS_GLCD0_SEG_DISABLED, BANKED	; see MIOS_GLCD0_WaitUnbusy for the purpose

	movlw	50			; 50 ms delay
	call	MIOS_Delay

	movlw	0x3e + 1		; Display On command
	rcall	MIOS_GLCD0_Cmd

	rgoto	MIOS_GLCD0_Clear
	
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Send data word to display and increment cursor
;; --------------------------------------------------------------------------
MIOS_GLCD0_Data
	;; store byte in data latch
	movwf	MIOS_GLCD0_LAT_D

	;; wait until LCD unbusy - zero flag set when segment enabled
	rcall	MIOS_GLCD0_WaitUnbusy
	bnz	MIOS_GLCD0_Data_End

	;; select data register
        bsf     MIOS_GLCD0_LAT_RS, MIOS_GLCD0_PIN_RS

	;; set write
        bcf     MIOS_GLCD0_LAT_RW, MIOS_GLCD0_PIN_RW	; LCD_WRITE

	IRQ_DISABLE
	;; determine chip select line(s)
	rcall	MIOS_GLCD0_DetermineCS

	;; strobe bus
        bsf     MIOS_GLCD0_LAT_E, MIOS_GLCD0_PIN_E
	nop
	nop
	nop
	nop
        bcf     MIOS_GLCD0_LAT_E, MIOS_GLCD0_PIN_E
	IRQ_ENABLE

MIOS_GLCD0_Data_End
	;; increment graphical cursor
	incf	MIOS_GLCD_GCURSOR_X, F, BANKED

	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	andlw	0x3f
	skpz
	return

	movlw	0x40
	;; 	rgoto	MIOS_GLCD0_Cmd

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Send display command
;; --------------------------------------------------------------------------
MIOS_GLCD0_Cmd
	;; store byte in data latch
	movwf	MIOS_GLCD0_LAT_D

	;; wait until LCD unbusy - zero flag set when segment enabled
	rcall	MIOS_GLCD0_WaitUnbusy
	bnz	MIOS_GLCD0_Cmd_End

	;; select command register
        bcf     MIOS_GLCD0_LAT_RS, MIOS_GLCD0_PIN_RS

	;; set write
        bcf     MIOS_GLCD0_LAT_RW, MIOS_GLCD0_PIN_RW	; LCD_WRITE

	IRQ_DISABLE
	;; determine chip select line(s)
	rcall	MIOS_GLCD0_DetermineCS

	;; strobe bus
        bsf     MIOS_GLCD0_LAT_E, MIOS_GLCD0_PIN_E
	nop
	nop
	nop
	nop
        bcf     MIOS_GLCD0_LAT_E, MIOS_GLCD0_PIN_E
	IRQ_ENABLE

MIOS_GLCD0_Cmd_End
	return

;; --------------------------------------------------------------------------
;;  INTERNAL: wait until LCD unbusy
;;  In:  -
;;  Out: sets the zero flag when display enabled
;; --------------------------------------------------------------------------
MIOS_GLCD0_WaitUnbusy
	;; exit if LCD not available due to timeout
	rcall	MIOS_GLCD0_GetCSMask
	andwf	MIOS_GLCD0_SEG_DISABLED, W, BANKED
	bnz	MIOS_GLCD0_WaitUnbusy_End

	;; turn off output drivers
	setf	MIOS_GLCD0_TRIS_D

	;; select command register
        bcf     MIOS_GLCD0_LAT_RS, MIOS_GLCD0_PIN_RS

	;; set read
        bsf     MIOS_GLCD0_LAT_RW, MIOS_GLCD0_PIN_RW	; LCD_READ

	;; poll busy bit
	clrf	MIOS_LCD_TIMEOUT0, BANKED
MIOS_GLCD0_WaitUnbusy_Loop
        bcf     MIOS_GLCD0_LAT_E, MIOS_GLCD0_PIN_E
	IRQ_ENABLE

	;; increment timeout counter
	incf	MIOS_LCD_TIMEOUT0, F, BANKED
	; leave loop on overrun and disable segment
	bz	MIOS_GLCD0_WaitUnbusy_Disable
		
	IRQ_DISABLE
	;; determine chip select lines
	rcall	MIOS_GLCD0_DetermineCS

	;; strobe bus
        bsf     MIOS_GLCD0_LAT_E, MIOS_GLCD0_PIN_E
	nop
	nop
	nop
	;; check bit 7 of command register
	BRA_IFSET MIOS_GLCD0_PORT_D, 7, ACCESS, MIOS_GLCD0_WaitUnbusy_Loop
        bcf     MIOS_GLCD0_LAT_E, MIOS_GLCD0_PIN_E

	;; set zero flag
	bsf	STATUS, Z

MIOS_GLCD0_WaitUnbusy_End
	;; enable interrupts again
	IRQ_ENABLE
	;; enable output driver again
        clrf    MIOS_GLCD0_TRIS_D
	return

MIOS_GLCD0_WaitUnbusy_Disable
	;; LCD timeout: set disable bit, busy routine will never be called again for this segment
	rcall	MIOS_GLCD0_GetCSMask
	iorwf	MIOS_GLCD0_SEG_DISABLED, F, BANKED

	;; clear zero flag (just to ensure, the last operation already cleared it)
	bcf	STATUS, Z

	rgoto	MIOS_GLCD0_WaitUnbusy_End


;; --------------------------------------------------------------------------
;;  Determine Chip Select Line
;; --------------------------------------------------------------------------
MIOS_GLCD0_DetermineCS
	SET_BSR	MIOS_LCD_OPTION1
	BRA_IFSET MIOS_LCD_OPTION1, 0, BANKED, MIOS_GLCD0_DetermineCSInv

MIOS_GLCD0_DetermineCSNonInv
	BRA_IFSET MIOS_GLCD0_LAT_RS, MIOS_GLCD0_PIN_RS, ACCESS, MIOS_GLCD0_DetermineCS_Data
	BRA_IFSET MIOS_GLCD0_LAT_RW, MIOS_GLCD0_PIN_RW, ACCESS, MIOS_GLCD0_DetermineCS_Data

	;; set all chip select lines on commands
        bsf     MIOS_GLCD0_LAT_CS1, MIOS_GLCD0_PIN_CS1
        bsf     MIOS_GLCD0_LAT_CS2, MIOS_GLCD0_PIN_CS2
        bsf     MIOS_GLCD0_LAT_CS3, MIOS_GLCD0_PIN_CS3
        bsf     MIOS_GLCD0_LAT_CS4, MIOS_GLCD0_PIN_CS4
	return

MIOS_GLCD0_DetermineCS_Data
	;; set only one chip select line on data transfers
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 7, BANKED, MIOS_GLCD0_DetermineCS_34
MIOS_GLCD0_DetermineCS_12
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 6, BANKED, MIOS_GLCD0_DetermineCS_2
MIOS_GLCD0_DetermineCS_1
        bsf     MIOS_GLCD0_LAT_CS1, MIOS_GLCD0_PIN_CS1
        bcf     MIOS_GLCD0_LAT_CS2, MIOS_GLCD0_PIN_CS2
        bcf     MIOS_GLCD0_LAT_CS3, MIOS_GLCD0_PIN_CS3
        bcf     MIOS_GLCD0_LAT_CS4, MIOS_GLCD0_PIN_CS4
	return
MIOS_GLCD0_DetermineCS_2
        bcf     MIOS_GLCD0_LAT_CS1, MIOS_GLCD0_PIN_CS1
        bsf     MIOS_GLCD0_LAT_CS2, MIOS_GLCD0_PIN_CS2
        bcf     MIOS_GLCD0_LAT_CS3, MIOS_GLCD0_PIN_CS3
        bcf     MIOS_GLCD0_LAT_CS4, MIOS_GLCD0_PIN_CS4
	return
	
MIOS_GLCD0_DetermineCS_34
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 6, BANKED, MIOS_GLCD0_DetermineCS_4
MIOS_GLCD0_DetermineCS_3
        bcf     MIOS_GLCD0_LAT_CS1, MIOS_GLCD0_PIN_CS1
        bcf     MIOS_GLCD0_LAT_CS2, MIOS_GLCD0_PIN_CS2
        bsf     MIOS_GLCD0_LAT_CS3, MIOS_GLCD0_PIN_CS3
        bcf     MIOS_GLCD0_LAT_CS4, MIOS_GLCD0_PIN_CS4
	return
MIOS_GLCD0_DetermineCS_4
        bcf     MIOS_GLCD0_LAT_CS1, MIOS_GLCD0_PIN_CS1
        bcf     MIOS_GLCD0_LAT_CS2, MIOS_GLCD0_PIN_CS2
        bcf     MIOS_GLCD0_LAT_CS3, MIOS_GLCD0_PIN_CS3
        bsf     MIOS_GLCD0_LAT_CS4, MIOS_GLCD0_PIN_CS4
	return


;; ----

MIOS_GLCD0_DetermineCSInv
	BRA_IFSET MIOS_GLCD0_LAT_RS, MIOS_GLCD0_PIN_RS, ACCESS, MIOS_GLCD0_DetermineCSInv_Data
	BRA_IFSET MIOS_GLCD0_LAT_RW, MIOS_GLCD0_PIN_RW, ACCESS, MIOS_GLCD0_DetermineCSInv_Data

	;; set all chip select lines on commands
        bcf     MIOS_GLCD0_LAT_CS1, MIOS_GLCD0_PIN_CS1
        bcf     MIOS_GLCD0_LAT_CS2, MIOS_GLCD0_PIN_CS2
        bcf     MIOS_GLCD0_LAT_CS3, MIOS_GLCD0_PIN_CS3
        bcf     MIOS_GLCD0_LAT_CS4, MIOS_GLCD0_PIN_CS4
	return

MIOS_GLCD0_DetermineCSInv_Data
	;; set only one chip select line on data transfers
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 7, BANKED, MIOS_GLCD0_DetermineCSInv_34
MIOS_GLCD0_DetermineCSInv_12
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 6, BANKED, MIOS_GLCD0_DetermineCSInv_2
MIOS_GLCD0_DetermineCSInv_1
        bcf     MIOS_GLCD0_LAT_CS1, MIOS_GLCD0_PIN_CS1
        bsf     MIOS_GLCD0_LAT_CS2, MIOS_GLCD0_PIN_CS2
        bsf     MIOS_GLCD0_LAT_CS3, MIOS_GLCD0_PIN_CS3
        bsf     MIOS_GLCD0_LAT_CS4, MIOS_GLCD0_PIN_CS4
	return
MIOS_GLCD0_DetermineCSInv_2
        bsf     MIOS_GLCD0_LAT_CS1, MIOS_GLCD0_PIN_CS1
        bcf     MIOS_GLCD0_LAT_CS2, MIOS_GLCD0_PIN_CS2
        bsf     MIOS_GLCD0_LAT_CS3, MIOS_GLCD0_PIN_CS3
        bsf     MIOS_GLCD0_LAT_CS4, MIOS_GLCD0_PIN_CS4
	return
	
MIOS_GLCD0_DetermineCSInv_34
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 6, BANKED, MIOS_GLCD0_DetermineCSInv_4
MIOS_GLCD0_DetermineCSInv_3
        bsf     MIOS_GLCD0_LAT_CS1, MIOS_GLCD0_PIN_CS1
        bsf     MIOS_GLCD0_LAT_CS2, MIOS_GLCD0_PIN_CS2
        bcf     MIOS_GLCD0_LAT_CS3, MIOS_GLCD0_PIN_CS3
        bsf     MIOS_GLCD0_LAT_CS4, MIOS_GLCD0_PIN_CS4
	return
MIOS_GLCD0_DetermineCSInv_4
        bsf     MIOS_GLCD0_LAT_CS1, MIOS_GLCD0_PIN_CS1
        bsf     MIOS_GLCD0_LAT_CS2, MIOS_GLCD0_PIN_CS2
        bsf     MIOS_GLCD0_LAT_CS3, MIOS_GLCD0_PIN_CS3
        bcf     MIOS_GLCD0_LAT_CS4, MIOS_GLCD0_PIN_CS4
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  returns the CS mask [3..0]
;; --------------------------------------------------------------------------
MIOS_GLCD0_GetCSMask
	SET_BSR	MIOS_GLCD_GCURSOR_X
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 7, BANKED, MIOS_GLCD0_GetCSMask_48
MIOS_GLCD0_GetCSMask_12
	btfss	MIOS_GLCD_GCURSOR_X, 6, BANKED
	retlw (1 << 0)
	retlw	(1 << 1)
MIOS_GLCD0_GetCSMask_48
	btfss	MIOS_GLCD_GCURSOR_X, 6, BANKED
	retlw (1 << 2)
	retlw	(1 << 3)

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Write a 5x8 character on display
;;  (compatible to MIOS_CLCD_PrintChar)
;; --------------------------------------------------------------------------
MIOS_GLCD0_PrintChar
	;; calc offset address to character
	SET_BSR	MIOS_GLCD_TMP1
	movwf	MIOS_GLCD_TMP1, BANKED

	;; how much bytes per character?
	movf	MIOS_GLCD_FONT_OFFSET, W, BANKED
	mulwf	MIOS_GLCD_FONT_HEIGHT, BANKED

	;; multiply with character value
	movf	MIOS_GLCD_TMP1, W, BANKED
	mulwf	PRODL

	movff	TBLPTRL, MIOS_GLCD_TMP1		; store current TBLPTR in temp. register
	movff	TBLPTRH, MIOS_GLCD_TMP2
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
	movff	TBLPTRU, MIOS_GLCD_TMP3
#endif

	movf	MIOS_GLCD_FONT_PTRL, W, BANKED
	addwf	MIOS_GLCD_FONT_X0, W, BANKED
	addwf	PRODL, W
	movwf	TBLPTRL
	movf	MIOS_GLCD_FONT_PTRH, W, BANKED
	addwfc	PRODH, W
	movwf	TBLPTRH
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
	movlw	0
	addwfc	MIOS_GLCD_FONT_PTRU, W, BANKED
	movwf	TBLPTRU
#endif

	movf	MIOS_GLCD_FONT_HEIGHT, W, BANKED
	movwf	FSR1H
MIOS_GLCD0_PrintCharOuterLoop
	movf	MIOS_GLCD_FONT_WIDTH, W, BANKED
	movwf	FSR1L
MIOS_GLCD0_PrintCharLoop
	tblrd*+				; read from flash and increment table pointer
	movf	TABLAT, W		; get result
	rcall	MIOS_GLCD0_Data		; write out
	decfsz	FSR1L, F		; loop until zero
	rgoto	MIOS_GLCD0_PrintCharLoop

	dcfsnz	FSR1H, F
	rgoto	MIOS_GLCD0_PrintCharLoop_End

	movf	MIOS_GLCD_FONT_WIDTH, W, BANKED
	subwf	MIOS_GLCD_FONT_OFFSET, W, BANKED
	bz	MIOS_GLCD0_PrintCharFixLoopEnd
	movwf	FSR1L
MIOS_GLCD0_PrintCharFixLoop
	tblrd*+
	decfsz	FSR1L, F
	rgoto	MIOS_GLCD0_PrintCharFixLoop

MIOS_GLCD0_PrintCharFixLoopEnd
	
	incf	MIOS_GLCD_GCURSOR_Y, F, BANKED
	movf	MIOS_GLCD_FONT_WIDTH, W, BANKED
	subwf	MIOS_GLCD_GCURSOR_X, F, BANKED
	rcall	MIOS_GLCD0_GCursorSet

	rgoto	MIOS_GLCD0_PrintCharOuterLoop

MIOS_GLCD0_PrintCharLoop_End
	decf	MIOS_GLCD_FONT_HEIGHT, W, BANKED
	bz	MIOS_GLCD0_PrintChar_NoYFix
	comf	MIOS_GLCD_FONT_HEIGHT, W, BANKED
	addlw	2
	addwf	MIOS_GLCD_GCURSOR_Y, F, BANKED
	rcall	MIOS_GLCD0_GCursorSet
MIOS_GLCD0_PrintChar_NoYFix
	movff	MIOS_GLCD_TMP1, TBLPTRL		; restore TBLPTR from temp. register
	movff	MIOS_GLCD_TMP2, TBLPTRH
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
	movff	MIOS_GLCD_TMP3, TBLPTRU
#endif
	return


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  F-UNCTION: MIOS_GLCD_GCursorSet
;;  DESCRIPTION: sets the graphical cursor on a LCD screen
;;  Note that this function works with graphical LCDs only!
;;  IN:	  X position in WREG (0-239)
;;        Y position in MIOS_PARAMETER1 (0-7)
;;  OUT:  -
;;  USES: BRS, TBLPTR
;;  EXAMPLES:
;;	;; set graphical cursor to 160/7:
;;	movlw	7
;;	movwf	MIOS_PARAMETER1
;;	movlw	160
;;	call	MIOS_GLCD_GCursorSet
;; --------------------------------------------------------------------------
MIOS_GLCD0_GCursorSet
	;; Set X position
	SET_BSR	MIOS_GLCD_GCURSOR_X
	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	andlw	0x3f
	iorlw	0x40
	rcall	MIOS_GLCD0_Cmd

	;; set Y position
	movf	MIOS_GLCD_GCURSOR_Y, W, BANKED
	andlw	0x07
	iorlw	0xb8
	rgoto	MIOS_GLCD0_Cmd

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Clear the display and set cursor to 0
;;  (compatible to MIOS_CLCD_Clear)
;; --------------------------------------------------------------------------
MIOS_GLCD0_Clear
	SET_BSR MIOS_GLCD_GCURSOR_Y	; 8 lines to clear
	clrf	MIOS_GLCD_GCURSOR_Y, BANKED
MIOS_GLCD0_ClearOuterLoop
	SET_BSR MIOS_GLCD_GCURSOR_X	; 256 columns to clear
	clrf	MIOS_GLCD_GCURSOR_X, BANKED
	rcall	MIOS_GLCD0_GCursorSet

MIOS_GLCD0_ClearInnerLoop
	movlw	0x00
	rcall	MIOS_GLCD0_Data
	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	bnz	MIOS_GLCD0_ClearInnerLoop

	incf	MIOS_GLCD_GCURSOR_Y, F, BANKED
	BRA_IFCLR MIOS_GLCD_GCURSOR_Y, 3, BANKED, MIOS_GLCD0_ClearOuterLoop

	movlw	0xc0 + 0		; Set Y0=0
	rcall	MIOS_GLCD0_Cmd

	movlw	0x00			; Set Y=0, X=0
	rgoto	MIOS_GLCD_CursorSet

;; ==========================================================================

#endif	; MIOS_ENABLE_GLCD0

