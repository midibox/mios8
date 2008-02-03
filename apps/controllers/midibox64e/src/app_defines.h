; $Id$
;
; Specific Defines and variables of MIOS Application
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; common MIDIbox status flags
#define MB_STAT_SEL_PARAMETER		0 ; if set, the parameter map will be modified with the encoder
#define MB_STAT_SEL_GROUP_AND_BANK	1 ; if set, the bank and group will be modified with the encoder
#define MB_STAT_SNAPSHOT_REQ		2 ; if set, request a snaphot (used for snapshot-at-poweron feature)
#define MB_STAT_SNAPSHOT_HOLD           3 ; if set, the snapshot button is pressed
#define MB_STAT_LEARN_MODE              4 ; if set, we are in learn mode
#define MB_STAT_LAST_WAS_BUTTON         5 ; if set, last controller move was a button, else encoder
#define MB_STAT_USE_BANKSTICK		6 ; if set, the bankstick will be used by the BANK_ routines
#define MB_STAT_BANKSTICK_AVAILABLE	7 ; if set, the bankstick is available

;; MIDIbox64E specific status flags
#define MB_STAT2_DONT_FLASH_L_GP_DOUT	0 ; if set, the 8 GP leds on the left side won't be flashed
#define MB_STAT2_DONT_FLASH_R_GP_DOUT	1 ; if set, the 8 GP leds on the right side won't be flashed
#define MB_STAT2_SHOW_SELECTED		2 ; if set, leds of the current selected encoders will be rotated
#define MB_STAT2_SHOW_SEL_PATTERN	3 ; if set, the pattern of the selected LED will be displayed
#define MB_STAT2_ENC_NORMAL		4 ; if set, all encoders are forced to normal speed
#define MB_STAT2_ENC_SLOW		5 ; if set, all encoders are forced to slow speed
#define MB_STAT2_ENC_FAST		6 ; if set, all encoders are forced to fast speed
#define MB_STAT2_MORPH_MODE		7 ; if set, we are in morphing mode

#define MB_STAT3_CALI_MODE		0 ; if set, we are in calibration mode
#define MB_STAT3_FADER_SOFT_UPDATE_REQ	1 ; if set, software implemented touch detection will be taken into account
#define MB_STAT3_FADER_UPDATE_REQ	2 ; if set, motorfaders will be repositioned
#define MB_STAT3_MUTE_FADERS		3 ; if set, no fader events will be sent
#define MB_STAT3_MUTE_ENCODERS		4 ; if set, no encoder events will be sent

;; ==========================================================================

MB64E_CFG0_MERGEROFF		EQU 0 ; if set, merger is disabled
MB64E_CFG0_TSMODE0		EQU 2 ; bit #0 of touch sensor mode
MB64E_CFG0_TSMODE1		EQU 3 ; bit #1 of touch sensor mode

MB64E_CFG1_SEND_PC_ON_BCHANGE      EQU 0 ; if set, the MIDIbox64E sends a Program Change controller on Bank Changes
MB64E_CFG1_RECEIVE_PC_FOR_BCHNGE   EQU 1 ; if set, the MIDIbox64E reacts on Program Change controllers for Bank Changes
MB64E_CFG1_MBLINK                  EQU 2 ; if set, the MIDIbox Link is enabled
MB64E_CFG1_SNAPSHOT_AT_POWERON     EQU 4 ; if set, a snapshot is sent at poweron
MB64E_CFG1_AUTO_SNAPSHOT           EQU 6 ; if set, MIDIbox64E will save/restore a snapshot on bankchanges

;; ==========================================================================

#define BANKSTICK_MAGIC0	0x58 ; magic numbers - if they don't exists in bankstick at BANK_MAGIC_ADDRESS, 
#define BANKSTICK_MAGIC1	0x23 ; the stick will be ignored until it has been formatted

#define BANKSTICK_PATCH_MAGIC0	0x9a ; the same for patches, here the 7th bit must be set!
#define BANKSTICK_PATCH_MAGIC1	0xb2

;; ==========================================================================

