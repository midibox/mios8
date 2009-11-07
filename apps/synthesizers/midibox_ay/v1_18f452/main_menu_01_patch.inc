;; main_menu_01_patch.inc
;; 
;; function: 
;; WRITE MAIN MENU1 = PATCH MENU
;; TO LCD (Patch Name, etc.)

;; ------[MB AY MAIN MENU 01: AY PATCH INFO ]------ 

LABEL_AY_MAIN_MENU_01_PATCH_INFO
	;; SHOW MIDIBOX AY PATCH INFO MENU
	
	;; CLEAR LCD
	call    MIOS_LCD_Clear
	
	;; -------------[LCD ROW 1 (PATCH NAME)]--------------------------------
   	TABLE_ADDR TEXT_AY_MB_AY_PATCH_NAME
	call    MIOS_LCD_PrintString
	
		;; LCD ROW1 :
		;; -----------[WRITE ASC PATCH NAME]----------
		;; ------------------------------------------- 
		;; 1) SET CURSOR POS TO PATCH NAME ASC POS1
			movlw 	LCD_ROW1 + 8
					
		;; 2) WRITE ASC CODE from PATCH_NAME_ASC_1 to LCD
			call    MIOS_LCD_CursorSet
			movff 	PATCH_NAME_ASC_0, MIOS_PARAMETER1
		 	movf 	MIOS_PARAMETER1, W
        		call    MIOS_LCD_PrintChar

		;; 3) INC CURSER POS
			movlw 	LCD_ROW1 + 9
		;; 4) WRITE ASC CODE from PATCH_NAME_ASC_2 to LCD
			call    MIOS_LCD_CursorSet
			movff 	PATCH_NAME_ASC_1, MIOS_PARAMETER1
		 	movf 	MIOS_PARAMETER1, W
        		call    MIOS_LCD_PrintChar

		;; 5) INC CURSER POS
			movlw 	LCD_ROW1 + 10
		;; 6) WRITE ASC CODE from PATCH_NAME_ASC_3 to LCD
			call    MIOS_LCD_CursorSet
			movff 	PATCH_NAME_ASC_2, MIOS_PARAMETER1
		 	movf 	MIOS_PARAMETER1, W
        		call    MIOS_LCD_PrintChar
		
		;; 7) INC CURSER POS
			movlw 	LCD_ROW1 + 11

		;; 8) WRITE ASC CODE from PATCH_NAME_ASC_4 to LCD
			call    MIOS_LCD_CursorSet
			movff 	PATCH_NAME_ASC_3, MIOS_PARAMETER1
		 	movf 	MIOS_PARAMETER1, W
        		call    MIOS_LCD_PrintChar

		;; 9) INC CURSER POS
			movlw 	LCD_ROW1 + 12

		;;10) WRITE ASC CODE from PATCH_NAME_ASC_5 to LCD
			call    MIOS_LCD_CursorSet
			movff 	PATCH_NAME_ASC_4, MIOS_PARAMETER1
		 	movf 	MIOS_PARAMETER1, W
        		call    MIOS_LCD_PrintChar

		;;11) INC CURSER POS
			movlw 	LCD_ROW1 + 13

		;;12) WRITE ASC CODE from PATCH_NAME_ASC_6 to LCD
			call    MIOS_LCD_CursorSet
			movff 	PATCH_NAME_ASC_5, MIOS_PARAMETER1
		 	movf 	MIOS_PARAMETER1, W
        		call    MIOS_LCD_PrintChar

		;;13) INC CURSER POS
			movlw 	LCD_ROW1 + 14

		;;14) WRITE ASC CODE from PATCH_NAME_ASC_7 to LCD
			call    MIOS_LCD_CursorSet
			movff 	PATCH_NAME_ASC_6, MIOS_PARAMETER1
		 	movf 	MIOS_PARAMETER1, W
        		call    MIOS_LCD_PrintChar

		;;15) INC CURSER POS
			movlw 	LCD_ROW1 + 15

		;;16) WRITE ASC CODE from PATCH_NAME_ASC_8 to LCD
			call    MIOS_LCD_CursorSet
			movff 	PATCH_NAME_ASC_7, MIOS_PARAMETER1
		 	movf 	MIOS_PARAMETER1, W
        		call    MIOS_LCD_PrintChar
		;; -------------------------------------------
		;; ------[END: WRITE ASC PATCH NAME]----------
		;; -----[END: LCD ROW 1 (PATCH NAME)]---------

