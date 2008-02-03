; $Id$
;
; Setup File for TK's MIDIbox16E
;
; Number of connected encoders: 1-64
#define DEFAULT_NUMBER_ENCS	16	; number of connected encoders
; NOTE: the encoders have to be assigned to the DIN pins in mios_tables.inc
;
; Width of a group - normaly matches with the number of connected encoders, but
; the MB64E_LCD_PrintGroupNumber function in mb64e_bank.inc only supports 
; group widths which are a power of two (1, 2, 4, 8, 16, 32, 64)
#define DEFAULT_ENCODER_GROUP_WIDTH	16

; Number of connected buttons
; Allowed values: 1-64
#define DEFAULT_NUMBER_BUTTONS  64	; number of connected buttons
;
; debounce counter (see the function description of MIOS_SRIO_DebounceSet)
; Use 0 for high-quality buttons, use higher values for low-quality buttons
; NOTE: with a value > 0 the ledrings will flicker on every button movement!
#define DEFAULT_SRIO_DEBOUNCE_CTR 0
;
;
; A difficult decition: define here if you want to store many banks or many patches
; in the (optional) BankStick(s).
;    - Bank based  (PATCHMODE 0): 8 complete configurations are stored in each BankStick
;    - Patch based (PATCHMODE 1): 1 configuration bank and 128 patches are stored in each BankStick
#define DEFAULT_PATCHMODE 0
;
; Chip Select Number of BankStick which should be used:
;    -1: allows the MIDIbox to address up to 8 BankSticks (-> 64/8 Banks)
;     0: the MIDIbox addresses only the BankStick with CS=0 (-> 8/1 Banks)
;     ...
;     7: the MIDIbox addresses only the BankStick with CS=7 (-> 8/1 Banks)
;     all others: don't use any BankStick
;
; * -1 is the most prefered choice, also when only one BankStick is connected.
; * specify a CS number 0...7 when you are using different Applications 
;   with your MIDIbox in order to assign a dedicated EEPROM to it.
; * specifiy any other number to disable BankStick support at all
#define DEFAULT_BANKSTICK_CS	-1
;
; Channel which is used to send a program change event on bank changes
; and to listen for bank change requests
; Allowed values: 1-16
#define DEFAULT_BANK_PRG_CHNG_CHANNEL 16
;
; Channel which is used to send a program change event on patch changes
; and to listen for patch change requests
; Allowed values: 1-16
#define DEFAULT_PATCH_PRG_CHNG_CHANNEL 15
;
; here you can change the default device ID
#define DEFAULT_DEVICE_ID	0x00
;
; if this option is enabled (1), the DEFAULT_DEVICE_ID won't be used, but
; it will be automatically derived from the MIOS Device ID instead
#define DEFAULT_AUTO_DEVICE_ID	1
;
; For MIDI activity monitor: define the DOUT pins for the Rx and Tx LED
#define DEFAULT_MIDI_MONITOR_ENABLED 0  ; if 1, the Tx/Rx LEDs are enabled
#define DEFAULT_MIDI_RX_LED 0x40	; DOUT SR#9, pin D0
#define DEFAULT_MIDI_TX_LED 0x41	; DOUT SR#9, pin D1
;
; --> define the DOUT registers which are connected to the LED rings here:
; --> the shift registers are counted from one here - means: 1 for the first, 2 for the second, etc...
; --> please mark unused ledrings with 0
#define LEDRINGS_SR_ENC1_16_CATHODES_1	1	; first shift register with cathodes of the first 16 LED rings
#define LEDRINGS_SR_ENC1_16_CATHODES_2	2	; second shift register with cathodes of the first 16 LED rings
#define LEDRINGS_SR_ENC1_16_ANODES_1	3	; first shift register with anodes of the first 16 LED rings
#define LEDRINGS_SR_ENC1_16_ANODES_2	4	; second shift register with anodes of the first 16 LED rings
	
#define LEDRINGS_SR_ENC17_32_CATHODES_1	0	; first shift register with cathodes of the first 16 LED rings
#define LEDRINGS_SR_ENC17_32_CATHODES_2	0	; second shift register with cathodes of the first 16 LED rings
#define LEDRINGS_SR_ENC17_32_ANODES_1	0	; first shift register with anodes of the first 16 LED rings
#define LEDRINGS_SR_ENC17_32_ANODES_2	0	; second shift register with anodes of the first 16 LED rings