#define CS_STAT_DISPLAY_GLCD		0	; flag 0: set if graphical lcd is connected
#define CS_STAT_DISPLAY_INIT_REQ	1	; flag 1: request a display initialization
#define CS_STAT_DISPLAY_UPDATE_REQ	2	; flag 2: request a display update
#define CS_STAT_CURSOR_FLASH		3	; flag 3: handled by cs_menu_timer.inc
#define CS_STAT_WITHIN_PAGE		4	; flag 4: if 0, show menu, else go into menu page
#define CS_STAT_DELAYED_CALLBACK	5	; flag 5: if set, the CALLBACK function will be executed after 2 seconds
#define CS_STAT_MESSAGE_REQ		6	; flag 6: if set, a CS message will be print for some seconds
#define CS_STAT_MESSAGE_HOLD		7	; flag 7: if set, the CS message will stay active

;; ==========================================================================
;;  Message IDs
;; ==========================================================================
MB64E_MSG_ID_BANK	EQU	0x00		; show the bank/group screen
MB64E_MSG_ID_MAP	EQU	0x01		; show the map screen

;; ==========================================================================
;;  Encoder Modes
;; ==========================================================================
ENC_MODE_ABSOLUTE       EQU     0x00
ENC_MODE_40SPEED	EQU	0x01
ENC_MODE_00SPEED	EQU	0x02
ENC_MODE_40_1		EQU	0x03
ENC_MODE_00_1		EQU	0x04
ENC_MODE_INCDEC		EQU	0x05

	;; following values will be ored to the mode
ENC_MODE_SPEED_NORMAL	EQU	0x00
ENC_MODE_SPEED_FAST	EQU	0x10
ENC_MODE_SPEED_SLOW	EQU	0x20
	;; for bit checks
ENC_MODE_SPEED_FAST_FLAG EQU	4
ENC_MODE_SPEED_SLOW_FLAG EQU	5

;; ==========================================================================
;;  LEDring patterns
;; ==========================================================================
LEDRING_PATTERN_0	EQU	0
LEDRING_PATTERN_1	EQU	1
LEDRING_PATTERN_2	EQU	2
LEDRING_PATTERN_3	EQU	3


;; ==========================================================================


	;; free memory for user applications:	
	;; 0x010-0x37f

;; ==========================================================================
;;  General MB64E Control Variables in ACCESS page
;; ==========================================================================
MB_STAT			EQU	0x010
MB_STAT2		EQU	0x011
MB_STAT3		EQU	0x012
MB_BANK			EQU	0x013
MB_NEW_BANK		EQU	0x014
MB_PATCH		EQU	0x015

MB_ADDRL		EQU	0x016
MB_ADDRH		EQU	0x017

MIDI_EVNT0		EQU	0x018
MIDI_EVNT1		EQU	0x019
MIDI_EVNT_VALUE		EQU	0x01a

MIDI_RXTX_RX_CTR	EQU	0x01b
MIDI_RXTX_TX_CTR	EQU	0x01c


;; ==========================================================================
;;  Variables used by the control surface
;; ==========================================================================
CS_STAT			EQU	0x020	; control surface status
CS_MENU_HEADER_TBLPTRL	EQU	0x021	; used by cs_menu.inc
CS_MENU_HEADER_TBLPTRH	EQU	0x022	; used by cs_menu.inc
CS_MENU_ENTRY_TBLPTRL	EQU	0x023	; used by cs_menu.inc
CS_MENU_ENTRY_TBLPTRH	EQU	0x024	; used by cs_menu.inc
CS_MENU_MESSAGE_CTR	EQU	0x025	; handled in cs_menu_timer.inc, used for temporary encoder messages
CS_MENU_POS		EQU	0x026	; menu position
CS_MENU_NEW_POS		EQU	0x027	; used by the change page function
CS_MENU_ENTRIES		EQU	0x028	; number of menu entries
CS_MENU_TIMER_CTR	EQU	0x029	; handled by cs_menu_timer.inc, used as predivider
CS_MENU_CURSOR_CTR	EQU	0x02a	; handled by cs_menu_timer.inc, used for flashing cursors
CS_MENU_CURSOR_POS	EQU	0x02b	; used by some cs_m_*.inc handlers
CS_MENU_DEL_CALLBACK_CTR EQU	0x02c	; handled by cs_menu_timer.inc
	
