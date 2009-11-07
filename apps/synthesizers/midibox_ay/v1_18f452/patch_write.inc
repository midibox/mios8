;; patch_write.inc
;; -------------------[WRITE internal patch to pic eeprom or Bankstick ]------------
;; ---------------------------------------------------------------------------------
PATCH_WRITE
	;; ----------------------------------------------------------------
	;; WRITE (SAVE) PATCH TO EEPROM OR BANKSTICK
	;; ----------------------------------------------------------------
	
	;; -----------------[MAIN MENU1 (PATCH MENU (F1))?]----------------
	;; PATCH MENU = MAIN MENU 1
	;; CHECK: MAIN MENU = 1 ?
	movlw	MAIN_MENU_01
	cpfseq 	CURRENT_MAIN_MENU
			;; NO NOT MAIN MENU1
			;; Exit Here
			return
		
		;; YES MAIN MENU1
		;; GO ON...

		;; --[CHECK ACTIVE ITEM @ Main Menu 1]-------
		;; CHECK LOCATION VIA EDITING MODE "*"
		;; COPY THE VALUE OF EDITING MODE TO  MIOS_PARAMETER1
		movff 	AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO, MIOS_PARAMETER1
		movlw 	MENU_ITEM_POS3
		cpfseq 	MIOS_PARAMETER1 ;; EDITING MODE ITEM  = 3? | [W]rite [F]ormat ...
				;; NO, NOT ITEM 3
				;; EXIT HERE
				return

		;; YES, ITEM 3 IS ACTIVE (EDITING MODE)
		;; GO ON
		
	;; ---------------------------[EEPROM or BANKSTICK?]----------------------------------
	;; CHECK:
	;; WRITE to EEPROM or BANKSTICK
	;; PATCH_MENU_STATE_FLAGS
	;; BIT2 = 0 = WRITE EEPROM PATCH (PIC INTERNAL PATCH)
	;; BIT2 = 1 = WRITE BANKSTICK PATCH
	btfss	PATCH_MENU_STATE_FLAGS, BIT2_SELECTED_PATCH_BANKSTICK_OR_INTERNAL
				;; WRITE EEPROM PATCH (PIC INTERNAL PATCH)
				;; BIT2 = CLEAR
				;; USER WANT TO WRITE A PATCH TO PIC EEPROM (INTERNAL PATCH)
				;; NO FUTHER CHECKING NEEDED - GO AND WRITE THE PATCH DATA 
				;; TO PIC EEPROM
				;; ---[GO AND WRITE PATCH TO EEPROM]------------------------------
				rgoto PATCH_WRITE_DO
				;; --------------------------------------------------------------


		;; -------------[STATE: SELECTED PATCH = BANKSTICK PATCH]------------------------
		;; -------------[CHECK: BANKSTICK AVAILABLE?]------------------------------------
		;; BIT2 = SET
		;; USER SELECTED A BANKSTICK PATCH
		;; USER WANT TO WRITE A PATCH TO BANKSTICK
		;; NEXT THING TO DO:
		;; CHECK IF THE BANKSTICK IS AVAILABLE
		btfss	PATCH_MENU_STATE_FLAGS, BIT0_BANKSTICK_FOUND
					;; ---[BANKSTICK NOT AVAILABLE]----------------
					;; ---[DO: EXIT HERE]--------------------------
					;; BIT0 = 0 = NO BANKSTICK FOUND
					;; A BANKSTICK PATCH WAS SELECTED
					;; BY THE USER, BUT THERE WAS NO BANKSTICK FOUND
					;; EXIT HERE
					return
			;; ---[BANKSTICK IS AVAILABLE]----------------
			;; BIT0 = 1 = BANKSTICK IS AVAILABLE
			;; GO ON WITH:  
	;; -------------[CHECK: IS THE "DO BANKSTICK FORMAT MODE" ON?]----------------------------- 
	;; ----------------------------------------------------------------------------------------
	;; CHECK IF THE "BANKSTICK DO FORMAT" BIT = ON or OFF
	btfsc	PATCH_MENU_STATE_FLAGS, BIT3_SELECTED_PATCH_FORMAT_BANKSTICK		
			;; ------------[FORMAT MODE = ON]----------------
			;; BIT3 = 1 = THE "FORMAT BANKSTICK" BIT3 is SET
			;; --------[DO FOARMAT BANKSTICK]----------------
			;; FORMAT BANSTICK PATCH BY PATCH SUBROUTINE
			;; @ format_bankstick.inc
			goto PATCH_WRITE_DO_FORMAT_BANKSTICK

		;; ------------[FORMAT MODE = OFF]----------------
		;; BIT3 = 0 = THE "FORMAT BANKSTICK" BIT3 is CLEAR 
		;; DO NOT FORMAT THE CURREND BANKSTICK 
		;; NEXT STEP: CHECK IF BANKSTICK IS FORMATTED

;; ---------------------------------------------------------------------------------------------------
;; -----------------------------------[STATE: SELECTED PATCH = BANKSTICK PATCH]-----------------------
;; -----------------------------------[STATE: BANKSTICK is AVAILABLE          ]-----------------------
;; -----------------------------------[STATE: DO FORAMT BANKSTICK MODE is OFF ]-----------------------
;; -----------------------------------[DO   : CHECK if BANKSTICK IS FORMATTED ]----------------------- 
PATCH_WRITE_CHECK_IF_BANKSTICK_IS_FORMATTED
	;; --------------------------[BANKSTICK FORMATTED?]-------------------------------------------
	;; CHECK:
	;; IS THE BANKSTICK FORMATTED?
	btfss	PATCH_MENU_STATE_FLAGS, BIT1_BANKSTICK_SELECTED_PATCH_FORMATED
				;; ----------[NO]---------------------------
				;; BIT1 = CLEAR = BANKSTICK IS NOT FORMATTED
				;; SO EXIT HERE 
				return
		;; --------[YES]------------------------			
		;; BIT1 = 1 = BANKSTICK IS FORMATTED :-)
		;; SO WE CAN GO ON
		;; WRITE PATCH DATA TO BANKSTICK
