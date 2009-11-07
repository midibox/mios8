;; sub_menu_minus_menu_1.inc



;; --------[MINUS @ MAIN MENU1]------------------
;; JUMPED IN HERE FROM sub_menu_key_minus.inc
MINUS_KEY_SUBROUTINE_AT_MAIN_MENU1
	;; MAIN MENU1 = PATCH INFO MENU
	;; MINUS IS NEEDED HERE SO

	;; Jumped in here from sub_menu_key_plus.inc
	
	;; 1) CHECK ITEM = EDITING MODE "*"
	;;
	;; 2) INC ITEM 

;; --[ 1) CHECK ITEM]-------

	;; CHECK LOCATION VIA EDITING MODE "*"
	;; COPY THE VALUE OF EDITING MODE TO  MIOS_PARAMETER1
	movff 	AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO, MIOS_PARAMETER1

LABEL_AY_SUB_MENU_MINUS_MAIN_MENU1_CHK_ITEM2	
	movlw 	MENU_ITEM_POS2
	cpfseq 	MIOS_PARAMETER1 ;; EDITING MODE ITEM  = 2?

			;; NO
			;; Exit here
			return

		;; YES
		;; SO DEC ITEM2
		;; ITEM2 => PATCH NUMBER [INT(0) - 1-128 (BANKSTICK])
		;; => RANGE 0    - 128   DEC.
		;; => RANGE 0x00 - 0x80  HEX.
		;; CHEK ITEM VALUE 
		;; compare f with WREG, 
		;; skip if BROWSE_PATCH_NR = 0
		clrw 	;; 0
		cpfseq 	BROWSE_PATCH_NR
		
			;; NO
			;; BROWSE_PATCH_NR > PATCH_NUMBER_VALUE_MAX
			rgoto LABEL_AY_SUB_MENU_MINUS_MAIN_MENU1_DEC_ITEM2

					;; YES 
					;; BROWSE_PATCH_NR = PATCH_NUMBER_VALUE_MAX
					;; Exit here
					return


LABEL_AY_SUB_MENU_MINUS_MAIN_MENU1_DEC_ITEM2
	;; ITEM [PATCH NUMBER] = ITEM -1
	decf BROWSE_PATCH_NR, F
	


;; -------------------------------------[exit here]------------------------------
					;; Redraw MAIN MENU LCD
					;; main_menu_01_patch.inc
					goto LABEL_AY_MAIN_MENU_01_PATCH_INFO
				

