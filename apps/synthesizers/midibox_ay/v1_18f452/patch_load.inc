;; patch_load.inc

;; ----------------------------------------------------------------------------------------
;; -------------[FUNCTION: LOAD PATCH DATA from PIC Eeprom or BANKSTICK to PIC RAM]--------
;; ----------------------------------------------------------------------------------------
;;                    LOAD A PATCH FROM EEPROM or BANKSTICK into PIC RAM
;; ----------------------------------------------------------------------------------------


PATCH_LOAD
	;; CHECH MAIN MENU = 1 ?
	movlw 	MAIN_MENU_01	
	cpfseq 	CURRENT_MAIN_MENU
		;; NO 
		;; NOT MAIN MENU1 (PATCH MENU)
		;; SO EXIT HERE
		return
		
		;; YES, MENU = PATCH MENU
		;; MAIN MENU = MAIN MENU1
	;; --[CHECK ACTIVE ITEM @ Main Menu 1]-------
	;; CHECK LOCATION VIA EDITING MODE "*"
	movff 	AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO, MIOS_PARAMETER1
	movlw 	MENU_ITEM_POS3
	cpfseq 	MIOS_PARAMETER1 ;; EDITING MODE ITEM  = 3?
		;; NO, NOT ITEM 3
		;; LOAD PATCH OPTION IS ONLY VALID VIA ITEM 3
		;; SO EXIT HERE
		return

		;; YES, ITEM 3 IS ACTIVE [ITEM = [L]oad and [W]rite [F]ormat...
		;; ------------[EEPROM or BANKSTICK?]--------------------------
		;; CHECK:
		;; DO WE HAVE TO LOAD DATA from EEPROM or BANKSTICK
		;; CHECK VIA PATCH_MENU_STATE_FLAGS
		;; BIT2 = 0 = LOAD EEPROM PATCH (PIC INTERNAL PATCH)
		;; BIT2 = 1 = LOAD A BANKSTICK PATCH
		btfss	PATCH_MENU_STATE_FLAGS, BIT2_SELECTED_PATCH_BANKSTICK_OR_INTERNAL
				;; LOAD EEPROM PATCH (PIC INTERNAL PATCH)
				;; BIT2 = CLEAR = 0
				;; USER WANT TO LOAD A PATCH TO PIC RAM from EEPROM (INTERNAL PATCH)
				;; NO FUTHER CHECKING NEEDED - GO AND LOAD THE PATCH DATA 
				;; TO PIC RAM FROM PIC EEPROM
				;; -------[STATE: USER WAND TO LOAD A PATCH               ]--------
				;; -------[STATE: SELECTED PATCH = INTERNAL (EEPROM) PATCH]--------
				;; ----------------------------------------------------------------
				;; FIRST THING TO DO (IF A PATCH IS TO BE LOADED INTO PIC RAM)
				;; IS TO CHECK THE PATCH DATA FINGERPRINT @ PATCH ARD 0
				rgoto CHECK_PATCH_FINGERPRINT
		
		;; -------------[STATE: USER WAND TO LOAD A PATCH       ]------------------------
		;; -------------[STATE: SELECTED PATCH = BANKSTICK PATCH]------------------------
		;; -------------[CHECK: BANKSTICK AVAILABLE?]------------------------------------
		;; BIT2 = SET
		;; USER SELECTED A *BANKSTICK* PATCH
		;; USER WANT TO LOAD A PATCH from BANKSTICK
		;; NEXT THING TO DO:
		;; CHECK IF THE BANKSTICK IS AVAILABLE
		btfss	PATCH_MENU_STATE_FLAGS, BIT0_BANKSTICK_FOUND
					;; ---[STAE: BANKSTICK NOT AVAILABLE]----------------
					;; ---[DO: EXIT HERE                ]----------------
					;; BIT0 = 0 = NO BANKSTICK FOUND
					;; A BANKSTICK PATCH WAS SELECTED
					;; BY THE USER, BUT THERE WAS NO BANKSTICK FOUND
					;; EXIT HERE
					return
					
	;; BIT0 = 1 = BANKSTICK IS AVAILABLE			
	;; -------------[STATE: USER WAND TO LOAD A PATCH         ]---------------------------
	;; -------------[STATE: SELECTED PATCH = BANKSTICK PATCH  ]---------------------------
	;; -------------[STATE: BANKSTICK IS AVAILABLE            ]---------------------------
	;; CHECK:
	;; IS THE BANKSTICK FORMATTED?
	btfss	PATCH_MENU_STATE_FLAGS, BIT1_BANKSTICK_SELECTED_PATCH_FORMATED
				;; BIT1 = CLEAR = BANKSTICK IS *NOT* FORMATTED
				;; ---------------[STATE: BANKSTICK NOT FORMATTED]------------------------
				;; SO THERE IS NO PATCH DATA TO LOAD
				;; EXIT HERE 
				return
		
		;; BIT1 = SET = BANKSTICK IS FORMATTED :-)
		;; -------------[STATE: USER WAND TO LOAD A PATCH         ]---------------------------
		;; -------------[STATE: SELECTED PATCH = BANKSTICK PATCH  ]---------------------------
		;; -------------[STATE: BANKSTICK IS AVAILABLE            ]---------------------------		
		;; -------------[STATE: BANKSTICK IS FORMATTED            ]---------------------------
		;; SO WE CAN GO ON WITH PATCH LOAD from BANKSTICK 
		;; FIRST WE HAVE TO CHECK THE BANKSTICK PATCH FINGERPRINT
		