;; ------------------------[START ITEM POS HANDLING]----------------------------------------------
;; ------------------------------[PATCH NAME]-----------------------------------------------------
;; Show the POSITION of "*" [LCD: MAIN MENU EDIT POINTER] 
LABEL_AY_MAIN_MENU_01_FIND_EDIT_POS1
	;; POS1 = PATCH NAME
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO = 1?
	movff  	AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO, MIOS_PARAMETER1
	movlw  	MENU_ITEM_POS1
	cpfseq 	MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
			;; NO
			;; POS < > 1
			rgoto LABEL_AY_MAIN_MENU_01_NOT_POS1

		;; YES
		;; POS = 1
		;; PRINT "*" ON EDIT POSITION1 = PATCH NAME
		;; PATCH NAME IS IN EDITING MODE!
		movlw 	LCD_ROW1 + 4
		call    MIOS_LCD_CursorSet
		movlw   '*'
       	 	call    MIOS_LCD_PrintChar
			rgoto LABEL_AY_MAIN_MENU_01_POS2

LABEL_AY_MAIN_MENU_01_NOT_POS1
		;; PRINT ":" ON EDIT POSITION1
		;; PATCH NAME IS *NOT* IN EDITING MODE
		movlw 	LCD_ROW1 + 4
		call    MIOS_LCD_CursorSet
		movlw   ':'
		call    MIOS_LCD_PrintChar


;; -----------------[START ROW2]-----------------------------------------------------
;; ----------------[ITEM: PATCH NUMBER]----------------------------------------------
LABEL_AY_MAIN_MENU_01_POS2
	;; LCD ROW 2 
	;; WRITE TXT PATCH NR. on LCD ROW2
	TABLE_ADDR TEXT_AY_MB_AY_PATCH_NR
	call    MIOS_LCD_PrintString
		
	
	;; ----[CHECK IF PATCH NR = 0 = INTERNAL (PIC EEPROM PATCH)]-----------------
	;; IF PATCH NR: = 0: WRITE "INTERNAL" on LCD for INTernal PATCH (PIC EEPROM)
	clrw  ;; WREG = 0
	cpfseq BROWSE_PATCH_NR ;; COMPARE F WITH WREG
			
		;; NO
		;; BROWSE_PATCH_NR < > 0
		;; SO PATCH = BANKSTICK PATCH (1-128)
		rgoto LABEL_AY_MAIN_MENU_01_POS2_BANKSTICK
	
			;; YES
			;; BROWSE_PATCH_NR = 0
			;; PATCH = INTERNAL (PIC EEPROM PATCH)
			;; PRINT "INTERNAL" on LCD
			TABLE_ADDR LCD_CLEAR_ROW2
			call    MIOS_LCD_PrintString
			TABLE_ADDR TEXT_AY_MB_AY_PATCH_NR 
			call    MIOS_LCD_PrintString
			TABLE_ADDR TEXT_AY_MB_AY_PATCH_INT_PATCH
			call    MIOS_LCD_PrintString
				;; GO AND CHECK IF POS2 IS IN EDITING "*" MODE
				rgoto 	LABEL_AY_MAIN_MENU_01_POS2_EDITING_MODE
	;; ----[END: CHECK IF PATCH NR = 0 = INTERNAL (PIC EEPROM PATCH)]------------

	;; ----[PRINT B.STICK PATCH ROW2 on LCD]-------------------------------------
LABEL_AY_MAIN_MENU_01_POS2_BANKSTICK
		;; PATCH = BANKSTICK PATCH (1-128)
		;; PRINT "B.STCK1" on LCD ROW 2
		TABLE_ADDR TEXT_AY_MB_AY_PATCH_BS_PATCH
		call    MIOS_LCD_PrintString
		
			;; PRINT PATCH NUMBER on LCD
			movlw   LCD_ROW2 + 0x04
			call    MIOS_LCD_CursorSet
			movf    BROWSE_PATCH_NR, W
			call    MIOS_LCD_PrintBCD3
        	
			movlw 	LCD_ROW2 + 3
			call    MIOS_LCD_CursorSet
			movlw   '['
			call    MIOS_LCD_PrintChar
        	
			movlw 	LCD_ROW2 + 7
			call    MIOS_LCD_CursorSet
			movlw   ']'
			call    MIOS_LCD_PrintChar



	;; ----[END: PRINT B.STICK PATCH ROW2 on LCD]--------------------------------


	;; ----[START: CHECKING EDITING MODE @ PATCH NUMBER (ROW2 on LCD)]------------
