;; patch_name_writing.inc

;; --------------------------------------
;; function: ptach name editing functions
;; --------------------------------------

;; 1) check if active MAIN MENU = 1  (PATCH MENU)
;; 2) check if active ITEM = 1 (PATCH NAME)
;; 3) check if the KEY State is pressed or depressed (pressed is needed)
;; 4) KEY ASCII CODE: check if the ASC CODE RANGE [0-9] is VALID 
;; 5) KEY ASCII CODE: check if the ASC CODE RANGE [A-Z] is VALID 
;; 6) KEY ASCII CODE: check if the SPACE KEY had been pressed 
;; 7) KEY ASCII CODE: check if the CURSOR RIGHT KEY had been pressed 
;; 8) KEY ASCII CODE: check if the CURSOR LEFT KEY had been pressed 

;; ---[Start of MENU, ITEM an KEY Checking (PATCH NAME WRITING)]---------
CHCK_NEED_FOR_PATCHNAME_EDITING
	;; 1) check if active MAIN MENU = 1  (PATCH MENU)
	; First check if there is the patch menu active
	; MAIN MENU = 1 ? 
	movlw  MAIN_MENU_01
	cpfseq CURRENT_MAIN_MENU
	
		; NO - GO ON WITH SUB MENU FUNCTION
		; @ sub_menu_functions.inc
		goto BRANCH_TO_SUB_MENUE_KEY
	
		; YES
		; GO ON AND CHECK IF the active item @ menu1
		rgoto CHCK_MAIN_MENU1_PATCHNAME_ITEM1

CHCK_MAIN_MENU1_PATCHNAME_ITEM1
	;; 2) check if active ITEM = 1 (ITEM: PATCH NAME)
	; ITEM 1 ?
	movff AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO, MIOS_PARAMETER1
	movlw MENU_ITEM_POS1
	cpfseq MIOS_PARAMETER1
		; NO - GO ON WITH SUB MENU FUNCTION
		; @ sub_menu_functions.inc
		goto BRANCH_TO_SUB_MENUE_KEY
	
		; YES, active ITEM = PATCH NAME
		; GO ON AND CHECK the KEY State (pressed or depressed)
		rgoto CHCK_KEY_STATE_VALID
	

CHCK_KEY_STATE_VALID
	;; 3) check if the KEY State is pressed or depressed
	; AY_KEY_STATE = 0 [=>PRESSED]
	; KEYBORAD: KEY HANDLING
	movlw  KEY_PRESSED_STATE_0
	cpfseq SM_KEYB_KEY_STATE ;; = 0? (0=PRESSED, 1=DEPRESSED)
		
		;; NO - EXIT HERE
		;; GO ON WITH SUB MENU FUNCTION
		;; @ sub_menu_functions.inc
		goto BRANCH_TO_SUB_MENUE_KEY

			;; YES
			;; KEY STATE IS VALID
			;; GO ON WITH ASCII CODE CHECKING
			rgoto CHCK_ASC_CODE_VALID_0_9_ASC_AREA_START


CHCK_ASC_CODE_VALID_0_9_ASC_AREA_START
	;; 4) Check KEY ASCII CODE: RANGE [0-9] 
	; CHECK if the KEY ASC CODE is in the ASC CODE [0-9] RANGE
	; (MAIN MENU =1, ITEM =1, KEY State = PRESSED)
	; CECK ASC CODE [0-9] START AREA = VALID?
	; IS PATCH_NAME_ASC_KEY > 0x2F ([0x30 = "0" in ASC CODE]) ?
	movff PATCH_NAME_ASC_KEY, MIOS_PARAMETER1
	; CHECK IF PATCH_NAME_ASC_KEY IS > 0x2F 
	movlw PATCH_NAME_ASC_CODE_0_MINUS1
	cpfsgt MIOS_PARAMETER1
		;; NO - NOT in the Range of ASC [0-9] - EXIT [0-9] checking
		;; GO ON WITH CHECKING ASC [A-Z] RANGE
		rgoto CHCK_ASC_CODE_VALID_A_Z_ASC_AREA_START

		;; YES - OK GO ON AND CHECK IF THE 
		;; ASC [0-9] AREA *END* IS VALID (not bigger than "9")
		rgoto CHCK_ASC_CODE_VALID_0_9_ASC_AREA_END

