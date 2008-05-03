; $Id$
;
; MIOS LCD Handler (2nd layer routines)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

MIOS_LCD_TYPE_CLCD		EQU	0x00
MIOS_LCD_TYPE_GLCD0		EQU	0x01
MIOS_LCD_TYPE_MLCD		EQU	0x06
MIOS_LCD_TYPE_CUSTOM_LCD	EQU	0x07

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_TypeSet
;;  C_DECLARATION: void MIOS_LCD_TypeSet(unsigned char type, unsigned char par1, unsigned char par2)
;;  DESCRIPTION: sets the LCD type.
;;  Following LCDs are provided:<BR>
;;     0x00:	MIOS_LCD_TYPE_CLCD      (character dotmatrix LCD)<BR>
;;     0x01:	MIOS_LCD_TYPE_GLCD0	(graphical LCD, KS0108 or HD61202 compatible)<BR>
;;     0x06:	MIOS_LCD_TYPE_MLCD	(MIDI display, see SysEx implementation)<BR>
;;     0x07:	MIOS_LCD_TYPE_CUSTOM_LCD (custom LCD driver)<BR>
;;  Note that the MIOS_LCD_TypeAutoSet function allows you to derive the LCD
;;  type from the PIC ID header!
;;
;;  Use this function only if you want to force a special LCD type without
;;  changing the ID header of your PIC or if you want to set additional LCD
;;  options which are different from the default values
;;  IN:	  LCD type in WREG
;;        LCD Option #1 in MIOS_PARAMETER1
;;        LCD Option #2 in MIOS_PARAMETER2
;;  C_IN: LCD type in <type>
;;        LCD Option #1 <par1>
;;        LCD Option #2 <par2>
;;  OUT:  -
;;  USES: -
;;  EXAMPLE:
;;
;;  The available LCD options differ with the used LCD type, here a list:
;;
;;  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;;
;;  Type 0, CLCD:
;;     with MIOS_PARAMETER1 and MIOS_PARAMETER2 the enable (E) pins of
;;     the character displays are specified. By default the E input of 
;;     the first CLCD should be connected to pin D.7, the E input of 
;;     the second CLCD (if available) to C.4
;;     Following lines of code are necessary to achieve this:
;;
;;	;; use a CLCD, E input of first CLCD at D.7, E of second CLCD @C.4
;;	movlw	0x37		; E1: D.7, 8bit interface
;;	movwf	MIOS_PARAMETER1
;;	movlw	0x24		; E2: C.4, 8bit interface
;;	movwf	MIOS_PARAMETER2
;;	movlw	MIOS_LCD_TYPE_CLCD ; LCD type 0
;;	call	MIOS_LCD_TypeSet
;;
;;     Note: the first digit of the parameter value sets the port number
;;           (A=0, B=1, C=2, D=3, E=4)
;;           the second digit sets the pin number (0-7)
;;           bit #7 sets the interface (0=8bit, 0=7bit)
;;
;;     Example for CLCD with 4bit interface:
;;	;; use a CLCD, E input of first CLCD at D.7, E of second CLCD @C.4
;;	;; using the 4-bit interface:
;;	;; -> connect MBHP_CORE:J15:D7-D4 of the core module to D7-D4 of the LCD
;;      ;; -> left MBHP_CORE:J15:D3-D0 of the core module open!
;;	;; -> tie D3-D0 of the LCD to ground
;;	movlw	0x37 | 0x80	; E1: D.7, 4bit interface
;;	movwf	MIOS_PARAMETER1
;;	movlw	0x24 | 0x80	; E2: C.4, 4bit interface
;;	movwf	MIOS_PARAMETER2
;;	movlw	MIOS_LCD_TYPE_CLCD ; LCD type 0
;;	call	MIOS_LCD_TypeSet
;;
;;     Tip: by changing these parameters dynamically you are also able to 
;;          drive more than 2 LCDs with one core module. Only the number
;;          of free pins limit the number of CLCDs which can be driven!
;;          Don't forget to switch the appr. pins to output before using 
;;          them, and call MIOS_LCD_Init for every display pair in your
;;          USER_Init
;;
;;  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;;
;; Type 1, KS0108 or HD61202 based
;;     with MIOS_PARAMETER1 it has to be specified if the chip select
;;     lines are inverted
;;
;;	;; use a KS0108 or HD61202 compatible graphical LCD with
;;	;; non-inverted chip selects
;;	movlw	0x00
;;	movwf	MIOS_PARAMETER1
;;	movlw	MIOS_LCD_TYPE_GLCD0
;;	call	MIOS_LCD_TypeSet
;;
;;   OR:
;;	;; use a KS0108 or HD61202 compatible graphical LCD with inverted
;;	;; chip selects
;;	movlw	0x01
;;	movwf	MIOS_PARAMETER1
;;	movlw	MIOS_LCD_TYPE_GLCD0
;;	call	MIOS_LCD_TypeSet
;;
;;  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;;
;; Type 2, T6963C based
;;     not supported anymore, use the custom driver instead which are
;;     available in the MIOS download section
;;
;;  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;;
;; Type 6, MIDI display
;;     see the MIOS SysEx implementation which is available in the MIOS
;;     download section
;;     MIOS_PARAMETER1 defines the device ID of the target core:
;;
;;	;; send MIDI messages to core with device ID 0x42
;;      clrf	MIOS_PARAMETER2		; unused (yet)
;;	movlw	0x42
;;	movwf	MIOS_PARAMETER1		; device ID of target core
;;	movlw	MIOS_LCD_TYPE_MLCD	; LCD type #6
;;	call	MIOS_LCD_TypeSet
;;
;;  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;;
;; Type 7, custom LCD driver
;;     options are forwarded to the custom LCD driver
;;     following hooks have been prepared for an easy integration into
;;     MIOS:
;;        USER_LCD_Init
;;        USER_LCD_Clear
;;        USER_LCD_CursorSet
;;        USER_LCD_PrintChar
;;
;;  C_EXAMPLE:
;;
;;  The available LCD options differ with the used LCD type, here a list:
;;
;;  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;;
;;  Type 0, CLCD:
;;     with <par1> and <par2> the enable (E) pins of
;;     the character displays are specified. By default the E input of 
;;     the first CLCD should be connected to pin D.7, the E input of 
;;     the second CLCD (if available) to C.4
;;     Following lines of code are necessary to achieve this:
;;
;;	// use a CLCD, E input of first CLCD at D.7, E of second CLCD @C.4
;;	MIOS_LCD_TypeSet(MIOS_LCD_TYPE_CLCD, 0x37, 0x24);
;;
;;     Note: the first digit of the parameter value sets the port number
;;           (A=0, B=1, C=2, D=3, E=4)
;;           the second digit sets the pin number (0-7)
;;           bit #7 sets the interface (0=8bit, 0=7bit)
;;
;;     Example for CLCD with 4bit interface:
;;	;; use a CLCD, E input of first CLCD at D.7, E of second CLCD @C.4
;;	;; using the 4-bit interface:
;;	;; -> connect MBHP_CORE:J15:D7-D4 of the core module to D7-D4 of the LCD
;;      ;; -> left MBHP_CORE:J15:D3-D0 of the core module open!
;;	;; -> tie D3-D0 of the LCD to ground
;;	MIOS_LCD_TypeSet(MIOS_LCD_TYPE_CLCD, 0x80 | 0x37, 0x80 | 0x24);
;;
;;     Tip: by changing these parameters dynamically you are also able to 
;;          drive more than 2 LCDs with one core module. Only the number
;;          of free pins limit the number of CLCDs which can be driven!
;;          Don't forget to switch the appr. pins to output before using 
;;          them, and call MIOS_LCD_Init for every display pair in your
;;          USER_Init
;;
;;  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;;
;; Type 1, KS0108 or HD61202 based
;;     with <par1> it has to be specified if the chip select
;;     lines are inverted
;;
;;	// use a KS0108 or HD61202 compatible graphical LCD with
;;	// non-inverted chip selects
;;	MIOS_LCD_TypeSet(MIOS_LCD_TYPE_GLCD0, 0x00, 0x00); // (par2 unusued)
;;
;;   OR:
;;	// use a KS0108 or HD61202 compatible graphical LCD with inverted
;;	// chip selects
;;	MIOS_LCD_TypeSet(MIOS_LCD_TYPE_GLCD0, 0x01, 0x00); // (par2 unusued)
;;
;;  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;;
;; Type 2, T6963C based
;;     not supported anymore, use the custom driver instead which are
;;     available in the MIOS download section
;;
;;  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;;
;; Type 6, MIDI display
;;     see the MIOS SysEx implementation which is available in the MIOS
;;     download section
;;     MIOS_PARAMETER1 defines the device ID of the target core:
;;
;;	;; send MIDI messages to core with device ID 0x42
;;	MIOS_LCD_TypeSet(MIOS_LCD_TYPE_MLCD, 0x42, 0x00); // (par2 unusued)
;;
;;  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;;
;; Type 7, custom LCD driver
;;     options are forwarded to the custom LCD driver
;;     following hooks have been prepared for an easy integration into
;;     MIOS:
;;        USER_LCD_Init
;;        USER_LCD_Clear
;;        USER_LCD_CursorSet
;;        USER_LCD_PrintChar
;;
;; --------------------------------------------------------------------------
MIOS_LCD_TypeSet
	bcf	MIOS_BOX_CFG0, 0	; ..or do you know a better method to
	bcf	MIOS_BOX_CFG0, 1	; mask out 4 bits w/o using WREG?
	bcf	MIOS_BOX_CFG0, 2
	bcf	MIOS_BOX_CFG0, 3
	andlw	0x07
	iorwf	MIOS_BOX_CFG0, F

	;; copy additional options
	movff	MIOS_PARAMETER1, MIOS_LCD_OPTION1
	movff	MIOS_PARAMETER2, MIOS_LCD_OPTION2

	;; clear timeout
	SET_BSR	MIOS_LCD_TIMEOUT1
	clrf	MIOS_LCD_TIMEOUT1, BANKED

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_TypeAutoSet
;;  C_DECLARATION: unsigned char MIOS_LCD_TypeAutoSet(void)
;;  DESCRIPTION: derives the LCD type from the PIC ID header
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  type in WREG,
;;        additional LCD parameters in MIOS_PARAMETER1 and MIOS_PARAMETER2
;;  C_OUT:  type in WREG,
;;        additional LCD parameters in MIOS_PARAMETER1 and MIOS_PARAMETER2
;;  USES: BSR, TBLPTR
;;  EXAMPLE:
;;
;;	;; set LCD type to the specified type in the PIC ID header
;;	call	MIOS_LCD_TypeAutoSet
;; 
;;  C_EXAMPLE:
;;
;;	// set LCD type to the specified type in the PIC ID header
;;	MIOS_LCD_TypeAutoSet();
;; 
;; --------------------------------------------------------------------------
MIOS_LCD_TypeAutoSet
	IRQ_DISABLE			; we change TBLPTRU - IRQs have to be disabled!
	TABLE_ADDR_FULL _IDLOC6		; derive interface type from ID sector
	tblrd*				; get first byte

	;; clear the parameters by default
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2

	;; set additional parameters depending on type
	BRA_IFSET TABLAT, 6, ACCESS, MIOS_LCD_TypeAutoSet_4567
