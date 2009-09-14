; $Id$
;
; MIOS Rotary Encoder Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; driver can be optionally disabled
#if MIOS_ENABLE_ENC


ENC_STAT_ACT_A          EQU 0	; Current status of channel A
ENC_STAT_ACT_B          EQU 1	; Current status of channel B
ENC_STAT_LAST_A         EQU 2	; Last status of channel A
ENC_STAT_LAST_B         EQU 3	; Last status of channel B
ENC_STAT_SPEED_SLOW     EQU 4	; 1 if encoder should be increment slower
ENC_STAT_SPEED_FAST     EQU 5	; 1 if encoder should be increment faster
ENC_STAT_DECINC         EQU 6	; 1 if last action was decrement, 0 if increment

	;; MEMO: encoder registers are saved in:
	;;   MIOS_DIN_REG0_x+0 (ENCx_STAT)
	;;   MIOS_DIN_REG0_x+1 (ENCx_PARAMETER)
	;;   MIOS_DIN_REG1_x+0 (ENCx_ACCL)
	;;   MIOS_DIN_REG1_x+1 (ENCx_INC)

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_ENC_NotifyChange
;;  C_DECLARATION: void ENC_NotifyChange(unsigned char encoder, char incrementer)
;;  DESCRIPTION: This function is called by MIOS when an encoder has been
;;  moved
;;  IN: Encoder number in WREG and MIOS_PARAMETER1
;;      signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;;  C_IN: Encoder number in <encoder>
;;      signed incrementer value in <incrementer>
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;;  OUT:  -
;;  ISR: no
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_ENC_NumberGet
;;  C_DECLARATION: unsigned char MIOS_ENC_NumberGet(void)
;;  DESCRIPTION: returns the number of encoders which have been defined in the
;;  encoder table
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  number of encoders in WREG and MIOS_PARAMETER1
;;  C_OUT:  number of encoders
;;  USES: FSR1, TBLPTR
;; --------------------------------------------------------------------------
MIOS_ENC_NumberGet
	TABLE_ADDR MIOS_ENC_PIN_TABLE
	clrf	MIOS_PARAMETER1
MIOS_ENC_NumberGetLoop
	tblrd*+						; read encoder pin from table
	incf	TABLAT, W				; end if 0xff has been specified
	bz	MIOS_ENC_NumberGet_End
	tblrd*+						; dummy read
	incf	MIOS_PARAMETER1, F
	BRA_IFCLR MIOS_PARAMETER1, 6, ACCESS, MIOS_ENC_NumberGetLoop

MIOS_ENC_NumberGet_End
	movf	MIOS_PARAMETER1, W			; return number
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_ENC_Abs7bitGet
;;  C_DECLARATION: unsigned char MIOS_ENC_Abs7bitGet(unsigned char enc)
;;  DESCRIPTION: returns absolute 7-bit value of encoder
;;  NOTE: This function uses a spare register which is only available
;;  when the encoder speed is set to MIOS_ENC_SPEED_NORMAL. In all other
;;  speed modes (MIOS_ENC_SPEED_SLOW and MIOS_ENC_SPEED_FAST) this function 
;;  should NOT be used, instead a dedicated handler for absolute values
;;  should be written in this case (see enc_example* applications)
;;  IN:	  encoder number in WREG
;;  C_IN:  encoder number in <enc>
;;  OUT:  absolute value in WREG and MIOS_PARAMETER1
;;  C_OUT: absolute value
;;  USES: BSR, FSR1, TBLPTR
;;  EXAMPLE:
;;
;;	;; get the current 7-bit value of encoder #0
;;	movlw	0x00
;;	call	MIOS_ENC_Abs7bitGet
;; 
;;  C_EXAMPLE:
;;
;;	// get the current 7-bit value of encoder #0
;;	value = MIOS_ENC_Abs7bitGet(0);
;; 
;; --------------------------------------------------------------------------
MIOS_ENC_Abs7bitGet
	rcall	MIOS_ENC_Abs7bit_PtrSet
	movf	INDF1, W
	andlw	0x7f
	movwf	MIOS_PARAMETER1
	return


	;; help function for MIOS_ENC_Abs7bit*