CHCK_ASC_CODE_VALID_0_9_ASC_AREA_END
	; CHECK IF PATCH_NAME_ASC_KEY IS < 0x3A [0x39 = "9" (ASC)]
	movlw PATCH_NAME_ASC_CODE_9_PLUS1
	cpfslt MIOS_PARAMETER1
		;; NO, ASC CODE is > than 0x39 [9 (ASC)]:
		;; SO ASC CODE IS *NOT* in the ASC [0-9] RANGE!
		;; SO EXIT THE [0-9] ASC AREA HERE AND
		;; GO ON WITH CHECKING THE [A-Z] ASC AREA
		rgoto CHCK_ASC_CODE_VALID_A_Z_ASC_AREA_START

			;; YES - OK VALID [0-9] RANGE
			;; a *NUMBER* key [0-9] had been pressed!
			;; GO ON AND WRITE A ASCI CHAR [0-9] TO THE PATCH NAME
			rgoto ASC_PATCH_NAME_WRITING

CHCK_ASC_CODE_VALID_A_Z_ASC_AREA_START
	;; 5) KEY ASCII CODE: check if the ASC CODE RANGE [A-Z] is VALID 
	; CHECK if the ASC CODE is START AREA ASC [A]
	; CHECK IF PATCH_NAME_ASC_KEY IS > 0x40 [41 = A (ASC)]
	movlw PATCH_NAME_ASC_CODE_A_MINUS1
	cpfsgt MIOS_PARAMETER1
		;; NO - NOT [A-Z] RANGE - EXIT HERE AND
		;; Check the SPACE KEY next
		rgoto CHCK_ASC_CODE_VALID_SPACE_KEY

		;; YES - OK GO ON
		rgoto CHCK_ASC_CODE_VALID_A_Z_ASC_AREA_END

CHCK_ASC_CODE_VALID_A_Z_ASC_AREA_END
	; CHECK IF PATCH_NAME_ASC_KEY IS < 0x5B [0x5A = "Z" (ASC)]
	movlw PATCH_NAME_ASC_CODE_Z_PLUS1
	cpfslt MIOS_PARAMETER1
		;; NO > 0x5A ["Z" (ASC)] - ASC CODE IS NOT [A-Z] ASC RANGE
		;; SO EXIT THE [A-Z] ASC AREA HERE
		;; Check SPACE KEY next
		rgoto CHCK_ASC_CODE_VALID_SPACE_KEY

		;; YES - OK VALID [A-Z] ASC RANGE
		;; GO ON AND WRITE [A-Z] ASCII CHAR TO THE PATCH NAME
		rgoto ASC_PATCH_NAME_WRITING


CHCK_ASC_CODE_VALID_SPACE_KEY
	;; 6) KEY ASCII CODE: check if the SPACE KEY had been pressed 
	; CHECK IF PATCH_NAME_ASC_KEY IS == 0x5F [SPACE KEY "_" for LCD]
	movlw PATCH_NAME_ASC_CODE_SPACE_KEY
	cpfseq MIOS_PARAMETER1
		;; NO not 0x5F [SPACE KEY => underscore in LCD code => "_"]
		;; SO Check CURSOR RIGHT next
		rgoto CHCK_ASC_CODE_VALID_DELITE_KEY

		;; YES = [SPACE KEY] was pressed
		;; WRITE "_"
		rgoto ASC_PATCH_NAME_WRITING


CHCK_ASC_CODE_VALID_DELITE_KEY
	;; 7) KEY ASCII CODE: check if the DEL KEY had been pressed 
	; CHECK IF PATCH_NAME_ASC_KEY IS == 0x14 [DEL KEY]
	movlw PATCH_NAME_ASC_CODE_DEL_KEY
	cpfseq MIOS_PARAMETER1
		;; NO, THE DEL key has not been pressed
		;; SO Check CURSOR RIGHT next
		rgoto CHCK_ASC_CODE_VALID_CURSOR_R

			;; YES = [DEL KEY] was pressed
			;; DEL ASC CHAR @ current POS and WRITE "." @ CURRENT POS x
			;; first check if pos = 0
			rgoto RESET_PATCH_NAME

