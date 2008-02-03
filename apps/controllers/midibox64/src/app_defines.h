; $Id$
;
; Specific Defines and variables of MIOS Application
;
; ==========================================================================
;
; Copyright (C) 1998-2003  Thorsten Klose (Thorsten.Klose@midibox.org)
;                          http://www.uCApps.de
; 
; ==========================================================================
; 
; This file is part of MIDIbox64
;
; MIDIbox64 is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; MIDIbox64 is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with MIDIbox64; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;
; ==========================================================================

#define MB_STAT_SHOW_MIDICLK		0 ; if set, MIDIclock will be displayed at first line
#define MB_STAT_SHOW_MTC		1 ; if set, MTC will be displayed at first line
#define MB_STAT_SNAPSHOT_REQ		2 ; if set, request a snaphot (used for snapshot-at-poweron feature)
#define MB_STAT_SNAPSHOT_HOLD           3 ; if set, the snapshot button is pressed
#define MB_STAT_LEARN_MODE              4 ; if set, we are in learn mode
#define MB_STAT_LAST_WAS_BUTTON         5 ; if set, last controller move was a button, else pot
#define MB_STAT_USE_BANKSTICK		6 ; if set, the bankstick will be used by the BANK_ routines
#define MB_STAT_BANKSTICK_AVAILABLE	7 ; if set, the bankstick is available

#define MB_STAT2_DONT_FLASH_L_GP_DOUT	0 ; if set, the 8 GP leds on the left side won't be flashed
#define MB_STAT2_DONT_FLASH_R_GP_DOUT	1 ; if set, the 8 GP leds on the right side won't be flashed

;; ==========================================================================

MB64_CFG0_MERGEROFF         EQU 0 ; if set, merger is disabled
MB64_CFG0_SNAP              EQU 2 ; if set, soft takeover feature is selected
MB64_CFG0_RELATIVE          EQU 3 ; if set, relative feature is selected (NOTE: in this mode, snap mode must also be set!!!)
MB64_CFG0_PARALLAX          EQU 4 ; if set, parallax feature is selected (NOTE: in this mode, snap mode must also be set!!!)

MB64_CFG1_SEND_PC_ON_BCHANGE      EQU 0 ; if set, the MIDIbox64 sends a Program Change controller on Bank Changes
MB64_CFG1_RECEIVE_PC_FOR_BCHNGE   EQU 1 ; if set, the MIDIbox64 reacts on Program Change controllers for Bank Changes
MB64_CFG1_MBLINK                  EQU 2 ; if set, the MIDIbox Link is enabled
MB64_CFG1_SNAPSHOT_AT_POWERON     EQU 4 ; if set, a snapshot is sent at poweron
MB64_CFG1_AUTO_SNAPSHOT           EQU 6 ; if set, MIDIbox64 will save/restore a snapshot on bankchanges

;; ==========================================================================

#define BANKSTICK_MAGIC0	0x47 ; magic numbers - if they don't exists in bankstick at BANK_MAGIC_ADDRESS, 
#define BANKSTICK_MAGIC1	0x11 ; the stick will be ignored until it has been formatted

#define BANKSTICK_PATCH_MAGIC0	0xc2 ; the same for patches, here the 7th bit must be set!
#define BANKSTICK_PATCH_MAGIC1	0xd6

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
MB64_MSG_ID_MORPH	EQU	0x00		; show the morph screen
MB64_MSG_ID_BANK	EQU	0x01		; show the bank screen

;; ==========================================================================

	;; free memory for user applications:	
	;; 0x010-0x37f

;; ==========================================================================
;;  General MB64 Control Variables in ACCESS page
;; ==========================================================================
MB_STAT			EQU	0x010
MB_STAT2		EQU	0x011
MB_BANK			EQU	0x012
MB_NEW_BANK		EQU	0x013
MB_PATCH		EQU	0x014

MB_ADDRL		EQU	0x015
MB_ADDRH		EQU	0x016

MIDI_EVNT0		EQU	0x017
MIDI_EVNT1		EQU	0x018
MIDI_EVNT_VALUE		EQU	0x019

;; used by aout.inc
AOUT_SR_NUMBER		EQU	0x01a
AOUT_SR_CTR		EQU	0x01b
AOUT_SR_BIT_CTR		EQU	0x01c
AOUT_SR_L		EQU	0x01d
AOUT_SR_H		EQU	0x01e

