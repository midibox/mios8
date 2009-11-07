;; main_menu_08_fx.inc

;; Function of Fx Menu 08:
;; Some kind of "Tracker Option" for the MIDIBox AY
;; Writes Values to AY REGISTER (timed by TIMER_FX(Timer0) and midi note on/off)
;; See => USER_Timer @ main.asm / fx_timer.inc (polling)
;; See => INIT_TIMER0 @ midibox_ay_ini.inc (timer0 ini)

;; -------------------------------------------------
;; -------------------------[MB AY MENU 8]---------- 
;; =======================> (F8 has been pressed)


LABEL_AY_MAIN_MENU_08_FX
 	;; LOAD FX DATA @ CURREND POS for LCD
 	call LOAD_FX_DATA_LCD_CURREND_POS
 
	;; SHOW MIDIBOX AY MENU SWITCH SCREEN
	call    MIOS_LCD_Clear
	
	;; WRITE LCD ROW 1 to 4
   	TABLE_ADDR TEXT_MB_AY_MENUE_FX_ROW1
	call    MIOS_LCD_PrintString
		 movlw   LCD_ROW1 + 14
        	 call    MIOS_LCD_CursorSet
        	 movf    FX_POS_CURREND, W
        	 call    MIOS_LCD_PrintBCD2 ;; 0-31

	
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_ROW2
	call    MIOS_LCD_PrintString
		movlw    LCD_ROW2 + 13
        	 call    MIOS_LCD_CursorSet
        	 movf    FX_TIME_PRE_EXECUTION_CURREND, W
        	 call    MIOS_LCD_PrintBCD3 

	;; Fx FUNCTION
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_ROW3
	call    MIOS_LCD_PrintString
		;; DECODE and WRITE CURREND 
		;; FUNCTION NAME TO LCD
		call 	DECODE_FX_FUNCTION_FOR_LCD

		
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_ROW4
	call    MIOS_LCD_PrintString
		movlw   LCD_ROW4 + 13
        	 call    MIOS_LCD_CursorSet
        	 movf    FX_DATA_CURREND, W
        	 call    MIOS_LCD_PrintBCD3
	
	

MAIN_MENU_08_FX_CHK_EDIT_MODE_POS1
	;; IS EDIT MODE POS =1?
	movff AY_LCD_COURSER_POS_MAIN_MENU_FX, MIOS_PARAMETER1
	movlw MENU_ITEM_POS1; WRITE 1 TO WREG
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto MAIN_MENU_08_NOT_EDIT_MODE_POS1

	;; YES
	;; MAIN MENU 08 EDIT MODE = POS1
	;; EDITING MODE @ Fx Pos
	;; PRINT "*" ON EDIT POSITION1
	movlw 	LCD_ROW1 + 11
	call    MIOS_LCD_CursorSet
	movlw   '*'
	call    MIOS_LCD_PrintChar
	
	movlw 	LCD_ROW1 + 12
	call    MIOS_LCD_CursorSet
	movlw   '>'
	call    MIOS_LCD_PrintChar
			;; Check POS2
			rgoto MAIN_MENU_08_CHK_EDIT_POS2


MAIN_MENU_08_NOT_EDIT_MODE_POS1
	;; NO EDITING MODE @ Fx Pos 
	;; PRINT "=" ON POS 1
	movlw 	LCD_ROW1 + 11
	call    MIOS_LCD_CursorSet
	movlw   '='
	call    MIOS_LCD_PrintChar
	;; NEXT CHECK POS2 
	
;; -------------------[POS2 ?]------------------------------------
MAIN_MENU_08_CHK_EDIT_POS2
	;; IS EDIT POS = 2?
	movff AY_LCD_COURSER_POS_MAIN_MENU_FX, MIOS_PARAMETER1
	movlw MENU_ITEM_POS2; WRITE 2 TO WREG
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto MAIN_MENU_08_EDIT_MODE_IS_NOT_POS2

	;; YES
	;; PRINT "*" ON EDIT POSITION2
	movlw 	LCD_ROW2 + 11
	call    MIOS_LCD_CursorSet
	movlw   '*'
	call    MIOS_LCD_PrintChar
	
	movlw 	LCD_ROW2 + 12
	call    MIOS_LCD_CursorSet
	movlw   '>'
	call    MIOS_LCD_PrintChar
			;; GO AND CHECK POS3
			rgoto MAIN_MENU_08_CHK_EDIT_POS3


