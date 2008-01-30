; $Id$
;
; MIOS Dotmatrix LCD Driver (1st layer routines)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; Pins of LC-Display
MIOS_CLCD_LAT_D		EQU	LATB	; Pin B.7-0
MIOS_CLCD_PORT_D	EQU     PORTB
MIOS_CLCD_TRIS_D	EQU     TRISB

MIOS_CLCD_LAT_RW	EQU     LATD
MIOS_CLCD_PIN_RW	EQU     6               ; Pin D.6
MIOS_CLCD_LAT_RS	EQU     LATD
MIOS_CLCD_PIN_RS	EQU     5               ; Pin D.5

;; E pins now defined as MIOS_LCD_OPTION1 and MIOS_LCD_OPTION2
;;MIOS_CLCD_LAT_E	EQU     LATD		; Pin D.7
;;MIOS_CLCD_PIN_E	EQU     7
;;MIOS_CLCD_LAT_E2	EQU	LATC		; Pin C.4
;;MIOS_CLCD_PIN_E2	EQU	4

;; new names for CLCD registers
MIOS_CLCD_PIN_E0	EQU	MIOS_LCD_OPTION1; (CLCD, enable pin for display #1)
MIOS_CLCD_PIN_E1	EQU	MIOS_LCD_OPTION2; (CLCD, enable pin for display #2)
MIOS_CLCD_SC_CTR	EQU	MIOS_GLCD_TMP1
MIOS_CLCD_PIN_E		EQU	MIOS_GLCD_TMP2
MIOS_CLCD_STATUS	EQU	MIOS_GLCD_TMP3

#define MIOS_CLCD_STATUS_LCD0_DISABLED	0	; bit0:	if set, first LCD disabled
#define MIOS_CLCD_STATUS_LCD1_DISABLED	1	; bit1:	if set, second LCD disabled
#define MIOS_CLCD_STATUS_CUR_DISABLED	2	; bit2:	if set, currently selected LCD disabled
#define MIOS_CLCD_STATUS_CUR_LCD	3	; bit3: if cleared: current LCD is first LCD, else second LCD

;; --------------------------------------------------------------------------
;;  Init Routine for LC Display
;; --------------------------------------------------------------------------
MIOS_CLCD_Init
	;; notify that no graphical LCD is connected
	bcf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD

	; (Initialization of Ports: done in Init_Ports)
	SET_BSR	MIOS_LCD_TIMEOUT1
	clrf	MIOS_CLCD_STATUS, BANKED

	movlw	100			; 100 ms delay
	call	MIOS_Delay

        bcf     MIOS_CLCD_LAT_RW, MIOS_CLCD_PIN_RW	; LCD_WRITE
        bcf     MIOS_CLCD_LAT_RS, MIOS_CLCD_PIN_RS	; MIOS_CLCD_PIN_RS_0

	;; initialize first LCD
	rcall	MIOS_CLCD_Strobe_Prepare; prepare pointer to strobe pin
	rcall	MIOS_CLCD_Init_Cmds	; call init commands

	;; initialize second LCD if MIOS_LCD_Y2_OFFSET[7] or MIOS_LCD_Y3_OFFSET[7] set
	BRA_IFSET MIOS_LCD_Y2_OFFSET, 7, BANKED, MIOS_CLCD_Init_2nd
	BRA_IFSET MIOS_LCD_Y3_OFFSET, 7, BANKED, MIOS_CLCD_Init_2nd
	;; else disable second LCD and return
	bsf	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_LCD1_DISABLED, BANKED
	return

MIOS_CLCD_Init_2nd
	bsf	MIOS_LCD_CURSOR_POS, 7, BANKED	; select second display
	rcall	MIOS_CLCD_Strobe_Prepare; prepare pointer to strobe pin
	rcall	MIOS_CLCD_Init_Cmds	; call init commands
	bcf	MIOS_LCD_CURSOR_POS, 7, BANKED	; reset to first display

	return
	

MIOS_CLCD_Init_Cmds
	movlw	0x30
	movwf	MIOS_CLCD_LAT_D
	rcall	MIOS_CLCD_Strobe_Toggle
	movlw	50			; 50 ms delayed toggle
	rcall	MIOS_CLCD_Strobe_DelayToggle
	movlw	50			; 50 ms delayed toggle
	rcall	MIOS_CLCD_Strobe_DelayToggle

	BRA_IFCLR MIOS_CLCD_PIN_E, 7, BANKED, MIOS_CLCD_Init_Cmds_No4Bit
MIOS_CLCD_Init_Cmds_4Bit
	;; switch to 4bit mode
	movlw	0x20
	movwf	MIOS_CLCD_LAT_D
	movlw	1			; 1 mS delayed toggle
	rcall	MIOS_CLCD_Strobe_DelayToggle			; DB7-DB4: 0010

	movlw	1			; 1 mS delayed toggle
	rcall	MIOS_CLCD_Strobe_DelayToggle			; DB7-DB4: 0010

	clrf	MIOS_CLCD_LAT_D
	movlw	1			; 1 mS delayed toggle
	rcall	MIOS_CLCD_Strobe_DelayToggle			; DB7-DB4: 0000 (N/F)

	movlw	1			; 1 mS delayed toggle
	rcall	MIOS_CLCD_Strobe_DelayToggle			; DB7-DB4: 0000

	movlw	0x10
	movwf	MIOS_CLCD_LAT_D
	movlw	1			; 1 mS delayed toggle
	rcall	MIOS_CLCD_Strobe_DelayToggle			; DB7-DB4: 0001

	clrf	MIOS_CLCD_LAT_D
	movlw	1			; 1 mS delayed toggle
	rcall	MIOS_CLCD_Strobe_DelayToggle			; DB7-DB4: 0000
MIOS_CLCD_Init_Cmds_No4Bit

	movlw	0x08			; Display Off
	rcall	MIOS_CLCD_Cmd
	movlw	0x0c			; Display On
	rcall	MIOS_CLCD_Cmd
	movlw	0x06			; Entry Mode
	rcall	MIOS_CLCD_Cmd
	movlw	0x01			; Clear Display
	call	MIOS_CLCD_Cmd
	bcf	MIOS_LCD_TIMEOUT1, 7, BANKED	; everything ok, make sure that LCD_TIMEOUT, bit 7 is cleared
	
	movlw	0x38					; select 8-bit interface again
	btfsc	MIOS_CLCD_PIN_E, 7, BANKED; select 4-bit interface again
	movlw 0x28	
	rcall	MIOS_CLCD_Cmd
	movlw	0x0c
	rcall	MIOS_CLCD_Cmd
	movlw	0x00			; set cursor to zero pos
	rgoto	MIOS_CLCD_CursorSet

	
;; ==========================================================================

MIOS_CLCD_Data
MIOS_CLCD_PrintChar
	;; store byte in data latch
	movwf	MIOS_CLCD_LAT_D

	;; wait until display unbusy
	rcall	MIOS_CLCD_WaitUnbusy

	;; exit if current LCD not available due to timeout
	btfsc	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_CUR_DISABLED, BANKED
	return

	;; select data register
        bsf     MIOS_CLCD_LAT_RS, MIOS_CLCD_PIN_RS

	;; activate write
        bcf     MIOS_CLCD_LAT_RW, MIOS_CLCD_PIN_RW

	BRA_IFCLR MIOS_CLCD_PIN_E, 7, BANKED, MIOS_CLCD_Data_No4bit
MIOS_CLCD_Data_4bit
	;; transfer upper 4 bit
	rcall	MIOS_CLCD_Strobe_Toggle
	;; transfer lower 4 bit
	swapf	MIOS_CLCD_LAT_D, F
MIOS_CLCD_Data_No4bit

	;; strobe and exit
	rgoto	MIOS_CLCD_Strobe_Toggle

;; ==========================================================================

MIOS_CLCD_Cmd
	;; store byte in data latch
	movwf	MIOS_CLCD_LAT_D

	;; wait until display unbusy
	rcall	MIOS_CLCD_WaitUnbusy

	;; exit if current LCD not available due to timeout
	btfsc	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_CUR_DISABLED, BANKED
	return

	;; select command register
        bcf     MIOS_CLCD_LAT_RS, MIOS_CLCD_PIN_RS

	;; activate write
        bcf     MIOS_CLCD_LAT_RW, MIOS_CLCD_PIN_RW

	BRA_IFCLR MIOS_CLCD_PIN_E, 7, BANKED, MIOS_CLCD_Cmd_No4bit
MIOS_CLCD_Cmd_4bit
	;; transfer upper 4 bit
	rcall	MIOS_CLCD_Strobe_Toggle
	;; transfer lower 4 bit
	swapf	MIOS_CLCD_LAT_D, F
MIOS_CLCD_Cmd_No4bit

	;; strobe and exit
	rgoto	MIOS_CLCD_Strobe_Toggle

;; --------------------------------------------------------------------------
;;  INTERNAL: wait until LCD unbusy
;; --------------------------------------------------------------------------
MIOS_CLCD_WaitUnbusy
	;; prepare strobe address
	rcall	MIOS_CLCD_Strobe_Prepare

	;; exit if current LCD not available due to timeout
	btfsc	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_CUR_DISABLED, BANKED
	return

	;; turn off output drivers
	movlw	0xff
	btfsc	MIOS_CLCD_PIN_E, 7, BANKED
	movlw	0xf0		; (for 4bit mode: don't touch TRISB[3:0]
	iorwf	MIOS_CLCD_TRIS_D, F

	;; select command register
        bcf     MIOS_CLCD_LAT_RS, MIOS_CLCD_PIN_RS

	;; poll busy bit
	clrf	MIOS_LCD_TIMEOUT0, BANKED
	clrf	MIOS_LCD_TIMEOUT1, BANKED

        bsf     MIOS_CLCD_LAT_RW, MIOS_CLCD_PIN_RW	; LCD_READ
MIOS_CLCD_WaitUnbusy_Loop
	rcall	MIOS_CLCD_Strobe_Clr
	incf	MIOS_LCD_TIMEOUT0, F, BANKED
	skpnz
	incf	MIOS_LCD_TIMEOUT1, F, BANKED
	bz	MIOS_CLCD_WaitUnbusy_Disable	; leave loop when LCD_TIMEOUT = 0xff. Up to now bit 7 is set and the LCD
						; busy routine will never be called again
	rcall	MIOS_CLCD_Strobe_Set
	BRA_IFCLR MIOS_CLCD_PORT_D, 7, ACCESS, MIOS_CLCD_WaitUnbusy_UnBusy
MIOS_CLCD_WaitUnbusy_Busy
	;; 8 bit: next iteration
	BRA_IFCLR MIOS_CLCD_PIN_E, 7, BANKED, MIOS_CLCD_WaitUnbusy_Loop
	;; 4 bit: strobe, thereafter next iteration
	rcall	MIOS_CLCD_Strobe_Clr
	rcall	MIOS_CLCD_Strobe_Set
	rgoto	MIOS_CLCD_WaitUnbusy_Loop

MIOS_CLCD_WaitUnbusy_UnBusy
	BRA_IFCLR MIOS_CLCD_PIN_E, 7, BANKED, MIOS_CLCD_WaitUnbusy_No4bit
MIOS_CLCD_WaitUnbusy_4bit
	rcall	MIOS_CLCD_Strobe_Clr
	rcall	MIOS_CLCD_Strobe_Set
MIOS_CLCD_WaitUnbusy_No4bit
	rcall	MIOS_CLCD_Strobe_Clr

MIOS_CLCD_WaitUnbusy_End
	;; turn on output drivers again
	movlw	0x00
	btfsc	MIOS_CLCD_PIN_E, 7, BANKED
	movlw	0x0f		; (for 4bit mode: don't touch TRISB[3:0]
	andwf	MIOS_CLCD_TRIS_D, F
	return

MIOS_CLCD_WaitUnbusy_Disable
	;; disable currently selected LCD
	btfss	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_CUR_LCD, BANKED
	bsf	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_LCD0_DISABLED, BANKED
	btfsc	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_CUR_LCD, BANKED
	bsf	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_LCD1_DISABLED, BANKED
	rgoto	MIOS_CLCD_WaitUnbusy_End

;; ==========================================================================

MIOS_CLCD_Strobe_Prepare
	;; set status flag depending on currently selected LCD
	SET_BSR	MIOS_CLCD_STATUS
	bcf	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_CUR_DISABLED, BANKED
	bcf	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_CUR_LCD, BANKED

	;; determine if first or second E pin should be addressed depending on cursor pos
	SET_BSR	MIOS_CLCD_PIN_E
	movf	MIOS_CLCD_PIN_E0, W, BANKED
	BRA_IFSET MIOS_LCD_Y2_OFFSET, 7, BANKED, MIOS_CLCD_Strobe_Prepare_Chk2nd
	BRA_IFSET MIOS_LCD_Y3_OFFSET, 7, BANKED, MIOS_CLCD_Strobe_Prepare_Chk2nd
MIOS_CLCD_Strobe_Prepare_Chk2nd
	BRA_IFSET MIOS_LCD_CURSOR_POS, 7, BANKED, MIOS_CLCD_Strobe_Prepare_2nd
MIOS_CLCD_Strobe_Prepare_1st
	;; select current LCD
	bcf	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_CUR_LCD, BANKED
	;; transfer pin number to MIOS_CLCD_PIN_E
	movff	MIOS_CLCD_PIN_E0, MIOS_CLCD_PIN_E
	;; set disable flag if required
	btfsc	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_LCD0_DISABLED, BANKED
	bsf	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_CUR_DISABLED, BANKED
	rgoto	MIOS_CLCD_Strobe_Prepare_Cont

MIOS_CLCD_Strobe_Prepare_2nd
	;; select current LCD
	bsf	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_CUR_LCD, BANKED
	;; transfer pin number to MIOS_CLCD_PIN_E
	movff	MIOS_CLCD_PIN_E1, MIOS_CLCD_PIN_E
	;; set disable flag if required
	btfsc	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_LCD1_DISABLED, BANKED
	bsf	MIOS_CLCD_STATUS, MIOS_CLCD_STATUS_CUR_DISABLED, BANKED
	;; 	rgoto	MIOS_CLCD_Strobe_Prepare_Cont

MIOS_CLCD_Strobe_Prepare_Cont	
	;; calc address to LATx -> FSR1
	lfsr	FSR1, LATA
	swapf	MIOS_CLCD_PIN_E, W, BANKED
	andlw	0x07
	addwf	FSR1L, F
	return

;; ----
MIOS_CLCD_Strobe_Set
	BRA_IFSET MIOS_CLCD_PIN_E, 2, BANKED, MIOS_CLCD_Strobe_Set_4567
MIOS_CLCD_Strobe_Set_0123
	BRA_IFSET MIOS_CLCD_PIN_E, 1, BANKED, MIOS_CLCD_Strobe_Set_23
MIOS_CLCD_Strobe_Set_12
	BRA_IFSET MIOS_CLCD_PIN_E, 0, BANKED, MIOS_CLCD_Strobe_Set_1
MIOS_CLCD_Strobe_Set_0
        bsf     INDF1, 0
	return
MIOS_CLCD_Strobe_Set_1
        bsf     INDF1, 1
	return

MIOS_CLCD_Strobe_Set_23
	BRA_IFSET MIOS_CLCD_PIN_E, 0, BANKED, MIOS_CLCD_Strobe_Set_3
MIOS_CLCD_Strobe_Set_2
        bsf     INDF1, 2
	return
MIOS_CLCD_Strobe_Set_3
        bsf     INDF1, 3
	return

MIOS_CLCD_Strobe_Set_4567
	BRA_IFSET MIOS_CLCD_PIN_E, 1, BANKED, MIOS_CLCD_Strobe_Set_67
MIOS_CLCD_Strobe_Set_45
	BRA_IFSET MIOS_CLCD_PIN_E, 0, BANKED, MIOS_CLCD_Strobe_Set_5
MIOS_CLCD_Strobe_Set_4
        bsf     INDF1, 4
	return
MIOS_CLCD_Strobe_Set_5
        bsf     INDF1, 5
	return

MIOS_CLCD_Strobe_Set_67
	BRA_IFSET MIOS_CLCD_PIN_E, 0, BANKED, MIOS_CLCD_Strobe_Set_7
MIOS_CLCD_Strobe_Set_6
        bsf     INDF1, 6
	return
MIOS_CLCD_Strobe_Set_7
        bsf     INDF1, 7
	return

;; ----

;; ----
MIOS_CLCD_Strobe_Clr
	;; use enable pin of first display
	lfsr	FSR1, LATA
	swapf	MIOS_CLCD_PIN_E, W, BANKED
	andlw	0x07
	addwf	FSR1L, F
	movf	MIOS_CLCD_PIN_E, W, BANKED

	BRA_IFSET MIOS_CLCD_PIN_E, 2, BANKED, MIOS_CLCD_Strobe_Clr_4567
MIOS_CLCD_Strobe_Clr_0123
	BRA_IFSET MIOS_CLCD_PIN_E, 1, BANKED, MIOS_CLCD_Strobe_Clr_23
MIOS_CLCD_Strobe_Clr_12
	BRA_IFSET MIOS_CLCD_PIN_E, 0, BANKED, MIOS_CLCD_Strobe_Clr_1
MIOS_CLCD_Strobe_Clr_0
        bcf     INDF1, 0
	return
MIOS_CLCD_Strobe_Clr_1
        bcf     INDF1, 1
	return

MIOS_CLCD_Strobe_Clr_23
	BRA_IFSET MIOS_CLCD_PIN_E, 0, BANKED, MIOS_CLCD_Strobe_Clr_3
MIOS_CLCD_Strobe_Clr_2
        bcf     INDF1, 2
	return
MIOS_CLCD_Strobe_Clr_3
        bcf     INDF1, 3
	return

MIOS_CLCD_Strobe_Clr_4567
	BRA_IFSET MIOS_CLCD_PIN_E, 1, BANKED, MIOS_CLCD_Strobe_Clr_67
MIOS_CLCD_Strobe_Clr_45
	BRA_IFSET MIOS_CLCD_PIN_E, 0, BANKED, MIOS_CLCD_Strobe_Clr_5
MIOS_CLCD_Strobe_Clr_4
        bcf     INDF1, 4
	return
MIOS_CLCD_Strobe_Clr_5
        bcf     INDF1, 5
	return

MIOS_CLCD_Strobe_Clr_67
	BRA_IFSET MIOS_CLCD_PIN_E, 0, BANKED, MIOS_CLCD_Strobe_Clr_7
MIOS_CLCD_Strobe_Clr_6
        bcf     INDF1, 6
	return
MIOS_CLCD_Strobe_Clr_7
        bcf     INDF1, 7
	return

;; ==========================================================================
;; help function which adds delay before toggling - mS in WREG
MIOS_CLCD_Strobe_DelayToggle
	call	MIOS_Delay
	;; 	rgoto	MIOS_CLCD_Strobe_Toggle

;; help function which toggles the strobe line
MIOS_CLCD_Strobe_Toggle
	rcall	MIOS_CLCD_Strobe_Set
	rgoto	MIOS_CLCD_Strobe_Clr

;; ==========================================================================

MIOS_CLCD_Clear
	movlw	0x01
	call	MIOS_CLCD_Cmd
	BRA_IFSET MIOS_LCD_Y2_OFFSET, 7, BANKED, MIOS_CLCD_Clear2
	BRA_IFSET MIOS_LCD_Y3_OFFSET, 7, BANKED, MIOS_CLCD_Clear2
	return
MIOS_CLCD_Clear2
	bsf	MIOS_LCD_CURSOR_POS, 7, BANKED
	movlw	0x01
	call	MIOS_CLCD_Cmd
	bcf	MIOS_LCD_CURSOR_POS, 7, BANKED
	return

;; ==========================================================================

MIOS_CLCD_CursorSet
	iorlw	0x80
	rgoto	MIOS_CLCD_Cmd

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_CLCD_SpecialCharInit
;;  C_DECLARATION: void MIOS_CLCD_SpecialCharInit(unsigned char num, code char *c_table)
;;  DESCRIPTION: initializes one of 8 special characters
;;  provided by a HD44780 compatible character LCD
;;  IN:	  number of special character (0-7) in WREG
;;        pointer to special char pattern in TBLPTR (must consist of 8
;;           entries for every character-line)
;;  C_IN: number of special character (0-7) in <num>
;;        pointer to special char pattern in <c_table> (must consist of 8
;;           entries for every character-line)
;;  OUT:  TBLPTR will be set to next table entry (TBLPTR+=8)
;;  C_OUT:  -
;;  USES: BRS, TBLPTR
;;  EXAMPLE:
;;	;; use a left-arrow as special char #2
;;	TABLE_ADDR SPECIAL_CHAR_LEFT_ARROW
;;      movlw   0x02
;;	call	MIOS_CLCD_SpecialCharInit
;;	;; set cursor to 0
;;	movlw	0x00
;;	call	MIOS_LCD_CursorSet
;;	;; print the special char
;;	movlw	0x02
;;	call	MIOS_LCD_PrintChar
;;	return
;;
;; SPECIAL_CHAR_LEFT_ARROW
;;      ;; due to an imperfection in the MPASM we have
;;      ;; to write two bytes in every line :-(
;;	db b'00000011', b'00000111' ; 1st and 2nd line of special char
;;	db b'00001111', b'00011111' ; 3rd and 4th line of special char
;;	db b'00011111', b'00001111' ; 5th and 6th line of special char
;;	db b'00000111', b'00000011' ; 7th and 8th line of special char
;;
;;  C_EXAMPLE:
;;    see http://www.ucapps.de/mios_c_lcd_schars.html
;; --------------------------------------------------------------------------
MIOS_CLCD_SpecialCharInit
	;; branch depending on LCD type
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE2, ACCESS, MIOS_CLCD_SpecialChar_4567
MIOS_CLCD_SpecialChar_1234
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_CLCD_SpecialChar_34
MIOS_CLCD_SpecialChar_12
	BRA_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0, ACCESS, MIOS_CLCD_SpecialCharInit_C
	return

MIOS_CLCD_SpecialChar_34
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return

MIOS_CLCD_SpecialChar_4567
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_CLCD_SpecialChar_67
MIOS_CLCD_SpecialChar_45
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return
MIOS_CLCD_SpecialChar_67
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	goto	USER_LCD_SpecialCharInit



	;; continue if common CLCD
MIOS_CLCD_SpecialCharInit_C
	swapf	WREG, F
	rrf	WREG, W
	andlw	0x38
	iorlw	0x40
	rcall	MIOS_CLCD_Cmd

	SET_BSR	MIOS_CLCD_SC_CTR
	clrf	MIOS_CLCD_SC_CTR, BANKED
MIOS_CLCD_SpecialCharInitLoop
	tblrd*+
	movf	TABLAT, W
	rcall	MIOS_CLCD_Data
	incf	MIOS_CLCD_SC_CTR, F, BANKED
	BRA_IFCLR MIOS_CLCD_SC_CTR, 3, BANKED, MIOS_CLCD_SpecialCharInitLoop

	movf	MIOS_LCD_CURSOR_POS, W, BANKED
	rgoto	MIOS_CLCD_CursorSet

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_CLCD_SpecialCharsInit
;;  C_DECLARATION: void MIOS_CLCD_SpecialCharsInit(const char *c_table)
;;  DESCRIPTION: initializes all 8 special characters
;;  provided by a HD44780 compatible character LCD<BR>
;;  See also: MIOS_CLCD_SpecialCharInit
;;  IN:	  pointer to special char patterns in TBLPTR (must consist of 8*8
;;           entries for every character and line)
;;  C_IN: pointer to special char patterns in <c_table> (must consist of 8*8
;;           entries for every character and line)
;;  OUT:  TBLPTR will be set to next table entry (TBLPTR+=64)
;;  C_OUT:  -
;;  USES: BRS, TBLPTR
;;  EXAMPLE:
;;     for a single character: see MIOS_CLCD_SpecialCharInit
;;     this function does the same, but it initializes
;;     all 8 characters at once
;;
;;  C_EXAMPLE:
;;    see http://www.ucapps.de/mios_c_lcd_schars.html
;; --------------------------------------------------------------------------
MIOS_CLCD_SpecialCharsInit
	SET_BSR	MIOS_TMP1
	clrf	MIOS_TMP1, BANKED
MIOS_CLCD_SpecialCharsInitLoop
	movf	MIOS_TMP1, W, BANKED
	rcall	MIOS_CLCD_SpecialCharInit
	incf	MIOS_TMP1, F, BANKED
	BRA_IFCLR MIOS_TMP1, 3, BANKED, MIOS_CLCD_SpecialCharsInitLoop
	return
