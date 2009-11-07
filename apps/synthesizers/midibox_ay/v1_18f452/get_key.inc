;; get_key.inc

GET_KEY
;; jumped in here from main.asm >> USER_DIN_NotifyToggle
;; (A Key has been pressed)

	;; RESET SUB MENU KEY VALUE
	movlw 0x00
	movwf AY_SUB_MENUE_KEY_SWITCH

	call DECODE_KEYBOARD_BUTTON
	;; => @keyb_decoding.inc
		
	;; 1)
	;; IF A *SUB MENU* KEY ([+] /[-] / etc.) HAS BEEN PRESSED
	;; A VALUE HAS BEEN WRITTEN TO 
	;; => AY_SUB_MENUE_KEY_SWITCH
	;; (FOR THE *SUB MENU* JUMPTABLE)
	;; 
	;; 2)
	;; IF A *MAIN MENU* KEY (F1, F2, ESC, etc.) HAS BEEN PRESSED
	;; A VALUE HAS BEEN WRITTEN TO 
	;; => CURRENT_MAIN_MENU
	;; (FOR THE *MAIN MENU* JUMPTABLE)
	;;
	;;2a)
	;; MAIN OR SUB MENU KEY? 
	;; GO MAINMENU ==> SWITCH_BETWEEN_MAIN_AND_SUBMENU 	= 0
	;; GO SUBMENU ==> SWITCH_BETWEEN_MAIN_AND_SUBMENU 	= 1
	;; IF A MAIN MENU KEY HAS BEEN PRESSED 
	;; TOGGLE TO MAIN MENU VIA
	;; => SWITCH_BETWEEN_MAIN_AND_SUBMENU = 0
	 
	
  	;; The [GO MAIN MENU [or] SUB MENU] switch
 	;; IS SWITCH_BETWEEN_MAIN_AND_SUBMENU = 0 ?
  	movlw 	0x00
  	cpfseq 	SWITCH_BETWEEN_MAIN_AND_SUBMENU
   
  	    	;; NO
  	    	;; SWITCH_BETWEEN_MAIN_AND_SUBMENU = 0x01
  	       	;; GO ON WITH A *SUB MENU* ROUTINE
  	   	;; SWITCH_BETWEEN_MAIN_AND_SUBMENU IS = 1
  	    	;; RUN A *SUB MENU* ROUTINE
  	    	;; A VALUE HAS BEEN WRITTEN TO 
  	    	;; => AY_SUB_MENUE_KEY_SWITCH (GO *SUB MENU* JUMPTABLE)
  	    	;; => @ sub_menu_functions.inc
  	    	goto 	LABEL_SUB_MENUE_SWITCH
      
      
      		;; YES
      		;; SWITCH_BETWEEN_MAIN_AND_SUBMENU = 0x00
      		;; SO GO ON WITH A MAIN MENU ROUTINE
      		;; RUN A *MAIN MENU* ROUTINE
      		;; A VALUE HAS BEEN WRITTEN TO 
      		;; => CURRENT_MAIN_MENU (GO *MAIN MENU* JUMPTABLE)
      		;; => @ main_menue_functions.inc
      		goto 	LABEL_MAIN_MENUE_SWITCH 	





;; -----------[get_key.inc END]--------------



