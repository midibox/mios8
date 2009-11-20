;; midibox_ay_ini.inc
;; function: initialisation of application variables etc. 

INIT_VARIABLES
;; JUMPED IN HERE FROM USER_Init (main.asm)

;; ----------------------------------------------------------
;;  SETTING THE AY 3 8912 (SHADOW) REGISTERS AFTER STARTUP
;;  Initialize the AY REGISTERS 0 to 14
;;  AND SETUP AY APPLICATION WORKING REGISTER
;;  Initialize the AY DOUT LED's CS
;; ----------------------------------------------------------

;; -----[SET AY 3 8912 REGISTERS TO STARTUP DEFAULT VALUES]--------- 
;; -----------------------------------------------------------------
; AY ADRESS 	=> AY_REG_ADDRESS [8BIT, DEC. 0-16, HEX 0x00 - 0x10]
; AY DATA	=> AY_REG_DATA	[8BIT, DEC. 0-255, HEX 0x00 - 0xFF]
	
	;; AY REG 0 - TONE PERIOD FINE CH A [8BIT]
	movlw AY_CHIP_ADR_00_TONE_A_FREQUENCY_FINE; ADRESS => AY REGISTER
	movwf AY_REG_ADDRESS 		; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG0_FINE_TUNE_A	; 8 BIT DATA FOR AY REG 0  
	movwf AY_R0_CH_A_FINE_TUNE	; WRITE => AY SHADOW REGISTER
	movwf AY_REG_DATA 		; FOR => AY_SR_Write (WRITE DATA TO AY REG)
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE	


	
	;; AY REG 1 - TONE PERIOD COASE CH A [4BIT]
	movlw AY_CHIP_ADR_01_TONE_A_FREQUENCY_PERIOD_COARSE; ADRESS => AY REGISTER
	movwf AY_REG_ADDRESS 		; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG1_COARSE_TUNE_A	; THE 4BIT VALUE OF THE AY REG DATA [0x00 - 0x0F]
	movwf AY_R1_CH_A_COARSE_TUNE	; WRITE => AY SHADOW REGISTER
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE


	;; AY REG 2 - TONE PERIOD FINE CH B [8BIT]
	movlw AY_CHIP_ADR_02_TONE_B_FREQUENCY_PERIOD_FINE; ADRESS => AY REGISTER
	movwf AY_REG_ADDRESS 		; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG2_FINE_TUNE_B	; 8 BIT DATA
	movwf AY_R2_CH_B_FINE_TUNE	; WRITE => AY SHADOW REGISTER
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE	

		
	
	;; AY REG 3 - TONE PERIOD COASE CH B [4BIT]
	movlw AY_CHIP_ADR_03_TONE_B_FREQUENCY_PERIOD_COARSE; ADRESS => AY REGISTER
	movwf AY_REG_ADDRESS		; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG3_COARSE_TUNE_B	; 4 BIT DATA
	movwf AY_R3_CH_B_COARSE_TUNE	; WRITE => AY SHADOW REGISTER
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE


	;; AY REG 4 - TONE PERIOD FINE CH C [8BIT]
	movlw AY_CHIP_ADR_04_TONE_C_FREQUENCY_PERIOD_FINE; ADRESS => AY REGISTER
	movwf AY_REG_ADDRESS 		; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG4_FINE_TUNE_C	; 8 BIT DATA
	movwf AY_R4_CH_C_FINE_TUNE	; WRITE => AY SHADOW REGISTER
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE	
		
	
	
	;; AY REG 5 - TONE PERIOD COASE CH C [4BIT]
	movlw AY_CHIP_ADR_05_TONE_C_TONE_PERIOD_COARSE; ADRESS => AY REGISTER
	movwf AY_REG_ADDRESS		; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG5_COARSE_TUNE_C	; 4 BIT DATA
	movwf AY_R5_CH_C_COARSE_TUNE	; WRITE => AY SHADOW REGISTER
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		

	;; AY REG 6 - NOISE PERIOD [5BIT]
	movlw AY_CHIP_ADR_06_NOISE_PERIOD; ADRESS => AY REGISTER
	movwf AY_REG_ADDRESS		; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG6_NOISE_PERIOD	; [0-31] 5 BIT DATA [BIN xxx1 1111]
	movwf AY_R6_NOISE_PERIOD	; WRITE => AY SHADOW REGISTER
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		


	;; AY REG 7
	;; TONE ABC ON/OFF
	;; MIX NOISE TO TONE ABC 
	;; [IO PORT A (+B): NOT IN USE YET] 
	movlw AY_CHIP_ADR_07_TONE_NOISE_ON_OFF; REG 7 => AY ENABLE IOA, TONE CH ABC, NOISE ABC
					; SEE NOTES @ APP_DEFINES.H => AY_R7_ENABLE_ABC_NOISE_IO
	movwf AY_REG_ADDRESS 		; ADDRESS FOR => AY_SR_Write

	movlw INI_REG7_TONE_ABC_NOISE_ABC_IO_AB			
	movwf AY_R7_ENABLE_ABC_NOISE_IO ; WRITE INI VALUE TO => AY SHADOW REGISTER 7

		;; TONE ABC is ON but there is NO MIDI NOTE On EVENT YET SO:
		;; MIDIFILTER ALL OFF
		movlw MIDI_FILTER_ALL_OFF		; BIN 00111111
		;; OR AY SHADOW REGISTER7 with MIDI FILTER VALUE [ALL OFF]
		iorwf AY_R7_ENABLE_ABC_NOISE_IO, W	; BIN 11111000 (AY_R7_ENABLE_ABC_NOISE_IO)
	  						; BIN 00111111	
	  						; =   11111111 ([TONE ABC OFF / NOISE ABC OFF])	
	;; WRITE REGISTER7 [with MIDI FILTER] to AY REGISTER7
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		

	;; AY REG 8 - THE AMPLITUDE OF THE CH. A SIGNAL
	movlw AY_CHIP_ADR_08_TONE_A_VOLUME_ENV_MODE_ON_OFF; REG 8 
	movwf AY_REG_ADDRESS 		; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG8_VOLUME_A_ENVELOPE_ON_OFF
	movwf AY_R8_CH_A_AMPLITUDE	; WRITE => AY SHADOW REGISTER 8
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE


	;; AY REG 9 - THE AMPLITUDE OF THE CH. B SIGNAL
	movlw AY_CHIP_ADR_09_TONE_B_VOLUME_ENV_MODE_ON_OFF; REG 9 
	movwf AY_REG_ADDRESS 		; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG9_VOLUME_B_ENVELOPE_ON_OFF
	movwf AY_R9_CH_B_AMPLITUDE	; WRITE => AY SHADOW REGISTER 9
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE


	;; AY REG 10 - THE AMPLITUDE OF THE CH. C SIGNAL
	movlw AY_CHIP_ADR_10_TONE_C_VOLUME_ENV_MODE_ON_OFF; REG 10 
	movwf AY_REG_ADDRESS 		; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG10_VOLUME_C_ENVELOPE_ON_OFF
	movwf AY_R10_CH_C_AMPLITUDE	; WRITE => AY SHADOW REGISTER 10
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE


	;; AY REG 11 - FREQUENCY OF THE ENVELOPE (FINE)
	movlw AY_CHIP_ADR_11_GLOBAL_ENVELOPE_FINE; REG 11
	movwf AY_REG_ADDRESS 		; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG11_ENVELOPE_FINE
	movwf AY_R11_ENV_PERIOD_FINE	; WRITE => AY SHADOW REGISTER 11
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE



	;; AY REG 12 - FREQUENCY OF THE ENVELOPE (COASE)
	movlw AY_CHIP_ADR_12_GLOBAL_ENVELOPE_COARSE; REG 12
	movwf AY_REG_ADDRESS 		; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG12_ENVELOPE_COARSE
	movwf AY_R12_ENV_PERIOD_COARSE	; WRITE => AY SHADOW REGISTER 12
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
	
	
	;; AY REG 13 - ENVELOPE SHAPE (+ENVELOPE CYCLE)
	;; IF "AMPLITUDE MODE" *BIT 5* IS SET @ TONE ABC
  	;; FOR DETAILS SEE AY-3-8912 DATASHEET
	movlw AY_CHIP_ADR_13_ENVELOPE_SHAPE_AND_CYCLE; REG 13
	movwf AY_REG_ADDRESS 	; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG13_ENVELOPE_SHAPE
	movwf AY_R13_ENV_SHAPE_CYCLE	; WRITE => AY SHADOW REGISTER 13
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE

	
	;; AY REG 14 - WRITE BIN '10101010' TO AY PORT A
	movlw AY_CHIP_ADR_14_IO_PORT_A_DATA; REG 14 => AY PORT A ADRESS
	movwf AY_REG_ADDRESS		; ADDRESS FOR => AY_SR_Write
	;; DEFINE AY REG DEFAULT VALUE HERE
	movlw INI_REG14_IO_A		; WRITE BIN '10101010' TO AY PORT A
	movwf AY_R14_AY_PORTA_DATA 	; WRITE => AY SHADOW REGISTER 14 
	movwf AY_REG_DATA 		; FOR => AY_SR_Write
	
		call LABEL_AY_SR_Write	