#define LEDRINGS_SR_ENC33_48_CATHODES_1	0	; first shift register with cathodes of the first 16 LED rings
#define LEDRINGS_SR_ENC33_48_CATHODES_2	0	; second shift register with cathodes of the first 16 LED rings
#define LEDRINGS_SR_ENC33_48_ANODES_1	0	; first shift register with anodes of the first 16 LED rings
#define LEDRINGS_SR_ENC33_48_ANODES_2	0	; second shift register with anodes of the first 16 LED rings
;
#define LEDRINGS_SR_ENC49_64_CATHODES_1	0	; first shift register with cathodes of the first 16 LED rings
#define LEDRINGS_SR_ENC49_64_CATHODES_2	0	; second register with cathodes of the first 16 LED rings
#define LEDRINGS_SR_ENC49_64_ANODES_1	0	; first shift register with anodes of the first 16 LED rings
#define LEDRINGS_SR_ENC49_64_ANODES_2	0	; second shift register with anodes of the first 16 LED rings

;; see the documentation of MIOS_ENC_SpeedSet for the purpose of these values
#define DEFAULT_ENC_SPEED_NORMAL_MODE   MIOS_ENC_SPEED_NORMAL
#define DEFAULT_ENC_SPEED_NORMAL_DIV	0
#define DEFAULT_ENC_SPEED_SLOW_MODE     MIOS_ENC_SPEED_SLOW
#define DEFAULT_ENC_SPEED_SLOW_DIV	6	; (6: increment on every 7th step)
#define DEFAULT_ENC_SPEED_FAST_MODE     MIOS_ENC_SPEED_FAST
#define DEFAULT_ENC_SPEED_FAST_DIV	2	; (2: divider = 2^(7-2) = 32)

; This DIN map allows you to customize the MB64E application to different hardwares
; The MB64E dump structure allows the use of up to 64 buttons, they are grouped to 8 buttons per shift register
; Define the used shift registers for the buttons here
; the shift registers are counted from one - means: 1 for the first, 2 for the second, etc...
; mark unused button groups with 0
	;; NOTE: by default, the first two DIN shift registers are used for the encoders
#define DEFAULT_DIN_SR_PIN_01_08	2
#define DEFAULT_DIN_SR_PIN_09_16	3
#define DEFAULT_DIN_SR_PIN_17_24	1
#define DEFAULT_DIN_SR_PIN_25_32	4
#define DEFAULT_DIN_SR_PIN_33_40	9
#define DEFAULT_DIN_SR_PIN_41_48	10
#define DEFAULT_DIN_SR_PIN_49_56	11
#define DEFAULT_DIN_SR_PIN_57_64	12
;
; Datawheel for menu navigation/data entry connected or not?
; A datawheel can replace the left/right buttons!
; if 0: datawheel not connected
; if >= 1: encoder number assigned to datawheel function
; don't forget to check the pins of the datawheel in MIOS_ENC_PIN_TABLE (-> mios_tables.inc or setup_*.asm)
; it has to be connected to pin 4 and 5 by default
#define DEFAULT_ENC_DATAWHEEL   0
;
; DIN pin numbers of menu buttons
#if DEFAULT_ENC_DATAWHEEL == 0
  #define DEFAULT_DIN_MENU_EXEC		7	; menu exec button assigned to DIN pin #7
  #define DEFAULT_DIN_MENU_RIGHT	6	; menu right button assigned to DIN pin #6
  #define DEFAULT_DIN_MENU_LEFT		5	; menu left button assigned to DIN pin #5
  #define DEFAULT_DIN_MENU_SNAPSHOT	4	; menu snapshot button assigned to DIN pin #4
#else
  #define DEFAULT_DIN_MENU_EXEC		7	; menu exec button assigned to DIN pin #7
  #define DEFAULT_DIN_MENU_SNAPSHOT	6	; menu snapshot button assigned to DIN pin #4
  #define DEFAULT_DIN_MENU_RIGHT	5	; NOT USED - overlayed by datawheel
  #define DEFAULT_DIN_MENU_LEFT		4	; NOT USED - overlayed by datawheel
