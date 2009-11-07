;; start_screen.inc
;; V0.1

AY_START_SCREEN
;; SHOW MIDIBOX USER THE LCD START SCREEN
        	TABLE_ADDR TEXT_AY_START_SCREEN_ROW1
        	call    MIOS_LCD_PrintString

		TABLE_ADDR TEXT_AY_START_SCREEN_ROW2
		call    MIOS_LCD_PrintString

		TABLE_ADDR TEXT_AY_START_SCREEN_ROW3
		call    MIOS_LCD_PrintString
		
		TABLE_ADDR TEXT_AY_START_SCREEN_ROW4
		call    MIOS_LCD_PrintString


		;; request a display update
		bsf	DISPLAY_UPDATE_REQ, 0


		
			;; WAIT THAT THE USER CAN READ THE START SCREEN
			;; wait for exact 255 ms
			clrw
			movwf MIOS_PARAMETER1			

AY_START_SCREEN_SHOW_TIME		
				
			;; wait for exact 255 ms
			movlw        255
			call        MIOS_Delay
		
		
			incf MIOS_PARAMETER1, F
				
				;; MIDIBOX AY START SCREEN TIME 
				movlw START_SCREEN_TIME ;; Adjust @ app_defines.h
				cpfseq 	MIOS_PARAMETER1
					
					;; WAIT										
					rgoto AY_START_SCREEN_SHOW_TIME
					
					;; EXIT START SCREEN NOW
					return