;; start_screen.inc

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

		
		;; wait for exact 255 ms
		;; THAT THE USER CAN READ THE STARTSCREEN
		clrw
		movwf MIOS_PARAMETER1			

AY_START_SCREEN_SHOW_TIME		
				
		;; one waitcycle 255 ms
		movlw   255
		call    MIOS_Delay
				
		incf MIOS_PARAMETER1, F
			;; MIDIBOX AY START SCREEN TIME 
			;; Adjust @ app_defines.h
			movlw START_SCREEN_TIME 
			cpfseq 	MIOS_PARAMETER1
				
				;; WAIT
				;; Do another wait cycle	
				rgoto AY_START_SCREEN_SHOW_TIME
					
				;; EXIT START SCREEN NOW
				return

