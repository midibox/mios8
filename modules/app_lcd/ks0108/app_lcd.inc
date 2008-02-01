; $Id: $
;
; MIOS Custom LCD Driver Example
; for KS0107/0108 --- with horizontal screen (-> 240x64)
;
; NOTE:	this is just a template for LCDs which are different to 
;       KS0108 (that is natively supported by MIOS -> LCD type #1)
;
; ==========================================================================
;
;  Copyright (C) 2003  Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Following system variables are given by MIOS and can be directly 
;;  accessed by the driver. The addresses are defined in mios.h and
;;  should not be changed
;; 
;;  MIOS_GLCD_BUFFER	     a 8 byte buffer for data transfers
;;  MIOS_LCD_OPTION1	     contains the first LCD option given by MIOS_LCD_TypeSet
;;  MIOS_LCD_OPTION2	     contains the second LCD option given by MIOS_LCD_TypeSet
;;  MIOS_LCD_CURSOR_POS	     the current cursor pos of characters (GLCD: multiplied by width)
;;  MIOS_GLCD_GCURSOR_X	     for GLCDs: the current X position of graphical cursor
;;  MIOS_GLCD_GCURSOR_Y	     for GLCDs: the current Y position of graphical cursor
;;  MIOS_GLCD_FONT_WIDTH     for GLCDs: the fontwidth given by MIOS_GLCD_FontInit
;;  MIOS_GLCD_FONT_HEIGHT    for GLCDs: the fontheight given by MIOS_GLCD_FontInit
;;  MIOS_GLCD_FONT_X0        for GLCDs: the first byte within a char entry
;;  MIOS_GLCD_FONT_OFFSET    for GLCDs: the byte offset between the characters
;;  MIOS_GLCD_FONT_PTRL      for GLCDs: pointer to the character table, low-byte
;;  MIOS_GLCD_FONT_PTRH      for GLCDs: pointer to the character table, high-byte
;;  MIOS_LCD_TIMEOUT0        can be used for timeout loops
;;  MIOS_LCD_TIMEOUT1        can be used for timeout loops
;;  MIOS_GLCD_TMP1           can be used as temporary buffer
;;  MIOS_GLCD_TMP2           can be used as temporary buffer
;;  MIOS_GLCD_TMP3           can be used as temporary buffer
;;  MIOS_GLCD_TMP4           can be used as temporary buffer
;;  MIOS_LCD_Y0_OFFSET       Y0 offset of LCD
;;  MIOS_LCD_Y1_OFFSET       Y1 offset of LCD
;;  MIOS_LCD_Y2_OFFSET       Y2 offset of LCD
;;  MIOS_LCD_Y3_OFFSET       Y3 offset of LCD
;;  MIOS_LCD_CURSOR_POS_REAL unmapped cursor position which has been set with MIOS_LCD_CursorSet
;; 
;;  Note: the addresses are located in an upper bank and therefore have to
;;        be accessed with the BANKED flag. Also the BSR has to be justified
;;        before using the registers
;;  Example:
;;	SET_BSR MIOS_LCD_OPTION1	; sets BSR to the bank where MIOS_LCD_*
;;					; has been located. You don't need to
;;					; change the BSR for the other LCD registers
;;      movf    MIOS_LCD_OPTION1, W, BANKED	; get LCD option #1
;; 
;;  Important: to allow a proper interaction with MIOS applications, you are
;;  only allowed to modify MIOS_PARAMETER[123], the mutliplication registers
;;  and FSR1. You are not allowed to change TMP[1-5] or FSR0
;;  if you need some temporary registers, use the given addresses above or
;;  locate them to addresses which are not used by the application
;; --------------------------------------------------------------------------
	
;; Pins of LC-Display
USER_LCD_LAT_D		EQU	LATB	; Pin B.7-0
USER_LCD_PORT_D		EQU	PORTB
USER_LCD_TRIS_D		EQU	TRISB

USER_LCD_LAT_E		EQU	LATD	; Pin D.7
USER_LCD_PIN_E		EQU	7             
USER_LCD_LAT_RW		EQU	LATD	; Pin D.6
USER_LCD_PIN_RW		EQU	6
USER_LCD_LAT_RS		EQU	LATD	; Pin D.5
USER_LCD_PIN_RS		EQU	5

USER_LCD_LAT_CS1	EQU	LATC		; Pin C.5
USER_LCD_PIN_CS1	EQU	5
USER_LCD_LAT_CS2	EQU	LATC		; Pin C.4
USER_LCD_PIN_CS2	EQU	4
USER_LCD_LAT_CS3	EQU	LATD		; Pin D.0
USER_LCD_PIN_CS3	EQU	0
USER_LCD_LAT_CS4	EQU	LATC		; Pin C.2
USER_LCD_PIN_CS4	EQU	2

;; ; CS inversion flag
USER_LCD_CS_INV	EQU	MIOS_LCD_OPTION1

;; TIMEOUT1 is used to enable/disable the four display segments
USER_LCD_SEG_DISABLED EQU	MIOS_LCD_TIMEOUT1


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the custom LCD should be initialized
;;  In:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_Init
	;; notify that a graphical LCD has been connected
	bsf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD

	; (Initialization of Ports: done in Init_Ports)
	SET_BSR	USER_LCD_SEG_DISABLED
	clrf	USER_LCD_SEG_DISABLED, BANKED	; see USER_LCD_WaitUnbusy for the purpose

	movlw	50			; 50 ms delay
	call	MIOS_Delay

	movlw	0x3e + 1		; Display On command
	rcall	USER_LCD_Cmd

	rgoto	USER_LCD_Clear


;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_Data
;;  DESCRIPTION: sends a data value to the LCD display.<BR>
;;  On CLCDs: branch directly to USER_LCD_PrintChar<BR>
;;  On GLCDs: ignore this function!
;;  IN:  data which should be sent
;;  OUT: -
;; --------------------------------------------------------------------------
USER_LCD_Data
	;; store byte in data latch
	movwf	USER_LCD_LAT_D

	;; wait until LCD unbusy - zero flag set when segment enabled
	rcall	USER_LCD_WaitUnbusy
	bnz	USER_LCD_Data_End

	;; select data register
        bsf     USER_LCD_LAT_RS, USER_LCD_PIN_RS

	;; set write
        bcf     USER_LCD_LAT_RW, USER_LCD_PIN_RW	; LCD_WRITE

	IRQ_DISABLE
	;; determine chip select line(s)
	rcall	USER_LCD_DetermineCS

	;; strobe bus
        bsf     USER_LCD_LAT_E, USER_LCD_PIN_E
	nop
	nop
	nop
	nop
        bcf     USER_LCD_LAT_E, USER_LCD_PIN_E
	IRQ_ENABLE

USER_LCD_Data_End
	;; increment graphical cursor
	incf	MIOS_GLCD_GCURSOR_X, F, BANKED

	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	andlw	0x3f
	skpz
	return

	movlw	0x40
	;; 	rgoto	USER_LCD_Cmd


;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_Cmd
;;  DESCRIPTION: sends a command to the LCD display.<BR>
;;  On CLCDs: use this function to decode the HD44780 commands if required<BR>
;;  On GLCDs: ignore this function!
;;  IN:  command which should be sent
;;  OUT: -
;; --------------------------------------------------------------------------
USER_LCD_Cmd
	;; store byte in data latch
	movwf	USER_LCD_LAT_D

	;; wait until LCD unbusy - zero flag set when segment enabled
	rcall	USER_LCD_WaitUnbusy
	bnz	USER_LCD_Cmd_End

	;; select command register
        bcf     USER_LCD_LAT_RS, USER_LCD_PIN_RS

	;; set write
        bcf     USER_LCD_LAT_RW, USER_LCD_PIN_RW	; LCD_WRITE

	IRQ_DISABLE
	;; determine chip select line(s)
	rcall	USER_LCD_DetermineCS

	;; strobe bus
        bsf     USER_LCD_LAT_E, USER_LCD_PIN_E
	nop
	nop
	nop
	nop
        bcf     USER_LCD_LAT_E, USER_LCD_PIN_E
	IRQ_ENABLE

USER_LCD_Cmd_End
	return


;; --------------------------------------------------------------------------
;;  This function is NOT called by MIOS, but only used by the custom driver
;;  to wait until the LCD is not busy
;;  In:  -
;;  Out: sets the zero flag when display enabled
;; --------------------------------------------------------------------------
USER_LCD_WaitUnbusy
	;; exit if LCD not available due to timeout
	rcall	USER_LCD_GetCSMask
	andwf	USER_LCD_SEG_DISABLED, W, BANKED
	bnz	USER_LCD_WaitUnbusy_End

	;; turn off output drivers
	setf	USER_LCD_TRIS_D

	;; select command register
        bcf     USER_LCD_LAT_RS, USER_LCD_PIN_RS

	;; set read
        bsf     USER_LCD_LAT_RW, USER_LCD_PIN_RW	; LCD_READ

	;; poll busy bit
	clrf	MIOS_LCD_TIMEOUT0, BANKED
USER_LCD_WaitUnbusy_Loop
        bcf     USER_LCD_LAT_E, USER_LCD_PIN_E
	IRQ_ENABLE

	;; increment timeout counter
	incf	MIOS_LCD_TIMEOUT0, F, BANKED
	; leave loop on overrun and disable segment
	bz	USER_LCD_WaitUnbusy_Disable
		
	IRQ_DISABLE
	;; determine chip select lines
	rcall	USER_LCD_DetermineCS

	;; strobe bus
        bsf     USER_LCD_LAT_E, USER_LCD_PIN_E
	nop
	nop
	nop
	;; check bit 7 of command register
	BRA_IFSET USER_LCD_PORT_D, 7, ACCESS, USER_LCD_WaitUnbusy_Loop
        bcf     USER_LCD_LAT_E, USER_LCD_PIN_E

	;; set zero flag
	bsf	STATUS, Z

USER_LCD_WaitUnbusy_End
	;; enable interrupts again
	IRQ_ENABLE
	;; enable output driver again
        clrf    USER_LCD_TRIS_D
	return

USER_LCD_WaitUnbusy_Disable
	;; LCD timeout: set disable bit, busy routine will never be called again for this segment
	rcall	USER_LCD_GetCSMask
	iorwf	USER_LCD_SEG_DISABLED, F, BANKED

	;; clear zero flag (just to ensure, the last operation already cleared it)
	bcf	STATUS, Z

	rgoto	USER_LCD_WaitUnbusy_End


;; --------------------------------------------------------------------------
;;  Determine Chip Select Line
;; --------------------------------------------------------------------------
USER_LCD_DetermineCS
	SET_BSR	MIOS_LCD_OPTION1
	BRA_IFSET MIOS_LCD_OPTION1, 0, ACCESS, USER_LCD_DetermineCSInv

USER_LCD_DetermineCSNonInv
	BRA_IFSET USER_LCD_LAT_RS, USER_LCD_PIN_RS, ACCESS, USER_LCD_DetermineCS_Data
	BRA_IFSET USER_LCD_LAT_RW, USER_LCD_PIN_RW, ACCESS, USER_LCD_DetermineCS_Data

	;; set all chip select lines on commands
        bsf     USER_LCD_LAT_CS1, USER_LCD_PIN_CS1
        bsf     USER_LCD_LAT_CS2, USER_LCD_PIN_CS2
        bsf     USER_LCD_LAT_CS3, USER_LCD_PIN_CS3
        bsf     USER_LCD_LAT_CS4, USER_LCD_PIN_CS4
	return

USER_LCD_DetermineCS_Data
	;; set only one chip select line on data transfers
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 7, BANKED, USER_LCD_DetermineCS_34
USER_LCD_DetermineCS_12
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 6, BANKED, USER_LCD_DetermineCS_2
USER_LCD_DetermineCS_1
        bsf     USER_LCD_LAT_CS1, USER_LCD_PIN_CS1
        bcf     USER_LCD_LAT_CS2, USER_LCD_PIN_CS2
        bcf     USER_LCD_LAT_CS3, USER_LCD_PIN_CS3
        bcf     USER_LCD_LAT_CS4, USER_LCD_PIN_CS4
	return
USER_LCD_DetermineCS_2
        bcf     USER_LCD_LAT_CS1, USER_LCD_PIN_CS1
        bsf     USER_LCD_LAT_CS2, USER_LCD_PIN_CS2
        bcf     USER_LCD_LAT_CS3, USER_LCD_PIN_CS3
        bcf     USER_LCD_LAT_CS4, USER_LCD_PIN_CS4
	return
	
USER_LCD_DetermineCS_34
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 6, BANKED, USER_LCD_DetermineCS_4
USER_LCD_DetermineCS_3
        bcf     USER_LCD_LAT_CS1, USER_LCD_PIN_CS1
        bcf     USER_LCD_LAT_CS2, USER_LCD_PIN_CS2
        bsf     USER_LCD_LAT_CS3, USER_LCD_PIN_CS3
        bcf     USER_LCD_LAT_CS4, USER_LCD_PIN_CS4
	return
USER_LCD_DetermineCS_4
        bcf     USER_LCD_LAT_CS1, USER_LCD_PIN_CS1
        bcf     USER_LCD_LAT_CS2, USER_LCD_PIN_CS2
        bcf     USER_LCD_LAT_CS3, USER_LCD_PIN_CS3
        bsf     USER_LCD_LAT_CS4, USER_LCD_PIN_CS4
	return


;; ----

USER_LCD_DetermineCSInv
	BRA_IFSET USER_LCD_LAT_RS, USER_LCD_PIN_RS, ACCESS, USER_LCD_DetermineCSInv_Data
	BRA_IFSET USER_LCD_LAT_RW, USER_LCD_PIN_RW, ACCESS, USER_LCD_DetermineCSInv_Data

	;; set all chip select lines on commands
        bcf     USER_LCD_LAT_CS1, USER_LCD_PIN_CS1
        bcf     USER_LCD_LAT_CS2, USER_LCD_PIN_CS2
        bcf     USER_LCD_LAT_CS3, USER_LCD_PIN_CS3
        bcf     USER_LCD_LAT_CS4, USER_LCD_PIN_CS4
	return

USER_LCD_DetermineCSInv_Data
	;; set only one chip select line on data transfers
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 7, BANKED, USER_LCD_DetermineCSInv_34
USER_LCD_DetermineCSInv_12
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 6, BANKED, USER_LCD_DetermineCSInv_2
USER_LCD_DetermineCSInv_1
        bcf     USER_LCD_LAT_CS1, USER_LCD_PIN_CS1
        bsf     USER_LCD_LAT_CS2, USER_LCD_PIN_CS2
        bsf     USER_LCD_LAT_CS3, USER_LCD_PIN_CS3
        bsf     USER_LCD_LAT_CS4, USER_LCD_PIN_CS4
	return
USER_LCD_DetermineCSInv_2
        bsf     USER_LCD_LAT_CS1, USER_LCD_PIN_CS1
        bcf     USER_LCD_LAT_CS2, USER_LCD_PIN_CS2
        bsf     USER_LCD_LAT_CS3, USER_LCD_PIN_CS3
        bsf     USER_LCD_LAT_CS4, USER_LCD_PIN_CS4
	return
	
USER_LCD_DetermineCSInv_34
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 6, BANKED, USER_LCD_DetermineCSInv_4
USER_LCD_DetermineCSInv_3
        bsf     USER_LCD_LAT_CS1, USER_LCD_PIN_CS1
        bsf     USER_LCD_LAT_CS2, USER_LCD_PIN_CS2
        bcf     USER_LCD_LAT_CS3, USER_LCD_PIN_CS3
        bsf     USER_LCD_LAT_CS4, USER_LCD_PIN_CS4
	return
USER_LCD_DetermineCSInv_4
        bsf     USER_LCD_LAT_CS1, USER_LCD_PIN_CS1
        bsf     USER_LCD_LAT_CS2, USER_LCD_PIN_CS2
        bsf     USER_LCD_LAT_CS3, USER_LCD_PIN_CS3
        bcf     USER_LCD_LAT_CS4, USER_LCD_PIN_CS4
	return


;; --------------------------------------------------------------------------
;;  returns the CS mask [3..0]
;; --------------------------------------------------------------------------
USER_LCD_GetCSMask
	SET_BSR	MIOS_GLCD_GCURSOR_X
	BRA_IFSET MIOS_GLCD_GCURSOR_X, 7, BANKED, USER_LCD_GetCSMask_48
USER_LCD_GetCSMask_12
	btfss	MIOS_GLCD_GCURSOR_X, 6
	retlw	(1 << 0)
	retlw	(1 << 1)
USER_LCD_GetCSMask_48
	btfss	MIOS_GLCD_GCURSOR_X, 6, BANKED
	retlw	(1 << 2)
	retlw	(1 << 3)


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the custom LCD should be cleared
;;  In:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_Clear
	SET_BSR MIOS_GLCD_GCURSOR_Y	; 8 lines to clear
	clrf	MIOS_GLCD_GCURSOR_Y, BANKED
USER_LCD_ClearOuterLoop
	SET_BSR MIOS_GLCD_GCURSOR_X	; 240 columns to clear
	clrf	MIOS_GLCD_GCURSOR_X, BANKED
	rcall	USER_LCD_CursorSet

USER_LCD_ClearInnerLoop
	movlw	0x00
	rcall	USER_LCD_Data
	movlw	240
	cpfseq	MIOS_GLCD_GCURSOR_X, BANKED
	rgoto	USER_LCD_ClearInnerLoop

	incf	MIOS_GLCD_GCURSOR_Y, F, BANKED
	BRA_IFCLR MIOS_GLCD_GCURSOR_Y, 3, BANKED, USER_LCD_ClearOuterLoop

	movlw	0xc0 + 0		; Set Y0=0
	rcall	USER_LCD_Cmd

	movlw	0x00			; Set Y=0, X=0
	goto	MIOS_LCD_CursorSet

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the cursor should be changed
;;  In:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;       MIOS_GLCD_CURSOR_X - horizontal cursor position (for GLCDs)
;;       MIOS_GLCD_CURSOR_Y - vertical cursor position   (for GLCDs)
;;       MIOS_LCD_CURSOR_POS - character cursor position  (for CLCDs)
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_CursorSet
USER_LCD_GCursorSet
	;; Set X position
	SET_BSR	MIOS_GLCD_GCURSOR_X
	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	andlw	0x3f
	iorlw	0x40
	rcall	USER_LCD_Cmd

	;; set Y position
	movf	MIOS_GLCD_GCURSOR_Y, W, BANKED
	andlw	0x07
	iorlw	0xb8
	rgoto	USER_LCD_Cmd

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a character should be print
;;  In:  WREG - character
;;       all other MIOS_*LCD_* registers
;;  Out: GLCDs should justify the X/Y cursor position
;; --------------------------------------------------------------------------
USER_LCD_PrintChar
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

	movf	MIOS_GLCD_FONT_PTRL, W, BANKED
	addwf	MIOS_GLCD_FONT_X0, W, BANKED
	addwf	PRODL, W
	movwf	TBLPTRL
	movf	MIOS_GLCD_FONT_PTRH, W, BANKED
	addwfc	PRODH, W
	movwf	TBLPTRH

	movf	MIOS_GLCD_FONT_HEIGHT, W, BANKED
	movwf	FSR1H
USER_LCD_PrintCharOuterLoop
	movf	MIOS_GLCD_FONT_WIDTH, W, BANKED
	movwf	FSR1L
USER_LCD_PrintCharLoop
	tblrd*+				; read from flash and increment table pointer
	movf	TABLAT, W		; get result
	rcall	USER_LCD_Data		; write out
	decfsz	FSR1L, F		; loop until zero
	rgoto	USER_LCD_PrintCharLoop

	dcfsnz	FSR1H, F
	rgoto	USER_LCD_PrintCharLoop_End

	movf	MIOS_GLCD_FONT_WIDTH, W, BANKED
	subwf	MIOS_GLCD_FONT_OFFSET, W, BANKED
	bz	USER_LCD_PrintCharFixLoopEnd
	movwf	FSR1L
USER_LCD_PrintCharFixLoop
	tblrd*+
	decfsz	FSR1L, F
	rgoto	USER_LCD_PrintCharFixLoop

USER_LCD_PrintCharFixLoopEnd
	
	incf	MIOS_GLCD_GCURSOR_Y, F, BANKED
	movf	MIOS_GLCD_FONT_WIDTH, W, BANKED
	subwf	MIOS_GLCD_GCURSOR_X, F, BANKED
	rcall	USER_LCD_GCursorSet

	rgoto	USER_LCD_PrintCharOuterLoop

USER_LCD_PrintCharLoop_End
	decf	MIOS_GLCD_FONT_HEIGHT, W, BANKED
	bz	USER_LCD_PrintChar_NoYFix
	comf	MIOS_GLCD_FONT_HEIGHT, W, BANKED
	addlw	2
	addwf	MIOS_GLCD_GCURSOR_Y, F, BANKED
	rcall	USER_LCD_GCursorSet
USER_LCD_PrintChar_NoYFix
	movff	MIOS_GLCD_TMP1, TBLPTRL		; restore TBLPTR from temp. register
	movff	MIOS_GLCD_TMP2, TBLPTRH
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_SpecialCharInit
;;  DESCRIPTION: see MIOS_CLCD_SpecialCharInit
;;  IN:  number of special character (0-7) in WREG
;;       pointer to special char pattern in TBLPTR (consists of 8
;;          entries for every character-line)
;;  OUT: TBLPTR has to be set to next table entry (TBLPTR+=8)
;; --------------------------------------------------------------------------
USER_LCD_SpecialCharInit
	;; (not used by this driver)
	return