LABEL_AY_MAIN_MENU_01_POS2_EDITING_MODE
	;; EDITING MODE @ PATCH NUMBER?
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO = 2?
	movff AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO, MIOS_PARAMETER1
	movlw MENU_ITEM_POS2
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			;; NO
			;; POS < > 2
			rgoto LABEL_AY_MAIN_MENU_01_NOT_POS2
	;; ----[EDITING MODE @ PATCH NUMBER (ROW2 on LCD) IS GIVEN]----------------
		;; YES
		;; POS = 2
		;; YES EDITING MODE @ PATCH NUMBER
		;; PRINT "*" ON POS2 (ROW2 - ITEM: PATCH NUMBER)
		movlw 	LCD_ROW2 + 2
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
			
			rgoto LABEL_AY_MAIN_MENU_01_POS3
	
	;; ----[EDITING MODE @ PATCH NUMBER (ROW2 on LCD) IS NOT GIVEN]----------------
LABEL_AY_MAIN_MENU_01_NOT_POS2
			;; NO EDITING MODE @ PATCH NAME
			;; PRINT ":" ON EDIT POSITION2
			movlw 	LCD_ROW2 + 2
			call    MIOS_LCD_CursorSet
			movlw   ':'
        		call    MIOS_LCD_PrintChar

	;; ----[END: CHECKING EDITING MODE @ PATCH NUMBER (ROW2 on LCD)]------------
	;; -------------------------------------------------------------------------
	
	
;; -------------------------------[START ROW3]--------------------------------------
LABEL_AY_MAIN_MENU_01_POS3
	;; PRINT TXT on LCD ROW 3
	TABLE_ADDR TEXT_AY_MB_AY_PATCH_DO
	call    MIOS_LCD_PrintString

	;; EDITING MODE @ EXECUTION ? (POS3)
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO = 3?
	movff AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO, MIOS_PARAMETER1
	movlw MENU_ITEM_POS3
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
			;; NO
			;; POS < > 3
			rgoto LABEL_AY_MAIN_MENU_01_NOT_POS3

		;; YES
		;; POS = 3
		;; EDITING MODE @ DO [L W F]
		;; PRINT "*" ON EDIT POS3
		movlw 	LCD_ROW3 + 2
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
			;; JUMP TO LCD ROW4
			rgoto LABEL_AY_MAIN_MENU_01_ROW4


LABEL_AY_MAIN_MENU_01_NOT_POS3
			;; NO EDITING MODE @ SONGNAME
			;; PRINT ":" ON EDIT POSITION3
			movlw 	LCD_ROW3 + 2
			call    MIOS_LCD_CursorSet
			movlw   ':'
        		call    MIOS_LCD_PrintChar

;; -------------------------------[END: LCD ROW3]--------------------------------------

;; -------------------------------[START: LCD ROW4]------------------------------------
LABEL_AY_MAIN_MENU_01_ROW4
	;; LCD ROW 4 (PATCH MENU STATUS)
	;; (ROW4 = NO MENU ITEM)
	
	;; -----------------[CHECK IF PATCH = INTERNAL]--------------------------------
	;; WRITE "----" [NO FORMAT] for INTernal PATCH (PIC EEPROM)
	;; ON LCD IF BROWSE_PATCH_NR = 0
	clrw 
	cpfseq BROWSE_PATCH_NR ;; COMPARE F WITH WREG
			
		;; NO
		;; BROWSE_PATCH_NR < > 0
		;; SO PATCH = BANKSTICK PATCH
		rgoto LABEL_AY_MAIN_MENU_01_POS4_FORMAT
	
			;; YES
			;; BROWSE_PATCH_NR = 0
			;; ;; PRINT "Load Write ------" on LCD	(NO FORMAT on ROW4)
			TABLE_ADDR TEXT_AY_MB_AY_PATCH_EXE_NO_FORMAT
			call    MIOS_LCD_PrintString
			;; DO WRITE A "-" over [F] => [-]
			;; BECAUSE ITS PATCH 0 = INT PATCH
			;; NO FORMAT POSSIBLE
			movlw 	LCD_ROW3 + 14
			call    MIOS_LCD_CursorSet
			movlw   '-'
			call    MIOS_LCD_PrintChar
        		
			;; WRITE PATCH FLAGS
			;; SET CURREND PATCH = INTERNAL
			;; CLEAR BIT 2 of PATCH_MENU_STATE_FLAGS
			bcf PATCH_MENU_STATE_FLAGS, BIT2_SELECTED_PATCH_BANKSTICK_OR_INTERNAL
        		
				;; INTERNAL PATCH, no need for checking BANKSTICK status
				;; EXIT HERE
				return

