; $Id$
;
; Specific Defines and variables of MIOS Application
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

#define DEFAULT_DEVICE_ID		0x00	; the default device ID

;; ==========================================================================

#define MB_STAT_DONT_FLASH_L_GP_DOUT	0 ; if set, the 8 GP leds on the left side won't be flashed
#define MB_STAT_DONT_FLASH_R_GP_DOUT	1 ; if set, the 8 GP leds on the right side won't be flashed
#define MB_STAT_CALIBRATION_MODE	2 ; if set, the input selected by CV_SELECTED_OUT is in calibration mode

;; ==========================================================================

#define BANKSTICK_MAGIC0	0xf3 ; magic numbers - if they don't exists in bankstick at BANK_MAGIC_ADDRESS, 
#define BANKSTICK_MAGIC1	0x48 ; the stick will be ignored until it has been formatted
	;; note: MAGIC1 is sysex ID for clear seperation from other projects

;; ==========================================================================

#define CV_CFG0_MERGEROFF	0 ; if set, merger is disabled
#define CV_CFG0_MBLINK		1 ; if set, the MIDIbox Link is enabled

;; ==========================================================================

#define CV_CLK_STAT_CLK_REQ	0 ; MIDI clock should be triggered
#define CV_CLK_STAT_RUNNING	1 ; MIDI clock is running

;; ==========================================================================

#define CS_STAT_DISPLAY_INIT_REQ	0	; request a display initialization
#define CS_STAT_DISPLAY_UPDATE_REQ	1	; request a display update
#define CS_STAT_CURSOR_FLASH		2	; handled by cs_menu_timer.inc
#define CS_STAT_WITHIN_PAGE		3	; if 0, show menu, else go into menu page
#define CS_STAT_DELAYED_CALLBACK	4	; if set, the CALLBACK function will be executed after 2 seconds
#define CS_STAT_MESSAGE_REQ		5	; if set, a CS message will be print for some seconds
#define CS_STAT_MESSAGE_HOLD		6	; if set, the CS message will stay active

#define CS_STAT2_FAST_DISPLAY_UPDATE_REQ 0	; for permanent display updates, call more frequently
#define CS_STAT2_ALLOW_FAST_DISP_UPDATE 1    ; allows to display realtime informations
#define CS_STAT2_FAST_DISPLAY_UPDATE    2       ; set by CS menu handler when only realtime informations should be print
#define CS_STAT2_SAVE_STEP1		3       ; step 1 of save function (select target patch)
#define CS_STAT2_SAVE_STEP2		4       ; step 2 of save function (edit name)

;; ==========================================================================

#define CS_MENU_CHARSET_ARROWS		0x01	; common charset
#define CS_MENU_CHARSET_VBARS		0x02	; horizontal bars
#define CS_MENU_CHARSET_HBARS		0x03	; vertical bars

;; ==========================================================================

	;; free memory for user applications:	
	;; 0x010-0x37f

;; ==========================================================================
;;  General MBCV Control Variables in ACCESS page
;; ==========================================================================
MB_STAT			EQU	0x010
MIDI_DEVICE_ID		EQU	0x011
CV_PATCH		EQU	0x012
CV_BANKSTICK_STATUS	EQU	0x013

;; used by midi_evnt.inc
MIDI_EVNT0		EQU	0x014
MIDI_EVNT1		EQU	0x015
MIDI_EVNT_VALUE		EQU	0x016

;; ==========================================================================

;; used by math_mul16_16.inc
MUL_A_L			EQU	0x017
MUL_A_H			EQU	0x018
MUL_B_L			EQU	0x019
MUL_B_H			EQU	0x01a
MUL_R_0			EQU	0x01b
MUL_R_1			EQU	0x01c
MUL_R_2			EQU	0x01d
MUL_R_3			EQU	0x01e