#endif
;
; This DOUT map allows you to customize the MB64E application to different hardwares
; The MB64E dump structure allows the use of up to 64 LEDs, they are grouped to 8 LEDs per shift register
; Define the used shift registers for the LEDs here
; the shift registers are counted from one - means: 1 for the first, 2 for the second, etc...
; mark unused LED groups with 0
	;; NOTE: by default, the first four DOUT shift registers are used for the encoders
#define DEFAULT_DOUT_SR_PIN_01_08	5
#define DEFAULT_DOUT_SR_PIN_09_16	6
#define DEFAULT_DOUT_SR_PIN_17_24	7
#define DEFAULT_DOUT_SR_PIN_25_32	8
#define DEFAULT_DOUT_SR_PIN_33_40	9
#define DEFAULT_DOUT_SR_PIN_41_48	10
#define DEFAULT_DOUT_SR_PIN_49_56	11
#define DEFAULT_DOUT_SR_PIN_57_64	12
;
;
; Some menus are providing the possibility to use 16 "general purpose" buttons
; for selecting a parameter - e.g. the Bank Select menu allows to directly change the
; Bank with these buttons. So long as you stay in this menu, the normal function of
; these buttons (triggering MIDI or SFB events) is disabled
; Define the two shift registers which are assigned to this function here:
; 0 disables the GP buttons (not recommented!)
#define DEFAULT_GP_DIN_SR0	2	; first GP DIN shift register assigned to SR#2
#define DEFAULT_GP_DIN_SR1	3	; second GP DIN shift register assigned to SR#3
;
; above these buttons LEDs should be mounted to visualize the selected parameter
; (e.g. to visualize the bank which corresponds with the buttons below)
; Define the two shift registers which are assigned to this function here:	
; 0 disables the GP LEDs (not recommented!)
#define DEFAULT_GP_DOUT_SR0	5	; first GP DOUT shift register assigned to SR#5
#define DEFAULT_GP_DOUT_SR1	6	; second GP DOUT shift register assigned to SR#6
;
; with following settings it is possible to center the screen on 2x20 and 2x40 LCDs
; see also the functional description of MIOS_LCD_YAddressSet
					; recommented values:
					; 2x16 | 2x20 | 4x20 | Comments
					; -----+------+------+----------
#define DEFAULT_YOFFSET_LINE0	0x00	; 0x00 | 0x02 | 0x42 | cursor pos: 0x00-0x0f
#define DEFAULT_YOFFSET_LINE1	0x40	; 0x40 | 0x42 | 0x16 | cursor pos: 0x40-0x4f
#define DEFAULT_YOFFSET_LINE2	0x14	; 0x14 | 0x16 | 0x02 | cursor pos: 0x80-0x8f (not used yet)
#define DEFAULT_YOFFSET_LINE3	0x54	; 0x54 | 0x56 | 0x56 | cursor pos: 0xc0-0xcf (not used yet)
;
; The morphing function uses addresses within the MIOS address range which are 
; reserved for the AIN handler. 
; NOTE: morphing is automatically disabled if analog pots/faders are connected
#define DEFAULT_MORPH_FUNCTION_ENABLED 1
;
; Although MIDIbox64E has been designed for rotary encoders, it can also handle with
; up to 64 pots/faders or up to 8 motorfaders.
; Pots and faders are mapped to the "encoder" entries 64-128.
; Example: if group width is 16, and group 1 is selected, encoders are using
; entry 1-16, and pots are using entry 64-70
; NOTE: morphing is automatically disabled if analog pots/faders are connected
#define DEFAULT_NUMBER_AIN		0
;
; you could enable the multiplexers here to test this application
; with a MIDIbox64 based hardware
; if 0: no multiplexers (a *must* when MF module connected)
; if 1: use multiplexers
#define DEFAULT_ENABLE_AIN_MUX		0
;
; motorfaders connected?
; if 0: MF module disabled
; if 1: MF module enabled
#define DEFAULT_ENABLE_MOTORDRIVER	0
;
; The well known motorfader calibration values -- can also be changed "online" in the calibration menu
#define DEFAULT_MF_PWM_DUTY_UP	  0x01	; PWM duty cycle for upward moves   (see http://www.ucapps.de/mbhp_mf.html)
#define DEFAULT_MF_PWM_DUTY_DOWN  0x01	; PWM duty cycle for downward moves (see http://www.ucapps.de/mbhp_mf.html)
#define DEFAULT_MF_PWM_PERIOD     0x03	; PWM period                        (see http://www.ucapps.de/mbhp_mf.html)
;
;
; following settings configure the touch sensors
; The DIN shift register to which the 8 touch sensors are connected has to be defined here
;    Shift Register 9-16: touch sensor changes won't trigger the MBMF button handler
;                         no MIDI event will be sent
;    Shift Register 1-8:  touch sensor changes will trigger the MBMF button handler
;    0:                   touch sensor disabled
#define DEFAULT_TOUCH_SENSOR_SR		0
;
; Now define one of three behaviours (note: value can be changed "online" in the TS mode menu)
; The TS mode works independent from the MBMF button handler, means: use TOUCH_SENSOR_SR >= 8
; to suspend the motors w/o allocating a button function
; 
; TOUCH_SENSOR_MODE EQU 0: no additional action when touch sensor pressed/depressed
; TOUCH_SENSOR_MODE EQU 1: the motor will be suspended via MIOS when the appr. touch sensor 
;                          is pressed, so that it will not be moved on incoming MIDI events
; TOUCH_SENSOR_MODE EQU 2: like mode 1, additionally no MIDI event will be sent when the
;                          touch sensor is *not* pressed.
; 
; Mode "1" is used by default to avoid circular troubleshooting requests in the MIDIbox forum from people
; who don't read this information before starting the application.
; Mode "2" should be the prefered setting if your touch sensors are working properly
#define DEFAULT_TOUCH_SENSOR_MODE	1
;
; The touch sensor sensitivity (can also be changed "online" in the "Touch Sensor" menu)
#define	DEFAULT_TOUCH_SENSOR_SENSITIVITY 3



	org	0x3280		; never change the origin!
