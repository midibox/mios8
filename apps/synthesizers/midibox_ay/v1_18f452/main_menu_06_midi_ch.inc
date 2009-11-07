;; main_menu_06_midi_ch.inc


;; ----------------------------------------------------------------------------
;; -------------------------[MB AY MENU 6: MIDI CH A B C CONFIG]----------------------------- 
;; =======================> (F6 has been pressed)


LABEL_AY_MAIN_MENU_06_MIDI_CONFIG

	call    MIOS_LCD_Clear
	
	;; LCD ROW 1 TO 4
   	TABLE_ADDR TEXT_AY_MIDI_CONFIG_ROW1
	call    MIOS_LCD_PrintString
	
	TABLE_ADDR TEXT_AY_MIDI_CONFIG_ROW2
	call    MIOS_LCD_PrintString
	
	movlw   LCD_ROW2 + 14
        call    MIOS_LCD_CursorSet
        movf    AY_TONE_A_MIDI_CHANNEL_USER, W
        addlw	MIDI_CH_PLUS_1_OFFSETT ;; MIDI CH. 0-15 => 1 - 16
        call    MIOS_LCD_PrintBCD2
	
	TABLE_ADDR TEXT_AY_MIDI_CONFIG_ROW3
	call    MIOS_LCD_PrintString
	
	movlw   LCD_ROW3 + 14
        call    MIOS_LCD_CursorSet
        movf    AY_TONE_B_MIDI_CHANNEL_USER, W
        addlw	MIDI_CH_PLUS_1_OFFSETT ;; MIDI CH. 0-15 => 1 - 16
        call    MIOS_LCD_PrintBCD2
	
	TABLE_ADDR TEXT_AY_MIDI_CONFIG_ROW4
	call    MIOS_LCD_PrintString
	
	movlw   LCD_ROW4 + 14
        call    MIOS_LCD_CursorSet
        movf    AY_TONE_C_MIDI_CHANNEL_USER, W
        addlw	MIDI_CH_PLUS_1_OFFSETT ;; MIDI CH. 0-15 => 1 - 16
        call    MIOS_LCD_PrintBCD2


LABEL_AY_MAIN_MENU_06_MIDI_CONFIG_FIND_EDIT_POS1
;; Show the POSITION of * 
;; [LCD: MAIN MENU EDIT POINTER] 
;; for Main Menu EDITING Posiotion
;; WHICH ITEM IS IN EDITING MODE?

	;; EDITING MODE @ Tone A?
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_MIDI_CONFIG = 1?
	movff AY_LCD_COURSER_POS_MAIN_MENU_MIDI_CONFIG, MIOS_PARAMETER1
	movlw MENU_ITEM_POS1	; WRITE 1 TO WREG
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto LABEL_AY_MAIN_MENU_06_NOT_POS1

		;; YES
		rgoto LABEL_AY_MAIN_MENU_06_POS1        	

LABEL_AY_MAIN_MENU_06_NOT_POS1
	;; NO EDITING MODE @ MIDI CH TONE A 
	;; PRINT ":" ON EDIT POSITION1
	movlw 	LCD_ROW2 + 12
	call    MIOS_LCD_CursorSet
	movlw   ':'
        call    MIOS_LCD_PrintChar
		  rgoto LABEL_AY_MAIN_MENU_06_MIDI_CONFIG_FIND_EDIT_POS2


LABEL_AY_MAIN_MENU_06_POS1
	;; EDITING MODE @ MIDI CH TONE A
	;; PRINT "*" ON EDIT POSITION1
	movlw 	LCD_ROW2 + 12
	call    MIOS_LCD_CursorSet
	movlw   '*'
        call    MIOS_LCD_PrintChar
		  rgoto LABEL_AY_MAIN_MENU_06_MIDI_CONFIG_FIND_EDIT_POS2


LABEL_AY_MAIN_MENU_06_MIDI_CONFIG_FIND_EDIT_POS2
	;; EDITING MODE @ MIDI CH Tone B?
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_MIDI_CONFIG = 2?
	movff AY_LCD_COURSER_POS_MAIN_MENU_MIDI_CONFIG, MIOS_PARAMETER1
	movlw MENU_ITEM_POS2	; WRITE 2 TO WREG
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto LABEL_AY_MAIN_MENU_06_NOT_POS2

		;; YES
		rgoto LABEL_AY_MAIN_MENU_06_POS2        	

LABEL_AY_MAIN_MENU_06_NOT_POS2
	;; NO EDITING MODE @ MIDI CH TONE B 
	;; PRINT ":" ON EDIT POSITION 2
	movlw 	LCD_ROW3 + 12
	call    MIOS_LCD_CursorSet
	movlw   ':'
        call    MIOS_LCD_PrintChar
		  rgoto LABEL_AY_MAIN_MENU_06_MIDI_CONFIG_FIND_EDIT_POS3


LABEL_AY_MAIN_MENU_06_POS2
	;; EDITING MODE @ MIDI CH TONE B
	;; PRINT "*" ON EDIT POSITION 2
	movlw 	LCD_ROW3 + 12
	call    MIOS_LCD_CursorSet
	movlw   '*'
        call    MIOS_LCD_PrintChar
		  rgoto LABEL_AY_MAIN_MENU_06_MIDI_CONFIG_FIND_EDIT_POS3




LABEL_AY_MAIN_MENU_06_MIDI_CONFIG_FIND_EDIT_POS3
	;; EDITING MODE @ MIDI CH Tone C?
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_MIDI_CONFIG = 3?
	movff 	AY_LCD_COURSER_POS_MAIN_MENU_MIDI_CONFIG, MIOS_PARAMETER1
	movlw 	MENU_ITEM_POS3	; WRITE 3 TO WREG
	cpfseq 	MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto LABEL_AY_MAIN_MENU_06_NOT_POS3

		;; YES
		rgoto LABEL_AY_MAIN_MENU_06_POS3        	

LABEL_AY_MAIN_MENU_06_NOT_POS3
	;; NO EDITING MODE @ MIDI CH TONE C 
	;; PRINT ":" ON EDIT POSITION 3
	movlw 	LCD_ROW4 + 12
	call    MIOS_LCD_CursorSet
	movlw   ':'
        call    MIOS_LCD_PrintChar
		  rgoto LABEL_AY_MAIN_MENU_06_MIDI_CONFIG_DONE


LABEL_AY_MAIN_MENU_06_POS3
	;; EDITING MODE @ MIDI CH TONE C
	;; PRINT "*" ON EDIT POSITION 3
	movlw 	LCD_ROW4 + 12
	call    MIOS_LCD_CursorSet
	movlw   '*'
        call    MIOS_LCD_PrintChar
		  rgoto LABEL_AY_MAIN_MENU_06_MIDI_CONFIG_DONE

LABEL_AY_MAIN_MENU_06_MIDI_CONFIG_DONE
	
		return

;; ------------------------[END OF MAIN MENU 6 FUNCTIONS]----------------------------------
