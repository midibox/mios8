;; main_menu_03_mixer.inc

;; ----------------------------------------------------------------------------
;; -------------------------[MB AY MENU 3: MIX CH ABC / FX / NOISE PERIOD]------------ 
;; =======================> (F3 has been pressed)


;; ---------------[WRITE MAIN MENU3 (MIXER) TO LCD]----------------------------------

LABEL_AY_MAIN_MENU_03_ABC_MIXER
	
	call    MIOS_LCD_Clear
   
    	;; PRINT MENUE MIXER [Fx] ON/OFF
    	TABLE_ADDR TEXT_AY_MENU_MIXER_FX_ON_OFF
    	call    MIOS_LCD_PrintString
    	;; PRINT LCD ROW 2 - TONE ABC on/off
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_NOISE_MIX
	call    MIOS_LCD_PrintString
	;; PRINT LCD ROW 3 MIX - add noise
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_NOISE_MIX2
	call    MIOS_LCD_PrintString
	;; LCD ROW 4 - SHOW AY NOISE PERIOD
	TABLE_ADDR TEXT_AY_MENU_NOISE_PERIOD
	call    MIOS_LCD_PrintString


;; ---------------[CHECK: NOISE PERIOD = VELOCITY CONTROLED?]-------------------

LABEL_AY_MENU_MIX_NOISE_PERIOD_CHK_VELOITY_NOISE_PERIOD_TONE_A
	;; check if velosity control noise period => velocity from TONE A ?
	;; CHECK IF IT NOISE PERIOD IS CONTROLLED By VELOCITY from TONE A
	;; BIT Test 5, skip if set
	btfss AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT5_NOISE_PERIOD_CONTROLED_VIA_VELOCITY_TONE_A_ON_OFF
		;; NO - CHECK TONE B
		rgoto LABEL_AY_MENU_MIX_NOISE_PERIOD_CHK_VELOITY_NOISE_PERIOD_TONE_B
		;; YES - IT's TONE A - GO AND WRIT IT TO LCD
		rgoto LABEL_AY_MENU_MIX_NOISE_PERIOD_WRITE_VEOCITY_A_NOISE_PERIOD
		

LABEL_AY_MENU_MIX_NOISE_PERIOD_CHK_VELOITY_NOISE_PERIOD_TONE_B
	;; BIT Test 6, skip if set
	;; CHECK IF IT NOISE PERIOD IS CONTROLLED by VELOCITY from TONE B
	btfss AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT6_NOISE_PERIOD_CONTROLED_VIA_VELOCITY_TONE_B_ON_OFF
		;; NO - Check next Tone
		rgoto LABEL_AY_MENU_MIX_NOISE_PERIOD_CHK_VELOITY_NOISE_PERIOD_TONE_C
		;; YES - IT's TONE B - GO AND WRIT IT TO LCD
		rgoto LABEL_AY_MENU_MIX_NOISE_PERIOD_WRITE_VEOCITY_B_NOISE_PERIOD


LABEL_AY_MENU_MIX_NOISE_PERIOD_CHK_VELOITY_NOISE_PERIOD_TONE_C
	;; BIT Test 7, skip if set
	;; CHECK IF IT NOISE PERIOD IS CONTROLLED BY VELOCITY from TONE C
	btfss AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT7_NOISE_PERIOD_CONTROLED_VIA_VELOCITY_TONE_C_ON_OFF
		;; NO - NOW WE CHECKED TONE ABC
		;; THERE WAS NO VELOCITY AT TONE ABC, SO
		;; JUST WRITE THE NOISE PERIOD VALUE ON LCD
		rgoto LABEL_AY_MENU_MIX_NOISE_PERIOD_WRITE_VALUE_TO_LCD
		;; YES - IT's TONE C - GO AND WRIT IT TO LCD
		rgoto LABEL_AY_MENU_MIX_NOISE_PERIOD_WRITE_VEOCITY_C_NOISE_PERIOD

LABEL_AY_MENU_MIX_NOISE_PERIOD_WRITE_VALUE_TO_LCD
	movlw   LCD_ROW4 + 0x0d
	call    MIOS_LCD_CursorSet
	
	;; SHOW REVERSE VALUE ON LCD
	;; 0  => LCD 31
	;; 31 => LCD 0
	movlw NOISE_PERIOD_MAX_VALUE  ;; 31
	movwf AY_R6_NOISE_PERIOD_REVERSE_VALUE
	movf AY_R6_NOISE_PERIOD, W
	subwf AY_R6_NOISE_PERIOD_REVERSE_VALUE, W
	;; PRINT REVERSE NOISE PERIOD VALUE
	;; TO LCD 
	call        MIOS_LCD_PrintBCD3


