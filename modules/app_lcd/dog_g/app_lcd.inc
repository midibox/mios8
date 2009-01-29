; $Id: app_lcd.inc 69 2008-02-01 00:20:18Z tk $
;
; MIOS Custom LCD Driver Example
; for up to 8 * DOG GLCD (every display provides a resolution of 128x64)
;;
; This driver allows to drive up to 8 of them, every display is connected
; to a dedicated chip select line at port B. They can be addressed with
; following (graphical) cursor positions:
; 
; I have swapped these round to make it easier for people with PIC18f4685
; so they are limited to 4x displays. Therefore the first display is on PortB.7
; 
; CS at PortB.7     CS at PortB.6     
; +--------------+  +--------------+  
; |              |  |              |  
; | X =   0..127 |  | X = 128..255 |    
; | Y =   0..  7 |  | Y =   0..  7 | 
; |              |  |              |  
; +--------------+  +--------------+  
;
; CS at PortB.5     CS at PortB.4     
; +--------------+  +--------------+ 
; |              |  |              | 
; | X =   0..127 |  | X = 128..255 | 
; | Y =   8.. 15 |  | Y =   8.. 15 | 
; |              |  |              | 
; +--------------+  +--------------+ 
;
; CS at PortB.3     CS at PortB.2   
; +--------------+  +--------------+
; |              |  |              |
; | X =   0..127 |  | X = 128..255 |
; | Y =  16.. 23 |  | Y =  16.. 23 |
; |              |  |              |
; +--------------+  +--------------+
;
; CS at PortB.1     CS at PortB.0   
; +--------------+  +--------------+
; |              |  |              |
; | X =   0..127 |  | X = 128..255 |
; | Y =  24.. 31 |  | Y =  24.. 31 |
; |              |  |              |
; +--------------+  +--------------+
;
; The arrangement can be modified below the USER_LCD_Data_CS and USER_LCD_GCursorSet label
;
; ==========================================================================
;
;  Copyright (C) 2009  Phil Taylor (phil@taylor.org.uk)
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

;; Clock/data/DC
USER_LCD_LAT_SCLK	EQU	LATD	; Pin D.7
USER_LCD_PIN_SCLK	EQU	7
USER_LCD_LAT_SDA	EQU	LATD	; Pin D.6
USER_LCD_PIN_SDA	EQU	6
USER_LCD_LAT_DC		EQU	LATD	; Pin D.5
USER_LCD_PIN_DC		EQU	5

;; Chip Select Lines 0-7
USER_LCD_LAT_CS		EQU	LATB	; Pin B.7 .. B.0
USER_LCD_PIN_D0		EQU 7
USER_LCD_PIN_D1		EQU 6 

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the custom LCD should be initialized
;;  In:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_Init
	;; notify that a graphical LCD has been connected
	bsf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD

	;movlw	100			; 100 ms delay
	;call	MIOS_Delay
	;; initialization sequence based on the example from the DOG datasheet
	movlw	0x40		; 2 - Display start line = 0
	rcall	USER_LCD_Cmd
	movlw	0xA1		; 8 - ADC Normal mode = 0 
	rcall	USER_LCD_Cmd
	movlw	0xC0		; 15 - COMS normal = 1/65  duty
	rcall	USER_LCD_Cmd
	movlw	0xA6		;  9 - Display  = normal  
	rcall	USER_LCD_Cmd
	movlw	0xA2		; 11 - 1/65 duty 1/9 bias for 65x132 display
	rcall	USER_LCD_Cmd
	movlw	0x2F		; 16  - Power control set = B.,R,F all ON
	rcall	USER_LCD_Cmd
	movlw	0xF8		; 20-1 - select Booster ratio set
	rcall	USER_LCD_Cmd
	movlw	0x00		; 20-2 - Booster ratio register  
	rcall	USER_LCD_Cmd ;	(must be preceeded by 20-1)
	movlw	0x27		; 17 - VO volt reg set 
	rcall	USER_LCD_Cmd
	movlw	0x81		; 18-1 - Elect vol control - contrast
	rcall	USER_LCD_Cmd 
	movlw	0x10		; 18-2 - Contrast level dec 22	
	rcall	USER_LCD_Cmd;	(must be preceeded by 18-1)
	movlw	0xAC		; 19-1 - Static Indicator - set off
	rcall	USER_LCD_Cmd
	movlw	0x00		; 19-2 - No Indicator
	rcall	USER_LCD_Cmd;	(must be preceeded with 19-1)
	movlw	0xAF		; 20 - Display ON
	rcall	USER_LCD_Cmd


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
USER_LCD_Data_New

	;; data byte
	bsf	USER_LCD_LAT_DC, USER_LCD_PIN_DC

	;; deselect all CS
	setf	USER_LCD_LAT_CS

	;; SELECT ALL CS **FIXME** 
	bcf	USER_LCD_LAT_CS, USER_LCD_PIN_D0

	rcall	USER_LCD_ShiftByte
	SET_BSR	MIOS_GLCD_GCURSOR_X	; (BSR already set by USER_LCD_ShiftByte)
	incf	MIOS_GLCD_GCURSOR_X, F, BANKED
	;; deselect displays
	setf	USER_LCD_LAT_CS

	return
	