MIOS_ENC_Abs7bit_PtrSet
	rcall	MIOS_ENC_Hlp_GetDIN_REG0Ptr
	incf	FSR1L, F
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_ENC_Abs7bitSet
;;  C_DECLARATION: void MIOS_ENC_Abs7bitSet(unsigned char enc, unsigned char value)
;;  DESCRIPTION: sets the absolute 7-bit value of encoder
;;  NOTE: This function uses a spare register which is only available
;;  when the encoder speed is set to MIOS_ENC_SPEED_NORMAL. In all other
;;  speed modes (MIOS_ENC_SPEED_SLOW and MIOS_ENC_SPEED_FAST) this function 
;;  should NOT be used, instead a dedicated handler for absolute values
;;  should be written in this case (see enc_example* applications)
;;  IN:	  encoder number in WREG, absolute value in MIOS_PARAMETER1
;;  C_IN: encoder number in <enc>, absolute value in <value>
;;  OUT:  -
;;  USES: BSR, FSR1, TBLPTR
;;  EXAMPLE:
;;
;;	;; set the 7-bit value of encoder #0 to 0x40
;;	movlw	0x40			; store value in MIOS_PARAMETER1
;;	movwf	MIOS_PARAMETER1
;;	movlw	0x00			; set this value for encoder #0
;;	call	MIOS_ENC_Abs7bitSet
;; 
;;  C_EXAMPLE:
;;
;;	// set the 7-bit value of encoder #0 to 0x40
;;	MIOS_ENC_Abs7bitSet(0, 0x40);
;; 
;; --------------------------------------------------------------------------
MIOS_ENC_Abs7bitSet
	rcall	MIOS_ENC_Abs7bit_PtrSet
	movf	MIOS_PARAMETER1, W
	movwf	INDF1
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_ENC_Abs7bitAdd
;;  C_DECLARATION: void MIOS_ENC_Abs7bitAdd(unsigned char enc, unsigned char value)
;;  DESCRIPTION: adds the incrementer to the absolute 7-bit value of encoder<BR>
;;  This function saturates the value. That means: if the resulting value
;;  is greater than 127, it will be saturated to 127. The same will be done
;;  when the value is less than 0
;;  NOTE: This function uses a spare register which is only available
;;  when the encoder speed is set to MIOS_ENC_SPEED_NORMAL. In all other
;;  speed modes (MIOS_ENC_SPEED_SLOW and MIOS_ENC_SPEED_FAST) this function 
;;  should NOT be used, instead a dedicated handler for absolute values
;;  should be written in this case (see enc_example* applications)
;;  IN:   encoder number in WREG, 
;;	  incrementer value in MIOS_PARAMETER1
;;  C_IN: encoder number in <enc>
;;	  incrementer value in <value>
;;  OUT:  returns new absolute value in WREG and MIOS_PARAMETER1
;;        MIOS_PARAMETER2[0] is 1, if the value has been changed, 0 if it
;;           is equal to the old value
;;  C_OUT: returns new absolute value in MIOS_PARAMETER1
;;        MIOS_PARAMETER2[0] is 1, if the value has been changed, 0 if it
;;           is equal to the old value
;;  USES: BSR, FSR1, TBLPTR
;;  EXAMPLE:
;;
;;	;; subtract -5 from the 7-bit value of encoder #0
;;	movlw	-5			; store incrementer in MIOS_PARAMETER1
;;	movwf	MIOS_PARAMETER1
;;	movlw	0x00			; add this value for encoder #0
;;	call	MIOS_ENC_Abs7bitAdd
;; 
;;  C_EXAMPLE:
;;
;;	// subtract -5 from the 7-bit value of encoder #0
;;	MIOS_ENC_Abs7bitAdd(0, -5);
;; 
;; --------------------------------------------------------------------------
MIOS_ENC_Abs7bitAdd
	rcall	MIOS_ENC_Abs7bit_PtrSet
	movf	INDF1, W
	movwf	MIOS_PARAMETER2
	movf	MIOS_PARAMETER1, W
	addwf	INDF1, F
	movf	INDF1, W
	BRA_IFSET MIOS_PARAMETER1, 7, ACCESS, MIOS_ENC_Abs7bitAdd_Dec
MIOS_ENC_Abs7bitAdd_Inc
	btfsc	INDF1, 7
	movlw 0x7f
	rgoto	MIOS_ENC_Abs7bitAdd_Cont
MIOS_ENC_Abs7bitAdd_Dec
	btfsc	INDF1, 7
	movlw 0x00
	;; 	rgoto	MIOS_ENC_Abs7bitAdd_Cont
MIOS_ENC_Abs7bitAdd_Cont
	movwf	INDF1
	movwf	MIOS_PARAMETER1
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto MIOS_ENC_Abs7bitAdd_EndC
MIOS_ENC_Abs7bitAdd_EndUC
	clrf	MIOS_PARAMETER2
	rgoto	MIOS_ENC_Abs7bitAdd_End
MIOS_ENC_Abs7bitAdd_EndC
	clrf	MIOS_PARAMETER2
	bsf	MIOS_PARAMETER2, 0