;; -----------------------[CHECK: FX A|B|C ON or OFF]---------------------------------------		


LABEL_AY_MENU_FX_A
        
        ;; IS FX @ A ON OR OFF? (BIT0 SET or CLEAR?)
        ;; btfsc = Überspringe wenn BIT0 = 0 ist
	btfsc FX_USER_ON_OFF_ABC, BIT0_FX_ON_OFF_TONE_A
        	
        	;; NO, BIT0 = 1  
        	;; SO FX @ CH A IS ENABLED 
        	rgoto  LABEL_AY_MENU_FX_A_ON
		
		;; YES, BIT0 = 0
		;; FX @ CH A IS NOT ENABLED 
		rgoto LABEL_AY_MENU_FX_A_OFF

LABEL_AY_MENU_FX_B
	
	;; IS FX @ B ON OR OFF? (BIT1 SET or CLEAR?)
	btfsc FX_USER_ON_OFF_ABC, BIT1_FX_ON_OFF_TONE_B
        	
        	;; NO, BIT1 = 1  
        	rgoto LABEL_AY_MENU_FX_B_ON
		
		;; YES, BIT1 = 0
		rgoto  LABEL_AY_MENU_FX_B_OFF

LABEL_AY_MENU_FX_C

	;; IS FX @ C ON OR OFF? (BIT2 SET or CLEAR?)
	btfsc FX_USER_ON_OFF_ABC, BIT2_FX_ON_OFF_TONE_C
        	
        	;; NO, BIT2 = 1  
        	rgoto  LABEL_AY_MENU_FX_C_ON
		
		;; YES, BIT2 = 0
		rgoto  LABEL_AY_MENU_FX_C_OFF	

	

;; -----------------------[CHECK: TONE A|B|C ON or OFF]---------------------------------------		


LABEL_AY_MENU_MIX_A
        
        ;; IS TONE A ON OR OFF?
        ;; btfsc: If bit 'b' in register ’f' is 0, then the
	;; next instruction is skipped
	;; IS BIT 0 IN AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH = 0?
        btfsc AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, BIT0_TONE_A_ON_OFF
        	
        	;; NO - BIT0 IS *NOT* = 0
        	;; BIT0 = 1 => SO TONE A IS *NOT* ON 
        	rgoto LABEL_AY_MENU_MIX_ABC_NOISE_A_NOT 
		
		;; YES - BIT0 = 0
		;; BIT0 = 0 => SO TONE A *IS* ON 
		rgoto LABEL_AY_MENU_MIX_ABC_NOISE_A_SET 

LABEL_AY_MENU_MIX_B

	;; IS TONE B ON OR OFF?
	;; IS BIT 1 IN AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH = 0?
        btfsc AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, BIT1_TONE_B_ON_OFF
        	
        	;; NO - SO BIT1 *IS* SET
        	rgoto LABEL_AY_MENU_MIX_ABC_NOISE_B_NOT
		
		;; YES - SO BIT1 IS *NOT* SET
		rgoto  LABEL_AY_MENU_MIX_ABC_NOISE_B_SET

LABEL_AY_MENU_MIX_C

	;; IS TONE C ON OR OFF?
	;; IS BIT2 IN AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH = 0?
        btfsc AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, BIT2_TONE_C_ON_OFF
        	
        	;; NO - SO BIT2 *IS* SET
        	rgoto 	LABEL_AY_MENU_MIX_ABC_NOISE_C_NOT
		
		;; YES - SO BIT2 IS *NOT* SET
		rgoto  	LABEL_AY_MENU_MIX_ABC_NOISE_C_SET



;; -----------------------[CHECK: NOISE A|B|C ON or OFF]---------------------------------------		

LABEL_AY_MENU_NOISE_MIX_A
	
	;; NOISE CH. A - ON OR OFF?
	;; IS BIT3 IN AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH = 0?
        btfsc AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, BIT3_NOISE_A_ON_OFF
        	
        	;; NO - SO BIT3 *IS* SET
        	rgoto  LABEL_AY_MENU_MIX_NOISE_A_NOT
		
		;; YES - SO BIT2 IS *NOT* SET
		rgoto  LABEL_AY_MENU_MIX_NOISE_A_SET


