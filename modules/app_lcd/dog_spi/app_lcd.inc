; $Id: app_lcd.inc 69 2008-02-01 00:20:18Z tk $
;
; MIOS Driver for DOG CLCD's using SPI interface
; Works much quicker than parallel 4/8 bit interface
; an allow up to 8 displays using CS lines.
; 
; USER_LCD_Select function is used like with clcd_multi
; parameter 0-7 selects which display to work with.
; By default all displays will get the same data unless this
; function is called by your application.
;
; ==========================================================================
;
;  Copyright (C) 2003  Thorsten Klose (tk@midibox.org)
;  Copyright (C) 2009  Phil Taylor (phil@taylor.org.uk)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

; 
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

;; Clock/data/DC
USER_LCD_LAT_SCLK	EQU	LATD	; Pin D.7 (E on J15 -> PIN29 CLK)
USER_LCD_PIN_SCLK	EQU	7
USER_LCD_LAT_SDA	EQU	LATD	; Pin D.6 (RW on J15 -> PIN28 SI)
USER_LCD_PIN_SDA	EQU	6
USER_LCD_LAT_DC		EQU	LATD	; Pin D.5 (RS on J15 -> PIN39 RS)
USER_LCD_PIN_DC		EQU	5

;; Chip Select Lines 0-7
USER_LCD_LAT_CS		EQU	LATB	; Pin B0-B7 (D0-D7 on J15 -> |CSB0-7)
USER_LCD_PIN_D0		EQU 7
USER_LCD_PIN_D1		EQU 6 

;; new names for CLCD registers
USER_LCD_STATUS		EQU	MIOS_GLCD_TMP1
USER_LCD_SELECT		EQU	MIOS_GLCD_TMP2
USER_LCD_SC_CTR		EQU	MIOS_GLCD_TMP3


#define USER_LCD_STATUS_LCD0_DISABLED	0	; bit0:	if set, first LCD disabled
#define USER_LCD_STATUS_LCD1_DISABLED	1	; bit1:	if set, second LCD disabled -- not provided by this driver!
#define USER_LCD_STATUS_CUR_DISABLED	2	; bit2:	if set, currently selected LCD disabled
#define USER_LCD_STATUS_CUR_LCD		3	; bit3: if cleared: current LCD is first LCD, else second LCD

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the custom LCD should be initialized
;;  In:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_Init
	;; notify that no graphical LCD is connected
	bcf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD

	; (Initialization of Ports: done in Init_Ports)
	SET_BSR	USER_LCD_STATUS
	clrf	USER_LCD_STATUS, BANKED
	clrf	USER_LCD_SELECT, BANKED

	movlw	100			; 100 ms delay
	call	MIOS_Delay


	;; perform additional display initialisation
	movlw	0x38			; 8bit interface, switch to instruction table 0
	call	MIOS_LCD_Cmd
	movlw	0x39			; 8bit interface, switch to instruction table 1
	call	MIOS_LCD_Cmd
	movlw	0x1d			; BS: 1/4, 3 line LCD
	call	MIOS_LCD_Cmd
	movlw	0x50			; Booster off, set contrast C5/C4
	call	MIOS_LCD_Cmd
	movlw	0x6c			; set Voltage follower and amplifier
	call	MIOS_LCD_Cmd
	movlw	0x7c			; set contrast C3/C2/C1
	call	MIOS_LCD_Cmd
	movlw	0x38			; back to instruction table 0
	call	MIOS_LCD_Cmd
	movlw	0x0c			; display on/cursor on (cursor on for testing.)
	call	MIOS_LCD_Cmd
	movlw	0x01			; Clear Display
	call	MIOS_LCD_Cmd
	movlw	0x06			; Entry Mode
	call	MIOS_LCD_Cmd

	SET_BSR	MIOS_LCD_TIMEOUT1			; We have no feedback from SPI display so no timeout
	bcf	MIOS_LCD_TIMEOUT1, 7, BANKED	; everything ok, make sure that LCD_TIMEOUT, bit 7 is cleared


	;; adapt Y0/Y1/Y2/Y3 offsets
	movlw	0x00		; Y Line 1 (MIOS cursor offset 0x00)
	movwf	MIOS_PARAMETER1
	movlw	0x10		; Y Line 2 (MIOS cursor offset 0x40)
	movwf	MIOS_PARAMETER2
	movlw	0x20		; Y Line 3 (MIOS cursor offset 0x80 - select second LCD)
	movwf	MIOS_PARAMETER3
	movlw	0x90		; Y Line 4 (MIOS cursor offset 0xc0 - select second LCD)
	goto	MIOS_LCD_YAddressSet


