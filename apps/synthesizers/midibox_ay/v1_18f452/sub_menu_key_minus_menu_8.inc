;; sub_menu_key_minus_menu8.inc

MINUS_KEY_SUBROUTINE_AT_MAIN_MENU8
	
;; CHECK LOCATION VIA EDITING MODE "*"
	;; COPY THE VALUE OF EDITING MODE TO  MIOS_PARAMETER1
	movff 	AY_LCD_COURSER_POS_MAIN_MENU_FX, MIOS_PARAMETER1
	
	;; JUMP TO [-] Routine @ item X
	movf MIOS_PARAMETER1, W
	JUMPTABLE_4BYTES 5 ;; [MENU 0 to 8]
        	goto        LABEL_MAIN_MENUE_SWITCH		 ;; Error: just reaload currend Main Menu
        	goto        DEC_FX_TAB_POS			 ;; INC FX Tab Pos
        	goto        DEC_FX_DELAY_TIME			 ;; INC Fx Time (Delay befor Fx execution)
        	goto        DEC_FX_FUNCTION			 ;; INC Fx Func
        	goto        DEC_FX_DATA				 ;; INC Fx Data
	
;; -------------------[DEC Fx Pos (0-31)]---------------------		
DEC_FX_TAB_POS
	;; DEC Pos of Fx Tab and load Currend pos Values
		
	;; dec FX TAB POS -1 [Range 1 to 32]
	decf FX_POS_CURREND, F
	
	;; POS > FX_POS_MAX?
	movlw FX_POS_MAX
	cpfsgt  FX_POS_CURREND
			;; NO - OK GO ON
			;; @ sub_menu_key_plus_menu_8.inc
			goto FX_TAB_LOAD_DATA_CURREND_POS

		;; YES - FX POS ist out of range 
		;; set Fx Pos to MAX VALUE
		rgoto SET_FX_POS_TO_MAX

SET_FX_POS_TO_MAX	
		;; POS is out of range
		;; so set to FX_POS_MAX	
		movlw FX_POS_MAX
		movwf FX_POS_CURREND
			goto FX_TAB_LOAD_DATA_CURREND_POS
	

;; -------------------[DEC Fx Func Delay Time (0-MAX)]---------------------	
DEC_FX_DELAY_TIME
	;; INC Fx Delay Time @ currend Fx Pos
	decf FX_TIME_PRE_EXECUTION_CURREND, F
		
		;; Fx Delay Time > Fx_DELAY_TIME_MAX ?
		movlw Fx_DELAY_TIME_MAX 
		cpfsgt FX_TIME_PRE_EXECUTION_CURREND
			
			;; NO
			;; OK, GO ON
			rgoto DEC_FX_DELAY_TIME_WRITE
		
				;; WRITE 200 TO Fx Delay Time
				movlw Fx_DELAY_TIME_MAX ;; WRITE Fx_DELAY_TIME_MAX to Fx Func delay Time
				movwf FX_TIME_PRE_EXECUTION_CURREND
					
DEC_FX_DELAY_TIME_WRITE	
	movlw SELECT_Fx_RAM_BANK  	;; INDIRECT RAM ADDRESSING:
	movwf FSR0H		  	;; SET THE RAM BANK 
	;; ;; Write result to FX Delay TIME Reg. @ Currend Fx Pos	
	movlw RAM_OFFSET_FX_TIME_PRE_EXECUTION 	;; FX TIME Regs OFFSET
	addwf FX_POS_CURREND, W			;; + OFFSET OF => Fx Tab Pos
	movwf FSR0L				;; LOAD CURREND RAM ADR To Ind. Reg Adress
	movff FX_TIME_PRE_EXECUTION_CURREND, INDF0	;; WRITE TO RAM
	
		goto        LABEL_MAIN_MENUE_SWITCH
;; ---------------------------------------------------------------------		

;; -------------------[DEC Fx Func (0-MAX)]---------------------	
DEC_FX_FUNCTION
	;; DEC Fx Function @ currend Fx Pos
	decf FX_FUNCTION_CURREND, F

	movlw FX_FUNCTIONS_MAX
	;; Fx fuction > FX_FUNCTIONS_MAX?
	cpfsgt  FX_FUNCTION_CURREND
			;; NO, FUNCTION IS NOT > FX_FUNCTIONS_MAX
			;; OK GO ON
			rgoto DEC_FX_FUNCTION_WRITE
		
		;; YES Fx FUNCTION IS > FX_FUNCTIONS_MAX
		;; WRITE FX_FUNCTIONS_MAX TO Fx Function
		movlw FX_FUNCTIONS_MAX 
		movwf FX_FUNCTION_CURREND
		

DEC_FX_FUNCTION_WRITE
	movlw SELECT_Fx_RAM_BANK  ;; INDIRECT RAM ADDRESSING:
	movwf FSR0H		  ;; SET THE RAM BANK TO 2
	;; Write result to FX FUNCTION Reg. @ Currend Fx Pos	
	movlw RAM_OFFSET_FX_FUNCTION  ;; FX TIME Regs OFFSET 
	addwf FX_POS_CURREND, W	  ;; + (0 to 31 [32]) => Fx Tab Pos
	movwf FSR0L		  ;; LOAD CURREND OFFSET To Ind. Reg Adress
	movff FX_FUNCTION_CURREND, INDF0	;; WRITE TO RAM
		goto        LABEL_MAIN_MENUE_SWITCH
;; ---------------------------------------------------------------------


;; -------------------[DEC Fx Func Data (0-255)]---------------------	
DEC_FX_DATA
	;; DEC Fx Data @ currend Fx Pos
	decf FX_DATA_CURREND, F
	
	movlw SELECT_Fx_RAM_BANK  ;; INDIRECT RAM ADDRESSING:
	movwf FSR0H		  ;; SET THE RAM BANK TO 2
	;; ;; Write result to DATA Reg. @ Currend Fx Pos	
	movlw RAM_OFFSET_FX_DATA  ;; FX TIME Regs OFFSET 
	addwf FX_POS_CURREND, W	  ;; + (0 to 31 [32]) => Fx Tab Pos
	movwf FSR0L		  ;; LOAD CURREND OFFSET To Ind. Reg Adress
	movff FX_DATA_CURREND, INDF0	;; WRITE

		goto        LABEL_MAIN_MENUE_SWITCH
;; ---------------------------------------------------------------------	