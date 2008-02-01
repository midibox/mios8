; $Id: $
;
; MIOS Custom LCD Driver Example
; for T6963C --- with vertical screen (-> 64x240)
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
LAT_GLCD_D	  EQU	  LATB		; Pin B.7-0
PORT_GLCD_D       EQU     PORTB
TRIS_GLCD_D       EQU     TRISB

LAT_GLCD_RD_N     EQU     LATD
GLCD_RD_N         EQU     7             ; Pin D.7
LAT_GLCD_WR_N     EQU     LATD
GLCD_WR_N         EQU     6             ; Pin D.6
LAT_GLCD_CD       EQU     LATD
GLCD_CD           EQU     5             ; Pin D.5

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the custom LCD should be initialized
;;  In:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_Init
	; (Initialization of Ports: done in Init_Ports)
	SET_BSR	MIOS_LCD_TIMEOUT1
	clrf	MIOS_LCD_TIMEOUT1, BANKED	; see MIOS_LCD_CmdLoop for the purpose

	movlw	50			; 50 ms delay
	call	MIOS_Delay

	;; set graphic home address to 0x0000
	movlw	0x00
	rcall	USER_LCD_Data
	movlw	0x00
	rcall	USER_LCD_Data
	movlw	0x42
	rcall	USER_LCD_Cmd

	;; set it again, Sam
	movlw	0x00
	rcall	USER_LCD_Data
	movlw	0x00
	rcall	USER_LCD_Data
	movlw	0x42
	rcall	USER_LCD_Cmd

	;; set graphic area to 0x20 bytes per line
	movlw	0x20
	rcall	USER_LCD_Data
	movlw	0x00
	rcall	USER_LCD_Data
	movlw	0x43
	rcall	USER_LCD_Cmd

	;; set mode (AND mode, CG ROM)
	movlw	0x82
	rcall	USER_LCD_Cmd

	;; set display mode (graphic, no text)
	movlw	0x98
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
	movwf	LAT_GLCD_D			; store data on data bus

	rcall	USER_LCD_WaitUnbusy		; poll busy flag

        bcf     LAT_GLCD_CD, GLCD_CD		; select data register
        bcf     LAT_GLCD_WR_N, GLCD_WR_N	; strobe write
	nop					; wait cycle
        bsf     LAT_GLCD_WR_N, GLCD_WR_N	; release write
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
	movwf	LAT_GLCD_D			; store data on data bus

	rcall	USER_LCD_WaitUnbusy		; poll busy flag

        bsf     LAT_GLCD_CD, GLCD_CD		; select instruction register
        bcf     LAT_GLCD_WR_N, GLCD_WR_N	; strobe write
	nop					; wait cycle
        bsf     LAT_GLCD_WR_N, GLCD_WR_N	; release write
	return

;; --------------------------------------------------------------------------
;;  This function is NOT called by MIOS, but only used by the custom driver
;;  to read from data register
;;  In:  -
;;  Out: WREG - data
;; --------------------------------------------------------------------------
USER_LCD_DataRead
	rcall	USER_LCD_WaitUnbusy		; wait until LCD unbusy

	setf	TRIS_GLCD_D			; DATA_IN
        bcf     LAT_GLCD_CD, GLCD_CD		; select data register
	nop					; wait cycle
        bcf     LAT_GLCD_RD_N, GLCD_RD_N	; strobe read
	nop					; wait cycle
	movf	PORT_GLCD_D, W			; fetch data from bus
        bsf     LAT_GLCD_RD_N, GLCD_RD_N	; release read
        clrf    TRIS_GLCD_D			; DATA_OUT
	return