;; ==========================================================================
;;  Variables used by the control surface
;; ==========================================================================
CS_STAT			EQU	0x020	; control surface status
CS_STAT2		EQU	0x021	; control surface status
CS_MENU_HEADER_TBLPTRL	EQU	0x022	; used by cs_menu.inc
CS_MENU_HEADER_TBLPTRH	EQU	0x023	; used by cs_menu.inc
CS_MENU_ENTRY_TBLPTRL	EQU	0x024	; used by cs_menu.inc
CS_MENU_ENTRY_TBLPTRH	EQU	0x025	; used by cs_menu.inc
CS_MENU_MESSAGE_CTR	EQU	0x026	; handled in cs_menu_timer.inc, used for temporary pot messages
CS_MENU_POS		EQU	0x027	; menu position
CS_MENU_NEW_POS		EQU	0x028	; used by the change page function
CS_MENU_ENTRIES		EQU	0x029	; number of menu entries
CS_MENU_TIMER_CTR	EQU	0x02a	; handled by cs_menu_timer.inc, used as predivider
CS_MENU_CURSOR_CTR	EQU	0x02b	; handled by cs_menu_timer.inc, used for flashing cursors
CS_MENU_REFRESH_ITEM_CTR EQU	0x02c	; used by cs_menu.inc to count through the displayed items
CS_MENU_DEL_CALLBACK_CTR EQU	0x02d	; handled by cs_menu_timer.inc
CS_MENU_CHARSET		EQU	0x02e	; used by cs_special_chars.inc
CS_MENU_SAVE_ORIGINAL_PATCH EQU	0x02f	; used in cs_m_display.inc to store original patch number
CS_MENU_PARAMETER_L	EQU	0x030	; contains low-byte of current parameter value, must be located to an even address!!!
CS_MENU_PARAMETER_H	EQU	0x031	; contains high-byte of current parameter value
CS_MENU_PARAMETER_MAX_L	EQU	0x032	; contains max value of current parameter, must be located to an even address!!!
CS_MENU_PARAMETER_MAX_H	EQU	0x033	; contains max value of current parameter
CS_MENU_PARAMETER_HOOK_L EQU	0x034	; contains low-byte of function which will be called after value change
CS_MENU_PARAMETER_HOOK_H EQU	0x035	; contains high-byte of function which will be called after value change
CS_MENU_DEL_CALLBACK_HOOK_L EQU	0x036	; contains low-byte of delayed callback function (e.g. used by save button)
CS_MENU_DEL_CALLBACK_HOOK_H EQU	0x037	; contains high-byte of delayed callback function (e.g. used by save button)
CS_MENU_GP_CALLBACK_HOOK_L EQU	0x038	; contains low-byte of callback function used for general purpose buttons
CS_MENU_GP_CALLBACK_HOOK_H EQU	0x039	; contains high-byte of callback function used for general purpose buttons
CS_MENU_GP_OVERLAY_DOUT_SR0 EQU	0x03a	; contains the LED status of GP DOUT #0
CS_MENU_GP_OVERLAY_DOUT_SR1 EQU	0x03b	; contains the LED status of GP DOUT #1
CS_MENU_PARAMETER_TABLE_L EQU	0x03c	; contains low-byte of parameter table
CS_MENU_PARAMETER_TABLE_H EQU	0x03d	; contains high-byte of parameter table
CS_MENU_CURSOR_POS	EQU	0x03e	; the cursor position within the current menu page
CS_MENU_CURSOR_POS_MAX	EQU	0x03f	; the max. cursor position
CS_MENU_CURSOR_LCD_POS	EQU	0x040	; LCD position of the cursor
CS_MENU_CURSOR_LCD_WIDTH EQU	0x041	; cursor width
CS_MENU_METER_DELAY_CTR	EQU	0x042	; delay counter for meters (cs_menu_timer.inc)
CS_MENU_METER_CTR_BEGIN	EQU	0x043	; meter decrement counter (for mute display, handled by cs_menu_timer.inc)
CS_MENU_METER_CTR_END	EQU	0x04a	; (8 meters for 8 channels)
CS_MENU_SAVE_ORIGINAL_PATCH_NAME EQU 0x04b ; used in cs_m_display to store the original patch name
			;; ... 0x05a    ; 16 chars

;; ==========================================================================

;; gates
GATES			EQU	0x06c
GATES_LAST		EQU	0x06d

;; ==========================================================================

;; for AOUT driver, located in $MIOS_PATH/modules/aout.inc
AOUT_INVERTED		EQU	0x06e
AOUT_UPDATE_REQ		EQU	0x06f  ; ..0x7f (8 * 16bit words)
AOUT_VALUES		EQU	0x070  ; ..0x7f (8 * 16bit words)