MAIN_MENU_08_EDIT_MODE_IS_NOT_POS2
	;; PRINT "=" ON EDIT POSITION2
	movlw 	LCD_ROW2 + 11
	call    MIOS_LCD_CursorSet
	movlw   '='
	call    MIOS_LCD_PrintChar
	;; GO CHECK POS3

;; -------------------[POS3 ?]------------------------------------	
MAIN_MENU_08_CHK_EDIT_POS3
	;; IS EDITING MODE POS = 3?
	movff AY_LCD_COURSER_POS_MAIN_MENU_FX, MIOS_PARAMETER1
	movlw MENU_ITEM_POS3; WRITE 3 TO WREG
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto MAIN_MENU_08_EDITING_MODE_IS_NOT_POS3

	;; YES
	;; MAIN MENU 08 EDITING MODE = POS3	
	;; Fx
	;; PRINT "*" ON EDIT POSITION3
	movlw 	LCD_ROW3 + 3
	call    MIOS_LCD_CursorSet
	movlw   '*'
	call    MIOS_LCD_PrintChar
		
	movlw 	LCD_ROW3 + 4
	call    MIOS_LCD_CursorSet
	movlw   '>'
	call    MIOS_LCD_PrintChar
		
		;; CHECK POS4
		rgoto MAIN_MENU_08_CHK_EDITING_MODE_POS4


MAIN_MENU_08_EDITING_MODE_IS_NOT_POS3
	;; Fx
	;; PRINT "=" ON EDIT POSITION3
	movlw 	LCD_ROW3 + 3
	call    MIOS_LCD_CursorSet
	movlw   '='
	call    MIOS_LCD_PrintChar
	;; GO ON WITH POS4

;; -------------------[POS4 ?]------------------------------------
MAIN_MENU_08_CHK_EDITING_MODE_POS4
	;; IS EDING MODE POS = 4?
	movff AY_LCD_COURSER_POS_MAIN_MENU_FX, MIOS_PARAMETER1
	movlw MENU_ITEM_POS4; WRITE 4 TO WREG
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
		;; NO
		rgoto MAIN_MENU_08_EDITING_MODE_IS_NOT_POS4

	;; YES
	;; PRINT "*" ON EDIT POSITION4
	movlw 	LCD_ROW4 + 11
	call    MIOS_LCD_CursorSet
	movlw   '*'
        call    MIOS_LCD_PrintChar
	
	movlw 	LCD_ROW4 + 12
	call    MIOS_LCD_CursorSet
	movlw   '>'
	call    MIOS_LCD_PrintChar	
			;; Exit Here
  			return


MAIN_MENU_08_EDITING_MODE_IS_NOT_POS4
	;; NO EDITING MODE @ NOISE PERIOD
	;; PRINT "=" ON EDIT POSITION4
	movlw 	LCD_ROW4 + 11
	call    MIOS_LCD_CursorSet
	movlw   '='
        call    MIOS_LCD_PrintChar
 			;; Exit Here
 			return


;; -------------------------------------------------------

DECODE_FX_FUNCTION_FOR_LCD

        ;; DECODE Fx FUNCTION
	movf FX_FUNCTION_CURREND, W
	JUMPTABLE_2BYTES 42 ;; [0 to 41]
        	rgoto        LCD_FX_FUNC_000	;; 0x00
        	rgoto        LCD_FX_FUNC_001	;; 0x01
        	rgoto        LCD_FX_FUNC_002	;; ...
        	rgoto        LCD_FX_FUNC_003
        	rgoto        LCD_FX_FUNC_004
        	rgoto        LCD_FX_FUNC_005
        	rgoto        LCD_FX_FUNC_006
        	rgoto        LCD_FX_FUNC_007
        	rgoto        LCD_FX_FUNC_008
        	rgoto        LCD_FX_FUNC_009
        	rgoto        LCD_FX_FUNC_010
        	rgoto        LCD_FX_FUNC_011
        	rgoto        LCD_FX_FUNC_012
        	rgoto        LCD_FX_FUNC_013
        	rgoto        LCD_FX_FUNC_014
        	rgoto        LCD_FX_FUNC_015
        	rgoto        LCD_FX_FUNC_016
        	rgoto        LCD_FX_FUNC_017
        	rgoto        LCD_FX_FUNC_018
		rgoto        LCD_FX_FUNC_019
		rgoto        LCD_FX_FUNC_020
		rgoto        LCD_FX_FUNC_021
		rgoto        LCD_FX_FUNC_022
		rgoto        LCD_FX_FUNC_023
		rgoto        LCD_FX_FUNC_024
		rgoto        LCD_FX_FUNC_025
		rgoto        LCD_FX_FUNC_026
		rgoto        LCD_FX_FUNC_027
		rgoto        LCD_FX_FUNC_028
		rgoto        LCD_FX_FUNC_029
		rgoto        LCD_FX_FUNC_030
		rgoto        LCD_FX_FUNC_031
		rgoto        LCD_FX_FUNC_032
		rgoto        LCD_FX_FUNC_033
		rgoto        LCD_FX_FUNC_034
		rgoto        LCD_FX_FUNC_035
		rgoto        LCD_FX_FUNC_036
		rgoto        LCD_FX_FUNC_037
		rgoto        LCD_FX_FUNC_038
		rgoto        LCD_FX_FUNC_039
		rgoto        LCD_FX_FUNC_040
		rgoto        LCD_FX_FUNC_041	;; 0x29


