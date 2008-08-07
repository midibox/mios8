; $Id$
;
; MIOS Custom LCD Driver for DOG LCDs
;
; Note that this driver switches to CLCD (type #0) after initialisation!
; Up to 2 DOG based LCDs are supported
;
; ==========================================================================
;
;  Copyright (C) 2008  Thorsten Klose (tk@midibox.org)
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


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the custom LCD should be initialized
;;  In:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_Init
	;; notify that no graphical LCD is connected
	bcf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD

	;; switch to CLCDdriver 0 to save code!
	movff	MIOS_LCD_OPTION1, MIOS_PARAMETER1
	movff	MIOS_LCD_OPTION2, MIOS_PARAMETER2
	movlw	0x00
	call	MIOS_LCD_TypeSet

	;; call common initialisation routine
	call	MIOS_LCD_Init

	;; perform additional display initialisation
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
	movlw	0x0c			; display on
	call	MIOS_LCD_Cmd
	movlw	0x01			; Clear Display
	call	MIOS_LCD_Cmd
	movlw	0x06			; Entry Mode
	call	MIOS_LCD_Cmd

	SET_BSR	MIOS_LCD_TIMEOUT1
	bcf	MIOS_LCD_TIMEOUT1, 7, BANKED	; everything ok, make sure that LCD_TIMEOUT, bit 7 is cleared

	;; adapt Y0/Y1/Y2/Y3 offsets
	movlw	0x00		; Y Line 1 (MIOS cursor offset 0x00)
	movwf	MIOS_PARAMETER1
	movlw	0x10		; Y Line 2 (MIOS cursor offset 0x40)
	movwf	MIOS_PARAMETER2
	movlw	0x80		; Y Line 3 (MIOS cursor offset 0x80 - select second LCD)
	movwf	MIOS_PARAMETER3
	movlw	0x90		; Y Line 4 (MIOS cursor offset 0xc0 - select second LCD)
	goto	MIOS_LCD_YAddressSet


;; --------------------------------------------------------------------------
;;  Dummy Functions
;;  Will never be called since the init routine switches to display type #0
;; --------------------------------------------------------------------------
USER_LCD_Data
USER_LCD_Cmd
USER_LCD_Clear
USER_LCD_CursorSet
USER_LCD_PrintChar
USER_LCD_SpecialCharInit
	return