;; ----------------------[ROW4 PATCH (1-128) = BANKSTICK]--------------------------- 
LABEL_AY_MAIN_MENU_01_POS4_FORMAT
	;; WRITE PATCH FLAGS
	;; SET CURREND PATCH = BANKSTICK
	;; SET BIT 2 of PATCH_MENU_STATE_FLAGS
	bsf PATCH_MENU_STATE_FLAGS, BIT2_SELECTED_PATCH_BANKSTICK_OR_INTERNAL
	
	;; LCD ROW4 = PATCH MENU: status line (NO MENU ITEM)
	;; PATCH Nr (1-128) = BANKSTICK AREA
	;; PRINT "Load Write Format" on LCD ROW4	
	TABLE_ADDR TEXT_AY_MB_AY_PATCH_EXE 
	call    MIOS_LCD_PrintString

	;; -----------------------[CHECK: BANKSTICK AVAILABLE?]-----------------------------------
LABEL_AY_MAIN_MENU_01_CHK_BANKSTICK_AVAILABLE
	;; 1) CHECK IF BANKSTICK IS AVAILABLE
	;; ------------------------------------------------------
	;; 1a) PREPARE THE: "IS THE BANKSTICK FORMATTED?" HANDLING
	;; ------------------------------------------------------
	;; DO: READ BANKSTICK ADR0 (BANKSTICK PATCH1 FINGERPRINT)
	;; ------------------------------------------------------
	clrf MIOS_PARAMETER1
	clrf MIOS_PARAMETER2
	call MIOS_BANKSTICK_Read
		
		;; ------------------------------------------
		;; Save BANKSTICK1 PATCH1 Fingerprint (Byte)
		;; FOR "IS THE BANKSTICK FORMATTED?" HANDLING
		;; ------------------------------------------
		movwf	CURREND_PATCH_DATA_BYTE_TEMP
		
	;; NOW (AFTER READING BS ADR 0) WE CAN CHECK IF BANKSTICK1 IS AVAILABLE
	;; BRANCH IF BANKSTICK1 IS NOT PRESENT
	;; ----------------------------------------------------------------------------------------------------
	;; --------------------------------------------------------[JUMP: IF BANKSTICK IST *NOT* AVAILABLE]----
	;; ----------------------------------------------------------------------------------------------------
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, LABEL_AY_MAIN_MENU_01_BANKSTICK_NOT_AVAILABLE
		
		;; --------------------------
		;; BANKSTICK IS AVAILABLE :-)
		;; --------------------------
		;; GO ON WITH BANKSTICK HANDLING
		rgoto LABEL_AY_MAIN_MENU_01_BANKSTICK_AVAILABLE

	
	;; -----------------------[END: CHECK BANKSTICK AVAILABLE]--------------------------------
	;; ---------------------------------------------------------------------------------------
	
	;; -----------------------[HANDLING: BANKSTICK NOT AVAILABLE]--------------------------------
	;; -----------------------[HANDLING: BANKSTICK NOT AVAILABLE - POS1]--------------------------------