LABEL_AY_MENU_NOISE_MIX_B
	
	;; NOISE CH. B - ON OR OFF?
	;; IS BIT4 IN AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH = 0?
        btfsc AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, BIT4_NOISE_B_ON_OFF
        	
        	;; NO - SO BIT4 *IS* SET
        	rgoto  LABEL_AY_MENU_MIX_NOISE_B_NOT
		
		;; YES - SO BIT4 IS *NOT* SET
		rgoto   LABEL_AY_MENU_MIX_NOISE_B_SET


LABEL_AY_MENU_NOISE_MIX_C
	
	;; NOISE CH. C - ON OR OFF?
	;; ;; IS BIT5 IN AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH = 0?
        btfsc AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH, BIT5_NOISE_C_ON_OFF
        	
        	;; NO - SO BIT5 *IS* SET
        	rgoto LABEL_AY_MENU_MIX_NOISE_C_NOT 
		
		;; YES - SO BIT5 IS *NOT* SET
		rgoto  LABEL_AY_MENU_MIX_NOISE_C_SET



;; -------------------[CHECK: ITEM POS @ MAIN MENU 3]------------------------------------
;; -------------------[POS1 ?]------------------------------------
;; ------------------------------
;; MAIN MENU 3: MIXER ABC +NOISE
;; Show the POSITION of * 
;; [LCD: MAIN MENU EDIT POINTER] 
;; for Main Menu EDITING Posiotion

LABEL_AY_MAIN_MENU_03_FIND_EDIT_POS1
	;; POS1 = Fx TONE ABC on off 
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE = 1?
	movff AY_LCD_COURSER_POS_MAIN_MENU_ABC_NOISE, MIOS_PARAMETER1
	
	movlw MENU_ITEM_POS1
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto LABEL_AY_MAIN_MENU_03_NOT_POS1

		;; YES
		rgoto LABEL_AY_MAIN_MENU_03_POS1


LABEL_AY_MAIN_MENU_03_POS1
		;; EDITING MODE
		;; PRINT "*" ON EDIT POSITION1
		movlw 	LCD_ROW1 + 12
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
		
			rgoto LABEL_AY_MAIN_MENU_03_FIND_EDIT_POS2


LABEL_AY_MAIN_MENU_03_NOT_POS1
		;; NO EDITING MODE 
		;; PRINT ":" ON EDIT POSITION1
		movlw 	LCD_ROW1 + 12
		call    MIOS_LCD_CursorSet
		movlw   ':'
		call    MIOS_LCD_PrintChar
			
        		rgoto LABEL_AY_MAIN_MENU_03_FIND_EDIT_POS2


;; -------------------[POS2 ?]------------------------------------

LABEL_AY_MAIN_MENU_03_FIND_EDIT_POS2
	
	;; EDITING MODE @ POS CH ABC TONE ON/OFF ?
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE = 2?
	movff AY_LCD_COURSER_POS_MAIN_MENU_ABC_NOISE, MIOS_PARAMETER1
	
	movlw MENU_ITEM_POS2
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto LABEL_AY_MAIN_MENU_03_NOT_POS2

		;; YES
		rgoto LABEL_AY_MAIN_MENU_03_POS2


LABEL_AY_MAIN_MENU_03_POS2
		;; EDITING MODE @ POS CH ABC TONE ON/OFF
		;; PRINT "*" ON EDIT POSITION2
		movlw 	LCD_ROW2 + 12
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
		
			rgoto LABEL_AY_MAIN_MENU_03_FIND_EDIT_POS3


LABEL_AY_MAIN_MENU_03_NOT_POS2
		;; NO EDITING MODE @ POS CH ABC TONE ON/OFF 
		;; PRINT ":" ON EDIT POSITION2
		movlw 	LCD_ROW2 + 12
		call    MIOS_LCD_CursorSet
		movlw   ':'
		call    MIOS_LCD_PrintChar
			
        		rgoto LABEL_AY_MAIN_MENU_03_FIND_EDIT_POS3


;; -------------------[POS3 ?]------------------------------------	
	
LABEL_AY_MAIN_MENU_03_FIND_EDIT_POS3

	;; EDITING MODE @ ADD NOISE TO CH ABC ?
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE = 3?
	movff AY_LCD_COURSER_POS_MAIN_MENU_ABC_NOISE, MIOS_PARAMETER1
	
	movlw MENU_ITEM_POS3
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto LABEL_AY_MAIN_MENU_03_NOT_POS3

		;; YES
		rgoto LABEL_AY_MAIN_MENU_03_POS3