MIOS_LCD_TypeAutoSet_0123
	BRA_IFSET TABLAT, 5, ACCESS, MIOS_LCD_TypeAutoSet_23
MIOS_LCD_TypeAutoSet_01
	BRA_IFSET TABLAT, 4, ACCESS, MIOS_LCD_TypeAutoSet_1
MIOS_LCD_TypeAutoSet_0
	;; CLCD: define default pins for first and second LCD
#if PIC_DERIVATIVE_SET_LCD_4BIT	; for CAN derivatives, where Tx/Rx is located at RB2/RB3
	movlw	0x37 | 0x80	; Pin D.7, 4bit mode
	movwf	MIOS_PARAMETER1
	movlw	0x24 | 0x80	; Pin C.4, 4bit mode
	movwf	MIOS_PARAMETER2
#else
	movlw	0x37		; Pin D.7
	movwf	MIOS_PARAMETER1
	movlw	0x24		; Pin C.4
	movwf	MIOS_PARAMETER2
#endif
	rgoto	MIOS_LCD_TypeAutoSet_Cont

MIOS_LCD_TypeAutoSet_1
	;; GLCD0: non-inverted chip select by default
	;; (MIOS_PARAMETER1==0)
	rgoto	MIOS_LCD_TypeAutoSet_Cont

MIOS_LCD_TypeAutoSet_23
	BRA_IFSET TABLAT, 4, ACCESS, MIOS_LCD_TypeAutoSet_3
MIOS_LCD_TypeAutoSet_2
	rgoto	MIOS_LCD_TypeAutoSet_Cont
MIOS_LCD_TypeAutoSet_3
	rgoto	MIOS_LCD_TypeAutoSet_Cont

MIOS_LCD_TypeAutoSet_4567
	BRA_IFSET TABLAT, 5, ACCESS, MIOS_LCD_TypeAutoSet_67
MIOS_LCD_TypeAutoSet_45
	BRA_IFSET TABLAT, 4, ACCESS, MIOS_LCD_TypeAutoSet_5
MIOS_LCD_TypeAutoSet_4
	rgoto	MIOS_LCD_TypeAutoSet_Cont
MIOS_LCD_TypeAutoSet_5
	rgoto	MIOS_LCD_TypeAutoSet_Cont

MIOS_LCD_TypeAutoSet_67
	BRA_IFSET TABLAT, 4, ACCESS, MIOS_LCD_TypeAutoSet_7
MIOS_LCD_TypeAutoSet_6
	;; MLCD: device ID == 0
	;; (MIOS_PARAMETER1==0)
	rgoto	MIOS_LCD_TypeAutoSet_Cont
MIOS_LCD_TypeAutoSet_7
	;; Custom LCD: no auto options
	rgoto	MIOS_LCD_TypeAutoSet_Cont