LABEL_AY_MAIN_MENU_01_BANKSTICK_NOT_AVAILABLE
				;; PRINT "[-NO BANSTICK1-]" on LCD ROW1 (!)
				TABLE_ADDR TEXT_AY_MB_AY_PATCH_NO_BS
				call    MIOS_LCD_PrintString
				;; REDRAW ROW2 on LCD
				TABLE_ADDR LCD_CLEAR_ROW2
				call    MIOS_LCD_PrintString
				TABLE_ADDR TEXT_AY_MB_AY_PATCH_NR
				call    MIOS_LCD_PrintString
				
	;; EDITING MODE @ PATCH NAME?
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO = 1?
	;; FOR "[-NO BANSTICK1-]" on LCD ROW1 (!)
	;; POS1 IS NOT A VALID POS for EDITING MODE WHEN A BANKSTICK1 IS *NOT* AVAILABLE
	movff AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO, MIOS_PARAMETER1
	movlw MENU_ITEM_POS1
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			;; NO
			;; POS < > 1
			;; OK, GO ON AND CHECK POS2
			rgoto LABEL_AY_MAIN_MENU_01_BANKSTICK_NOT_AVAILABLE_CHK_POS2

		;; YES
		;; POS = 1
		;; EDITING MODE @ PATCH NAME
		;; ITS NOT A VALID POS
		;; SO SET POS TO POS2
		movlw  MENU_ITEM_POS2
		movwf  MIOS_PARAMETER1
		movff  MIOS_PARAMETER1, AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO
		
		;; PRINT EDITING MODE "*" ON POS2
		movlw 	LCD_ROW2 + 2
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
			;; GO AND REWRITE PATCH NUMBER @ POS2 on LCD
			rgoto  LABEL_AY_MAIN_MENU_01_BANKSTICK_NOT_AVAILABLE_PRINT_PATCH_NUMBER
		
		;; ------------------[HANDLING: BANKSTICK NOT AVAILABLE - POS2]-------------------------
		;; ------------------[CHECK: POS2 (PATCH NUMBER) = in EDITING MODE?]--------------------
LABEL_AY_MAIN_MENU_01_BANKSTICK_NOT_AVAILABLE_CHK_POS2
	;; EDITING MODE @ PATCH NUMBER? (POS2)
	;; IS AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO = 2?
	;; IF POS = ITEM2
	;; WE MUST PRINT ACTIVE POS = "*" @ ITEM2 (PATCH NUMBER)
	movff AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO, MIOS_PARAMETER1
	movlw MENU_ITEM_POS2
	cpfseq MIOS_PARAMETER1 ;; COMPARE F WITH WREG
			
			;; NO
			;; POS < > 2
			;; PATCH NUMBER IS NOT in EDITING MODE
			rgoto LABEL_AY_MAIN_MENU_01_BANKSTICK_NOT_AVAILABLE_PRINT_PATCH_NUMBER

		;; YES
		;; POS = 2
		;; ITEM: PATCH NUMBER is in EDITING MODE
		;; PRINT "*" ON POS2
		movlw 	LCD_ROW2 + 2
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar

		
		;; ------------------[STATE: EDITING MODE @ POS2 (PATCH NUMBER)]-------------
		;; ------------------[STATE: NO BANKSTICK AVAILABLE]-------------------------
		;; ------------------[DO   : DELITE [LOAD WRITE FORMAT] @ ROW3 and ROW4]-----