MIOS_ENC_Abs7bitAdd_End
	movf	MIOS_PARAMETER1, W	; return absolute value
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_ENC_SpeedGet
;;  C_DECLARATION: unsigned char MIOS_ENC_SpeedGet(unsigned char enc)
;;  DESCRIPTION: returns the speed setting for an encoder<BR>
;;  following settings are available:<BR>
;;     MIOS_ENC_SPEED_SLOW    0x00<BR>
;;     MIOS_ENC_SPEED_NORMAL  0x01<BR>
;;     MIOS_ENC_SPEED_FAST    0x02<BR>
;;  IN:	  encoder number in WREG
;;  C_IN: encoder number in <enc>
;;  OUT:  speed mode in WREG and MIOS_PARAMETER1
;;        speed parameter in MIOS_PARAMETER2
;;  C_OUT:  returns speed mode
;;        speed parameter in MIOS_PARAMETER2
;;  USES: BSR, FSR1, TBLPTR
;;  EXAMPLE:
;;
;;	;; return the speed setting of encoder #0
;;	movlw	0x00
;;	call	MIOS_ENC_SpeedGet
;; 
;;  C_EXAMPLE:
;;
;;	// return the speed setting of encoder #0
;;	speed = MIOS_ENC_SpeedGet(0);
;; 
;; --------------------------------------------------------------------------
MIOS_ENC_SpeedGet
	rcall	MIOS_ENC_Hlp_GetDIN_REG0Ptr
	movlw	MIOS_ENC_SPEED_NORMAL
	btfsc	INDF1, ENC_STAT_SPEED_FAST
	movlw MIOS_ENC_SPEED_FAST
	btfsc	INDF1, ENC_STAT_SPEED_SLOW
	movlw MIOS_ENC_SPEED_SLOW
	movwf	MIOS_PARAMETER1
	andlw	0xff		; update STATUS
	movff	PREINC1, MIOS_PARAMETER2
	return


	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_ENC_SpeedSet
;;  C_DECLARATION: void MIOS_ENC_SpeedSet(unsigned char enc, unsigned char mode, unsigned char parameter)
;;  DESCRIPTION: sets the speed for an encoder<BR>
;;  following settings are available:<BR>
;;     MIOS_ENC_SPEED_SLOW    0x00   (requires additional parameter)<BR>
;;     MIOS_ENC_SPEED_NORMAL  0x01   (no additional parameter)<BR>
;;     MIOS_ENC_SPEED_FAST    0x02   (requires additional parameter)<BR>
;;  NOTE: When using MIOS_ENC_SPEED_SLOW or MIOS_ENC_SPEED_FAST, the 
;;  MIOS_ENC_Abs* functions are not available since these speed modes
;;  allocate a spare register which is normaly used to store the absolute value.
;;  So, in this case a dedicated handler for absolute values should be
;;  written (see enc_example* applications)
;;  IN:	  encoder number in WREG, 
;;        speed mode in MIOS_PARAMETER1
;;        speed parameter in MIOS_PARAMETER2
;;  C_IN:  encoder number in <enc>
;;        speed mode in <mode>
;;        speed parameter in <parameter>
;;  OUT:  -
;;  USES: BSR, FSR1, TBLPTR
;;  EXAMPLE:
;;
;;  MIOS_ENC_SPEED_NORMAL
;;  ~~~~~~~~~~~~~~~~~~~~~
;;	;; this speed mode requires no additional parameter
;;
;;	;; set speed of encoder #0 to "normal"
;;	movlw	MIOS_ENC_SPEED_NORMAL	; normal speed mode
;;	movwf	MIOS_PARAMETER1
;;	movlw	0x00			; rotary encoder #0
;;	call	MIOS_ENC_SpeedSet
;;  
;;  MIOS_ENC_SPEED_SLOW
;;  ~~~~~~~~~~~~~~~~~~~
;;	;; this speed mode allows to define a predivider value 
;;	;; from 0 to 7 which has to be specified in
;;	;; MIOS_PARAMETER2
;;	
;;	;; set speed for encoder #0 to "slow", use a predivider 
;;	;; of 3 so that the encoder will increment every 4th
;;	;; step
;;	movlw	0x03			; predivider value (- 1)
;;	movwf	MIOS_PARAMETER2
;;	movlw	MIOS_ENC_SPEED_SLOW	; slow speed mode
;;	movwf	MIOS_PARAMETER1
;;	movlw	0x00			; rotary encoder #0
;;	call	MIOS_ENC_SpeedSet
;; 
;;  MIOS_ENC_SPEED_FAST
;;  ~~~~~~~~~~~~~~~~~~~
;;	;; in this speed mode the increment value depends on the
;;	;; rotational speed based on the following formula:
;;
;;	;;    speed_ctr: decremented with every update cycle
;;	;;    (-> MIOS_SRIO_UpdateFrqSet)
;;	;;		 (normaly 1 ms) to measure the time between
;;	;;		 two encoder steps
;;	;;		 Init value: 0x7f
;;	;;		 reaches 0x00 after 127 update cycles 
;;	;;		(normaly after 127 ms)
;;	;;    MIOS_PARAMETER2: specified with the MIOS_ENC_SpeedSet 
;;	;;	
;;	;;		       function, allowed values:
;;	;;			  0 (fast)     -> divider = 2^(7-0) = 128
;;	;;			  1 (faster)   -> divider = 2^(7-1) =  64
;;	;;			  ...
;;	;;			  7 (fastest)  -> divider = 2^(7-7) =	1
;;	;; ->
;;	;;    incrementer = speed_ctr / (2^(7-MIOS_PARAMETER2))
;; 
;;	;; set speed for encoder #0 to "fast", speed exponent value is 2
;;	movlw	0x02			; speed exponent
;;	movwf	MIOS_PARAMETER2
;;	movlw	MIOS_ENC_SPEED_FAST	; fast speed mode
;;	movwf	MIOS_PARAMETER1
;;	movlw	0x00			; rotary encoder #0
;;	call	MIOS_ENC_SpeedSet
;; 
;;  C_EXAMPLE:
;;
;;  MIOS_ENC_SPEED_NORMAL
;;  ~~~~~~~~~~~~~~~~~~~~~
;;	// this speed mode requires no additional parameter (should be zero)
;;
;;	// set speed of encoder #0 to "normal"
;;	MIOS_ENC_SpeedSet(0, MIOS_ENC_SPEED_NORMAL, 0);
;;  
;;  MIOS_ENC_SPEED_SLOW
;;  ~~~~~~~~~~~~~~~~~~~
;;	// this speed mode allows to define a predivider value 
;;	// from 0 to 7
;;	
;;	// set speed for encoder #0 to "slow", use a predivider 
;;	// of 3 so that the encoder will increment every 4th
;;	// step (predivider value (- 1))
;;	MIOS_ENC_SpeedSet(0, MIOS_ENC_SPEED_SLOW, 3);
;; 
;;  MIOS_ENC_SPEED_FAST
;;  ~~~~~~~~~~~~~~~~~~~
;;	// in this speed mode the increment value depends on the
;;	// rotational speed based on the following formula:
;;
;;	//    speed_ctr: decremented with every update cycle
;;	//    (-> MIOS_SRIO_UpdateFrqSet)
;;	//		 (normaly 1 ms) to measure the time between
;;	//		 two encoder steps
;;	//		 Init value: 0x7f
;;	//		 reaches 0x00 after 127 update cycles 
;;	//		(normaly after 127 ms)
;;	//    <parameter>: specified with the MIOS_ENC_SpeedSet 
;;	//	
;;	//		       function, allowed values:
;;	//			  0 (fast)     -> divider = 2^(7-0) = 128
;;	//			  1 (faster)   -> divider = 2^(7-1) =  64
;;	//			  ...
;;	//			  7 (fastest)  -> divider = 2^(7-7) =	1
;;	// ->
;;	//    incrementer = speed_ctr / (2^(7-parameter))
;; 
;;	// set speed for encoder #0 to "fast", speed exponent value is 2
;;	MIOS_ENC_SpeedSet(0, MIOS_ENC_SPEED_FAST, 2);
;; 
;; --------------------------------------------------------------------------
MIOS_ENC_SpeedSet
	rcall	MIOS_ENC_Hlp_GetDIN_REG0Ptr
	BRA_IFSET MIOS_PARAMETER1, 1, ACCESS, MIOS_ENC_SpeedSet_2
	BRA_IFSET MIOS_PARAMETER1, 0, ACCESS, MIOS_ENC_SpeedSet_1