CS_MENU_PARAMETER_L	EQU	0x02e	; contains low-byte of current parameter value, must be located to an even address!!!
CS_MENU_PARAMETER_H	EQU	0x02f	; contains high-byte of current parameter value
CS_MENU_PARAMETER_MAX_L	EQU	0x030	; contains max value of current parameter, must be located to an even address!!!
CS_MENU_PARAMETER_MAX_H	EQU	0x031	; contains max value of current parameter
CS_MENU_PARAMETER_HOOK_L EQU	0x032	; contains low-byte of function which will be called after value change
CS_MENU_PARAMETER_HOOK_H EQU	0x033	; contains high-byte of function which will be called after value change
CS_MENU_DEL_CALLBACK_HOOK_L EQU	0x034	; contains low-byte of delayed callback function (e.g. used by snapshot save button)
CS_MENU_DEL_CALLBACK_HOOK_H EQU	0x035	; contains high-byte of delayed callback function (e.g. used by snapshot save button)
CS_MENU_GP_CALLBACK_HOOK_L EQU	0x036	; contains low-byte of callback function used for general purpose buttons
CS_MENU_GP_CALLBACK_HOOK_H EQU	0x037	; contains high-byte of callback function used for general purpose buttons
CS_MENU_GP_OVERLAY_DOUT_SR0 EQU	0x038	; contains the LED status of GP DOUT #0
CS_MENU_GP_OVERLAY_DOUT_SR1 EQU	0x039	; contains the LED status of GP DOUT #1

CS_MENU_LAST_ENC_ENTRY	EQU	0x03a	; used by cs_m_encmode.inc and cs_m_ledrp.inc to determine changes
CS_MENU_LAST_BANK	EQU	0x03b	; used by cs_m_encmode.inc and cs_m_ledrp.inc to determine changes

MY_BUTTON_TOGGLE_STATE	EQU	0x03c

;; ==========================================================================
;; ==========================================================================
;; ==========================================================================

MB64E_LEDRING_PATTERNS	EQU	0x040
MB64E_LEDRING_PATTERNS_END EQU	0x07f

MB64E_ENC_MODE_BUFFER	EQU	MB64E_LEDRING_PATTERNS	   ; (temporary used when cs_m_encmode.inc is active)
MB64E_ENC_MODE_BUFFER_END EQU	MB64E_LEDRING_PATTERNS_END ; (in this mode the common ledring patterns are switched off)

;; ==========================================================================

MB64E_BASE		EQU	0x080
MB64E_CFG0		EQU	0x080
MB64E_CFG1		EQU	0x081
MB64E_DEVICE_ID		EQU	0x082
MB64E_DISPLAY_PAGE	EQU	0x083
MB64E_BANKNAME_BEGIN	EQU	0x084; (16 bytes)
MB64E_BANKNAME_END	EQU	0x093
MB64E_LED_MAP_BEGIN	EQU	0x094; (8 bytes)
MB64E_LED_MAP_END	EQU	0x09b

MB64E_CURRENT_ENTRY	EQU	0x09c
MB64E_LAST_ENTRY	EQU	0x09d
MB64E_LAST_ENTRY_MAPPED	EQU	0x09e
MB64E_ENTRY_MIN_VALUE	EQU	0x09f
MB64E_ENTRY_MAX_VALUE	EQU	0x0a0
	
MB64E_CURRENT_BUTTON	EQU	0x0a1
MB64E_LAST_BUTTON	EQU	0x0a2

MB64E_LEARN_EVNT0	EQU	0x0a3
MB64E_LEARN_EVNT1	EQU	0x0a4
MB64E_LEARN_EVNT2	EQU	0x0a5
MB64E_LEARN_BMODE	EQU	0x0a6

MB64E_SYSEX_STATE	EQU	0x0a7
MB64E_SYSEX_ACTION	EQU	0x0a8
MB64E_SYSEX_IN		EQU	0x0a9
MB64E_SYSEX_CHECKSUM	EQU	0x0aa

