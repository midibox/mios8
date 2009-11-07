;; note_off.inc

;; Function:
;; NOTE OFF workflow (Data handling)


; Note Off
; JUMP IN LABLE
MIDI_EVNT_IS_8x	

	
		
MIDI_EVNT_IS_8x_CHECK_MIDI_CHANNEL_A

	;; CHECK TONE A MIDI CHANNEL
	movf MIDI_EVNT_CHANNEL, W
	cpfseq AY_TONE_A_MIDI_CHANNEL_USER

		;NO
		rgoto MIDI_EVNT_IS_8x_CHECK_MIDI_CHANNEL_B
		
		;YES - NOTE OFF @ TONE A
		rgoto MIDI_EVNT_IS_8x_NOTE_OFF_TONE_A


MIDI_EVNT_IS_8x_NOTE_OFF_TONE_A

	;; CHCKING if the Note OFF value correlates with the last
	;; NOTE ON value (importend for MIDI KEYBOARD HANDLING of delayed NOTE OFF values) 
	;; via CURRENT_MIDI_EVENT_NOTE_ON_OFF
	movf MIDI_EVNT1, W
	cpfseq CURRENT_MIDI_EVENT_NOTE_ON_OFF_A
			;; current NOTE OFF value is <> currend NOTE ON value
			;; so do nothing and exit here
			return

		;; current NOTE OFF value is = currend NOTE ON value
		;; *CLEAR* FX MIDI NOTE ON FLAG @ TONE A
		bcf Fx_MIDI_NOTE_ON_OR_OFF, BIT0_TONE_A_ON_OFF
		
						
		;; CHECK: IS ENVELOPE MODE @ TONE A ON?
		;; BIT 4 SET 	= ENV. MODE ON
		;; BIT 4 CLEAT 	= ENV. MODE OFF
		btfsc AY_R8_CH_A_AMPLITUDE, BIT4_ENVELOPE_ON_OFF
        		;; YES - BIT 4 is SET - ENV. MODE ON
        		;; DO *NOT* SWITCH OFF TONE A
        		;; LET THE AY 3 8912 ENV. MODE WORK SO QUIT HERE
        		rgoto MIDI_EVNT_IS_8x_CHECK_MIDI_CHANNEL_B
			;; NO - BIT 4 is CLEAR - ENV. MODE OFF
			;; GO ON WITH SWITCH OFF TONE A
			rgoto MIDI_EVNT_IS_8x_SWITCH_OFF_TONE_A


MIDI_EVNT_IS_8x_SWITCH_OFF_TONE_A
	;; MIDI SWITCH TONE A NOISE A *OFF*
	;; CLEAR BIT0, BIT3
	;; => FILTER FOR REG 7
	;; => SWITCH OFF TONE A, NOISE A  
	bsf	AY_TONE_ABC_MIDI_ON_OFF, BIT0_TONE_A_ON_OFF
	bsf	AY_TONE_ABC_MIDI_ON_OFF, BIT3_NOISE_A_ON_OFF
		
		rgoto MIDI_EVNT_IS_8x_CHECK_MIDI_CHANNEL_B



;; ----------

MIDI_EVNT_IS_8x_CHECK_MIDI_CHANNEL_B
	;; CHECK TONE B MIDI CHANNEL
	movf MIDI_EVNT_CHANNEL, W
	cpfseq AY_TONE_B_MIDI_CHANNEL_USER

		;NO
		rgoto MIDI_EVNT_IS_8x_CHECK_MIDI_CHANNEL_C
		
		;YES - NOTE OFF @ TONE B
		rgoto MIDI_EVNT_IS_8x_NOTE_OFF_TONE_B

MIDI_EVNT_IS_8x_NOTE_OFF_TONE_B
	;; CHCKING if the Note OFF value correlates with the last
	;; NOTE ON value (importend for MIDI KEYBOARD HANDLING of delayed NOTE OFF values) 
	;; via CURRENT_MIDI_EVENT_NOTE_ON_OFF
	movf MIDI_EVNT1, W
	cpfseq CURRENT_MIDI_EVENT_NOTE_ON_OFF_B
			;; current NOTE OFF value is <> currend NOTE ON value
			;; so do nothing and exit here
		 	return

		;; current NOTE OFF value is = currend NOTE ON value
		;; *CLEAR* FX MIDI NOTE ON FLAG @ TONE B
		bcf Fx_MIDI_NOTE_ON_OR_OFF, BIT1_TONE_B_ON_OFF
		
		;; CHECK: IS ENVELOPE MODE @ TONE B ON?
		;; BIT 4 SET 	= ENV. MODE ON
		;; BIT 4 CLEAT 	= ENV. MODE OFF
		btfsc AY_R9_CH_B_AMPLITUDE, BIT4_ENVELOPE_ON_OFF
        		;; YES - BIT 4 is SET - ENV. MODE ON
        		;; DO *NOT* SWITCH OFF TONE A
        		;; LET THE AY 3 8912 ENV. MODE WORK SO QUIT HERE
        		rgoto MIDI_EVNT_IS_8x_CHECK_MIDI_CHANNEL_C
			;; NO - BIT 4 is CLEAR - ENV. MODE OFF
			;; GO ON WITH SWITCH OFF TONE A
			rgoto MIDI_EVNT_IS_8x_SWITCH_OFF_TONE_B


