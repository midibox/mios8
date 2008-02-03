; $Id$
;
; Special Setup File for TK's MIDIbox LC
;
; Differences compared to setup_midibox_lc.asm:
;    o TOUCH_SENSOR_MODE set to 2
;
;
#define LC_EMULATION_ID	0x10		; use 0x10 for Logic Control
					;     0x11 for Logic Control XT
					;     0x14 for Mackie Control
					;     0x15 for Mackie Control XT (?)

#define LCD_USE_SECOND	1		; the emulation handles with two character LCDs if 1
#define LCD_EMU_COL     55		; number of emulated columns (characters per line)
					;    o graphical LCDs: use 55 (like a Logic Control)
					;    o one 2x40 LCD: use 40   (unfortunately..)
					;    o two 2x40 LCDs: use 55  (like a Logic Control)

#define INITIAL_DISPLAY_PAGE	0	; initial display page after startup (choose your favourite one: 0-3)

#define ENABLE_MOTORDRIVER	1	; if 0, MF module will not be enabled
#define NUMBER_MOTORFADERS	8	; number of motorfaders
#define MOTORFADERS_DEADBAND	3	; so called "deadband" of motorfaders
#define AIN_DEADBAND		3	; so called "deadband" of ADC
#define MOTORFADERS_PWM_DUTY_UP	  0x01	; PWM duty cycle for upward moves   (see http://www.ucapps.de/mbhp_mf.html)
#define MOTORFADERS_PWM_DUTY_DOWN 0x01	; PWM duty cycle for downward moves (see http://www.ucapps.de/mbhp_mf.html)
#define MOTORFADERS_PWM_PERIOD    0x03	; PWM period                        (see http://www.ucapps.de/mbhp_mf.html)

#define MOTORFADER0_IS_MASTERFADER 0	; if set, the first motorfader acts as master fader

;; following setting configures the touch sensor behaviour. The touch sensors of the motorfaders
;; have to be assigned to the DIN pins in lc_io_tables.inc - the appr. IDs are:
;; ID_FADER_TOUCH_CHN1, ID_FADER_TOUCH_CHN2, ... ID_FADER_TOUCH_CHN8 (8 sensors)
;; If the master fader option is used, the ID is ID_FADER_TOUCH_MASTER
;; 
;; TOUCH_SENSOR_MODE EQU 0: touch sensor events (pressed/depressed) are forwarded to the host program
;;                          like specified in the Logic/Mackie Control specification
;; TOUCH_SENSOR_MODE EQU 1: like mode 0, but additionaly the motors will be suspended via MIOS, so
;;                          that they are not moved so long as the touch sensor is pressed
;; TOUCH_SENSOR_MODE EQU 2: like mode 0+1, additionally no fader move (PitchBender event) will be
;;                          sent when the touch sensor is not pressed.
;; 
;; The mode "1" by default to avoid circular troubleshooting requests in the MIDIbox forum from people
;; who don't read this information before starting the application.
;; Mode "2" should be the prefered setting if your touch sensors are working properly
#define TOUCH_SENSOR_MODE	2

#define NUMBER_SHIFTREGISTERS	16	; length of shift register chain (max value: 16 == 128 DIN/DOUT pins)
#define SRIO_UPDATE_FREQUENCY	1	; we are using rotary encoders: use 1 ms!
#define TOUCHSENSOR_SENSITIVITY 3	; for the motorfader touchsensors


;; define the encoder speed modes and dividers here
#define ENC_VPOT_SPEED_MODE          2  ; 0 = slow, 1 = normal, 2 = fast
#define ENC_VPOT_SPEED_DIVIDER       2  ; see description about MIOS_ENC_SpeedSet

#define ENC_JOGWHEEL_SPEED_MODE      2  ; 0 = slow, 1 = normal, 2 = fast
#define ENC_JOGWHEEL_SPEED_DIVIDER   1  ; see description about MIOS_ENC_SpeedSet

