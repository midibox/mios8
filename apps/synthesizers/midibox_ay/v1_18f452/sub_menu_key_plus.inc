;; sub_menu_key_plus.inc

;; ----------------------------------------------------------------------------
;; -------------------------[PLUS]----------------------------- 
;; =======================> [+] HAD BEEN PRESSED
;; ================> Jumped in here from sub_menu_functions.inc


LABEL_AY_SUB_MENU_PLUS

LABEL_AY_SUB_MENU_PLUS_CHK_MAINMENU
	;; CHECK WHICH MAIN MENU IS ACTIVATED
	;; ==> VIA CURRENT_MAIN_MENU

	;; JUMP TO KEY @ MENU X
	movf CURRENT_MAIN_MENU, W
	JUMPTABLE_4BYTES 9 ;; [MENU 0 to 8]
        	goto        PLUS_KEY_SUBROUTINE_AT_MAIN_MENU0 ;; MENU OVERVIEW 		(ESC)
        	goto        PLUS_KEY_SUBROUTINE_AT_MAIN_MENU1 ;; PATCH MENU 		(F1)
        	goto        PLUS_KEY_SUBROUTINE_AT_MAIN_MENU2 ;; DETUNE ABC 		(F2)
        	goto        PLUS_KEY_SUBROUTINE_AT_MAIN_MENU3 ;; ABC + NOISE MIXER 	(F3)
        	goto        PLUS_KEY_SUBROUTINE_AT_MAIN_MENU4 ;; VOLUME 		(F4)
        	goto        PLUS_KEY_SUBROUTINE_AT_MAIN_MENU5 ;; ENVELOPE 		(F5)
        	goto        PLUS_KEY_SUBROUTINE_AT_MAIN_MENU6 ;; MIDI CH. CONFIG 	(F6)
        	goto        PLUS_KEY_SUBROUTINE_AT_MAIN_MENU7 ;; VELOCITY 		(F7)
		goto        PLUS_KEY_SUBROUTINE_AT_MAIN_MENU8 ;; Fx Menu 		(F8)

;; -------[END OF: CHECK WHICH MAIN MENU IS ACTIVATED]---
;; ----------------------------------------------------------------

;; --------[END OF KEY [PLUS (+)] FUNCTIONS ]------------------