;; ---------------------------------------------------------------------------------------------------
;; -------------[STATE: SELECTED PATCH = BANKSTICK PATCH  ]-------------------------------------------
;; -------------[STATE: BANKSTICK is AVAILABLE            ]-------------------------------------------
;; -------------[STATE: DO FORAMT BANKSTICK MODE is OFF   ]-------------------------------------------
;; -------------[STATE: BANKSTICK IS FORMATTED            ]-------------------------------------------
;; -------------[DO: WRITE DATA to PIC EEPROM or BANKSTICK]-------------------------------------------
;; ---------------------------------------------------------------------------------------------------

PATCH_WRITE_DO
;; MEMO:
;; ALSO JUMP IN POINT FROM BANKSTICK FORMAT SUBROUTINE (format_bankstick.inc)
	
	;; --------------------------------------------------------------------------
	;; -------------------------[Write Fingerprint]------------------------------
	;; --------------------------------------------------------------------------
	;; DO: WRITE the FINGERPRINT BYTE TO PIC EEPROM (INTERNAL PATCH) or BANKSTICK
	;; DO: INITIALIZE EEPROM START ADRESS or BANKSTICK PATCH START ADRESS
	call WRITE_PATCH_FINGERPRINT	;; [EEPROM/BANKSTICK PATCH-DATA BYTE1 @ ADR0]
	
	;; MEMO:
	;;
	;; EEPROM:
	;; PATCH DATA BYTE ADR 0 has been Written (FINGERPRINT BYTE) - 
	;; FINGERPRINT ADR = 0
	;;
	;; or 
	;;
	;;    BANKSTICK: 
	;;    BANKSTICK ADR [PATCH OFFSET + 0] has been written with the BANKSTICK (FINGERPRINT BYTE)
	;;    FINGERPRINT ADR  = 0x00
	;;    PATCH NUMBER ADR = 0x01
	
	
;; --------------------------------------------------------------------
;; ---------[SAVE PATCH DATA to EEPROM (PIC EEPROM INT PATCH)]---------
;;          Liste der zu speichernden MIDIBOX AY Patch variablen
;; --------------------------------------------------------------------
	;; MEMO:
	;; -----
	;; EEPROM      ADR 0 = FINGERPRINT BYTE
	;; NEXT EEPROM ADR THAT WILL BE WRITTEN           = 0x01
	;; BANKSTICK ADR 0 = FINGERPRINT BYTE
	;; BANKSTICK ADR 1 = PATCH NUMBER BYTE
	;; NEXT BANKSTICK PATCH ADR THAT WILL BE WRITTEN  = 0x02 
	
	;; WRITE:
	;; ------
	;; PATCH DATA BYTE 001
	;; [INTERNAL PATCH - ADR 0x01 @ EEPROM] [BANKSTICK PATCH ADR 0x02 @ BANKSTICK]
	movff	PATCH_NAME_ASC_0, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	;; PATCH DATA BYTE 002
	;; PATCH DATA BYTE [INTERNAL PATCH - ADR 0x02 @ EEPROM] [BANKSTICK PATCH ADR 0x03 @ BANKSTICK]
	movff	PATCH_NAME_ASC_1, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	;; PATCH DATA BYTE 003
	;; PATCH DATA BYTE [INTERNAL PATCH - ADR 0x03 @ EEPROM] [BANKSTICK PATCH ADR 0x04 @ BANKSTICK]
	movff	PATCH_NAME_ASC_2, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	;; PATCH DATA BYTE 004
	;; PATCH DATA BYTE [INTERNAL PATCH - ADR 0x04 @ EEPROM] [BANKSTICK PATCH ADR 0x05 @ BANKSTICK]
	movff	PATCH_NAME_ASC_3, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	;; PATCH DATA BYTE 005
	;; PATCH DATA BYTE [INTERNAL PATCH - ADR 0x05 @ EEPROM] [BANKSTICK PATCH ADR 0x06 @ BANKSTICK]
	movff	PATCH_NAME_ASC_4, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	;; PATCH DATA BYTE 006
	;; PATCH DATA BYTE [INTERNAL PATCH - ADR 0x06 @ EEPROM] [BANKSTICK PATCH ADR 0x07 @ BANKSTICK]
	movff	PATCH_NAME_ASC_5, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	;; PATCH DATA BYTE 007
