;; format_bankstick.inc

FORMAT_BANKSTICK
	;; CHECK:
	;; BANKSTICK AVAILABLE?
	;; -------------[STATE: SELECTED PATCH = BANKSTICK PATCH]------------------------
	;; -------------[CHECK: BANKSTICK AVAILABLE?]------------------------------------
		
		;; CHECK: -----[BANKSTICK IS AVAILABLE?]----------------
		;; BIT0 of = PATCH_MENU_STATE_FLAGS?
		btfss	PATCH_MENU_STATE_FLAGS, BIT0_BANKSTICK_FOUND
					;; ---[BANKSTICK NOT AVAILABLE]----------------
					;; BIT0 = 0 = NO BANKSTICK FOUND
					;; EXIT HERE
					goto LABEL_MAIN_MENUE_SWITCH
			;; ---[BANKSTICK IS AVAILABLE]----------------
			;; BIT0 = 1 = BANKSTICK IS AVAILABLE
			
		;; CHECK: MENU = MAIN MENU 1
		movlw	MAIN_MENU_01
		cpfseq 	CURRENT_MAIN_MENU
				;; NO IT'S NOT MAIN MENU1
				;; Exit Here
				return
		
			;; YES MAIN MENU1
			;; GO ON...

		;; --[CHECK ACTIVE ITEM]-------
		;; CHECK VIA EDITING MODE "*"
		movff 	AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO, MIOS_PARAMETER1
		movlw 	MENU_ITEM_POS3
		cpfseq 	MIOS_PARAMETER1 ;; EDITING MODE ITEM  = 3? | [W]rite [F]ormat ...
				;; NO, NOT ITEM 3
				;; EXIT HERE
				return

			;; YES, ITEM 3 IS ACTIVE (EDITING MODE)
			;; GO ON
			
			;; SO DO FORMAT CURRENT BANKSTICK
			;; SET THE "DO FORMAT THE BANKSTICK BIT"
			bsf	PATCH_MENU_STATE_FLAGS, BIT3_FORMAT_BANKSTICK_MODE_ON_OR_OFF
			
			;; RESET (CLEAR) PATCHNAME
			call BLANK_PATCH_NAME
						
			;; GO AND DO FORMAT BANKSTICK
			goto PATCH_WRITE_DO_FORMAT_BANKSTICK	
	

;;-------------------------------------------------------------------------------------------------