;; used by lc_srio.inc
;; the schematic can be found under http://www.ucapps.de/midibox_lc/midibox_lc_ledrings_meters.pdf
;; NOTE: it's possible to display the meter values with the LEDrings by using ID_MBLC_*LEDMETER* buttons!
;; this feature saves you from adding additional LEDs to your MIDIbox
#define LEDRINGS_ENABLED	1	; if 1, ledrings are enabled
#define METERS_ENABLED		0	; if 1, meters are enabled
;; NOTE2: the shift registers are counted from zero here, means: 0 is the first shift register, 1 the second...
#define LEDRINGS_SR_CATHODES	8	; shift register with cathodes of the 8 LED rings
#define METERS_SR_CATHODES	9	; shift register with cathodes of the 8 meters
#define LEDRINGS_METERS_SR_ANODES_1 10	; first shift register with anodes of the 8 LED rings (and 8 meters)
#define LEDRINGS_METERS_SR_ANODES_2 11	; second shift register with anodes of the 8 LED rings (and 8 meters)

;; used by lc_leddigits.inc
;; the schematic can be found under http://www.ucapps.de/midibox_lc/midibox_lc_leddigits.pdf
;; NOTE: in principle this driver supports up to 16 LED digits, but only 12 of them are used
#define LEDDIGITS_ENABLED		0	; if 1, leddigits are enabled
#define LEDDIGITS_SR_SEGMENTS_1		12	; shift register which drives the segments of digit 7-0 (right side)
#define LEDDIGITS_SR_SELECT_1		13	; shift register which selects the digits 7-0
#define LEDDIGITS_SR_SEGMENTS_2		14	; shift register which drives the segments of digit 15-8 (left side)
#define LEDDIGITS_SR_SELECT_2		15	; shift register which selects the digits 15-8

;; the GPC (General Purpose Controller) feature can be activated by using ID_MBLC_*GPC* buttons
;; up to 128 midi events are defined in mios_tables.inc
;; the lables are defined in lc_gpc_lables.inc
;; optionally a MIDI event will be sent when entering/leaving this mode:
#define GPC_BUTTON_SENDS_MIDI_EVENT	0	; enables the "send button" feature

#define GPC_BUTTON_ON_EVNT0		0x9f	; on event, byte #0
#define GPC_BUTTON_ON_EVNT1		0x3c	; on event, byte #1
#define GPC_BUTTON_ON_EVNT2		0x7f	; on event, byte #2

#define GPC_BUTTON_OFF_EVNT0		0x9f	; off event, byte #0
#define GPC_BUTTON_OFF_EVNT1		0x3c	; off event, byte #1
#define GPC_BUTTON_OFF_EVNT2		0x00	; off event, byte #2

;; with MIOS V1.4 the MIDI Rx/Tx streams can be monitored, this gives you the possibility
;; to realize software controlled MIDI status LEDs - feature has to be enabled here:
#define MIDI_MONITOR_ENABLED		0	; if 1, the Tx/Rx LEDs are enabled
#define MIDI_MONITOR_RX_LED		0x00	; DOUT pin of the MIDI Rx LED
#define MIDI_MONITOR_TX_LED		0x01	; DOUT pin of the MIDI Tx LED


	org	0x3280		; never change the origin!
;; --------------------------------------------------------------------------
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
;;          or MIOS_ENC_MODE_DETENTED2
;;          or MIOS_ENC_MODE_DETENTED3
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
	ENC_ENTRY 13,  0,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 1
	ENC_ENTRY 13,  2,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 2
	ENC_ENTRY 13,  4,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 3
	ENC_ENTRY 13,  6,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 4
	ENC_ENTRY 14,  0,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 5
	ENC_ENTRY 14,  2,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 6
	ENC_ENTRY 14,  4,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 7
	ENC_ENTRY 14,  6,  MIOS_ENC_MODE_NON_DETENTED	; V-Pot 8

	ENC_ENTRY 15,  0,  MIOS_ENC_MODE_NON_DETENTED	; Jog-Wheel
	ENC_EOT


#include "src/main.inc"