LCD_FX_FUNC_000
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC0
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_001
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC1
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_002
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC2
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_003
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC3
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_004
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC4
	call    MIOS_LCD_PrintString
		return
	
LCD_FX_FUNC_005
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC5
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_006
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC6
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_007
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC7
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_008
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC8
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_009
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC9
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_010
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC10
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_011
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC11
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_012
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC12
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_013
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC13
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_014
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC14
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_015
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC15
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_016
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC16
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_017
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC17
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_018
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC18
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_019
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC19
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_020
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC20
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_021
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC21
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_022
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC22
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_023
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC23
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_024
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC24
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_025
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC25
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_026
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC26
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_027
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC27
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_028
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC28
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_029
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC29
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_030
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC30
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_031
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC31
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_032
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC32
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_033
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC33
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_034
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC34
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_035
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC35
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_036
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC36
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_037
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC37
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_038
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC38
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_039
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC39
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_040
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC40
	call    MIOS_LCD_PrintString
		return

LCD_FX_FUNC_041
	TABLE_ADDR TEXT_MB_AY_MENUE_FX_FUNC41
	call    MIOS_LCD_PrintString
		return


;; ------------------------------------------------------------------------
;; LOAD Fx CURREND DATA for POS (FX_POS_CURREND) for LCD
;; ------------------------------------------------------------------------
LOAD_FX_DATA_LCD_CURREND_POS
	;; used by [+] and [-] submenu pos routine for Fx Main Menu (LCD)
	movlw SELECT_Fx_RAM_BANK;; INDIRECT RAM ADDRESSING:
	movwf FSR0H		;; SET THE RAM BANK TO 2
	
	;; LOAD FX FUNCTION Reg. @ Currend Fx Pos	
	movlw RAM_OFFSET_FX_FUNCTION ;; FX FUNCTIONS regs OFFSET 
	addwf FX_POS_CURREND, W	;; + (0 to 31 [32]) => Fx Tab Pos
	movwf FSR0L		;; LOAD CURREND OFFSET IND. ADR. for Fx Fuction Reg. @ current Fx Pos.
	movff INDF0, FX_FUNCTION_CURREND ;; WRITE FX FUNCTION @ currend Pos

	;; LOAD FX Data Reg. @ Currend Fx Pos	
	movlw RAM_OFFSET_FX_DATA;; FX Data Regs OFFSET
	addwf FX_POS_CURREND, W	;; + (0 to 31 [32]) => Fx Tab Pos
	movwf FSR0L		;; LOAD CURREND OFFSET to Fx Fuction Reg. @ current Fx Pos.
	movff INDF0,FX_DATA_CURREND	;; WRITE FX Data @ currend Pos

	;; LOAD FX Delay TIME Reg. @ Currend Fx Pos	
	movlw RAM_OFFSET_FX_TIME_PRE_EXECUTION ;; FX TIME Regs OFFSET 
	addwf FX_POS_CURREND, W	;; + (0 to 31 [32]) => Fx Tab Pos
	movwf FSR0L		;; LOAD CURREND OFFSET To Ind. Reg Adress
	movff INDF0,FX_TIME_PRE_EXECUTION_CURREND	;; WRITE FX Data @ currend Pos
	
		return