MIOS_LCD_TypeAutoSet_Cont
	swapf	TABLAT, W
	rcall	MIOS_LCD_TypeSet
	clrf	TBLPTRU			; clear upper table pointer so
	IRQ_ENABLE			; that the IRQs can be enabled again
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_TypeGet
;;  C_DECLARATION: unsigned char MIOS_LCD_TypeGet(void)
;;  DESCRIPTION: returns the LCD type
;;  Following LCDs are provided:<BR>
;;     0x00:	MIOS_LCD_TYPE_CLCD      (character dotmatrix LCD)<BR>
;;     0x01:	MIOS_LCD_TYPE_GLCD0	(graphical LCD, KS0108 or HD61202 compatible)<BR>
;;     0x06:	MIOS_LCD_TYPE_MLCD	(MIDI display, see SysEx implementation)<BR>
;;     0x07:	MIOS_LCD_TYPE_CUSTOM_LCD (custom LCD driver)<BR>
;;  IN:	  -
;;  C_IN:	  -
;;  OUT:  LCD type in WREG and MIOS_PARAMETER1
;;  C_OUT:  LCD type
;;  USES: -
;;  EXAMPLE:
;; 
;;	;; branch depending on LCD type
;;	call	MIOS_LCD_TypeGet
;;	bnz     NoCLCD
;;
;;
;;	;; !!! Better for branching depending on CLCD/GLCD type display !!!
;;	;; !!! (works also with custom LCD driver                       !!!
;;	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD, ACCESS, NoCLCD
;;
;;  C_EXAMPLE:
;; 
;;	// branch depending on LCD type
;;	if( MIOS_LCD_TypeGet() == MIOS_LCD_TYPE_CLCD ) {
;;	  // do something special if CLCD connected
;;	}
;;
;;
;;	// !!! Better for branching depending on CLCD/GLCD type display !!!
;;	// !!! (works also with custom LCD driver                       !!!
;;	if( !MIOS_BOX_CFG0.USE_GLCD ) {
;;	  // do something special if CLCD connected
;;	}
;;
;; --------------------------------------------------------------------------
MIOS_LCD_TypeGet
	movf	MIOS_BOX_CFG0, W
	andlw	0x07
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_YAddressSet
;;  C_DECLARATION: void MIOS_LCD_YAddressSet(unsigned char line0, unsigned char line1, unsigned char line2, unsigned char line3);
;;  DESCRIPTION: maps the Y position of the cursor to the appr. cursor
;;  address of character and graphical displays.<BR>
;;  By default the positions are configured for 2x16, 2x20, 4x20 and 2x40 displays:<BR>
;;     Line0 -> MIOS cursor offset: 0x00 -> cursor address 0x00<BR>
;;     Line1 -> MIOS cursor offset: 0x40 -> cursor address 0x40<BR>
;;     Line2 -> MIOS cursor offset: 0x80 -> cursor address 0x14 (4x20 only)<BR>
;;     Line3 -> MIOS cursor offset: 0xc0 -> cursor address 0x54 (4x20 only)<BR>
;;  For 4x16 displays, the configuration has to be changed:<BR>
;;     Line0 -> MIOS cursor offset: 0x00 -> cursor address 0x00<BR>
;;     Line1 -> MIOS cursor offset: 0x40 -> cursor address 0x40<BR>
;;     Line2 -> MIOS cursor offset: 0x80 -> cursor address 0x10 (4x16 only)<BR>
;;     Line3 -> MIOS cursor offset: 0xc0 -> cursor address 0x50 (4x16 only)<BR>
;;  For 2 character displays connected to one core use:
;;     Line0 -> MIOS cursor offset: 0x00 -> cursor address 0x00<BR>
;;     Line1 -> MIOS cursor offset: 0x40 -> cursor address 0x40<BR>
;;     Line2 -> MIOS cursor offset: 0x80 -> cursor address 0x80<BR>
;;     Line3 -> MIOS cursor offset: 0xc0 -> cursor address 0xc0<BR>
;;  (first LCD is addressed by 0x00-0x7f, second LCD is addressed by 0x80-0xff)<BR>
;;  <BR>
;;  This function allows also to center the screen if you want to run<BR>
;;  Applications which have been developed for a 2x16 display on a<BR>
;;  larger display. Example for a 2x20 Display:<BR>
;;     Line0 -> MIOS cursor offset: 0x00 -> cursor address 0x04<BR>
;;     Line1 -> MIOS cursor offset: 0x40 -> cursor address 0x44<BR>
;;     Line2 -> don't care<BR>
;;     Line3 -> don't care<BR>
;;  IN:	  Line Y0 address in MIOS_PARAMETER1,
;;        Line Y1 address in MIOS_PARAMETER2,
;;        Line Y2 address in MIOS_PARAMETER3,
;;        Line Y3 address in WREG
;;  C_IN: Line Y0 address in <line0>
;;        Line Y1 address in <line1>
;;        Line Y2 address in <line2>
;;        Line Y3 address in <line3>
;;  OUT:  -
;;  USES: -
;;  EXAMPLE:
;; 
;;	;; configure the offsets for a 4x16 LCD:
;;	movlw	0x00
;;	movwf	MIOS_PARAMETER1
;;	movlw	0x40
;;	movwf	MIOS_PARAMETER2
;;	movlw	0x10
;;	movwf	MIOS_PARAMETER3
;;	movlw	0x50
;;	call	MIOS_LCD_YAddressSet
;;	
;;	;; configure MIOS for 2 character LCDs:
;;      ;; (see also http://www.ucapps.de/mbhp_lcd.html )
;;	movlw	0x00
;;	movwf	MIOS_PARAMETER1
;;	movlw	0x40
;;	movwf	MIOS_PARAMETER2
;;	movlw	0x80
;;	movwf	MIOS_PARAMETER3
;;	movlw	0xc0
;;	call	MIOS_LCD_YAddressSet
;;	
;;  C_EXAMPLE:
;; 
;;	// configure the offsets for a 4x16 LCD:
;;	MIOS_LCD_YAddressSet(0x00, 0x40, 0x10, 0x50);
;;	
;;	// configure MIOS for 2 character LCDs:
;;      // (see also http://www.ucapps.de/mbhp_lcd.html )
;;	MIOS_LCD_YAddressSet(0x00, 0x40, 0x80, 0xc0);
;;	
;; --------------------------------------------------------------------------
MIOS_LCD_YAddressSet
	movff	MIOS_PARAMETER1, MIOS_LCD_Y0_OFFSET
	movff	MIOS_PARAMETER2, MIOS_LCD_Y1_OFFSET
	movff	MIOS_PARAMETER3, MIOS_LCD_Y2_OFFSET
	movff	WREG, MIOS_LCD_Y3_OFFSET
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_YAddressGet
;;  C_DECLARATION: unsigned char MIOS_LCD_YAddressGet(void)
;;  DESCRIPTION: returns the Y offsets
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  Line Y0 address in MIOS_PARAMETER1,
;;        Line Y1 address in MIOS_PARAMETER2,
;;        Line Y2 address in MIOS_PARAMETER3,
;;        Line Y3 address in WREG
;;  C_OUT: Line Y0 address in MIOS_PARAMETER1 (global variable defined in cmios.h),
;;        Line Y1 address in MIOS_PARAMETER2 (global variable defined in cmios.h),
;;        Line Y2 address in MIOS_PARAMETER3 (global variable defined in cmios.h),
;;        Line Y3 address directly returned
;;  USES: -
;; --------------------------------------------------------------------------
MIOS_LCD_YAddressGet
	movff	MIOS_LCD_Y0_OFFSET, MIOS_PARAMETER1
	movff	MIOS_LCD_Y1_OFFSET, MIOS_PARAMETER2
	movff	MIOS_LCD_Y2_OFFSET, MIOS_PARAMETER3
	movff	MIOS_LCD_Y3_OFFSET, WREG
	andlw	0xff		; (update STATUS)
	return

;; --------------------------------------------------------------------------
;;  branches to the appr. GLCD/DLCD functions
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_Init
;;  C_DECLARATION: void MIOS_LCD_Init(void)
;;  DESCRIPTION: initializes the LCD display
;;  this function is called automatically after startup
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BRS, TBLPTR
;; --------------------------------------------------------------------------
MIOS_LCD_Init
	;; clear cursor pos
	SET_BSR	MIOS_LCD_CURSOR_POS
	clrf	MIOS_LCD_CURSOR_POS, BANKED
	clrf	MIOS_LCD_CURSOR_POS_REAL, BANKED

	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE2, ACCESS, MIOS_LCD_Init_4567
MIOS_LCD_Init_1234
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_LCD_Init_34
MIOS_LCD_Init_12
	BRA_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0, ACCESS, MIOS_CLCD_Init	; type 0
#if MIOS_ENABLE_GLCD0
	rgoto	MIOS_GLCD0_Init	; type 1
#else
	return
#endif

MIOS_LCD_Init_34
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return

MIOS_LCD_Init_4567
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_LCD_Init_67
MIOS_LCD_Init_45
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return

MIOS_LCD_Init_67
#if MIOS_ENABLE_MLCD
	GOTO_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0, ACCESS, MIOS_MLCD_Init
#else
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
#endif
	;; set GLCD flag by default
	bsf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD
	;; call user LCD init routine
	goto	USER_LCD_Init	; type 7



;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_PrintChar
;;  C_DECLARATION: void MIOS_LCD_PrintChar(unsigned char c)
;;  DESCRIPTION: prints a single ASCII character
;;  IN:	  character which should be print in WREG
;;  C_IN:  character which should be print in <c>
;;  OUT:  the character on LCD screen
;;  C_OUT: the character on LCD screen
;;  EXAMPLE:
;;	;; print four characters on screen
;;	movlw   'M'
;;	call    MIOS_LCD_PrintChar
;;	movlw   'I'
;;	call    MIOS_LCD_PrintChar
;;	movlw   'O'
;;	call    MIOS_LCD_PrintChar
;;	movlw   'S'
;;	call    MIOS_LCD_PrintChar
;;  C_EXAMPLE:
;;	;; print four characters on screen
;;	MIOS_LCD_PrintChar('M');
;;	MIOS_LCD_PrintChar('I');
;;	MIOS_LCD_PrintChar('O');
;;	MIOS_LCD_PrintChar('S');
;;  USES: BRS, TBLPTR
;; --------------------------------------------------------------------------
MIOS_LCD_PrintChar		; (used by mios_vectors.inc - no MLCD optimization)
	rcall	MIOS_LCD_PrintChar_Internal
	rgoto	MIOS_LCD_TransferFinished

MIOS_LCD_PrintChar_Internal	; (used by MIOS routines -- have to finish the character transfer with MIOS_LCD_TransferFinsihed!)
	SET_BSR	MIOS_LCD_TIMEOUT1
	btfsc	MIOS_LCD_TIMEOUT1, 7, BANKED; leave routine if LCD was disabled after LCD timeout
	return	

	SET_BSR	MIOS_LCD_TIMEOUT1
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE2, ACCESS, MIOS_LCD_PrintChar_4567
MIOS_LCD_PrintChar_1234
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_LCD_PrintChar_34
MIOS_LCD_PrintChar_12
	BRA_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0, ACCESS, MIOS_CLCD_PrintChar
#if MIOS_ENABLE_GLCD0
	rgoto	MIOS_GLCD0_PrintChar
#else
	return
#endif

MIOS_LCD_PrintChar_34
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return

MIOS_LCD_PrintChar_4567
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_LCD_PrintChar_67
MIOS_LCD_PrintChar_45
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return

MIOS_LCD_PrintChar_67
#if MIOS_ENABLE_MLCD
	GOTO_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0, ACCESS, MIOS_MLCD_PrintChar; type 6
#else
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
#endif
	goto	USER_LCD_PrintChar	; type 7

	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_Data
;;  C_DECLARATION: void MIOS_LCD_Data(unsigned char data)
;;  DESCRIPTION: sends a data value to the LCD display
;;  IN:	  data which should be sent in WREG
;;  C_IN:  data which should be sent in <data>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BRS, TBLPTR
;; --------------------------------------------------------------------------
MIOS_LCD_Data
	SET_BSR	MIOS_LCD_TIMEOUT1
	btfsc	MIOS_LCD_TIMEOUT1, 7, BANKED; leave routine if LCD was disabled after LCD timeout
	return	

	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE2, ACCESS, MIOS_LCD_Data_4567
MIOS_LCD_Data_1234
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_LCD_Data_34
MIOS_LCD_Data_12
	BRA_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0, ACCESS, MIOS_CLCD_Data	; type 0
#if MIOS_ENABLE_GLCD0
	rgoto	MIOS_GLCD0_Data	; type 1
#else
	return
#endif

MIOS_LCD_Data_34
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return

MIOS_LCD_Data_4567
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_LCD_Data_67
MIOS_LCD_Data_45
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return

MIOS_LCD_Data_67
	GOTO_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0, ACCESS, USER_LCD_Data ; type 7
#if MIOS_ENABLE_MLCD
	call	MIOS_MLCD_PrintChar; type 6
	goto	MIOS_MLCD_SendSyxFooter
#else
	return
#endif
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_Cmd
;;  C_DECLARATION: void MIOS_LCD_Cmd(unsigned char cmd)
;;  DESCRIPTION: sends a command to the LCD display
;;  IN:	  command which should be sent in WREG
;;  C_IN:  command which should be sent in <cmd>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BRS, TBLPTR
;; --------------------------------------------------------------------------
MIOS_LCD_Cmd
	SET_BSR	MIOS_LCD_TIMEOUT1
	btfsc	MIOS_LCD_TIMEOUT1, 7, BANKED; leave routine if LCD was disabled after LCD timeout
	return	

	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE2, ACCESS, MIOS_LCD_Cmd_4567
MIOS_LCD_Cmd_1234
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_LCD_Cmd_34
MIOS_LCD_Cmd_12
	BRA_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0, ACCESS, MIOS_CLCD_Cmd	; type 0
#if MIOS_ENABLE_GLCD0
	rgoto	MIOS_GLCD0_Cmd	; type 1
#else
	return
#endif

MIOS_LCD_Cmd_34
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return

MIOS_LCD_Cmd_4567
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_LCD_Cmd_67
MIOS_LCD_Cmd_45
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return

MIOS_LCD_Cmd_67
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	goto	USER_LCD_Cmd	; type 7
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_Clear
;;  C_DECLARATION: void MIOS_LCD_Clear(void)
;;  DESCRIPTION: clears the LCD screen
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  EXAMPLE:
;;	;; clear LCD and print a character
;;	call    MIOS_LCD_Clear
;;	movlw   'A'
;;	call    MIOS_LCD_PrintChar
;;  C_EXAMPLE:
;;	// clear LCD and print a character
;;	MIOS_LCD_Clear();
;;	MIOS_LCD_PrintChar('A');
;;  USES: BRS, TBLPTR
;; --------------------------------------------------------------------------
MIOS_LCD_Clear
	SET_BSR	MIOS_LCD_TIMEOUT1
	btfsc	MIOS_LCD_TIMEOUT1, 7, BANKED; leave routine if LCD was disabled after LCD timeout
	return	

	clrf	MIOS_LCD_CURSOR_POS, BANKED
	clrf	MIOS_LCD_CURSOR_POS_REAL, BANKED

	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE2, ACCESS, MIOS_LCD_Clear_4567
MIOS_LCD_Clear_1234
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_LCD_Clear_34
MIOS_LCD_Clear_12
	BRA_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0, ACCESS, MIOS_CLCD_Clear
#if MIOS_ENABLE_GLCD0
	rgoto	MIOS_GLCD0_Clear
#else
	return
#endif

MIOS_LCD_Clear_34
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return

MIOS_LCD_Clear_4567
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_LCD_Clear_67
MIOS_LCD_Clear_45
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return

MIOS_LCD_Clear_67
#if MIOS_ENABLE_MLCD
	GOTO_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0, ACCESS, MIOS_MLCD_Clear
#else
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
#endif
	goto	USER_LCD_Clear	; type 7

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_CursorSet
;;  C_DECLARATION: void MIOS_LCD_CursorSet(unsigned char pos)
;;  DESCRIPTION: sets the text cursor on LCD screen<BR>
;;     0x00-0x3f: first line<BR>
;;     0x40-0x7f: second line<BR>
;;     0x80-0xbf: third line or second LCD (see MIOS_LCD_YAddressSet)<BR>
;;     0xc0-0xff: fourth line or second LCD (see MIOS_LCD_YAddressSet)
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  EXAMPLE:
;;	;; print char at first line, first column
;;	movlw   0x00 + 0
;;	call    MIOS_LCD_CursorSet
;;	movlw   'A'
;;	call    MIOS_LCD_PrintChar
;;
;;	;; print char at second line, last column (2x16 LCD)
;;	movlw   0x40 + 15
;;	call    MIOS_LCD_CursorSet
;;	movlw   'B'
;;	call    MIOS_LCD_PrintChar
;;
;;	;; print chars on second LCD
;;	;; (or 3rd and 4th line, see see MIOS_LCD_YAddressSet)
;;	movlw   0x80 + 0
;;	call    MIOS_LCD_CursorSet
;;	movlw   'C'
;;	call    MIOS_LCD_PrintChar
;;
;;	movlw   0xc0 + 15
;;	call    MIOS_LCD_CursorSet
;;	movlw   'D'
;;	call    MIOS_LCD_PrintChar
;;
;;  C_EXAMPLE:
;;	// print char at first line, first column
;;	MIOS_LCD_CursorSet(0x00 + 0);
;;	MIOS_LCD_PrintChar('A');
;;
;;	// print char at second line, last column (2x16 LCD)
;;	MIOS_LCD_CursorSet(0x40 + 15);
;;	MIOS_LCD_PrintChar('B');
;;
;;	// print chars on second LCD
;;	// (or 3rd and 4th line, see see MIOS_LCD_YAddressSet)
;;	MIOS_LCD_CursorSet(0x80 + 0);
;;	MIOS_LCD_PrintChar('C');
;;	MIOS_LCD_CursorSet(0xc0 + 15);
;;	MIOS_LCD_PrintChar('D');
;;
;;  USES: BRS, TBLPTR
;; --------------------------------------------------------------------------
MIOS_LCD_CursorSet
	SET_BSR	MIOS_LCD_TIMEOUT1
	btfsc	MIOS_LCD_TIMEOUT1, 7, BANKED; leave routine if LCD was disabled after LCD timeout
	return	

	movwf	MIOS_LCD_CURSOR_POS, BANKED
	movwf	MIOS_LCD_CURSOR_POS_REAL, BANKED
	andlw	0x3f

	BRA_IFSET MIOS_LCD_CURSOR_POS, 7, BANKED, MIOS_LCD_CursorSet_Y23
MIOS_LCD_CursorSet_Y01
	BRA_IFSET MIOS_LCD_CURSOR_POS, 6, BANKED, MIOS_LCD_CursorSet_Y1
MIOS_LCD_CursorSet_Y0
	addwf	MIOS_LCD_Y0_OFFSET, W, BANKED
	rgoto	MIOS_LCD_CursorSet_Cont
MIOS_LCD_CursorSet_Y1
	addwf	MIOS_LCD_Y1_OFFSET, W, BANKED
	rgoto	MIOS_LCD_CursorSet_Cont

MIOS_LCD_CursorSet_Y23
	BRA_IFSET MIOS_LCD_CURSOR_POS, 6, BANKED, MIOS_LCD_CursorSet_Y3
MIOS_LCD_CursorSet_Y2
	addwf	MIOS_LCD_Y2_OFFSET, W, BANKED
	rgoto	MIOS_LCD_CursorSet_Cont
MIOS_LCD_CursorSet_Y3
	addwf	MIOS_LCD_Y3_OFFSET, W, BANKED
	;; 	rgoto	MIOS_LCD_CursorSet_Cont

MIOS_LCD_CursorSet_Cont
	movwf	MIOS_LCD_CURSOR_POS, BANKED

	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE2, ACCESS, MIOS_LCD_CursorSet_4567
MIOS_LCD_CursorSet_1234
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_LCD_CursorSet_34
MIOS_LCD_CursorSet_12
	BRA_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0, ACCESS, MIOS_CLCD_CursorSet
MIOS_LCD_CursorSet_34
MIOS_LCD_CursorSet_4567
	rgoto	MIOS_GLCD_CursorSet

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_CursorGet
;;  C_DECLARATION: unsigned char MIOS_LCD_CursorGet(void)
;;  DESCRIPTION: returns the text cursor position<BR>
;;     0x00-0x3f: first line<BR>
;;     0x40-0x7f: second line<BR>
;;     0x80-0xbf: third line or second LCD (if available)<BR>
;;     0xc0-0xff: fourth line or second LCD (if available)
;;  IN:	  -
;;  C_IN:	  -
;;  OUT:  text cursor position in WREG and MIOS_PARAMETER1
;;  C_OUT:  text cursor position
;;  USES: BRS, TBLPTR
;; --------------------------------------------------------------------------
MIOS_LCD_CursorGet
	movff	MIOS_LCD_CURSOR_POS_REAL, WREG
	andlw	0xff		; (update STATUS)
	return

;; ==========================================================================
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_GLCD_FontInit
;;  C_DECLARATION: void MIOS_GLCD_FontInit(code char *font)
;;  DESCRIPTION: initializes a font, (animated icon) or bitmap
;;  Note that this function works with graphical LCDs only!
;;  IN:	  pointer to font header in TBLPTR
;;  C_IN:  pointer to font header in <font>
;;  OUT:  -
;;  USES: BRS, TBLPTR
;;  EXAMPLE:
;;	;; set default font:
;;	TABLE_ADDR MIOS_GLCD_FONT
;;	call	MIOS_GLCD_FontInit
;;	;; set cursor to 0
;;	movlw	0x00
;;	call	MIOS_LCD_CursorSet
;;	;; print character 'A'
;;	movlw	'A'
;;	call	MIOS_LCD_PrintChar
;;  C_EXAMPLE:
;;	// set default font:
;;	MIOS_GLCD_FontInit(MIOS_GLCD_FONT)
;;	// set cursor to 0
;;	MIOS_LCD_CursorSet(0x00);
;;	// print character 'A'
;;	MIOS_LCD_PrintChar('A');
;; --------------------------------------------------------------------------
MIOS_GLCD_FontInit
	tblrd*+
	movff	TABLAT, MIOS_GLCD_FONT_WIDTH
	tblrd*+
	movff	TABLAT, MIOS_GLCD_FONT_HEIGHT
	tblrd*+
	movff	TABLAT, MIOS_GLCD_FONT_X0
	tblrd*+
	movff	TABLAT, MIOS_GLCD_FONT_OFFSET

	movff	TBLPTRL, MIOS_GLCD_FONT_PTRL
	movff	TBLPTRH, MIOS_GLCD_FONT_PTRH
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
	movff	TBLPTRU, MIOS_GLCD_FONT_PTRU
#endif
	return
	
;; --------------------------------------------------------------------------
;;  set graphical cursor and send it to display
;;  (compatible with MIOS_CLCD_CursorSet)
;; --------------------------------------------------------------------------
MIOS_GLCD_CursorSet
	SET_BSR	MIOS_GLCD_GCURSOR_X
	movwf	MIOS_GLCD_GCURSOR_X, BANKED

	swapf	MIOS_GLCD_GCURSOR_X, W, BANKED
	rrf	WREG, F
	rrf	WREG, W
	andlw	0x03
	movwf	MIOS_GLCD_GCURSOR_Y, BANKED
	movwf	MIOS_PARAMETER1

	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	andlw	0x3f
	mulwf	MIOS_GLCD_FONT_WIDTH, BANKED
	movf	PRODL, W
	movwf	MIOS_GLCD_GCURSOR_X, BANKED

	;; 	rgoto	MIOS_GLCD_GCursorSet

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_GLCD_GCursorSet
;;  C_DECLARATION: void MIOS_GLCD_GCursorSet(unsigned char x, unsigned char y)
;;  DESCRIPTION: sets the graphical cursor on a LCD screen<BR>
;;  Note that this function works with graphical LCDs only!
;;  IN:	  X position in WREG (0-239)
;;        Y position in MIOS_PARAMETER1 (0-7)
;;  C_IN: X position in <x> (0-239)
;;        Y position in <y> (0-7)
;;  OUT:  -
;;  USES: BRS, TBLPTR
;;  EXAMPLE:
;;	;; set graphical cursor to 160/7:
;;	movlw	7
;;	movwf	MIOS_PARAMETER1
;;	movlw	160
;;	call	MIOS_GLCD_GCursorSet
;;  C_EXAMPLE:
;;	// set graphical cursor to 160/7:
;;	MIOS_GLCD_GCursorSet(160, 7);
;; --------------------------------------------------------------------------
MIOS_GLCD_GCursorSet
	SET_BSR	MIOS_GLCD_GCURSOR_X
	movwf	MIOS_GLCD_GCURSOR_X, BANKED
	movff	MIOS_PARAMETER1, MIOS_GLCD_GCURSOR_Y

	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE2, ACCESS, MIOS_LCD_GCursorSet4567
MIOS_LCD_GCursorSet1234
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_LCD_GCursorSet34
MIOS_LCD_GCursorSet12
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
#if MIOS_ENABLE_GLCD0
	rgoto	MIOS_GLCD0_GCursorSet	; type 1
#else
	return
#endif

MIOS_LCD_GCursorSet34
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return

MIOS_LCD_GCursorSet4567
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE1, ACCESS, MIOS_LCD_GCursorSet67
MIOS_LCD_GCursorSet45
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
	return

MIOS_LCD_GCursorSet67
#if MIOS_ENABLE_MLCD
	GOTO_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0, ACCESS, MIOS_MLCD_CursorSet
#else
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_LCD_TYPE0
	return
#endif
	goto	USER_LCD_CursorSet	; type 7

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_GLCD_GCursorGet
;;  C_DECLARATION: unsigned char MIOS_GLCD_GCursorGet(void)
;;  DESCRIPTION: returns the position of the graphical cursor<BR>
;;  Note that this function works with graphical LCDs only!
;;  IN:  -
;;  C_IN:  -
;;  OUT: X position in WREG (0-239)
;;       Y position in MIOS_PARAMETER1 (0-7)
;;  C_OUT: X position returned directly
;;       Y position in MIOS_PARAMETER1 (global variable defined in cmios.h)
;;  USES: BRS, TBLPTR
;; --------------------------------------------------------------------------
MIOS_GLCD_GCursorGet
	SET_BSR	MIOS_GLCD_GCURSOR_X
	movff	MIOS_GLCD_GCURSOR_Y, MIOS_PARAMETER1
	movf	MIOS_GLCD_GCURSOR_X, W, BANKED
	andlw	0xff		; (update STATUS)
	return


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_PrintHex2
;;  C_DECLARATION: void MIOS_LCD_PrintHex2(unsigned char h)
;;  DESCRIPTION: prints a 8-bit hex value
;;  IN:   Value in WREG
;;  C_IN: Value in <h>
;;  OUT:  two digits on LCD
;;  USES: BSR
;;  EXAMPLE:
;;	;; print content of TMP1 on screen
;;	movf    TMP1, W
;;	call	MIOS_LCD_PrintHex2
;;  C_EXAMPLE:
;;	unsigned char value;
;;	
;;	// print content of 'value' on screen
;;	MIOS_LCD_PrintHex2(value);
;; --------------------------------------------------------------------------
MIOS_LCD_PrintHex2
	SET_BSR	MIOS_TMP1
	movwf	MIOS_TMP1, BANKED
	swapf	MIOS_TMP1, W, BANKED
	rcall	MIOS_LCD_PrintHex1_Internal
	movf	MIOS_TMP1, W, BANKED
	rcall	MIOS_LCD_PrintHex1_Internal
	rgoto	MIOS_LCD_TransferFinished


;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_PrintHex1
;;  C_DECLARATION: void MIOS_LCD_PrintHex1(unsigned char h)
;;  DESCRIPTION: prints a 4-bit hex value
;;  IN:   Value in WREG
;;  C_IN:   Value in <h>
;;  OUT:  one digit on LCD
;;  C_OUT:  one digit on LCD
;;  USES: BSR
;;  EXAMPLE:
;;	;; print leftmost nibble of TMP1 on screen
;;	movf    TMP1, W
;;	call	MIOS_LCD_PrintHex1
;;  C_EXAMPLE:
;;	unsigned char value;
;;	
;;	// print leftmost nibble of 'value' on screen
;;	MIOS_LCD_PrintHex1(value);
;; --------------------------------------------------------------------------
MIOS_LCD_PrintHex1
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_PrintBCD1
;;  C_DECLARATION: void MIOS_LCD_PrintBCD1(unsigned char v)
;;  DESCRIPTION: prints a 8-bit as BCD (decimal value) -- one digit only
;;  IN:   Value in WREG
;;  OUT:  one digit on LCD
;;  USES: BSR
;;  EXAMPLE:
;;	;; print leftmost digit of TMP1 on screen
;;	movf    TMP1, W
;;	call	MIOS_LCD_PrintBCD1
;;  C_EXAMPLE:
;;	unsigned char value;
;;	
;;	// print leftmost digit of 'value' on screen
;;	MIOS_LCD_PrintBCD1(value);
;; --------------------------------------------------------------------------
MIOS_LCD_PrintBCD1		; (used by mios_vectors.inc - no MLCD optimization)
	rcall	MIOS_LCD_PrintBCD1_Internal
	rgoto	MIOS_LCD_TransferFinished

MIOS_LCD_PrintHex1_Internal	; (used by MIOS routines -- have to finish the character transfer with MIOS_LCD_TransferFinsihed!)
MIOS_LCD_PrintBCD1_Internal
	andlw	0x0f
	addlw	-0x0a
	skpnc
	addlw	0x07
	addlw	0x3a
	rgoto	MIOS_LCD_PrintChar_Internal

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_PrintBCD2
;;  C_DECLARATION: void MIOS_LCD_PrintBCD2(unsigned char v)
;;  DESCRIPTION: prints a 8-bit as BCD (decimal value) -- two digits only
;;  IN:   Value in WREG
;;  C_IN: Value in <v>
;;  OUT:  two digits on LCD
;;  C_OUT: two digits on LCD
;;  USES: BSR, MIOS_PARAMETER[123]
;;  EXAMPLE:
;;	;; print two leftmost digits of TMP1 on screen
;;	movf    TMP1, W
;;	call	MIOS_LCD_PrintBCD2
;;  C_EXAMPLE:
;;	unsigned char value;
;;	
;;	// print two leftmost digits of 'value' on screen
;;	MIOS_LCD_PrintBCD2(value);
;; --------------------------------------------------------------------------
MIOS_LCD_PrintBCD2
	clrf	MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	rgoto	MIOS_LCD_PrintBCD2_Cont

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_PrintBCD3
;;  C_DECLARATION: void MIOS_LCD_PrintBCD3(unsigned char v)
;;  DESCRIPTION: prints a 8-bit as BCD (decimal value) -- all three digits
;;  IN:   Value in WREG
;;  C_IN: Value in <v>
;;  OUT:  three digits on LCD
;;  C_OUT:  three digits on LCD
;;  USES: BSR, MIOS_PARAMETER[123]
;;  EXAMPLE:
;;	;; print all three digits of TMP1 on screen
;;	movf    TMP1, W
;;	call	MIOS_LCD_PrintBCD3
;;  C_EXAMPLE:
;;	unsigned char value;
;;	
;;	// print all three digits of 'value' on screen
;;	MIOS_LCD_PrintBCD3(value);
;; --------------------------------------------------------------------------
MIOS_LCD_PrintBCD3
	clrf	MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	rgoto	MIOS_LCD_PrintBCD3_Cont

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_PrintBCD4
;;  C_DECLARATION: void MIOS_LCD_PrintBCD4(unsigned int v)
;;  DESCRIPTION: prints a 16-bit as BCD (decimal value) -- four digits
;;  IN:   Low-byte in WREG, High-byte in MIOS_PARAMETER1
;;  C_IN: 16bit value in <v>
;;  OUT:  four digits on LCD
;;  USES: BSR, MIOS_PARAMETER[123]
;;  EXAMPLE:
;;	;; print four leftmost digits of TMP[12] on screen
;;	movff   TMP2, MIOS_PARAMETER1
;;	movf    TMP1, W
;;	call	MIOS_LCD_PrintBCD4
;;  C_EXAMPLE:
;;	unsigned int value;
;;	
;;	// print four leftmost digits of 'value' on screen
;;	MIOS_LCD_PrintBCD4(value);
;; --------------------------------------------------------------------------
MIOS_LCD_PrintBCD4
	call	MIOS_HLP_Dec2BCD
	rgoto	MIOS_LCD_PrintBCD4_Cont

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_PrintBCD5
;;  C_DECLARATION: void MIOS_LCD_PrintBCD5(unsigned int v)
;;  DESCRIPTION: prints a 16-bit as BCD (decimal value) -- five digits
;;  IN:   Low-byte in WREG, High-byte in MIOS_PARAMETER1
;;  C_IN: 16bit value in <v>
;;  OUT:  five digits on LCD
;;  USES: BSR
;;  EXAMPLE:
;;	;; print all five digits of TMP[12] on screen
;;	movff   TMP2, MIOS_PARAMETER1
;;	movf    TMP1, W
;;	call	MIOS_LCD_PrintBCD5
;;  C_EXAMPLE:
;;	unsigned int value;
;;	
;;	// print all five digits of 'value' on screen
;;	MIOS_LCD_PrintBCD5(value);
;; --------------------------------------------------------------------------
MIOS_LCD_PrintBCD5
	call	MIOS_HLP_Dec2BCD
	;; 	rgoto	MIOS_LCD_PrintBCD5_Cont

;; -----------------
MIOS_LCD_PrintBCD5_Cont
	movf	MIOS_PARAMETER3, W
	andlw	0x0f
	bnz	MIOS_LCD_PrintBCD_D5

	movlw	' '
	rcall	MIOS_LCD_PrintChar_Internal

MIOS_LCD_PrintBCD4_Cont
	swapf	MIOS_PARAMETER2, W
	andlw	0x0f
	bnz	MIOS_LCD_PrintBCD_D4

	movlw	' '
	rcall	MIOS_LCD_PrintChar_Internal

MIOS_LCD_PrintBCD3_Cont
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	bnz	MIOS_LCD_PrintBCD_D3

	movlw	' '
	rcall	MIOS_LCD_PrintChar_Internal

MIOS_LCD_PrintBCD2_Cont
	swapf	MIOS_PARAMETER1, W
	andlw	0x0f
	bnz	MIOS_LCD_PrintBCD_D2

	movlw	' '
	rcall	MIOS_LCD_PrintChar_Internal

MIOS_LCD_PrintBCD1_Cont
	rgoto	MIOS_LCD_PrintBCD_D1
	

	;; ---
MIOS_LCD_PrintBCD_D5
	rcall	MIOS_LCD_PrintBCD1_Internal
MIOS_LCD_PrintBCD_D4
	swapf	MIOS_PARAMETER2, W
	rcall	MIOS_LCD_PrintBCD1_Internal
MIOS_LCD_PrintBCD_D3
	movf	MIOS_PARAMETER2, W
	rcall	MIOS_LCD_PrintBCD1_Internal
MIOS_LCD_PrintBCD_D2
	swapf	MIOS_PARAMETER1, W
	rcall	MIOS_LCD_PrintBCD1_Internal
MIOS_LCD_PrintBCD_D1
	movf	MIOS_PARAMETER1, W
	rcall	MIOS_LCD_PrintBCD1_Internal
	rgoto	MIOS_LCD_TransferFinished


;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_PrintCString
;;  C_DECLARATION: void MIOS_LCD_PrintCString(code char *str)
;;  DESCRIPTION: prints a 0-terminated string --- only provided by the MIOS C Wrapper!
;;  IN:   not available in assembler
;;  C_IN: Pointer to 0-terminated String in <str>
;;  OUT:  some characters on LCD
;;  USES: BSR
;;  C_EXAMPLE:
;;	// print string on LCD - first line, first column
;;	MIOS_LCD_CursorSet(0x00 + 0);
;;	MIOS_LCD_PrintCString("Hello World!");
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_PrintString
;;  C_DECLARATION: void MIOS_LCD_PrintString(code char *str)
;;  DESCRIPTION: prints a string
;;  IN:   Pointer to String in TBLPTR[LH]
;;        First word of string must contain length and LCD position
;;  C_IN: Pointer to String in <str>
;;        First word of string must contain length and LCD position
;;  OUT:  some characters on LCD
;;  USES: BSR
;;  EXAMPLE:
;;	;; defined somewhere *outside* the function!
;;  TEXT_WELCOME_0  STRING 12, 0x00, "Hello World!"
;;
;;	;; within the function: print string
;;	TABLE_ADDR TEXT_WELCOME_0
;;	call    MIOS_LCD_PrintString
;;  C_EXAMPLE:
;;	// defined somewhere *outside* the function!
;;  const unsigned char text_welcome_0[] = { 12, 0x00, "Hello World!" };
;;
;;	// within the function: print string
;;	MIOS_LCD_PrintString(text_welcome_0);
;;
;;	// please see also the MIOS_LCD_PrintCString function!
;; --------------------------------------------------------------------------
MIOS_LCD_PrintString
	;; get text counter and cursor pos
	tblrd*+
	movf	TABLAT, W
	rcall	MIOS_LCD_CursorSet

	tblrd*+
	movf	TABLAT, W
	;; 	rgoto	MIOS_LCD_PrintStringLoop

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_PrintPreconfString
;;  C_DECLARATION: void MIOS_LCD_PrintPreconfString(unsigned char len, code char *str)
;;  DESCRIPTION: prints a preconfigured string
;;  IN:   Pointer to String in TBLPTR[LH]
;;        Stringlength in WREG
;;  C_IN: Pointer to String in <str>
;;        Stringlength in <len>
;;  OUT:  some characters on LCD
;;  USES: BSR
;;  EXAMPLE:
;;	;; defined somewhere *outside* the function!
;;  TEXT_WELCOME_0  db "Hello World!"
;;
;;	;; within the function: print string
;;	movlw   0x00 + 0
;;	call    MIOS_LCD_CursorSet
;;	TABLE_ADDR TEXT_WELCOME_0
;;	movlw   12
;;	call    MIOS_LCD_PrintPreconfString
;;  C_EXAMPLE:
;;	// defined somewhere *outside* the function!
;;  const unsigned char text_welcome_0[] = { "Hello World!" };
;;
;;	// within the function: print string
;;	MIOS_LCD_CursorSet(0x00 + 0);
;;	MIOS_LCD_PrintPreconfString(12, text_welcome_0);
;;
;;	// please see also the MIOS_LCD_PrintCString function!
;; --------------------------------------------------------------------------
MIOS_LCD_PrintPreconfString	; (LCD_TEXT_CTR must be preconfigured)
	SET_BSR	MIOS_TMP1
	movwf	MIOS_TMP1, BANKED

MIOS_LCD_PrintStringLoop
	tblrd*+				; print out first char
	movf	TABLAT, W
	rcall	MIOS_LCD_PrintChar_Internal
	
	decfsz	MIOS_TMP1, F, BANKED	; decrement text counter, leave routine if zero
	rgoto	MIOS_LCD_PrintStringLoopNext
	tblrd*+				; dummy read
	return
MIOS_LCD_PrintStringLoopNext

	tblrd*+				; print out second char
	movf	TABLAT, W
	rcall	MIOS_LCD_PrintChar_Internal

	decfsz	MIOS_TMP1, F		; decrement text counter, leave routine if zero
	rgoto	MIOS_LCD_PrintStringLoop

	rgoto	MIOS_LCD_TransferFinished


;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_PrintMessage
;;  C_DECLARATION: void MIOS_LCD_PrintMessage(code char *str)
;;  DESCRIPTION: prints a message for about 2 seconds. Normal
;;  program flow will not be stopped during this time, just only the
;;  "DISPLAY_Tick" routine will not be called. After the message,
;;  "DISPLAY_Init" will be invoked before the next "DISPLAY_Tick"
;;  IN:   Pointer to String in TBLPTR[LH]
;;        First word of string must contain length and LCD position
;;  C_IN: Pointer to String in <str>
;;        First word of string must contain length and LCD position
;;  OUT:  some characters on LCD for 2 seconds
;;  C_OUT:  some characters on LCD for 2 seconds
;;  USES: BSR
;;  EXAMPLE:
;;	;; defined somewhere *outside* the function!
;;  TEXT_POT_VALUE_0  STRING 11, 0x00, "Pot value: "
;;
;;	;; within the function: print string
;;	TABLE_ADDR TEXT_POT_VALUE_0
;;	call    MIOS_LCD_PrintMessage
;;	movlw   0x00
;;	call    MIOS_AIN_Pin7bitGet
;;	call    MIOS_LCD_PrintHex2
;;  C_EXAMPLE:
;;	// consider the use of MIOS_LCD_PrintMessageStart
;;      // see also the appr. example
;; --------------------------------------------------------------------------
MIOS_LCD_PrintMessage
	rcall	MIOS_LCD_PrintString
	SET_BSR MIOS_MESSAGE_CTR
	movlw	0xff
	movwf	MIOS_MESSAGE_CTR, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_MessageStart
;;  C_DECLARATION: void MIOS_LCD_MessageStart(unsigned char delay)
;;  DESCRIPTION: a replacement for MIOS_LCD_PrintMessage which allows to
;;  start a message without calling "MIOS_LCD_PrintString"
;;  IN:   message delay in WREG
;;  C_IN:   message delay in <delay>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;;  EXAMPLE:
;;	;; don't invoke DISPLAY_Tick for 2 seconds
;;	movlw	255
;;	call	MIOS_LCD_MessageStart
;;  C_EXAMPLE:
;;	// print message on LCD for 2 seconds
;;	MIOS_LCD_CursorSet(0x00 + 0);
;;	MIOS_LCD_PrintCString("Pot value: ");
;;	MIOS_LCD_PrintHex2(MIOS_AIN_Pin7bitGet(0));
;;	MIOS_LCD_MessageStart(255);
;; --------------------------------------------------------------------------
MIOS_LCD_MessageStart
	SET_BSR	MIOS_MESSAGE_CTR
	movwf	MIOS_MESSAGE_CTR, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_LCD_MessageStop
;;  C_DECLARATION: void MIOS_LCD_MessageStop(void)
;;  DESCRIPTION: this function allows to stop the message immediately so
;;  that MIOS_LCD_Tick will be called again
;;  IN:   -
;;  C_IN: -
;;  OUT:  -
;;  C_OUT: -
;;  USES: BSR
;;  EXAMPLE:
;;	;; stop message immediately and invoke DISPLAY_Tick again
;;	call	MIOS_LCD_MessageStop
;;  C_EXAMPLE:
;;	// stop message immediately and invoke DISPLAY_Tick again
;;	MIOS_LCD_MessageStop();
;; --------------------------------------------------------------------------
MIOS_LCD_MessageStop
	SET_BSR	MIOS_MESSAGE_CTR
	;; exit if message counter already 0
	movf	MIOS_MESSAGE_CTR, W, BANKED
	skpnz
	return
	;; else set it to 1 for a proper reinitialization of the screen
	movlw	0x01
	movwf	MIOS_MESSAGE_CTR, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_Init
;;  C_DECLARATION: not available in C - code is part of LCD driver (app_lcd.inc)
;;  DESCRIPTION: This function is called by MIOS when the custom LCD should be initialized
;;  IN:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  OUT: -
;;  NOTE: see the custom_lcd_example for further details
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_Clear
;;  C_DECLARATION: not available in C - code is part of LCD driver (app_lcd.inc)
;;  DESCRIPTION: This function is called by MIOS when the custom LCD should be cleared
;;  IN:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;  OUT: -
;;  NOTE: see the custom_lcd_example for further details
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_CursorSet
;;  C_DECLARATION: not available in C - code is part of LCD driver (app_lcd.inc)
;;  DESCRIPTION: This function is called by MIOS when the cursor should be changed
;;  IN:  MIOS_LCD_OPTION1 - contains the first LCD option given by MIOS_LCD_TypeSet
;;       MIOS_LCD_OPTION2 - contains the second LCD option given by MIOS_LCD_TypeSet
;;       MIOS_GLCD_CURSOR_X - horizontal cursor position (for GLCDs)
;;       MIOS_GLCD_CURSOR_X - vertical cursor position   (for GLCDs)
;;       MIOS_LCD_CURSOR    - character cursor position  (for CLCDs)
;;  OUT: -
;;  NOTE: see the custom_lcd_example for further details
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_PrintChar
;;  C_DECLARATION: not available in C - code is part of LCD driver (app_lcd.inc)
;;  DESCRIPTION: This function is called by MIOS when a character should be print
;;  IN:  WREG - character
;;       all other MIOS_*LCD_* registers
;;  OUT: GLCDs should justify the X/Y cursor position
;;  NOTE: see the custom_lcd_example for further details
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_Data
;;  C_DECLARATION: not available in C - code is part of LCD driver (app_lcd.inc)
;;  DESCRIPTION: sends a data value to the LCD display.<BR>
;;  On CLCDs: branch directly to USER_LCD_PrintChar<BR>
;;  On GLCDs: ignore this function!
;;  IN:  data which should be sent
;;  OUT: -
;;  NOTE: see the custom_lcd_example for further details
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_Cmd
;;  C_DECLARATION: not available in C - code is part of LCD driver (app_lcd.inc)
;;  DESCRIPTION: sends a command to the LCD display.<BR>
;;  On CLCDs: use this function to decode the HD44780 commands if required<BR>
;;  On GLCDs: ignore this function!
;;  IN:  command which should be sent
;;  OUT: -
;;  NOTE: see the custom_lcd_example for further details
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_LCD_SpecialCharInit
;;  C_DECLARATION: not available in C - code is part of LCD driver (app_lcd.inc)
;;  DESCRIPTION: see MIOS_CLCD_SpecialCharInit
;;  IN:  number of special character (0-7) in WREG
;;       pointer to special char pattern in TBLPTR (consists of 8
;;          entries for every character-line)
;;  OUT: TBLPTR has to be set to next table entry (TBLPTR+=8)
;;  NOTE: see the custom_lcd_example for further details
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  MIOS_LCD_TransferFinished
;;  called by MIOS internal functions after a transfer of characters (i.E BCD
;;  numbers or strings) has been finished
;;  Allowes the MLCD driver to close the SysEx string properly
;;  Could also be used by similar drivers
;; --------------------------------------------------------------------------
MIOS_LCD_TransferFinished
#if MIOS_ENABLE_MLCD
	GOTO_IFSET MIOS_BOX_STAT, MIOS_BOX_STAT_MLCD_TRANSFER, ACCESS, MIOS_MLCD_SendSyxFooter
#endif
	return
