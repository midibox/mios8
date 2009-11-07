;; sub_menu_key_plus_menu_1.inc


;; --------[PLUS @ MAIN MENU1 - PATCH MENU]------------------

PLUS_KEY_SUBROUTINE_AT_MAIN_MENU1
	;; Jumped in here from sub_menu_key_plus.inc
	
	;; 1) CHECK ITEM = EDITING MODE "*"
	;;
	;; 2) INC ITEM 

;; --[ 1) CHECK ITEM]-------

	;; CHECK LOCATION VIA EDITING MODE "*"
	;; COPY THE VALUE OF EDITING MODE TO  MIOS_PARAMETER1
	movff 	AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO, MIOS_PARAMETER1

LABEL_AY_SUB_MENU_PLUS_MAIN_MENU1_CHK_ITEM2	
	movlw 	MENU_ITEM_POS2
	cpfseq 	MIOS_PARAMETER1 ;; EDITING MODE ITEM  = 2?

				;; NO
				;; Exit here
				return

		;; YES
		;; INC ITEM2
		;; ITEM2 => PATCH NUMBER [INT(0) - 1-128 (BANKSTICK])
		;; => RANGE 0    - 128   DEC.
		;; => RANGE 0x00 - 0x80  HEX.
		;; CHEK ITEM VALUE 
		;; compare f with WREG, 
		;; skip if CURREND_PATCH_NR = PATCH_NUMBER_VALUE_MAX
		movlw 	PATCH_NUMBER_VALUE_MAX
		cpfseq 	BROWSE_PATCH_NR
		
			;; NO
			;; CURREND_PATCH_NR < PATCH_NUMBER_VALUE_MAX
			rgoto LABEL_AY_SUB_MENU_PLUS_MAIN_MENU1_INC_ITEM2

				;; YES 
				;; CURREND_PATCH_NR = PATCH_NUMBER_VALUE_MAX
				;; Exit Here
				return


LABEL_AY_SUB_MENU_PLUS_MAIN_MENU1_INC_ITEM2
			;; ITEM [PATCH NUMBER] = ITEM +1
			incf BROWSE_PATCH_NR, F


;; -------------------------------------[exit here]------------------------------
				;; Redraw LCD Main Menu
				goto LABEL_AY_MAIN_MENU_01_PATCH_INFO