LABEL_AY_MAIN_MENU_03_POS3	
		;; EDITING MODE @ ADD NOISE TO CH ABC
		;; PRINT "*" ON EDIT POSITION3
		movlw 	LCD_ROW3 + 12
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
		
		rgoto LABEL_AY_MAIN_MENU_03_FIND_EDIT_POS4


LABEL_AY_MAIN_MENU_03_NOT_POS3
		;; NO EDITING MODE @ ADD NOISE TO CH ABC
		;; PRINT ":" ON EDIT POSITION3
		movlw 	LCD_ROW3 + 12
		call    MIOS_LCD_CursorSet
		movlw   ':'
		call    MIOS_LCD_PrintChar
			
        		rgoto LABEL_AY_MAIN_MENU_03_FIND_EDIT_POS4

;; -------------------[POS4 ?]------------------------------------

LABEL_AY_MAIN_MENU_03_FIND_EDIT_POS4

	;; EDITING MODE @ NOISE PERIOD ?
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE = 4?
	movff AY_LCD_COURSER_POS_MAIN_MENU_ABC_NOISE, MIOS_PARAMETER1
	
	movlw MENU_ITEM_POS4
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto LABEL_AY_MAIN_MENU_03_NOT_POS4

		;; YES
		rgoto LABEL_AY_MAIN_MENU_03_POS4



LABEL_AY_MAIN_MENU_03_POS4
		;; EDITING MODE @ NOISE PERIOD
		;; PRINT "*" ON EDIT POSITION4
		movlw 	LCD_ROW4 + 12
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
				
				;; Exit Here
				return


LABEL_AY_MAIN_MENU_03_NOT_POS4
		;; NO EDITING MODE @ NOISE PERIOD
		;; PRINT ":" ON EDIT POSITION4
		movlw 	LCD_ROW4 + 12
		call    MIOS_LCD_CursorSet
		movlw   ':'
		call    MIOS_LCD_PrintChar
			
				;; Exit Here
				return


;; --------------------------------------------------
;; ---[END OF MAIN MENU3 ROUTINE]--------------------






;; ---------------------------------------------------------------------------------------
;; ----------[START LCD SUB ROUTINE FOR MENU: FX ABC / CH ABC / NOISE PERIOD]------------------
;; ---------------------------------------------------------------------------------------


;; ----------------[PRINT FX ABC on or off]--------------------------------------------

;; MIXER FX ITEM:
;; PRINT "A" or "-" LCD ROW1 
;; FX @ TONE A on or of
LABEL_AY_MENU_FX_A_OFF
	TABLE_ADDR	TEXT_AY_MENU3_FX_A_OFF
        call    MIOS_LCD_PrintString
        rgoto 	LABEL_AY_MENU_FX_B

LABEL_AY_MENU_FX_A_ON
	TABLE_ADDR  	TEXT_AY_MENU3_FX_A_ON
        call    MIOS_LCD_PrintString
        rgoto	LABEL_AY_MENU_FX_B

;; PRINT "B" or "-" LCD ROW1 
;; FX @ TONE B on or off
LABEL_AY_MENU_FX_B_OFF
	TABLE_ADDR 	TEXT_AY_MENU3_FX_B_OFF
        call    MIOS_LCD_PrintString
        rgoto 	LABEL_AY_MENU_FX_C

LABEL_AY_MENU_FX_B_ON
	TABLE_ADDR 	TEXT_AY_MENU3_FX_B_ON
        call    MIOS_LCD_PrintString
        rgoto	LABEL_AY_MENU_FX_C
        

;; PRINT "C" or "-" LCD ROW1 
;; FX @ TONE C on or off
LABEL_AY_MENU_FX_C_OFF
	TABLE_ADDR  	TEXT_AY_MENU3_FX_C_OFF
        call    MIOS_LCD_PrintString
        rgoto 	LABEL_AY_MENU_MIX_A

LABEL_AY_MENU_FX_C_ON
	TABLE_ADDR  	TEXT_AY_MENU3_FX_C_ON
        call    MIOS_LCD_PrintString
        rgoto	LABEL_AY_MENU_MIX_A



;; ----------------[PRINT TONE ABC on or off]--------------------------------------------

;; JUMPED IN HERE FROM: MIX CH ABC 
;; SEE: LABEL_AY_MENU_MIX_A
;; PRINT "A" or "-" LCD ROW2 
;; TONE A
LABEL_AY_MENU_MIX_ABC_NOISE_A_NOT
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_MIX_A_NOT
        call    MIOS_LCD_PrintString
        rgoto 	LABEL_AY_MENU_MIX_B