CHECK_PATCH_FINGERPRINT
	;; LOAD FINGERPRINT FROM EEPROM or BANKSTICK?
	;; ------------------------------------------
	;; CHECK:
	;; PATCH_MENU_STATE_FLAGS
	;; BIT2 = 0 = LOAD EEPROM PATCH (PIC INTERNAL PATCH)
	;; BIT2 = 1 = LOAD BANKSTICK PATCH
	btfsc	PATCH_MENU_STATE_FLAGS, BIT2_SELECTED_PATCH_BANKSTICK_OR_INTERNAL
			;; BANKSTICK
			;; BIT2 = SET	
			;; ============[GO AND LOAD BANKSTICK PATCH FINGERPRINT]==========
			rgoto LOAD_PATCH_FINGERPRINT_BANKSTICK
				
		;; ================[LOAD INTERNAL (EEPROM) PATCH FINGERPRINT]=============
		;; PATCH_MENU_STATE_FLAGS BIT2 = CLEAR
		;; PATCH = INTERNAL (PIC EEPROM)
		;; LOAD FINGERPRINT from INTERNAL PATCH (PIC EEPROM)
		;; -------------------------------------------------------------------	
		;; CHECK THE FINGERPRINT @ EEPROM ADR 0
		;; DATA MUST BE = PIC_INT_EEPROM_FINGERPRINT (0x12)
		;; ELSE THE PATCH DATA IS NOT VALID
		movlw   EEPROM_ADR_FINGERPRINT		;; ADR 0
       		movwf   EEADR				;; MOVE ADR 0 TO EEPROM ARD REG
        	call    MIOS_EEPROM_Read   		;; read Byte from EEPROM and INC EEADR +1 (AUTO INC ADR)
        	movwf	MIOS_PARAMETER1			;; MOVE DATA from EEPROM ADR0 to MIOS_PARAMETER1
        	;; MEMO:
		;; PIC_INT_EEPROM_FINGERPRINT = CONST 
        	;; DATA BYTE from PIC int EEPROM 
		;; @ ADR0 = PIC_INT_EEPROM_FINGERPRINT?
		movlw 	PIC_INT_EEPROM_FINGERPRINT	
        	cpfseq	MIOS_PARAMETER1
        		;; NO,
			;; THERE IS NO VALID FINGERPRINT @ PIC EEPROM ADR0 (INTERNAL PATCH)
			;; Fingerprint < > PIC_INT_EEPROM_FINGERPRINT
        		rgoto PATCH_DATA_LOADING_Failed
        		
        	;; YES
		;; Fingerprint = PIC_INT_EEPROM_FINGERPRINT
		;; GO ON AND LOAD PATCH FROM PIC EEPROM (INTERNAL PATCH)
		rgoto	PATCH_LOAD_DO
		;; ============[END: LOAD INTERNAL (PIC EEPROM) PATCH FINGERPRINT]=================

LOAD_PATCH_FINGERPRINT_BANKSTICK
	;; =======================[LOAD BANKSTICK PATCH FINGERPRINT]===============================
	;; MEMO: BANKSTICK DATA STRUCTURE
	;; BANKSTICK ADRESS GOT A HIGH and a LOW BYTE [16 BIT ADRESS]
	;; [HIGH BYTE ADR] [LOW BYTE ADR]
	;; [  0000 0000      0000 0000  ] 16 BIT BANKSTICK ADR
	;; THE [HIGH BYTE ADR] = [PATCH NUMBER (-1)] | RANGE: 0 to 127 | = PATCH NUMBER POINTER
	;; THE [LOW  BYTE ADR] = PATCH DATA AREA     | RANGE: 0 to 255 | = PATCH DATA POINTER
	;; EVERY SINGLE BANKSTICK PATCH GOT A OWN 255 BYTES DATA AREA. THE [LOW BYTE ADR] IS THE POINTER
	;; TO EVERY SINGLE DATA BYTE OF A SINGLE PATCH. IN OTHER WORDS THE [LOW BYTE] IS THE PATCH
	;; DATA POINTER. THE [HIGH BYTE ADR] DO DEFINE THE PATCH NUMBER (0 to 127). IN OTHER WORDS
	;; THE [HIGH BYTE ADR] IS THE POINTER THE BANKSTICK PATCH NUMBER.
			;; -----------[SET BANKSTICK LOWBYTE ADR]------------------------------------------
			;; Store LOW-BYTE (FINGERPRINT) BANKSTICK ADRESS in MIOS_PARAMETER1 
			;; SET LOW BYTE BANKSTICK ADRRESS to 0 ( = FINGERPRINT ADR)
			clrf        MIOS_PARAMETER1
       			;; -----------[SET BANKSTICK ADR HIGHBYTE]-------------------------------------
       			;; Store HIGH-byte BANKSTICK ADRESS in MIOS_PARAMETER2
       			;; FINGERPRINT (HIGH BYTE) = PATCH NR = [1 to 127]
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
        		;; LOAD BANKSTICK FINGERPRINT  
        		;; @ CURREND PATCH ADR 0x00
        		call    MIOS_BANKSTICK_Read        ; load FINGERPRINT-BYTE from BankStick
        		movwf	MIOS_PARAMETER1	
                        ;; branch to error exception
                        ;; routine if neccesary
        		;; BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, PATCH_DATA_LOADING_Failed
			
				;; FINGERPRINT OK ?
        			;; MUST BE CONST. BANKSTICK_PATCH_FINGERPRINT
				movlw 	BANKSTICK_PATCH_FINGERPRINT
        			cpfseq	MIOS_PARAMETER1
        				;; NO,
					;; Fingerprint < > BANKSTICK_PATCH_FINGERPRINT
        				;; ERROR !
					rgoto PATCH_DATA_LOADING_Failed
		
        		
				;; YES
				;; Fingerprint = BANKSTICK_PATCH_FINGERPRINT
			;; ----------------------------------------------------------------------
			;; ---------[STATE: BANKSTICK FINGERPRINT  = OK]-------------------------
			;; ----------------------------------------------------------------------
			
			;; ---------[SET: BANKSTICK_LOW_BYTE_ADR_COUNTER]------------------------
			;; THE FINGERPRINT IS  @ BANKSTICK ADR0 
			;; THE PATCH NUMBER IS @ BANKSTICK ADR1
			;; ALL OTHER PATCH DATA WILL FOLOW NOW @ ADR2
			;; SO SET BANKSTICK LOW BYTE COUNTER TO  ADR2
			movlw 	0x02
			movwf	BANKSTICK_LOW_BYTE_ADR_COUNTER
		
	;; ============[STATE: BANKSTICK PATCH FINGERPRINT OK]=====
	;; --------------------------------------------------------
	;; GO AND LOAD BANKSTICK PATCH DATA BYTES...