LABEL_AY_MAIN_MENU_01_BANKSTICK_NOT_AVAILABLE_PRINT_PATCH_NUMBER
				;; PRINT PATCH NUMBER on LCD
				movlw   LCD_ROW2 + 0x04
				call    MIOS_LCD_CursorSet
				movf    BROWSE_PATCH_NR, W
				call    MIOS_LCD_PrintBCD3
        	
				movlw 	LCD_ROW2 + 3
				call    MIOS_LCD_CursorSet
				movlw   '['
				call    MIOS_LCD_PrintChar
        	
				movlw 	LCD_ROW2 + 7
				call    MIOS_LCD_CursorSet
				movlw   ']'
				call    MIOS_LCD_PrintChar

				
				;; WRITE PATCH MENU STATUS FLAG: 
				;; NO BANKSTICK FOUND (BIT0 = CLEAR = NO BANKSTICK FOUND)
				bcf PATCH_MENU_STATE_FLAGS, BIT0_BANKSTICK_FOUND
				
				;; THERE IS NOTHING THE USER CAN DO, IF THERE IS NO BANKSTICK
				;; DEL [LOAD WRT FORMAT] on LCD ROW3 and ROW4
				TABLE_ADDR TEXT_AY_MB_AY_PATCH_EXE_NO_LT_ERT_FMT
				call    MIOS_LCD_PrintString

				;; DO WRITE A "-" over [L] => [-]
				;; BECAUSE ITS PATCH 0 = INT PATCH
				;; NO FORMAT POSSIBLE
				movlw 	LCD_ROW3 + 6
				call    MIOS_LCD_CursorSet
				movlw   '-'
				call    MIOS_LCD_PrintChar

				;; DO WRITE A "-" over [W] => [-]
				;; BECAUSE ITS PATCH 0 = INT PATCH
				;; NO FORMAT POSSIBLE
				movlw 	LCD_ROW3 + 10
				call    MIOS_LCD_CursorSet
				movlw   '-'
				call    MIOS_LCD_PrintChar
				
				;; DO WRITE A "-" over [F] => [-]
				;; BECAUSE ITS PATCH 0 = INT PATCH
				;; NO FORMAT POSSIBLE
				movlw 	LCD_ROW3 + 14
				call    MIOS_LCD_CursorSet
				movlw   '-'
				call    MIOS_LCD_PrintChar
					;; EXIT HERE
					return
		;; ------------[END - HANDLING: NO BANKSTICK]--------------------------------

;; --------------------------------[STATE: BANKSTICK IS AVAILABLE]---------------------------
;; --------------------------------[DO   : CHECK: FINGERPRINT (BANKSTICK FORMATED?)]---------
LABEL_AY_MAIN_MENU_01_BANKSTICK_AVAILABLE				
			;; WRITE PATCH STATUS FLAG: 
			;; BANKSTICK FOUND
			;; SET BIT0
			bsf PATCH_MENU_STATE_FLAGS, BIT0_BANKSTICK_FOUND
			
			;; CHECK IF THERE IS THE BANKSTICK FINGERPRINT
			;; (THE QUESTION IS: DO WE HAVE TO FORMAT THE BANKSTICK?)
			;; BANKSTICK_PATCH_FINGERPRINT = CURREND_PATCH_DATA_BYTE_TEMP 
			;; (REMEMBER WE HAVE STORED THE "BANKSTICK ADR0" BYTE in CURREND_PATCH_DATA_BYTE_TEMP)
			movlw 	BANKSTICK_PATCH_FINGERPRINT
			cpfseq	CURREND_PATCH_DATA_BYTE_TEMP
					;; -------------------------------------------------
					;; -----[STATE: BANKSTICK IS NOT FORMATTED]---------
					;; -------------------------------------------------
					;; NO
					;; BANKSTICK_EEPROM_FINGERPRINT < > CURREND_PATCH_DATA_BYTE_TEMP 
					;; SO THE BANKSTICK IS NOT FORMETTED!
					rgoto	LABEL_AY_MAIN_MENU_01_BANKSTICK_AVAILABLE_NO_FINGERPRINT
			
				;; ----------------------------------------------------
				;; -----[STATE: BANKSTICK IS FORMATTED]----------------
				;; ----------------------------------------------------
				;; YES
				;; BANKSTICK_EEPROM_FINGERPRINT = CURREND_PATCH_DATA_BYTE_TEMP 
				;; (SAVED BYTE: BANKSTICK1 ADR0)
				;; OK, WRITE PATCH MENU STATUS FLAG:
				;; BANKSTICK = FORMATED :-)
				;; SET BIT1
				;; ----[DO: WRITE PATCH STAUS FLAG BIT1]---------------
				bsf PATCH_MENU_STATE_FLAGS, BIT1_BANKSTICK_SELECTED_PATCH_FORMATED
						;; ALL DONE, EXIT HERE
						return

		;; --------------------[END: CHECKING BANKSTICK FINGERPRINT]------------			