LABEL_AY_MENU_MIX_ABC_NOISE_A_SET
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_MIX_A_SET
        call    MIOS_LCD_PrintString
        rgoto	LABEL_AY_MENU_MIX_B

;; SEE: LABEL_AY_MENU_MIX_B
;; PRINT "B" or "-" LCD ROW2 
;; TONE B
LABEL_AY_MENU_MIX_ABC_NOISE_B_NOT
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_MIX_B_NOT
        call    MIOS_LCD_PrintString
        rgoto 	LABEL_AY_MENU_MIX_C

LABEL_AY_MENU_MIX_ABC_NOISE_B_SET
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_MIX_B_SET
        call    MIOS_LCD_PrintString
        rgoto	LABEL_AY_MENU_MIX_C
        

;; PRINT "C" or "-" LCD ROW2 
;; TONE C
LABEL_AY_MENU_MIX_ABC_NOISE_C_NOT
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_MIX_C_NOT
        call    MIOS_LCD_PrintString
        rgoto 	LABEL_AY_MENU_NOISE_MIX_A

LABEL_AY_MENU_MIX_ABC_NOISE_C_SET
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_MIX_C_SET
        call    MIOS_LCD_PrintString
        rgoto	LABEL_AY_MENU_NOISE_MIX_A


;; ----------------[PRINT NOISE ABC on or off]--------------------------------------------

;; PRINT IF NOISE IS ENABLED OR NOT
;; PRINT "A" or "-" LCD ROW2 
;; NOISE MIX CH. A
LABEL_AY_MENU_MIX_NOISE_A_NOT
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_MIX_NOISE_A_NOT
        call    MIOS_LCD_PrintString
        rgoto	LABEL_AY_MENU_NOISE_MIX_B


LABEL_AY_MENU_MIX_NOISE_A_SET
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_MIX_NOISE_A_SET
        call    MIOS_LCD_PrintString
        rgoto	LABEL_AY_MENU_NOISE_MIX_B

;; PRINT "B" or "-" LCD ROW2 
;; NOISE MIX CH. B
LABEL_AY_MENU_MIX_NOISE_B_NOT
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_MIX_NOISE_B_NOT
        call    MIOS_LCD_PrintString
        rgoto	LABEL_AY_MENU_NOISE_MIX_C


LABEL_AY_MENU_MIX_NOISE_B_SET
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_MIX_NOISE_B_SET
        call    MIOS_LCD_PrintString
        rgoto	LABEL_AY_MENU_NOISE_MIX_C


;; PRINT "C" or "-" LCD ROW2 
;; NOISE MIX CH. C
LABEL_AY_MENU_MIX_NOISE_C_NOT
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_MIX_NOISE_C_NOT
        call    MIOS_LCD_PrintString
        rgoto	LABEL_AY_MAIN_MENU_03_FIND_EDIT_POS1


LABEL_AY_MENU_MIX_NOISE_C_SET
	TABLE_ADDR TEXT_AY_MENU_TONE_ABC_MIX_NOISE_C_SET
        call    MIOS_LCD_PrintString
        rgoto	LABEL_AY_MAIN_MENU_03_FIND_EDIT_POS1


;; -----------------[IF VELOCITY TO NOISE PERIOD IS ACTIVATED]--------------------
;; -----------------PRINT THIS INFORMATION ON LCD---------------------------------

LABEL_AY_MENU_MIX_NOISE_PERIOD_WRITE_VEOCITY_A_NOISE_PERIOD
			TABLE_ADDR TEXT_AY_MENU_NOISE_PERIOD_VELOCITY_A
			call    MIOS_LCD_PrintString
        			rgoto	LABEL_AY_MENU_FX_A

			
LABEL_AY_MENU_MIX_NOISE_PERIOD_WRITE_VEOCITY_B_NOISE_PERIOD
			TABLE_ADDR TEXT_AY_MENU_NOISE_PERIOD_VELOCITY_B
			call    MIOS_LCD_PrintString
        			rgoto	LABEL_AY_MENU_FX_A


LABEL_AY_MENU_MIX_NOISE_PERIOD_WRITE_VEOCITY_C_NOISE_PERIOD
			TABLE_ADDR TEXT_AY_MENU_NOISE_PERIOD_VELOCITY_C
			call    MIOS_LCD_PrintString
        			rgoto	LABEL_AY_MENU_FX_A


;; ---------------------[END LCD SUB ROUTINES]----------------------------------------