PATCH_LOAD_DO
	;; --------------------------------------------------------------
	;; DISABLE IRQ ROUTINES
	IRQ_DISABLE
	;; --------------------------------------------------------------
	
	;; --------------------------------------------------------------
	;; -------------[READ PATCH NAME]--------------------------------
	;; --------------------------------------------------------------

	;; LOAD DATA BYTE @ EEPROM ADR 0x01 | @ BANKSTICK PATCH DATA BYTE ADR 0x02
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, PATCH_NAME_ASC_0
	
	
	
	;; LOAD DATA BYTE @ EEPROM ADR 0x02 | @ BANKSTICK PATCH DATA BYTE ADR 0x03
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, PATCH_NAME_ASC_1


	
	;; LOAD DATA BYTE @ EEPROM ADR 0x03 | @ BANKSTICK PATCH DATA BYTE ADR 0x04
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, PATCH_NAME_ASC_2


	;; @ EEPROM ADR 0x04 
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, PATCH_NAME_ASC_3

	;; @ EEPROM ADR 0x05
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, PATCH_NAME_ASC_4

	;; @ EEPROM ADR 0x06
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, PATCH_NAME_ASC_5

	;; @ EEPROM ADR 0x07
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, PATCH_NAME_ASC_6

	
	;; LOAD DATA BYTE @ EEPROM ADR 0x08 | @ BANKSTICK PATCH DATA BYTE ADR 0x09
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, PATCH_NAME_ASC_7

	
	;; -----------------------------------------------------------
	;; -------------[READ MIDIBOX AY OTHER PATCH DATA]------------
	;; -----------------------------------------------------------
	
	;; @ EEPROM ADR 0x09
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_TONE_A_TUNE_FINE_USER
	
	;; @ EEPROM ADR 0x0A
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_TONE_B_TUNE_FINE_USER
	

	;; @ EEPROM ADR 0x0B
	call    LOAD_PATCH_DATA_BYTE	
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_TONE_C_TUNE_FINE_USER
	
	
	;; @ EEPROM ADR 0x0C
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_TONE_A_TUNE_COARSE_USER
	
	
	;; @ EEPROM ADR 0x0D
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_TONE_B_TUNE_COARSE_USER
	
	
	;; @ EEPROM ADR 0x0E
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_TONE_C_TUNE_COARSE_USER
	
	
	;; @ EEPROM ADR 0x0F
	call    LOAD_PATCH_DATA_BYTE	
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_TONE_A_MIDI_CHANNEL_USER
	
	
	;; @ EEPROM ADR 0x10
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_TONE_B_MIDI_CHANNEL_USER
	
	
	;; @ EEPROM ADR 0x11
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_TONE_C_MIDI_CHANNEL_USER
	
	
	;; @ EEPROM ADR 0x12
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF
	

	;; @ EEPROM ADR 0x13
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_R6_NOISE_PERIOD
	

	;; @ EEPROM ADR 0x14
	call    LOAD_PATCH_DATA_BYTE	
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_R8_CH_A_AMPLITUDE
	
	;; ----------------[ CHK: TONE A ENV ON?  ]-------------------	
	;; CS LED ==> TONE A ENVELOPE MODE (BIT4)	
	;; IS "ENVELOPE MODE" (BIT4) ON OR OFF?
        btfss AY_R8_CH_A_AMPLITUDE, BIT4_ENVELOPE_ON_OFF
        	;; BIT 4 is CLEAR - ENV OFF
		rgoto EEPROM_PATCH_LOAD_LED_CS_ENV_A_OFF
		
		;; BIT 4 is SET - ENV ON
        	;; WRITE PATCH DATA TO LED CS
		call LED_EnvelopeModeA_on
		rgoto EEPROM_PATCH_LOAD_R9

			
EEPROM_PATCH_LOAD_LED_CS_ENV_A_OFF
		;; WRITE PATCH DATA TO LED CS
		call LED_EnvelopeModeA_off
		;; GO ON WITH EEPROM_PATCH_LOAD_R9
			
	;; ----------------[END]-------------------------------------	

EEPROM_PATCH_LOAD_R9
	;; @ EEPROM ADR 0x15
	call    LOAD_PATCH_DATA_BYTE	
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_R9_CH_B_AMPLITUDE
	
	;; ----------------[CHK: TONE B ENV ON?  ]-------------------	
	;; CS LED ==> TONE B ENVELOPE MODE (BIT4)	
	;; IS "ENVELOPE MODE" (BIT4) ON OR OFF?
        btfss AY_R9_CH_B_AMPLITUDE, 4
        	;; BIT 4 is CLEAR
		rgoto EEPROM_PATCH_LOAD_LED_CS_ENV_B_OFF

		;; BIT 4 is SET
        	;; CS LED ENV @ TONE B = ON
		call LED_EnvelopeModeB_on
		rgoto EEPROM_PATCH_LOAD_R10


EEPROM_PATCH_LOAD_LED_CS_ENV_B_OFF
		;; CS LED ENV @ TONE B = OFF
		call LED_EnvelopeModeB_off
		;; GO ON WITH NEXT REG LOAD
	;; ------------------[END]-------------------------------------
	
EEPROM_PATCH_LOAD_R10
	;; @ EEPROM ADR 0x16
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_R10_CH_C_AMPLITUDE
	;; ----------------[ CHK: TONE C ENV ON?  ]-------------------	
	;; CS LED ==> TONE C ENVELOPE MODE (BIT4)	
	;; IS "ENVELOPE MODE" (BIT4) ON OR OFF?
        btfss AY_R10_CH_C_AMPLITUDE, 4
        	;; BIT 4 is CLEAR
		rgoto EEPROM_PATCH_LOAD_LED_CS_ENV_C_OFF
		
		;; BIT 4 is SET
        	;; CS LED ENV @ TONE C = ONN
		call LED_EnvelopeModeC_on
		rgoto EEPROM_PATCH_LOAD_R11

;; CS LED ENV @ TONE B = OFF
EEPROM_PATCH_LOAD_LED_CS_ENV_C_OFF
		call LED_EnvelopeModeC_off
		;; GO ON WITH NEXT REG LOAD
	;; ----------------[ END ]-----------------------------------
		
