;; user_tick_lcd_redraw.inc
;; 
;; FUNCTION:
;; CHECKING FOR REDRAW THE MAIN MENU
;; IS THERE WAS A CC / PITCH BEND / MODULATION WHEEL 
;; EVENT @ THE MATCHING MAIN MENU?...


CHECK_EVENT_MENU_REDRAW

		;; CHECK IF A MIDI EVENT 
		;; (CC / PITCH BEND / MODULATION WHEEL) 
		;; VALUE HAS Been Written 
		;; IF NO MIDI VALUE HAS BEEN WRITTEN 
		;; THE MIDI_EVENT_CHANGE_FLAG MUST BE == 0
		;; IF A MIDI VALUE HAS BEEN WRITTEN
		;; THE MIDI_EVENT_CHANGE_FLAG == THE CC / PITCH BEND / MODULATION WHEEL VALUE 
		;; (*NOT* THE CC, PITCH BEND, MOD WHEEL VALUE!)
		;; SO IF THERE WAS A PITCH BEND / MODULATION WHEEL / CONTROLLER CHANGE
		;; A NUMBER HAS BEEN WRITTEN
		;; TO THE MIDI_EVENT_CHANGE_FLAG 
		;; CC RANGE 			== 0 to 127
		;; EXEPTION: PITCH BEND 	== 30 
		;; EXEPTION: MOD WHEEL		== 32
		clrw
		cpfsgt MIDI_EVENT_CHANGE_FLAG

				;; NO - THE CC FLAG = 0 
				;; SO just QUIT HERE
				rgoto NO_EVENT_FLAG_SO_EXIT

				;; YES - OK, CC FLAG IS ON
				;; SO CLEAR THE CC FLAG
				;; AND THEN CHECK IF THE CONTROLLER
				;; HIT A ITEM IN THE ACTIVE
				;; MAIN MENU
				rgoto LCD_REDRAW_EVENT_JUMTABLE

;; ---------------------
				
NO_EVENT_FLAG_SO_EXIT
		  ;; NO MAIN MENU REWRITING
		  ;; CC Change Flag = 0
		  ;; EXIT HERE
		  return

;; ----------------------

