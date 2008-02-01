; $Id: $
;
; MIOS Custom LCD Driver Example
; for up to 8 * PCD8544 (every display provides a resolution of 84x48)
;
; This display can mostly be found in Nokia mobile phones
;
; This driver allows to drive up to 8 of them, every display is connected
; to a dedicated chip select line at port B. They can be addressed with
; following (graphical) cursor positions:
; 
; 
; CS at PortB.0     CS at PortB.1     CS at PortB.2
; +--------------+  +--------------+  +--------------+
; |              |  |              |  |              |
; | X =   0.. 83 |  | X =  84..167 |  | X = 168..251 |  
; | Y =   0..  5 |  | Y =   0..  5 |  | Y =   0..  5 |
; |              |  |              |  |              |
; +--------------+  +--------------+  +--------------+  
;
; CS at PortB.3     CS at PortB.4     CS at PortB.5
; +--------------+  +--------------+  +--------------+
; |              |  |              |  |              |
; | X =   0.. 83 |  | X =  84..167 |  | X = 168..251 |  
; | Y =   6.. 11 |  | Y =   6.. 11 |  | Y =   6.. 11 |
; |              |  |              |  |              |
; +--------------+  +--------------+  +--------------+  
;
; CS at PortB.6     CS at PortB.7   
; +--------------+  +--------------+
; |              |  |              |
; | X =   0.. 83 |  | X =  84..167 |
; | Y =  12.. 17 |  | Y =  12.. 17 |
; |              |  |              |
; +--------------+  +--------------+
;
; The arrangement can be modified below the USER_LCD_Data_CS and USER_LCD_GCursorSet label
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

;; Clock/data/DC
USER_LCD_LAT_SCLK	EQU	LATD	; Pin D.7
USER_LCD_PIN_SCLK	EQU	7
USER_LCD_LAT_SDA	EQU	LATD	; Pin D.6
USER_LCD_PIN_SDA	EQU	6
USER_LCD_LAT_DC		EQU	LATD	; Pin D.5
USER_LCD_PIN_DC		EQU	5

;; Chip Select Lines 0-7
USER_LCD_LAT_CS		EQU	LATB	; Pin B.7 .. B.0


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

	;; initialization sequence based on the example from the PCD8544 datasheet
	movlw	0x21		; PD=0 and V=0, select extended instruction set (H=1 mode)
	rcall	USER_LCD_Cmd
	movlw	0x90		; Vop is set to a + 16 x b[V]
	rcall	USER_LCD_Cmd
	movlw	0x20		; PD=0 and V=0, select normal instruction set (H=1 mode)
	rcall	USER_LCD_Cmd
	movlw	0x0c		; enter normal mode (D=1 and E=0)
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
USER_LCD_Data
	;; store byte in buffer
	SET_BSR	MIOS_GLCD_BUFFER
	movwf	MIOS_GLCD_BUFFER+0, BANKED
	;; data byte
	bsf	USER_LCD_LAT_DC, USER_LCD_PIN_DC

	;; deselect all CS
	setf	USER_LCD_LAT_CS

	;; select display depending on current cursor position
	;; THIS PART COULD BE CHANGED TO ARRANGE THE 8 DISPLAY ON ANOTHER WAY
USER_LCD_Data_CS
	movlw	1*6-1
	cpfsgt	MIOS_GLCD_GCURSOR_Y, BANKED
	rgoto	USER_LCD_Data_CS012
	movlw	2*6-1
	cpfsgt	MIOS_GLCD_GCURSOR_Y, BANKED
	rgoto	USER_LCD_Data_CS345
	rgoto	USER_LCD_Data_CS67

USER_LCD_Data_CS012
	movlw	1*84-1
	cpfsgt	MIOS_GLCD_GCURSOR_X, BANKED
	rgoto	USER_LCD_Data_CS0
	movlw	2*84-1
	cpfsgt	MIOS_GLCD_GCURSOR_X, BANKED
	rgoto	USER_LCD_Data_CS1
USER_LCD_Data_CS2
	bcf	USER_LCD_LAT_CS, 2
	rgoto	USER_LCD_Data_CS_Done
USER_LCD_Data_CS1
	bcf	USER_LCD_LAT_CS, 1
	rgoto	USER_LCD_Data_CS_Done
USER_LCD_Data_CS0
	bcf	USER_LCD_LAT_CS, 0
	rgoto	USER_LCD_Data_CS_Done

USER_LCD_Data_CS345
	movlw	1*84-1
	cpfsgt	MIOS_GLCD_GCURSOR_X, BANKED
	rgoto	USER_LCD_Data_CS3
	movlw	2*84-1
	cpfsgt	MIOS_GLCD_GCURSOR_X, BANKED
	rgoto	USER_LCD_Data_CS4
USER_LCD_Data_CS5
	bcf	USER_LCD_LAT_CS, 5
	rgoto	USER_LCD_Data_CS_Done
USER_LCD_Data_CS4
	bcf	USER_LCD_LAT_CS, 4
	rgoto	USER_LCD_Data_CS_Done
USER_LCD_Data_CS3
	bcf	USER_LCD_LAT_CS, 3
	rgoto	USER_LCD_Data_CS_Done

USER_LCD_Data_CS67
	movlw	1*84-1
	cpfsgt	MIOS_GLCD_GCURSOR_X, BANKED
	rgoto	USER_LCD_Data_CS6
USER_LCD_Data_CS7
	bcf	USER_LCD_LAT_CS, 7
	rgoto	USER_LCD_Data_CS_Done