EEPROM_PATCH_LOAD_R11
	;; @ EEPROM ADR 0x17
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_R11_ENV_PERIOD_FINE
		;; PREPARE ITEM DATA FOR TRANSFERE TO AY CHIP VIA ay_sr.inc
		;; --------------------------------------------------------
		;; WRITE AY REG *ADRESS* TO AY_REG_ADDRESS FOR => AY_SR_Write
		movlw AY_CHIP_ADR_11_GLOBAL_ENVELOPE_FINE ; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 		; ADDRESS FOR => AY_SR_Write
		;; WRITE AY REG *DATA* TO AY_REG_DATA FOR => AY_SR_Write
		movff AY_R11_ENV_PERIOD_FINE, AY_REG_DATA 
			;; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
			;; @ay_sr.inc
			call LABEL_AY_SR_Write
		;; --------------------------------------------------------
	
	;; @ EEPROM ADR 0x18
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_R12_ENV_PERIOD_COARSE

		;; PREPARE ITEM DATA FOR TRANSFERE TO AY CHIP VIA ay_sr.inc
		;; --------------------------------------------------------
		;; WRITE AY REG *ADRESS* TO AY_REG_ADDRESS FOR => AY_SR_Write
		movlw AY_CHIP_ADR_12_GLOBAL_ENVELOPE_COARSE ; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 		; ADDRESS FOR => AY_SR_Write
		;; WRITE AY REG *DATA* TO AY_REG_DATA FOR => AY_SR_Write
		movff AY_R12_ENV_PERIOD_COARSE, AY_REG_DATA 
	
			;; WRITING 8BIT DATA 
			;; (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
			call LABEL_AY_SR_Write
		;; --------------------------------------------------------
	
	;; -------------[WRITE ENV SHAPE 1-8]--------------------------
	;; @ EEPROM ADR 0x19
	call    LOAD_PATCH_DATA_BYTE
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, ENV_SHAPE_0_15_to_1_8_TRANSLATOR
		;; FUNCTION:
		;; 1) TRANSLATE ENV SHAPE 1-8 (ENV SHAPES) to 0-15 (AY CHIP REG) 
		;; 2) Write Value to AY_R13_ENV_SHAPE_CYCLE (but do not write the AY_R13_ENV_SHAPE_CYCLE
		;;    Value to AY Chip, this would cause a immediate ENVELOPE-Action, this is up 
		;;    to MIDI Note On)
		;; 3) Set ENV CS LED
		call ENVELOPE_TRANSLATOR_CALL
	;; -------------[END: WRITE ENV SHAPE 1-8]---------------------


	;; @ EEPROM ADR 0x1A
	;; ----------------[ START @ EEPROM ADR 0x1A ]------------------
	;; -------------[LOAD TONE NOISE ON OFF SWITCH]-----------------
	call    LOAD_PATCH_DATA_BYTE	
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH
		;; WRITE TO CS LED
		;; IS BIT 0 CLEAR? [(=> TONE A) = 0 (=> ON)]?
		btfsc AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, BIT0_TONE_A_ON_OFF
        	
        		;; NO - BIT0 IS SET
        		;; BIT0 = SET => SO TONE A IS *OFF* 
        		;; (DONT BE CONFUSED HAVE A LOOK IN THE DATASHEET OF THE AY CHIP ;-)
        		rgoto PATCH_LOAD_CS_TONE_A_OFF
		
			;; YES - BIT0 *IS* = 0
			;; BIT0 = CLEAR => SO TONE A *IS* ON 
			call LED_ToneA_on
			;; GO ON WITH NEXT BIT (TONE B ON / OFF)
			rgoto PATCH_LOAD_CHK_TONE_B_ON_OFF


PATCH_LOAD_CS_TONE_A_OFF
		;; MB AY CS:
		call LED_ToneA_off
		;; GO ON WITH NEXT BIT (TONE B ON / OFF)


PATCH_LOAD_CHK_TONE_B_ON_OFF	
	;; IS BIT 1 = CLEAR? 
	;; [(TONE B BIT1) = CLEAR = (TONE B = ON)]?
        btfsc AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, BIT1_TONE_B_ON_OFF
        		;; NO - BIT1 IS IS = SET
        		;; BIT1 = 1 => SO TONE B IS *OFF* 
        		;; (DONT BE CONFUSED HAVE A LOOK IN THE DATASHEET OF THE AY CHIP ;-)
        		rgoto PATCH_LOAD_CS_TONE_B_OFF
		
			;; YES - BIT1 *IS* = 0
			;; BIT1 = 0 => SO TONE B *IS* ON 
			call LED_ToneB_on
			;; CHK NEXT BIT (TONE C)
			rgoto PATCH_LOAD_CHK_TONE_C

PATCH_LOAD_CS_TONE_B_OFF
			call LED_ToneB_off
			;; CHK NEXT BIT (TONE C)

PATCH_LOAD_CHK_TONE_C
	;; IS BIT 2 = CLEAR? [(=> TONE C) = 0 (=> ON)]?
        btfsc AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, BIT2_TONE_C_ON_OFF
        		;; NO - BIT2 IS IS = SET
        		;; BIT2 = 1 => SO TONE C IS *OFF*
        		rgoto PATCH_LOAD_CS_TONE_C_OFF
		
			;; YES - BIT2 *IS* = 0
			;; BIT2 = 0 => SO TONE C *IS* ON 
			call LED_ToneC_on
			;; GO AND CHK NOISE TONE A
			rgoto PATCH_LOAD_CHK_NOISE_A

PATCH_LOAD_CS_TONE_C_OFF
			call LED_ToneC_off
			;; GO AND CHK NOISE TONE A


PATCH_LOAD_CHK_NOISE_A
	;; IS BIT 3 = CLEAR [(=> NOISE A) = 0 (=> ON)]?
        btfsc AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, BIT3_NOISE_A_ON_OFF
        		;; NO - BIT3 IS IS = SET
        		;; BIT3 = 1 => SO NOISE A IS *OFF*
        		rgoto PATCH_LOAD_CS_NOISE_A_OFF
		
			;; YES - BIT3 *IS* = CLEAR
			;; BIT3 = 0 => SO NOISE A *IS* ON 
			call LED_NoiseA_on
			;; GO AND CHK NOISE TONE B
			rgoto PATCH_LOAD_CHK_NOISE_B

PATCH_LOAD_CS_NOISE_A_OFF
			call LED_NoiseA_off
			;; GO AND CHK NOISE TONE B

PATCH_LOAD_CHK_NOISE_B
	;; IS BIT 4 = CLEAR [(=> NOISE B) = 0 (=> ON)]?
        btfsc AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, BIT4_NOISE_B_ON_OFF
        		;; NO - BIT4 IS IS = SET
        		;; BIT4 = 1 => SO NOISE B IS *OFF*
        		rgoto PATCH_LOAD_CS_NOISE_B_OFF
		
			;; YES - BIT4 *IS* = 0
			;; BIT4 = 0 => SO NOISE B *IS* ON 
			call LED_NoiseB_on
			;; GO AND CHK NOISE TONE C
			rgoto PATCH_LOAD_CHK_NOISE_C

PATCH_LOAD_CS_NOISE_B_OFF
			call LED_NoiseB_off
			;; GO AND CHK NOISE TONE C

PATCH_LOAD_CHK_NOISE_C
	;; IS BIT 5 = CLEAR? [(=> NOISE C) = 0 (=> ON)]?
        btfsc AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, BIT5_NOISE_C_ON_OFF
        	
        		;; NO - BIT5 IS IS = 1
        		;; BIT5 = 1 => SO NOISE C IS *OFF*
        		rgoto EEPROM_PATCH_LOAD_CS_NOISE_C_OFF
		
			;; YES - BIT5 *IS* = 0
			;; BIT5 = 0 => SO NOISE C *IS* ON 
			call LED_NoiseC_on
			;; GO AND CHK IF USR Fx
			rgoto PATCH_LOAD_FX_USER_ON_OFF

EEPROM_PATCH_LOAD_CS_NOISE_C_OFF
			call LED_NoiseC_off
			;; GO AND CHK IF USR Fx
