;; app_defines.h
;; Specific Defines, 
;; constants and variables 
;; of the MIOS Application
; 
; ==========================================================================
;; MEMO:
;; free memory for user applications:	
;; YOU CAN USE 		=> 0x010-0x37f
;; BUT DON'T USE 	=> 0x080-0x0FF [SFR's] 
; ==========================================================================

;; =========================================================================
;; ======================[START: ACCESS RAM AREA]===========================
;; =========================================================================
;; -------------------------------------------------------------------------
;; ----------[ACESS RAM - RAM BANK0 [[0x010] to [0x07f]]--------------------
;; -------------------------------------------------------------------------

;;  FREE_RAM_BYTE				EQU	0x010

;; used by midi_evnt.inc
;; =========================================
MIDI_EVNT0					EQU	0x011 ;; @ midi_functions.inc
MIDI_EVNT1					EQU	0x012 ;; @ midi_functions.inc
MIDI_EVNT_VALUE					EQU	0x013 ;; @ midi_functions.inc

;; used by sm_simple.inc
SM_BUTTON_COLUMN				EQU	0x014
SM_BUTTON_ROW					EQU	0x015
SM_BUTTON_VALUE					EQU	0x016
SM_SELECTED_COLUMN				EQU	0x017
SM_BUTTON_COLUMN_CTR				EQU	0x018


;; ------------------------------------
;;    APPLICATION WORKING REGISTERS
;; ------------------------------------
;; ----------------------------------------------------------------------------------------------
;; These Regs are used to check the last 
;; *note off* midi event == the last *note on* midi event @ Tone a|b|c
;; if so, switch off tone a|b|c if *not* - ignore the note off event
CURRENT_MIDI_EVENT_NOTE_ON_OFF_A		EQU   	0x01B 	;; => note_on.inc, note_off.inc
CURRENT_MIDI_EVENT_NOTE_ON_OFF_B		EQU   	0x01C 	;; => note_on.inc, note_off.inc
CURRENT_MIDI_EVENT_NOTE_ON_OFF_C		EQU   	0x01D 	;; => note_on.inc, note_off.inc
					      			;; @ main_menu_functions.inc
;; ----------------------------------------------------------------------------------------------
TUNE_COARSE_TEMP				EQU	0x01E	; [TO DO] Fx Call for +/- Tune
TUNE_FINE_TEMP					EQU	0x01F	; [TO DO] Fx Call for +/- Tune
;; ----------------------------------------------------------------------------------------------	
AY_TONE_NOISE_ABC_USER_ON_OFF_SWITCH		EQU	0x020 	; FUNCTION: 
								; USER SWITCH TONE/NOISE ABC ON/OFF 
								; FILTER FOR AY REG 7 (NOTE ON / OFF)
;; ----------------------------------------------------------------------------------------------
MIDI_EVNT_CHANNEL				EQU	0x021 	;; MIDI CHANNEL @ midi_functions.inc
;; ----------------------------------------------------------------------------------------------
AY_TONE_ABC_MIDI_ON_OFF				EQU	0x022 	;; FOR MIDI NOTE ON NOTE OFF SWITCHING
								;; @midi_functions.inc
;; ----------------------------------------------------------------------------------------------
AY_TONE_A_TUNE_FINE_USER                     	EQU   	0x023 ; => midi_functions.inc, main + submenu
AY_TONE_B_TUNE_FINE_USER                     	EQU   	0x024 ; => midi_functions.inc, main + submenu
AY_TONE_C_TUNE_FINE_USER                     	EQU   	0x025 ; => midi_functions.inc, main + submenu
AY_TONE_A_TUNE_COARSE_USER                    	EQU   	0x026 ; => midi_functions.inc, main + submenu
AY_TONE_B_TUNE_COARSE_USER                    	EQU   	0x027 ; => midi_functions.inc, main + submenu
AY_TONE_C_TUNE_COARSE_USER                    	EQU   	0x028 ; => midi_functions.inc, main + submenu
;; ----------------------------------------------------------------------------------------------
AY_TONE_A_TUNE_FINE_MIDI                     	EQU   	0x029 ; => midi_evnt_pipeline.inc
AY_TONE_B_TUNE_FINE_MIDI                     	EQU   	0x02A ; => midi_evnt_pipeline.inc
AY_TONE_C_TUNE_FINE_MIDI                     	EQU   	0x02B ; => midi_evnt_pipeline.inc
AY_TONE_A_TUNE_COARSE_MIDI                    	EQU   	0x02C ; => midi_evnt_pipeline.inc
AY_TONE_B_TUNE_COARSE_MIDI                    	EQU   	0x02D ; => midi_evnt_pipeline.inc
AY_TONE_C_TUNE_COARSE_MIDI                    	EQU   	0x02E ; => midi_evnt_pipeline.inc
;; ----------------------------------------------------------------------------------------------
AY_DETUNE_TEMP					EQU   	0x02F ; => midi_evnt_pipeline.inc
;; ----------------------------------------------------------------------------------------------
AY_TONE_A_MIDI_CHANNEL_USER                  	EQU   	0x030 ; => midi_functions.inc, main + submenu
AY_TONE_B_MIDI_CHANNEL_USER                  	EQU   	0x031 ; => midi_functions.inc, main + submenu
AY_TONE_C_MIDI_CHANNEL_USER                  	EQU   	0x032 ; => midi_functions.inc, main + submenu
;; ----------------------------------------------------------------------------------------------
AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF		EQU   	0x033 ; SWITCH: Velocity to ABC / NOISE PERIOD
					      ; => midi_evnt_pipeline.inc
					      ; => MAIN/SUB MENU 7
					      ; => sub_menu_key_minus_menu_7.inc
					      ; => etc.
					      ; BIT 0 = SET => VELOCITY to VOL. TONE A ON
					      ; BIT 1 = SET => VELOCITY to VOL. TONE B ON
					      ; BIT 2 = SET => VELOCITY to VOL. TONE C ON
					      ; BIT 3 = SET => VELOCITY to NOISE PERIOD ON
					      ; BIT 4 - 
					      ; BIT 5 = SET => SWITCH ON VELOCITY TONE A
					      ; BIT 6 = SET => SWITCH ON VELOCITY TONE B
					      ; BIT 7 = SET => SWITCH ON VELOCITY TONE C
;; ----------------------------------------------------------------------------------------------
AY_MIDI_VELOCITY_TO_VOLUME			EQU   	0x034 ; => midi_evnt_pipeline.inc
					      ;; Ergebnis der Umrechnung
					      ;; VON MIDI VELOCITY WERT
					      ;; ZUM VOLUME AY REG WERT
					      ;; 0-127 => 0-15
					      ;; => MIDI_EVNT_VALUE/8
;; ----------------------------------------------------------------------------------------------
AY_MIDI_VELOCITY_TO_NOISE_PERIOD		EQU   	0x035 ; => midi_evnt_pipeline.inc
					      ;; Ergebnis der Umrechnung
					      ;; VON MIDI VELOCITY WERT
					      ;; ZUM NOISE RATE AY REG WERT
					      ;; 0-127 => 0-31
					      ;; => MIDI_EVNT_VALUE/4
;; ----------------------------------------------------------------------------------------------
AY_R6_NOISE_PERIOD_REVERSE_VALUE		EQU     0x036 ; => LABEL_AY_MENU_MIX_NOISE_PERIOD, FX Func
;; ----------------------------------------------------------------------------------------------
MIDI_CC_VALUE					EQU	0x037 ;; => midi_controller.inc
;; ----------------------------------------------------------------------------------------------
MIDI_EVENT_CHANGE_FLAG				EQU	0x038 ;; => user_tick_lcd_redraw.inc + CC Handling
;; ----------------------------------------------------------------------------------------------
;; ay_sr.inc - Shift Register Service Routine 
;; MBHP CORE: AY Chip SERIAL COMMUNICATION
;; ----------------------------------------------------------------------------------
AY_REG_ADDRESS					EQU	0x039 	;; => AY CHIP REG ADRESS BYTE
AY_REG_DATA			        	EQU	0x03A 	;; => AY CHIP DATA BYTE
;; ----------------------------------------------------------------------------------

;; see => user_tick_key_repeat.inc
;; -------------------------------------------------------------
C64_KEY_AUTO_REPEAT_GENERAL_COUNTER		EQU	0x03B ;; 
FIRST_KEY_STRIKE_CYCLE_COUNTER			EQU	0x03C ;; 
KEY_AUTO_REPEAT_COUNTER				EQU	0x03D ;; 
;; --------------------------------------------------------------

;; ------------------[PATCH NUMBER (BANKSTICK, PIC EEPROM)]-----------------------------
;;                       PATCH MENU / MAIN MENU 1
CURRENT_PATCH_NR				EQU	0x03E 	;; 0 = INT (PIC EEPROM PATCH)
								;; 1 - 128 (BANK STICK PATCHES)
BROWSE_PATCH_NR					EQU	0x07C	;; (!!! ADR !!!) FOR BROWSING PATCHES
PATCH_MENU_STATE_FLAGS				EQU	0x07D 	;; (!!! ADR !!!) PATCH STATE BYTE 
								;; BIT0 = 1 = BANKSTICK FOUND
								;; BIT0 = 0 = NO BANKSTICK FOUND
								;; BIT1 = 1 = BANKSTICK PATCH FORMATED, OK
								;; BIT1 = 0 = BANKSTICK PATCH NOT FORMATED
								;; BIT2 = 1 = browsed PATCH = INTERNAL (0 [PIC Eeprom])
								;; BIT2 = 0 = browsed PATCH = BANKSTICK (1-128)
								;; BIT3 = 1 = DO FORMAT B.STICK MODE = ON
								;; BIT3 = 0 = DO FORMAT B.STICK MODE = OFF
								;; BIT4 = 1 = CURRENT PATCH NR =  BROWSE PATCH NR
								;; BIT4 = 0 = CURRENT PATCH NR <> BROWSE PATCH NR


FORMAT_BANKSTICK_CYCLE_COUNTER			EQU	0x07E	;; (!!! ADR !!!)
CURRENT_PATCH_DATA_BYTE_TEMP			EQU	0x03F 	;; 
BANKSTICK_LOW_BYTE_ADR_COUNTER			EQU	0x07F 	;; !!! (see above) !!!

;;-------------------------------------[ CC ]-------------------------------------------
;; midi controller
AY_CC_6_TMP					EQU	0x040 ;; => midi_controller.inc
AY_CC_7_TMP					EQU	0x041 ;; => midi_controller.inc
AY_CC_8_TMP					EQU	0x042 ;; => midi_controller.inc
AY_CC_9_TMP					EQU	0x043 ;; => midi_controller.inc

;;---------------------------------------------
;; pitch bend
AY_TONE_A_TUNE_PITCH_BEND			EQU	0x044 	;; => midi_event_pitch_wheel.inc
AY_TONE_B_TUNE_PITCH_BEND			EQU	0x045 	;; => midi_event_pitch_wheel.inc
AY_TONE_C_TUNE_PITCH_BEND			EQU	0x046 	;; => midi_event_pitch_wheel.inc
AY_TONE_A_TUNE_COARSE_MIDI_PITCH_BEND_TMP	EQU 	0x047 	;; => midi_event_pitch_wheel.inc
						      		;; PITCH BEND TEMP FOR
						      		;; AY_TONE_A_TUNE_COASE_MIDI
AY_TONE_B_TUNE_COARSE_MIDI_PITCH_BEND_TMP	EQU 	0x048 
AY_TONE_C_TUNE_COARSE_MIDI_PITCH_BEND_TMP	EQU 	0x049 

AY_TONE_A_TUNE_FINE_MIDI_PITCH_BEND_TMP		EQU 	0x04A 	;; => midi_event_pitch_wheel.inc
						      		;; PITCH BEND TEMP FOR
						     		;; AY_TONE_A_TUNE_FINE_MIDI
AY_TONE_B_TUNE_FINE_MIDI_PITCH_BEND_TMP		EQU 	0x04B
AY_TONE_C_TUNE_FINE_MIDI_PITCH_BEND_TMP		EQU 	0x04C


;;-----------------------------------------------------------
;; Fx REGISTER => MAIN MENU [Fx] = Main Menu 8

FX_USER_ON_OFF_ABC				EQU 	0x04D
						;; ON OFF FX @ TONE A|B|C
						;; BIT 0 => FX ON (SET) @ TONE A
						;; BIT 1 => FX ON (SET) @ TONE B
						;; BIT 2 => FX ON (SET) @ TONE C

Fx_MIDI_NOTE_ON_OR_OFF				EQU 	0x04E
						;; BIT 0 => MIDI NOTE ON (SET) @ TONE A
						;; BIT 1 => MIDI NOTE ON (SET) @ TONE B
						;; BIT 2 => MIDI NOTE ON (SET) @ TONE C
						;; BIT 3 => Reserved
						;; BIT 4 => Reserved
						;; BIT 5 => Reserved
						;; BIT 6 => Reserved
						;; BIT 7 => Reserved

FX_POS_CURRENT					EQU 	0x04F
						;; CURRENT POS OF Fx Table (ROW)
						;; Fx Tab ROW Pointer

FX_FUNCTION_CURRENT				EQU 	0x050
						;; Fx FUNCTIONS
						;; Fx = 0 => DO x
						;; FX = 1 => DO y
						;; FX = 2 => DO z
						;; etc.
						
FX_DATA_CURRENT					EQU 	0x051
						;; USED for WRITING DATA TO AN AY CHIP REG.
						;; Corresponding to FX_FUNCTION_X
						

FX_TIME_PRE_EXECUTION_CURRENT			EQU	0x052
						;; TIME PERIOD TO THE NEXT FX POS.
						;; EXECUTION

FX_TIMER					EQU 	0x053
						;; Triggert (inc +1) by timer0 overflow

FX_MIDI_NOTE_ON_SESSION				EQU	0x054
						;; EVERY NOTE ON @ [A|B|C] GENERATES A NEW
						;; SESSION NUMBER (with inc FX_SESSION) FOR A
						;; NEW FX SESSION. EVERY TIME THERE IS
						;; A NEW FX SESSION NUMBER THE "FX POS"
						;; WILL BE SET TO 1 (A RESET OF FX POS WILL TAKE PLACE)

FX_PIPELINE_SESSION				EQU	0x055	

FX_NOP_ON_OFF					EQU	0x056
						;; = 0 = Fx NOP IS OF (FX OPERATION IS ON)
						;; > 0 = Fx NOP IS ON (FX OPERATION IS OFF)

FX_FILTER					EQU	0x057

Fx_TEMP						EQU	0x058

Fx_Note_On_1_tune_fine				EQU	0x059
Fx_Note_On_1_tune_coarse			EQU	0x05A

Fx_Note_On_2_tune_fine				EQU	0x05B
Fx_Note_On_2_tune_coarse			EQU	0x05C

Fx_Note_On_3_tune_fine				EQU	0x05D
Fx_Note_On_3_tune_coarse			EQU	0x05E
Fx_PATCH_DATA_TMP				EQU	0x05F
Fx_LOAD_WRITE_ADR_COUNTER_TMP			EQU	0x060

FX_POS_SESSION_CURRENT				EQU 	0x061
FX_FUNCTION_SESSION_CURRENT			EQU	0x062
FX_TIME_PRE_EXECUTION_SESSION_CURRENT		EQU	0x063
FX_DATA_SESSION_CURRENT				EQU	0x07B ;; [(!!ADR!!!)]


;; ===========================================
;; -------[AY-3-8912 SHADOW REGISTERS]-------
;; ==========================================
;; NOTE (!): 
;; IN SOME DATASHEETS OF THE AY 3 8912/8910  
;; THE AY REGISTERS ARE *OCTAL* 
AY_R0_CH_A_FINE_TUNE			EQU	0x064 ;; 0x00 to 0xFF
AY_R1_CH_A_COARSE_TUNE			EQU	0x065 ;; 0x00 to 0x0F
AY_R2_CH_B_FINE_TUNE			EQU	0x066 ;; 0x00 to 0xFF
AY_R3_CH_B_COARSE_TUNE			EQU	0x067 ;; 0x00 to 0x0F
AY_R4_CH_C_FINE_TUNE			EQU	0x068 ;; 0x00 to 0xFF
AY_R5_CH_C_COARSE_TUNE			EQU	0x069 ;; 0x00 to 0x0F
AY_R6_NOISE_PERIOD			EQU	0x06a ;; 0x00 to 0x1F
AY_R7_ENABLE_ABC_NOISE_IO		EQU	0x06b ;; 0x00 to 0xFF
  ;; ----------------[AY CHIP REG 0x07: ASSIGNMENT]-----------------
  ;; >>> | BIT7  | BIT6  | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 |
  ;; -------------------------------------------------------------
  ;; >>> | MODE  | MODE  | NOISE| NOISE| NOISE| TONE | TONE | TONE |
  ;; >>> | IOB   | IOA   | CH C | CH B | CH A | CH C | CH B | CH A |
  ;; >>> | /IN   | /IN   | /ON  | /ON  | /ON  | /ON  | /ON  | /ON  |
  ;; >>> | OUT   | OUT   | OFF  | OFF  | OFF  | OFF  | OFF  | OFF  |
  ;; ---------------------------------------------------------------
  ;; HEX.| 0x80  | 0x40  | 0x20 | 0x10 | 0x08 | 0x04 | 0x02 | 0x01 |
  ;; ---------------------------------------------------------------
  ;; DEC.| D128  | D064  | D032 | D016 | D008 | D004 | D002 | D001 |
  ;; ---------------------------------------------------------------

AY_R8_CH_A_AMPLITUDE		  	EQU	0x06c ; 0x00 to 0x1F (D031) 
  ;; THE AMPLITUDE (VOLUME) OF THE CH. A SIGNAL GENERATED BY 
  ;; THE D/A CONVERTER IS DETERMINED BY THE CONTENS OF THE LOWER BITS OF REGISTER 8
  ;;
  ;; >>> | BIT7 | BIT6 |BIT5|   BIT4   | BIT3 | BIT2 | BIT1 |  BIT0 |
  ;; ----------------------------------------------------------------
  ;; >>> | ---- | ---- |----|    M     | L3   | L2   | L1   | L0    |
  ;; >>> | ---- | ---- |----|AMPLITUDE | VOL. | VOL. | VOL. | VOLUME|
  ;; >>> | ---- | ---- |----|MODE=>R13 | BIT 3| BIT 2| BIT 1| BIT 0 |
  ;; ----------------------------------------------------------------
  ;; HEX.| ---- | ---- |----|   0x10   | 0x08 | 0x04 | 0x02 | 0x01  |
  ;; ----------------------------------------------------------------
  ;; DEC.| ---- | ---- |----|   D016   | D008 | D004 | D002 | D001  |
  ;; ----------------------------------------------------------------
  ;; VOLUME => 0x00 to 0x0F (DEC. 000 to 015)
  ;; IF BIT4 IS SET => THE ENVELOPE SHAPE/CYCLE IS DEFINED BY REGISTER 13
  ;; (OCTAL R15 (!)) SEE DATASHEET FOR DETAILS

AY_R9_CH_B_AMPLITUDE		  	EQU	0x06d ; 0x00 to 0x1F (D031) 
  ;; THE AMPLITUDE OF THE SIGNAS (CH. B) GENERATED BY 
  ;; THE D/A CONVERTER (ONE EACH FOR CHANNELS A, B, C) IS DETERMINED
  ;; BY THE CONTENS OF THE LOWER BITS OF REGISTER 8,9 AND 10 
  ;; (REGISTER *OCTAL* 10,11 AND 12(ACCORDING TO SOME PDF DATASHEETS)): 
  ;;
  ;; >>> | BIT7 | BIT6 |BIT5|   BIT4   | BIT3 | BIT2 | BIT1 |  BIT0 |
  ;; ----------------------------------------------------------------
  ;; >>> | ---- | ---- |----|    M     | L3   | L2   | L1   | L0    |
  ;; >>> | ---- | ---- |----|AMPLITUDE | VOL. | VOL. | VOL. | VOLUME|
  ;; >>> | ---- | ---- |----|MODE=>R13 | BIT 3| BIT 2| BIT 1| BIT 0 |
  ;; ----------------------------------------------------------------
  ;; HEX.| ---- | ---- |----|   0x10   | 0x08 | 0x04 | 0x02 | 0x01  |
  ;; ----------------------------------------------------------------
  ;; DEC.| ---- | ---- |----|   D016   | D008 | D004 | D002 | D001  |
  ;; ----------------------------------------------------------------
  ;; VOLUME => 0x00 to 0x0F (DEC. 000 to 015)
  ;; IF BIT4 IS SET => THE ENVELOPE SHAPE/CYCLE IS DEFINED BY REGISTER 13
  ;; (OCTAL R15 (!)) SEE DATASHEET FOR DETAILS

AY_R10_CH_C_AMPLITUDE		  	EQU	0x06E ; 0x00 to 0x1F (D031) 
  ;; THE AMPLITUDE OF THE SIGNAL (CH. C) GENERATED BY EACH
  ;; OF THE 3 D/A CONVERTERS (ONE EACH FOR CHANNELS A, B, C) IS DETERMINED
  ;; BY THE CONTENS OF THE LOWER BITS OF REGISTER 8,9 AND 10 
  ;; (REGISTER *OCTAL* 10,11 AND 12 (ACCORDING TO SOME PDF DATASHEETS)): 
  ;;
  ;; >>> | BIT7 | BIT6 |BIT5|   BIT4   | BIT3 | BIT2 | BIT1 |  BIT0 |
  ;; ----------------------------------------------------------------
  ;; >>> | ---- | ---- |----|    M     | L3   | L2   | L1   | L0    |
  ;; >>> | ---- | ---- |----|AMPLITUDE | VOL. | VOL. | VOL. | VOLUME|
  ;; >>> | ---- | ---- |----|MODE=>R13 | BIT 3| BIT 2| BIT 1| BIT 0 |
  ;; ----------------------------------------------------------------
  ;; HEX.| ---- | ---- |----|   0x10   | 0x08 | 0x04 | 0x02 | 0x01  |
  ;; ----------------------------------------------------------------
  ;; DEC.| ---- | ---- |----|   D016   | D008 | D004 | D002 | D001  |
  ;; ----------------------------------------------------------------
  ;; VOLUME => 0x00 to 0x0F (DEC. 000 to 015)
  ;; IF BIT4 IS SET => THE ENVELOPE SHAPE/CYCLE IS DEFINED BY REGISTER 13
  ;; (OCTAL R15 (!)) SEE DATASHEET FOR DETAILS

AY_R11_ENV_PERIOD_FINE			EQU	0x06f ; (RANGE 0x00 to 0xFF) | OCTAL R13 (!)
AY_R12_ENV_PERIOD_COARSE		EQU	0x070 ; (RANGE 0x00 to 0xFF) | OCTAL R14 (!)
  ;; THE FREQUENCY OF THE ENVELOPE IS OPTAINED BY FIRST COUTING DOWN
  ;; THE INPUT CLOCK BY 256, THEN BY FURTHER COUNTING DOWN THE RESULT
  ;; BY THE PROGRAMMED 16BIT (SEE BIT 0-15) ENVELOPE PERIOD VALUE
  ;; R11 => BIT 0-7
  ;; R12 => BIT 8-15

AY_R13_ENV_SHAPE_CYCLE			EQU	0x071 ; 0x00 to 0x0F (D015) | OCTAL R15 (!)
  ;; THE RELATIVE SHAPE AND CYCLE PATTERN REGISTER
  ;; CH A,B,C ENVELOVE REGISTER IF "AMPLITUDE MODE" *BIT 4*
  ;; @ REGISTER 10 or 11 or 12 (OCTAL 12,13,14) is SET
  ;; FOR DETAILS OF THE GRAPHIC REPRESENTATION SEE AY-3-8912 DATASHEET
  ;;
  ;; >>> | BIT7 | BIT6 |BIT5 | BIT4 | BIT3 | BIT2 | BIT1 |  BIT0 |
  ;; -------------------------------------------------------------
  ;; >>> | ---- | ---- |---- | ---- | ENV. | ENV. | ENV. |  ENV. |
  ;; >>> | ---- | ---- |---- | ---- |  B3  |  B2  |  B1  |  B0   |
  ;; >>> | ---- | ---- |---- | ---- |CONTI-|ATTACK|ALTER-|  B0   |
  ;; >>> | ---- | ---- |---- | ---- |  NUE |      | NATE | HOLD  |
  ;; -------------------------------------------------------------
  ;; HEX.| ---- | ---- |---- | ---- | 0x08 | 0x04 | 0x02 | 0x01  |
  ;; -------------------------------------------------------------
  ;; DEC.| ---- | ---- |---- | ---- | D008 | D004 | D002 | D001  |
  ;; -------------------------------------------------------------


AY_R14_AY_PORTA_DATA			EQU	0x072 ;; OCTAL R16 (!)
  ;; NOTE: R15 = I/O PORT B (NOT IMPLEMENTED IN AY 3 8912)


;; APPLICATION WORKING REGISTERS 
;; =============================================
  
SWITCH_BETWEEN_MAIN_AND_SUBMENU		EQU	0x073 	;; => MAIN - SUB MENU SWITCH 

CURRENT_MAIN_MENU  			EQU	0x074 	;; => CURRENT main menu (INDICATES WHITCH MAIN MENU IS ACTIVE)
						      	;; SWITCHED @ keyb_decoding.inc
						      	;; SEE LABEL_AY_DECODE_KEYBOARD_BUTTON_MAINMENU

AY_SUB_MENUE_KEY_SWITCH			EQU	0x075 	;; => C64 KEYBOARD KEY (VALUE) - JUMP-SWITCH to sub menu (x)

SM_DRIVER_KEYBOARD_KEY			EQU	0x076	;; @ keyb_decoding.inc [C64 Keyboard KEY value]
							;; from the 8x8 scan matrix driver

SM_KEYB_KEY_STATE			EQU	0x077 	;; => 0 = PRESSED
							;; => 1 = DEPRESSED
							;; @ sm_simple_c64keyb.inc
							;; =>SM_NotifyToggle
							;; + many subfunctions
							
SM_KEYB_KEY_GATE			EQU	0x078	;; sm_simple_c64keyb.inc
							;; =>SM_NotifyToggle


AY_KEYB_SPECIAL_KEY_TOGGLE		EQU	0x079 	;; FOR SPECIAL KEY HANDLING
							;; ------------------------
							;; BIT0 = SHIFT KEY:
							;; SET  = KEY PRESSED
							;; CLR  = KEY DEPRESSED
							;; BIT0 = SHIFT
							;; --------
							;; BIT 1 @ constant_key_pressing.inc
							;; => ONE LOOP DELAY BIT
							;; --------
							;; BIT 2 - 7 NC YET

;; -----------------------------------------------------------------------------------------
;; Envelope Shape Pointer (Env. Shape Help Reg.)
ENV_SHAPE_0_15_to_1_8_TRANSLATOR		EQU	0x07A
;; FX_DATA_SESSION_CURRENT			EQU	0x07B ;; !!! (see above) !!!

;; -----------------------------------------------------------------------------------------
;; BROWSE_PATCH_NR				EQU	0x07C ;; !!! (see above) !!!
;; PATCH_MENU_STATE_FLAGS			EQU	0x07D ;; !!! (see above) !!!
;; FORMAT_BANKSTICK_CYCLE_COUNTER		EQU	0x07E ;; !!! (see above) !!!
;; BANKSTICK_LOW_BYTE_ADR_COUNTER		EQU	0x07F ;; !!! (see above) !!!

;; =========================================================================
;; ======================[END: ACCESS RAM AREA]===========================
;; =========================================================================



;; ---[!SFR!]----------------------------------------------------------
;; DO NOT USE 0x080 - 0x0ff => SFR's
;; ---[!SFR!]----------------------------------------------------------


;; =========================================================================
;; ======================[START: RAM BANK1]=================================
;; =========================================================================
;; ----------------------------------[BSR1]---------------------------------
;; -------------------------------------------------------------------------

;; ---------[SM DRIVER]-------------------
;; [0x100 - 0x10f in use by the sm driver]
SM_ROW0_VALUE		EQU	0x100
SM_ROW1_VALUE		EQU	0x101
SM_ROW2_VALUE		EQU	0x102
SM_ROW3_VALUE		EQU	0x103
SM_ROW4_VALUE		EQU	0x104
SM_ROW5_VALUE		EQU	0x105
SM_ROW6_VALUE		EQU	0x106
SM_ROW7_VALUE		EQU	0x107

SM_ROW0_CHANGED		EQU	0x108
SM_ROW1_CHANGED		EQU	0x109
SM_ROW2_CHANGED		EQU	0x10a
SM_ROW3_CHANGED		EQU	0x10b
SM_ROW4_CHANGED		EQU	0x10c
SM_ROW5_CHANGED		EQU	0x10d
SM_ROW6_CHANGED		EQU	0x10e
SM_ROW7_CHANGED		EQU	0x10f

;; ---------------------------------------
;; FREE: 0x10F - 0x1FF
;; ---------------------------------------



;; =========================================================================
;; ======================[START: RAM BANK2]=================================
;; ----------------------------[BSR2]---------------------------------------
;; AREA: 0x200 - 0x2ff
;; -------------------------------------------------------------------------
;; EDITING MODE POINTER IN MAIN MENU - print "*" or ":" @ POS X on LCD 
AY_LCD_COURSER_POS_MAIN_MENU_PATCH_INFO		EQU 	0x200	;; used by main - and /submenu functions
AY_LCD_COURSER_POS_MAIN_MENU_ABC_TUNE		EQU 	0x201	;; used by main - and /submenu functions
AY_LCD_COURSER_POS_MAIN_MENU_ABC_NOISE		EQU 	0x202	;; used by main - and /submenu functions
AY_LCD_COURSER_POS_MAIN_MENU_AMPLITUDE		EQU 	0x203	;; used by main - and /submenu functions
AY_LCD_COURSER_POS_MAIN_MENU_ENVELOPE		EQU 	0x204	;; used by main - and /submenu functions
AY_LCD_COURSER_POS_MAIN_MENU_MIDI_CONFIG	EQU	0x205	;; used by main - and /submenu functions
AY_LCD_COURSER_POS_MAIN_MENU_VELOCITY_CONFIG	EQU	0x206	;; used by main - and /submenu functions
AY_LCD_COURSER_POS_MAIN_MENU_FX			EQU	0x207	;; used by main - and /submenu functions

;; -----------------[Fx Function and Function data Registers]----------------
FX_FUNCTION_POS0				EQU	0x220
FX_FUNCTION_POS1				EQU	0x221
FX_FUNCTION_POS2				EQU	0x222
FX_FUNCTION_POS3				EQU	0x223
FX_FUNCTION_POS4				EQU	0x224
FX_FUNCTION_POS5				EQU	0x225
FX_FUNCTION_POS6				EQU	0x226
FX_FUNCTION_POS7				EQU	0x227
FX_FUNCTION_POS8				EQU	0x228
FX_FUNCTION_POS9				EQU	0x229
FX_FUNCTION_POS10				EQU	0x22A
FX_FUNCTION_POS11				EQU	0x22B
FX_FUNCTION_POS12				EQU	0x22C
FX_FUNCTION_POS13				EQU	0x22D
FX_FUNCTION_POS14				EQU	0x22E
FX_FUNCTION_POS15				EQU	0x22F
FX_FUNCTION_POS16				EQU	0x230
FX_FUNCTION_POS17				EQU	0x231
FX_FUNCTION_POS18				EQU	0x232
FX_FUNCTION_POS19				EQU	0x233
FX_FUNCTION_POS20				EQU	0x234
FX_FUNCTION_POS21				EQU	0x235
FX_FUNCTION_POS22				EQU	0x236
FX_FUNCTION_POS23				EQU	0x237
FX_FUNCTION_POS24				EQU	0x238
FX_FUNCTION_POS25				EQU	0x239
FX_FUNCTION_POS26				EQU	0x23A
FX_FUNCTION_POS27				EQU	0x23B
FX_FUNCTION_POS28				EQU	0x23C
FX_FUNCTION_POS29				EQU	0x23D
FX_FUNCTION_POS30				EQU	0x23E
FX_FUNCTION_POS31				EQU	0x23F

FX_FUNCTION_DATA_POS0				EQU	0x240
FX_FUNCTION_DATA_POS1				EQU	0x241
FX_FUNCTION_DATA_POS2				EQU	0x242
FX_FUNCTION_DATA_POS3				EQU	0x243
FX_FUNCTION_DATA_POS4				EQU	0x244
FX_FUNCTION_DATA_POS5				EQU	0x245
FX_FUNCTION_DATA_POS6				EQU	0x246
FX_FUNCTION_DATA_POS7				EQU	0x247
FX_FUNCTION_DATA_POS8				EQU	0x248
FX_FUNCTION_DATA_POS9				EQU	0x249
FX_FUNCTION_DATA_POS10				EQU	0x24A
FX_FUNCTION_DATA_POS11				EQU	0x24B
FX_FUNCTION_DATA_POS12				EQU	0x24C
FX_FUNCTION_DATA_POS13				EQU	0x24D
FX_FUNCTION_DATA_POS14				EQU	0x24E
FX_FUNCTION_DATA_POS15				EQU	0x24F
FX_FUNCTION_DATA_POS16				EQU	0x250
FX_FUNCTION_DATA_POS17				EQU	0x251
FX_FUNCTION_DATA_POS18				EQU	0x252
FX_FUNCTION_DATA_POS19				EQU	0x253
FX_FUNCTION_DATA_POS20				EQU	0x254
FX_FUNCTION_DATA_POS21				EQU	0x255
FX_FUNCTION_DATA_POS22				EQU	0x256
FX_FUNCTION_DATA_POS23				EQU	0x257
FX_FUNCTION_DATA_POS24				EQU	0x258
FX_FUNCTION_DATA_POS25				EQU	0x259
FX_FUNCTION_DATA_POS26				EQU	0x25A
FX_FUNCTION_DATA_POS27				EQU	0x25B
FX_FUNCTION_DATA_POS28				EQU	0x25C
FX_FUNCTION_DATA_POS29				EQU	0x25D
FX_FUNCTION_DATA_POS30				EQU	0x25E
FX_FUNCTION_DATA_POS31				EQU	0x25F

FX_FUNCTION_TIME_POS0				EQU	0x260
FX_FUNCTION_TIME_POS1				EQU	0x261
FX_FUNCTION_TIME_POS2				EQU	0x262
FX_FUNCTION_TIME_POS3				EQU	0x263
FX_FUNCTION_TIME_POS4				EQU	0x264
FX_FUNCTION_TIME_POS5				EQU	0x265
FX_FUNCTION_TIME_POS6				EQU	0x266
FX_FUNCTION_TIME_POS7				EQU	0x267
FX_FUNCTION_TIME_POS8				EQU	0x268
FX_FUNCTION_TIME_POS9				EQU	0x269
FX_FUNCTION_TIME_POS10				EQU	0x26A
FX_FUNCTION_TIME_POS11				EQU	0x26B
FX_FUNCTION_TIME_POS12				EQU	0x26C
FX_FUNCTION_TIME_POS13				EQU	0x26D
FX_FUNCTION_TIME_POS14				EQU	0x26E
FX_FUNCTION_TIME_POS15				EQU	0x26F
FX_FUNCTION_TIME_POS16				EQU	0x270
FX_FUNCTION_TIME_POS17				EQU	0x271
FX_FUNCTION_TIME_POS18				EQU	0x272
FX_FUNCTION_TIME_POS19				EQU	0x273
FX_FUNCTION_TIME_POS20				EQU	0x274
FX_FUNCTION_TIME_POS21				EQU	0x275
FX_FUNCTION_TIME_POS22				EQU	0x276
FX_FUNCTION_TIME_POS23				EQU	0x277
FX_FUNCTION_TIME_POS24				EQU	0x278
FX_FUNCTION_TIME_POS25				EQU	0x279
FX_FUNCTION_TIME_POS26				EQU	0x27A
FX_FUNCTION_TIME_POS27				EQU	0x27B
FX_FUNCTION_TIME_POS28				EQU	0x27C
FX_FUNCTION_TIME_POS29				EQU	0x27D
FX_FUNCTION_TIME_POS30				EQU	0x27E
FX_FUNCTION_TIME_POS31				EQU	0x27F


;; =========================================================================
;; =============================[START: RAM BANK3]==========================
;; ---------------------------------[BSR3]----------------------------------
;; internal eeprom patch
PATCH_NAME_ASC_KEY				EQU	0x300	;; SAVED CURRENT ASC VALUE
PATCH_NAME_ASC_POS				EQU	0x301	;; SAVED CURRENT CURSOR POS PATCH NAME
FREE_RAM_0x302					EQU	0x302

PATCH_NAME_ASC_0				EQU	0x303	;; CURRENT PATCH NAME
PATCH_NAME_ASC_1				EQU	0x304	;; CURRENT PATCH NAME
PATCH_NAME_ASC_2				EQU	0x305	;; CURRENT PATCH NAME
PATCH_NAME_ASC_3				EQU	0x306	;; CURRENT PATCH NAME
PATCH_NAME_ASC_4				EQU	0x307	;; CURRENT PATCH NAME
PATCH_NAME_ASC_5				EQU	0x308	;; CURRENT PATCH NAME
PATCH_NAME_ASC_6				EQU	0x309	;; CURRENT PATCH NAME
PATCH_NAME_ASC_7				EQU	0x30A	;; CURRENT PATCH NAME

FREE_RAM_0x30B					EQU	0x30B
FREE_RAM_0x30C					EQU	0x30C
FREE_RAM_0x30D					EQU	0x30D
FREE_RAM_0x30E					EQU	0x30E
FREE_RAM_0x30F					EQU	0x30F

;; RESERVED / NOT IN USE (YET)
BROWSED_PATCH_NAME_ASC_0			EQU	0x310	;; BROWSEND PATCH NAME
BROWSED_PATCH_NAME_ASC_1			EQU	0x311	;; BROWSEND PATCH NAME
BROWSED_PATCH_NAME_ASC_2			EQU	0x312	;; BROWSEND PATCH NAME
BROWSED_PATCH_NAME_ASC_3			EQU	0x313	;; BROWSEND PATCH NAME
BROWSED_PATCH_NAME_ASC_4			EQU	0x314	;; BROWSEND PATCH NAME
BROWSED_PATCH_NAME_ASC_5			EQU	0x315	;; BROWSEND PATCH NAME
BROWSED_PATCH_NAME_ASC_6			EQU	0x316	;; BROWSEND PATCH NAME
BROWSED_PATCH_NAME_ASC_7			EQU	0x317	;; BROWSEND PATCH NAME




;; -------------------------------------------------------------------------
;; BSR3 / RAM_BANK3
;; 0x300 - 0x37F [0x380 - 0x3FF is reserved for MIOS!] 
;; -------------------------------------------------------------------------

;; ---------------------------[RAM END]--------------------------------------
;; --------------------------------------------------------------------------

;; ----------------------------------------------------------
;; ##########################################################
;; # MIDIBox AY APPLICATION CONST DEF                       #
;; ##########################################################

;; POS0 ROW 1 to 4 - @ LCD (4x16) 
#define LCD_ROW1 					0x00 	; Pos.0 @ LCD Row1
#define LCD_ROW2 					0x40	; Pos.0 @ LCD Row2 
#define LCD_ROW3 					0x10	; Pos.0 @ LCD Row3 
#define LCD_ROW4 					0x50	; Pos.0 @ LCD Row4

;; AY CHIP SHADOW REGISTERS CONST DEF
;; ------------------------
#define AY_CHIP_ADR_00_TONE_A_FREQUENCY_FINE	 	0x00
#define AY_CHIP_ADR_01_TONE_A_FREQUENCY_PERIOD_COARSE 	0x01
#define AY_CHIP_ADR_02_TONE_B_FREQUENCY_PERIOD_FINE	0x02
#define AY_CHIP_ADR_03_TONE_B_FREQUENCY_PERIOD_COARSE 	0x03
#define AY_CHIP_ADR_04_TONE_C_FREQUENCY_PERIOD_FINE	0x04
#define AY_CHIP_ADR_05_TONE_C_TONE_PERIOD_COARSE	0x05
#define AY_CHIP_ADR_06_NOISE_PERIOD			0x06
#define AY_CHIP_ADR_07_TONE_NOISE_ON_OFF		0x07
#define AY_CHIP_ADR_08_TONE_A_VOLUME_ENV_MODE_ON_OFF	0x08
#define AY_CHIP_ADR_09_TONE_B_VOLUME_ENV_MODE_ON_OFF	0x09
#define AY_CHIP_ADR_10_TONE_C_VOLUME_ENV_MODE_ON_OFF	0x0A
#define AY_CHIP_ADR_11_GLOBAL_ENVELOPE_FINE		0x0B
#define AY_CHIP_ADR_12_GLOBAL_ENVELOPE_COARSE		0x0C
#define AY_CHIP_ADR_13_ENVELOPE_SHAPE_AND_CYCLE		0x0D
#define AY_CHIP_ADR_14_IO_PORT_A_DATA			0x0E
#define AY_CHIP_ADR_15_IO_PORT_B_DATA			0x0F


;; MAIN MENU CONST.
;; -----------------
#define	MAIN_MENU_ON_VIA_0	0x00	;; GO MAIN MENU ROUTINE FIRST (DO NOT CHANGE!)
#define INI_START_UP_MAIN_MENU	0x00	;; SHOW MAIN MENU 0 after Startup
#define MAIN_MENU_00		0x00	;; MAIN MENU SWITCH
#define MAIN_MENU_01		0x01	;; PATCH LOAD / WRITE
#define MAIN_MENU_02		0x02	;; ABC (DE) TUNE
#define MAIN_MENU_03		0x03	;; TONE/NOISE/FX ABC - MIXER
#define MAIN_MENU_04		0x04	;; TONE ABC AMPLITUDE
#define MAIN_MENU_05		0x05	;; ENVELOPE
#define MAIN_MENU_06		0x06	;; MIDI CHANNEL CONFIG
#define MAIN_MENU_07		0x07	;; VELOCITY CONFIG
#define MAIN_MENU_08		0x08	;; Fx EDITOR


;; SUB MENU (MAIN MENU SUB FUNCTIONS) CONST DEF
#define INI_SUB_MENU 		0x00	;; (Do not Change)


;; MENU POS - ACTIVE ITEM CONST DEF
;; --------------------------------
#define MENU_ITEM_POS0_NOT_VALID	0x00
#define MENU_ITEM_POS1			0x01
#define MENU_ITEM_POS2			0x02
#define MENU_ITEM_POS3			0x03
#define MENU_ITEM_POS4			0x04
#define MENU_ITEM_POS5			0x05
#define MENU_ITEM_POS6			0x06


;; COURSER POS CONST DEF
#define INI_COURSER_POS		0x01 	;; (Do not Change, POS 0 is NOT VALID!)


;; :::::::::::::: C64 KYBOARD :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;; JUSTAGE FOR THE AUTOREPEAT-C64-KEYBOAD FUNCTION @ [+]/[-]
;; @ user_tick_key_repeat.inc
;; RANGE: 0 = Fast / 255 = Slow
#define C64_KEY_AUTO_REPEAT_GENERAL_TIME		0x5F	;; ADJUST general AUTO KEY REPEAT TEMPO HERE
#define FIRST_KEY_STRIKE_CYCLE_TIME			0x8F	;; ADJUST the "first key strike" delay time here
#define KEY_AUTO_REPEAT_TIME				0x0F	;; ADJUST the AUTO REPEAT TIME here

;; KEYBOARD KEYS @ SUB MENU ROUTINES CONST DEF
#define PLUS_SUBMENU_KEY_VALUE				0x03	;; see also keyb_decoding.inc
#define MINUS_SUBMENU_KEY_VALUE				0x04	;; see also keyb_decoding.inc

;; KEY STATES CONST DEF
#define KEY_PRESSED_STATE_0				0x00
#define FIRST_KEY_STRIKE_TRUE_OR_FALSE_BIT1		1	;; user_tick_key_repeat.inc


;; -------------- PATCH -------------------------------------
;;          EEPROM / BANKSTICK
;; ----------------------------------------------------------
#define EEPROM_ADR_FINGERPRINT				0x00	;; ADRESS FINGERPRINT @ INT PATCH (EEPROM)
#define BANKSTICK_ADR_FINGERPRINT			0x00	;; ADRESS FINGERPRINT @ INT PATCH (BANKSTICK)
#define PIC_INT_EEPROM_FINGERPRINT			0x12	;; Fingerprint Patch PIC EEPROM (Fingerprint = Formatted)
#define BANKSTICK_PATCH_FINGERPRINT			0x88	;; Fingerprint Patch BANKSTICK (Fingerprint = Formatted)
;; ----------------------------------------------------------
;; ==> For PATCH_MENU_STATE_FLAGS	
;; 	;; BIT0 = 1 = BANKSTICK 1 FOUND
;; 	;; BIT0 = 0 = NO BANKSTICK 1 FOUND
;; 	;; BIT1 = 1 = BANKSTICK 1 CURRENT PATCH FORMATED, OK
;; 	;; BIT1 = 0 = BANKSTICK 1 CURRENT PATCH NOT FORMATED
;; 	;; BIT2 = 1 = CURRENT PATCH = BANKSTICK (1-128)
;; 	;; BIT2 = 0 = CURRENT PATCH = INTERNAL (PIC EEPROM)
;;	;; BIT3 = 1 = DO FORMAT B.STICK MODE = ON
;; 	;; BIT3 = 0 = DO FORMAT B.STICK MODE = OFF
;;	;; BIT4 = 1 = CURRENT PATCH NR =  BROWSE PATCH NR
;;	;; BIT4 = 0 = CURRENT PATCH NR <> BROWSE PATCH NR
;;	;; (BIT5-7 NOT USED YET)
#define BIT0_BANKSTICK_FOUND				0
#define BIT1_BANKSTICK_SELECTED_PATCH_FORMATED		1
#define BIT2_SELECTED_PATCH_BANKSTICK_OR_INTERNAL	2
#define BIT3_FORMAT_BANKSTICK_MODE_ON_OR_OFF		3
#define BIT4_A_PATCH_HAS_BEEN_LOADED			4
#define BIT5_PATCH_NAME_IS_NEW				5
;; ------------------------------------------------------------
#define BANKSTICK_MAX_PATCH_NUMBER			127

;; ------------------------------------------------------------
;;          Patch - CONST DEF
;; ------------------------------------------------------------
;; ==> patch_name_writing.inc
;; CHECK PATCH_NAME_ASC_KEY > 0x2F ([0x30 = "0" in ASC CODE]) ?
#define PATCH_NAME_ASC_CODE_0_MINUS1			0x2F	
;; CHECK IF PATCH_NAME_ASC_KEY IS < 0x3A [0x39 = "9" (ASC)] ?
#define PATCH_NAME_ASC_CODE_9_PLUS1			0x3A	
;; CHECK IF PATCH_NAME_ASC_KEY IS > 0x40 [0x41 = "A" (ASC)] ?
#define PATCH_NAME_ASC_CODE_A_MINUS1			0x40
;; CHECK IF PATCH_NAME_ASC_KEY IS < 0x5B [0x5A = "Z" (ASC)] ?
#define PATCH_NAME_ASC_CODE_Z_PLUS1			0x5B
;; CHECK IF PATCH_NAME_ASC_KEY IS == 0x5F [SPACE KEY "_" for LCD] ?
#define PATCH_NAME_ASC_CODE_SPACE_KEY			0x5F	
#define PATCH_NAME_ASC_CODE_DEL_KEY			0x14	
#define PATCH_NAME_CURSOR_RIGHT_KEY			0x1D 	
#define PATCH_NAME_CURSOR_LEFT_KEY			0x9D	
#define PATCH_NAME_CURSOR_POS_MAX			0x07	
#define PATCH_NUMBER_VALUE_MAX				0x7F	



;; :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

;; TONE ABC ON/OFF
;; NOISE ABC ON/OFF
;; -----------------------------
#define BIT0_TONE_A_ON_OFF	0
#define BIT1_TONE_B_ON_OFF	1
#define BIT2_TONE_C_ON_OFF	2
#define BIT3_NOISE_A_ON_OFF	3
#define BIT4_NOISE_B_ON_OFF	4
#define BIT5_NOISE_C_ON_OFF	5



;; VELOCITY BIT - VELOCITY @ TONE ABC / VELOCITY @ NOISE | CONST DEF
;; ---------------------------------------------------------------------------------------------------------------------
;; MEMO
;; VELOCITY => NOISE PERIOD:
;; VELOCITY 	      => NOISE PERID is OFF	==> AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF: BIT3=0, BIT5=0,BIT6=0,BIT7=0
;; VELOCITY of TONE A => NOISE PERIOD 		==> AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF: BIT3=1, BIT5=1,BIT6=0,BIT7=0 
;; VELOCITY of TONE B => NOISE PERIOD 		==> AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF: BIT3=1, BIT5=0,BIT6=1,BIT7=0 
;; VELOCITY of TONE C => NOISE PERIOD 		==> AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF: BIT3=1, BIT5=0,BIT6=0,BIT7=1 
;; ----------------------------------------------------------------------------------------------------------------------
#define BIT0_VELOCITY_ON_OFF_TONE_A 	0
#define BIT1_VELOCITY_ON_OFF_TONE_B 	1
#define BIT2_VELOCITY_ON_OFF_TONE_C 	2
#define BIT3_VELOCITY_TO_NOISE_PERIOD_ON_OFF 	3
#define BIT5_VELOCITY_A_TO_NOISE_PERIOD_ON_OFF	5
#define BIT6_VELOCITY_B_TO_NOISE_PERIOD_ON_OFF	6
#define BIT7_VELOCITY_C_TO_NOISE_PERIOD_ON_OFF	7
;; VELOCITY BIT TONE / NOISE ABC
;; ----------------------------------------------------------------------
#define BIT5_NOISE_PERIOD_CONTROLED_VIA_VELOCITY_TONE_A_ON_OFF 		5
#define BIT6_NOISE_PERIOD_CONTROLED_VIA_VELOCITY_TONE_B_ON_OFF 		6
#define BIT7_NOISE_PERIOD_CONTROLED_VIA_VELOCITY_TONE_C_ON_OFF 		7

;; ENVELOPE MODE ON / OFF (BIT4 @ TONE ABC)
;; ----------------------------------------
#define BIT4_ENVELOPE_ON_OFF		4
#define ENVELOPE_SHAPE_MAX_VALUE_8	0x08

;; FX ON/OFF @ TONE ABC
;; -----------------------------
#define BIT0_FX_ON_OFF_TONE_A	0
#define BIT1_FX_ON_OFF_TONE_B	1
#define BIT2_FX_ON_OFF_TONE_C	2


;; USER DETUNE INI
#define INI_TUNE_FINE_USER		0x80; INITIAL VALUE = 128 (0 on LCD!)
#define INI_TUNE_COARSE_USER		0x07 ; INITIAL VALUE = 7  (0 on LCD!)

;; DETUNE ABC
;; ----------
#define PROOF_POSITIVE_OR_NEGATIVE_VIA_0X06	0x06
#define PROOF_POSITIVE_OR_NEGATIVE_VIA_0X7F 	0x7F
#define OFFSET_COARSE_TUNE_IS_7			0x07
#define OFFSET_FINE_TUNE_IS_128 		0x80
#define PLUS_1_CARRY_BIT_KILLER			0x01

;; TUNE @ TONE ABC
;; MIN / MAX VALUE
;; ---------------
;; LOWEST TUNE - MAX VALUE
#define COARSE_TUNE_MAX_VALUE_B00001111		0x0F	;; dec 15, hex 0x0F, B00001111
#define COARSE_TUNE_MAX_MINUS_1_B00001110	0x0E 	;; dec 14, hex 0x0E, B00001110
#define FINE_TUNE_MAX_VALUE_B11010010		0xD2 	;; dec 210, hex 0xD2, B11010010
;; HIGHEST TUNE - MIN VALUE
#define COARSE_TUNE_MIN_VALUE_B00000000		0x00 ;; dec 0, hex 0x00, B00000000
#define FINE_TUNE_MIN_VALUE_B00001010		0x0A ;; dec 10, hex 0x0A, B00001010
;; MAX VALUE - NOISE PERIOD
#define NOISE_PERIOD_MAX_VALUE			0x1F ;; dec 31; bin 0001 1111
;; MAX VOLUME TONE ABC
#define VOLUME_ABC_MAX_VALUE			0x0F ;; dec 15; bin 0000 1111


;; NIBBLE
;; A BYTE WITH BIT 0 to 3 SET 
;; ------------------------
#define NIBBLE_B00001111			0x0F ;; dec 15, hex 0x0F, B00001111 


;; Fx Option Const
;; PIC Fx RAM AREA 	: 0x220 to  0x280 (DEC 544-640)
;; Fx EEPROM AREA	: 0x20  to  0x80
;; Fx BANKSTICK AREA    : 0x1D  to  0x7C
;; --------------------------
#define SELECT_Fx_RAM_BANK				0x02	;; Fx DATA IS LOCATED @ RAM BANK 2
;; --------------------------
#define RAM_OFFSET_FX_FUNCTION				0x20	;; PIC RAM PATCH: Fx Func. Area OFFSET
#define EEPROM_OFFSET_FX_FUNCTION			0x20	;; EEPROM INT PATCH: Fx Func. Area OFFSET
#define BANKSTICK_PATCH_OFFSET_FX_FUNCTION		0x1D	;; BANKSTICK PATCH : Fx Func. Area OFFSET
;; --------------------------
#define RAM_OFFSET_FX_DATA				0x40	;; PIC RAM PATCH: Fx DATA Area OFFSET
#define EEPROM_OFFSET_FX_DATA				0x40	;; PIC EEPROM INT PATCH: Fx DATA Area OFFSET
#define BANKSTICK_OFFSET_FX_DATA			0x3D	;; BANKSTICK PATCH     : Fx DATA Area OFFSET
;; --------------------------
#define RAM_OFFSET_FX_TIME_PRE_EXECUTION		0x60	;; PIC RAM PATCH: Fx TIME Area OFFSET
#define EEPROM_OFFSET_FX_TIME_PRE_EXECUTION		0x60	;; PIC EEPROM INT PATCH: Fx TIME Area OFFSET
#define BANKSTICK_OFFSET_FX_TIME_PRE_EXECUTION		0x5D	;; BANKSTICK PATCH     : Fx TIME Area OFFSET
;; --------------------------
#define FX_POS_MAX_31					0x1F 	;; 31 dec.(=> fx_pipeline.inc)
#define FX_POS_MAX_PLUS1				0x20 	;; Fx Pos Max +1 = 32
#define Fx_DELAY_TIME_MAX				0xC8 	;; 200 dec
#define FX_FUNCTIONS_MAX				0x29 	;; MAX Number of Fx Functions
#define FX_FUNCTIONS_MAX_PULS_1				0x2A 	;; MAX Number of Fx Functions +1
#define FX_POS_MAX					0x1F 	;; Fx Pos Max = 31


;; CS - LEDs
;; see @ dout_calls.inc
;; ---------
#define LED_ToneA		0x08
#define LED_ToneB		0x09
#define LED_ToneC		0x0A
#define LED_NoiseA		0x0B
#define LED_NoiseB		0x0C
#define LED_NoiseC		0x0D
#define LED_EnvelopeModeA	0x0E
#define LED_EnvelopeModeB	0x0F
#define LED_EnvelopeModeC	0x10
#define LED_Fx_A		0x11
#define LED_Fx_B		0x12
#define LED_Fx_C		0x13
#define LED_Envelope_Shape_1	0x14
#define LED_Envelope_Shape_2	0x15
#define LED_Envelope_Shape_3	0x16
#define LED_Envelope_Shape_4	0x17
#define LED_Envelope_Shape_5	0x18
#define LED_Envelope_Shape_6	0x19
#define LED_Envelope_Shape_7	0x1A
#define LED_Envelope_Shape_8	0x1B


;; LCD
#define PLUS_POS_Fx		5

;; MIDI / USER
;; =================================
#define INI_MIOS_MIDI_MERGER_ON_OFF		0x01	;; [ENABLED] | (1 = ENABLED, 0 = DISABLED)
;; MIDI CH int. [0-15]
;; MIDI CH ext. [1-16]
#define MIDI_CH_PLUS_1_OFFSETT			0x01
#define INI_TONE_A_MIDI_CHANNEL_USER		0x00 ; INITIAL VALUE = 0 = MIDI CH. 1 on LCD
#define INI_TONE_B_MIDI_CHANNEL_USER		0x01 ; INITIAL VALUE = 1 = MIDI CH. 2 on LCD
#define INI_TONE_C_MIDI_CHANNEL_USER		0x02 ; INITIAL VALUE = 2 = MIDI CH. 3 on LCD
;; MIDI SWITCH ABC TONE/NOISE ON OF
#define	INI_MIDI_ALL_OFF 			0x3F 	; INITIAL VALUE = BIN 00 111 111
							; AY REG 07     =     IO CBA CBA
							; 	    	=     BA
							; 	    	=        NOISE
							; 	    	=            TONE
							; ABC = 1 = TONE OFF
#define INI_USER_TONE_ABC_NOISE_ABC_ON_OFF	0xF8	;; USER SETTING
							;; HEX 0xF8
							;; BIN 11 111 000
							;;	  CBA CBA
							;;	  NOI TONE
#define INI_VELOCITY_TO_ACB_NOISE_PERIOD	0x07 	;; VELOCITY TO ABC VOLUME = ON
							;; VELOCITY TO NOISE PERIOD = OFF
							;; BIN 0000 0111 
							;;           CBA = ON (VELOCITY @ TONE A/B/C = 1 on / 0 = OFF)
							;;NOISE:
							;;NOISE  ON (BIT3 SET) VELOCITY
							;;NOISE  OFF(BIT3 CLEAR) VELOCITY
							;;[FOR DETAILS SEE abowe: => "VELOCITY BIT TONE / NOISE ABC"]
#define INI_FX_USER_ON_OFF_ABC			0x00	;; After Startup: All Fx = OFF
							;; USER: FX ABC off (See MAIN MENU 3 / Mixer)
							;; BIT0 = A (SET = ON / CLR = OFF)
							;; BIT1 = B (SET = ON / CLR = OFF)
							;; BIT2 = C (SET = ON / CLR = OFF)
#define MIDI_FILTER_ALL_OFF 			0x3F	; BIN 00111111 (Do not change)


							

;; CC MAX (MIDI Controller 1-29)
#define CC_MAX_VALUE_29		0x1D	;; 0x1D = 29

;; MIDI CC
#define CC22_VALUE_1		0x01
#define CC22_VALUE_2		0x02
#define CC22_VALUE_3		0x03

;; MIDI PITCH BEND
#define CC_30_PITCH_BEND	0x1E	;; 30 dec
#define CHK_POS_NEG_VIA_63	0x3F	;; 64-1 (63 dec)
#define CLEAR_OFFSET_128	0x80	;; 128

;; SWITCH: MAIN MENU <-> SUB MENU
#define SWITCH_MAIN_MENU_OFF_SUB_MENU_ON_0x01	0x01

;; LCD START SCREEN TIME (STARTUP)
#define START_SCREEN_TIME 	0x12


;; INI VALUES FOR AY REGISTER
#define INI_REG0_FINE_TUNE_A			0x40
#define INI_REG1_COARSE_TUNE_A			0x01
#define INI_REG2_FINE_TUNE_B			0x42
#define INI_REG3_COARSE_TUNE_B			0x09
#define INI_REG4_FINE_TUNE_C			0x44
#define INI_REG5_COARSE_TUNE_C			0x02
#define INI_REG6_NOISE_PERIOD			0x04
;; SET AY REGISTER 07 = 0xF8 = BIN 1111 1000 [TONE ABC ON / NOISE ABC OFF, IO AB OUT]
#define INI_REG7_TONE_ABC_NOISE_ABC_IO_AB	0xF8
;; VOLUME TONE A = 12 (BIT0-BIT3) , CLEAR BIT4 ENVELOPE=OFF
;; DEC 12, HEX 0x0C, BIN 0000 1100:
#define INI_REG8_VOLUME_A_ENVELOPE_ON_OFF	0x0C	
#define INI_REG9_VOLUME_B_ENVELOPE_ON_OFF	0x0C	
#define INI_REG10_VOLUME_C_ENVELOPE_ON_OFF	0x0C
#define INI_REG11_ENVELOPE_FINE			0x0F	; DEC 15 [0-255]
#define INI_REG12_ENVELOPE_COARSE		0x0F	; DEC 15 [0-255]
#define INI_REG13_ENVELOPE_SHAPE		0x01	; ENV. SHAPE = 1
#define INI_REG14_IO_A				0xAA 	; IO A NOT IN USE YET
#define INI_REG15_IO_B				0x00	; IO B NOT IN USE 





