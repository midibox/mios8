;; ------------------------------
;; note_on_write_ay_reg7.inc    #
;; ------------------------------

;; What happend ?
;;
;; There was a [MIDI EVENT :: NOTE ON]
;;	||
;;	|| DO *NOTE ON* Processing @ TONE A,B,C
;; 	\/
;; note_on_pipeline_a.inc 
;; note_on_pipeline_b.inc 
;; note_on_pipeline_c.inc
;; 	||
;;      \/
;; NOW WRITE [NOTE ON] A|B|C DATA to 
;; => AY CHIP REG. 7 

;; --------------------------------


;;; --[NOTE ON WORKFLOW - TONE [+NOISE] A|B|C ON]--

MIDI_EVNT_NOTE_ON_TONE_FILTER	

	;; NOTE ON WORKFLOW - TONE [+NOISE] A|B|C ON
	;; MIDI NOTE ON :: TOGGLE TONE A|B|C @ AY CHIP REG 7
	;; SWITCH TONE ABC / NOISE ABC ON
	
	;; WHAT WAS HAPPEND?
	
	;; THRE WAS A MIDI *NOTE ON* EVENT
	;; A VALUE ACCORDING TO MIDI_EVNT1 HAS BEEN WRITTEN TO
	;; MIOS_PARAMETER1 (COASE TUNE)
	;; MIOS_PARAMETER2 (FINE TUNE)
	;; 
	;; THE MIDI-CHANNEL FILTER HAD BEEN PASSED
	;; SWITCH/FILTER MIDICHANNEL SEE LABEL => MIDI_EVNT_CHECK_MIDI_CHANNEL
	;; THE TONE SWITCH HAD BEEN PASSED ACCORDING TO THE MIDICHANNEL FILTER	
	;; BIT [X] CLEAR => TONE  [X] ON 	| X = TONE 	A or B or C
	;; BIT [X] CLEAR => NOISE [X] ON 	| X = NOISE 	A or B or C
	;; 
	;; MIDI_EVNT_CHECK_MIDI_CHANNEL_TONE_A_B_C_TONE
	;; AY REG 7 => TONE A/B/C + NOISE A/B/C
	;; 
	;; NEXT:
	;; USER SETTING FILTER 
	;; MIDI TONE ON FILTER 
	;; ROR ==> AY_R7_ENABLE_ABC_NOISE_IO
	
	
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


MIDI_EVNT_NOTE_ON_SENT_DATA_TO_AY_CHIP
	;; SEND DATA TO AY REG 7
	movlw 0x07			; REG ADRESS 	=> AY REGISTER FOR AY CHIP REG 7
	movwf AY_REG_ADDRESS 		; ADDRESS FOR 	=> AY_SR_Write
        
        ;; WRITE AY_R7_ENABLE_ABC_NOISE_IO TO AY_REG_DATA
        movff AY_R7_ENABLE_ABC_NOISE_IO, AY_REG_DATA
        
		;; --------------[SEND DATA TO AY REG 7]-----------------------------------------------------
		call LABEL_AY_SR_Write	; WRITING 8BIT DATA (AY_REG_ADDRESS, AY_REG_DATA)TO AY REG(X) ROUTINE
		;; ==========================================================================================
			

				return