;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_Data
;;  DESCRIPTION: sends a data value to the LCD display.<BR>
;;  On CLCDs: branch directly to USER_LCD_PrintChar<BR>
;;  On GLCDs: ignore this function!
;;  IN:  data which should be sent
;;  OUT: -
;; --------------------------------------------------------------------------
USER_LCD_Data
	;; data byte
	bsf	USER_LCD_LAT_DC, USER_LCD_PIN_DC
	;SET_BSR	USER_LCD_SELECT
	movff	USER_LCD_SELECT, USER_LCD_LAT_CS
	rcall	USER_LCD_ShiftByte
	setf	USER_LCD_LAT_CS
	return
	

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_Cmd
;;  DESCRIPTION: sends a command to the LCD display.<BR>
;;  On CLCDs: use this function to decode the HD44780 commands if required<BR>
;;  On GLCDs: ignore this function!
;;  IN:  command which should be sent
;;  OUT: -
;; --------------------------------------------------------------------------
USER_LCD_Cmd
	;; command byte
	bcf	USER_LCD_LAT_DC, USER_LCD_PIN_DC
	;SET_BSR	USER_LCD_SELECT
	movff	USER_LCD_SELECT, USER_LCD_LAT_CS
	rcall	USER_LCD_ShiftByte
	setf	USER_LCD_LAT_CS
	
	return
	
	
;; --------------------------------------------------------------------------
;;  This function is NOT called by MIOS, but only used by the custom driver
;;  to transfer a data/command byte
;;  In:  WREG - data/command byte
;;       chip select lines must be set before calling this function!
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_ShiftByte

USER_LCD_ShiftByte_HLP MACRO bit
	bsf	USER_LCD_LAT_SCLK, USER_LCD_PIN_SCLK	; rising clock edge
	nop
	bcf	USER_LCD_LAT_SDA, USER_LCD_PIN_SDA		; set SDA depending on current MSB
	btfsc	WREG, bit
	bsf	USER_LCD_LAT_SDA, USER_LCD_PIN_SDA
	rcall USER_LCD_1uS_Delay
    bcf	USER_LCD_LAT_SCLK, USER_LCD_PIN_SCLK	; falling clock edge
	ENDM

	USER_LCD_ShiftByte_HLP 7
	USER_LCD_ShiftByte_HLP 6
	USER_LCD_ShiftByte_HLP 5
	USER_LCD_ShiftByte_HLP 4
	USER_LCD_ShiftByte_HLP 3
	USER_LCD_ShiftByte_HLP 2
	USER_LCD_ShiftByte_HLP 1
	USER_LCD_ShiftByte_HLP 0
	nop
	bsf	USER_LCD_LAT_SCLK, USER_LCD_PIN_SCLK	; rising clock edge

	return

	
	
USER_LCD_1uS_Delay
	bra	$+2 ; Remove as many of these until display stops working and then add one back in !!!!!
	bra	$+2
	bra	$+2
	;bra	$+2
	;bra	$+2

	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the custom LCD should be cleared
;;  In:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_Clear
	movlw	0x01
	call	USER_LCD_Cmd
	movlw	1			; 1 ms delay as clear command takes about 1mS to complete.
	call	MIOS_Delay
	SET_BSR	MIOS_LCD_Y2_OFFSET
	BRA_IFSET MIOS_LCD_Y2_OFFSET, 7, BANKED, USER_LCD_Clear2
	BRA_IFSET MIOS_LCD_Y3_OFFSET, 7, BANKED, USER_LCD_Clear2
	return
USER_LCD_Clear2
	bsf	MIOS_LCD_CURSOR_POS, 7, BANKED
	movlw	0x01
	call	USER_LCD_Cmd
	movlw	1			; 1 ms delay as clear command takes about 1mS to complete.
	call	MIOS_Delay
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
;;  This function is called by MIOS when a character should be printed
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

	
;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_Select
;;  DESCRIPTION: Select current display 
;;  In:  number of display, 0-7
;;  Out: USER_LCD_LAT_CS set to inverted bit or mask
;; --------------------------------------------------------------------------
_USER_LCD_Select
USER_LCD_Select
	SET_BSR	USER_LCD_SELECT
	; for d0=lcd1, d1=lcd2 etc uncomment this section and comment swap section
	call MIOS_HLP_GetBitORMask
	xorlw 0xff ; Invert bits as a clear bit selects pin.
	movwf	USER_LCD_SELECT, BANKED 
	return
	