;; main_menu_02_detune.inc


;; -----------------------------------------------------
;; ---------[MB AY MAIN MENU 02: DETUNE ABC]------------ 
;; =======================> (F2 has been pressed)
;; This Menu shows the (de)tune values 
;; detune is not transpose (!)

LABEL_AY_MAIN_MENU_02_ABC_TUNE
    	      ;; WRITE LCD SCREN => TUNE MENU
	      call    MIOS_LCD_Clear
   
    		;; PRINT MENUE HEADLINE (LCD LINE1)
        	TABLE_ADDR TEXT_AY_MENU_ABC_TUNE
        	call    MIOS_LCD_PrintString
        

LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_A
;; ----[MAIN MENU TONE A - COARSE TUNE]------------
	;; LCD LOCATION: ROW2
	;; CHECK IF COARSE TUNE TONE A IS POSITIVE (+ZERO) or  NEGATIVE 
	;; ...
	;; 3 = -4
	;; 4 = -3
	;; 5 = -2
	;; 6 = -1
	;; 7 =  0
	;; 8 = +1
	;; 9 = +2
	;; ...
	;; TONE A COARSE TUNE USER > 6?
	movlw	PROOF_POSITIVE_OR_NEGATIVE_VIA_0X06 
	cpfsgt 	AY_TONE_A_TUNE_COARSE_USER
			;; NO - VALUE @ LCD WILL BE NEGATIVE
			rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_A_NEGATIVE

			;; YES - VALUE @ LCD WILL BE POSITIVE or ZERO
			rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_A_ZERO_AND_POSITIVE
			

LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_A_ZERO_AND_POSITIVE
			;; PRINT LCD LINE2 POSIVE VALUE
			; CLEAR LCD ROW2
			TABLE_ADDR LCD_CLEAR_ROW2			
        		call    MIOS_LCD_PrintString

        		; PRINT LCD TXT ROW2
        		TABLE_ADDR TEXT_AY_MENU_ABC_TUNE_CHA_TUNE 	
        		call    MIOS_LCD_PrintString
	
			;; PREPARE USER COARSE VALUE FOR LCD VIEW
			movlw	OFFSET_COARSE_TUNE_IS_7
			subwf	AY_TONE_A_TUNE_COARSE_USER, W	; - 0x07 OFFSET: FOR LCD MENU VALUE
			movwf	MIOS_PARAMETER1			; LCD VALUE "USER DETUNE COARSE"
								; IS NOW IN MIOS_PARAMETER1 
        
			;; SET LCD POS
        		movlw   LCD_ROW2 + 0x08
        		call    MIOS_LCD_CursorSet
		
			;; WRITE "USER DETUNE COARSE" AS A *LCD VALUE*	
        		movf    MIOS_PARAMETER1,W		
        		call    MIOS_LCD_PrintBCD1

			rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_A
        



LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_A_NEGATIVE
        		;; PRINT LCD LINE2 NEGATIVE VALUE
        
       			 ;; CLEAR ROW2
        		TABLE_ADDR LCD_CLEAR_ROW2			
        		call    MIOS_LCD_PrintString
        
        		; WRITE ITEM ROW2
        		TABLE_ADDR TEXT_AY_MENU_ABC_TUNE_CHA_TUNE 	
        		call    MIOS_LCD_PrintString
        
       			; POS FOR "-"  
        		movlw   LCD_ROW2 + 0x06				
        		call    MIOS_LCD_CursorSet
			; PRINT "-"
			movlw   '-'
        		call    MIOS_LCD_PrintChar			

			;; : MEMO :
			;; Die folgende Prozedur ist notwendig da auf dem LCD
			;; +/- Zahlen gewünscht sind, das bedeutet:
			;; ...
			;; LCD: +3 # DETUNE USER WERT ohne OFFSET
			;; LCD: +2 # DETUNE USER WERT ohne OFFSET 
			;; LCD: +1 # DETUNE USER WERT ohne OFFSET 
			;; LCD:  0 # DETUNE USER WERT ohne OFFSET
			;; LCD: -1 # DETUNE USER WERT ohne OFFSET
			;; LCD: -2 # DETUNE USER WERT ohne OFFSET
			;; LCD: -3 # DETUNE USER WERT ohne OFFSET
			;; ....
	
			;; Der *Minus* Wert muss aus der Variable (die nur positive zahlen beherbergt) errechnet
			;; werden
			;; ...
			;; LCD: +3 # => DETUNE USER VALUE: 10 - 0x0A (Dieser Bereich wird hier nicht behandelt)
			;; LCD: +2 # => DETUNE USER VALUE:  9 - 0x09 (Dieser Bereich wird hier nicht behandelt)
			;; LCD: +1 # => DETUNE USER VALUE:  8 - 0x08 (Dieser Bereich wird hier nicht behandelt)
			;; LCD:  0 # => DETUNE USER VALUE:  7 - 0x07 (Dieser Bereich wird hier nicht behandelt)
			;; LCD: -1 # => DETUNE USER VALUE:  6 - 0x06 (Dieser Bereich ist hier relevant)
			;; LCD: -2 # => DETUNE USER VALUE:  5 - 0x05 (Dieser Bereich ist hier relevant)
			;; LCD: -3 # => DETUNE USER VALUE:  4 - 0x04 (Dieser Bereich ist hier relevant)
			;; LCD: -4 # => DETUNE USER VALUE:  3 - 0x03 (Dieser Bereich ist hier relevant)
			;; LCD: -5 # => DETUNE USER VALUE:  2 - 0x02 (Dieser Bereich ist hier relevant)
			;; ....
			
			;; :: MINUSZAHLBERECHNUNG FÜR DIE LCD ANZEIGE ::
			;; Aufgabe: Ziehe 0x07 (-OFFSET) vom COARSE TUNE REG A/B/C ab und berechne die Minuszahl
			;; 1) comf 0x07 (comf OFFSET ) => complement 0x07 = 0xF8 // 1er komplement bilden
			;; 2) inc 0xF8 => 0xF9 // 2er komplemnt bilden
			;; 3) add TUNE REG + 0xF9
			;; 4) comf (TUNE REG + 0xF9) // 1er komplement bilden
			;; 5) inc {comf (TUNE REG + 0xF9)} // 2er komplemnt bilden
			
	
			movlw	OFFSET_COARSE_TUNE_IS_7  ; 7 => W
			movwf	MIOS_PARAMETER1			; W => 	MIOS_PARAMETER1
			comf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 1er komplement
			incf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 2er komplemnt
			movf	MIOS_PARAMETER1, W		; MIOS_PARAMETER1 => W
			addwf	AY_TONE_A_TUNE_COARSE_USER, W	; AY_TONE_A_TUNE_COARSE_USER + W => W
			movwf	MIOS_PARAMETER1			; W => MIOS_PARAMETER1
			comf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 1er komplement
			incf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 2er komplemnt
			;; Nun ist die gewünschte MINUSZAHL in MIOS_PARAMETER1 hinterlegt	
		
			; POS FOR TUNE VALUE
			movlw   LCD_ROW2 + 0x08			
			call    MIOS_LCD_CursorSet
			; Print TUNE-VALUE @ LCD
			movf    MIOS_PARAMETER1,W		
			call        MIOS_LCD_PrintBCD1			
        
        		goto LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_A
        
               
        
;; ----[MAIN MENU TONE A - COARSE TUNE END]------------ 

	
LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_A
;; ----[MAIN MENU TONE A - FINE TUNE]------------ 
	;; CHECK IF FINE TUNE TONE A IS POSITIVE (+ZERO) or NEGATIVE 
	movlw	PROOF_POSITIVE_OR_NEGATIVE_VIA_0X7F ;; 127
	;; TONE A COARSE TUNE > 127 DEC.? (128 DEC. = 0 DETUNE)
	cpfsgt 	AY_TONE_A_TUNE_FINE_USER		
			;; NO - VALUE @ LCD WILL BE NEGATIVE
			rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_A_NEGATIVE

			;; YES - VALUE @ LCD WILL BE 0 OR POSITIVE
			rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_A_ZERO_AND_POSITIVE

LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_A_ZERO_AND_POSITIVE
			;; calculate *LCD VALUE* FINE TUNE TONE A 
			movlw	OFFSET_FINE_TUNE_IS_128
			subwf	AY_TONE_A_TUNE_FINE_USER, W
			movwf	MIOS_PARAMETER1
			
				;; CURSER SET
				movlw   LCD_ROW2 + 0x0d
				call    MIOS_LCD_CursorSet
				
				;; Print A COARSE TUNE + *LCD VALUE*
				movf    	MIOS_PARAMETER1,W		
				call        	MIOS_LCD_PrintBCD3
		
					rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_B



	
LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_A_NEGATIVE
			; POS FOR "-"
			movlw   LCD_ROW2 + 0x0c				
			call    MIOS_LCD_CursorSet
			; WRITE "-" to LCD
			movlw   '-'
			call    MIOS_LCD_PrintChar			
				
			;; :: MINUSZAHLBERECHNUNG NUR FÜR DIE LCD ANZEIGE ::
			;; Aufgabe Offset abziehen und MINUSZAHL für die LCD Anzeige berechnen 
			;; 1) comf 0x80 => complement 0x80 => 0x7F
			;; 2) inc 0x7F => 0x80
			;; 3) add TUNE REG + 0x80
			;; 4) comf (TUNE REG + 0x80)
			;; 5) inc {comf (TUNE REG + 0x80)}
			;; 6) PRINT TO LCD: - (inc {comf (TUNE REG + 0x80)})
			
			;; : MEMO :
			;; Die Prozedur ist notwendig da auf dem LCD
			;; +/- Zahlen gewünscht sind - das bedeutet:
			;; ...
			;; LCD: +3 # DETUNE USER WERT ohne OFFSET: 131 - 0x83 (Dieser Bereich wird hier nicht behandelt)
			;; LCD: +2 # DETUNE USER WERT ohne OFFSET: 130 - 0x82 (Dieser Bereich wird hier nicht behandelt)
			;; LCD: +1 # DETUNE USER WERT ohne OFFSET: 129 - 0x81 (Dieser Bereich wird hier nicht behandelt)
			;; LCD:  0 # DETUNE USER WERT ohne OFFSET: 128 - 0x80 (Dieser Bereich wird hier nicht behandelt)
			;; LCD: -1 # DETUNE USER WERT ohne OFFSET: 127 - 0x7F (Dieser Bereich ist hier relevant)
			;; LCD: -2 # DETUNE USER WERT ohne OFFSET: 126 - 0x7E (Dieser Bereich ist hier relevant)
			;; LCD: -3 # DETUNE USER WERT ohne OFFSET: 125 - 0x7D (Dieser Bereich ist hier relevant)
			;; ....
			
			movlw	OFFSET_FINE_TUNE_IS_128	; 0x80 => W
			movwf	MIOS_PARAMETER1			; W => 	MIOS_PARAMETER1
			comf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 1er komplement
			incf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 2er komplemnt
			movf	MIOS_PARAMETER1, W		; MIOS_PARAMETER1 => W
			addwf	AY_TONE_A_TUNE_FINE_USER, W	; AY_TONE_A_TUNE_FINE_USER + W => W
			movwf	MIOS_PARAMETER1			; W => MIOS_PARAMETER1
			comf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 1er komplement
			incf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 2er komplemnt
			;; Das Ergebnis ist nun in MIOS_PARAMETER1 hinterlegt
			
			; POS FOR TUNE VALUE
			movlw   LCD_ROW2 + 0x0d			
			call    MIOS_LCD_CursorSet
			
			; WRITE NEGATIVE *LCD VALUE* FINE TUNE USER TONE A to LCD
			movf    MIOS_PARAMETER1,W		
			call        MIOS_LCD_PrintBCD3			
			
				rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_B
		

;; ----[MAIN MENU TONE A - FINE TUNE END]------------ 



LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_B        
;; ----[MAIN MENU TONE B - COARSE TUNE]------------     
        ;; CHECK IF COARSE (DE)TUNE TONE B is POSITIVE (+0) or NEGATIVE 
	movlw	PROOF_POSITIVE_OR_NEGATIVE_VIA_0X06
	;; TONE A COARSE TUNE > 6?
	cpfsgt 	AY_TONE_B_TUNE_COARSE_USER
			;; NO - VALUE @ LCD IS NEGATIVE
			rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_B_NEGATIVE

			;; YES - VALUE @ LCD IS POSITIVE (or zero)
			rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_B_ZERO_AND_POSITIVE


LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_B_ZERO_AND_POSITIVE

			;; PRINT LCD LINE3 POSIVE VALUE
			; CLEAR ROW3
			TABLE_ADDR LCD_CLEAR_ROW3			
			call    MIOS_LCD_PrintString
			; PRINT TXT ROW3
			TABLE_ADDR TEXT_AY_MENU_ABC_TUNE_CHB_TUNE 	
			call    MIOS_LCD_PrintString
			
			;; calculate the *LCD* value
			movlw	OFFSET_COARSE_TUNE_IS_7
			subwf	AY_TONE_B_TUNE_COARSE_USER, W
			;; save the result to MIOS_PARAMETER1
			movwf	MIOS_PARAMETER1
			
			;; SET THE CURSOR POS
			movlw   LCD_ROW3 + 0x08
			call    MIOS_LCD_CursorSet
				; WRITE USER COARSE TUNE TONE B to LCD
				movf    	MIOS_PARAMETER1,W		
				call        	MIOS_LCD_PrintBCD1
					
					rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_B
        


LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_B_NEGATIVE
			;; WRITE USER DETUNE COARSE TONE B VALUE 
			;; (is a NEGATIVE value) to  LCD LINE3 
				
			;; CLEAR ROW3	
			TABLE_ADDR LCD_CLEAR_ROW3			
			call    MIOS_LCD_PrintString
			
			; PRINT ITEM ROW3
			TABLE_ADDR TEXT_AY_MENU_ABC_TUNE_CHB_TUNE 	
			call    MIOS_LCD_PrintString
			
			; SET LCD POS FOR "-"
			movlw   LCD_ROW3 + 0x06				
			call    MIOS_LCD_CursorSet
			; WRITE "-" to LCD
			movlw   '-'
			call    MIOS_LCD_PrintChar	
		
			;; :: MINUSZAHLBERECHNUNG FÜR DIE LCD ANZEIGE ::
			;; Ziehe 0x07 (-OFFSET) vom COARSE TUNE REG A/B/C ab und berechne
			;; die MINUSZAHL für die LCD Anzeige
			;; 1) comf 0x07 (comf OFFSET ) = complement 0x07 = 0xF8 // 1er komplement bilden
			;; 2) inc 0xF8 => 0xF9 //  2er komplemnt bilden
			;; 3) add TUNE REG + 0xF9
			;; 4) comf (TUNE REG + 0xF9) //  1er komplement bilden
			;; 5) inc {comf (TUNE REG + 0xF9)} // 2er komplemnt bilden
			
			;; : MEMO :
			;; Die Prozedur ist notwendig da auf dem LCD
			;; +/- Zahlen gewünscht sind - das bedeutet:
			;; ...
			;; LCD: +3 # DETUNE USER WERT ohne OFFSET:10 - 0x0A (Dieser Bereich wird hier nicht behandelt)
			;; LCD: +2 # DETUNE USER WERT ohne OFFSET: 9 - 0x09 (Dieser Bereich wird hier nicht behandelt)
			;; LCD: +1 # DETUNE USER WERT ohne OFFSET: 8 - 0x08 (Dieser Bereich wird hier nicht behandelt)
			;; LCD:  0 # DETUNE USER WERT ohne OFFSET: 7 - 0x07 (Dieser Bereich wird hier nicht behandelt)
			;; LCD: -1 # DETUNE USER WERT ohne OFFSET: 6 - 0x06 (Dieser Bereich ist hier relevant)
			;; LCD: -2 # DETUNE USER WERT ohne OFFSET: 5 - 0x05 (Dieser Bereich ist hier relevant)
			;; LCD: -3 # DETUNE USER WERT ohne OFFSET: 4 - 0x04 (Dieser Bereich ist hier relevant)
			;; ....
			
			
			movlw	OFFSET_COARSE_TUNE_IS_7  ; 7 => W
			movwf	MIOS_PARAMETER1			; W => 	MIOS_PARAMETER1
			comf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 1er komplement
			incf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 2er komplemnt
			movf	MIOS_PARAMETER1, W		; MIOS_PARAMETER1 => W
			addwf	AY_TONE_B_TUNE_COARSE_USER, W	; AY_TONE_B_TUNE_COARSE_USER + W => W
			movwf	MIOS_PARAMETER1			; W => MIOS_PARAMETER1
			comf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 1er komplement
			incf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 2er komplemnt
		
			
			; SET LCD POS
			movlw   LCD_ROW3 + 0x08			
			call    MIOS_LCD_CursorSet
			; WRITE the result to LCD
			movf    MIOS_PARAMETER1,W		
			call        MIOS_LCD_PrintBCD1			
			
				rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_B
                
;; ----[MAIN MENU TONE B - COARSE TUNE END]------------ 

LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_B
;; ----[MAIN MENU TONE B - FINE (DE)TUNE]------------ 
	;; CHECK: IS USER DETUNE TONE B POSITIVE (+zero) or  NEGATIVE
	movlw	PROOF_POSITIVE_OR_NEGATIVE_VIA_0X7F
	;; USER DETUNE FINE TONE B > 127? (OFFSET = 128)
	;; 127 = -1
	;; 128 =  0
	;; 129 = +1
	cpfsgt 	AY_TONE_B_TUNE_FINE_USER
			;; NO - VALUE @ LCD WILL BE NEGATIVE
			rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_B_NEGATIVE

			;; YES - VALUE @ LCD WILL BE POSITIVE
			rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_B_ZERO_AND_POSITIVE

LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_B_ZERO_AND_POSITIVE

			movlw	OFFSET_FINE_TUNE_IS_128
			subwf	AY_TONE_B_TUNE_FINE_USER, W
			movwf	MIOS_PARAMETER1
			
			movlw   LCD_ROW3 + 0x0d
			call    MIOS_LCD_CursorSet
			
				movf    	MIOS_PARAMETER1,W		; Print A COARSE TUNE + *LCD VALUE*
				call        	MIOS_LCD_PrintBCD3
		
					rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_C



	
LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_B_NEGATIVE

			movlw   LCD_ROW3 + 0x0c				; POS FOR "-"
			call    MIOS_LCD_CursorSet
		
			movlw   '-'
			call    MIOS_LCD_PrintChar			; PRINT "-"
		
			;; :: MINUSZAHLBERECHNUNG FÜR DIE LCD ANZEIGE ::
			;; 1) comf 0x80 => complement 0x80 => 0x7F
			;; 2) inc 0x7F => 0x80
			;; 3) add TUNE REG + 0x80
			;; 4) comf (TUNE REG + 0x80)
			;; 5) inc {comf (TUNE REG + 0x80)}
			;; 6) PRINT TO LCD: - (inc {comf (TUNE REG + 0x80)})
			
			;; : MEMO :
			;; Die Prozedur ist notwendig da auf dem LCD
			;; +/- Zahlen gewünscht sind - das bedeutet:
			;; ...
			;; LCD: +3 # DETUNE USER WERT ohne OFFSET: 131 - 0x83 (Dieser Bereich wird hier nicht behandelt)
			;; LCD: +2 # DETUNE USER WERT ohne OFFSET: 130 - 0x82 (Dieser Bereich wird hier nicht behandelt)
			;; LCD: +1 # DETUNE USER WERT ohne OFFSET: 129 - 0x81 (Dieser Bereich wird hier nicht behandelt)
			;; LCD:  0 # DETUNE USER WERT ohne OFFSET: 128 - 0x80 (Dieser Bereich wird hier nicht behandelt)
			;; LCD: -1 # DETUNE USER WERT ohne OFFSET: 127 - 0x7F (Dieser Bereich ist hier relevant)
			;; LCD: -2 # DETUNE USER WERT ohne OFFSET: 126 - 0x7E (Dieser Bereich ist hier relevant)
			;; LCD: -3 # DETUNE USER WERT ohne OFFSET: 125 - 0x7D (Dieser Bereich ist hier relevant)
			;; ....
			
			movlw	OFFSET_FINE_TUNE_IS_128	; 0x80 => W
			movwf	MIOS_PARAMETER1			; W => 	MIOS_PARAMETER1
			comf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 1er komplement
			incf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 2er komplemnt
			movf	MIOS_PARAMETER1, W		; MIOS_PARAMETER1 => W
			addwf	AY_TONE_B_TUNE_FINE_USER, W	; AY_TONE_B_TUNE_FINE_USER + W => W
			movwf	MIOS_PARAMETER1			; W => MIOS_PARAMETER1
			comf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 1er komplement
			incf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 2er komplemnt
		
			
			movlw   LCD_ROW3 + 0x0d			; POS FOR TUNE VALUE
			call    MIOS_LCD_CursorSet
			
			movf    MIOS_PARAMETER1,W		
			call        MIOS_LCD_PrintBCD3			; Print TUNE-VALUE @ LCD
			
				rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_C
		

;; ----[MAIN MENU TONE B - FINE TUNE END]------------ 



LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_C        
;; ----[MAIN MENU TONE C - COARSE TUNE]------------     
        ;; CHECK POSITIVE / NEGATIVE TUNE
	movlw	PROOF_POSITIVE_OR_NEGATIVE_VIA_0X06
	;; TONE A COARSE TUNE > 6?
	cpfsgt 	AY_TONE_C_TUNE_COARSE_USER
			
			;; NO - VALUE @ LCD WILL BE NEGATIVE
			rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_C_NEGATIVE

			;; YES - VALUE @ LCD WILL BE POSITIVE
			rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_C_ZERO_AND_POSITIVE



LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_C_ZERO_AND_POSITIVE
			;; PRINT LCD LINE4 POSIVE VALUE
			
			; CLEAR ROW4
			TABLE_ADDR LCD_CLEAR_ROW4			
			call    MIOS_LCD_PrintString
			
			; PRINT ITEM ROW4
			TABLE_ADDR TEXT_AY_MENU_ABC_TUNE_CHC_TUNE 	
			call    MIOS_LCD_PrintString
			
			;; calculate *LCD VALUE*
			movlw	OFFSET_COARSE_TUNE_IS_7
			subwf	AY_TONE_C_TUNE_COARSE_USER, W
			movwf	MIOS_PARAMETER1
			
			;; SET LCD POS
			movlw   LCD_ROW4 + 0x08
			call    MIOS_LCD_CursorSet
			
				; Print A COARSE TUNE *LCD VALUE*
				movf    	MIOS_PARAMETER1,W		
				call        	MIOS_LCD_PrintBCD1
		
					rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_C
		


LABEL_AY_MAIN_MENU_02_CHK_TUNE_COARSE_C_NEGATIVE
			;; PRINT LCD LINE4 NEGATIVE VALUE
			
			
			; CLEAR ROW4
			TABLE_ADDR LCD_CLEAR_ROW4			
			call    MIOS_LCD_PrintString
			
			;; WRITE LCD ITEM LINE4
			TABLE_ADDR TEXT_AY_MENU_ABC_TUNE_CHC_TUNE
			call    MIOS_LCD_PrintString
			
			; POS FOR "-"
			movlw   LCD_ROW4 + 0x06				
			call    MIOS_LCD_CursorSet
			; PRINT "-"
			movlw   '-'
			call    MIOS_LCD_PrintChar			
		
			
			;; :: MINUSZAHLBERECHNUNG FÜR DIE LCD ANZEIGE ::
			;; Ziehe 0x07 vom TUNE REG A/B/C ab und errechne den
			;; MINUSWERT für die LCD Anzeige:
			;; 1) comf 0x07 => complement 0x07 => 0xF8
			;; 2) inc 0xF8 => 0xF9
			;; add TUNE REG + 0xF9
			;; 3) comf (TUNE REG + 0xF9)
			;; 4) inc {comf (TUNE REG + 0xF9)}
			
			
			movlw	OFFSET_COARSE_TUNE_IS_7	; 7 => W
			movwf	MIOS_PARAMETER1			; W => 	MIOS_PARAMETER1
			comf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 1er komplement
			incf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 2er komplemnt
			movf	MIOS_PARAMETER1, W		; MIOS_PARAMETER1 => W
			addwf	AY_TONE_C_TUNE_COARSE_USER, W	; AY_TONE_C_TUNE_COARSE_USER + W => W
			movwf	MIOS_PARAMETER1			; W => MIOS_PARAMETER1
			comf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 1er komplement
			incf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 2er komplemnt
		;; Nun ist das Resultat MIOS_PARAMETER1 in hinterlegt
			
			movlw   LCD_ROW4 + 0x08			; POS FOR TUNE VALUE
			call    MIOS_LCD_CursorSet
			
			; WRITE VALUE => LCD
			movf    MIOS_PARAMETER1,W		
			call        MIOS_LCD_PrintBCD1			
			
				rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_C
        	
               
;; ----[MAIN MENU TONE C - COARSE TUNE END]------------ 



LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_C
;; ----[MAIN MENU TONE C - FINE TUNE]------------ 
	;; CHECK POSITIVE / NEGATIVE 
	movlw	PROOF_POSITIVE_OR_NEGATIVE_VIA_0X7F
	;; TONE A FINE DETUNE > 127?
	cpfsgt 	AY_TONE_C_TUNE_FINE_USER
			;; NO - VALUE @ LCD WILL BE NEGATIVE
			rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_C_NEGATIVE

			;; YES - VALUE @ LCD WILL BE 0 or POSITIVE
			rgoto LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_C_ZERO_AND_POSITIVE

LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_C_ZERO_AND_POSITIVE
			;; Berechnung des Pluswertes für die LCD Anzeige
			;; Der Tune Wert bleibt davon unberührt
			movlw	OFFSET_FINE_TUNE_IS_128
			subwf	AY_TONE_C_TUNE_FINE_USER, W
			movwf	MIOS_PARAMETER1
			
			movlw   LCD_ROW4 + 0x0d
			call    MIOS_LCD_CursorSet
			
				movf    	MIOS_PARAMETER1,W		; Print C FINE TUNE + *LCD VALUE*
				call        	MIOS_LCD_PrintBCD3
		
					rgoto LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS1



	
LABEL_AY_MAIN_MENU_02_CHK_TUNE_FINE_C_NEGATIVE
			; POS FOR "-"
			movlw   LCD_ROW4 + 0x0c				
			call    MIOS_LCD_CursorSet
			; PRINT "-"
			movlw   '-'
			call    MIOS_LCD_PrintChar			
		
			;; :: MINUSZAHLBERECHNUNG FÜR DIE LCD ANZEIGE ::
			;; Ziehe 0x80 vom FINE TUNE REG A/B/C ab und berechne den Minuswert 
			;; für die LCD Anzeige
			
			movlw	OFFSET_FINE_TUNE_IS_128	; 0x80 => W
			movwf	MIOS_PARAMETER1			; W => 	MIOS_PARAMETER1
			comf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 1er komplement
			incf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 2er komplemnt
			movf	MIOS_PARAMETER1, W		; MIOS_PARAMETER1 => W
			addwf	AY_TONE_C_TUNE_FINE_USER, W	; AY_TONE_C_TUNE_COARSE_USER + W => W
			movwf	MIOS_PARAMETER1			; W => MIOS_PARAMETER1
			comf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 1er komplement
			incf	MIOS_PARAMETER1, F		; MIOS_PARAMETER1 2er komplemnt
		
			
			movlw   LCD_ROW4 + 0x0d			; POS FOR TUNE VALUE
			call    MIOS_LCD_CursorSet
			
			movf    MIOS_PARAMETER1,W		
			call        MIOS_LCD_PrintBCD3			; Print TUNE-VALUE @ LCD
			
				rgoto LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS1
		

;; ----[MAIN MENU TONE C - FINE TUNE END]------------ 


;; -----------------------------------------------------------------------------------


LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS1
;; MAIN MENU 2 => CH ABC TUNE:
;; Show the POSITION of * [LCD: MAIN MENU EDIT POINTER] for Main Menu EDITING Posiotion

	;; CH A COARSE:
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE = 1?
	movff AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE, MIOS_PARAMETER1
	
	movlw MENU_ITEM_POS1
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto LABEL_AY_MAIN_MENU_02_NOT_POS1

		;; YES
		rgoto LABEL_AY_MAIN_MENU_02_POS1


LABEL_AY_MAIN_MENU_02_POS1
		;; CH A COARSE EDIT MODE *ON*
		;; PRINT "*" ON EDIT POSITION1
		movlw 	LCD_ROW2 + 5
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
			
				rgoto LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS2


LABEL_AY_MAIN_MENU_02_NOT_POS1
		;; CH A COARSE EDIT MODE *OFF*
		;; PRINT ":" ON EDIT POSITION1
		movlw 	LCD_ROW2 + 5
		call    MIOS_LCD_CursorSet
		movlw   ':'
		call    MIOS_LCD_PrintChar
			
				rgoto LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS2


	;; --------------------------------
	
LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS2
	;; CH A FINE:
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE = 2?
	movff AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE, MIOS_PARAMETER1
	
	movlw MENU_ITEM_POS2
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto LABEL_AY_MAIN_MENU_02_NOT_POS2

		;; YES
		rgoto LABEL_AY_MAIN_MENU_02_POS2


LABEL_AY_MAIN_MENU_02_POS2
		;; EDIT MODE *ON* @ CH A FINE
		;; PRINT "*" ON EDIT POSITION2
		movlw 	LCD_ROW2 + 0x0b
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
			
				rgoto LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS3


LABEL_AY_MAIN_MENU_02_NOT_POS2
		;; EDIT MODE *OFF* @ CH A FINE
		;; PRINT ":" ON EDIT POSITION2
		movlw 	LCD_ROW2 + 0x0b
		call    MIOS_LCD_CursorSet
		movlw   ':'
		call    MIOS_LCD_PrintChar
			
				rgoto LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS3


	;; --------------------------------


LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS3

	;; CH B COARSE TUNE:
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE = 3?
	movff AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE, MIOS_PARAMETER1
	
	movlw MENU_ITEM_POS3
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto LABEL_AY_MAIN_MENU_02_NOT_POS3

		;; YES
		rgoto LABEL_AY_MAIN_MENU_02_POS3


LABEL_AY_MAIN_MENU_02_POS3
		;; EDIT MODE *ON* @ CH B COARSE TUNE
		;; PRINT "*" ON EDIT POSITION3
		movlw 	LCD_ROW3 + 5
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
			
				rgoto LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS4


LABEL_AY_MAIN_MENU_02_NOT_POS3
		;; EDIT MODE *OFF* @ CH B COARSE TUNE
		;; PRINT ":" ON EDIT POSITION3
		movlw 	LCD_ROW3 + 5
		call    MIOS_LCD_CursorSet
		movlw   ':'
		call    MIOS_LCD_PrintChar
			
				rgoto LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS4


	;; --------------------------------



LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS4

	;; CH B FINE TUNE:
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE = 4?
	movff AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE, MIOS_PARAMETER1
	
	movlw MENU_ITEM_POS4
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto LABEL_AY_MAIN_MENU_02_NOT_POS4

		;; YES
		rgoto LABEL_AY_MAIN_MENU_02_POS4


LABEL_AY_MAIN_MENU_02_POS4
		;; CH B FINE TUNE EDIT MODE *ON*
		;; PRINT "*" ON EDIT POSITION4
		movlw 	LCD_ROW3 + 0x0b
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
			
				rgoto LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS5


LABEL_AY_MAIN_MENU_02_NOT_POS4
		;; CH B FINE TUNE EDIT MODE *OFF*
		;; PRINT ":" ON EDIT POSITION4
		movlw 	LCD_ROW3 + 0x0b
		call    MIOS_LCD_CursorSet
		movlw   ':'
		call    MIOS_LCD_PrintChar
			
				rgoto LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS5


;;--------


LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS5

	;; CH C COARSE TUNE:
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE = 5?
	movff AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE, MIOS_PARAMETER1
	
	movlw MENU_ITEM_POS5
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto LABEL_AY_MAIN_MENU_02_NOT_POS5

		;; YES
		rgoto LABEL_AY_MAIN_MENU_02_POS5


LABEL_AY_MAIN_MENU_02_POS5
		;; CH C COARSE TUNE EDIT MODE *ON*
		;; PRINT "*" ON EDIT POSITION4
		movlw 	LCD_ROW4 + 0x05
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
			
				rgoto LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS6


LABEL_AY_MAIN_MENU_02_NOT_POS5
		;; CH C COARSE TUNE EDIT MODE *OFF*
		;; PRINT ":" ON EDIT POSITION4
		movlw 	LCD_ROW4 + 0x05
		call    MIOS_LCD_CursorSet
		movlw   ':'
		call    MIOS_LCD_PrintChar
			
				rgoto LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS6

;; -------

LABEL_AY_MAIN_MENU_02_FIND_EDIT_POS6
	;; CH C FINE TUNE:
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE = 6?
	movff AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE, MIOS_PARAMETER1
	
	movlw MENU_ITEM_POS6
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto LABEL_AY_MAIN_MENU_02_NOT_POS6

		;; YES
		rgoto LABEL_AY_MAIN_MENU_02_POS6


LABEL_AY_MAIN_MENU_02_POS6
		;; CH C FINE TUNE EDIT MODE *ON*
		;; PRINT "*" ON EDIT POSITION4
		movlw 	LCD_ROW4 + 0x0b
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
			
				rgoto LABEL_AY_MAIN_MENU_02_DONE


LABEL_AY_MAIN_MENU_02_NOT_POS6
		;; CH C FINE TUNE EDIT MODE *OFF*
		;; PRINT ":" ON EDIT POSITION4
		movlw 	LCD_ROW4 + 0x0b
		call    MIOS_LCD_CursorSet
		movlw   ':'
		call    MIOS_LCD_PrintChar
			
				rgoto LABEL_AY_MAIN_MENU_02_DONE

	
;; --------------------------------

LABEL_AY_MAIN_MENU_02_DONE
	;; END of MAIN MENU 2 ROUTINE
	return
	
	