USER_LCD_Data_CS6
	bcf	USER_LCD_LAT_CS, 6
	;; 	rgoto	USER_LCD_Data_CS_Done

USER_LCD_Data_CS_Done
	;; transfer data to display
	movf	MIOS_GLCD_BUFFER+0, W, BANKED
	rcall	USER_LCD_ShiftByte
	;; increment graphical cursor
	;; 	SET_BSR	MIOS_GLCD_GCURSOR_X	; (BSR already set by USER_LCD_ShiftByte)
	incf	MIOS_GLCD_GCURSOR_X, F, BANKED
	;; deselect displays
	setf	USER_LCD_LAT_CS

	;; if new X position reached, set LCD cursor of all displays to 0
USER_LCD_Data_RstTst2
	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	xorlw	1*84
	bz	USER_LCD_Data_Rst
	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	xorlw	2*84
	bz	USER_LCD_Data_Rst
	return

USER_LCD_Data_Rst
	movlw	0x80
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
	;; store byte in GLCD buffer
	SET_BSR	MIOS_GLCD_BUFFER
	movwf	MIOS_GLCD_BUFFER+0, BANKED

        bcf	USER_LCD_LAT_SCLK, USER_LCD_PIN_SCLK	; clear clock

	;; NOTE: you will notice that the instructions are sometimes arranged
	;; in a special order to ensure proper output signals - for example:
	;; between a rising and a falling SCLK edge there is at least one
	;; other instruction to ensure that the high pulse of the SCLK
	;; is longer than 100 nS (the PCD8544 specifies a setup/hold time of 100 nS)

	clrf	MIOS_GLCD_BUFFER+1, BANKED	; init loop counter
USER_LCD_ShiftByte_Loop
	bcf	USER_LCD_LAT_SDA, USER_LCD_PIN_SDA	; set SDA depending on current MSB
	btfsc	MIOS_GLCD_BUFFER+0, 7, BANKED
	bsf	USER_LCD_LAT_SDA, USER_LCD_PIN_SDA
	rlf	MIOS_GLCD_BUFFER+0, F, BANKED		; shift the 8-bit value
	bsf	USER_LCD_LAT_SCLK, USER_LCD_PIN_SCLK	; rising clock edge
	incf	MIOS_GLCD_BUFFER+1, F, BANKED
	bcf	USER_LCD_LAT_SCLK, USER_LCD_PIN_SCLK	; falling clock edge
	BRA_IFCLR MIOS_GLCD_BUFFER+1, 3, BANKED, USER_LCD_ShiftByte_Loop ; loop 8 times

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
	SET_BSR MIOS_GLCD_GCURSOR_Y	; 5 lines to clear
	clrf	MIOS_GLCD_GCURSOR_Y, BANKED
USER_LCD_ClearOuterLoop
	SET_BSR MIOS_GLCD_GCURSOR_X	; 84 columns to clear
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
	;; 	SET_BSR	MIOS_GLCD_GCURSOR_X	; (BSR already set by USER_LCD_ShiftByte)
	incf	MIOS_GLCD_GCURSOR_X, F, BANKED
	movlw	84-1
	cpfsgt	MIOS_GLCD_GCURSOR_X, BANKED
	rgoto	USER_LCD_ClearInnerLoop

	incf	MIOS_GLCD_GCURSOR_Y, F, BANKED
	movlw	6-1
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
	;; 	rgoto	USER_LCD_GCursorSet


;; --------------------------------------------------------------------------
;;  This function is NOT called by MIOS, but only used by the custom driver
;;  to set the cursor positions
;;  In:  graphical cursor position in MIOS_GLCD_GCURSOR_X and MIOS_GLCD_GCURSOR_Y
;;  Out: -
;; --------------------------------------------------------------------------
USER_LCD_GCursorSet
	;; Set X position, wrap at 84 (LCD X cursor must be less than 84!)
	SET_BSR	MIOS_GLCD_GCURSOR_X
	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	movwf	MIOS_GLCD_BUFFER+0, BANKED
	movlw	84-1
	cpfsgt	MIOS_GLCD_BUFFER+0, BANKED
	rgoto	USER_LCD_GCursorSet_X_Cont
	movlw	-84
	addwf	MIOS_GLCD_BUFFER+0, F, BANKED
	movlw	84-1
	cpfsgt	MIOS_GLCD_BUFFER+0, BANKED
	rgoto	USER_LCD_GCursorSet_X_Cont
	movlw	-84
	addwf	MIOS_GLCD_BUFFER+0, F, BANKED
USER_LCD_GCursorSet_X_Cont
	movf	MIOS_GLCD_BUFFER+0, W, BANKED
	iorlw	0x80
	rcall	USER_LCD_Cmd

	;; Set X position, wrap at 6 (LCD Y cursor must be less than 6!)
	movf	MIOS_GLCD_GCURSOR_Y, W, BANKED
	movwf	MIOS_GLCD_BUFFER+0, BANKED
	movlw	6-1
	cpfsgt	MIOS_GLCD_BUFFER+0, BANKED
	rgoto	USER_LCD_GCursorSet_Y_Cont
	movlw	-6
	addwf	MIOS_GLCD_BUFFER+0, F, BANKED
	movlw	6-1
	cpfsgt	MIOS_GLCD_BUFFER+0, BANKED
	rgoto	USER_LCD_GCursorSet_Y_Cont
	movlw	-6
	addwf	MIOS_GLCD_BUFFER+0, F, BANKED
USER_LCD_GCursorSet_Y_Cont
	movf	MIOS_GLCD_BUFFER+0, W, BANKED
	iorlw	0x40
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