LCD_REDRAW_EVENT_JUMTABLE
	;; Branch MIDI MIDI_EVENT_CHANGE_FLAG
	;; 
	movf MIDI_EVENT_CHANGE_FLAG, W
	
	JUMPTABLE_2BYTES 33 ; entries (0-32)
	      	rgoto	USER_TICK_CC_DO_NO_REDRAW		; CONTROLLER 00 - NOT USED 
	      	rgoto	CC1TO5_CHECK_IF_MAIN_MENU_IS_5		; CONTROLLER 01 - ENVELOPE SHAPE 
		rgoto	CC1TO5_CHECK_IF_MAIN_MENU_IS_5		; CONTROLLER 02 - ENV. FINE 
	      	rgoto	CC1TO5_CHECK_IF_MAIN_MENU_IS_5		; CONTROLLER 03 - ENV. FINE OFFSET + 128 (BIT7)
	      	rgoto	CC1TO5_CHECK_IF_MAIN_MENU_IS_5		; CONTROLLER 04 - ENV. COARSE
	      	rgoto	CC1TO5_CHECK_IF_MAIN_MENU_IS_5		; CONTROLLER 05 - ENV. COARSE OFFSET + 128 (BIT7)
		rgoto	CC6_CHECK_IF_MAIN_MENU_IS_3		; CONTROLLER 06	- NOISE PERIOD
		rgoto	CC7TO9_CHECK_IF_MAIN_MENU_IS_4		; CONTROLLER 07	- VOLUME A
		rgoto	CC7TO9_CHECK_IF_MAIN_MENU_IS_4		; CONTROLLER 08	- VOLUME B
		rgoto	CC7TO9_CHECK_IF_MAIN_MENU_IS_4		; CONTROLLER 09	- VOLUME C
		rgoto	CC10TO15_CHECK_IF_MAIN_MENU_IS_3	; CONTROLLER 10	- ON/OFF A
		rgoto	CC10TO15_CHECK_IF_MAIN_MENU_IS_3	; CONTROLLER 11	- ON/OFF B
		rgoto	CC10TO15_CHECK_IF_MAIN_MENU_IS_3	; CONTROLLER 12	- ON/OFF C
		rgoto	CC10TO15_CHECK_IF_MAIN_MENU_IS_3	; CONTROLLER 13	- NOISE A ON/OFF
		rgoto	CC10TO15_CHECK_IF_MAIN_MENU_IS_3	; CONTROLLER 14	- NOISE B ON/OFF
		rgoto	CC10TO15_CHECK_IF_MAIN_MENU_IS_3	; CONTROLLER 15	- NOISE C ON/OFF
		rgoto	CC16TO18_CHECK_IF_MAIN_MENU_IS_5	; CONTROLLER 16	- TONE A ENV. ON/OFF
		rgoto	CC16TO18_CHECK_IF_MAIN_MENU_IS_5	; CONTROLLER 17	- TONE B ENV. ON/OFF
		rgoto	CC16TO18_CHECK_IF_MAIN_MENU_IS_5	; CONTROLLER 18	- TONE C ENV. ON/OFF
		rgoto	CC19TO22_CHECK_IF_MAIN_MENU_IS_7	; CONTROLLER 19	- TONE A VELOCITY TO VOLUME ON/OFF
		rgoto	CC19TO22_CHECK_IF_MAIN_MENU_IS_7	; CONTROLLER 20	- TONE B VELOCITY TO VOLUME ON/OFF
		rgoto	CC19TO22_CHECK_IF_MAIN_MENU_IS_7	; CONTROLLER 21	- TONE C VELOCITY TO VOLUME ON/OFF
		rgoto	CC19TO22_CHECK_IF_MAIN_MENU_IS_7	; CONTROLLER 22	- VELOCITY TO NOISE PERIOD ABC ON/OFF
		rgoto	USER_TICK_CC_DO_NO_REDRAW		; CONTROLLER 23	- RESERVED for CC
		rgoto	USER_TICK_CC_DO_NO_REDRAW		; CONTROLLER 24	- RESERVED
		rgoto	USER_TICK_CC_DO_NO_REDRAW		; CONTROLLER 25	- RESERVED
		rgoto	USER_TICK_CC_DO_NO_REDRAW		; CONTROLLER 26	- RESERVED
		rgoto	USER_TICK_CC_DO_NO_REDRAW		; CONTROLLER 27	- RESERVED
		rgoto	USER_TICK_CC_DO_NO_REDRAW		; CONTROLLER 28	- RESERVED
		rgoto	USER_TICK_CC_DO_NO_REDRAW		; CONTROLLER 29	- RESERVED
		rgoto	USER_TICK_PITCH_BEND_CHECK_IF_MAIN_MENU_IS_2	; [30] PITCH BEND - PITCH BEND TONE A B C
		rgoto	USER_TICK_CC_DO_NO_REDRAW			; [31] - RESERVED
		rgoto	USER_TICK_CC_DO_NO_REDRAW			; [32] - MODULATION WHEEL
		


;; ----------------------------------------------------------------
;; ---------[EXIT AND DO *NOT* REDRAW THE MAIN MENU]---------------
USER_TICK_CC_DO_NO_REDRAW	
	;;RESET CC FLAG
	movlw 0x00
	movwf MIDI_EVENT_CHANGE_FLAG
		;; EXIT 
		return


;; -------------------------------------------------------------------
;; -------------------------------------------------------------------


;; ----------[CC IS 1-5 CHECK IF MAIN MENU = ENVELOPE (5) ?]-----------------

CC1TO5_CHECK_IF_MAIN_MENU_IS_5

		;; MAIN MENU = 5 ?
		movlw MAIN_MENU_05
		cpfseq CURRENT_MAIN_MENU
		
			;; NO - OK, QUIT 
			goto USER_TICK_CC_DO_NO_REDRAW

			; YES 
			goto USER_TICK_CC_DO_MAIN_MENU_REDRAW