;; ---------------------[END: LOAD TONE NOISE ON OFF SWITCH]----------------


PATCH_LOAD_FX_USER_ON_OFF
	;; LOAD:
	;; PATCH DATA BYTE 027
	;; INTERNAL PATCH DATA BYTE   [INTERNAL EEPROM PATCH - ADR 0x1B (027 DEC)@ EEPROM   ] 
	;; BANKSTICK PATCH DATA BAYTE [BANKSTICK PATCH         ADR 0x1C (028 DEC)@ BANKSTICK]
	call    LOAD_PATCH_DATA_BYTE	
	movff 	CURREND_PATCH_DATA_BYTE_TEMP, FX_USER_ON_OFF_ABC
	
	;; USER: FX ABC off (See MAIN MENU 3 / Mixer)
	;; BIT0 = A (SET = ON / CLR = OFF)
	;; BIT1 = B (SET = ON / CLR = OFF)
	;; BIT2 = C (SET = ON / CLR = OFF)
	
	;; WRITE TO CS LED
	;; ------------[Fx @ TONE A]-------------------
	;; IS BIT 0 = SET? [(=> Fx A) = 1 (=> ON)]?
        btfss FX_USER_ON_OFF_ABC, BIT0_FX_ON_OFF_TONE_A
	
        		;; BIT0 = CLEAR = 0
        		;; Fx TONE A IS *NOT* ENABLED 
        		rgoto PATCH_LOAD_LED_CS_FX_A_OFF
		
			;; BIT0 = SET = 1
			;; Fx @ TONE A *IS* ON 
			call LED_Fx_A_on
			;; GO AND CHECK FX @ TONE B
			rgoto PATCH_LOAD_CHK_FX_B

PATCH_LOAD_LED_CS_FX_A_OFF
			call LED_Fx_A_off
			;; GO AND CHECK FX @ TONE B

	;; ------------[Fx @ TONE B]-------------------
			
PATCH_LOAD_CHK_FX_B
	;; IS BIT 1 (=> Fx B) = 1 (=> ON)?
        btfss FX_USER_ON_OFF_ABC, BIT1_FX_ON_OFF_TONE_B
        		;; BIT1 = CLEAR = 0
        		;; Fx B IS OFF 
        		rgoto PATCH_LOAD_LED_CS_FX_B_OFF
		
			;; BIT1 *IS* = 1
			;; Fx B *IS* ON 
			call LED_Fx_B_on
			;; GO AND CHK USER Fx TONE C 
			rgoto PATCH_LOAD_CHK_FX_C

PATCH_LOAD_LED_CS_FX_B_OFF
			call LED_Fx_B_off
			;; GO AND CHK USER Fx TONE C 

	;; ------------[Fx @ TONE C]-------------------
PATCH_LOAD_CHK_FX_C
	;; IS BIT 2 = SET [(=> Fx C) = 1 (=> ON)]?
        btfss FX_USER_ON_OFF_ABC, BIT2_FX_ON_OFF_TONE_C
        		;; BIT2 = CLEAR = 0
        		;; Fx C IS OFF 
        		rgoto PATCH_LOAD_LED_CS_FX_C_OFF
		
			;; BIT2 *IS* = 1
			;; Fx C *IS* ON 
			call LED_Fx_C_on
			;; GO AND LOAD PATCH FX DATA
			rgoto DO_LOAD_CURREND_PATCH_FX_DATA


PATCH_LOAD_LED_CS_FX_C_OFF
			call LED_Fx_C_off
			;; GO AND LOAD PATCH FX DATA

;; ----------------[END: USER FX OPTION ON/OFF]-------------------
;; ---------------------------------------------------------------

;; ---------------------------------------------------------------
;; ----------------[START: LOAD FX DATA]--------------------------			
DO_LOAD_CURREND_PATCH_FX_DATA
;; ---------------------------[LOAD Fx DATA]----------------------

	;; ------------[PREPEAR Fx DATA LOADING]----------------
	;; MEMO: Load THE Fx DATA from EEPROM to PIC Fx RAM AREA
	movlw SELECT_Fx_RAM_BANK		;; INDIRECT RAM ADDRESSING:
	movwf FSR0H				;; SET THE Fx RAM BANK 
	clrf  Fx_LOAD_WRITE_ADR_COUNTER_TMP	;; reset READ CYCLE pos. to 0


	;; CHECK: LOAD DATA from EEPROM or BANKSTICK?
	;; -------------------------------------------------------
	;; PATCH_MENU_STATE_FLAGS
	;; BIT2 = 0 = LOAD Fx DATA from EEPROM (PIC INTERNAL PATCH)
	;; BIT2 = 1 = LOAD Fx DATA from BANKSTICK 
	btfsc	PATCH_MENU_STATE_FLAGS, BIT2_SELECTED_PATCH_BANKSTICK_OR_INTERNAL
			;; LOAD Fx DATA FROM BANKSTICK
			;; BIT2 = SET	
			rgoto BANKSTICK_PATCH_LOAD_FX_FUNC_FROM_BANKSTICK
		
		;; LOAD FX DATA From INTERNAL PATCH (EEPROM)
		;; BIT2 = CLEAR
	;; -----------[LOAD Fx Function (Pos 0-31)]---------------------------------
EEPROM_PATCH_LOAD_FX_FUNC_FROM_EEPROM
	;; LOAD Fx FUNCTION BYTES CYCLE - DO 32x
	;; LOAD FX FUNCTION @ Currend Fx Pos from PIC EEPROM to PIC RAM
	;; -----------------------------------------------------------------
 	movlw		EEPROM_OFFSET_FX_FUNCTION		;; Fx Functions AREA - EEPROM OFFSET (!)
	addwf		Fx_LOAD_WRITE_ADR_COUNTER_TMP, W	;; OFFSET + POS
	movwf        	EEADR 			;; SET EEPROM ADRESS [OFFSET+POS]
        call        	MIOS_EEPROM_Read       	;; READ FX FUNCTION @ currend Pos of PIC EEPROM (EEADR will be incremented)
 	movwf        	Fx_PATCH_DATA_TMP       ;; WRITE FX FUNCTION @ currend Pos TO TMP
 	
 	;; ------[ WRITE FX FUNCTION @ Currend Fx Pos ==> to PIC RAM ]---------�
 	;; -----------------------------------------------------
	movlw	RAM_OFFSET_FX_FUNCTION			;; Fx RAM AREA OFFSET (!)
 	addwf   Fx_LOAD_WRITE_ADR_COUNTER_TMP, W      	;; + (0 to 31 [32]) => Fx Tab Pos
	movwf   FSR0L		        	;; LOAD CURREND OFFSET IND. ADR. for Fx Fuction Reg. @ current Fx Pos.
 	movff   Fx_PATCH_DATA_TMP, INDF0 	;; WRITE FX FUNCTION @ currend Pos to Fx RAM
	
		;; ----------[PERFORM CYCLE or Exit READ CYCLE]----
		;; ------------------------------------------------
		incf Fx_LOAD_WRITE_ADR_COUNTER_TMP, F ;; pos = pos +1 | pos range 0 to 31 [32 pos]
		
		;; check if pos = 32 [range = 0 to 31]
		movlw 	FX_POS_MAX_PLUS1 ;; dec 31+1
		cpfseq 	Fx_LOAD_WRITE_ADR_COUNTER_TMP
			
			;; < 32
			;; write 32x
			rgoto EEPROM_PATCH_LOAD_FX_FUNC_FROM_EEPROM
		
			;; Pos = 32 
			;; RESET POS and EXIT THIS CYCLE
			clrf  Fx_LOAD_WRITE_ADR_COUNTER_TMP	;; reset pos
		;; ----------[PERFORM CYCLE or Exit READ CYCLE]----
			