MIOS_ENC_SpeedSet_0
	bsf	INDF1, ENC_STAT_SPEED_SLOW
	bcf	INDF1, ENC_STAT_SPEED_FAST
	rgoto	MIOS_ENC_SpeedSet_Parameter
MIOS_ENC_SpeedSet_1
	bcf	INDF1, ENC_STAT_SPEED_SLOW
	bcf	INDF1, ENC_STAT_SPEED_FAST
	return
MIOS_ENC_SpeedSet_2
	bcf	INDF1, ENC_STAT_SPEED_SLOW
	bsf	INDF1, ENC_STAT_SPEED_FAST
	;; 	rgoto	MIOS_ENC_SpeedSet_Parameter
MIOS_ENC_SpeedSet_Parameter
	movff	MIOS_PARAMETER2, PREINC1
	return
		
	;; help function, returns pointer to MIOS_DIN_REG0_00 register
MIOS_ENC_Hlp_GetDIN_REG0Ptr
	clrc
	rlf	WREG, W
	andlw	0x7e
	movwf	FSR1L
	TABLE_ADDR MIOS_ENC_PIN_TABLE
	movf	FSR1L, W
	TABLE_ADD_W
	tblrd*+

	lfsr	FSR1, MIOS_DIN_REG0_00
	movf	TABLAT, W
	addwf	FSR1L, F
	return

;; --------------------------------------------------------------------------
;;  MIOS Encoders Handler: Get the status of rotary encoders,
;;  save it in ENCx_STAT Bit 0 and 1
;; --------------------------------------------------------------------------
MIOS_ENC_Tick
	;; speedup measure: if no DIN pin change, only decrement ENCx_ACCL and do nothing else!
	lfsr	FSR2, MIOS_SR_DIN_CHANGED_0
	TABLE_ADDR MIOS_ENC_PIN_TABLE
	clrf	IRQ_TMP1
