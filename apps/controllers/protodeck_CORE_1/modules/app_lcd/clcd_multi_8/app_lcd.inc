; $Id: app_lcd.inc 349 2008-06-06 00:10:40Z tk $
;
; MIOS Custom LCD Driver Example for multiple (more than two) character LCDs
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
USER_LCD_PORT_D	EQU     PORTB
USER_LCD_TRIS_D	EQU     TRISB

USER_LCD_LAT_RW	EQU     LATD
USER_LCD_PIN_RW	EQU     6               ; Pin D.6
USER_LCD_LAT_RS	EQU     LATD
USER_LCD_PIN_RS	EQU     5               ; Pin D.5

USER_LCD_LAT_E0	EQU     LATD		; Pin D.7
USER_LCD_PIN_E0	EQU     7
USER_LCD_LAT_E1	EQU     LATC		; Pin C.5
USER_LCD_PIN_E1	EQU     5
USER_LCD_LAT_E2	EQU     LATC		; Pin C.4
USER_LCD_PIN_E2	EQU     4
USER_LCD_LAT_E3	EQU     LATC		; Pin C.3
USER_LCD_TRIS_E3 EQU    TRISC		; Pin C.3
USER_LCD_PIN_E3	EQU     3


;; stryd_one mods start here

USER_LCD_LAT_E4	EQU     LATC		; Pin C.0
USER_LCD_PIN_E4	EQU     0
USER_LCD_LAT_E5	EQU     LATC		; Pin C.1
USER_LCD_PIN_E5	EQU     1
USER_LCD_LAT_E6	EQU     LATC		; Pin C.2
USER_LCD_PIN_E6	EQU     2
USER_LCD_LAT_E7	EQU     LATD		; Pin D.4
USER_LCD_PIN_E7	EQU     4

;; stryd_one mods end here



;; new names for CLCD registers
USER_LCD_STATUS		EQU	MIOS_GLCD_TMP1
USER_LCD_SELECT		EQU	MIOS_GLCD_TMP2
USER_LCD_SC_CTR		EQU	MIOS_GLCD_TMP3


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the custom LCD should be initialized
;;  In:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_Init
	bcf	USER_LCD_TRIS_E3, USER_LCD_PIN_E3
	bcf	USER_LCD_LAT_E3, USER_LCD_PIN_E3

	;; notify that no graphical LCD is connected
	bcf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD

	; (Initialization of Ports: done in Init_Ports)
	SET_BSR	USER_LCD_STATUS
	movlw	0xff		; change to 0xff if 8 CLCD should be connected
	movwf	USER_LCD_STATUS, BANKED

	movlw	100			; 100 ms delay
	call	MIOS_Delay

        bcf     USER_LCD_LAT_RW, USER_LCD_PIN_RW	; LCD_WRITE
        bcf     USER_LCD_LAT_RS, USER_LCD_PIN_RS	; USER_LCD_PIN_RS_0

	;; initialize LCD
	clrf	USER_LCD_SELECT, BANKED
USER_LCD_Init_Loop
	rcall	USER_LCD_Init_Cmds
	incf	USER_LCD_SELECT, F, BANKED
	BRA_IFCLR USER_LCD_SELECT, 3, BANKED, USER_LCD_Init_Loop	; currently 8 LCDs supported 

	;; switch back to first LCD
	clrf	USER_LCD_SELECT, BANKED
	return