;; -----------[END: LOAD Fx Function (Pos 0-31)]-------------------

;; -----------[LOAD Fx DATA (Pos 0-31)]-------------------
EEPROM_PATCH_LOAD_FX_DATA_FROM_EEPROM
	;; LOAD Fx DATA BYTES CYCLE - DO 32x
 	movlw		EEPROM_OFFSET_FX_DATA			;; EEPROM OFFSET (!)
	addwf		Fx_LOAD_WRITE_ADR_COUNTER_TMP, W	;; OFFSET + POS
	movwf        	EEADR 			;; SET EEPROM ADRESS [OFFSET+POS]
        call        	MIOS_EEPROM_Read       ;; READ FX FUNCTION @ currend Pos of PIC EEPROM (EEADR will be incremented)
 	movwf        	Fx_PATCH_DATA_TMP        ;; WRITE FX FUNCTION @ currend Pos TO MIOS_PARAMETER2
 	
 	;; WRITE FX FUNCTION @ Currend Fx Pos to PIC RAM
 	movlw	RAM_OFFSET_FX_DATA			;; Fx RAM AREA OFFSET (!)
 	addwf   Fx_LOAD_WRITE_ADR_COUNTER_TMP, W        ;; + (0 to 31 [32]) => Fx Tab Pos
	movwf   FSR0L		        	;; LOAD CURREND OFFSET IND. ADR. for Fx Fuction Reg. @ current Fx Pos.
 	movff   Fx_PATCH_DATA_TMP, INDF0 	;; WRITE FX FUNCTION @ currend Pos to Fx RAM
; 	
	
	
		incf Fx_LOAD_WRITE_ADR_COUNTER_TMP, F ;; pos = pos +1 | pos range 0 to 31 [32 pos gesamt]
		
		;; check if pos = 32 [range = 0 to 31]
		movlw 	FX_POS_MAX_PLUS1 ;; dec 31+1
		cpfseq Fx_LOAD_WRITE_ADR_COUNTER_TMP
			
			;; < 32
			;; write 32x
			;; DO ANOTHER CYCLE
			rgoto EEPROM_PATCH_LOAD_FX_DATA_FROM_EEPROM
			
			;; = 32
			;; QUIT THE CYCLE
			;; GO ON LOAD Fx Pre Exec. Time
			clrf  Fx_LOAD_WRITE_ADR_COUNTER_TMP	;; reset pos; 	

;; -----------------------------[END: LOAD Fx Function (Pos 0-31)]-------------------
			

;; -----------------------------[LOAD Fx TIME (Pos 0-31)]----------------------------
EEPROM_PATCH_LOAD_FX_TIME_FROM_EEPROM
	;; LOAD Fx Pre Exec. Time BYTES - DO CYCLE 32x	
 	movlw	EEPROM_OFFSET_FX_TIME_PRE_EXECUTION	;; EEPROM OFFSET (!)
	addwf	Fx_LOAD_WRITE_ADR_COUNTER_TMP, W	;; OFFSET + POS
	movwf   EEADR 			;; SET EEPROM ADRESS [OFFSET+POS]
        call    MIOS_EEPROM_Read        ;; READ FX FUNCTION @ currend Pos of PIC EEPROM (EEADR will be incremented)
 	movwf   Fx_PATCH_DATA_TMP       ;; WRITE FX FUNCTION @ currend Pos TO MIOS_PARAMETER2
 	
 	;; WRITE FX FUNCTION @ Currend Fx Pos to PIC RAM
 	movlw	RAM_OFFSET_FX_TIME_PRE_EXECUTION      ;; Fx RAM AREA OFFSET (!)
 	addwf   Fx_LOAD_WRITE_ADR_COUNTER_TMP, W      ;; + (0 to 31 [32]) => Fx Tab Pos
	movwf   FSR0L		        	;; LOAD CURREND OFFSET IND. ADR. for Fx Fuction Reg. @ current Fx Pos.
 	movff   Fx_PATCH_DATA_TMP, INDF0 	;; WRITE FX FUNCTION @ currend Pos to Fx RAM

	
		incf Fx_LOAD_WRITE_ADR_COUNTER_TMP, F ;; pos = pos +1 | pos range 0 to 31 [32 pos gesamt]
		
		;; check if pos = 32 [range = 0 to 31]
		movlw FX_POS_MAX_PLUS1 ;; dec 31+1
		cpfseq Fx_LOAD_WRITE_ADR_COUNTER_TMP
			
			;; < 32
			;; write 32x
			rgoto EEPROM_PATCH_LOAD_FX_TIME_FROM_EEPROM
			
			;; = 32
			;; GO AND WRITE "L: OK" on LCD Row 4
			;; AND THEN WRITE PATCH NAME ON LCD ROW 1
			rgoto PATCH_LOAD_OK
;; --------------------------------------------------------------------------------------------------
;; -----------[END: LOAD Fx Functions from INTERNAL PATCH (PIC EEPROM) (Pos 0-31)]-------------------
;; --------------------------------------------------------------------------------------------------