;; --------------------------------------------------------------------------
;;  SET ENVELOPE SHAPE
;;  Initialize the AY DOUT LED's
;; --------------------------------------------------------------------------
	
	call LED_ToneA_on
	call LED_ToneB_on
	call LED_ToneC_on
	
	;; ENVELOPE SHAPE	
	movlw INI_REG13_ENVELOPE_SHAPE
	movwf ENV_SHAPE_0_15_to_1_8_TRANSLATOR
	call LED_Envelope_Shape_1_on
	

;; --------------------------------------------------------------------------
;;  Initialize the AY WORKING REGISTER
;; --------------------------------------------------------------------------
	
	;; THE MAIN MENU / SUB MENU SWITCH
	;; 0 = MAIN MENU (INITIAL)
	;; (1 = SUB MENU )
	movlw 	MAIN_MENU_ON_VIA_0
	movwf	SWITCH_BETWEEN_MAIN_AND_SUBMENU

	;; THE MAIN MENU SWITCH
	;; INITIAL MAIN MENU = 0
	movlw 	INI_START_UP_MAIN_MENU
	movwf	CURRENT_MAIN_MENU
	
	;; THE SUB MENU SWITCH
	;; INITIAL SUB MENU SWITCH = 0
	movlw 	INI_SUB_MENU 
	movwf 	AY_SUB_MENUE_KEY_SWITCH

	;; INITAL VALUES LCD MAIN MENU COURSER POSITION
	movlw 	INI_COURSER_POS	
	movwf   MIOS_PARAMETER1
	movff   MIOS_PARAMETER1, AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO
	movff   MIOS_PARAMETER1, AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE
	movff   MIOS_PARAMETER1, AY_LCD_COURSER_POS_MAIN_MENU_ABC_NOISE
	movff   MIOS_PARAMETER1, AY_LCD_COURSER_POS_MAIN_MENU_AMPLITUDE
	movff   MIOS_PARAMETER1, AY_LCD_COURSER_POS_MAIN_MENU_ENVELOPE
	movff   MIOS_PARAMETER1, AY_LCD_COURSER_POS_MAIN_MENU_MIDI_CONFIG
	movff   MIOS_PARAMETER1, AY_LCD_COURSER_POS_MAIN_MENU_VELOCITY_CONFIG
	movff 	MIOS_PARAMETER1, AY_LCD_COURSER_POS_MAIN_MENU_FX
	
	
	movlw 	INI_TUNE_FINE_USER
	movwf 	AY_TONE_A_TUNE_FINE_USER 
	movwf  	AY_TONE_B_TUNE_FINE_USER 
	movwf 	AY_TONE_C_TUNE_FINE_USER 
	
	movlw 	INI_TUNE_COARSE_USER
	movwf	AY_TONE_A_TUNE_COARSE_USER
	movwf	AY_TONE_B_TUNE_COARSE_USER
	movwf	AY_TONE_C_TUNE_COARSE_USER
	
	movlw 	INI_TONE_A_MIDI_CHANNEL_USER
	movwf	AY_TONE_A_MIDI_CHANNEL_USER
	
	movlw 	INI_TONE_B_MIDI_CHANNEL_USER
	movwf	AY_TONE_B_MIDI_CHANNEL_USER
	
	movlw 	INI_TONE_C_MIDI_CHANNEL_USER
	movwf	AY_TONE_C_MIDI_CHANNEL_USER

	;; MIDI SWITCH ABC TONE/NOISE ON OF
	movlw 	INI_MIDI_ALL_OFF	
			; INITIAL VALUE = BIN 00 111 111
			; AY REG 07     =     IO CBA CBA
			; 	    	=     BA
			; 	    	=        NOISE
			; 	    	=            TONE
	movwf   AY_TONE_ABC_MIDI_ON_OFF
	
	;; THE USER SWITCH FOR TONE/NOISE ABC ON/OFF
	movlw INI_USER_TONE_ABC_NOISE_ABC_ON_OFF
	movwf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH 	
	
	;; ALL SPECIAL KEYS OFF
	clrf AY_KEYB_SPECIAL_KEY_TOGGLE

	;; VELOCITY TO TONE A C B AND NOISE RATE ON/OFF
	movlw INI_VELOCITY_TO_ACB_NOISE_PERIOD
	movwf AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF
	
	;; => midi_evnt_pipeline.inc
	clrf AY_MIDI_VELOCITY_TO_VOLUME
	clrf AY_MIDI_VELOCITY_TO_NOISE_PERIOD
	
	
	;; midi_controller.inc
	;; CC HAndling
	clrf MIDI_EVENT_CHANGE_FLAG
	
	;; -----------[PATCH DATA]------------------------------
	;; -----------------------------------------------------
	;; SET BANKSTICK [0 to 7]
	;; DEFAULT BANKSTICK = 1
	;; INTERNAL: 	BANKSTICK 1 = 0x00
	;; 		BANKSTICK 2 = 0x01
	;; 		...
	;; 		BANKSTICK 8 = 0x07
	movlw       0x00
        call        MIOS_BANKSTICK_CtrlSet  ;; Select BankStick
	;; ----------------------------------------------------
	
	;; -------[CLEAR / RESET PATCH NAME]-------------------
	call BLANK_PATCH_NAME

	
	;; POS1 PATCH NAME [POS1=0,...POS8=7]
	clrf 	MIOS_PARAMETER1
	movff 	MIOS_PARAMETER1, PATCH_NAME_ASC_POS
	
	;; PATCH NUMBER, STAE, ETC.
	clrf	CURRENT_PATCH_NR 	;; INITIAL PATCH NUMBER = 0 = INT (PIC EEPROM)
	clrf	PATCH_MENU_STATE_FLAGS	;; See PATCH_MENU_STATE_FLAGS @ CONST DEF	
	clrf	FORMAT_BANKSTICK_CYCLE_COUNTER	;; See patch_write.inc 	
	
	;; USER: FX ABC off (See MAIN MENU 3 / Mixer)
	;; BIT0 = A (SET = ON / CLR = OFF)
	;; BIT1 = B (SET = ON / CLR = OFF)
	;; BIT2 = C (SET = ON / CLR = OFF)
	movlw INI_FX_USER_ON_OFF_ABC
	movwf FX_USER_ON_OFF_ABC
	
	;; RESET Fx 
	clrf Fx_MIDI_NOTE_ON_OR_OFF
	clrf FX_TIMER
	clrf FX_FUNCTION_CURRENT
	clrf FX_DATA_CURRENT
	clrf FX_TIME_PRE_EXECUTION_CURRENT
	clrf FX_POS_CURRENT
	clrf FX_NOP_ON_OFF
	clrf FX_POS_SESSION_CURRENT	
	clrf FX_FUNCTION_SESSION_CURRENT		
	clrf FX_TIME_PRE_EXECUTION_SESSION_CURRENT
	
	;; C64 KEYS (user_tick_key_repeat.inc)
	clrf 	AY_KEYB_SPECIAL_KEY_TOGGLE
	bsf 	AY_KEYB_SPECIAL_KEY_TOGGLE, FIRST_KEY_STRIKE_TRUE_OR_FALSE_BIT1	;; = SET (DO NOT CHANGE)
	clrf 	C64_KEY_AUTO_REPEAT_GENERAL_COUNTER
	clrf	KEY_AUTO_REPEAT_COUNTER
	
	
		;; exit here
		return

	