; 	;; PATCH DATA BYTE [INTERNAL PATCH - ADR 0x07 @ EEPROM] [BANKSTICK PATCH ADR 0x08 @ BANKSTICK]
	movff	PATCH_NAME_ASC_6, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	;; PATCH DATA BYTE 008
	;; PATCH DATA BYTE [INTERNAL PATCH - ADR 0x08 @ EEPROM] [BANKSTICK PATCH ADR 0x09 @ BANKSTICK]
	movff	PATCH_NAME_ASC_7, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	;; PATCH DATA BYTE 009
	;; PATCH DATA BYTE [INTERNAL PATCH - ADR 0x09 @ EEPROM] [BANKSTICK PATCH ADR 0x0A @ BANKSTICK]
	movff AY_TONE_A_TUNE_FINE_USER, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	;; PATCH DATA BYTE 010
	;; PATCH DATA BYTE [INTERNAL PATCH - ADR 0x0A @ EEPROM] [BANKSTICK PATCH ADR 0x0B @ BANKSTICK]
	movff AY_TONE_B_TUNE_FINE_USER, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	;; PATCH DATA BYTE 011
	;; etc. ...
	movff AY_TONE_C_TUNE_FINE_USER, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	;; PATCH DATA BYTE 012 ...
	movff AY_TONE_A_TUNE_COARSE_USER, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE

	movff AY_TONE_B_TUNE_COARSE_USER, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	movff AY_TONE_C_TUNE_COARSE_USER,CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	movff AY_TONE_A_MIDI_CHANNEL_USER, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	movff AY_TONE_B_MIDI_CHANNEL_USER, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	movff AY_TONE_C_MIDI_CHANNEL_USER, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	movff AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE

	movff AY_R6_NOISE_PERIOD, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	movff AY_R8_CH_A_AMPLITUDE, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE

	movff AY_R9_CH_B_AMPLITUDE, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE

	movff AY_R10_CH_C_AMPLITUDE, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE

	movff AY_R11_ENV_PERIOD_FINE, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	movff AY_R12_ENV_PERIOD_COARSE, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE
	
	movff ENV_SHAPE_0_15_to_1_8_TRANSLATOR, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE

	movff AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE

	;; WRITE:
	;; PATCH DATA BYTE 027
	;; INTERNAL PATCH DATA BYTE   [INTERNAL EEPROM PATCH - ADR 0x1B (027 DEC)@ EEPROM   ] 
	;; BANKSTICK PATCH DATA BAYTE [BANKSTICK PATCH         ADR 0x1C (028 DEC)@ BANKSTICK]
	movff FX_USER_ON_OFF_ABC, CURREND_PATCH_DATA_BYTE_TEMP
	call 	WRITE_PATCH_DATA_BYTE


	;; -------------------------------------------------------------------------------------------
	;; -------------------------------------------------------------------------------------------

	;; -----------------------------------------------------------------------------------
	;; --------[PREPARE to STORE Fx (Fx FUNCTION, Fx DATA, Fx TIME)]----------------------
	;;    PREPARE TO WRITE ALL Fx REGS to PIC EEPROM (INTernal Patch) or BANKSTICK
	;; ------------------------------------------------------------------------------------	
	movlw SELECT_Fx_RAM_BANK		;; INDIRECT RAM ADDRESSING - SELECT RAM BANK
	movwf FSR0H				;; SET THE RAM BANK (WRITE TO IND. FILE SELECT REGISTER HIBYTE)
	clrf  Fx_LOAD_WRITE_ADR_COUNTER_TMP 	;; RESET WRITE CYCLE [0-31] COUNTER
	;; ------------------------------------------------------------------------------------
	;; ---------[END: PREPARE to SAVE Fx (Fx FUNCTION, Fx DATA, Fx TIME)]------------------
	;; ------------------------------------------------------------------------------------

	;; CHECK: 
	;; DO WRITE DATA TO EEPROM or BANKSTICK?
	;; PATCH_MENU_STATE_FLAGS
	;; BIT2 = 0 = WRITE Fx DATA to EEPROM (PIC INTERNAL PATCH)
	;; BIT2 = 1 = WRITE Fx DATA to BANKSTICK 
	btfsc	PATCH_MENU_STATE_FLAGS, BIT2_SELECTED_PATCH_BANKSTICK_OR_INTERNAL
			;; BANKSTICK
			;; BIT2 = SET	
			rgoto WRITE_ALL_FX_PATCH_DATA_TO_BANKSTICK
		
			;; INTERNAL
			;; BIT2 = CLEAR
			;; WRITE PATCH FINGERPRINT INTERNAL 
			rgoto WRITE_ALL_FX_PATCH_DATA_TO_EEPROM
			
;; -----------------------------------------------------------------------



;; -----------------------------------------------------------------------
;; -----------------------[WRITE Fx DATA TO EEPROM]-----------------------
;; -----------------------------------------------------------------------

WRITE_ALL_FX_PATCH_DATA_TO_EEPROM	
	
	;; DO LOOP 32x 
WRITE_PATCH_FX_FUNC_TO_EEPROM
	;; -------------------------------------------------------------------------------
	;; ---------------[WRITE Fx *Func* to EEPROM]-------------------------
	;; LOAD FX FUNCTION [POS 0-31] from PIC RAM
	;; Fx FUNCTIONS @ Currend Fx Pos [0-31]	
	movlw RAM_OFFSET_FX_FUNCTION 			;; PIC RAM BANK OFFSET - fx functions 
	addwf Fx_LOAD_WRITE_ADR_COUNTER_TMP, W 		;; OFFSET + (0 to 31 [32]) => Fx Tab Pos
	movwf FSR0L					;; CURREND Fx Func. ADR. (current Fx Func. Pos.)
 	movff INDF0, Fx_PATCH_DATA_TMP 			;; WRITE FX FUNCTION (currend Pos) to TMP
	;; ------------------------------------------------------------------------------
	;; WRITE Fx FUNTION @ POS [0-31] TO EEPROM
	movlw	EEPROM_OFFSET_FX_FUNCTION		;; CONST: EEPROM OFFSET (0x20)
	addwf	Fx_LOAD_WRITE_ADR_COUNTER_TMP, W	;; ADD EEPROM OFFSET + POS
	movwf   EEADR 					;; SETUP EEPROM ADRESS [OFFSET+POS] for WRITING
	movf      Fx_PATCH_DATA_TMP, W        		;; WRITE FX FUNCTION @ currend Pos TO WREG
	call      MIOS_EEPROM_Write        		;; WRITE FX FUNCTION @ currend Pos TO PIC EEPROM 
									;; (EEADR will be incremented)
	;; ------------------------------------------------------------------------------
	;; ------------------------------------------------------------------------------
		;; POS = POS +1
		incf Fx_LOAD_WRITE_ADR_COUNTER_TMP, F ;; pos = pos +1 | pos range 0 to 31 [32 pos gesamt]
		;; check if pos = 32 [range = 0 to 31]
		movlw FX_POS_MAX_PLUS1 ;; dec 31+1
		cpfseq Fx_LOAD_WRITE_ADR_COUNTER_TMP
			
			;; POS < 32
			;; Do another Fx Func WRITE CYCLE
			;; write 32x
			rgoto WRITE_PATCH_FX_FUNC_TO_EEPROM
			
			;; POS = 32
			;; ALL Fx Func. REGS HAD BEEN SAVED TO EEPROM
			;; RESET WRITE CYCLE [0-31] COUNTER
			;; go on with Write Fx DATA
			clrf Fx_LOAD_WRITE_ADR_COUNTER_TMP
	;; ---------------[END: Fx Func to EEPROM WRITING CYCLE]-------------------------
	;; ------------------------------------------------------------------------------
	

	;; ---------------[WRITE Fx DATA to EEPROM WRITING CYCLE]------------------------