;; ---------[END: CHECK CC 1-5 AND MAIN MENU 5 (ENV)------------------------------

;; ---------[CC=6 (NOISE PERIOD) - IS MAIN MENU = 3 (MIXER)?]--------------------

CC6_CHECK_IF_MAIN_MENU_IS_3
	;; CC = 6 => NOISE PERIOD HAS BEEN WRITTEN
		;; CHECK IF MAIN MENU = 3
		movlw MAIN_MENU_03
		cpfseq CURRENT_MAIN_MENU
		
			;; NO 
			goto USER_TICK_CC_DO_NO_REDRAW

			; YES 
			goto USER_TICK_CC_DO_MAIN_MENU_REDRAW
	

;; ---------[IS MAIN MENU = 4 (TONE A B C VOLUME)?]--------------------
CC7TO9_CHECK_IF_MAIN_MENU_IS_4
		;; MAIN MENU = 4 (TONE A B C VOLUME)?
		movlw MAIN_MENU_04
		cpfseq CURRENT_MAIN_MENU
		
			;; NO - OK, QUIT 
			goto USER_TICK_CC_DO_NO_REDRAW

			; YES 
			goto USER_TICK_CC_DO_MAIN_MENU_REDRAW


;; ---------[IS MAIN MENU = 3 (TONE A B C + NOISE = ON or OFF)?]--------------------
CC10TO15_CHECK_IF_MAIN_MENU_IS_3
		;; MAIN MENU = 3 (MIXER - TONE ABC and NOISE ABC => ON OFF)?
		movlw MAIN_MENU_03
		cpfseq CURRENT_MAIN_MENU
		
			;; NO - OK, QUIT 
			goto USER_TICK_CC_DO_NO_REDRAW

			; YES 
			goto USER_TICK_CC_DO_MAIN_MENU_REDRAW



;; ---------[IS MAIN MENU = 5 (TONE A B C VELOCITY MAIN MENU)?]--------------------
CC16TO18_CHECK_IF_MAIN_MENU_IS_5

		;; MAIN MENU = 5?
		movlw MAIN_MENU_05
		cpfseq CURRENT_MAIN_MENU
			;; NO - OK, QUIT 
			goto USER_TICK_CC_DO_NO_REDRAW
			; YES 
			goto USER_TICK_CC_DO_MAIN_MENU_REDRAW



;; ---------[IS MAIN MENU = 7 (TONE A B C ENVELOPE MAIN MENU)?]--------------------
CC19TO22_CHECK_IF_MAIN_MENU_IS_7
		;; MAIN MENU = 7?
		movlw MAIN_MENU_07
		cpfseq CURRENT_MAIN_MENU
			;; NO - OK, QUIT 
			goto USER_TICK_CC_DO_NO_REDRAW
			; YES 
			goto USER_TICK_CC_DO_MAIN_MENU_REDRAW


;; ---------[IS MAIN MENU == 2 (DETUNE A B C / PITCH BEND A B C)]-------
USER_TICK_PITCH_BEND_CHECK_IF_MAIN_MENU_IS_2
		;; MAIN MENU = 2?
		movlw MAIN_MENU_02
		cpfseq CURRENT_MAIN_MENU
			;; NO - OK, QUIT 
			goto USER_TICK_CC_DO_NO_REDRAW
			; YES 
			goto USER_TICK_CC_DO_MAIN_MENU_REDRAW



;; ----------------------------------------------------------------
;; ---------[*REDRAW LCD / currend MAIN MENU*]---------------------
USER_TICK_CC_DO_MAIN_MENU_REDRAW	
			
			;;RESET CC FLAG
			movlw 0x00
			movwf MIDI_EVENT_CHANGE_FLAG
	
				;; CC VALUE MATCH MAIN MENU!
				;; AND A CC VALUE HAS CHANGED - SO WRITE
				;; MAIN MENU TO SHOW THE CHANGEND VAUES
				goto LABEL_MAIN_MENUE_SWITCH_GO