;; In this table DIN pins have to be assigned to rotary encoders for the
;; MIOS_ENC driver 
;; 
;; up to 64 entries are provided
;; 
;; The table must be terminated with an ENC_EOT entry. Unused entries should
;; be filled with ENC_EOT
;; 
;; ENC_ENTRY provides following parameters
;;    o first parameter: number of shift register - 1, 2, 3, ... 16
;;    o second parameter: number of pin; since two pins are necessary
;;      for each encoder, an even number is expected: 0, 2, 4 or 6
;;    o the third parameter contains the encoder mode:
;;      either MIOS_ENC_MODE_NON_DETENTED
;;          or MIOS_ENC_MODE_DETENTED
;;
;; Configuration Examples:
;;    ENC_ENTRY  1,  0,  MIOS_ENC_MODE_NON_DETENTED    ; non-detented encoder at pin 0 and 1 of SR 1
;;    ENC_ENTRY  1,  2,  MIOS_ENC_MODE_DETENTED        ; detented encoder at pin 2 and 3 of SR 1
;;    ENC_ENTRY  9,  6,  MIOS_ENC_MODE_NON_DETENTED    ; non-detented encoder at pin 6 and 7 of SR 9
;; --------------------------------------------------------------------------
	;; encoder entry structure
ENC_ENTRY MACRO sr, din_0, mode
	dw	(mode << 8) | (din_0 + 8*(sr-1))
	ENDM	
ENC_EOT	MACRO
	dw	0xffff
	ENDM

MIOS_ENC_PIN_TABLE
	;; encoders 1-16
	;;        SR  Pin  Mode
	ENC_ENTRY  5,  0,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 1
	ENC_ENTRY  5,  2,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 2
	ENC_ENTRY  5,  4,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 3
	ENC_ENTRY  5,  6,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 4
	ENC_ENTRY  6,  0,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 5
	ENC_ENTRY  6,  2,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 6
	ENC_ENTRY  6,  4,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 7
	ENC_ENTRY  6,  6,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 8
	ENC_ENTRY  7,  0,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 9
	ENC_ENTRY  7,  2,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 10
	ENC_ENTRY  7,  4,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 11
	ENC_ENTRY  7,  6,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 12
	ENC_ENTRY  8,  0,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 13
	ENC_ENTRY  8,  2,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 14
	ENC_ENTRY  8,  4,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 15
	ENC_ENTRY  8,  6,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 16

	;; encoders 17-32
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT

	;; encoders 33-48
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT

	;; encoders 49-64
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT
	ENC_EOT


#include "src/main.inc"