WRITE_PATCH_FX_DATA_TO_EEPROM
	;; DO 32x 
	;; LOAD Fx DATA [POS 0-31] from PIC RAM
	;; Fx DATA @ Currend Fx Pos [0-31]	
	;; LOAD FX DATA
	;; Fx DATA Reg. @ Currend Fx Pos [0-31]	
	movlw RAM_OFFSET_FX_DATA 			;; PIC RAM OFFSET - to fx DATA Registers
	addwf Fx_LOAD_WRITE_ADR_COUNTER_TMP, W 		;; OFFSETT + POS (0 to 31 [32]) => Fx Tab. Pos
	movwf FSR0L					;; LOAD CURREND OFFSET IND. ADR. 
							;; for Fx DATA Registers @ current Fx Pos.
	movff INDF0, Fx_PATCH_DATA_TMP 			;; WRITE FX DATA @ currend Pos to TEMP
	;; --------------------------------------------------------------------------------------
	movlw	EEPROM_OFFSET_FX_DATA			;; LOAD EEPROM OFFSET
	addwf	Fx_LOAD_WRITE_ADR_COUNTER_TMP, W	;; EEPROM OFFSET + POS (0-31)
	movwf   EEADR 					;; SET EEPROM ADRESS [OFFSET+POS]
        movf    Fx_PATCH_DATA_TMP, W    		;; WRITE FX DATA @ currend Pos TO WREG
        call    MIOS_EEPROM_Write    			;; WRITE FX DATA @ currend Pos TO PIC EEPROM 
							;; EEADR will be incremented (AUTO INC)
	;; ------------------------------------------------------------------------------
		incf Fx_LOAD_WRITE_ADR_COUNTER_TMP, F ;; pos = pos +1 | pos range 0 to 31 [32 pos gesamt]
		
		;; check if pos = FX_POS_MAX_PLUS1	(32 =  Fx Tab Max. +1 (31+1))
		movlw 	FX_POS_MAX_PLUS1	 ;; dec 32
		cpfseq 	Fx_LOAD_WRITE_ADR_COUNTER_TMP
			
			;; Fx Tab. Pos < 32
			;; GO ON WITH AONTHER "WRITE FX DATA TO EEPROM" CYCLE
			rgoto WRITE_PATCH_FX_DATA_TO_EEPROM
			
			;; POS = 32
			;; ALL Fx DATA REGS HAD BEEN SAVED TO EEPROM
			;; RESET WRITE CYCLE [0-31] COUNTER
			;; go on with Write Fx TIME
			clrf Fx_LOAD_WRITE_ADR_COUNTER_TMP
	;; ---------------[END: Fx DATA to EEPROM WRITING CYCLE]--------------------------------

WRITE_PATCH_FX_TIME_TO_EEPROM
	;; DO 32x 
	;; ---------------[Fx PRE DELAY TIME to EEPROM WRITING CYCLE]---------------------------
	;; LOAD FX PRE DELAY TIME
	;; Fx PRE DELAY TIME Reg. @ Currend Fx Pos [0-31]	
	movlw RAM_OFFSET_FX_TIME_PRE_EXECUTION		;; PIC RAM OFFSET - to fx functions 
	addwf Fx_LOAD_WRITE_ADR_COUNTER_TMP, W 		;; OFFSET + (0 to 31 [32]) => Fx Tab Pos
	movwf FSR0L					;; LOAD CURREND OFFSET IND. ADR. 
							;; for Fx TIME Registers @ current Fx Pos.
 	movff INDF0, Fx_PATCH_DATA_TMP 			;; WRITE FX FUNCTION @ currend Pos to TEMP
	;; --------------------------------------------------------------------------------------
	movlw	EEPROM_OFFSET_FX_TIME_PRE_EXECUTION  	;; EEPROM OFFSET (!)
	addwf	Fx_LOAD_WRITE_ADR_COUNTER_TMP, W	;; OFFSET + POS
	movwf   EEADR 					;; SET EEPROM ADRESS [OFFSET+POS]
        movf    Fx_PATCH_DATA_TMP, W     		;; WRITE FX FUNCTION @ currend Pos TO WREG
        call    MIOS_EEPROM_Write        		;; WRITE FX FUNCTION @ currend Pos TO PIC EEPROM 
									;; (EEADR will be incremented)
	;; ------------------------------------------------------------------------------
	
		incf Fx_LOAD_WRITE_ADR_COUNTER_TMP, F ;; pos = pos +1 | pos range 0 to 31 [32 pos gesamt]
		
		;; check if pos = FX_POS_MAX_PLUS1	(32 =  Fx Tab Max. +1 (31+1))
		movlw 	FX_POS_MAX_PLUS1 ;; Fx Tab Pos > Fx Pos Max  ?
		cpfseq  Fx_LOAD_WRITE_ADR_COUNTER_TMP
			
			;; Fx_LOAD_WRITE_ADR_COUNTER_TMP < 32
			;; DO ANOTHER CYCLE
			;; TILL Fx_LOAD_WRITE_ADR_COUNTER_TMP = FX_POS_MAX_PLUS1
			rgoto WRITE_PATCH_FX_TIME_TO_EEPROM
			
			;; Fx_LOAD_WRITE_ADR_COUNTER_TMP = FX_POS_MAX_PLUS1
			;; go on with FINALIZING 
			rgoto WRITE_PATCH_FINISH
			
;; ---------------[END: Fx PRE DELAY TIME to EEPROM WRITING CYCLE]-----------------


;; ---------------------------------------------------------------------------
;; -----------------------[END: WRITE Fx DATA TO EEPROM]----------------------
;; ---------------------------------------------------------------------------


;; ---------------------------------------------------------------------------
;; -----------------------[WRITE Fx DATA TO BANKSTICK]---------------------------
;; ---------------------------------------------------------------------------
WRITE_ALL_FX_PATCH_DATA_TO_BANKSTICK
	
	
	;; DO LOOP 32x 