;; ------------------------------------------------------------------------------
;; ------------------------------------------------------------------------------


INIT_MIDI_CONF
	;; SET the MIDIbox AY Merger ON or OFF
	;; MEANS [MIDI DATA:IN] => [MIDI DATA:OUT]
	movlw       INI_MIOS_MIDI_MERGER_ON_OFF
        call        MIOS_MIDI_MergerSet
		;; exit here
	      	return

;; ------------------------------------------------------------------------------

INIT_TIMER0
;; JUMPED IN HERE FROM USER_Init

	 ;; TIMER0 is used as a MIDIBox AY Fx timer
	 ;; initialize (reset) timer0
	 
	 ;; ensure that timer0 interrupt not enabled!
	 ;; because the PIC based MIOS doesn't allow to add interrupt handlers 
        bcf     INTCON, T0IE
        
        ;; reset timer0 overflow (interrupt flag)
        bcf     INTCON, T0IF

        ;; ====[timer0 MEMO]======================================================
	;;  Bit 0 – 2     Timer prescale
	;;  Bit 3         Timer prescale assignment.  0=use prescal, 1=no prescale
	;;  Bit 4         Timer edge source (used only when counting)
	;;  Bit 5         Timer Source.  0=Internal, 1=Transitions on IO port 17
	;;  Bit 6         Timer mode.  0=16 bit mode, 1=8 bit mode
	;;  Bit 7         Timer On/Off.  0=Off, 1=On
	;;  ======================================================================
	;; -----------------------------------------
	;; SETUP timer0 for for MIDIBox AY Fx Option 
	;; ------------------------------------------
        ;; set to 16bit counter (BIT 6 = Clear), 
        ;; use internal clock source (BIT 5 = Clear), 
        ;; Ext. Clck adjust (Bit 4 = Clear) [nicht relevant, da int. Tacktquelle]
        ;; PSA (Vorteiler) BIT3 (= clear | Clear=on [slower] / SET=off [faster]) 
        ;; timer0 Prescaler [BIT 2,1,0]:
        ;; 000 = timer0/2 -> fast (this is what we need here :)
        ;; 001 = timer0/4
        ;; 010 = timer0/8
        ;; 011 = timer0/16
        ;; 100 = timer0/32
        ;; 101 = timer0/64
        ;; 110 = timer0/128
        ;; 111 = timer0/256 - slow
        movlw   B'00000000'
        movwf   T0CON
        
        ;; start timer0
        bsf     T0CON, TMR0ON		

		;; EXIT HERE
	      	return