;; -------------------------------------------------------------------------------
;; ----------------[SUB ROUTINE: FORMAT BANKSTICK PATCHES PATCH BY PATCH]---------
;; -------------------------------------------------------------------------------
;; ---------------------------------------------------------------------------------------------------
;; -----------------------------[FORMAT BANKSTICK JUMP IN POINT]--------------------------------------
PATCH_WRITE_DO_FORMAT_BANKSTICK
	;; FORMAT BANKSTICK (STEP 1)
	;; ----SELECT (A NEW) PATCH NUMBER-----
	;; -------------------------------------
	;; INI VALUE FORMAT_BANKSTICK_CYCLE_COUNTER = 0
	;; DO INC FORMAT_BANKSTICK_CYCLE_COUNTER    + 1
	;; [FORMAT_BANKSTICK_CYCLE_COUNTER: DO COUNT FROM 1 to 128]
	;;    IF FORMAT_BANKSTICK_CYCLE_COUNTER = 128 ( = BANKSTICK_MAX_PATCH_NUMBER +1) 
	;;    IT WILL BE THE "EXIT THE FORMAT BANSTICK ROUTINE" VALUE  
	incf 	FORMAT_BANKSTICK_CYCLE_COUNTER, F
	;; -------------------------------------
	;; FORMAT BANKSTICK (STEP 1a)
	;; -------------------------------------
	;; BROWSE_PATCH_NR = FORMAT_BANKSTICK_CYCLE_COUNTER
	movff FORMAT_BANKSTICK_CYCLE_COUNTER, BROWSE_PATCH_NR
	;; -------------------------------------
	;; FORMAT BANKSTICK (STEP 2)
	;; -------------------------------------
	;; CLEAR LCD ROW4
	TABLE_ADDR LCD_CLEAR_ROW4
	call    MIOS_LCD_PrintString
	;; WRITE "F:" on LCD ROW4
	TABLE_ADDR TEXT_AY_MB_AY_PATCH_FORMATTING_BS
	call    MIOS_LCD_PrintString
	;; Write CURRENT FORMATTING PATCH NR on LCD
	movlw 	LCD_ROW4 + 12
	call    MIOS_LCD_CursorSet
	movf	FORMAT_BANKSTICK_CYCLE_COUNTER, W
	call 	MIOS_LCD_PrintBCD3
	;; -------------------------------------
	;; FORMAT BANKSTICK (STEP 3)
	;; --------------------------------------
	;; CHECK: FORMAT_BANKSTICK_CYCLE_COUNTER > BANKSTICK_MAX_PATCH_NUMBER ?
	;; IF NOT: DO ANOTHER BANKSTICK PATCH WRITE
	movlw	BANKSTICK_MAX_PATCH_NUMBER
	cpfsgt 	FORMAT_BANKSTICK_CYCLE_COUNTER
			;; FORMAT_BANKSTICK_CYCLE_COUNTER < or = BANKSTICK_MAX_PATCH_NUMBER
			;; ----------------------------------------------------------------
			;; STATE: GO ON FORMAT BANKSTICK 
			;; DO: WRITE BANKTICK @ BROWSE_PATCH_NR [WRITE BANKSTICK PATCH1 to 127]
			;; ----------------------------------------------------------------
			;; (STEP 4 WRITE PATCH)
			;; --------------------------------------
			rgoto PATCH_WRITE_DO
		
		;; FORMAT_BANKSTICK_CYCLE_COUNTER > BANKSTICK_MAX_PATCH_NUMBER
		;; -----------------------------------------------------------
		;; (STEP 5=FINALIZE FORMATTING)
		;; --------------------------------------
		;; STATE:
		;; ALL BANKSTICK PATCHES ARE WRITTEN (FORMATTED)					
		;; DO: 
		;; FINALIZE THE BANKSTICK FORMATTING PROCESS
		;; -----------------------------------------------------------
		;; (STEP 5a)
		;; RESET: FORMAT_BANKSTICK_CYCLE_COUNTER to 0
		clrf	FORMAT_BANKSTICK_CYCLE_COUNTER
		;; -----------------------------------------------------------
		;; (STEP 5b)
		;; -----------------------------------------------------------
		;; (RE)SET CURRENT PATCH TO 1 (BANKSTICK PATCH1)
		;; AFTER FORMATTING BANKSTICK PATCH 1-127
		movlw 0x01
		movwf CURRENT_PATCH_NR
		movff CURRENT_PATCH_NR, BROWSE_PATCH_NR
		;; -----------------------------------------------------------
		;; (STEP 5c)
		;; -----------------------------------------------------------
		;; SWITCH OF THE "DO FORMAT BANKSTICK" MODE 
		;; RESET: BIT3_SELECTED_PATCH_FORMAT_BANKSTICK @ PATCH_MENU_STATE_FLAGS
		;; DO CLEAR BIT3 PATCH_MENU_STATE_FLAGS
		bcf	PATCH_MENU_STATE_FLAGS, BIT3_FORMAT_BANKSTICK_MODE_ON_OR_OFF
		;; -----------------------------------------------------------
		;; (STEP 5d)
		;; -----------------------------------------------------------
		;; EXIT THE BANKSTICK FORMATTING CYCLE HERE
		;; AND WRITE MAIN MENU1 on LCD
		goto LABEL_AY_MAIN_MENU_01_PATCH_INFO
			

;; -------------------------[END: FORMATTING BANKSTICK SUB ROUTINE]---------------------------------------
;; ---------------------------------------------------------------------------------------------------