;; --------------------------------------------------------------------------------------------------
;; -----------[START: LOAD Fx Functions from BANKSTICK (Pos 0-31)]-----------------------------------
;; --------------------------------------------------------------------------------------------------
BANKSTICK_PATCH_LOAD_FX_FUNC_FROM_BANKSTICK
	;; LOAD Fx FUNCTION BYTES CYCLE - DO 32x
	;; LOAD FX FUNCTIONS @ from BANKSTICK to PIC RAM
	;; -----------------------------------------------------------------
 	movlw	BANKSTICK_PATCH_OFFSET_FX_FUNCTION	;; Fx AREA - BANKSTICK OFFSET (!)
	addwf	Fx_LOAD_WRITE_ADR_COUNTER_TMP, W	;; OFFSET + POS
	; store BANKSTICK low-byte of address in MIOS_PARAMETER1
	movwf    MIOS_PARAMETER1
      	; store BANKSTICK high-byte of address in MIOS_PARAMETER2
        movff   BROWSE_PATCH_NR	, MIOS_PARAMETER2
        decf	MIOS_PARAMETER2, F	;; ADR (HI BYTE) BS PATCH = CURREND PATCH NR -1
        ;; READ BYTE @ BANKSTICK
        call        	MIOS_BANKSTICK_Read    	;; READ FX FUNCTION @ from BANKSTICK 
 	movwf        	Fx_PATCH_DATA_TMP       ;; WRITE FX FUNCTION @ TO TMP
 	;; --------------------------------------------------------------------
 	;; ------[ WRITE FX FUNCTION @ Currend Fx Pos ==> to PIC RAM ]---------
 	;; --------------------------------------------------------------------
	movlw	RAM_OFFSET_FX_FUNCTION			;; Fx RAM AREA OFFSET (!)
 	addwf   Fx_LOAD_WRITE_ADR_COUNTER_TMP, W      	;; + (0 to 31 [32]) => Fx Tab Pos
	movwf   FSR0L		        	;; LOAD CURREND OFFSET IND. ADR. for Fx Fuction Reg. @ current Fx Pos.
 	movff   Fx_PATCH_DATA_TMP, INDF0 	;; WRITE FX FUNCTION @ currend Pos to Fx RAM
	
		;; ----------[PERFORM CYCLE or Exit READ CYCLE]----
		;; ------------------------------------------------
		incf Fx_LOAD_WRITE_ADR_COUNTER_TMP, F ;; pos = pos +1 | pos range 0 to 31 [32 pos]
		
		;; check if pos = 32 [range = 0 to 31]
		movlw 	FX_POS_MAX_PLUS1 ;; dec 31+1
		cpfseq 	Fx_LOAD_WRITE_ADR_COUNTER_TMP
			
			;; < 32
			;; write 32x
			rgoto BANKSTICK_PATCH_LOAD_FX_FUNC_FROM_BANKSTICK
		
			;; Pos = 32 
			;; RESET POS and EXIT THIS CYCLE
			clrf  Fx_LOAD_WRITE_ADR_COUNTER_TMP	;; reset pos
		;; ----------[PERFORM CYCLE or Exit READ CYCLE]----
			
;; -----------[END: LOAD Fx Function (Pos 0-31)]-------------------

;; -----------[LOAD Fx DATA (Pos 0-31)]-------------------
BANKSTICK_PATCH_LOAD_FX_DATA_FROM_BANKSTICK
	;; LOAD Fx DATA BYTES CYCLE - DO 32x
 	;; -----------------------------------------------------------------
 	movlw	BANKSTICK_OFFSET_FX_DATA		;; Fx AREA - BANKSTICK OFFSET (!)
	addwf	Fx_LOAD_WRITE_ADR_COUNTER_TMP, W	;; OFFSET + POS
	; store BANKSTICK low-byte of address in MIOS_PARAMETER1
	movwf    MIOS_PARAMETER1
      	; store BANKSTICK high-byte of address in MIOS_PARAMETER2
        movff   BROWSE_PATCH_NR	, MIOS_PARAMETER2
        decf	MIOS_PARAMETER2, F	;; ADR (HI BYTE) BS PATCH = CURREND PATCH NR -1
        ;; READ BYTE @ BANKSTICK
        call    MIOS_BANKSTICK_Read    	;; READ FX FUNCTION BYTE from BANKSTICK
 	movwf   Fx_PATCH_DATA_TMP       ;; WRITE FX FUNCTION TO TMP
 	;; ------------------------------------------------------------------------------
 	;; WRITE FX FUNCTION @ Currend Fx Pos to PIC RAM
 	movlw	RAM_OFFSET_FX_DATA			;; Fx RAM AREA OFFSET (!)
 	addwf   Fx_LOAD_WRITE_ADR_COUNTER_TMP, W        ;; + (0 to 31 [32]) => Fx Tab Pos
	movwf   FSR0L		        	;; LOAD CURREND OFFSET IND. ADR. for Fx Fuction Reg. @ current Fx Pos.
 	movff   Fx_PATCH_DATA_TMP, INDF0 	;; WRITE FX FUNCTION @ currend Pos to Fx RAM
	
		;; READ CYCLE MANAGEMENT	
		incf Fx_LOAD_WRITE_ADR_COUNTER_TMP, F ;; pos = pos +1 | pos range 0 to 31 [32 pos gesamt]
		
		;; check if pos = 32 [range = 0 to 31]
		movlw 	FX_POS_MAX_PLUS1 ;; dec 31+1
		cpfseq Fx_LOAD_WRITE_ADR_COUNTER_TMP
			
			;; < 32
			;; write 32x
			;; DO ANOTHER CYCLE
			rgoto BANKSTICK_PATCH_LOAD_FX_DATA_FROM_BANKSTICK
			
			;; = 32
			;; QUIT THE CYCLE
			;; GO ON LOAD Fx Pre Exec. Time
			clrf  Fx_LOAD_WRITE_ADR_COUNTER_TMP	;; reset pos; 	

;; -----------------------------[END: LOAD Fx Function (Pos 0-31)]-------------------
			