USER_LCD_Init_Cmds
	movlw	0x38
	movwf	USER_LCD_LAT_D
	rcall	USER_LCD_Strobe_Toggle
	movlw	50			; 50 ms delayed toggle
	rcall	USER_LCD_Strobe_DelayToggle
	movlw	50			; 50 ms delayed toggle
	rcall	USER_LCD_Strobe_DelayToggle

	;; switch to 4bit mode
	movlw	0x20
	movwf	USER_LCD_LAT_D
	movlw	1			; 1 ms delayed toggle
	rcall	USER_LCD_Strobe_DelayToggle			; DB7-DB4: 0010 

	clrf	USER_LCD_LAT_D
	movlw	1			; 1 ms delayed toggle
	rcall	USER_LCD_Strobe_DelayToggle			; DB7-DB4: 0010 

	movlw	1			; 1 ms delayed toggle
	rcall	USER_LCD_Strobe_DelayToggle			; DB7-DB4: 0000

	movlw	0x10
	movwf	USER_LCD_LAT_D
	movlw	1			; 1 ms delayed toggle
	rcall	USER_LCD_Strobe_DelayToggle			; DB7-DB4: 0001

	clrf	USER_LCD_LAT_D
	movlw	1			; 1 ms delayed toggle
	rcall	USER_LCD_Strobe_DelayToggle			; DB7-DB4: 0010 

	movlw	0x08			; Display Off
	rcall	USER_LCD_Cmd
	movlw	0x0c			; Display On
	rcall	USER_LCD_Cmd
	movlw	0x06			; Entry Mode
	rcall	USER_LCD_Cmd
	movlw	0x01			; Clear Display
	call	USER_LCD_Cmd
	bcf	MIOS_LCD_TIMEOUT1, 7, BANKED	; everything ok, make sure that LCD_TIMEOUT, bit 7 is cleared
	
	movlw	0x28			; without these lines the LCD will not work
	rcall	USER_LCD_Cmd		; correctly after a second USER_LCD_Init	; (select 4bit interface again)
	movlw	0x0c
	rcall	USER_LCD_Cmd
	movlw	0x00			; set cursor to zero pos
	rgoto	USER_LCD_CursorSet

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

	;; wait until display unbusy
	rcall	USER_LCD_WaitUnbusy

	;; exit if current LCD not available due to timeout
	SET_BSR	USER_LCD_SELECT
	movf	USER_LCD_SELECT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	USER_LCD_STATUS, W, BANKED
	skpnz
	return

	;; select data register
        bsf     USER_LCD_LAT_RS, USER_LCD_PIN_RS

	;; activate write
        bcf     USER_LCD_LAT_RW, USER_LCD_PIN_RW

	;; transfer upper 4 bit
	rcall	USER_LCD_Strobe_Toggle

	;; transfer lower 4 bit and exit
	swapf	USER_LCD_LAT_D, F
	rgoto	USER_LCD_Strobe_Toggle


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

	;; wait until display unbusy
	rcall	USER_LCD_WaitUnbusy

	;; exit if current LCD not available due to timeout
	SET_BSR	USER_LCD_SELECT
	movf	USER_LCD_SELECT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	USER_LCD_STATUS, W, BANKED
	skpnz
	return

	;; select command register
        bcf     USER_LCD_LAT_RS, USER_LCD_PIN_RS

	;; activate write
        bcf     USER_LCD_LAT_RW, USER_LCD_PIN_RW

	;; transfer upper 4 bit
	rcall	USER_LCD_Strobe_Toggle

	;; transfer lower 4 bit and exit
	swapf	USER_LCD_LAT_D, F
	rgoto	USER_LCD_Strobe_Toggle


;; --------------------------------------------------------------------------
;;  This function is NOT called by MIOS, but only used by the custom driver
;;  to wait until the LCD is unbusy
;;  In:  -
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_WaitUnbusy
	;; exit if current LCD not available due to timeout
	SET_BSR	USER_LCD_SELECT
	movf	USER_LCD_SELECT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	USER_LCD_STATUS, W, BANKED
	skpnz
	return

	;; turn off output drivers (upper 4 bits)
	movlw	0xf0
	iorwf	USER_LCD_TRIS_D, F

	;; select command register
        bcf     USER_LCD_LAT_RS, USER_LCD_PIN_RS

	;; poll busy bit
	clrf	MIOS_LCD_TIMEOUT0, BANKED
	clrf	MIOS_LCD_TIMEOUT1, BANKED

        bsf     USER_LCD_LAT_RW, USER_LCD_PIN_RW	; LCD_READ
USER_LCD_WaitUnbusy_Loop
	rcall	USER_LCD_Strobe_Clr
	incf	MIOS_LCD_TIMEOUT0, F, BANKED
	skpnz
	incf	MIOS_LCD_TIMEOUT1, F, BANKED
	bz	USER_LCD_WaitUnbusy_Disable	; leave loop when LCD_TIMEOUT = 0xff. Up to now bit 7 is set and the LCD
						; busy routine will never be called again
	rcall	USER_LCD_Strobe_Set
	BRA_IFCLR USER_LCD_PORT_D, 7, ACCESS, USER_LCD_WaitUnbusy_UnBusy