RESET_PATCH_NAME  	
	;; reset patch name to "........"
	movlw "."
	movwf MIOS_PARAMETER1
	movff MIOS_PARAMETER1, PATCH_NAME_ASC_0
	movff MIOS_PARAMETER1, PATCH_NAME_ASC_1
	movff MIOS_PARAMETER1, PATCH_NAME_ASC_2
	movff MIOS_PARAMETER1, PATCH_NAME_ASC_3
	movff MIOS_PARAMETER1, PATCH_NAME_ASC_4
	movff MIOS_PARAMETER1, PATCH_NAME_ASC_5
	movff MIOS_PARAMETER1, PATCH_NAME_ASC_6
	movff MIOS_PARAMETER1, PATCH_NAME_ASC_7
	
	;; POS1 PATCH NAME
	clrf MIOS_PARAMETER1
	movff MIOS_PARAMETER1, PATCH_NAME_ASC_POS
			
		;; REDRAW LCD
		;; main_menu_01_patch.inc
		goto LABEL_AY_MAIN_MENU_01_PATCH_INFO
				



CHCK_ASC_CODE_VALID_CURSOR_R
	;; 7) KEY ASCII CODE: check if the CURSOR RIGHT KEY had been pressed 
	; CHECK IF PATCH_NAME_ASC_KEY IS == 0x1D [CURSOR RIGHT KEY]
	movlw PATCH_NAME_CURSOR_RIGHT_KEY
	cpfseq MIOS_PARAMETER1
		;; NO not [CURSOR RIGHT KEY]
		;; GO and Check CURSOR Left next
		rgoto CHCK_ASC_CODE_VALID_CURSOR_L

		;; YES [CURSOR RIGHT KEY] was pressed
		;; INC POS OF CURSOR @ THE PATCH NAME
		rgoto INC_PATCH_NAME_POS


CHCK_ASC_CODE_VALID_CURSOR_L
	;; 8) KEY ASCII CODE: check if the CURSOR LEFT KEY had been pressed 
	; CHECK IF PATCH_NAME_ASC_KEY IS == 0x9D [CURSOR LEFT KEY]
	movlw PATCH_NAME_CURSOR_LEFT_KEY
	cpfseq MIOS_PARAMETER1
		;; NO not [CURSOR LEFT KEY]
		;; SO EXIT HERE AND
		;; GO ON WITH [SUB MENU - KEY SWITCH]
		;; @ sub_menu_functions.inc
		goto BRANCH_TO_SUB_MENUE_KEY

		;; YES [CURSOR LEFT KEY]
		;; DEC POS OF CURSOR @ THE PATCH NAME
		rgoto DEC_PATCH_NAME_POS


;; -------------[END of MENU, ITEM an KEY Checking]--------------------

;; -------------[Start of PATCH NAME EDITING Routines]-----------------

INC_PATCH_NAME_POS
	
	;; INC PATCH_NAME_ASC_POS [0-7]
	;; IF PATCH_NAME_ASC_POS = 7 (CURSOR POS MAX) do not inc
	movff PATCH_NAME_ASC_POS, MIOS_PARAMETER1
	
	movlw PATCH_NAME_CURSOR_POS_MAX	
	cpfseq MIOS_PARAMETER1
	
		;; NO, POS is not 7 (CURSOR POS MAX)
		rgoto DO_INC_PATCH_NAME_CURSOR_POS

		;; YES = CURSOR POS MAX
		;; DO NOT INC CURSOR POS
		return

DO_INC_PATCH_NAME_CURSOR_POS	
	incf MIOS_PARAMETER1, F
	movff MIOS_PARAMETER1, PATCH_NAME_ASC_POS
		return

DEC_PATCH_NAME_POS
	;; DEC PATCH_NAME_ASC_POS [0-7]
	;; IF PATCH_NAME_ASC_POS = 0 DO NOT DEC
	movff 	PATCH_NAME_ASC_POS, MIOS_PARAMETER1
	movlw 	0x00
	cpfseq 	MIOS_PARAMETER1
	
		;; NO, POS is not 0
		;; SO IT'S ALLRIGHT TO DEC CURSOR POS -1
		rgoto DO_DEC_PATCH_NAME_CURSOR_POS

		;; YES
		;; DO NOT DEC POS
		return

DO_DEC_PATCH_NAME_CURSOR_POS	
	decf 	MIOS_PARAMETER1, F
	movff 	MIOS_PARAMETER1, PATCH_NAME_ASC_POS
		return