;; ==========================================================================

CV_C1_NOTE_STACK_0	EQU	0x080
	;; ...
CV_C1_NOTE_STACK_15	EQU	0x08f
CV_NOTE_STACK_LEN	EQU	CV_C1_NOTE_STACK_15-CV_C1_NOTE_STACK_0 + 1

CV_C2_NOTE_STACK_0	EQU	0x090
CV_C2_NOTE_STACK_15	EQU	0x09f

CV_C3_NOTE_STACK_0	EQU	0x0a0
CV_C3_NOTE_STACK_15	EQU	0x0af

CV_C4_NOTE_STACK_0	EQU	0x0b0
CV_C4_NOTE_STACK_15	EQU	0x0bf

CV_C5_NOTE_STACK_0	EQU	0x0c0
CV_C5_NOTE_STACK_15	EQU	0x0cf

CV_C6_NOTE_STACK_0	EQU	0x0d0
CV_C6_NOTE_STACK_15	EQU	0x0df

CV_C7_NOTE_STACK_0	EQU	0x0e0
CV_C7_NOTE_STACK_15	EQU	0x0ef

CV_C8_NOTE_STACK_0	EQU	0x0f0
CV_C8_NOTE_STACK_15	EQU	0x0ff

;; ==========================================================================

CV_BASE			EQU	0x100
CV_SELECTED_OUT		EQU	0x101	; selected output in menu

CV_SYSEX_STATE		EQU	0x102	; used by cv_sysex.inc
CV_SYSEX_ACTION		EQU	0x103	; used by cv_sysex.inc
CV_SYSEX_IN		EQU	0x104	; used by cv_sysex.inc
CV_SYSEX_CHECKSUM	EQU	0x105	; used by cv_sysex.inc
CV_SYSEX_ADDRESS	EQU	0x106	; used by cv_sysex.inc
CV_SYSEX_PATCH		EQU	0x107	; used by cv_sysex.inc

CV_LAST_RX_0		EQU	0x108	; stores the last received MIDI event
CV_LAST_RX_1		EQU	0x109	; stores the last received MIDI event
CV_LAST_RX_2		EQU	0x10a	; stores the last received MIDI event

CV_CURRENT_CHANNEL	EQU	0x10b	; current channel which will be handled by cv_midi.inc and cv_map.inc
	
CV_AOUT_L		EQU	0x10c	; used by cv_map.inc as temporary storage
CV_AOUT_H		EQU	0x10d	; used by cv_map.inc as temporary storage

CV_CLK_STAT		EQU	0x10e	; used by cv_clk.inc
CV_INCOMING_CLK_CTR	EQU	0x10f	; used by cv_clk.inc
CV_INCOMING_CLK_DELAY	EQU	0x110	; used by cv_clk.inc
CV_SENT_CLK_CTR		EQU	0x111	; used by cv_clk.inc
CV_SENT_CLK_DELAY	EQU	0x112	; used by cv_clk.inc
CV_CLK_REQ_CTR		EQU	0x113	; used by cv_clk.inc
CV_CLK_PULSEWIDTH_CTR	EQU	0x114	; used by cv_clk.inc
CV_CLK_DIVIDER		EQU	0x115	; used by cv_clk.inc

CV_CALIB_VALUE		EQU	0x116	; used in calibration mode (cs_m_calib.inc)

CV_MIDI_MODE		EQU	0x117	; used in cv_midi.inc to temporary store MIDI mode

	;; free: 0x116-0x11f
	
CV_C1_STAT		EQU	0x120	; current status of channel 1
CV_C2_STAT		EQU	0x121	; current status of channel 2
CV_C3_STAT		EQU	0x122	; current status of channel 3
CV_C4_STAT		EQU	0x123	; current status of channel 4
CV_C5_STAT		EQU	0x124	; current status of channel 5
CV_C6_STAT		EQU	0x125	; current status of channel 6
CV_C7_STAT		EQU	0x126	; current status of channel 7
CV_C8_STAT		EQU	0x127	; current status of channel 8