INIT_USER_Timer
;; JUMPED IN HERE FROM USER_Init

	;; initialize the USER_Timer
	;; #########################
	;; The resolution of the timer is 100nS with Prescaler 1:1, 
	;; 200nS with 1:2, 
	;; 400nS with 1:4, 
	;; 800nS with 1:8. 
	;; The period (number of clocks) is specified as 16 bit value.
	;; prescaler value in WREG:
	;; 0x00: 1:1
	;; 0x01: 1:2
	;; 0x02: 1:4
	;; 0x03: 1:8
	;; number of ticks (low-byte) in MIOS_PARAMETER1
	;; number of ticks (high-byte) in MIOS_PARAMETER2

        ;; we want to setup the timer with a frequency of 500 Hz = 2 mS
        ;; prescaler 1:1 should be used
        ;; calculate the required number of clocks for this period:
        ;; clocks = period / 100 nS = 2 mS / 100 nS = 20000
        ;; calculate low and high byte:
        ;;    low byte  = period & 0xff
        ;;    high byte = period >> 8
        ;;    therefore:
        movlw        20000 & 0xff
        movwf        MIOS_PARAMETER1
        movlw        20000 >> 8
        movwf        MIOS_PARAMETER2
        movlw        0x00
        call  MIOS_TIMER_Init

        ;; now the USER_Timer function is called every 2 mS!
		
		;; EXIT HERE
		return

;; -----------------------------------------------------------
;; ----------------[SUBROUTINE]-------------------------------
;; -----------------------------------------------------------
BLANK_PATCH_NAME
	;; RESET PATCH NAME
	;; WRITE "        " 
	;; FOR INITIAL "NO PATCH LOADED AND
	;; FOR NEW FORMATTED BANKSTICK PATCH NAME
	;; 8x PATCH KEY ASC
	movlw 	" "
	movwf 	MIOS_PARAMETER1
	movff 	MIOS_PARAMETER1, PATCH_NAME_ASC_0
	movff 	MIOS_PARAMETER1, PATCH_NAME_ASC_1
	movff 	MIOS_PARAMETER1, PATCH_NAME_ASC_2
	movff 	MIOS_PARAMETER1, PATCH_NAME_ASC_3
	movff 	MIOS_PARAMETER1, PATCH_NAME_ASC_4
	movff 	MIOS_PARAMETER1, PATCH_NAME_ASC_5
	movff 	MIOS_PARAMETER1, PATCH_NAME_ASC_6
	movff 	MIOS_PARAMETER1, PATCH_NAME_ASC_7
		return
