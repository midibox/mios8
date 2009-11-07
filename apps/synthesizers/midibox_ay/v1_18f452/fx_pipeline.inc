;; fx_pipeline.inc
;; ---------------

;; fucnction: perform fx routines if fx mode @ a|b|c is on
;; this function is called by USER_Tick periodically

FX_PIPELINE
	
	;; CHECK IF FX MODE IS ON
	;; AND FILTER: 
	;; FX MIDI ON  A|B|C [AND WITH] FX USER ON A|B|C 
	;; MIDI CH ABC		[x|x|x|x|x|x|C|B|A]
	;; USER FX SETTING	[x|x|x|x|x|x|C|B|A]
		
	movf	Fx_MIDI_NOTE_ON_OR_OFF, W
	andwf	FX_USER_ON_OFF_ABC, W
	movwf	FX_FILTER
		
		clrw
		cpfsgt FX_FILTER
		
				;; GO ON IF A|B|C NOTE ON MATCH
				;; with     A|B|C Fx User ON 
				;; IF FX Filter = 0 Exit here
				rgoto FX_PIPELINE_EXIT


			;; OK. LET's GO ON...
			;; CHECK IF THERE IS A NEW FX SESSION
			movf FX_MIDI_NOTE_ON_SESSION, W
			cpfseq FX_PIPELINE_SESSION
				;; NO, NOT THE SAME NOTE ON SESSION
				;; OK, LET'S RESET Fx SESSION POS
				rgoto FX_NEW_SESSION_RESET_POS

				;; YES, THE SAME NOTE ON SESSION - GO ON WITH A NOTHER FX POS BUT
				;; FIRST CHECK IF Fx Tab Pos > 31 ==> NOP ON (NO Fx OPERATION)
				rgoto CHECK_IF_FX_POS_CURREND_SESSION_IS_NOP


FX_NEW_SESSION_RESET_POS
	;; reset currend session/pos
	;; after every new *note on* event
	movff	FX_MIDI_NOTE_ON_SESSION, FX_PIPELINE_SESSION
	clrf	FX_POS_SESSION_CURREND	;; RESET FX TABLE POS.
	clrf 	FX_TIMER		;; RESET FX Timer
	clrf 	FX_NOP_ON_OFF		;; RESET Fx NOP [NO OPERATION] (0x00 = NOP:OFF)
	;; NOW THE RESET WAS DONE
	;; GO AND LOAD Fx Pos DATA @ Pos 0
	rgoto LOAD_CURREND_SESSION_POS_FX_DATA	