CV_C1_NOTE		EQU	0x128	; current note of channel 1
CV_C2_NOTE		EQU	0x129	; current note of channel 2
CV_C3_NOTE		EQU	0x12a	; current note of channel 3
CV_C4_NOTE		EQU	0x12b	; current note of channel 4
CV_C5_NOTE		EQU	0x12c	; current note of channel 5
CV_C6_NOTE		EQU	0x12d	; current note of channel 6
CV_C7_NOTE		EQU	0x12e	; current note of channel 7
CV_C8_NOTE		EQU	0x12f	; current note of channel 8

CV_C1_VELOCITY		EQU	0x130	; current velocity of channel 1
CV_C2_VELOCITY		EQU	0x131	; current velocity of channel 2
CV_C3_VELOCITY		EQU	0x132	; current velocity of channel 3
CV_C4_VELOCITY		EQU	0x133	; current velocity of channel 4
CV_C5_VELOCITY		EQU	0x134	; current velocity of channel 5
CV_C6_VELOCITY		EQU	0x135	; current velocity of channel 6
CV_C7_VELOCITY		EQU	0x136	; current velocity of channel 7
CV_C8_VELOCITY		EQU	0x137	; current velocity of channel 8

CV_C1_AFTERTOUCH	EQU	0x138	; current aftertouch of channel 1
CV_C2_AFTERTOUCH	EQU	0x139	; current aftertouch of channel 2
CV_C3_AFTERTOUCH	EQU	0x13a	; current aftertouch of channel 3
CV_C4_AFTERTOUCH	EQU	0x13b	; current aftertouch of channel 4
CV_C5_AFTERTOUCH	EQU	0x13c	; current aftertouch of channel 5
CV_C6_AFTERTOUCH	EQU	0x13d	; current aftertouch of channel 6
CV_C7_AFTERTOUCH	EQU	0x13e	; current aftertouch of channel 7
CV_C8_AFTERTOUCH	EQU	0x13f	; current aftertouch of channel 8

CV_C1_CC_L		EQU	0x140	; current CC of channel 1
CV_C2_CC_L		EQU	0x141	; current CC of channel 2
CV_C3_CC_L		EQU	0x142	; current CC of channel 3
CV_C4_CC_L		EQU	0x143	; current CC of channel 4
CV_C5_CC_L		EQU	0x144	; current CC of channel 5
CV_C6_CC_L		EQU	0x145	; current CC of channel 6
CV_C7_CC_L		EQU	0x146	; current CC of channel 7
CV_C8_CC_L		EQU	0x147	; current CC of channel 8

CV_C1_CC_H		EQU	0x148	; current CC of channel 1
CV_C2_CC_H		EQU	0x149	; current CC of channel 2
CV_C3_CC_H		EQU	0x14a	; current CC of channel 3
CV_C4_CC_H		EQU	0x14b	; current CC of channel 4
CV_C5_CC_H		EQU	0x14c	; current CC of channel 5
CV_C6_CC_H		EQU	0x14d	; current CC of channel 6
CV_C7_CC_H		EQU	0x14e	; current CC of channel 7
CV_C8_CC_H		EQU	0x14f	; current CC of channel 8

CV_C1_PITCHBENDER_L	EQU	0x150	; current pitchbender of channel 1
CV_C2_PITCHBENDER_L	EQU	0x151	; current pitchbender of channel 2
CV_C3_PITCHBENDER_L	EQU	0x152	; current pitchbender of channel 3
CV_C4_PITCHBENDER_L	EQU	0x153	; current pitchbender of channel 4
CV_C5_PITCHBENDER_L	EQU	0x154	; current pitchbender of channel 5
CV_C6_PITCHBENDER_L	EQU	0x155	; current pitchbender of channel 6
CV_C7_PITCHBENDER_L	EQU	0x156	; current pitchbender of channel 7
CV_C8_PITCHBENDER_L	EQU	0x157	; current pitchbender of channel 8