USER_LCD_WaitUnbusy_Busy
	;; strobe lower 4 bits
	rcall	USER_LCD_Strobe_Clr
	rcall	USER_LCD_Strobe_Set
	rgoto	USER_LCD_WaitUnbusy_Loop

USER_LCD_WaitUnbusy_UnBusy
	;; strobe lower 4 bits
	rcall	USER_LCD_Strobe_Clr
	rcall	USER_LCD_Strobe_Toggle

USER_LCD_WaitUnbusy_End
	;; turn on output drivers again (upper 4 bits)
	movlw	0x0f
	andwf	USER_LCD_TRIS_D, F
	return

USER_LCD_WaitUnbusy_Disable
	;; disable currently selected LCD
	movf	USER_LCD_SELECT, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andwf	USER_LCD_STATUS, F, BANKED
	rgoto	USER_LCD_WaitUnbusy_End


;; --------------------------------------------------------------------------
;;  This function is NOT called by MIOS, but only used by the custom driver
;;  to set the strobe line to logic-1
;;  In:  -
;;  Out: -
;; --------------------------------------------------------------------------
;; stryd_one mods start here
USER_LCD_Strobe_Set
	;; code for variable E output removed
	;; 4 additional enable outputs could be added here 
	movf	USER_LCD_SELECT, W, BANKED
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	USER_LCD_Strobe_Set0
	rgoto	USER_LCD_Strobe_Set1
	rgoto	USER_LCD_Strobe_Set2
	rgoto	USER_LCD_Strobe_Set3
	rgoto	USER_LCD_Strobe_Set4
	rgoto	USER_LCD_Strobe_Set5
	rgoto	USER_LCD_Strobe_Set6
	rgoto	USER_LCD_Strobe_Set7
;; stryd_one mods end here

USER_LCD_Strobe_Set0
	bsf	USER_LCD_LAT_E0, USER_LCD_PIN_E0
	rgoto	USER_LCD_Strobe_Set_End

USER_LCD_Strobe_Set1
	bsf	USER_LCD_LAT_E1, USER_LCD_PIN_E1
	rgoto	USER_LCD_Strobe_Set_End

USER_LCD_Strobe_Set2
	bsf	USER_LCD_LAT_E2, USER_LCD_PIN_E2
	rgoto	USER_LCD_Strobe_Set_End

USER_LCD_Strobe_Set3
	bsf	USER_LCD_LAT_E3, USER_LCD_PIN_E3
	rgoto	USER_LCD_Strobe_Set_End

;; stryd_one mods start here
USER_LCD_Strobe_Set4
	bsf	USER_LCD_LAT_E4, USER_LCD_PIN_E4
	rgoto	USER_LCD_Strobe_Set_End

USER_LCD_Strobe_Set5
	bsf	USER_LCD_LAT_E5, USER_LCD_PIN_E5
	rgoto	USER_LCD_Strobe_Set_End

USER_LCD_Strobe_Set6
	bsf	USER_LCD_LAT_E6, USER_LCD_PIN_E6
	rgoto	USER_LCD_Strobe_Set_End

USER_LCD_Strobe_Set7
	bsf	USER_LCD_LAT_E7, USER_LCD_PIN_E7
	rgoto	USER_LCD_Strobe_Set_End
;; stryd_one mods end here

USER_LCD_Strobe_Set_End
	return

;; --------------------------------------------------------------------------
;;  This function is NOT called by MIOS, but only used by the custom driver
;;  to set the strobe line to logic-0
;;  In:  -
;;  Out: -
;; --------------------------------------------------------------------------
;; stryd_one mods start here
USER_LCD_Strobe_Clr
	;; code for variable E output removed
	;; 4 additional enable outputs could be added here 
	movf	USER_LCD_SELECT, W, BANKED
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	USER_LCD_Strobe_Clr0
	rgoto	USER_LCD_Strobe_Clr1
	rgoto	USER_LCD_Strobe_Clr2
	rgoto	USER_LCD_Strobe_Clr3
	rgoto	USER_LCD_Strobe_Clr4
	rgoto	USER_LCD_Strobe_Clr5
	rgoto	USER_LCD_Strobe_Clr6
	rgoto	USER_LCD_Strobe_Clr7
;; stryd_one mods end here

USER_LCD_Strobe_Clr0
	bcf	USER_LCD_LAT_E0, USER_LCD_PIN_E0
	rgoto	USER_LCD_Strobe_Clr_End