;; ----------------------------------------------------------------------------------------------------
;; --------------------------------[STATE: BANKSTICK IS AVAILABLE BUT NOT FORMATTED]-------------------		
LABEL_AY_MAIN_MENU_01_BANKSTICK_AVAILABLE_NO_FINGERPRINT
					;; ---[DO: WRITE PATCH MENU STATUS FLAG]-----------------------
					;; BANKSTICK1 = NOT FORMATED
					bcf PATCH_MENU_STATE_FLAGS, BIT1_BANKSTICK_SELECTED_PATCH_FORMATED
					
					;; IF BANKSTICK IS NOT FORMATTED ROW1
					;; IS NOT A VALID EDITING MODE POS (!)
					;; BECAUSE THERE IS A "DO FORMAT BANKSTICK" MASSAGE on LCD @ ROW1
					;; CHECK: CURREND POS = 1?
					movff 	AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO, MIOS_PARAMETER1
					movlw 	MENU_ITEM_POS1
					cpfsgt 	MIOS_PARAMETER1 ;; EDITING MODE ITEM  = 1?

							;; NO
							;; POS = 1 (NOT > 1)
							;; POS NOT OK :-(
							rgoto LABEL_AY_MAIN_MENU_01_POS_1_NOT_VALID_GO_POS_2
				
						
						;; YES
						;; POS > 1
						;; POS OK :-)
						rgoto LABEL_AY_MAIN_MENU_01_BANKSTICK_AVAILABLE_BUT_NOT_FORMATED
						

		;; ---------------[STATE: BANKSTICK IS AVAILABLE BUT NOT FORMATTED]------
		;; ---------------[STATE: POS = POS1]------------------------------------
		;; ---------------[STATE: POS1 IS NOT VALID]-----------------------------
		;; ---------------[DO   : SET POS to POS2 (SET EDITING MODE = ITEM2)]----
LABEL_AY_MAIN_MENU_01_POS_1_NOT_VALID_GO_POS_2		
		;; EDITING MODE @ POS1 IS NOT VALID
		;; SET EDITING MODE POS = 2	
		movff 	MENU_ITEM_POS2, MIOS_PARAMETER1
		movff 	MIOS_PARAMETER1, AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO
		
		;; PRINT "*" ON EDIT POSITION2
		movlw 	LCD_ROW2 + 2
		call    MIOS_LCD_CursorSet
		movlw   '*'
		call    MIOS_LCD_PrintChar
		;; ---------------[END: SET POS to POS2 (SET EDITING MODE = ITEM2)]----
		
		;; ---------------[DO: UPDATE LCD - JUST FORMATTING POSSIBLE]------------
		;; ---------------[STATE: BANKSTICK IS AVAILABLE BUT NOT FORMATTED]------
		;; ---------------[STATE: NO PATCH WRITING POSSIBLE]------
		;; ---------------[STATE: NO PATCH LOADING POSSIBLE]------
		;; ---------------[STATE: BANKSTICK FORMATTING POSSIBLE]------
LABEL_AY_MAIN_MENU_01_BANKSTICK_AVAILABLE_BUT_NOT_FORMATED
				;; PRINT "DO FORMAT BANKSTICK1" on LCD ROW1
				;; NOW ROW1 is NOT a ITEM anymore
				TABLE_ADDR TEXT_AY_MB_AY_BANKSTICK_NOT_FORMATED
				call    MIOS_LCD_PrintString
				;; -[STATE: NO PATCH LOADING POSSIBLE]------
				;; -[STATE: NO PATCH WRITING POSSIBLE]------
				;; ROW4 (STAUS):
				;; PRINT "---- ---- Format" on LCD ROW4
				TABLE_ADDR TEXT_AY_MB_AY_PATCH_EXE_JUST_FORMAT
				call    MIOS_LCD_PrintString
				
				;; -[STATE: NO PATCH LOADING POSSIBLE]------
				;; DO WRITE A "-" over [L] => [-]
				;; NO PATCH LOADING POSSIBLE
				movlw 	LCD_ROW3 + 6
				call    MIOS_LCD_CursorSet
				movlw   '-'
				call    MIOS_LCD_PrintChar
				
				;; -[STATE: NO PATCH WRITING POSSIBLE]------
				;; DO WRITE A "-" over [W] => [-]
				;; BECAUSE ITS PATCH 0 = INT PATCH
				;; NO PATCH WRITING POSSIBLE
				movlw 	LCD_ROW3 + 10
				call    MIOS_LCD_CursorSet
				movlw   '-'
				call    MIOS_LCD_PrintChar
					;; EXIT HERE					
					return