MIOS_ENC_Tick_ShortCutChkLoop
	movf	POSTINC2, W
	iorwf	IRQ_TMP1, F
	movf	FSR2L, W
	andlw	0x0f
	bnz	MIOS_ENC_Tick_ShortCutChkLoop

	;; branch to common encoder loop if any "change" flag is set
	movf	IRQ_TMP1, W
	bnz	MIOS_ENC_LoopStart

	SET_BSR	MIOS_IRQ_TMP_CTR
	clrf	MIOS_IRQ_TMP_CTR, BANKED
MIOS_ENC_Tick_ShortCutLoop
	tblrd*+					; read encoder pin from table
	incf	TABLAT, W			; end if 0xff has been specified
	bz	MIOS_ENC_Tick_End

	lfsr	FSR2, MIOS_DIN_REG0_00		; select ENCx_ACCL
	movf	TABLAT, W
	movwf	FSR2L
	bsf	FSR2L, 7
	movf	INDF2, W			; check if ENCx_ACCL zero
	skpz
	decf	INDF2, F			; decrement if not zero

	tblrd*+					; switch to next entry
	incf	MIOS_IRQ_TMP_CTR, F, BANKED
	BRA_IFCLR MIOS_IRQ_TMP_CTR, 6, BANKED, MIOS_ENC_Tick_ShortCutLoop
	;; shortcut end
	rgoto	MIOS_ENC_Tick_End
	
	

	;; ----
	;; any DIN pin has been changed --- check all encoder entries
MIOS_ENC_LoopStart
	TABLE_ADDR MIOS_ENC_PIN_TABLE
	SET_BSR	MIOS_IRQ_TMP_CTR
	clrf	MIOS_IRQ_TMP_CTR, BANKED
MIOS_ENC_Loop
	tblrd*						; read encoder pin from table
	incf	TABLAT, W				; end if 0xff has been specified
	bz	MIOS_ENC_Loop_End

	;; -----------------------------------------------------
	;; TABLAT contains number of first pin
	;; copy enc registers to working registers
	lfsr	FSR2, MIOS_DIN_REG0_00
	movf	TABLAT, W
	movwf	FSR2L
	movff	INDF2, IRQ_TMP4 ; (ENCx_STAT)
	bsf	FSR2L, 7

	movf	INDF2, W			; check if ENCx_ACCL zero
	skpz
	decf	INDF2, F			; decrement if not zero
	movf	INDF2, W
	movwf	IRQ_TMP5			; save result in ENCx_ACCL

	;; get current status of pin A and B
	movf	TABLAT, W
	call	MIOS_DIN_2PinsGet
	movwf	IRQ_TMP2

	;; check if status has been changed, jump to Atomic Handler if true
	movf	IRQ_TMP4, W			; (ENCx_STAT)
	andlw	0x03
	xorwf	IRQ_TMP2, W
	bnz	MIOS_ENC_AtomicHandler
MIOS_ENC_AtomicHandler_Return

	;; copy working registers back to enc registers
	lfsr	FSR2, MIOS_DIN_REG0_00
	movff	TABLAT, FSR2L
	movff	IRQ_TMP4, INDF2			; (ENCx_STAT)
	bsf	FSR2L, 7
	movff	IRQ_TMP5, INDF2			; (ENCx_ACCL)
	;; -----------------------------------------------------

	tblrd*+						; switch to next entry
	tblrd*+
	incf	MIOS_IRQ_TMP_CTR, F, BANKED
	BRA_IFCLR MIOS_IRQ_TMP_CTR, 6, BANKED, MIOS_ENC_Loop
MIOS_ENC_Loop_End
MIOS_ENC_Tick_End
	return

; ==============================================================

MIOS_ENC_AtomicHandler
	;; copy bit ACT_[AB] to LAST_[AB]
	;; IRQ_TMP4 == ENCx_STAT
	bcf	IRQ_TMP4, ENC_STAT_LAST_A
	btfsc	IRQ_TMP4, ENC_STAT_ACT_A
	bsf	IRQ_TMP4, ENC_STAT_LAST_A
	bcf	IRQ_TMP4, ENC_STAT_LAST_B
	btfsc	IRQ_TMP4, ENC_STAT_ACT_B
	bsf	IRQ_TMP4, ENC_STAT_LAST_B

	;; copy new status to ACT_[AB]
	movlw	0xfc
	andwf	IRQ_TMP4, F
	movf	IRQ_TMP2, W
	iorwf	IRQ_TMP4, F

	;; clear CHANGED flag of encoder
	movf	TABLAT, W
	call	MIOS_DIN_ChangedSet00

	;; set FSR2 to ENCx_INC
	lfsr	FSR2, MIOS_DIN_REG1_00
	incf	TABLAT, W
	addwf	FSR2L, F

	;; read second byte of entry --- now TABLAT contains the encoder mode!
	;; MIOS_ENC_MODE_NON_DETENTED or MIOS_ENC_MODE_DETENTED[12]
	tblrd*+
	tblrd*

	; State Machine (own Design, the Machine from Microchip on AN519
        ; is too simple and doesn't work correctly!)
	; changed 2000-1-5: special "analyse" state which corrects the ENC direction
	; if encoder is rotated to fast - I should patent it ;-)

	;; ENC_STAT[3..0] = LB, LA, B, A

	;; non-detented - INC: 2, B, D, 4
	;;                DEC: 1, 7, E, 8
	;; detented1    - INC: B, 4
	;;                DEC: 7, 8
	;; detented2    - INC: B
	;;                DEC: 7
	;; detented3    - INC: 4
	;;                DEC: 8

	BRA_IFSET IRQ_TMP4, 3, ACCESS, MIOS_ENC_Do_89ABCDEF