MIDI_EVNT_IS_8x_SWITCH_OFF_TONE_B
	;; MIDI SWITCH TONE B NOISE B *OFF*
	;; CLEAR BIT1, BIT4
	;; => FILTER FOR REG 7
	;; => SWITCH OFF TONE B, NOISE B  
	bsf	AY_TONE_ABC_MIDI_ON_OFF, BIT1_TONE_B_ON_OFF
	bsf	AY_TONE_ABC_MIDI_ON_OFF, BIT4_NOISE_B_ON_OFF

		rgoto MIDI_EVNT_IS_8x_CHECK_MIDI_CHANNEL_C



;; -------------


MIDI_EVNT_IS_8x_CHECK_MIDI_CHANNEL_C

	;; CHECK TONE C MIDI CHANNEL
	movf MIDI_EVNT_CHANNEL, W
	cpfseq AY_TONE_C_MIDI_CHANNEL_USER

		;NO - GO AND SEND DATA TO AY CHIP
		rgoto	MIDI_EVNT_IS_8x_SEND_NOTE_OFF_TO_AY_CHIP
		
		;YES - NOTE OFF @ TONE C
		rgoto MIDI_EVNT_IS_8x_NOTE_OFF_TONE_C


MIDI_EVNT_IS_8x_NOTE_OFF_TONE_C
	;; CHCKING if the Note OFF value correlates with the last
	;; NOTE ON value (importend for MIDI KEYBOARD HANDLING of delayed NOTE OFF values) 
	;; via CURRENT_MIDI_EVENT_NOTE_ON_OFF
	movf MIDI_EVNT1, W
	cpfseq CURRENT_MIDI_EVENT_NOTE_ON_OFF_C
			;; current NOTE OFF value is <> currend NOTE ON value
			;; so do nothing and exit here
			return
		
		;; current NOTE OFF value is = currend NOTE ON value
		;; *CLEAR* FX MIDI NOTE ON FLAG @ TONE C
		bcf Fx_MIDI_NOTE_ON_OR_OFF, BIT2_TONE_C_ON_OFF
		
		;; CHECK: IS ENVELOPE MODE @ TONE C ON?
		;; BIT 4 SET 	= ENV. MODE ON
		;; BIT 4 CLEAT 	= ENV. MODE OFF
		btfsc AY_R10_CH_C_AMPLITUDE, BIT4_ENVELOPE_ON_OFF
        		;; YES - BIT 4 is SET - ENV. MODE ON
        		;; DO *NOT* SWITCH OFF TONE C
        		;; LET THE AY 3 8912 ENV. MODE WORK SO QUIT HERE
        		rgoto MIDI_EVNT_IS_8x_SEND_NOTE_OFF_TO_AY_CHIP
			;; NO - BIT 4 is CLEAR - ENV. MODE OFF
			;; GO ON WITH SWITCH OFF TONE A
			rgoto MIDI_EVNT_IS_8x_SWITCH_OFF_TONE_C

MIDI_EVNT_IS_8x_SWITCH_OFF_TONE_C
	;; MIDI SWITCH TONE C NOISE C *OFF*
	;; CLEAR BIT2, BIT5
	;; => FILTER FOR REG 7
	;; => SWITCH OFF TONE C, NOISE C  
	bsf	AY_TONE_ABC_MIDI_ON_OFF, BIT2_TONE_C_ON_OFF
	bsf	AY_TONE_ABC_MIDI_ON_OFF, BIT5_NOISE_C_ON_OFF

		rgoto MIDI_EVNT_IS_8x_SEND_NOTE_OFF_TO_AY_CHIP



;; ---------------------------------------------------------------


MIDI_EVNT_IS_8x_SEND_NOTE_OFF_TO_AY_CHIP

	;; ==============================================================================
	;; MIDIFILTER NOTE OFF @ AY REG 7
	;; SWITCH TONE  A|B|C OFF 
	;; SWITCH NOISE A|B|C OFF
	movlw 0xFF 	
	;; AND BIN 11111111
	;; WITH USER ON/OFF: TONE ABC / NOISE ABC 
	andwf AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, W 
	;; OR W-REG with TONE ABC MIDI ON/OFF
	iorwf AY_TONE_ABC_MIDI_ON_OFF, W
	
	;; WRITE RESULT from W-REG TO AY REG7
	movwf AY_REG_DATA 
	;; SEND DATA TO AY REG 7
	movlw AY_CHIP_ADR_07_TONE_NOISE_ON_OFF		; ADRESS => AY REGISTER
	movwf AY_REG_ADDRESS 				; ADDRESS FOR => AY_SR_Write
	;; --------------[SEND DATA TO AY REG 7]------------------------------------------
	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
	call LABEL_AY_SR_Write	
	;; ==============================================================================	
	
			;; Exit here
			return
	
	
; END - Note Off