USER_LCD_Strobe_Clr1
	bcf	USER_LCD_LAT_E1, USER_LCD_PIN_E1
	rgoto	USER_LCD_Strobe_Clr_End

USER_LCD_Strobe_Clr2
	bcf	USER_LCD_LAT_E2, USER_LCD_PIN_E2
	rgoto	USER_LCD_Strobe_Clr_End

USER_LCD_Strobe_Clr3
	bcf	USER_LCD_LAT_E3, USER_LCD_PIN_E3
	rgoto	USER_LCD_Strobe_Clr_End

	
;; stryd_one mods start here

USER_LCD_Strobe_Clr4
	bcf	USER_LCD_LAT_E4, USER_LCD_PIN_E4
	rgoto	USER_LCD_Strobe_Clr_End

USER_LCD_Strobe_Clr5
	bcf	USER_LCD_LAT_E5, USER_LCD_PIN_E5
	rgoto	USER_LCD_Strobe_Clr_End

USER_LCD_Strobe_Clr6
	bcf	USER_LCD_LAT_E6, USER_LCD_PIN_E6
	rgoto	USER_LCD_Strobe_Clr_End

USER_LCD_Strobe_Clr7
	bcf	USER_LCD_LAT_E7, USER_LCD_PIN_E7
	rgoto	USER_LCD_Strobe_Clr_End

;; stryd_one mods end here
	
USER_LCD_Strobe_Clr_End
	return



;; --------------------------------------------------------------------------
;;  
;;  In:  -
;;  Out: -
;; --------------------------------------------------------------------------
_USER_LCD_Select
USER_LCD_Select
	SET_BSR	USER_LCD_SELECT
	movwf	USER_LCD_SELECT, BANKED
	return


;; ==========================================================================
;; help function which adds delay before toggling - mS in WREG
USER_LCD_Strobe_DelayToggle
	call	MIOS_Delay
	;; 	rgoto	USER_LCD_Strobe_Toggle

;; help function which toggles the strobe line
USER_LCD_Strobe_Toggle
	rcall	USER_LCD_Strobe_Set
	rgoto	USER_LCD_Strobe_Clr

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the custom LCD should be cleared
;;  In:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_Clear
	movlw	0x01
	call	USER_LCD_Cmd
	SET_BSR	MIOS_LCD_Y2_OFFSET
	BRA_IFSET MIOS_LCD_Y2_OFFSET, 7, BANKED, USER_LCD_Clear2
	BRA_IFSET MIOS_LCD_Y3_OFFSET, 7, BANKED, USER_LCD_Clear2
	return
USER_LCD_Clear2
	bsf	MIOS_LCD_CURSOR_POS, 7, BANKED
	movlw	0x01
	call	USER_LCD_Cmd
	bcf	MIOS_LCD_CURSOR_POS, 7, BANKED
	return

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
	SET_BSR	MIOS_LCD_CURSOR_POS
	movf	MIOS_LCD_CURSOR_POS, W, BANKED
	iorlw	0x80
	rgoto	USER_LCD_Cmd

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a character should be print
;;  In:  WREG - character
;;       all other MIOS_*LCD_* registers
;;  Out: GLCDs should justify the X/Y cursor position
;; --------------------------------------------------------------------------
USER_LCD_PrintChar
	rgoto	USER_LCD_Data


;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_SpecialCharInit
;;  DESCRIPTION: see MIOS_CLCD_SpecialCharInit
;;  IN:  number of special character (0-7) in WREG
;;       pointer to special char pattern in TBLPTR (consists of 8
;;          entries for every character-line)
;;  OUT: TBLPTR has to be set to next table entry (TBLPTR+=8)
;; --------------------------------------------------------------------------
USER_LCD_SpecialCharInit
	;; transfer special character to display
	swapf	WREG, F
	rrf	WREG, W
	andlw	0x38
	iorlw	0x40
	rcall	USER_LCD_Cmd

	SET_BSR	USER_LCD_SC_CTR
	clrf	USER_LCD_SC_CTR, BANKED
USER_LCD_SpecialCharInitLoop
	tblrd*+
	movf	TABLAT, W
	rcall	USER_LCD_Data
	incf	USER_LCD_SC_CTR, F, BANKED
	BRA_IFCLR USER_LCD_SC_CTR, 3, BANKED, USER_LCD_SpecialCharInitLoop

	goto	USER_LCD_CursorSet