;; ==========================================================================
;;  Variables used by the control surface
;; ==========================================================================
CS_STAT			EQU	0x020	; control surface status
CS_MENU_HEADER_TBLPTRL	EQU	0x021	; used by cs_menu.inc
CS_MENU_HEADER_TBLPTRH	EQU	0x022	; used by cs_menu.inc
CS_MENU_ENTRY_TBLPTRL	EQU	0x023	; used by cs_menu.inc
CS_MENU_ENTRY_TBLPTRH	EQU	0x024	; used by cs_menu.inc
CS_MENU_MESSAGE_CTR	EQU	0x025	; handled in cs_menu_timer.inc, used for temporary pot messages
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

;; ==========================================================================

;; used by midi_rxtxt.inc
MIDI_RXTX_RX_CTR	EQU	0x03a
MIDI_RXTX_TX_CTR	EQU	0x03b

;; ==========================================================================

;; used by amux.inc
AMUX_CTR		EQU	0x03c

;; ==========================================================================

LEDDIGITS_CTR		EQU	0x040	; led digits counter, used by leddigits.inc
LEDDIGITS_0		EQU	0x041	; define registers for all available digits
LEDDIGITS_1		EQU	0x042
LEDDIGITS_2		EQU	0x043
LEDDIGITS_3		EQU	0x044
LEDDIGITS_4		EQU	0x045
LEDDIGITS_5		EQU	0x046
LEDDIGITS_6		EQU	0x047
LEDDIGITS_7		EQU	0x048
;; define the number of available digits here - it can be derived from the address
;; this number is only used by the initialization routine and not by the update handler itself
LEDDIGITS_NUMBER	EQU	(LEDDIGITS_7-LEDDIGITS_0) + 1

;; ==========================================================================

;; used by mtc.inc
MTC_STATE		EQU	0x050	; state of received MTC code
MTC_HR			EQU     0x051	; current "hours"
MTC_MN			EQU     0x052	; current "minutes"
MTC_SC			EQU     0x053	; current "seconds"
MTC_FR			EQU     0x054	; current "frames"
MTC_HR_NEW		EQU	0x055	; received "hours"
MTC_MN_NEW		EQU	0x056	; received "minutes"
MTC_SC_NEW		EQU	0x057	; received "seconds"
MTC_FR_NEW		EQU	0x058	; received "frames"

;; used by midiclk.inc
MIDICLK_STATE		EQU	0x059	; state of MIDI clock handler
MIDICLK_BEAT_MSB	EQU     0x05a	; current "beat" (most significant bits))
MIDICLK_BEAT_LSB	EQU     0x05b	; current "beat" (least significant bits))
MIDICLK_QN		EQU     0x05c	; current "quarter note"
MIDICLK_CTR		EQU     0x05d	; current "clock counter"
MIDICLK_POS_LSB		EQU	0x05e	; temporary used to receive the song position
MIDICLK_POS_MSB		EQU	0x05f	; temporary used to receive the song position

;; used by bpm.inc
BPM_CTR			EQU	0x060	; running F8 clock counter
BPM_CTR_REG      	EQU     0x061	; registered counter value
BPM_TIMER_CTR_L 	EQU     0x062	; used as predivider for a 2-second-timer
BPM_TIMER_CTR_H		EQU     0x063	; dito, high byte

;; used by drums.inc
DRUMS_STATE_BEGIN	EQU	0x068	; for 8 drum triggers
DRUMS_STATE_END		EQU	0x06f
DRUMS_MAX_VALUE_BEGIN	EQU	0x060	; for 8 drum triggers
DRUMS_MAX_VALUE_END	EQU	0x067
DRUMS_CTR		EQU	0x068	; used as loop counter


;; ==========================================================================

MB64_BASE		EQU	0x080
MB64_CFG0		EQU	0x080
MB64_CFG1		EQU	0x081
MB64_DEVICE_ID		EQU	0x082
MB64_DISPLAY_PAGE	EQU	0x083
MB64_BANKNAME_BEGIN	EQU	0x084; (16 bytes)
MB64_BANKNAME_END	EQU	0x093
MB64_LED_MAP_BEGIN	EQU	0x094; (8 bytes)
MB64_LED_MAP_END	EQU	0x09b

MB64_CURRENT_POT	EQU	0x09c
MB64_LAST_POT0		EQU	0x09d
MB64_LAST_POT1		EQU	0x09e
MB64_POT_LAST_TURNED_00	EQU	0x09f
MB64_POT_LAST_TURNED_7F	EQU	0x0a0
MB64_POT_MIN_VALUE	EQU	0x0a1
MB64_POT_MAX_VALUE	EQU	0x0a2
MB64_POT_NEW_VALUE	EQU	0x0a3
MB64_POT_LAST_VALUE	EQU	0x0a4
	