USER_LCD_Data
	;; store byte in buffer
	SET_BSR	MIOS_GLCD_BUFFER
	movwf	MIOS_GLCD_BUFFER+0, BANKED
	;; data byte
	bsf	USER_LCD_LAT_DC, USER_LCD_PIN_DC

	;; deselect all CS
	setf	USER_LCD_LAT_CS

	;; select display depending on current cursor position
	;; THIS PART COULD BE CHANGED FOR DIFFERENT DISPLAY CONFIGURATIONS
	;; 
USER_LCD_Data_CS
	movlw	1*8-1
	cpfsgt	MIOS_GLCD_GCURSOR_Y, BANKED
	rgoto	USER_LCD_Data_CS_BANKA
	movlw	2*8-1
	cpfsgt	MIOS_GLCD_GCURSOR_Y, BANKED
	rgoto	USER_LCD_Data_CS_BANKB
	movlw	3*8-1
	cpfsgt	MIOS_GLCD_GCURSOR_Y, BANKED
	rgoto	USER_LCD_Data_CS_BANKC
	;rgoto	USER_LCD_Data_CS_BANKD
	;; I am not currently checking whether the cursor Y position exceeds 31
	;; User MUST make sure it doesn't!
USER_LCD_Data_CS_BANKD
	movlw	1*128-1
	cpfsgt	MIOS_GLCD_GCURSOR_X, BANKED
	rgoto	USER_LCD_Data_CS_BANKD_LOW
USER_LCD_Data_CS_BANKD_HIGH 
	bcf	USER_LCD_LAT_CS, 6
	rgoto	USER_LCD_Data_CS_Done
USER_LCD_Data_CS_BANKD_LOW
	bcf	USER_LCD_LAT_CS, 7
	rgoto	USER_LCD_Data_CS_Done

USER_LCD_Data_CS_BANKC
	movlw	1*128-1
	cpfsgt	MIOS_GLCD_GCURSOR_X, BANKED
	rgoto	USER_LCD_Data_CS_BANKC_LOW
USER_LCD_Data_CS_BANKC_HIGH
	bcf	USER_LCD_LAT_CS, 4
	rgoto	USER_LCD_Data_CS_Done
USER_LCD_Data_CS_BANKC_LOW
	bcf	USER_LCD_LAT_CS, 5
	rgoto	USER_LCD_Data_CS_Done

USER_LCD_Data_CS_BANKB
	movlw	1*128-1
	cpfsgt	MIOS_GLCD_GCURSOR_X, BANKED
	rgoto	USER_LCD_Data_CS_BANKB_LOW
USER_LCD_Data_CS_BANKB_HIGH
	bcf	USER_LCD_LAT_CS, 4
	rgoto	USER_LCD_Data_CS_Done
USER_LCD_Data_CS_BANKB_LOW
	bcf	USER_LCD_LAT_CS, 5
	rgoto	USER_LCD_Data_CS_Done
	
USER_LCD_Data_CS_BANKA
	movlw	1*128-1
	cpfsgt	MIOS_GLCD_GCURSOR_X, BANKED
	rgoto	USER_LCD_Data_CS_BANKA_LOW
USER_LCD_Data_CS_BANKA_HIGH
	bcf	USER_LCD_LAT_CS, 6
	rgoto	USER_LCD_Data_CS_Done
USER_LCD_Data_CS_BANKA_LOW
	bcf	USER_LCD_LAT_CS, 7
	;rgoto	USER_LCD_Data_CS_Done 

USER_LCD_Data_CS_Done
	;; transfer data to display
	movf	MIOS_GLCD_BUFFER+0, W, BANKED
	rcall	USER_LCD_ShiftByte
	;; increment graphical cursor
	SET_BSR	MIOS_GLCD_GCURSOR_X;
	incf	MIOS_GLCD_GCURSOR_X, F, BANKED
	;; deselect displays
	setf	USER_LCD_LAT_CS

	;; if new X position reached, set LCD cursor of all displays to 0
USER_LCD_Data_RstTst2
	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	xorlw	1*128-1
	bz	USER_LCD_Data_Rst
	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	xorlw	2*128-1
	bz	USER_LCD_Data_Rst
	return