CV_C1_PITCHBENDER_H	EQU	0x158	; current pitchbender of channel 1
CV_C2_PITCHBENDER_H	EQU	0x159	; current pitchbender of channel 2
CV_C3_PITCHBENDER_H	EQU	0x15a	; current pitchbender of channel 3
CV_C4_PITCHBENDER_H	EQU	0x15b	; current pitchbender of channel 4
CV_C5_PITCHBENDER_H	EQU	0x15c	; current pitchbender of channel 5
CV_C6_PITCHBENDER_H	EQU	0x15d	; current pitchbender of channel 6
CV_C7_PITCHBENDER_H	EQU	0x15e	; current pitchbender of channel 7
CV_C8_PITCHBENDER_H	EQU	0x15f	; current pitchbender of channel 8

CV_C1_NRPN_LSB		EQU	0x160	; received NRPN LSB of channel 1
CV_C2_NRPN_LSB		EQU	0x161	; received NRPN LSB of channel 2
CV_C3_NRPN_LSB		EQU	0x162	; received NRPN LSB of channel 3
CV_C4_NRPN_LSB		EQU	0x163	; received NRPN LSB of channel 4
CV_C5_NRPN_LSB		EQU	0x164	; received NRPN LSB of channel 5
CV_C6_NRPN_LSB		EQU	0x165	; received NRPN LSB of channel 6
CV_C7_NRPN_LSB		EQU	0x166	; received NRPN LSB of channel 7
CV_C8_NRPN_LSB		EQU	0x167	; received NRPN LSB of channel 8

CV_C1_NRPN_MSB		EQU	0x168	; received NRPN MSB of channel 1
CV_C2_NRPN_MSB		EQU	0x169	; received NRPN MSB of channel 2
CV_C3_NRPN_MSB		EQU	0x16a	; received NRPN MSB of channel 3
CV_C4_NRPN_MSB		EQU	0x16b	; received NRPN MSB of channel 4
CV_C5_NRPN_MSB		EQU	0x16c	; received NRPN MSB of channel 5
CV_C6_NRPN_MSB		EQU	0x16d	; received NRPN MSB of channel 6
CV_C7_NRPN_MSB		EQU	0x16e	; received NRPN MSB of channel 7
CV_C8_NRPN_MSB		EQU	0x16f	; received NRPN MSB of channel 8

;; -----------------------------------

;; free: 0x170..0x17f

;; -----------------------------------

CV_CFG0			EQU	0x180	; merger configuration
CV_CFG1			EQU	0x181	; reserved
CV_GATE_POLARITY	EQU	0x182	; the gate polarity
CV_CLK_DIVIDER_PRELOAD	EQU	0x183	; clock divider preload value
	;; 0x184-0x187: reserved

CV_C1_MIDI_CHANNEL	EQU	0x188	; MIDI channel of channel 1
CV_C2_MIDI_CHANNEL	EQU	0x189	; MIDI channel of channel 2
CV_C3_MIDI_CHANNEL	EQU	0x18a	; MIDI channel of channel 3
CV_C4_MIDI_CHANNEL	EQU	0x18b	; MIDI channel of channel 4
CV_C5_MIDI_CHANNEL	EQU	0x18c	; MIDI channel of channel 5
CV_C6_MIDI_CHANNEL	EQU	0x18d	; MIDI channel of channel 6
CV_C7_MIDI_CHANNEL	EQU	0x18e	; MIDI channel of channel 7
CV_C8_MIDI_CHANNEL	EQU	0x18f	; MIDI channel of channel 8

CV_C1_MIDI_MODE		EQU	0x190	; MIDI mode of channel 1
CV_C2_MIDI_MODE		EQU	0x191	; MIDI mode of channel 2
CV_C3_MIDI_MODE		EQU	0x192	; MIDI mode of channel 3
CV_C4_MIDI_MODE		EQU	0x193	; MIDI mode of channel 4
CV_C5_MIDI_MODE		EQU	0x194	; MIDI mode of channel 5
CV_C6_MIDI_MODE		EQU	0x195	; MIDI mode of channel 6
CV_C7_MIDI_MODE		EQU	0x196	; MIDI mode of channel 7
CV_C8_MIDI_MODE		EQU	0x197	; MIDI mode of channel 8