MIOS_ENC_Do_01234567
	BRA_IFSET IRQ_TMP4, 2, ACCESS, MIOS_ENC_Do_4567
MIOS_ENC_Do_0123
	BRA_IFSET IRQ_TMP4, 1, ACCESS, MIOS_ENC_Do_23
MIOS_ENC_Do_01
	;; 0: Do_Nothing
	;; 1: Do_Dec
	BRA_IFCLR IRQ_TMP4, 0, ACCESS, MIOS_ENC_Do_Nothing
	BRA_IFSET TABLAT, 4, ACCESS, MIOS_ENC_Do_Dec
	rgoto	MIOS_ENC_Do_Nothing

MIOS_ENC_Do_23
	;; 2: Do_Inc
	;; 3: Do_Nothing
	BRA_IFSET IRQ_TMP4, 0, ACCESS, MIOS_ENC_Do_Nothing
	BRA_IFSET TABLAT, 0, ACCESS, MIOS_ENC_Do_Inc
	rgoto	MIOS_ENC_Do_Nothing

MIOS_ENC_Do_4567
	BRA_IFSET IRQ_TMP4, 1, ACCESS, MIOS_ENC_Do_67
MIOS_ENC_Do_45
	;; 4: Do_Inc
	;; 5: Do_Nothing
	BRA_IFSET IRQ_TMP4, 0, ACCESS, MIOS_ENC_Do_Nothing
	BRA_IFSET TABLAT, 3, ACCESS, MIOS_ENC_Do_Inc
	rgoto	MIOS_ENC_Do_Nothing

MIOS_ENC_Do_67
	;; 6: Do_Nothing
	;; 7: Do_Dec
	BRA_IFSET IRQ_TMP4, 0, ACCESS, MIOS_ENC_Do_Nothing
	BRA_IFSET TABLAT, 5, ACCESS, MIOS_ENC_Do_Inc
	rgoto	MIOS_ENC_Do_Nothing

MIOS_ENC_Do_89ABCDEF
	BRA_IFSET IRQ_TMP4, 2, ACCESS, MIOS_ENC_Do_CDEF
MIOS_ENC_Do_89AB
	BRA_IFSET IRQ_TMP4, 1, ACCESS, MIOS_ENC_Do_AB
MIOS_ENC_Do_89
	;; 8: Do_Dec
	;; 9: Do_Nothing
	BRA_IFSET IRQ_TMP4, 0, ACCESS, MIOS_ENC_Do_Nothing
	BRA_IFSET TABLAT, 7, ACCESS, MIOS_ENC_Do_Dec
	rgoto	MIOS_ENC_Do_Nothing

MIOS_ENC_Do_AB
	;; A: Do_Nothing
	;; B: Do_Inc
	BRA_IFCLR IRQ_TMP4, 0, ACCESS, MIOS_ENC_Do_Nothing
	BRA_IFSET TABLAT, 1, ACCESS, MIOS_ENC_Do_Inc
	rgoto	MIOS_ENC_Do_Nothing

MIOS_ENC_Do_CDEF
	BRA_IFSET IRQ_TMP4, 1, ACCESS, MIOS_ENC_Do_EF
MIOS_ENC_Do_CD
	;; C: Do_Nothing
	;; D: Do_Inc
	BRA_IFCLR IRQ_TMP4, 0, ACCESS, MIOS_ENC_Do_Nothing
	BRA_IFSET TABLAT, 2, ACCESS, MIOS_ENC_Do_Inc
	rgoto	MIOS_ENC_Do_Nothing

MIOS_ENC_Do_EF
	;; E: Do_Dec
	;; F: Do_Nothing
	BRA_IFSET IRQ_TMP4, 0, ACCESS, MIOS_ENC_Do_Nothing
	BRA_IFSET TABLAT, 6, ACCESS, MIOS_ENC_Do_Dec
	;; 	rgoto	MIOS_ENC_Do_Nothing

