;; sub_menu_key_minus.inc

;; ----------------------------------------------------------------------------
;; -------------------------[MINUS]-------------------------------------------- 
;; ===============> [-] KEY HAD BEEN PRESSED
;; ===================> SWITCH TO THE corresponding SUB MENU ROUTINE


LABEL_AY_SUB_MENU_MINUS
	;; 1) CHECK WHICH MAIN MENU IS ACTIVATED
	;; ==> VIA CURRENT_MAIN_MENU
	;;
	;; 2) GOTO THE NEEDED SUB ROUTINE OF THE ACTIVATED
	;;    MAIN MENU

	;; JUMP TO KEY @ MENU X
	movf CURRENT_MAIN_MENU, W
	JUMPTABLE_4BYTES 9 ;; [MENU 0 to 8]
		goto        MINUS_KEY_SUBROUTINE_AT_MAIN_MENU0			;; MENU OVERVIEW (VIA ESC)
		goto        MINUS_KEY_SUBROUTINE_AT_MAIN_MENU1
		goto        MINUS_KEY_SUBROUTINE_AT_MAIN_MENU2
		goto        MINUS_KEY_SUBROUTINE_AT_MAIN_MENU3
		goto        MINUS_KEY_SUBROUTINE_AT_MAIN_MENU4
		goto        MINUS_KEY_SUBROUTINE_AT_MAIN_MENU5
		goto        MINUS_KEY_SUBROUTINE_AT_MAIN_MENU6
		goto	    MINUS_KEY_SUBROUTINE_AT_MAIN_MENU7
		goto	    MINUS_KEY_SUBROUTINE_AT_MAIN_MENU8	 		;; Fx Menu (F8)
		


;; -------[END OF: CHECK WHICH MAIN MENU IS ACTIVATED]---
;; ----------------------------------------------------------------
	
;; -------------------------[END OF SUB MENU: MINUS]----------------------------- 