WRITE_PATCH_FX_FUNC_TO_BANKSTICK
	;; -------------------------------------------------------------------------------
	;; ---------------[WRITE Fx *Functions* to BANKSTICK]----------------------------------
	;; LOAD FX FUNCTION [POS 0-31] from PIC RAM
	;; Fx FUNCTIONS @ Currend Fx Pos [0-31]	
	movlw RAM_OFFSET_FX_FUNCTION 		;; CONST: PIC RAM BANK OFFSET - fx functions Area 
	addwf Fx_LOAD_WRITE_ADR_COUNTER_TMP, W 	;; OFFSET + (POS 0 to 31 [32]) => Fx Tab Pos
	movwf FSR0L				;; CURREND Fx Func. ADR. (current Fx Func. Pos.)
 	movff INDF0, Fx_PATCH_DATA_TMP 		;; WRITE FX FUNCTION (currend Pos) to TMP
	;; ------------------------------------------------------------------------------
	;; MEMO:
	;; HOW DOES THIS ROUTINE CHOOSE THE RIGHT BANKSTICK ADRESS?
	;; Fx_LOAD_WRITE_ADR_COUNTER_TMP WAS ALREADY CLEARD (WRITE CYCLE COUNTER [0-31])
	;; THE BANKSTICK ADRESS WAS predefined by the *WRITE_PATCH_FINGERPRINT_BANKSTICK* ROUTINE
	;; THE WRITE_PATCH_FINGERPRINT_BANKSTICK ROUTINE DO:
	;; * SET THE BANKSTICK *HIGH BYTE* ADR to the *CURREND PATCH NUMBER* -1 [[[RANGE: 1 to 128] -1] (=> 0 to 127)]
	;; * CLEAR THE BANKSTICK LOW ADR BYTE COUNTER to 0
	;; * WRITE THE FINGERPRINT BYTE @ PATCH DATA LOW BYTE ADR. 0 (THEN THE LOW ADR. BYTE = 1 (AUTO INC))
	;; * WRITE THE PATCH NUMBER BYTE @ PATCH DATA LOW BYTE ARD. 1 (THEN THE LOW ADR. BYTE = 2 (AUTO INC))
	;; * SET THE BANKSTICK_LOW_BYTE_ADR_COUNTER to 2 (not relevant here)
	;; -------------------------------------------------------------------------------
	;; -----[DO WRITE BYTE TO BANKSTICK]----------------------------------------------
	movf    Fx_PATCH_DATA_TMP, W        		;; WRITE FX FUNCTION @ currend Pos TO WREG
	call   	MIOS_BANKSTICK_Write        		;; WRITE [FX FUNCTION] @ currend Pos TO PIC BANKSTICK
							;; (BANKSTICK address will be incremented after write)

		;; -------------------------------------------------
		;; -------------------------------------------------
		;; Cycle POS = CYCLE POS +1
		incf Fx_LOAD_WRITE_ADR_COUNTER_TMP, F ;; pos = pos +1 | pos range 0 to 31 [32 BYTES]
		;; Check:
		;; Pos = 32 ?
		movlw 	FX_POS_MAX_PLUS1 ;; dec 31+1
		cpfseq 	Fx_LOAD_WRITE_ADR_COUNTER_TMP
			;; NO,
			;; Cycle POS < 32
			;; Do another RAM READ / BANKSTICK WRITE CYCLE
			;; DO 32 CYCLES
			rgoto WRITE_PATCH_FX_FUNC_TO_BANKSTICK
			
			;; YES,
			;; POS = 32
			;; ALL Fx Func. REGISTERS HAVE BEEN SAVED TO BANKSTICK PATCH
			;; RESET WRITE CYCLE [0-31] COUNTER
			;; GO ON with Write Fx DATA
			clrf Fx_LOAD_WRITE_ADR_COUNTER_TMP
			;; --------------------------------------------
	;; ---------------[END: Fx Func to EEPROM WRITING CYCLE]-------------------------
	;; ------------------------------------------------------------------------------

	;; ---------------[WRITE Fx DATA to EEPROM WRITING CYCLE]------------------------
WRITE_PATCH_FX_DATA_TO_BANKSTICK
	;; DO 32x 
	;; LOAD Fx DATA [POS 0-31] from PIC RAM
	;; Fx DATA @ Currend Fx Pos [0-31]	
	;; LOAD FX DATA
	;; Fx DATA Reg. @ Currend Fx Pos [0-31]	
	movlw RAM_OFFSET_FX_DATA 		;; PIC RAM OFFSET - to fx DATA Registers
	addwf Fx_LOAD_WRITE_ADR_COUNTER_TMP, W 	;; OFFSETT + POS (0 to 31 [32]) => Fx Tab. Pos
	movwf FSR0L				;; LOAD CURREND OFFSET IND. ADR. for Fx DATA Registers @ current Fx Pos.
	movff INDF0, Fx_PATCH_DATA_TMP 		;; WRITE FX DATA @ currend Pos to TEMP
	;; --------------------------------------------------------------------------------------
	;; -----[DO WRITE BYTE TO BANKSTICK]----------------------------------------------
	movf      Fx_PATCH_DATA_TMP, W        		;; WRITE FX FUNCTION @ currend Pos TO WREG
	;; movlw	0xCC	;; Just for testing
	call   	MIOS_BANKSTICK_Write        		;; WRITE [FX DATA] @ currend Pos TO BANKSTICK
							;; (BANKSTICK ADDRESS will be incremented after write)
							;; = ADR AUTO INC
		;; -------------------------------------------------
		;; -------------------------------------------------
		;; Cycle POS = CYCLE POS +1
		incf Fx_LOAD_WRITE_ADR_COUNTER_TMP, F ;; pos = pos +1 | pos range 0 to 31 [32]
		;; Check if pos = 32 
		movlw 	FX_POS_MAX_PLUS1 ;; dec 31+1
		cpfseq 	Fx_LOAD_WRITE_ADR_COUNTER_TMP
			;; NO,
			;; Cycle POS < 32
			;; Do another RAM READ / BANKSTICK WRITE CYCLE
			;; DO 32 RAM READ / BANKSTICK WRITE CYCLE
			rgoto WRITE_PATCH_FX_DATA_TO_BANKSTICK
			
			;; YES,
			;; POS = 32
			;; ALL Fx DATA REGS HAVE BEEN SAVED TO BANKSTICK PATCH
			;; RESET WRITE CYCLE [0-31] COUNTER
			;; Go on with Write Fx TIME
			clrf Fx_LOAD_WRITE_ADR_COUNTER_TMP
			;; --------------------------------------------
			;; --------------------------------------------
	;; ---------------[END: Fx DATA to EEPROM WRITING CYCLE]--------------------------------