MIOS_ENC_Do_Nothing
MIOS_ENC_Do_Return
	tblrd*-			; switch back to first byte of entry
	tblrd*
	rgoto	MIOS_ENC_AtomicHandler_Return

;; --------------------------------------------------------------------------

MIOS_ENC_Do_Inc
	;; plausibility check: when accl > 0xe0, exit if last event was a DEC
	movlw	0xe0
	cpfsgt	IRQ_TMP5, ACCESS; (ENCx_ACCL)
	rgoto MIOS_ENC_Do_Inc_Ok	
	BRA_IFSET IRQ_TMP4, ENC_STAT_DECINC, ACCESS, MIOS_ENC_Do_Nothing
MIOS_ENC_Do_Inc_Ok
	bcf	IRQ_TMP4, ENC_STAT_DECINC	;; memorize INC

	;; branch depending on encoder mode
	BRA_IFSET IRQ_TMP4, ENC_STAT_SPEED_FAST, ACCESS, MIOS_ENC_Do_Inc_Fast
	BRA_IFSET IRQ_TMP4, ENC_STAT_SPEED_SLOW, ACCESS, MIOS_ENC_Do_Inc_Slow

	;; normal speed
MIOS_ENC_Do_Inc_Normal
	incf	INDF2, F		; (ENCx_INC)
	rgoto	MIOS_ENC_SaveState

	;; fast speed
MIOS_ENC_Do_Inc_Fast
	rcall	MIOS_ENC_ModifyAccelerator
	addwf	INDF2, F		; (ENCx_INC)        
	rgoto	MIOS_ENC_SaveState

	;; slow speed
MIOS_ENC_Do_Inc_Slow
	bcf	FSR2L, 7		; switch to ENCx_PARAMETER
	rlf	INDF2, W		; get ENCx_PARAMETER << 1
	swapf	WREG, W			; swap
	andlw	0x0f			; mask out
	movwf	IRQ_TMP3		; store in IRQ_TMP3
	incf	IRQ_TMP3, F		; increment
	movf	INDF2, W		; check if max value reached
	andlw	0x07
	cpfsgt	IRQ_TMP3, ACCESS
	rgoto MIOS_ENC_Do_Inc_Slow_NoO
	clrf	IRQ_TMP3		; reset predivider
	bsf	FSR2L, 7		; switch to ENCx_INC
	incf	INDF2, F		; increment ENCx_INC
	bcf	FSR2L, 7		; switch to ENCx_PARAMETER
MIOS_ENC_Do_Inc_Slow_NoO
	rgoto	MIOS_ENC_Do_Inc_Slow_Cont

; ------------------

MIOS_ENC_Do_Dec
	movwf	IRQ_TMP3

	;; plausibility check: when accl > 0xe0, exit if last event was a DEC
	movlw	0xe0
	cpfsgt	IRQ_TMP5, ACCESS; (ENCx_ACCL)
	rgoto MIOS_ENC_Do_Dec_Ok	
	BRA_IFCLR IRQ_TMP4, ENC_STAT_DECINC, ACCESS, MIOS_ENC_Do_Nothing
MIOS_ENC_Do_Dec_Ok
	bsf	IRQ_TMP4, ENC_STAT_DECINC	;; memorize DEC

	;; branch depending on encoder mode
	BRA_IFSET IRQ_TMP4, ENC_STAT_SPEED_FAST, ACCESS, MIOS_ENC_Do_Dec_Fast
	BRA_IFSET IRQ_TMP4, ENC_STAT_SPEED_SLOW, ACCESS, MIOS_ENC_Do_Dec_Slow

	;; normal speed
MIOS_ENC_Do_Dec_Normal
	decf	INDF2, F		; (ENCx_INC)
	rgoto	MIOS_ENC_SaveState

	;; fast speed
MIOS_ENC_Do_Dec_Fast
	rcall	MIOS_ENC_ModifyAccelerator
	subwf	INDF2, F		; (ENCx_INC)        
	rgoto	MIOS_ENC_SaveState

	;; slow speed
MIOS_ENC_Do_Dec_Slow
	bcf	FSR2L, 7		; switch to ENCx_PARAMETER
	rlf	INDF2, W		; get ENCx_PARAMETER << 1
	swapf	WREG, W			; swap
	andlw	0x0f			; mask out
	movwf	IRQ_TMP3		; store in IRQ_TMP3
	decf	IRQ_TMP3, F		; decrement
	BRA_IFCLR IRQ_TMP3, 7, ACCESS, MIOS_ENC_Do_Dec_Slow_NoO
	movf	INDF2, W
	andlw	0x07
	movwf	IRQ_TMP3
	bsf	FSR2L, 7		; switch to ENCx_INC
	decf	INDF2, F		; decrement ENCx_INC
	bcf	FSR2L, 7		; switch to ENCx_PARAMETER
