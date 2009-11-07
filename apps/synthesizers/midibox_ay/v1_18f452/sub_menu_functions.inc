;; -----------------------
;; sub_menu_functions.inc
;; -----------------------

LABEL_SUB_MENUE_SWITCH

;; ----------[SUB MENU SWITCH]------------------
;; ----------------------------------------------
;; JUMP IN HERE FROM get_key.inc
;; A KEY ON THE KEYBOARD HAS BEEN PRESSED
;; AND A VALUE COULD HAVE BEEN WRITTEN TO:
;; ==> AY_SUB_MENUE_KEY_SWITCH and/or to 
;; ==> PATCH_NAME_ASC_KEY 


	;; -------------[PATCHNAME interception]--------------------------
	;; CHECK IF THERE IS A NEED FOR GOING TO 
	;; THE *WRITE THE PATCH NAME* SUBMENU ROUTINE
	;; @ Main MENU1 (write patch name @ Patch menu)
	;; MENU, ITEM an ASCII CODE KEY Checking for PATCH NAME WRITING
	;; @ eeprom_patch_name_writing.inc
	goto CHCK_NEED_FOR_PATCHNAME_EDITING
	;; -------------[END of WRITE PATCHNAME interception]----------------


;; BRANCH TO SUB MENU FUNCTION 
;; SUBMENU KEY => SUB MENU FUNCTION 
BRANCH_TO_SUB_MENUE_KEY

	;; KEYBORAD: KEY HANDLING
	;; IF MAIN MENU KEY 
	;; IS = DEPRESSED (AY_KEY_STATE=1)
	;; THEN DO NOT EXECUTE A
	;; MAIN MENU FUNCTION
	movlw 0x00			;; 0 = KEY PRESSED, 1 = DEPRESSED
	cpfseq SM_KEYB_KEY_STATE 	;; = 0?
		
		;; NO, Exit Here
		goto LABEL_AY_SUB_MENU_EXIT

		;; YES
		rgoto BRANCH_TO_SUB_MENUE_KEY_GO


BRANCH_TO_SUB_MENUE_KEY_GO

	;; YES KEY WAS PRESSED	
	;; ------[WHICH SUBMENU KEY HAT BEEN PRESSED?]-------------------
	;; JUMP TO SUB MENU KEY 
	movf AY_SUB_MENUE_KEY_SWITCH, W
	JUMPTABLE_4BYTES 11	;; [0 - 10]
		goto        LABEL_AY_SUB_MENU_EXIT	;; 0 ==> SKIP HERE
		goto        LABEL_AY_SUB_MENU_UP	;; 1 ==> sub_menu_key_up.inc - CUSOR UP etc
		goto        LABEL_AY_SUB_MENU_DOWN	;; 2 ==> sub_menu_key_down.inc - RETURN, CUSOR DOWN etc
		goto        LABEL_AY_SUB_MENU_PLUS	;; 3 ==> sub_menu_key_plus.inc - [+], CUSOR R etc
		goto        LABEL_AY_SUB_MENU_MINUS	;; 4 ==> sub_menu_key_minus.inc - [-], CUSOR L etc
		goto        LABEL_AY_SUB_MENU_A		;; 5 ==> sub_menu_key_a.inc [SUB MENU KEY: [A]]
		goto        LABEL_AY_SUB_MENU_B		;; 6 ==> sub_menu_key_b.inc [SUB MENU KEY: [B]]
		goto        LABEL_AY_SUB_MENU_C		;; 7 ==> sub_menu_key_c.inc [SUB MENU KEY: [C]]
		goto	    PATCH_WRITE			;; 8 ==> patch_write.inc [SUB MENU KEY:    [W]]
		goto	    PATCH_LOAD			;; 9 ==> patch_load.inc [SUB MENU KEY:     [L]]
		goto	    FORMAT_BANKSTICK		;;10 ==> format_bankstick.inc[SUB MENU KEY:[F]]
							;; --    see allso patch_write.inc 

	;; SUB FUNCTIONS:
	;; IF NO BUTTON MATCHED AY_SUB_MENUE_KEY_SWITCH = 0
	;; UP	 	- => AY_SUB_MENUE_KEY_SWITCH = 0x01 / SUB MENU FUNC.:EDITING MODE DEC. POS -1  
	;; DOWN	 	- => AY_SUB_MENUE_KEY_SWITCH = 0x02 / SUB MENU FUNC.:EDITING MODE INC. POS +1 
	;; + (PLUS) 	- => AY_SUB_MENUE_KEY_SWITCH = 0x03 / SUB MENU FUNC.:VALUE = VALUE +1
	;; - (MINUS) 	- => AY_SUB_MENUE_KEY_SWITCH = 0x04 / SUB MENU FUNC.:VALUE = VALUE -1
	;; A	 	- => AY_SUB_MENUE_KEY_SWITCH = 0x05 / TOGGLE TONE A @ TONE A USER MENU etc
	;; B 		- => AY_SUB_MENUE_KEY_SWITCH = 0x06 / TOGGLE TONE B @ TONE B USER MENU etc 
	;; C	 	- => AY_SUB_MENUE_KEY_SWITCH = 0x07 / TOGGLE TONE C @ TONE C USER MENU etc	
	;; ...
			  


;; ----------[SUB MENU SWITCH END]-----------------------
;; ------------------------------------------------------

;; -----[EXIT FROM SUB MENU FUNCTIONS]-------------------
LABEL_AY_SUB_MENU_EXIT
	;; NOTHING TO DO EXIT
	return