WRITE_PATCH_FX_TIME_TO_BANKSTICK
	;; DO 32x 
	;; ---------------[Fx PRE DELAY TIME to EEPROM WRITING CYCLE]---------------------------
	;; LOAD FX PRE DELAY TIME
	;; Fx PRE DELAY TIME Reg. @ Currend Fx Pos [0-31]	
	movlw RAM_OFFSET_FX_TIME_PRE_EXECUTION		;; PIC RAM OFFSET - to fx functions 
	addwf Fx_LOAD_WRITE_ADR_COUNTER_TMP, W 		;; OFFSET + (0 to 31 [32]) => Fx Tab Pos
	movwf FSR0L					;; LOAD CURREND OFFSET IND. ADR. 
							;; for Fx TIME Registers @ current Fx Pos.
 	movff INDF0, Fx_PATCH_DATA_TMP 			;; WRITE FX FUNCTION @ currend Pos to TEMP
	;; --------------------------------------------------------------------------------------
	;; -----[DO WRITE BYTE TO BANKSTICK]-----------------------------------------------------
	movf      Fx_PATCH_DATA_TMP, W        		;; WRITE FX FUNCTION @ currend Pos TO WREG
	;; movlw	0xDD	;; Just for testing
	call   	MIOS_BANKSTICK_Write        		;; WRITE [FX PRE EXECUTION TIME] @ currend Pos TO BANKSTICK
							;; (BANKSTICK address will be incremented after write)
							;; = ADR AUTO INC
		;; ------------------------------------------------------------------------------
		;; ------------------------------------------------------------------------------
		;; Cycle POS = CYCLE POS +1
		incf Fx_LOAD_WRITE_ADR_COUNTER_TMP, F ;; pos = pos +1 | pos range 0 to 31 [32]
		;; Check if pos = 32 
		movlw 	FX_POS_MAX_PLUS1 ;; dec 31+1
		cpfseq 	Fx_LOAD_WRITE_ADR_COUNTER_TMP
			;; NO,
			;; Cycle POS < 32
			;; Do another RAM READ / BANKSTICK WRITE CYCLE
			;; DO 32 RAM READ / BANKSTICK WRITE CYCLE
			rgoto WRITE_PATCH_FX_TIME_TO_BANKSTICK
			
			;; YES,
			;; POS = 32
			;; ALL Fx TIME REGS HAVE BEEN SAVED TO BANKSTICK PATCH
			;; RESET WRITE CYCLE [0-31] COUNTER
			clrf Fx_LOAD_WRITE_ADR_COUNTER_TMP
	
	;; END OF WRITING Fx DATA TO BANKSTICK
	rgoto WRITE_PATCH_FINISH

;; ---------------------------------------------------------------------------
;; ---------------------------------------------------------------------------
;; -----------------------[END: WRITE Fx DATA TO BANKSTICK]-------------------
;; ---------------------------------------------------------------------------


WRITE_PATCH_FINISH
;; --------------------------------------------------------------------------------------
	;; ------------[STATE: A PATCH HAD BEEN WRITTEN TO BANKSTICK or PIC EEPROM]------
	;; ------------[       OR A BANKSTICK PATCH HAD BEEN FORMATTED            ]------
	;; ------------------------------------------------------------------------------
	;; ------------------------------------------------------------------------------
	;; 
	;; CHECK: 
	;; IS THE "DO FORMAT BANKSTICK" BIT = ON or OFF
	;; [MEMO:]-----------------------------------------------------------------------
	;; WE DO NEED TO KNOW whether a FORMATTING PROCESS IS RUNNING OR NOT
	;; IF THE BANKSTICK FORMATTING PROCESS IS ACTIVE LET'S FORMAT ANOTHER
	;; BANKSTICK PATCH TILL ALL BS PATCHES ARE FORMATTED - BUT IF THE USER STORED A 
	;; PATCH TO INTERNAL EEPROM or BANKSTICK THE "DO FORMAT BANKSTICK" BIT = OFF
	;; IN THIS CASE WE CAN GO ON WITH A
	;; LCD "OK" MESSAGE on LCD ROW4
	btfss	PATCH_MENU_STATE_FLAGS, BIT3_SELECTED_PATCH_FORMAT_BANKSTICK
			;; ------------[FORMAT MODE = OFF]----------------------
			;; BIT3 = 0 = THE "FORMAT BANKSTICK" BIT3 is CLEAR 
			;; DO NOT FORMAT THE CURREND BANKSTICK 
			;; DO WRITE GO AND WRITE "OK" on LCD 
			rgoto WRITE_PATCH_FINISH_WRITE_ON_LCD		

			;; ------------[FORMAT MODE = ON]------------------------
			;; BIT3 = SET: THE "FORMAT BANKSTICK" MODE is ON
			;; ----------[DO ANOTHER FORRMAT BANKSTICK CYCLE]--------
			;; ------------------------------------------------------
			;; MEMO:
			;; THIS IS THE JUMP OUT POINT FROM BANKSTICK FORMAT SUBROUTINE 
			;; (format_bankstick.inc)
			;; ------------------------------------------------------
				;; --[DO: WAIT CYCLES for BANKSTICK FORMATTING]--
				;; THIS DELAY IS NEEDED FOR WRITING ALL 
				;; PATCH DATA TO THE BANKSTICK (127 times).
				;; WITHOUT THIS DELAY MIOS CRASH AND REBOOT (!)
				;; ----[ STATE: FORMAT ANOTHER BANKSTICK PATCH]--
				;; ----[ BUT DO Wait for 120 ms               ]--
				movlw	120
				call    MIOS_Delay
				;; ----------------------------------------------
			;; GO and format another BANKSTICK PATCH
			rgoto PATCH_WRITE_DO_FORMAT_BANKSTICK
			;; -------------------------------------------------------
		