USER_LCD_Data_Rst
	movlw	0x80
	;;rgoto	USER_LCD_Cmd

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
	;; select all LCDs at once
	clrf	USER_LCD_LAT_CS
	;; transfer command to display
	rcall	USER_LCD_ShiftByte
	;; deselect displays and exit
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
	;btfss	WREG, bit
	bcf	USER_LCD_LAT_SDA, USER_LCD_PIN_SDA		; set SDA depending on current MSB
	btfsc	WREG, bit
	bsf	USER_LCD_LAT_SDA, USER_LCD_PIN_SDA
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

	
;; --------------------------------------------------------------------------
;;  Determine Chip Select Line
;; --------------------------------------------------------------------------
USER_LCD_DetermineCS
	return


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

	SET_BSR MIOS_GLCD_GCURSOR_X	; 128 columns to clear
	clrf	MIOS_GLCD_GCURSOR_X, BANKED
	rcall	USER_LCD_CursorSet

	;; data bytes
	bsf	USER_LCD_LAT_DC, USER_LCD_PIN_DC
	;; select all displays at once
	clrf	USER_LCD_LAT_CS
USER_LCD_ClearInnerLoop
	;; transfer 0x00 to display
	movlw	0x00
	rcall	USER_LCD_ShiftByte
	;; increment graphical cursor
	SET_BSR	MIOS_GLCD_GCURSOR_X	; 
	incf	MIOS_GLCD_GCURSOR_X, F, BANKED
	movlw	128-1
	cpfsgt	MIOS_GLCD_GCURSOR_X, BANKED
	rgoto	USER_LCD_ClearInnerLoop

	incf	MIOS_GLCD_GCURSOR_Y, F, BANKED
	movlw	8-1
	cpfsgt	MIOS_GLCD_GCURSOR_Y, BANKED
	rgoto	USER_LCD_ClearOuterLoop

	;; set cursor to 0 and exit
	movlw	0x00
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
	 	rgoto	USER_LCD_GCursorSet


;; --------------------------------------------------------------------------
;;  This function is NOT called by MIOS, but only used by the custom driver
;;  to set the cursor positions
;;  In:  graphical cursor position in MIOS_GLCD_GCURSOR_X and MIOS_GLCD_GCURSOR_Y
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_GCursorSet
	
	;; Set X position, wrap at 128 (LCD X cursor must be less than 128!)
	SET_BSR	MIOS_GLCD_GCURSOR_X
	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	movwf	MIOS_GLCD_BUFFER+0, BANKED
	movlw	128-1
	cpfsgt	MIOS_GLCD_BUFFER+0, BANKED
	rgoto	USER_LCD_GCursorSet_X_Cont
	movlw	-128
	addwf	MIOS_GLCD_BUFFER+0, F, BANKED
	movlw	128-1
	cpfsgt	MIOS_GLCD_BUFFER+0, BANKED
	rgoto	USER_LCD_GCursorSet_X_Cont
	movlw	-128
	addwf	MIOS_GLCD_BUFFER+0, F, BANKED

USER_LCD_GCursorSet_X_Cont
	; First remove 4 LSB and send
	; Then remove 4 MSB and send
	swapf	MIOS_GLCD_BUFFER+0, W, BANKED ; Swap bottom 4 bits with top 4
	andlw	0x0f ; Mask out top 4 bits (actually bottom 4!)
	iorlw	0x10   ; Column address set upper bit
	rcall	USER_LCD_Cmd
	movf	MIOS_GLCD_BUFFER+0, W, BANKED 
	andlw	0x0f ; Mask out top 4 bits
	rcall	USER_LCD_Cmd
	
	
	;; Set Y position, wrap at 8 (LCD Y cursor must be less than 8!)
	movf	MIOS_GLCD_GCURSOR_Y, W, BANKED
	movwf	MIOS_GLCD_BUFFER+0, BANKED
	movlw	8-1
	cpfsgt	MIOS_GLCD_BUFFER+0, BANKED
	rgoto	USER_LCD_GCursorSet_Y_Cont
	movlw	-8
	addwf	MIOS_GLCD_BUFFER+0, F, BANKED
	movlw	8-1
	cpfsgt	MIOS_GLCD_BUFFER+0, BANKED
	rgoto	USER_LCD_GCursorSet_Y_Cont
	movlw	-8
	addwf	MIOS_GLCD_BUFFER+0, F, BANKED
	movlw	8-1
	cpfsgt	MIOS_GLCD_BUFFER+0, BANKED
	rgoto	USER_LCD_GCursorSet_Y_Cont
	movlw	-8
	addwf	MIOS_GLCD_BUFFER+0, F, BANKED
USER_LCD_GCursorSet_Y_Cont
	movf	MIOS_GLCD_BUFFER+0, W, BANKED
	iorlw	0xb0 ; Display Column H=W+0xb0 (select bank+line)
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
	;; not used by this driver
	return