ASC_PATCH_NAME_WRITING
		movff PATCH_NAME_ASC_POS, MIOS_PARAMETER1
		movf 	MIOS_PARAMETER1, W
		;; index is in WREG now
		JUMPTABLE_2BYTES 8        ; (8 entries)
				rgoto        WRITE_PATCH_NAME_ASC_POS1
 				rgoto        WRITE_PATCH_NAME_ASC_POS2
 				rgoto        WRITE_PATCH_NAME_ASC_POS3
 				rgoto        WRITE_PATCH_NAME_ASC_POS4
 				rgoto        WRITE_PATCH_NAME_ASC_POS5
 				rgoto        WRITE_PATCH_NAME_ASC_POS6
 				rgoto        WRITE_PATCH_NAME_ASC_POS7
 				rgoto        WRITE_PATCH_NAME_ASC_POS8


WRITE_PATCH_NAME_ASC_POS1
	movff PATCH_NAME_ASC_KEY, PATCH_NAME_ASC_0
		rgoto ASC_PATCH_NAME_WRITING_TO_LCD

WRITE_PATCH_NAME_ASC_POS2
	movff PATCH_NAME_ASC_KEY, PATCH_NAME_ASC_1
		rgoto ASC_PATCH_NAME_WRITING_TO_LCD

WRITE_PATCH_NAME_ASC_POS3
	movff PATCH_NAME_ASC_KEY, PATCH_NAME_ASC_2
		rgoto ASC_PATCH_NAME_WRITING_TO_LCD

WRITE_PATCH_NAME_ASC_POS4
	movff PATCH_NAME_ASC_KEY, PATCH_NAME_ASC_3
		rgoto ASC_PATCH_NAME_WRITING_TO_LCD

WRITE_PATCH_NAME_ASC_POS5
	movff PATCH_NAME_ASC_KEY, PATCH_NAME_ASC_4
		rgoto ASC_PATCH_NAME_WRITING_TO_LCD

WRITE_PATCH_NAME_ASC_POS6
	movff PATCH_NAME_ASC_KEY, PATCH_NAME_ASC_5
		rgoto ASC_PATCH_NAME_WRITING_TO_LCD

WRITE_PATCH_NAME_ASC_POS7
	movff PATCH_NAME_ASC_KEY, PATCH_NAME_ASC_6
		rgoto ASC_PATCH_NAME_WRITING_TO_LCD

WRITE_PATCH_NAME_ASC_POS8
	movff PATCH_NAME_ASC_KEY, PATCH_NAME_ASC_7
		rgoto ASC_PATCH_NAME_WRITING_TO_LCD



ASC_PATCH_NAME_WRITING_TO_LCD
		    	;;WRITE ASCII CHAR
		 	;; SET CURSOR
		 	movlw 	LCD_ROW1 + 8 
		 	movff 	PATCH_NAME_ASC_POS, MIOS_PARAMETER1
			addwf   MIOS_PARAMETER1, W
		 	call    MIOS_LCD_CursorSet
		 	;; WRITE ASCII CHAR
		 	movff   PATCH_NAME_ASC_KEY, MIOS_PARAMETER1
		 	movf 	MIOS_PARAMETER1, W
		 	call    MIOS_LCD_PrintChar
			
			movff 	PATCH_NAME_ASC_POS, MIOS_PARAMETER1
			incf 	MIOS_PARAMETER1, F
			movff	MIOS_PARAMETER1, PATCH_NAME_ASC_POS

			;; SET THE "NEW PATCH NAME" bit5
			;; BECAUSE THE USER EDITED THE PATCH NAME
			bsf	PATCH_MENU_STATE_FLAGS, BIT5_PATCH_NAME_IS_NEW

				;; CHECK: CURSOR POS > 7 (PATCH_NAME_CURSOR_POS_MAX)?
				movlw PATCH_NAME_CURSOR_POS_MAX
				cpfsgt MIOS_PARAMETER1
					;; NO,
					;; OK - JUST EXIT HERE
					return

						;; YES,
						;; WRITE PATCH_NAME_CURSOR_POS_MAX to CURSOR PATCH NAME POS
						movlw PATCH_NAME_CURSOR_POS_MAX
						movwf MIOS_PARAMETER1
						movff MIOS_PARAMETER1, PATCH_NAME_ASC_POS
						;; EXIT HERE
			 			return



;; ---------------------------------------------------------------



;; ---------------------------------------------------------------
;; -----------[SUBROUTINE: WRITE [PATCH NAME] on LCD]-------------
;; ---------------------------------------------------------------
WRITE_PATCH_NAME_ON_LCD
		;; WRITE PATCH NAME ON LCD ROW1 
		
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
					
					;; EXIT HERE
					return
					
;; -----------[END: WRITE [PATCH NAME] ON LCD]--------------------
;; ---------------------------------------------------------------