MIOS_ENC_Do_Dec_Slow_NoO
MIOS_ENC_Do_Inc_Slow_Cont	; (also used by the INC routine)
	swapf	IRQ_TMP3, F		; swap
	rrf	IRQ_TMP3, W		; IRQ_TMP3 >> 1
	andlw	0x78
	movwf	IRQ_TMP3		; copy back to ENCx_PARAMETER
	movlw	0x07
	andwf	INDF2, F
	movf	IRQ_TMP3, W
	iorwf	INDF2, F	
	bsf	FSR2L, 7		; switch to ENCx_INC
	rgoto	MIOS_ENC_SaveState

; ------------------

MIOS_ENC_SaveState
	setf	IRQ_TMP5		; (ENCx_ACCL)
	rgoto	MIOS_ENC_Do_Return

;; --------------------------------------------------------------------------
	;; used when MIOS_ENC_SPEED_FAST is set
	;; determines increment/decrement value depending on accelerator counter
MIOS_ENC_ModifyAccelerator
	movff	IRQ_TMP5, IRQ_TMP3	; copy accelerator counter to IRQ_TMP3
	bcf	FSR2L, 7		; switch to ENCx_PARAMETER
	movf	INDF2, W		; get speed parameter
	bsf	FSR2L, 7		; switch back to ENCx_INC
	andlw	0x07			; branch depending on parameter
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MIOS_ENC_ModifyAccelerator0
	rgoto	MIOS_ENC_ModifyAccelerator1
	rgoto	MIOS_ENC_ModifyAccelerator2
	rgoto	MIOS_ENC_ModifyAccelerator3
	rgoto	MIOS_ENC_ModifyAccelerator4
	rgoto	MIOS_ENC_ModifyAccelerator5
	rgoto	MIOS_ENC_ModifyAccelerator6
	rgoto	MIOS_ENC_ModifyAccelerator7
MIOS_ENC_ModifyAccelerator0
	clrc
	rrf	IRQ_TMP3, F
MIOS_ENC_ModifyAccelerator1
	clrc
	rrf	IRQ_TMP3, F
MIOS_ENC_ModifyAccelerator2
	clrc
	rrf	IRQ_TMP3, F
MIOS_ENC_ModifyAccelerator3
	clrc
	rrf	IRQ_TMP3, F
MIOS_ENC_ModifyAccelerator4
	clrc
	rrf	IRQ_TMP3, F
MIOS_ENC_ModifyAccelerator5
	clrc
	rrf	IRQ_TMP3, F
MIOS_ENC_ModifyAccelerator6
	clrc
	rrf	IRQ_TMP3, F
MIOS_ENC_ModifyAccelerator7
	clrc
	rrf	IRQ_TMP3, W
	skpnz
	movlw	1
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  MIOS ENC Handler
;;  check changed Encoder pins, call USER_ENC_NotifyChange when rotary 
;;  encoder has been moved
;; --------------------------------------------------------------------------
MIOS_ENC_Handler
	SET_BSR	MIOS_TMP_CTR
	clrf	MIOS_TMP_CTR, BANKED

	TABLE_ADDR MIOS_ENC_PIN_TABLE
MIOS_ENC_Handler_Loop
	tblrd*+						; read encoder pin from table
	incf	TABLAT, W				; end if 0xff has been specified
	bz	MIOS_ENC_Handler_Loop_End

	;; -----------------------------------------------------
	;; TABLAT contains number of first pin
	;; copy ENCx_INC register to working register
	lfsr	FSR0, MIOS_DIN_REG1_00
	movf	TABLAT, W
	andlw	0x7e
	addlw	1
	addwf	FSR0L, F

	;; temporary disable the IRQs
	IRQ_DISABLE

	;; process next entry if encoder incrementer is zero
	movf	INDF0, W
	bz	MIOS_ENC_Handler_Loop_Next

	;; clear incrementer
	clrf	INDF0

	;; enable IRQs again
	IRQ_ENABLE

	;; save incrementer in MIOS_PARAMETER2
	movwf	MIOS_PARAMETER2

	;; call hook
	movf	MIOS_TMP_CTR, W, BANKED
	movwf	MIOS_PARAMETER1
	CALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, USER_ENC_NotifyChange
	SET_BSR	MIOS_TMP_CTR
	TABLE_ADDR MIOS_ENC_PIN_TABLE
	clrc
	rlf	MIOS_TMP_CTR, W, BANKED
	addlw	1
	TABLE_ADD_W

MIOS_ENC_Handler_Loop_Next
	IRQ_ENABLE
	tblrd*+			; dummy read
	incf	MIOS_TMP_CTR, F, BANKED
	BRA_IFSET MIOS_TMP_CTR, 6, BANKED, MIOS_ENC_Handler_Loop_End
	rgoto	MIOS_ENC_Handler_Loop
MIOS_ENC_Handler_Loop_End	
	return


#endif	; MIOS_ENABLE_ENC