CV_C1_PITCHRANGE	EQU	0x198	; MIDI pitchrange of channel 1
CV_C2_PITCHRANGE	EQU	0x199	; MIDI pitchrange of channel 2
CV_C3_PITCHRANGE	EQU	0x19a	; MIDI pitchrange of channel 3
CV_C4_PITCHRANGE	EQU	0x19b	; MIDI pitchrange of channel 4
CV_C5_PITCHRANGE	EQU	0x19c	; MIDI pitchrange of channel 5
CV_C6_PITCHRANGE	EQU	0x19d	; MIDI pitchrange of channel 6
CV_C7_PITCHRANGE	EQU	0x19e	; MIDI pitchrange of channel 7
CV_C8_PITCHRANGE	EQU	0x19f	; MIDI pitchrange of channel 8

CV_C1_SPLIT_LOWER	EQU	0x1a0	; Lowest Note of channel 1
CV_C2_SPLIT_LOWER	EQU	0x1a1	; Lowest Note of channel 2
CV_C3_SPLIT_LOWER	EQU	0x1a2	; Lowest Note of channel 3
CV_C4_SPLIT_LOWER	EQU	0x1a3	; Lowest Note of channel 4
CV_C5_SPLIT_LOWER	EQU	0x1a4	; Lowest Note of channel 5
CV_C6_SPLIT_LOWER	EQU	0x1a5	; Lowest Note of channel 6
CV_C7_SPLIT_LOWER	EQU	0x1a6	; Lowest Note of channel 7
CV_C8_SPLIT_LOWER	EQU	0x1a7	; Lowest Note of channel 8

CV_C1_SPLIT_UPPER	EQU	0x1a8	; Highest Note of channel 1
CV_C2_SPLIT_UPPER	EQU	0x1a9	; Highest Note of channel 2
CV_C3_SPLIT_UPPER	EQU	0x1aa	; Highest Note of channel 3
CV_C4_SPLIT_UPPER	EQU	0x1ab	; Highest Note of channel 4
CV_C5_SPLIT_UPPER	EQU	0x1ac	; Highest Note of channel 5
CV_C6_SPLIT_UPPER	EQU	0x1ad	; Highest Note of channel 6
CV_C7_SPLIT_UPPER	EQU	0x1ae	; Highest Note of channel 7
CV_C8_SPLIT_UPPER	EQU	0x1af	; Highest Note of channel 8

CV_C1_TRANSP_OCT	EQU	0x1b0	; transpose octave of channel 1
CV_C2_TRANSP_OCT	EQU	0x1b1	; transpose octave of channel 2
CV_C3_TRANSP_OCT	EQU	0x1b2	; transpose octave of channel 3
CV_C4_TRANSP_OCT	EQU	0x1b3	; transpose octave of channel 4
CV_C5_TRANSP_OCT	EQU	0x1b4	; transpose octave of channel 5
CV_C6_TRANSP_OCT	EQU	0x1b5	; transpose octave of channel 6
CV_C7_TRANSP_OCT	EQU	0x1b6	; transpose octave of channel 7
CV_C8_TRANSP_OCT	EQU	0x1b7	; transpose octave of channel 8

CV_C1_TRANSP_SEM	EQU	0x1b8	; transpose semitones of channel 1
CV_C2_TRANSP_SEM	EQU	0x1b9	; transpose semitones of channel 2
CV_C3_TRANSP_SEM	EQU	0x1ba	; transpose semitones of channel 3
CV_C4_TRANSP_SEM	EQU	0x1bb	; transpose semitones of channel 4
CV_C5_TRANSP_SEM	EQU	0x1bc	; transpose semitones of channel 5
CV_C6_TRANSP_SEM	EQU	0x1bd	; transpose semitones of channel 6
CV_C7_TRANSP_SEM	EQU	0x1be	; transpose semitones of channel 7
CV_C8_TRANSP_SEM	EQU	0x1bf	; transpose semitones of channel 8

CV_C1_CC_NUMBER		EQU	0x1c0	; CC number of channel 1
CV_C2_CC_NUMBER		EQU	0x1c1	; CC number of channel 2
CV_C3_CC_NUMBER		EQU	0x1c2	; CC number of channel 3
CV_C4_CC_NUMBER		EQU	0x1c3	; CC number of channel 4
CV_C5_CC_NUMBER		EQU	0x1c4	; CC number of channel 5
CV_C6_CC_NUMBER		EQU	0x1c5	; CC number of channel 6
CV_C7_CC_NUMBER		EQU	0x1c6	; CC number of channel 7
CV_C8_CC_NUMBER		EQU	0x1c7	; CC number of channel 8