WRITE_PATCH_FINISH_WRITE_ON_LCD	
		;; "W: OK" INFO ON LCD ROW4:
		;; WRITING PATCH DATA WAS SUCCSESSFULL! :-)
		TABLE_ADDR LCD_CLEAR_ROW4
		call    MIOS_LCD_PrintString
		TABLE_ADDR TEXT_AY_MB_AY_PATCH_WRITE_OK
		call    MIOS_LCD_PrintString
				
				;; All Done, so exit WTITE PACH subroutines here 
				return


;; -----------------------------------------------------------------------------------------
;; -------------------------[END OF WRITING PATCH DATA ]------------------------------------
;; -----------------------------------------------------------------------------------------
;; -------------------------[END SAVE Fx PATCH DATA    ]------------------------------------
;; -----------------------------------------------------------------------------------------



;; #########################################################################################
;; -----------------------------------------------------------------------------------------
;; -------------------------------[:: SUBROUTINES ::]---------------------------------------
;; -----------------------------------------------------------------------------------------
;; #########################################################################################
;; ---------------------------------------------------------------------------
;; ---------------------------------------------------------------------------
;; -----------------------[WRITE DATA BYTE TO EEPROM OR BANKSTICK]------------
;; ---------------------------------------------------------------------------
WRITE_PATCH_DATA_BYTE
	;; CHECK:
	;; PATCH_MENU_STATE_FLAGS
	;; BIT2 = 0 = WRITE EEPROM PATCH (PIC INTERNAL PATCH)
	;; BIT2 = 1 = WRITE BANKSTICK PATCH
	btfsc	PATCH_MENU_STATE_FLAGS, BIT2_SELECTED_PATCH_BANKSTICK_OR_INTERNAL
			;; ===[WRITE DATA BYTE TO BANKSTICK]=====
			;; BIT2 = SET	
			rgoto WRITE_PATCH_DATA_BYTE_TO_BANKSTICK
				
		;; ===[WRITE DATA BYTE TO INTERNAL EEPROM]=======
		;; BIT2 = CLEAR
		;; WRITE A BYTE to INTERNAL PATCH (PIC EEPROM)
		movf 	CURREND_PATCH_DATA_BYTE_TEMP, W
		call    MIOS_EEPROM_Write
		;; BYTE HAS BEEN WRITTEN TO EEPROM
		;; EEPROM ADR = EEPROM ADR +1
			;; EXIT THIS SUBROUTINE
			return
		;; ==============================================

WRITE_PATCH_DATA_BYTE_TO_BANKSTICK
			;; ===================[WRITE DATA BYTE TO BANKSTICK]================
			;; store LOW-byte of the BANKSTICK Address in MIOS_PARAMETER1
			movff    BANKSTICK_LOW_BYTE_ADR_COUNTER, MIOS_PARAMETER1
      			;; store HIGH-byte of THE BANKSTICK Address in MIOS_PARAMETER2
			movff   BROWSE_PATCH_NR, MIOS_PARAMETER2
				;; -----------------------------------------------------------------
				;; SET THE -1 OFFSET TO THE PATCH NR SELECTOR
				decf	MIOS_PARAMETER2, F	;; ADR (HI BYTE) BS PATCH = CURREND PATCH NR -1
			;; -----------------------------------------------------------------
			;; STORE THE CURREND PATCH DATA BYTE IN WREG
			;; movlw 0xAA	;; JUST FOR TESTING
			movf	CURREND_PATCH_DATA_BYTE_TEMP, W
			;; WRITE THE CURRENT DATA BYTE (WREG) TO BANKSTICK
			call    MIOS_BANKSTICK_Write        		
			;; bnz   BSProgrammingFailed       		; branch to your exception handler
			;; BYTE HAS BEEN WRITTEN TO BAKSTICK
			;; --------------------------------------------------------------------
			;; BANKSTICK ADR = BANKSTICK ADR +1 (LOW BYTE)
			;; WE CAN NOT USE BANKSTICK ADR AUTO INC +1
			;; SO WE DO IT THIS WAY :-)
			incf	BANKSTICK_LOW_BYTE_ADR_COUNTER, F
			;; --------------------------------------------------------------------  
				;; NOW THE STORE BYTE in BANKSTICK ROUTINE IS DONE
				;; SO LET'S EXIT HERE
				return
			;; =====================================================================
;; ---------------------------------------------------------------------------------------------
;; -----------------------[END: WRITE DATA BYTE TO EEPROM OR BANKSTICK]-------------------------
;; ---------------------------------------------------------------------------------------------

