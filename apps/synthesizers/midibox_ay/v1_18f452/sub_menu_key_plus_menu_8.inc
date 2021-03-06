;; sub_menu_key_plus_menu_8.inc

;; plus key @ Fx Menu 

PLUS_KEY_SUBROUTINE_AT_MAIN_MENU8
	;; CHECK LOCATION VIA EDITING MODE "*"
	;; COPY THE VALUE OF EDITING MODE TO  MIOS_PARAMETER1
	movff 	AY_LCD_COURSER_POS_MAIN_MENU_FX, MIOS_PARAMETER1
	
	;; JUMP TO [+] Routine @ item X
	movf MIOS_PARAMETER1, W
	JUMPTABLE_4BYTES 5 ;; [MENU 0 to 8]
        	goto        LABEL_MAIN_MENUE_SWITCH		 ;; Error: just reaload CURRENT Main Menu
        	goto        INC_FX_TAB_POS			 ;; INC FX Tab Pos
        	goto        INC_FX_DELAY_TIME			 ;; INC Fx Time (Delay befor Fx execution)
        	goto        INC_FX_FUNCTION			 ;; INC Fx Func
        	goto        INC_FX_DATA				 ;; INC Fx Data
	
	
	
;; -------------------[INC Fx Pos (0-31)]---------------------	
INC_FX_TAB_POS
	;; INC Pos of Fx Tab and load CURRENT pos Values
		
	;; inc FX TAB POS +1 [Range 1 to 32]
	incf FX_POS_CURRENT, F
		
		;; POS > 31 ?
		movlw 0x1F ;; = 31 DEC. [0-31 [32 positions]]
		cpfsgt  FX_POS_CURRENT
			;; NO - OK GO ON
			goto FX_TAB_LOAD_DATA_CURRENT_POS

			;; YES - reset Fx Tab Pos
			clrf FX_POS_CURRENT
		

FX_TAB_LOAD_DATA_CURRENT_POS
		;; LOAD FX DATA @ THE CURRENT POS
		call LOAD_FX_DATA_LCD_CURRENT_POS

			goto   LABEL_MAIN_MENUE_SWITCH



	
;; -------------------[INC Fx Func Delay Time (0-200)]---------------------		
INC_FX_DELAY_TIME
	;; INC Fx Delay Time @ CURRENT Fx Pos
	incf FX_TIME_PRE_EXECUTION_CURRENT, F
		
		;; Fx Delay Time > 200 ?
		movlw Fx_DELAY_TIME_MAX 
		cpfsgt FX_TIME_PRE_EXECUTION_CURRENT
			
			;; NO, go on
			rgoto DEC_FX_DELAY_TIME_WRITE
		
				;; YES, Reset Fx Delay Time to 0
				clrf FX_TIME_PRE_EXECUTION_CURRENT
	;; -------------------------------------------------
	movlw SELECT_Fx_RAM_BANK 	;; INDIRECT RAM ADDRESSING
	movwf FSR0H			;; SET THE RAM BANK TO 2
	;; ;; Write result to FX Delay TIME Reg. @ CURRENT Fx Pos	
	movlw RAM_OFFSET_FX_TIME_PRE_EXECUTION	;; FX TIME Regs OFFSET 
	addwf FX_POS_CURRENT, W			;; + Pos + Offset => Fx Tab Pos
	movwf FSR0L				;; LOAD CURRENT OFFSET To Ind. Reg Adress
	movff FX_TIME_PRE_EXECUTION_CURRENT, INDF0	;; WRITE
	;; -------------------------------------------------
			;; OK, WORK IS DONE
			goto        LABEL_MAIN_MENUE_SWITCH
	
	
;; -------------------[INC Fx Func (0-37)]---------------------	
INC_FX_FUNCTION
	;; INC Fx Function @ CURRENT Fx Pos
	incf FX_FUNCTION_CURRENT, F

	movlw  FX_FUNCTIONS_MAX_PULS_1  ;; = 42 DEC = (Functions [0-41] +1)
	;; Fx fuction < 42?
	cpfslt  FX_FUNCTION_CURRENT
			;; NO, FUNCTION IS NOT < 42 
			;; IT's NOT VALID
			;; [VALID RANGE = 0-41]
			;; SO RESET Fx FUNCTION to 0 
			clrf FX_FUNCTION_CURRENT ;; FUNC = 0x00 =  
	
		;; Yes < 42	
		;; ALLRIGHT WE DO NOT HAVE TO RESET FX FUNC. 
		;; --------------------------------------------------
		movlw SELECT_Fx_RAM_BANK  ;; INDIRECT RAM ADDRESSING
		movwf FSR0H		;; SET THE RAM BANK TO 2
			;; Write result to FX Function. @ CURRENT Fx Pos	
			movlw RAM_OFFSET_FX_FUNCTION ;; FX TIME Regs OFFSET 
			addwf FX_POS_CURRENT, W	;; + (0 to 31 [32]) => Fx Tab Pos
			movwf FSR0L		;; LOAD CURRENT OFFSET To Ind. Reg Adress
			movff FX_FUNCTION_CURRENT, INDF0	;; WRITE
		;; --------------------------------------------------
				;; OK, WORK IS DONE
				goto  LABEL_MAIN_MENUE_SWITCH



;; -------------------[INC Fx Func Data (0-255)]---------------------	
INC_FX_DATA
	;; INC Fx Data @ CURRENT Fx Pos
	incf FX_DATA_CURRENT, F
	;; -----------------------------------------------
	movlw SELECT_Fx_RAM_BANK;; INDIRECT RAM ADDRESSING:
	movwf FSR0H		;; SET THE RAM BANK TO 2
	;; ;; Write result to FX DATA Reg. @ CURRENT Fx Pos	
	movlw RAM_OFFSET_FX_DATA;; FX DATA Regs OFFSET 
	addwf FX_POS_CURRENT, W	;; + (0 to 31 [32]) => Fx Tab Pos
	movwf FSR0L		;; LOAD CURRENT OFFSET To Ind. Reg Adress
	movff FX_DATA_CURRENT, INDF0	;; WRITE
	;; -----------------------------------------------
				;; OK, WORK IS DONE
				goto        LABEL_MAIN_MENUE_SWITCH
	