MB64E_MORPH_GROUP_SELECT EQU	0x0ab
MB64E_MORPH_VALUE	EQU	0x0ac
MB64E_MORPH_BANK	EQU	0x0ad

MB64E_CURRENT_ELEMENT	EQU	0x0ae
MB64E_CURRENT_ELEMENT_LEN EQU	0x0af

MB64E_GLOBAL_CHANNEL	EQU	0x0b0

MB64E_CURRENT_ENC_MODE	EQU	0x0b1

MB64E_LEDRINGS_CTR	EQU	0x0b2
MB64E_LEDRINGS_CTR2	EQU	0x0b3

MB64E_GROUP_OFFSET	EQU	0x0b4

MB64E_MSG_ID		EQU	0x0b5

MB64E_SELECTED_BANK	EQU	0x0b6
MB64E_SELECTED_PATCH	EQU	0x0b7
MB64E_SELECTED_BP_TMP	EQU	0x0b8

MB64E_TS_STATUS		EQU	0x0b9
MB64E_FADER_LAST_TURNED_00 EQU	0x0ba
MB64E_FADER_LAST_TURNED_7F EQU	0x0bb
MB64E_FADER_NEW_VALUE	 EQU	0x0bc
MB64E_FADER_LAST_VALUE	 EQU	0x0bd

MB64E_DATA_BUFFER	EQU	0x0c0	; used for page writes/reads
MB64E_DATA_BUFFER_END	EQU	0x0ff	; could also be overlayed with restorable registers (like MB64E_ENC_EVNT*)

;; -----------------------------------
;; 256 bytes which are temporary used for SysEx uploads
;; the content of this address area will be restored after the upload
MB64E_UPLOAD_BUFFER	EQU	0x100
MB64E_UPLOAD_BUFFER_END	EQU	0x1ff
;; -----------------------------------

	;; encoder events, byte 0
MB64E_ENC_EVNT0_00	EQU	0x100
	;; ...
MB64E_ENC_EVNT0_7F	EQU	0x17f

	;; encoder events, byte 1
MB64E_ENC_EVNT1_00	EQU	0x180
	;; ...
MB64E_ENC_EVNT1_7F	EQU	0x1ff

	;; encoder values (events, byte 2)
MB64E_ENC_VALUES_00	EQU	0x200
	;; ...
MB64E_ENC_VALUES_7F	EQU	0x27f

	;; encoder parameter map
MB64E_ENC_MAP_00	EQU	0x280
	;; ...
MB64E_ENC_MAP_3F	EQU	0x2bf

	;; button values (packed)
MB64E_BUTTON_VALUES_SR0	EQU	0x2c0
	;; ...
MB64E_BUTTON_VALUES_SR7	EQU	0x2c7

	;; midi button values (packed)
MB64E_MBUTTON_VALUES_SR0	EQU	0x2c8
	;; ...
MB64E_MBUTTON_VALUES_SR7	EQU	0x2cf

	;; morph values of group 0-7
MB64E_MORPH_VALUES_G0		EQU	0x2d0
	;; ...
MB64E_MORPH_VALUES_G7		EQU	0x2d7

;; ==========================================================================

	;; button events, byte 0
MB64E_BUTTON_EVNT0_00	EQU	0x300
	;; ...
MB64E_BUTTON_EVNT0_3F	EQU	0x33f

	;; button events, byte 1
MB64E_BUTTON_EVNT1_00	EQU	0x340
	;; ...
MB64E_BUTTON_EVNT1_3F	EQU	0x37f

;; ==========================================================================
;; Attention: only used by the morph handler to save the last values
;; Located to 0x380-0x3ff which is normaly reserved by MIOS
;; but only used by the AIN handler which is suspended here (number of pots == 0)
;; This possibility to get extra memory should be documented sooner or later

#if DEFAULT_NUMBER_AIN == 0 && DEFAULT_MORPH_FUNCTION_ENABLED == 1
MB64E_ENC_LAST_VALUES_00 EQU	0x380
	;; ...
MB64E_ENC_LAST_VALUES_7F EQU	0x3ff
#endif