MB64_CURRENT_BUTTON	EQU	0x0a5
MB64_LAST_BUTTON0	EQU	0x0a6
MB64_LAST_BUTTON1	EQU	0x0a7

MB64_LEARN_EVNT0	EQU	0x0a8
MB64_LEARN_EVNT1	EQU	0x0a9
MB64_LEARN_EVNT2	EQU	0x0aa
MB64_LEARN_BMODE	EQU	0x0ab

MB64_SYSEX_STATE	EQU	0x0ac
MB64_SYSEX_ACTION	EQU	0x0ad
MB64_SYSEX_IN		EQU	0x0ae
MB64_SYSEX_CHECKSUM	EQU	0x0af

MB64_MORPH_VALUE	EQU	0x0b0
MB64_MORPH_GROUP_SELECT	EQU	0x0b1
MB64_MORPH_POT_GLOBAL	EQU	0x0b2 ; don't change the order of these registers (see cs_m_morph.inc)
MB64_MORPH_POT_G0	EQU	0x0b3 ; don't change the order of these registers (see cs_m_morph.inc)
MB64_MORPH_POT_G1	EQU	0x0b4 ; don't change the order of these registers (see cs_m_morph.inc)
MB64_MORPH_POT_G2	EQU	0x0b5 ; don't change the order of these registers (see cs_m_morph.inc)
MB64_MORPH_POT_G3	EQU	0x0b6 ; don't change the order of these registers (see cs_m_morph.inc)
MB64_MORPH_BANK		EQU	0x0b7 ; don't change the order of these registers (see cs_m_morph.inc)

MB64_CURRENT_ELEMENT	EQU	0x0b8
MB64_CURRENT_ELEMENT_LEN EQU	0x0b9

MB64_GLOBAL_CHANNEL	EQU	0x0ba

MB64_MSG_ID		EQU	0x0bb

MB64_PAGE		EQU	0x0bc

MB64_DATA_BUFFER	EQU	0x0c0	; used for page writes/reads
MB64_DATA_BUFFER_END	EQU	0x0ff	; could also be overlayed with restorable registers (like MB64_POT_EVNT*)

;; -----------------------------------
;; 256 bytes which are temporary used for SysEx uploads
;; the content of this address area will be restored after the upload
MB64_UPLOAD_BUFFER	EQU	0x100
MB64_UPLOAD_BUFFER_END	EQU	0x1ff
;; -----------------------------------

	;; pot events, byte 0
MB64_POT_EVNT0_00	EQU	0x100
	;; ...
MB64_POT_EVNT0_3F	EQU	0x13f

	;; pot events, byte 1
MB64_POT_EVNT1_00	EQU	0x140
	;; ...
MB64_POT_EVNT1_3F	EQU	0x17f

	;; pot values (events, byte 2)
MB64_POT_VALUES_00	EQU	0x180
	;; ...
MB64_POT_VALUES_3F	EQU	0x1bf

	;; last saved pot values (for snap functions)
MB64_POT_LAST_VALUES_00	EQU	0x1c0
	;; ...
MB64_POT_LAST_VALUES_3F	EQU	0x1ff

	;; button events, byte 0
MB64_BUTTON_EVNT0_00	EQU	0x200
	;; ...
MB64_BUTTON_EVNT0_3F	EQU	0x23f

	;; button events, byte 1
MB64_BUTTON_EVNT1_00	EQU	0x240
	;; ...
MB64_BUTTON_EVNT1_3F	EQU	0x27f

	;; button values (packed)
MB64_BUTTON_VALUES_SR0	EQU	0x280
	;; ...
MB64_BUTTON_VALUES_SR7	EQU	0x287

	;; midi button values (packed)
MB64_MBUTTON_VALUES_SR0	EQU	0x288
	;; ...
MB64_MBUTTON_VALUES_SR7	EQU	0x28f

;; ==========================================================================

CSMD_BASE	EQU	0x290 
;;----permanent storage (while page3 is displayed)
CSMD_POT	EQU	0x290	;;previous pot index 0..63
CSMD_ZONE	EQU	0x291	;;previous ZoneIndex 0..7
CSMD_FIELD	EQU	0x292	;;previous field 0..7
;;---temp storage
CSMD_TMP1	EQU	0x293	;;
CSMD_TMP2	EQU	0x294
CSMD_TMP3	EQU	0x295	;;
CSMD_TMP4	EQU	0x296	;;
CSMD_LOOP	EQU	0x297	;;counter for printing all fields 
CSMD_X		EQU	0x298	;;index for pointing to field  
CSMD_POTBASE	EQU	0x299	;;holds POT# of top left pot in zone