CV_C1_CURVE		EQU	0x1c8	; CV output curve of channel 1
CV_C2_CURVE		EQU	0x1c9	; CV output curve of channel 2
CV_C3_CURVE		EQU	0x1ca	; CV output curve of channel 3
CV_C4_CURVE		EQU	0x1cb	; CV output curve of channel 4
CV_C5_CURVE		EQU	0x1cc	; CV output curve of channel 5
CV_C6_CURVE		EQU	0x1cd	; CV output curve of channel 6
CV_C7_CURVE		EQU	0x1ce	; CV output curve of channel 7
CV_C8_CURVE		EQU	0x1cf	; CV output curve of channel 8

CV_C1_SLEW_RATE		EQU	0x1d0	; CV output Slew Rate of channel 1
CV_C2_SLEW_RATE		EQU	0x1d1	; CV output Slew Rate of channel 2
CV_C3_SLEW_RATE		EQU	0x1d2	; CV output Slew Rate of channel 3
CV_C4_SLEW_RATE		EQU	0x1d3	; CV output Slew Rate of channel 4
CV_C5_SLEW_RATE		EQU	0x1d4	; CV output Slew Rate of channel 5
CV_C6_SLEW_RATE		EQU	0x1d5	; CV output Slew Rate of channel 6
CV_C7_SLEW_RATE		EQU	0x1d6	; CV output Slew Rate of channel 7
CV_C8_SLEW_RATE		EQU	0x1d7	; CV output Slew Rate of channel 8

	;; 0x1d8-0x1ff: reserved

CV_C1_AOUT_L		EQU	0x200	; stored AOUT_L value of channel 1
CV_C2_AOUT_L		EQU	0x201	; stored AOUT_L value of channel 2
CV_C3_AOUT_L		EQU	0x202	; stored AOUT_L value of channel 3
CV_C4_AOUT_L		EQU	0x203	; stored AOUT_L value of channel 4
CV_C5_AOUT_L		EQU	0x204	; stored AOUT_L value of channel 5
CV_C6_AOUT_L		EQU	0x205	; stored AOUT_L value of channel 6
CV_C7_AOUT_L		EQU	0x206	; stored AOUT_L value of channel 7
CV_C8_AOUT_L		EQU	0x207	; stored AOUT_L value of channel 8

CV_C1_AOUT_H		EQU	0x208	; stored AOUT_H value of channel 1
CV_C2_AOUT_H		EQU	0x209	; stored AOUT_H value of channel 2
CV_C3_AOUT_H		EQU	0x20a	; stored AOUT_H value of channel 3
CV_C4_AOUT_H		EQU	0x20b	; stored AOUT_H value of channel 4
CV_C5_AOUT_H		EQU	0x20c	; stored AOUT_H value of channel 5
CV_C6_AOUT_H		EQU	0x20d	; stored AOUT_H value of channel 6
CV_C7_AOUT_H		EQU	0x20e	; stored AOUT_H value of channel 7
CV_C8_AOUT_H		EQU	0x20f	; stored AOUT_H value of channel 8

	;; 0x210-0x26f: reserved

CV_PATCH_NAME0		EQU	0x270	; patch name, first character
;...
CV_PATCH_NAME15		EQU	0x27f	; patch name, last character

;; -----------------------------------

;; free: 0x280..0x37f

;; -----------------------------------

;; ==========================================================================
;;  CV Output Interpolation
;; ==========================================================================
CVIP1_BASE		EQU	0x300
CVIP2_BASE		EQU	0x308
CVIP3_BASE		EQU	0x310
CVIP4_BASE		EQU	0x318
CVIP5_BASE		EQU	0x320
CVIP6_BASE		EQU	0x328
CVIP7_BASE		EQU	0x330
CVIP8_BASE		EQU	0x338

CVIPx_TARGET_VALUE_L	EQU	0x00
CVIPx_TARGET_VALUE_H	EQU	0x01
CVIPx_VALUE_L		EQU	0x02
CVIPx_VALUE_H		EQU	0x03
CVIPx_DIV_VALUE_L	EQU	0x04
CVIPx_DIV_VALUE_H	EQU	0x05