;; --------------------------------------------------------------------------
;;  This function is NOT called by MIOS, but only used by the custom driver
;;  to wait until the LCD is not busy
;;  In:  -
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_WaitUnbusy
	setf	TRIS_GLCD_D	; DATA_IN	; wait until busy bit is cleared

	SET_BSR	MIOS_LCD_TIMEOUT0
	btfsc	MIOS_LCD_TIMEOUT1, 7, BANKED	; if timeout bit (#7), no display connected
	return
	
	clrf	MIOS_LCD_TIMEOUT0, BANKED	; clear time out counter
	clrf	MIOS_LCD_TIMEOUT1, BANKED

        bsf     LAT_GLCD_CD, GLCD_CD
USER_LCD_WaitUnbusyLoop
        bsf     LAT_GLCD_RD_N, GLCD_RD_N
	nop
	incf	MIOS_LCD_TIMEOUT0, F, BANKED
	skpnz
	incf	MIOS_LCD_TIMEOUT1, F, BANKED
	bz	USER_LCD_WaitUnbusy_Disable	; leave loop when LCD_TIMEOUT = 0xff. Up to now bit 7 is set and the LCD
		
					; busy routine will never be called again
        bcf     LAT_GLCD_RD_N, GLCD_RD_N
	nop
	BRA_IFCLR PORT_GLCD_D, 0, ACCESS, USER_LCD_WaitUnbusyLoop
	BRA_IFCLR PORT_GLCD_D, 1, ACCESS, USER_LCD_WaitUnbusyLoop

        bsf     LAT_GLCD_RD_N, GLCD_RD_N

	bcf	MIOS_LCD_TIMEOUT1, 7, BANKED	; everything ok, make sure that LCD_TIMEOUT, bit 7 is cleared

USER_LCD_WaitUnbusy_End
        clrf    TRIS_GLCD_D	; DATA_OUT
	return

USER_LCD_WaitUnbusy_Disable
	bsf	MIOS_LCD_TIMEOUT1, 7, BANKED	; LCD time out: set bit 7, busy routine will never be called again
	rgoto	USER_LCD_WaitUnbusy_End


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the custom LCD should be cleared
;;  In:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_Clear
	movlw	0x00			; Set Y=0, X=0
	rcall	USER_LCD_Data
	movlw	0x00
	rcall	USER_LCD_Data
	movlw	0x24
	rcall	USER_LCD_Cmd

	;; clear the first 32*64 bytes
	;; use MIOS_GLCD_GCURSOR_X and _Y as counter register
	SET_BSR MIOS_GLCD_GCURSOR_Y	; 64 lines to clear
	clrf	MIOS_GLCD_GCURSOR_Y, BANKED
USER_LCD_ClearOuterLoop
	SET_BSR MIOS_GLCD_GCURSOR_X	; 32 bytes to clear
	clrf	MIOS_GLCD_GCURSOR_X, BANKED

USER_LCD_ClearInnerLoop
	movlw	0x00
	rcall	USER_LCD_Data
	movlw	0xc0		; write and increment
	rcall	USER_LCD_Cmd
	incf	MIOS_GLCD_GCURSOR_X, F, BANKED
	BRA_IFCLR MIOS_GLCD_GCURSOR_X, 5, BANKED, USER_LCD_ClearInnerLoop

	clrf	MIOS_GLCD_GCURSOR_X, BANKED

	incf	MIOS_GLCD_GCURSOR_Y, F, BANKED
	BRA_IFCLR MIOS_GLCD_GCURSOR_Y, 6, BANKED, USER_LCD_ClearOuterLoop

	movlw	0x00			; Set Y=0, X=0
	rcall	USER_LCD_Data
	movlw	0x00
	rcall	USER_LCD_Data
	movlw	0x24
	rcall	USER_LCD_Cmd

	clrf	MIOS_PARAMETER1
	movlw	0x00			; the same for the registered X/Y position
	goto	MIOS_LCD_CursorSet	; *** use MIOS_LCD_CursorSet here to update the cursor registers! ***

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
	;; not used here, USER_LCD_PrintChar modifies the position directly
	return

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
	;; calc and set address to X/Y position (x*0x20 + (29-y))
	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	mullw	0x20
	movf	MIOS_GLCD_GCURSOR_Y, W, BANKED
	sublw	29		; for 240x64 display --- for 120x64 change this offset to 14
	addwf	PRODL, W	; set low-byte of address
	rcall	USER_LCD_Data
	movf	PRODH, W	; set high-byte of address
	rcall	USER_LCD_Data
	movlw	0x24
	rcall	USER_LCD_Cmd

	;; write data
	tblrd*+				; read from flash and increment table pointer
	movf	TABLAT, W		; get result
	rcall	USER_LCD_Data		; write out
	movlw	0xc0
	rcall	USER_LCD_Cmd

	;; increment X cursor
	incf	MIOS_GLCD_GCURSOR_X, F, BANKED

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
	rgoto	USER_LCD_PrintCharOuterLoop

USER_LCD_PrintCharLoop_End
	decf	MIOS_GLCD_FONT_HEIGHT, W, BANKED
	bz	USER_LCD_PrintChar_NoYFix
	comf	MIOS_GLCD_FONT_HEIGHT, W, BANKED
	addlw	2
	addwf	MIOS_GLCD_GCURSOR_Y, F, BANKED
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
	;; not used by this driver
	return