CHECK_IF_FX_POS_CURREND_SESSION_IS_NOP
	;; = 0 = Fx NOP IS OF (FX OPERATION IS ON)
	;; > 0 = Fx NOP IS ON (FX OPERATION IS OFF)
	movlw 	0x00
	cpfseq 	FX_NOP_ON_OFF
		
		;; NO - Fx NOP > 0
		;; SO EXIT HERE ((0x01-0xFF = NOP:ON)
		rgoto FX_PIPELINE_EXIT

	;; YES - Fx NOP = 0 (0x00 = NOP:OFF)
	;; SO GO ON AND LOAD Fx DATA 
	
LOAD_CURREND_SESSION_POS_FX_DATA
	;; UPDATE Currend Fx 
	;; POINTER TO FX SESSION CURREND POS [Pos Range: 0 to 31]
	;; ------------------------
	;; LOAD FX_FUNCTION_SESSION_CURREND		@ THE CURREND Fx POS = FX_POS_SESSION_CURREND
	;; FX_TIME_PRE_EXECUTION_SESSION_CURREND	@ THE CURREND Fx POS = FX_POS_SESSION_CURREND
	;; FX_DATA_SESSION_CURREND			@ THE CURREND Fx POS = FX_POS_SESSION_CURREND
	call LOAD_FX_SESSION_DATA_CURREND_POS		
	
		
	;; WORKFLOW *PRE Fx TIME PERIOD* 
	;; (TIME Bevor the Fx will be Executed)
	;; principle: 
	;; IS Fx Timer (triggert by Timer0) [0-255] > FX_TIME_PRE_EXECUTION_SESSION_CURREND [0-200]
	;; FX TIMER > FX_TIME_PRE_EXECUTION_SESSION_CURREND ?
	movf FX_TIME_PRE_EXECUTION_SESSION_CURREND, W
	cpfsgt FX_TIMER
		;; NO - Currend Fx Pre Delay Time takes more Timer0 Cycles
		;; SO QUIT HERE and try again...
		;; FX TIMER < or = FX_TIME_PRE_EXECUTION_SESSION_CURREND
		rgoto FX_PIPELINE_EXIT

	;; YES 
	;; FX TIMER > FX_TIME_PRE_EXECUTION_SESSION_CURREND
	;; SO GO ON WITH Corrend Pos Fx Execution
	;; TO DO - GO ON WITH Fx Function Switch
	

EXECUTE_FX_AT_CURREND_POS
	
	;; Fx Functions

	;; FUNCTIONS [0x00 to 0x25]
	;; ------------------------
	;; 0  0x00 => End Fx			(Fx Off)
	;; 1  0x01 => On  Tone A		
	;; 2  0x02 => Off Tone A
	;; 3  0x03 => On  Tone B
	;; 4  0x04 => Off Tone B
	;; 5  0x05 => On  Tone C
	;; 6  0x06 => Off Tone C
	;; 7  0x07 => +Tune A	[Data: 0-255]
	;; 8  0x08 => -Tune A	[Data: 0-255]
	;; 9  0x09 => +Tune B	[Data: 0-255]
	;; 10 0x0A => -Tune B	[Data: 0-255]
	;; 11 0x0B => +Tune C	[Data: 0-255]
	;; 12 0x0C => -Tune C	[Data: 0-255]
	;; 13 0x0D => Vol.A	[Data: 0-15] (This function overrides the ENVELOPE Mode during the Fx period)
	;; 14 0x0E => Vol.B	[Data: 0-15] (This function overrides the ENVELOPE Mode during the Fx period)
	;; 15 0x0F => Vol.C	[Data: 0-15] (This function overrides the ENVELOPE Mode during the Fx period)
	;; 16 0x10 => On  Noise A
	;; 17 0x11 => Off Noise A
	;; 18 0x12 => On  Noise B
	;; 19 0x13 => Off Noise B
	;; 20 0x14 => On  Noise C 
	;; 21 0x15 => Off Noise C
	;; 22 0x16 => Noise Frq. [Data: 0-31]
	;; 23 0x17 => ENV ON  A
	;; 24 0x18 => ENV OFF A
	;; 25 0x19 => ENV ON  B
	;; 26 0x1A => ENV OFF B
	;; 27 0x1B => ENV ON  C
	;; 28 0x1C => ENV OFF C
	;; 29 0x1D => ENV Shape	[Data: 0-15]
	;; 30 0x1E => ENV Fine	[Data: 0-255]
	;; 31 0x1F => ENV Coar	[Data: 0-255]
	;; 32 0x20 => WRITE NOTE ON #1 ==> Tone A
	;; 33 0x21 => WRITE NOTE ON #2 ==> Tone A
	;; 34 0x22 => WRITE NOTE ON #3 ==> Tone A
	;; 35 0x23 => WRITE NOTE ON #1 ==> Tone B
	;; 36 0x24 => WRITE NOTE ON #2 ==> Tone B
	;; 37 0x25 => WRITE NOTE ON #3 ==> Tone B
	;; 38 0x26 => WRITE NOTE ON #1 ==> Tone C
	;; 39 0x27 => WRITE NOTE ON #2 ==> Tone C
	;; 40 0x28 => WRITE NOTE ON #3 ==> Tone C
	;; 41 0x29 => GOTO Fx POS [Data: 0-31]

	;; JUMP TO THE GIVEN Fx FUNCTION
	movf FX_FUNCTION_SESSION_CURREND, W
	JUMPTABLE_2BYTES 42 ;; [0 to 41]
        	rgoto        FX_FUNC_000 ;;   0x00	End Fx (Fx Off)
        	rgoto        FX_FUNC_001 ;;   0x01	On  Tone A
        	rgoto        FX_FUNC_002 ;;   0x02	Off Tone A
        	rgoto        FX_FUNC_003 ;;   0x03	On  Tone B
        	rgoto        FX_FUNC_004 ;; 	 ...	Off Tone B
        	rgoto        FX_FUNC_005 ;; 			On  Tone C
        	rgoto        FX_FUNC_006 ;; 			Off Tone C
        	rgoto        FX_FUNC_007	;; 			...
        	rgoto        FX_FUNC_008
        	rgoto        FX_FUNC_009
        	rgoto        FX_FUNC_010
        	rgoto        FX_FUNC_011
        	rgoto        FX_FUNC_012
        	rgoto        FX_FUNC_013
        	rgoto        FX_FUNC_014
        	rgoto        FX_FUNC_015
        	rgoto        FX_FUNC_016
        	rgoto        FX_FUNC_017
        	rgoto        FX_FUNC_018
		rgoto        FX_FUNC_019
		rgoto        FX_FUNC_020
		rgoto        FX_FUNC_021
		rgoto        FX_FUNC_022
		rgoto        FX_FUNC_023
		rgoto        FX_FUNC_024
		rgoto        FX_FUNC_025
		rgoto        FX_FUNC_026
		rgoto        FX_FUNC_027
		rgoto        FX_FUNC_028
		rgoto        FX_FUNC_029
		rgoto        FX_FUNC_030
		rgoto        FX_FUNC_031
		rgoto        FX_FUNC_032
		rgoto        FX_FUNC_033
		rgoto        FX_FUNC_034
		rgoto        FX_FUNC_035
		rgoto        FX_FUNC_036
		rgoto        FX_FUNC_037
		rgoto        FX_FUNC_038
		rgoto        FX_FUNC_039
		rgoto        FX_FUNC_040
		rgoto        FX_FUNC_041


FX_FUNC_000
	;; 0x00 => End Fx (Fx Off)
	;; Do NOTHING and Quit Here
	return
	
FX_FUNC_001	
	;; 0x01 => On Tone A
	;; TONE A ON

	;; USER SETTING FILTER 
	;; MIDI TONE ON FILTER 
	;; OR ==> AY_R7_ENABLE_ABC_NOISE_IO
	
	
	;; USER SETTING => TONE A|B|C ON/OFF [FILTER]:
	;; move USER SWITCH settings to W REG
	movf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, W 
			;; ABC TONE + NOISE ON/OFF USER SWITCH
			;; USER CLEAR or SET A BIT BY SWITCHING
			;; TONE/NOISE A/B/C 

	;; MIDI EVENT FILTER:
	;; *OR* [AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH] WITH [AY_TONE_ABC_MIDI_ON_OFF]
	;; MEMO: => MIDI EVENTS SET OR CLEAR BITS @ AY_TONE_ABC_MIDI_ON_OFF
	;; MEMO: => CLEAR BIT = [TONE / NOISE] *ON*
	 
	
	iorwf AY_TONE_ABC_MIDI_ON_OFF, W	
		;; OR AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH
		;; WITH AY_TONE_ABC_MIDI_ON_OFF
		;; NOW JUST THE RIGHT TONE/NOISE IS SWITCHED
		;; MOVE THE RESULT TO W REG
	
        ;; WRITE W REG TO THE SHADOW AY CHIP REG 7 
        ;; ==> AY_R7_ENABLE_ABC_NOISE_IO
        movwf AY_R7_ENABLE_ABC_NOISE_IO

	;; TONE A ON
	movff AY_R7_ENABLE_ABC_NOISE_IO, Fx_TEMP
	bcf  Fx_TEMP, BIT0_TONE_A_ON_OFF	
	
		;; SEND DATA TO AY REG 7 (AY Chip)
		movlw AY_CHIP_ADR_07_TONE_NOISE_ON_OFF	; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 7
		movwf AY_REG_ADDRESS 			; ADDRESS FOR 	=> AY_SR_Write
        
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
        
		;; --------------[SEND DATA TO AY REG 7]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================	
		
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_002
	;; 0x02 => Off Tone A
	;; TONE A OFF
	
	
	;; USER SETTING FILTER 
	;; MIDI TONE ON FILTER 
	;; OR ==> AY_R7_ENABLE_ABC_NOISE_IO
	
	
	;; USER SETTING => TONE A|B|C ON/OFF [FILTER]:
	;; move USER SWITCH settings to W REG
	movf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, W 
			;; ABC TONE + NOISE ON/OFF USER SWITCH
			;; USER CLEAR or SET A BIT BY SWITCHING
			;; TONE/NOISE A/B/C 

	;; MIDI EVENT FILTER:
	;; *OR* [AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH] WITH [AY_TONE_ABC_MIDI_ON_OFF]
	;; MEMO: => MIDI EVENTS SET OR CLEAR BITS @ AY_TONE_ABC_MIDI_ON_OFF
	;; MEMO: => CLEAR BIT = [TONE / NOISE] *ON*
	 
	
	iorwf AY_TONE_ABC_MIDI_ON_OFF, W	
		;; OR AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH
		;; WITH AY_TONE_ABC_MIDI_ON_OFF
		;; NOW JUST THE RIGHT TONE/NOISE IS SWITCHED
		;; MOVE THE RESULT TO W REG
	
        ;; WRITE W REG TO THE SHADOW AY CHIP REG 7 
        ;; ==> AY_R7_ENABLE_ABC_NOISE_IO
        movwf AY_R7_ENABLE_ABC_NOISE_IO

	
	;; TONE A OFF
	movff AY_R7_ENABLE_ABC_NOISE_IO, Fx_TEMP
	bsf  Fx_TEMP,BIT0_TONE_A_ON_OFF
	
		;; SEND DATA TO AY REG 7 (AY Chip)
		movlw AY_CHIP_ADR_07_TONE_NOISE_ON_OFF	; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 7
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
        
		;; --------------[SEND DATA TO AY REG 7]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================


				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_003
	;; 0x03 => On  Tone B
	
	;; USER SETTING FILTER 
	;; MIDI TONE ON FILTER 
	;; OR ==> AY_R7_ENABLE_ABC_NOISE_IO
	
	
	;; USER SETTING => TONE A|B|C ON/OFF [FILTER]:
	;; move USER SWITCH settings to W REG
	movf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, W 
			;; ABC TONE + NOISE ON/OFF USER SWITCH
			;; USER CLEAR or SET A BIT BY SWITCHING
			;; TONE/NOISE A/B/C 

	;; MIDI EVENT FILTER:
	;; *OR* [AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH] WITH [AY_TONE_ABC_MIDI_ON_OFF]
	;; MEMO: => MIDI EVENTS SET OR CLEAR BITS @ AY_TONE_ABC_MIDI_ON_OFF
	;; MEMO: => CLEAR BIT = [TONE / NOISE] *ON*
	 
	
	iorwf AY_TONE_ABC_MIDI_ON_OFF, W	
		;; OR AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH
		;; WITH AY_TONE_ABC_MIDI_ON_OFF
		;; NOW JUST THE RIGHT TONE/NOISE IS SWITCHED
		;; MOVE THE RESULT TO W REG
	
        ;; WRITE W REG TO THE SHADOW AY CHIP REG 7 
        ;; ==> AY_R7_ENABLE_ABC_NOISE_IO
        movwf AY_R7_ENABLE_ABC_NOISE_IO


	movff AY_R7_ENABLE_ABC_NOISE_IO, Fx_TEMP
	bcf  Fx_TEMP, BIT1_TONE_B_ON_OFF	
	
		;; SEND DATA TO AY REG 7
		movlw AY_CHIP_ADR_07_TONE_NOISE_ON_OFF	; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 7
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
        
		;; --------------[SEND DATA TO AY REG 7]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================	
		

				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_004
	;; 0x04 => Off Tone B

	;; USER SETTING FILTER 
	;; MIDI TONE ON FILTER 
	;; OR ==> AY_R7_ENABLE_ABC_NOISE_IO
	
	
	;; USER SETTING => TONE A|B|C ON/OFF [FILTER]:
	;; move USER SWITCH settings to W REG
	movf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, W 
			;; ABC TONE + NOISE ON/OFF USER SWITCH
			;; USER CLEAR or SET A BIT BY SWITCHING
			;; TONE/NOISE A/B/C 

	;; MIDI EVENT FILTER:
	;; *OR* [AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH] WITH [AY_TONE_ABC_MIDI_ON_OFF]
	;; MEMO: => MIDI EVENTS SET OR CLEAR BITS @ AY_TONE_ABC_MIDI_ON_OFF
	;; MEMO: => CLEAR BIT = [TONE / NOISE] *ON*
	 
	
	iorwf AY_TONE_ABC_MIDI_ON_OFF, W	
		;; OR AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH
		;; WITH AY_TONE_ABC_MIDI_ON_OFF
		;; NOW JUST THE RIGHT TONE/NOISE IS SWITCHED
		;; MOVE THE RESULT TO W REG
	
        ;; WRITE W REG TO THE SHADOW AY CHIP REG 7 
        ;; ==> AY_R7_ENABLE_ABC_NOISE_IO
        movwf AY_R7_ENABLE_ABC_NOISE_IO


	movff AY_R7_ENABLE_ABC_NOISE_IO, Fx_TEMP
	bsf  Fx_TEMP, BIT1_TONE_B_ON_OFF	
	
		;; SEND DATA TO AY REG 7
		movlw AY_CHIP_ADR_07_TONE_NOISE_ON_OFF	; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 7
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
        
		;; --------------[SEND DATA TO AY REG 7]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================

				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_005
	;; 0x05 => On  Tone C

	;; USER SETTING FILTER 
	;; MIDI TONE ON FILTER 
	;; OR ==> AY_R7_ENABLE_ABC_NOISE_IO
	
	
	;; USER SETTING => TONE A|B|C ON/OFF [FILTER]:
	;; move USER SWITCH settings to W REG
	movf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, W 
			;; ABC TONE + NOISE ON/OFF USER SWITCH
			;; USER CLEAR or SET A BIT BY SWITCHING
			;; TONE/NOISE A/B/C 

	;; MIDI EVENT FILTER:
	;; *OR* [AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH] WITH [AY_TONE_ABC_MIDI_ON_OFF]
	;; MEMO: => MIDI EVENTS SET OR CLEAR BITS @ AY_TONE_ABC_MIDI_ON_OFF
	;; MEMO: => CLEAR BIT = [TONE / NOISE] *ON*
	 
	
	iorwf AY_TONE_ABC_MIDI_ON_OFF, W	
		;; OR AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH
		;; WITH AY_TONE_ABC_MIDI_ON_OFF
		;; NOW JUST THE RIGHT TONE/NOISE IS SWITCHED
		;; MOVE THE RESULT TO W REG
	
        ;; WRITE W REG TO THE SHADOW AY CHIP REG 7 
        ;; ==> AY_R7_ENABLE_ABC_NOISE_IO
        movwf AY_R7_ENABLE_ABC_NOISE_IO

	;; On  Tone C
	movff AY_R7_ENABLE_ABC_NOISE_IO, Fx_TEMP
	bcf  Fx_TEMP, BIT2_TONE_C_ON_OFF
	
		;; SEND DATA TO AY REG 7
		movlw AY_CHIP_ADR_07_TONE_NOISE_ON_OFF	; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 7
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
        
		;; --------------[SEND DATA TO AY REG 7]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================	
		
				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_006
	;; 0x06 => Off Tone C

	;; USER SETTING FILTER 
	;; MIDI TONE ON FILTER 
	;; OR ==> AY_R7_ENABLE_ABC_NOISE_IO
	
	
	;; USER SETTING => TONE A|B|C ON/OFF [FILTER]:
	;; move USER SWITCH settings to W REG
	movf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, W 
			;; ABC TONE + NOISE ON/OFF USER SWITCH
			;; USER CLEAR or SET A BIT BY SWITCHING
			;; TONE/NOISE A/B/C 

	;; MIDI EVENT FILTER:
	;; *OR* [AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH] WITH [AY_TONE_ABC_MIDI_ON_OFF]
	;; MEMO: => MIDI EVENTS SET OR CLEAR BITS @ AY_TONE_ABC_MIDI_ON_OFF
	;; MEMO: => CLEAR BIT = [TONE / NOISE] *ON*
	 
	
	iorwf AY_TONE_ABC_MIDI_ON_OFF, W	
		;; OR AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH
		;; WITH AY_TONE_ABC_MIDI_ON_OFF
		;; NOW JUST THE RIGHT TONE/NOISE IS SWITCHED
		;; MOVE THE RESULT TO W REG
	
        ;; WRITE W REG TO THE SHADOW AY CHIP REG 7 
        ;; ==> AY_R7_ENABLE_ABC_NOISE_IO
        movwf AY_R7_ENABLE_ABC_NOISE_IO

	;; Off Tone C
	movff AY_R7_ENABLE_ABC_NOISE_IO, Fx_TEMP
	bsf  Fx_TEMP, BIT2_TONE_C_ON_OFF
	
		;; SEND DATA TO AY REG 7
		movlw AY_CHIP_ADR_07_TONE_NOISE_ON_OFF	; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 7
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
        
		;; --------------[SEND DATA TO AY REG 7]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			
				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION


FX_FUNC_007
	;; PLUS Tune A	[Data: 0-255]
	;; Fx Call for Tune +
	
	;; TUNE COARSE A|B|C => TUNE_COARSE_TEMP
	;; TUNE FINE   A|B|C => TUNE_FINE_TEMP
	movff AY_TONE_A_TUNE_COARSE_MIDI, TUNE_COARSE_TEMP
	movff AY_TONE_A_TUNE_FINE_MIDI, TUNE_FINE_TEMP

	
	;; [ADD TUNE +] TUNE [PLUS] CURREND Fx DATA BYTE
	call FX_TUNE_PLUS

	;; TUNE_COARSE_TEMP  => TUNE COARSE A|B|C
	;; TUNE_FINE_TEMP    => TUNE FINE   A|B|C
	movff TUNE_COARSE_TEMP, AY_TONE_A_TUNE_COARSE_MIDI
	movff TUNE_FINE_TEMP, AY_TONE_A_TUNE_FINE_MIDI

		;; SEND TUNE A TO AY CHIP
		movlw AY_CHIP_ADR_01_TONE_A_FREQUENCY_PERIOD_COARSE; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 			; ADDRESS FOR => AY_SR_Write
	
		movf  AY_TONE_A_TUNE_COARSE_MIDI, W	; DATA FOR AY REG X
		movwf AY_REG_DATA 			; FOR => AY_SR_Write (WRITE DATA TO AY REG)
	
			call LABEL_AY_SR_Write		; WRITING 8BIT DATA 
						;(AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE	

		movlw AY_CHIP_ADR_00_TONE_A_FREQUENCY_FINE; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 			; ADDRESS FOR => AY_SR_Write

		movf  AY_TONE_A_TUNE_FINE_MIDI, W	; DATA FOR AY REG X
		movwf AY_REG_DATA 			; FOR => AY_SR_Write (WRITE DATA TO AY REG)
	
			call LABEL_AY_SR_Write		; WRITING 8BIT DATA 
						;; (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE

	
				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_008
	;; MINUS Tune A	[Data: 0-255]
	;; TUNE COARSE A|B|C => TUNE_COARSE_TEMP
	;; TUNE FINE   A|B|C => TUNE_FINE_TEMP
	movff AY_TONE_A_TUNE_COARSE_MIDI, TUNE_COARSE_TEMP
	movff AY_TONE_A_TUNE_FINE_MIDI, TUNE_FINE_TEMP

	
	;; [TUNE -] Currend TUNE [MINUS] CURREND Fx DATA
	call FX_TUNE_MINUS

	;; TUNE_COARSE_TEMP  => TUNE COARSE A|B|C
	;; TUNE_FINE_TEMP    => TUNE FINE   A|B|C
	movff TUNE_COARSE_TEMP, AY_TONE_A_TUNE_COARSE_MIDI
	movff TUNE_FINE_TEMP, AY_TONE_A_TUNE_FINE_MIDI

		;; SEND TUNE A TO AY CHIP
		movlw AY_CHIP_ADR_01_TONE_A_FREQUENCY_PERIOD_COARSE	; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 			; ADDRESS FOR => AY_SR_Write
	
		movf  AY_TONE_A_TUNE_COARSE_MIDI, W	; DATA FOR AY REG X
		movwf AY_REG_DATA 			; FOR => AY_SR_Write (WRITE DATA TO AY REG)
	
			call LABEL_AY_SR_Write		; WRITING 8BIT DATA 
							;(AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE	

		movlw AY_CHIP_ADR_00_TONE_A_FREQUENCY_FINE ; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 			; ADDRESS FOR => AY_SR_Write

		movf  AY_TONE_A_TUNE_FINE_MIDI, W	; DATA FOR AY REG X
		movwf AY_REG_DATA 			; FOR => AY_SR_Write (WRITE DATA TO AY REG)
	
			call LABEL_AY_SR_Write		; WRITING 8BIT DATA 
							;; (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
	
				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_009
	;; 0x09 => PLUS Tune B	[Data: 0-255]
	;; Fx Call for Tune +
	
	;; TUNE COARSE A|B|C => TUNE_COARSE_TEMP
	;; TUNE FINE   A|B|C => TUNE_FINE_TEMP
	movff AY_TONE_B_TUNE_COARSE_MIDI, TUNE_COARSE_TEMP
	movff AY_TONE_B_TUNE_FINE_MIDI, TUNE_FINE_TEMP

	
	;; [ADD TUNE +] TUNE TEMP - CURREND Fx DATA BYTE
	call FX_TUNE_PLUS

	;; TUNE_COARSE_TEMP  => TUNE COARSE A|B|C
	;; TUNE_FINE_TEMP    => TUNE FINE   A|B|C
	movff TUNE_COARSE_TEMP, AY_TONE_A_TUNE_COARSE_MIDI
	movff TUNE_FINE_TEMP, AY_TONE_B_TUNE_FINE_MIDI

		;; SEND TUNE B TO AY CHIP
		movlw AY_CHIP_ADR_03_TONE_B_FREQUENCY_PERIOD_COARSE	; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 			; ADDRESS FOR => AY_SR_Write
	
		movf  AY_TONE_B_TUNE_COARSE_MIDI, W	; DATA FOR AY REG X
		movwf AY_REG_DATA 			; FOR => AY_SR_Write (WRITE DATA TO AY REG)
	
			call LABEL_AY_SR_Write		; WRITING 8BIT DATA 
						;(AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE	

		movlw AY_CHIP_ADR_02_TONE_B_FREQUENCY_PERIOD_FINE	; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 			; ADDRESS FOR => AY_SR_Write

		movf  AY_TONE_B_TUNE_FINE_MIDI, W	; DATA FOR AY REG X
		movwf AY_REG_DATA 			; FOR => AY_SR_Write (WRITE DATA TO AY REG)
	
			call LABEL_AY_SR_Write		; WRITING 8BIT DATA 
							;; (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_010
	;; 0x0A => MINUS Tune B	[Data: 0-255]
	;; TUNE COARSE A|B|C => TUNE_COARSE_TEMP
	;; TUNE FINE   A|B|C => TUNE_FINE_TEMP
	movff AY_TONE_B_TUNE_COARSE_MIDI, TUNE_COARSE_TEMP
	movff AY_TONE_B_TUNE_FINE_MIDI, TUNE_FINE_TEMP

	
	;; [TUNE -] Currend TUNE [MINUS] CURREND Fx DATA
	call FX_TUNE_MINUS

	;; TUNE_COARSE_TEMP  => TUNE COARSE A|B|C
	;; TUNE_FINE_TEMP    => TUNE FINE   A|B|C
	movff TUNE_COARSE_TEMP, AY_TONE_B_TUNE_COARSE_MIDI
	movff TUNE_FINE_TEMP, AY_TONE_B_TUNE_FINE_MIDI

		;; SEND TUNE A TO AY CHIP
		movlw AY_CHIP_ADR_03_TONE_B_FREQUENCY_PERIOD_COARSE	; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 			; ADDRESS FOR => AY_SR_Write
	
		movf  AY_TONE_B_TUNE_COARSE_MIDI, W	; DATA FOR AY REG X
		movwf AY_REG_DATA 			; FOR => AY_SR_Write (WRITE DATA TO AY REG)
	
			call LABEL_AY_SR_Write		; WRITING 8BIT DATA 
						;(AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE	

		movlw AY_CHIP_ADR_02_TONE_B_FREQUENCY_PERIOD_FINE	; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 			; ADDRESS FOR => AY_SR_Write

		movf  AY_TONE_B_TUNE_FINE_MIDI, W	; DATA FOR AY REG X
		movwf AY_REG_DATA 			; FOR => AY_SR_Write (WRITE DATA TO AY REG)
	
			call LABEL_AY_SR_Write		; WRITING 8BIT DATA 
							;; (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
 
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_011
	;; 0x0B => +Tune C	[Data: 0-255]
	;; Fx Call for Tune +
	
	;; TUNE COARSE A|B|C => TUNE_COARSE_TEMP
	;; TUNE FINE   A|B|C => TUNE_FINE_TEMP
	movff AY_TONE_C_TUNE_COARSE_MIDI, TUNE_COARSE_TEMP
	movff AY_TONE_C_TUNE_FINE_MIDI, TUNE_FINE_TEMP

	
	;; [ADD TUNE +] TUNE TEMP - CURREND Fx DATA BYTE
	call FX_TUNE_PLUS

	;; TUNE_COARSE_TEMP  => TUNE COARSE A|B|C
	;; TUNE_FINE_TEMP    => TUNE FINE   A|B|C
	movff TUNE_COARSE_TEMP, AY_TONE_A_TUNE_COARSE_MIDI
	movff TUNE_FINE_TEMP, AY_TONE_C_TUNE_FINE_MIDI

		;; SEND TUNE C TO AY CHIP
		movlw AY_CHIP_ADR_05_TONE_C_TONE_PERIOD_COARSE	; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 			; ADDRESS FOR => AY_SR_Write
	
		movf  AY_TONE_C_TUNE_COARSE_MIDI, W	; DATA FOR AY REG X
		movwf AY_REG_DATA 			; FOR => AY_SR_Write (WRITE DATA TO AY REG)
	
			call LABEL_AY_SR_Write		; WRITING 8BIT DATA 
						;(AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE	

		movlw AY_CHIP_ADR_04_TONE_C_FREQUENCY_PERIOD_FINE	; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 			; ADDRESS FOR => AY_SR_Write

		movf  AY_TONE_C_TUNE_FINE_MIDI, W	; DATA FOR AY REG X
		movwf AY_REG_DATA 			; FOR => AY_SR_Write (WRITE DATA TO AY REG)
	
			call LABEL_AY_SR_Write		; WRITING 8BIT DATA 
						;; (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE

				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_012
	;; 0x0C => -Tune C	[Data: 0-255]
	;; TUNE COARSE A|B|C => TUNE_COARSE_TEMP
	;; TUNE FINE   A|B|C => TUNE_FINE_TEMP
	movff AY_TONE_C_TUNE_COARSE_MIDI, TUNE_COARSE_TEMP
	movff AY_TONE_C_TUNE_FINE_MIDI, TUNE_FINE_TEMP

	
	;; [TUNE -] Currend TUNE [MINUS] CURREND Fx DATA
	call FX_TUNE_MINUS

	;; TUNE_COARSE_TEMP  => TUNE COARSE A|B|C
	;; TUNE_FINE_TEMP    => TUNE FINE   A|B|C
	movff TUNE_COARSE_TEMP, AY_TONE_C_TUNE_COARSE_MIDI
	movff TUNE_FINE_TEMP, AY_TONE_C_TUNE_FINE_MIDI

		;; SEND TUNE A TO AY CHIP
		movlw AY_CHIP_ADR_05_TONE_C_TONE_PERIOD_COARSE	; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 			; ADDRESS FOR => AY_SR_Write
	
		movf  AY_TONE_C_TUNE_COARSE_MIDI, W	; DATA FOR AY REG X
		movwf AY_REG_DATA 			; FOR => AY_SR_Write (WRITE DATA TO AY REG)
	
			call LABEL_AY_SR_Write		; WRITING 8BIT DATA 
						;(AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE	

		movlw AY_CHIP_ADR_04_TONE_C_FREQUENCY_PERIOD_FINE	; ADRESS => AY REGISTER
		movwf AY_REG_ADDRESS 			; ADDRESS FOR => AY_SR_Write

		movf  AY_TONE_C_TUNE_FINE_MIDI, W	; DATA FOR AY REG X
		movwf AY_REG_DATA 			; FOR => AY_SR_Write (WRITE DATA TO AY REG)
	
			call LABEL_AY_SR_Write		; WRITING 8BIT DATA 
							;; (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE

			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_013
	;; 0x0D FUNCTION WRITE => Vol.A	[Data: 0-15]
	;; FORMAT VALUE TO 0-15 (DELETE values > 15)
	
	movlw VOLUME_ABC_MAX_VALUE ;; B'00001111'
	cpfsgt FX_DATA_SESSION_CURREND
		;; =< 15
		rgoto FX_FUNC_013_WRITE_TO_AY_CHIP
	 	
	 	;; >15	
	 	;; FORMAT VALUE TO 0-15 (DELETE values > 15)
	 	movlw B'00001111'
	 	movwf FX_DATA_SESSION_CURREND
		
FX_FUNC_013_WRITE_TO_AY_CHIP		
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_08_TONE_A_VOLUME_ENV_MODE_ON_OFF ; REG ADRESS 	=> AY REGISTER 
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	;; WRITE VOL Tone A
        	movff FX_DATA_SESSION_CURREND, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_014
	;; 0x0E => Vol.B	[Data: 0-15]
	movlw VOLUME_ABC_MAX_VALUE  ;; B'00001111'
	cpfsgt FX_DATA_SESSION_CURREND
		;; <= 15
		rgoto FX_FUNC_014_WRITE_TO_AY_CHIP
	 	
	 	;; > 15	
	 	;; FORMAT VALUE TO 0-15 (DELETE values > 15)
	 	movlw B'00001111'
	 	movwf FX_DATA_SESSION_CURREND
		
FX_FUNC_014_WRITE_TO_AY_CHIP		
		;; SEND DATA TO AY REG 9
		movlw AY_CHIP_ADR_09_TONE_B_VOLUME_ENV_MODE_ON_OFF	; REG ADRESS 	=> AY REGISTER 
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	;; WRITE VOL Tone B
        	movff FX_DATA_SESSION_CURREND, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_015
	;; 0x0F => Vol.C	[Data: 0-15]
	movlw VOLUME_ABC_MAX_VALUE  ;; B'00001111'
	cpfsgt FX_DATA_SESSION_CURREND
		;; <=15
		rgoto FX_FUNC_015_WRITE_TO_AY_CHIP
	 	
	 	;; >15	
	 	;; FORMAT VALUE TO 0-15 (DELETE values > 15)
	 	movlw B'00001111'
	 	movwf FX_DATA_SESSION_CURREND
		
FX_FUNC_015_WRITE_TO_AY_CHIP		
		;; SEND DATA TO AY REG 10
		movlw AY_CHIP_ADR_10_TONE_C_VOLUME_ENV_MODE_ON_OFF	; REG ADRESS 	=> AY REGISTER 
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff FX_DATA_SESSION_CURREND, AY_REG_DATA
        	;; WRITE VOL Tone C
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_016
	;; On  Noise A
	;; USER SETTING FILTER 
	;; MIDI TONE ON FILTER 
	;; OR ==> AY_R7_ENABLE_ABC_NOISE_IO
	
	;; USER SETTING => TONE A|B|C ON/OFF [FILTER]:
	;; move USER SWITCH settings to W REG
	movf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, W 
			;; ABC TONE + NOISE ON/OFF USER SWITCH
			;; USER CLEAR or SET A BIT BY SWITCHING
			;; TONE/NOISE A/B/C 

	;; MIDI EVENT FILTER:
	;; *OR* [AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH] WITH [AY_TONE_ABC_MIDI_ON_OFF]
	;; MEMO: => MIDI EVENTS SET OR CLEAR BITS @ AY_TONE_ABC_MIDI_ON_OFF
	;; MEMO: => CLEAR BIT = [TONE / NOISE] *ON*
	 
	
	iorwf AY_TONE_ABC_MIDI_ON_OFF, W	
		;; OR AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH
		;; WITH AY_TONE_ABC_MIDI_ON_OFF
		;; NOW JUST THE RIGHT TONE/NOISE IS SWITCHED
		;; MOVE THE RESULT TO W REG
	
        ;; WRITE W REG TO THE SHADOW AY CHIP REG 7 
        ;; ==> AY_R7_ENABLE_ABC_NOISE_IO
        movwf AY_R7_ENABLE_ABC_NOISE_IO
	movff AY_R7_ENABLE_ABC_NOISE_IO, Fx_TEMP
	
	;; On  Noise A
	bcf  Fx_TEMP, BIT3_NOISE_A_ON_OFF
		
		;; ==========================================================================================
		;; SEND DATA TO AY REG 7
		movlw AY_CHIP_ADR_07_TONE_NOISE_ON_OFF	; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 7
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
		;; --------------[SEND DATA TO AY REG 7]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_017
	;; Off Noise A
	;; USER SETTING FILTER 
	;; MIDI TONE ON FILTER 
	;; OR ==> AY_R7_ENABLE_ABC_NOISE_IO
	
	;; USER SETTING => TONE A|B|C ON/OFF [FILTER]:
	;; move USER SWITCH settings to W REG
	movf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, W 
			;; ABC TONE + NOISE ON/OFF USER SWITCH
			;; USER CLEAR or SET A BIT BY SWITCHING
			;; TONE/NOISE A/B/C 

	;; MIDI EVENT FILTER:
	;; *OR* [AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH] WITH [AY_TONE_ABC_MIDI_ON_OFF]
	;; MEMO: => MIDI EVENTS SET OR CLEAR BITS @ AY_TONE_ABC_MIDI_ON_OFF
	;; MEMO: => CLEAR BIT = [TONE / NOISE] *ON*
	 
	
	iorwf AY_TONE_ABC_MIDI_ON_OFF, W	
		;; OR AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH
		;; WITH AY_TONE_ABC_MIDI_ON_OFF
		;; NOW JUST THE RIGHT TONE/NOISE IS SWITCHED
		;; MOVE THE RESULT TO W REG
	
        ;; WRITE W REG TO THE SHADOW AY CHIP REG 7 
        ;; ==> AY_R7_ENABLE_ABC_NOISE_IO
        movwf AY_R7_ENABLE_ABC_NOISE_IO
	movff AY_R7_ENABLE_ABC_NOISE_IO, Fx_TEMP
	
	;; OFF  Noise A
	bsf  Fx_TEMP, BIT3_NOISE_A_ON_OFF
		;; ==========================================================================================	
		;; SEND DATA TO AY REG 7
		movlw AY_CHIP_ADR_07_TONE_NOISE_ON_OFF	; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 7
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
		;; --------------[SEND DATA TO AY REG 7]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_018
	;; On  Noise B
	;; USER SETTING FILTER 
	;; MIDI TONE ON FILTER 
	;; OR ==> AY_R7_ENABLE_ABC_NOISE_IO
	
	;; USER SETTING => TONE A|B|C ON/OFF [FILTER]:
	;; move USER SWITCH settings to W REG
	movf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, W 
			;; ABC TONE + NOISE ON/OFF USER SWITCH
			;; USER CLEAR or SET A BIT BY SWITCHING
			;; TONE/NOISE A/B/C 

	;; MIDI EVENT FILTER:
	;; *OR* [AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH] WITH [AY_TONE_ABC_MIDI_ON_OFF]
	;; MEMO: => MIDI EVENTS SET OR CLEAR BITS @ AY_TONE_ABC_MIDI_ON_OFF
	;; MEMO: => CLEAR BIT = [TONE / NOISE] *ON*
	 
	
	iorwf AY_TONE_ABC_MIDI_ON_OFF, W	
		;; OR AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH
		;; WITH AY_TONE_ABC_MIDI_ON_OFF
		;; NOW JUST THE RIGHT TONE/NOISE IS SWITCHED
		;; MOVE THE RESULT TO W REG
	
        ;; WRITE W REG TO THE SHADOW AY CHIP REG 7 
        ;; ==> AY_R7_ENABLE_ABC_NOISE_IO
        movwf AY_R7_ENABLE_ABC_NOISE_IO
	movff AY_R7_ENABLE_ABC_NOISE_IO, Fx_TEMP

	;; On  Noise B
	bcf  Fx_TEMP, BIT4_NOISE_B_ON_OFF	
		;; ==========================================================================================	
		;; SEND DATA TO AY REG 7
		movlw AY_CHIP_ADR_07_TONE_NOISE_ON_OFF	; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 7
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
		;; --------------[SEND DATA TO AY REG 7]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================

			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_019
	;; Off Noise B

	;; USER SETTING FILTER 
	;; MIDI TONE ON FILTER 
	;; OR ==> AY_R7_ENABLE_ABC_NOISE_IO
	
	;; USER SETTING => TONE A|B|C ON/OFF [FILTER]:
	;; move USER SWITCH settings to W REG
	movf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, W 
			;; ABC TONE + NOISE ON/OFF USER SWITCH
			;; USER CLEAR or SET A BIT BY SWITCHING
			;; TONE/NOISE A/B/C 

	;; MIDI EVENT FILTER:
	;; *OR* [AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH] WITH [AY_TONE_ABC_MIDI_ON_OFF]
	;; MEMO: => MIDI EVENTS SET OR CLEAR BITS @ AY_TONE_ABC_MIDI_ON_OFF
	;; MEMO: => CLEAR BIT = [TONE / NOISE] *ON*
	 
	
	iorwf AY_TONE_ABC_MIDI_ON_OFF, W	
		;; OR AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH
		;; WITH AY_TONE_ABC_MIDI_ON_OFF
		;; NOW JUST THE RIGHT TONE/NOISE IS SWITCHED
		;; MOVE THE RESULT TO W REG
	
        ;; WRITE W REG TO THE SHADOW AY CHIP REG 7 
        ;; ==> AY_R7_ENABLE_ABC_NOISE_IO
        movwf AY_R7_ENABLE_ABC_NOISE_IO
	movff AY_R7_ENABLE_ABC_NOISE_IO, Fx_TEMP
	 
	;; Off Noise B
	bsf  Fx_TEMP, BIT4_NOISE_B_ON_OFF	

		;; ==========================================================================================
		;; SEND DATA TO AY REG 7
		movlw AY_CHIP_ADR_07_TONE_NOISE_ON_OFF ; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 7
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
		;; --------------[SEND DATA TO AY REG 7]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================

			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_020
	;; On  Noise C 
	
	;; USER SETTING FILTER 
	;; MIDI TONE ON FILTER 
	;; OR ==> AY_R7_ENABLE_ABC_NOISE_IO
	
	;; USER SETTING => TONE A|B|C ON/OFF [FILTER]:
	;; move USER SWITCH settings to W REG
	movf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, W 
			;; ABC TONE + NOISE ON/OFF USER SWITCH
			;; USER CLEAR or SET A BIT BY SWITCHING
			;; TONE/NOISE A/B/C 

	;; MIDI EVENT FILTER:
	;; *OR* [AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH] WITH [AY_TONE_ABC_MIDI_ON_OFF]
	;; MEMO: => MIDI EVENTS SET OR CLEAR BITS @ AY_TONE_ABC_MIDI_ON_OFF
	;; MEMO: => CLEAR BIT = [TONE / NOISE] *ON*
	 
	iorwf AY_TONE_ABC_MIDI_ON_OFF, W	
		;; OR AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH
		;; WITH AY_TONE_ABC_MIDI_ON_OFF
		;; NOW JUST THE RIGHT TONE/NOISE IS SWITCHED
		;; MOVE THE RESULT TO W REG
	
        ;; WRITE W REG TO THE SHADOW AY CHIP REG 7 
        ;; ==> AY_R7_ENABLE_ABC_NOISE_IO
        movwf AY_R7_ENABLE_ABC_NOISE_IO
	movff AY_R7_ENABLE_ABC_NOISE_IO, Fx_TEMP
	
	;; On  Noise C 
	bcf  Fx_TEMP, BIT5_NOISE_C_ON_OFF	
	
		;; ==========================================================================================
		;; SEND DATA TO AY REG 7
		movlw AY_CHIP_ADR_07_TONE_NOISE_ON_OFF ; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 7
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
		;; --------------[SEND DATA TO AY REG 7]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_021
	;; Off Noise C
	
	;; USER SETTING FILTER 
	;; MIDI TONE ON FILTER 
	;; OR ==> AY_R7_ENABLE_ABC_NOISE_IO
	
	;; USER SETTING => TONE A|B|C ON/OFF [FILTER]:
	;; move USER SWITCH settings to W REG
	movf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, W 
			;; ABC TONE + NOISE ON/OFF USER SWITCH
			;; USER CLEAR or SET A BIT BY SWITCHING
			;; TONE/NOISE A/B/C 

	;; MIDI EVENT FILTER:
	;; *OR* [AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH] WITH [AY_TONE_ABC_MIDI_ON_OFF]
	;; MEMO: => MIDI EVENTS SET OR CLEAR BITS @ AY_TONE_ABC_MIDI_ON_OFF
	;; MEMO: => CLEAR BIT = [TONE / NOISE] *ON*
	 
	iorwf AY_TONE_ABC_MIDI_ON_OFF, W	
		;; OR AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH
		;; WITH AY_TONE_ABC_MIDI_ON_OFF
		;; NOW JUST THE RIGHT TONE/NOISE IS SWITCHED
		;; MOVE THE RESULT TO W REG
	
        ;; WRITE W REG TO THE SHADOW AY CHIP REG 7 
        ;; ==> AY_R7_ENABLE_ABC_NOISE_IO
        movwf AY_R7_ENABLE_ABC_NOISE_IO
	movff AY_R7_ENABLE_ABC_NOISE_IO, Fx_TEMP
	
	;; Off Noise C
	bsf  Fx_TEMP, BIT5_NOISE_C_ON_OFF	

		;; ==========================================================================================	
		;; SEND DATA TO AY REG 7
		movlw 0x07			; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 7
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
		;; --------------[SEND DATA TO AY REG 7]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_022
	;;  Noise Frq.	[Data: 0-31]
	movlw NOISE_PERIOD_MAX_VALUE ;; NOISE PERIOD MAX VALUE = B'00011111', 31 dec, 01F hex
	cpfsgt FX_DATA_SESSION_CURREND
		;; OK, GO ON
		;; FX_DATA_SESSION_CURREND is not out of range [0-31]
		rgoto FX_FUNC_022_GO_ON
		
		;; write noise period max MAX VALUE [fX DATA WAS OUT OF RAGE] 
		movlw	NOISE_PERIOD_MAX_VALUE
		movwf	FX_DATA_SESSION_CURREND
		
FX_FUNC_022_GO_ON		
	;; REVERSE NOISE PERIOD VALUE 
	;; [THE NOISE PERIOD VALUE IS REVERSED ON LCD ALSO]
	;; [FX DATA]  0 => [AY CHIP REG 6 REVERSE VALUE] = 31
	;; [FX DATA]  1 => [AY CHIP REG 6 REVERSE VALUE] = 30
	;; [FX DATA]  2 => [AY CHIP REG 6 REVERSE VALUE] = 29
	;; [FX DATA]  3 => [AY CHIP REG 6 REVERSE VALUE] = 28
	;; ...
	;; [FX DATA] 31 => [AY CHIP REG 6REVERSE VALUE]  = 0
	movlw NOISE_PERIOD_MAX_VALUE  ;; 31
	movwf AY_R6_NOISE_PERIOD_REVERSE_VALUE
	movf FX_DATA_SESSION_CURREND, W
	subwf AY_R6_NOISE_PERIOD_REVERSE_VALUE, F
		;; ==========================================================================================
		;; SEND DATA TO AY REG 6
		movlw AY_CHIP_ADR_06_NOISE_PERIOD ; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 6
		movwf AY_REG_ADDRESS 		  ; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff AY_R6_NOISE_PERIOD_REVERSE_VALUE, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_023
	;; ENV ON A
	
	;; get currend envelope shape
	call ENVELOPE_TRANSLATOR_CALL
			
	movff AY_R8_CH_A_AMPLITUDE, Fx_TEMP
	;; SET bit 4 => ENV MODE ON A
	bsf	Fx_TEMP, BIT4_ENVELOPE_ON_OFF

		;; ==========================================================================================
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_08_TONE_A_VOLUME_ENV_MODE_ON_OFF	; SELECT ADRESS => SET REGISTER OF AY CHIP
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
		;;; -------------[WRITE ENVELOPE SHAPE/CYCLE]------------------
		;; SEND DATA TO AY REG 13 (ENV. SHAPE Cycle REG)
		;; SET AY REGISTER
		movlw  AY_CHIP_ADR_13_ENVELOPE_SHAPE_AND_CYCLE	
		movwf  AY_REG_ADDRESS 		
		;; WRITE GLOBAL [A|B|C] ENVELOPE SHAPE DATA TO AY CHIP
		movff  AY_R13_ENV_SHAPE_CYCLE, AY_REG_DATA
		;; --------------[SEND DATA TO AY REG 13]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
	
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_024
	;; ENV OFF A
	movff AY_R8_CH_A_AMPLITUDE, Fx_TEMP
	;; SET bit 4 => ENV MODE ON A
	bcf	Fx_TEMP, BIT4_ENVELOPE_ON_OFF

		;; ==========================================================================================
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_08_TONE_A_VOLUME_ENV_MODE_ON_OFF	; SET AY CHIP REGISTER 	
		movwf AY_REG_ADDRESS 		; => AY_SR_Write
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION


FX_FUNC_025
	;; ENV ON B

	;; get currend envelope shape
	call ENVELOPE_TRANSLATOR_CALL

	movff AY_R9_CH_B_AMPLITUDE, Fx_TEMP
	;; SET bit 4 => ENV MODE ON B
	bsf	Fx_TEMP, BIT4_ENVELOPE_ON_OFF
	
		;; ==========================================================================================
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_09_TONE_B_VOLUME_ENV_MODE_ON_OFF ; SELECT ADRESS => SET REGISTER OF AY CHIP
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
		;;; -------------[WRITE ENVELOPE SHAPE/CYCLE]------------------
		;; SEND DATA TO AY REG 13 (ENV. SHAPE Cycle REG)
		;; SET AY REGISTER
		movlw  AY_CHIP_ADR_13_ENVELOPE_SHAPE_AND_CYCLE	
		movwf  AY_REG_ADDRESS 		
		;; WRITE GLOBAL [A|B|C] ENVELOPE SHAPE DATA TO AY CHIP
		movff  AY_R13_ENV_SHAPE_CYCLE, AY_REG_DATA
		;; --------------[SEND DATA TO AY REG 13]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_026
	;; ENV OFF B
	movff AY_R9_CH_B_AMPLITUDE, Fx_TEMP
	;; SET bit 4 => ENV MODE ON A
	bcf	Fx_TEMP, BIT4_ENVELOPE_ON_OFF
	
		;; ==========================================================================================
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_09_TONE_B_VOLUME_ENV_MODE_ON_OFF  ; SET AY CHIP REGISTER 	
		movwf AY_REG_ADDRESS 		; => AY_SR_Write
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION


FX_FUNC_027
	;; ENV ON C

	;; get currend envelope shape
	call ENVELOPE_TRANSLATOR_CALL	

	movff AY_R10_CH_C_AMPLITUDE, Fx_TEMP
	;; SET bit 4 => ENV MODE ON C
	bsf	Fx_TEMP, BIT4_ENVELOPE_ON_OFF
	
		;; ==========================================================================================
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_10_TONE_C_VOLUME_ENV_MODE_ON_OFF  ; SELECT ADRESS => SET REGISTER OF AY CHIP
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
		;;; -------------[WRITE ENVELOPE SHAPE/CYCLE]------------------
		;; SEND DATA TO AY REG 13 (ENV. SHAPE Cycle REG)
		;; SET AY REGISTER
		movlw  AY_CHIP_ADR_13_ENVELOPE_SHAPE_AND_CYCLE	
		movwf  AY_REG_ADDRESS 		
		;; WRITE GLOBAL [A|B|C] ENVELOPE SHAPE DATA TO AY CHIP
		movff  AY_R13_ENV_SHAPE_CYCLE, AY_REG_DATA
		;; --------------[SEND DATA TO AY REG 13]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_028
	;; ENV OFF C
	movff AY_R10_CH_C_AMPLITUDE, Fx_TEMP
	;; SET bit 4 => ENV MODE ON A
	bcf	Fx_TEMP, BIT4_ENVELOPE_ON_OFF
	
		;; ==========================================================================================
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_10_TONE_C_VOLUME_ENV_MODE_ON_OFF ; SET AY CHIP REGISTER 	
		movwf AY_REG_ADDRESS 		; => AY_SR_Write
        	;; COPY DATA
        	movff Fx_TEMP, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION




FX_FUNC_029
	;; WRITE ENV Shape/CYCLE	[Data: 0-15]
	movlw ENVELOPE_SHAPE_MAX_VALUE_8 ; ENVELOPE SHAPE 1-8 (8 = Max Value)
	cpfsgt FX_DATA_SESSION_CURREND
		
		rgoto FX_FUNC_014_WRITE_TO_AY_CHIP
	 		
	 	;; FORMAT VALUE TO 0-15 (DELETE values > 15)
	 	movlw ENVELOPE_SHAPE_MAX_VALUE_8
	 	movwf FX_DATA_SESSION_CURREND


		
FX_FUNC_029_WRITE_TO_AY_CHIP	
		;; SEND DATA TO AY CHIP - ENV SHAPE CYCLE
		movlw AY_CHIP_ADR_13_ENVELOPE_SHAPE_AND_CYCLE  ; REG ADRESS 	=> AY REGISTER 13
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	
		;; ==========================================================================================
        	;; COPY DATA
        	movff FX_DATA_SESSION_CURREND, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION


FX_FUNC_030
	;; ENV PERIOD Fine	[Data: 0-255]
		;; ==========================================================================================
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_11_GLOBAL_ENVELOPE_FINE ; REG ADRESS 	=> AY REGISTER 11
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff FX_DATA_SESSION_CURREND, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_031
	;; WRITE ENV PERIOD Coarse	[Data: 0-255]
		;; ==========================================================================================
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_12_GLOBAL_ENVELOPE_COARSE ; REG ADRESS 	=> AY REGISTER 12
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff FX_DATA_SESSION_CURREND, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_032
	;; WRITE NOTE ON #1 ==> Tone A
	
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_00_TONE_A_FREQUENCY_FINE	; REG ADRESS 	=> AY REGISTER 0
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	

		;; ==========================================================================================
        	;; COPY DATA
        	movff Fx_Note_On_1_tune_fine, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
		movlw AY_CHIP_ADR_01_TONE_A_FREQUENCY_PERIOD_COARSE	; REG ADRESS 	=> AY REGISTER 12
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_Note_On_1_tune_coarse, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_033
	;; WRITE NOTE ON #1 ==> Tone B
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_02_TONE_B_FREQUENCY_PERIOD_FINE  ; REG ADRESS 	=> AY REGISTER 12
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
		;; ==========================================================================================        	
        	;; COPY DATA
        	movff Fx_Note_On_1_tune_fine, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
		movlw AY_CHIP_ADR_03_TONE_B_FREQUENCY_PERIOD_COARSE ; REG ADRESS 	=> AY REGISTER 12
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_Note_On_1_tune_coarse, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_034
	;; WRITE NOTE ON #1 ==> Tone C
		
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_04_TONE_C_FREQUENCY_PERIOD_FINE ; REG ADRESS 	=> AY REGISTER
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
		
		;; ==========================================================================================        	
        	;; COPY DATA
        	movff Fx_Note_On_1_tune_fine, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
		movlw AY_CHIP_ADR_05_TONE_C_TONE_PERIOD_COARSE ; REG ADRESS 	=> AY REGISTER
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_Note_On_1_tune_coarse, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_035
	;; 0x1F => WRITE NOTE ON #2 ==> Tone A
	;; SEND DATA TO AY CHIP
	movlw AY_CHIP_ADR_00_TONE_A_FREQUENCY_FINE  ; REG ADRESS 	=> AY REGISTER 
	movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
		
		;; ==========================================================================================        	
        	;; COPY DATA
        	movff Fx_Note_On_2_tune_fine, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
		movlw AY_CHIP_ADR_01_TONE_A_FREQUENCY_PERIOD_COARSE  ; REG ADRESS 	=> AY REGISTER 
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_Note_On_2_tune_coarse, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
				rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_036
	;; 0x20 => WRITE NOTE ON #2 ==> Tone B
	;; SEND DATA TO AY CHIP
	movlw AY_CHIP_ADR_02_TONE_B_FREQUENCY_PERIOD_FINE  ; REG ADRESS 	=> AY REGISTER 
	movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
		;; ==========================================================================================
        	;; COPY DATA
        	movff Fx_Note_On_2_tune_fine, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
		movlw AY_CHIP_ADR_03_TONE_B_FREQUENCY_PERIOD_COARSE  ; REG ADRESS 	=> AY REGISTER 
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_Note_On_2_tune_coarse, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
		
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_037
	;; 0x21 => WRITE NOTE ON #2 ==> Tone C
	;; SEND DATA TO AY CHIP
		
		;; ==========================================================================================
		movlw AY_CHIP_ADR_04_TONE_C_FREQUENCY_PERIOD_FINE  ; REG ADRESS 	=> AY REGISTER 
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
		;; COPY DATA
        	movff Fx_Note_On_2_tune_fine, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
		movlw AY_CHIP_ADR_05_TONE_C_TONE_PERIOD_COARSE  ; REG ADRESS 	=> AY REGISTER 
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_Note_On_2_tune_coarse, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================

			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_038
	;; WRITE NOTE ON #3 ==> Tone A
		;; ==========================================================================================
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_00_TONE_A_FREQUENCY_FINE  ; REG ADRESS 	=> AY REGISTER 
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_Note_On_3_tune_fine, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
		movlw AY_CHIP_ADR_01_TONE_A_FREQUENCY_PERIOD_COARSE  ; REG ADRESS 	=> AY REGISTER 
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_Note_On_3_tune_coarse, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_039
	;; 0x23 => WRITE NOTE ON #3 ==> Tone B
		;; ==========================================================================================
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_02_TONE_B_FREQUENCY_PERIOD_FINE  ; REG ADRESS 	=> AY REGISTER 
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_Note_On_3_tune_fine, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
		movlw AY_CHIP_ADR_03_TONE_B_FREQUENCY_PERIOD_COARSE  ; REG ADRESS 	=> AY REGISTER 
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_Note_On_3_tune_coarse, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION

FX_FUNC_040
	;; WRITE NOTE ON #3 ==> Tone C
		;; ==========================================================================================
		;; SEND DATA TO AY CHIP
		movlw AY_CHIP_ADR_04_TONE_C_FREQUENCY_PERIOD_FINE  ; REG ADRESS 	=> AY REGISTER 
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_Note_On_3_tune_fine, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
		movlw AY_CHIP_ADR_05_TONE_C_TONE_PERIOD_COARSE ; REG ADRESS 	=> AY REGISTER 
		movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        	;; COPY DATA
        	movff Fx_Note_On_3_tune_coarse, AY_REG_DATA
		;; --------------[SEND DATA TO AY CHIP]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			rgoto INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION


FX_FUNC_041
	;; GOTO Fx POS 	[Data: 0-31]
	;; set FX_POS_CURREND
	movff FX_DATA_SESSION_CURREND, FX_POS_SESSION_CURREND
	
	;; IS THE GOTO POS VALID?
	movlw FX_POS_MAX_31 
	cpfsgt FX_POS_SESSION_CURREND
		;; FX POS IS OK
		rgoto FX_FUNC_041_RESET_FX_TIMER
	 		
	 	;; FX_POS IS OUT OF RANG 
	 	;; SET FX POS TO MAX = POS 31
	 	movlw FX_POS_MAX_31 
	 	movwf FX_POS_SESSION_CURREND

	
FX_FUNC_041_RESET_FX_TIMER			
	;; RESET FX Timer for the next Fx Pos Cycle
	clrf 	FX_TIMER	
			;; GO AND LOAD NEW POS
			rgoto FX_PIPELINE_EXIT

;; -------------------------------------------------------------------------
;; -------------------------------------------------------------------------
;; -------------------------------------------------------------------------


INC_FX_POS_FOR_THE_NEXT_FX_EXECUTION
	;; Fx Pos +1
	
	;; First: RESET FX Timer for the next Fx Pos Cycle
	clrf 	FX_TIMER	
	
	;; INC Fx POS	
	;; +1 to Fx Pos
	incf 	FX_POS_SESSION_CURREND, F
		;; chck if Fx Tab Pos > 31
		movlw FX_POS_MAX_31  ;; 31
		cpfsgt FX_POS_SESSION_CURREND
			;; NO - OK QUIT HERE
			goto FX_PIPELINE_EXIT

				;; YES - END OF Fx Tab
				;; SET Fx NOP = ON = END OF Fx
				;; SET NOP (NO Fx OPERATION)
				setf 	FX_NOP_ON_OFF	;; SET Fx NOP

	


FX_PIPELINE_EXIT
			;; LOAD NEXT FX DATA for the NEXT CURREND POS /
			;; for the NEXT Fx CYCLE
			call LOAD_FX_SESSION_DATA_CURREND_POS
					;; EXIT
					;; END OF CURREND FX CYCLE			
					return


;; ----------------------------------------------------------------

;; ----------------------------------------------------------------
;; #####################  Fx Calls  ###############################
;; ----------------------------------------------------------------
;; Fx Call to Load Curren Fx date @ POS X

;; LOAD CURREND POS for Fx Pipeline

LOAD_FX_SESSION_DATA_CURREND_POS
	;; used by Fx pipeline 
	;; DO INDIRECT RAM ACCESS via FSR0
	movlw SELECT_Fx_RAM_BANK 	;; SET IDIRECT RAM ACCESS REG FSR0 
	movwf FSR0H			;; RAM BANK: Fx DATA RAM AREA
					
	;; LOAD FX FUNCTION @ Currend SESSION Pos (in the Fx Data Area)	
	movlw RAM_OFFSET_FX_FUNCTION 		 ;; OFFSET @ RAM BANK 2 @ The RAM AREA of FX FUNCTIONS (in the Fx Data Area)	
	addwf FX_POS_SESSION_CURREND, W		 ;; + Fx Tab SESSION Pos (0 to 31 [32])
	movwf FSR0L				 ;; SET CURREND SESSION OFFSET for Fx Function @ current Fx SESSION Pos.
	movff INDF0, FX_FUNCTION_SESSION_CURREND ;; WRITE FX FUNCTION @ currend Session Pos to Working Register

	;; LOAD FX Data Reg. @ Currend SESSION Pos (in the Fx Data Area)	
	movlw RAM_OFFSET_FX_DATA	;; OFFSET @ RAM BANK 2 @ The RAM AREA of FX Data (in the Fx Data Area)	
	addwf FX_POS_SESSION_CURREND, W	;; + (0 to 31 [32]) => Fx Tab Pos
	movwf FSR0L		;; SET CURREND OFFSET to Fx DATA @ current Fx SESSION Pos.
	movff INDF0,FX_DATA_SESSION_CURREND	;; WRITE FX Data @ currend Session Pos to Working Register

	;; LOAD FX TIME DELAY @ Currend SESSION Pos (in the Fx Data Area)
	movlw RAM_OFFSET_FX_TIME_PRE_EXECUTION  	;; OFFSET @ RAM BANK 2 -  
							;; The RAM AREA of FX TIME DELAY REGS (in the Fx Data Area)	 
	addwf FX_POS_SESSION_CURREND, W			;; + (0 to 31 [32]) => Fx Tab Pos
	movwf FSR0L					;; SET CURREND OFFSET To Ind. Reg Adress
	movff INDF0,FX_TIME_PRE_EXECUTION_SESSION_CURREND	;; WRITE FX Pre Execution Time 
								;; @ currend Session Pos to Working Register
		;; End of Subroutine
		return

;; --------------------------------------------------------------------------



;; ##############################################################################
;; Fx Call to Load TUNE #1, #2 + #3
WRITE_FX_TUNE_1_2_3
		;; For Fx Tunes #123
		;; Save the last 3 Note On Values
		;; 1) Fx Tune #2 	=> Fx Tune #3 (last but two Tune)
		;; 2) Fx Tune #1 	=> Fx Tune #2 (last but one Tune)
		;; 3) Currend Note On 	=> Fx Tune #1 (last Tune)
		movff Fx_Note_On_2_tune_coarse, Fx_Note_On_3_tune_coarse
		movff Fx_Note_On_2_tune_fine, Fx_Note_On_3_tune_fine

		movff Fx_Note_On_1_tune_coarse, Fx_Note_On_2_tune_coarse
		movff Fx_Note_On_1_tune_fine, Fx_Note_On_2_tune_fine
		
		movff MIOS_PARAMETER1, Fx_Note_On_1_tune_coarse
		movff MIOS_PARAMETER2, Fx_Note_On_1_tune_fine
			return


;; ##############################################################################
;; ##############################################################################
;; Fx Call for Tune +
;; (1)
;; TUNE COARSE A|B|C => TUNE_COARSE_TEMP
;; TUNE FINE   A|B|C => TUNE_FINE_TEMP
;; bevor call
;;
;; (2)
;; TUNE_FINE_TEMP - CURREND Fx DATA BYTE
;; 
;; (3)
;; BORROW HANDLING IF NEEDED (DEC TUNE_COARSE_TEMP -1)
;; (3a) SET TUNE_COARSE_TEMP and TUNE_FINE_TEMP Min. Value if needed
;; 
;; (4)
;; after return
;; TUNE_COARSE_TEMP  => TUNE COARSE A|B|C
;; TUNE_FINE_TEMP    => TUNE FINE   A|B|C
FX_TUNE_PLUS

	movf FX_DATA_SESSION_CURREND, W
	setc
	;; TUNE_FINE_TEMP - CURREND Fx DATA BYTE
	subwfb TUNE_FINE_TEMP, F	
			;; CHECK BORROW BIT
			;; IF THE BORROW BIT appear DEC BORROR BIT @ COARSE TUNE REG
			bnc FX_TUNE_PLUS_BORROW_BIT_HANDLING
		
					;; NO BORROW BIT
					;; ALL DONE EXIT
					return

		

FX_TUNE_PLUS_BORROW_BIT_HANDLING
			;; CHECK IF COARSE IS ALLREADY B'0000 0000'
			movlw  0x00
			cpfseq TUNE_COARSE_TEMP
				;; COARSE TUNE REG IS > 0
				rgoto FX_TUNE_PLUS_BORROW_BIT_HANDLING_DEC_COARSE
			
				;; COARSE TUNE REG IS = 0
				rgoto FX_TUNE_PLUS_BORROW_BIT_HANDLING_SET_HIGHEST_TUNE

FX_TUNE_PLUS_BORROW_BIT_HANDLING_DEC_COARSE
				;; SUBTRACT THE BORROW BIT FROM COARSE TUNE REG
				decf TUNE_COARSE_TEMP, F
					;; ALL DONE EXIT
					return

FX_TUNE_PLUS_BORROW_BIT_HANDLING_SET_HIGHEST_TUNE
					;; highest Value of FINE TUNE REG IF COARSE TUNE REG IS 0
					movlw FINE_TUNE_MIN_VALUE_B00001010 
					movwf TUNE_FINE_TEMP
		
						;; ALL DONE EXIT
						return
	

;; ##############################################################################

;; FX Call for Tune -
;; (1)
;; TUNE COARSE A|B|C => TUNE_COARSE_TEMP
;; TUNE FINE   A|B|C => TUNE_FINE_TEMP
;; bevor call
;;
;; (2)
;; TUNE_COARSE_TEMP + CURREND Fx DATA BYTE
;; 
;; (3)
;; CARRY HANDLING IF NEEDED (INC TUNE_COARSE_TEMP +1)
;; (3a) SET TUNE_COARSE_TEMP and TUNE_FINE_TEMP Max Value if needed
;; 
;; (4)
;; after return
;; TUNE_COARSE_TEMP  => TUNE COARSE A|B|C
;; TUNE_FINE_TEMP    => TUNE FINE   A|B|C
FX_TUNE_MINUS
	;; COPY CURREND FX DATA BYTE TO WREG
	movf FX_DATA_SESSION_CURREND, W
		
	clrc ;; CLEAR CARRY BIT @ STATUS REG
	;; TUNE_FINE_TEMP REG [PLUS] CURREND Fx DATA BYTE
	addwfc TUNE_FINE_TEMP, F	
			;; CHECK CARRY BIT (BRACH IF IT IS SET) 
			bc FX_TUNE_MINUS_CARRY_BIT_HANDLING
		
				;; NO CARRY BIT
				;; ALL DONE EXIT
				return

		

FX_TUNE_MINUS_CARRY_BIT_HANDLING
			;; CHECK IF COARSE REG IS ALLREADY B'0000 1111' DEC. 15, HEX 0x0F
			movlw COARSE_TUNE_MAX_MINUS_1_B00001110
			cpfsgt TUNE_COARSE_TEMP
				;; COARSE TUNE REG IS < 0x0F (VALID)
				rgoto FX_TUNE_MINUS_CARRY_BIT_HANDLING_INC_COARSE
				
				;; VALUE NOT VALID 
				;; COARSE TUNE REG IS > 0x0E = 0x0F or greater so +1 inc Carry will 
				;; not be a VALID value
				rgoto FX_TUNE_MINUS_CARRY_BIT_HANDLING_SET_LOWEST_TUNE
				
FX_TUNE_MINUS_CARRY_BIT_HANDLING_INC_COARSE
				;; INC THE CARRY BIT @ COARSE TUNE REG
				incf TUNE_COARSE_TEMP, F
					;; ALL DONE EXIT
					return

FX_TUNE_MINUS_CARRY_BIT_HANDLING_SET_LOWEST_TUNE
		;; Lowest Value of COARSE TUNE REG IS B'0000 1111' / DEC. 15 / HEX 0x0F
		movlw COARSE_TUNE_MAX_VALUE_B00001111
		movwf TUNE_COARSE_TEMP
		;; Lowest Value of FINE TUNE REG IS in this case 0xD2
		movlw FINE_TUNE_MAX_VALUE_B11010010
		movwf TUNE_FINE_TEMP
					return


;; ##############################################################################
;; ##############################################################################
;; ----------------------------[END Fx Calls]------------------------------------