;; ----------------------------------------------------------------------------------------
;; ----------------------------------[START: Write PATCH Fingerprint]----------------------
;; ----------------------------------------------------------------------------------------
WRITE_PATCH_FINGERPRINT
	;; WRITE FINGERPRINT to EEPROM or BANKSTICK?
	;; -----------------------------------------
	;; CHECK:
	;; PATCH_MENU_STATE_FLAGS
	;; BIT2 = 0 = WRITE EEPROM PATCH (PIC INTERNAL PATCH)
	;; BIT2 = 1 = WRITE BANKSTICK PATCH
	btfsc	PATCH_MENU_STATE_FLAGS, BIT2_SELECTED_PATCH_BANKSTICK_OR_INTERNAL
			;; BANKSTICK
			;; BIT2 = SET	
			rgoto WRITE_PATCH_FINGERPRINT_BANKSTICK
				
		;; =========[WRITE INTERNAL EEPROM PATCH FINGERPRINT]===
		;; PATCH_MENU_STATE_FLAGS BIT2 = CLEAR
		;; PATCH = INTERNAL (PIC EEPROM)
		;; WRITE FINGERPRINT to INTERNAL PATCH
		;; ==========[WRITE INTERNAL EEPROM FINGERPRINT]========
		;; INITIALIZE EEPROM ADRESS and WRITE FINGERPRINT
		; CONST: ADRESS (=0) @ EEPROM ADDRESS REG
		movlw       EEPROM_ADR_FINGERPRINT
		movwf       EEADR
		; CONST: [the "INTERNAL PATCH WAS WRITTEN" Fingerprint]
		movlw       PIC_INT_EEPROM_FINGERPRINT
		; WRITE FINGERPRINT TO EEPROM (EEADR will be incremented)
		call        MIOS_EEPROM_Write
		;; ----------[END: WRITING FINGERPRINT TO EEPROM]--------
		
		;; Exit SUB ROUTINE
		return
		;; ======================================================


WRITE_PATCH_FINGERPRINT_BANKSTICK
		;; ==================[WRITE BANKSTICK PATCH FINGERPRINT]=====
		;; ----------------------------------------------------------------------------
		;; 1) INITIALIZE BANKSTICK ADRESS 
		;; 2) WRITE FINGERPRINT TO BANKSTICK PATCH [1-127] @ CURREND PATCH ADR0
		;; 3) WRITE BROWSE_PATCH_NR TO BANKSTICK PATCH [1-127] @ CURREND PATCH ADR1
		;; 4) SET BANKSTICK_LOW_BYTE_ADR_COUNTER to 2 (NOW CURREND PATCH ADR = 2 @ CURREND PATCH)
			;; ----------------------------------------------------------------------------
			;; -----------[SET BANKSTICK ADR LOWBYTE]------------------
			;; Store LOW-byte (FINGERPRINT) BANKSTICK ADRESS in MIOS_PARAMETER1
			;; (LOW BYTE) = PATCH [1-128] BANSKSTICK ADRESS0 [+ HIGHBYTE OFFSET] 
			;; SET LOW BYTE BANKSTICK ADRRESS to 0
			clrf        MIOS_PARAMETER1
       			;; -----------[SET BANKSTICK ADR HIGHBYTE]-------------------------------------
       			; Store HIGH-byte BANKSTICK ADRESS in MIOS_PARAMETER2
       			; FINGERPRINT (HIGH BYTE) = BROWSE_PATCH_NR = [1 to 128]
       			movff       BROWSE_PATCH_NR, MIOS_PARAMETER2
       			;; -1 OFFSET
       				;; ---------------------------------------------------------------------
       				;; MEMO - HIGH BYTE ADR OFFSET:
       				;; EVERY BANKSTICK PATCH NEED A HIGH BYTE OFFSET of -1
       				;; SO THAT PATCH Nr 1   => HIGH BYTE = 0
       				;; SO THAT PATCH Nr 2   => HIGH BYTE = 1
       				;; SO THAT PATCH Nr 3   => HIGH BYTE = 2
       				;; ...
       				;; SO THAT PATCH Nr 127 => HIGH BYTE = 126
       				;; THE HIGHEST BANKSTICK ADR = 7FFF 
       				;; => [7F(ADR.HIGH_BYTE)FF(ADR.LOW_BYTE)]
       				;; THERE WOULD BE NO ADR SPACE for A PATCH
       				;; STARTING WITH A HIGH BYTE ADR @ 127 (!)
       				;; SO THE LAST VALID HIGH BYTE ADR = 126 
       				;; ----------------------------------------------------------------------
        		decf        MIOS_PARAMETER2, F 
        		;; ---------------------------------------------------------------------
        		;; WRITE BANKSTICK FINGERPRINT TO PATCH [1-127] FINGERPRINT BYTE 
        		;; @ CURREND PATCH ADR 0x00
        		movlw        BANKSTICK_PATCH_FINGERPRINT
        		;; ... TO BANKSTICK
        		call         MIOS_BANKSTICK_Write        ; (address will be incremented
        		                                         ;  after write)
        		;; bnz        BSProgrammingFailed        ; branch to your exception handler
			;; ----------------------------------------------------------------------
			;; WRITE BANKSTICK PATCH NUMBER TO PATCH [1-127] 
			;; @ CURREND PATCH ADR 0x01
        		movf      BROWSE_PATCH_NR, W
        		;; ... TO BANKSTICK
        		call      MIOS_BANKSTICK_Write       	 ; (address will be incremented
        		                                         ;  after write)
        		;; bnz        BSProgrammingFailed        ; branch to your exception handler
			;; ----------------------------------------------------------------------
			
			;; AFTER WRITING THE PATCH THE CURRENT PATCH IS THE browser Patch 
			movff       BROWSE_PATCH_NR, CURREND_PATCH_NR
			
			;; ------------[RESET: BANKSTICK_LOW_BYTE_ADR_COUNTER]-------------------
			;; THE FINGERPRINT HAS BEEN WRITTEN TO PATCH [VALID PATCH NR. RANGE 1 to 127] 
			;; ALL THE OTHER PATCH DATA WILL FOLOW NOW - BUT FIRST
			;; RESET BANKSTICK LOW BYTE COUNTER TO ADR2
			;; SEE WRITE_PATCH_DATA_BYTE_TO_BANKSTICK for futher backgrund
			movlw 	0x02
			movwf	BANKSTICK_LOW_BYTE_ADR_COUNTER
				;; JUMP BACK
				return
			;; ===================================================
;; ---------------------------------------------------------------------------
;; ---------------------[END: Write PATCH Fingerprint]------------------------
;; ---------------------------------------------------------------------------

;; #########################################################################################
;; -----------------------------------------------------------------------------------------
;; ---------------------------[:: END SUBROUTINES ::]---------------------------------------
;; -----------------------------------------------------------------------------------------
;; #########################################################################################