;; -----------------------------[LOAD Fx TIME (Pos 0-31)]----------------------------
BANKSTICK_PATCH_LOAD_FX_TIME_FROM_BANKSTICK
	;; LOAD Fx Pre Exec. Time BYTES - DO CYCLE 32x	
 	;; -----------------------------------------------------------------
 	movlw	BANKSTICK_OFFSET_FX_TIME_PRE_EXECUTION	;; Fx AREA - BANKSTICK OFFSET (!)
	addwf	Fx_LOAD_WRITE_ADR_COUNTER_TMP, W	;; OFFSET + POS
	; store BANKSTICK low-byte of address in MIOS_PARAMETER1
	movwf    MIOS_PARAMETER1
      	; store BANKSTICK high-byte of address in MIOS_PARAMETER2
        movff   BROWSE_PATCH_NR	, MIOS_PARAMETER2
        decf	MIOS_PARAMETER2, F	;; ADR (HI BYTE) BS PATCH = CURREND PATCH NR -1
        ;; READ BYTE @ BANKSTICK
        call    MIOS_BANKSTICK_Read    	;; READ FX FUNCTION BYTE from BANKSTICK
 	movwf   Fx_PATCH_DATA_TMP       ;; WRITE FX FUNCTION TO TMP
 	;; ------------------------------------------------------------------------------
  	;; WRITE FX FUNCTION @ Currend Fx Pos to PIC RAM
 	movlw	RAM_OFFSET_FX_TIME_PRE_EXECUTION      ;; Fx RAM AREA OFFSET (!)
 	addwf   Fx_LOAD_WRITE_ADR_COUNTER_TMP, W      ;; + (0 to 31 [32]) => Fx Tab Pos
	movwf   FSR0L		        	;; LOAD CURREND OFFSET IND. ADR. for Fx Fuction Reg. @ current Fx Pos.
 	movff   Fx_PATCH_DATA_TMP, INDF0 	;; WRITE FX FUNCTION @ currend Pos to Fx RAM

	
		incf Fx_LOAD_WRITE_ADR_COUNTER_TMP, F ;; pos = pos +1 | pos range 0 to 31 [32 pos gesamt]
		
		;; check if pos = 32 [range = 0 to 31]
		movlw FX_POS_MAX_PLUS1 ;; dec 31+1
		cpfseq Fx_LOAD_WRITE_ADR_COUNTER_TMP
			
			;; < 32
			;; write 32x
			rgoto BANKSTICK_PATCH_LOAD_FX_TIME_FROM_BANKSTICK
			
			;; = 32
			;; GO AND WRITE "L: OK" on LCD Row 4
			;; AND THEN WRITE PATCH NAME ON LCD ROW 1
			rgoto PATCH_LOAD_OK
;; --------------------------------------------------------------------------------------------------
;; -------------------------[END: LOAD Fx Functions from BANKSTICK (Pos 0-31)]-----------------------
;; --------------------------------------------------------------------------------------------------


PATCH_LOAD_OK
		;; IRQ ROUTINES HANDLING: ON
		IRQ_ENABLE
		
		;; INFO ON LCD:
		;; LOADING PATCH DATA OK
		TABLE_ADDR LCD_CLEAR_ROW4
		call    MIOS_LCD_PrintString
		TABLE_ADDR TEXT_AY_MB_AY_PATCH_LOAD_OK
		call    MIOS_LCD_PrintString
		
		;; AFTER LOADING THE PATCH: 
		;; Currendt Patch = BROWSE PATCH
		movff       BROWSE_PATCH_NR, CURREND_PATCH_NR

		;; WRITE ASC. NAME @ LCD ROW1
		;; subfunction @ patch_name_writing.inc
		call WRITE_PATCH_NAME_ON_LCD
			
			;; EXIT
			return

;; ---------------------------------------------------------------

;; -------------------[ERROR MESSAGE]-----------------------------
;; -------------- LOAD or WRITE FAILED ---------------------------

PATCH_DATA_LOADING_Failed
	;; ERROR - SORRY, NO PATCH WAS LOADED :-(
	;; WRITE ERROR on LCD
	TABLE_ADDR LCD_CLEAR_ROW4
	call    MIOS_LCD_PrintString
	TABLE_ADDR  TEXT_AY_MB_AY_PATCH_ERROR
	call    MIOS_LCD_PrintString
		;; EXIT	AFTER ERROR MESSAGE
		return




;; 

;; #########################################################################################
;; -----------------------------------------------------------------------------------------
;; -------------------------------[:: SUBROUTINES ::]---------------------------------------
;; -----------------------------------------------------------------------------------------
;; #########################################################################################
;; -----------------------------------------------------------------------------------------
;; -----------------------------------------------------------------------------------------
;; -----------------------[LOAD DATA BYTE from EEPROM OR BANKSTICK to PIC RAM]--------------
;; -----------------------------------------------------------------------------------------
LOAD_PATCH_DATA_BYTE
	;; CHECK:
	;; PATCH_MENU_STATE_FLAGS
	;; BIT2 = 0 = LOAD EEPROM PATCH (PIC INTERNAL PATCH)
	;; BIT2 = 1 = LOAD BANKSTICK PATCH
	btfsc	PATCH_MENU_STATE_FLAGS, BIT2_SELECTED_PATCH_BANKSTICK_OR_INTERNAL
			;; ===[LOAD DATA BYTE from BANKSTICK TO PIC RAM]=====
			;; BIT2 = SET	
			rgoto LOAD_PATCH_DATA_BYTE_FROM_BANKSTICK
				
		;; ===========[WRITE DATA BYTE TO INTERNAL EEPROM]===========
		;; BIT2 = CLEAR
		;; LOAD INTERNAL PATCH (from PIC EEPROM)
		call    MIOS_EEPROM_Read
		movwf 	CURREND_PATCH_DATA_BYTE_TEMP
		;; LOADED BYTE HAS BEEN WRITTEN TO CURREND_PATCH_DATA_BYTE_TEMP
		;; EEPROM ADR = EEPROM ADR +1
		;; EXIT THIS SUBROUTINE
		return
		;; ==============================================

LOAD_PATCH_DATA_BYTE_FROM_BANKSTICK
			;; ===================[WRITE DATA BYTE TO BANKSTICK]================
			; store low-byte of address in MIOS_PARAMETER1
			movff    BANKSTICK_LOW_BYTE_ADR_COUNTER, MIOS_PARAMETER1
      			; store high-byte of address in MIOS_PARAMETER2
        		movff   BROWSE_PATCH_NR	, MIOS_PARAMETER2
        		decf	MIOS_PARAMETER2, F	;; ADR (HI BYTE) BS PATCH = CURREND PATCH NR -1
        						;; -1 OFFSET
        		;; LOAD THE CURREND DATA BYTE from BANKSTICK
        		call    MIOS_BANKSTICK_Read        		; (address will be incremented
        			                                      	;  after write)
        		;; bnz   BSProgrammingFailed       		; branch to your exception handler
						
			;; STORE PATCH DATA BYTE TO CURREND_PATCH_DATA_BYTE_TEMP
        		movwf	CURREND_PATCH_DATA_BYTE_TEMP
			;; --------------------------------------------------------------------
			;; BANKSTICK ADR = BANKSTICK ADR +1 (LOW BYTE)
			incf	BANKSTICK_LOW_BYTE_ADR_COUNTER, F
			;; --------------------------------------------------------------------  
			;; EXIT THIS SUBROUTINE
			return
			;; =====================================================================
;; ---------------------------------------------------------------------------------------------
;; -----------------------[END: WRITE DATA BYTE TO EEPROM OR BANKSTICK]-------------------------
;; ---------------------------------------------------------------------------------------------
