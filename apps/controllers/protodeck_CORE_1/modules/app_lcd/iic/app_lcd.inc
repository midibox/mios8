; $Id: app_lcd.inc 69 2008-02-01 00:20:18Z tk $
;
; MIOS Custom LCD Driver Example
; for Matrix Orbital IIC display
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

;; IIC address of display
#ifndef APP_LCD_IIC_DEVICE_ADDRESS
#define APP_LCD_IIC_DEVICE_ADDRESS 0x5a
#endif

;; contrast value, 0xff == max contrast
#ifndef APP_LCD_CONTRAST_VALUE
#define APP_LCD_CONTRAST_VALUE 0x90
#endif

;; backlight value, 0 == backlight will remain on indefinitely
#ifndef APP_LCD_BACKLIGHT_VALUE
#define APP_LCD_BACKLIGHT_VALUE 0x00
#endif

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the custom LCD should be initialized
;;  In:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_Init
	IRQ_DISABLE				; interrupts have to be disabled during IIC transfer

	;; set contrast
	call	MIOS_IIC_Start			; start IIC
	movlw	APP_LCD_IIC_DEVICE_ADDRESS	; send device address
	call	MIOS_IIC_ByteSend
	movlw	254				; control code sequence for contrast: 254 80
	call	MIOS_IIC_ByteSend
	movlw	80
	call	MIOS_IIC_ByteSend
	movlw	APP_LCD_CONTRAST_VALUE		; send contrast value
	call	MIOS_IIC_ByteSend
	call	MIOS_IIC_Stop			; stop IIC
	
#if APP_LCD_BACKLIGHT_VALUE
	;; set backlight
	call	MIOS_IIC_Start			; start IIC
	movlw	APP_LCD_IIC_DEVICE_ADDRESS	; send device address
	call	MIOS_IIC_ByteSend
	movlw	254				; control code sequence for backlight: 254 66
	call	MIOS_IIC_ByteSend
	movlw	70				; 66 [timeout] to set backlight, 70 to turn off
	call	MIOS_IIC_ByteSend
	movlw	APP_LCD_BACKLIGHT_VALUE		; send backlight value
	call	MIOS_IIC_ByteSend
	call	MIOS_IIC_Stop			; stop IIC
#endif

	IRQ_ENABLE				; enable interrupts

	;; clear display
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
	;; same as the "send char" function
	rgoto	USER_LCD_PrintChar

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_Cmd
;;  DESCRIPTION: sends a command to the LCD display.<BR>
;;  On CLCDs: use this function to decode the HD44780 commands if required<BR>
;;  On GLCDs: ignore this function!
;;  IN:  command which should be sent
;;  OUT: -
;; --------------------------------------------------------------------------
USER_LCD_Cmd
	;; not supported by Matrix Orbital protocol
	;; MEMO: a special handler could be added here which provides the
	;; initialization of special characters
	return

;; --------------------------------------------------------------------------
;;  This function is NOT called by MIOS, but only used by the custom driver
;;  to read from data register
;;  In:  -
;;  Out: WREG - data
;; --------------------------------------------------------------------------
USER_LCD_DataRead
	;; not supported by Matrix Orbital protocol
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the custom LCD should be cleared
;;  In:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_Clear
	IRQ_DISABLE				; interrupts have to be disabled during IIC transfer
	call	MIOS_IIC_Start			; start IIC
	movlw	APP_LCD_IIC_DEVICE_ADDRESS	; send device address
	call	MIOS_IIC_ByteSend
	movlw	254				; control code sequence for clear command: 254 88
	call	MIOS_IIC_ByteSend
	movlw	88
	call	MIOS_IIC_ByteSend
	call	MIOS_IIC_Stop			; stop IIC
	IRQ_ENABLE				; enable interrupts
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
	IRQ_DISABLE				; interrupts have to be disabled during IIC transfer
	call	MIOS_IIC_Start			; start IIC
	movlw	APP_LCD_IIC_DEVICE_ADDRESS	; send device address
	call	MIOS_IIC_ByteSend
	movlw	254				; control code sequence for cursor set: 254 71
	call	MIOS_IIC_ByteSend
	movlw	71
	call	MIOS_IIC_ByteSend
	SET_BSR	MIOS_LCD_CURSOR_POS		; get and send column
	movf	MIOS_LCD_CURSOR_POS, W, BANKED
	andlw	0x3f				; cursor pos & 0x3f
	addlw	1                               ; positions start at 1, not 0...
	call	MIOS_IIC_ByteSend
	SET_BSR	MIOS_LCD_CURSOR_POS		; get and send row (line)
	swapf	MIOS_LCD_CURSOR_POS, W, BANKED
	rrf	WREG, F
	rrf	WREG, W
	andlw	0x03				; (cursor pos >> 6) & 0x03
	addlw	1                               ; positions start at 1, not 0...
	call	MIOS_IIC_ByteSend
	call	MIOS_IIC_Stop			; stop IIC
	IRQ_ENABLE				; enable interrupts
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a character should be print
;;  In:  WREG - character
;;       all other MIOS_*LCD_* registers
;;  Out: GLCDs should justify the X/Y cursor position
;; --------------------------------------------------------------------------
USER_LCD_PrintChar 
	;; temporary save character in MIOS_GLCD_TMP3 
	SET_BSR MIOS_GLCD_TMP3 
	movwf MIOS_GLCD_TMP3, BANKED 
 
	IRQ_DISABLE				; interrupts have to be disabled during IIC transfer 
	call MIOS_IIC_Start			; start IIC 
	movlw APP_LCD_IIC_DEVICE_ADDRESS	; send device address 
	call MIOS_IIC_ByteSend 
	SET_BSR MIOS_GLCD_TMP3 
	movf MIOS_GLCD_TMP3, W, BANKED		; send character 
	call MIOS_IIC_ByteSend 
	call MIOS_IIC_Stop			; stop IIC 
	IRQ_ENABLE				; enable interrupts 
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
	;; not used by this driver
	return
