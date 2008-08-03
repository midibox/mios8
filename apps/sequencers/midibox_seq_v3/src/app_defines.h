; $Id$
;
; Specific Defines and variables of MIOS Application
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; disable message "Operand of HIGH operator was larger than H'FFFF'"
 	errorlevel -311
 
;; ==========================================================================

#define MB_STAT_DONT_FLASH_L_GP_DOUT	0 ; if set, the 8 GP leds on the left side won't be flashed
#define MB_STAT_DONT_FLASH_R_GP_DOUT	1 ; if set, the 8 GP leds on the right side won't be flashed
#define MB_STAT_INFO_IN_SONG_PAGE	2 ; tmp. flag used by SEQ_LCD_PrintInfo
#define MB_STAT_USE_S_BANKSTICK		3 ; if set, the song bankstick will be used by the SEQ_SBANK_ routines
#define MB_STAT_S_BANKSTICK_AVAILABLE	4 ; if set, the song bankstick is available
#define MB_STAT_USE_M_BANKSTICK		5 ; if set, the mixer bankstick will be used by the SEQ_MBANK_ routines
#define MB_STAT_M_BANKSTICK_AVAILABLE	6 ; if set, the mixer map bankstick is available

;; ==========================================================================

#define BANKSTICK_MAGIC0	0x36 ; magic numbers - if they cannot be found in the BankStick
#define BANKSTICK_MAGIC1	0x2b ; at a specific location, the stick will be formatted
                                     ; (note: by changing this number, a format can be forced!
                                     ;  the number should be changed back to the original value
                                     ;  thereafter to ensure compatibility with firmware updates)

#define BANKSTICK_SONG_MAGIC0	0x7a ; the same for songs
#define BANKSTICK_SONG_MAGIC1	0x5c

#define BANKSTICK_MIXER_MAGIC0	0xa3 ; and the same for mixer maps
#define BANKSTICK_MIXER_MAGIC1	0x16

;; ==========================================================================

#define SEQ_CFG0_MERGER		0 ; if set, merger is enabled
#define SEQ_CFG0_T_A_SPLIT	1 ; if set, transpose and arpeggiator are using a splitted keyboard
#define SEQ_CFG0_BPM_CLK_SLAVE	2 ; 0=Master Clock, 1=Slave Clock Mode
#define SEQ_CFG0_BPM_CLK_AUTO	3 ; 1=Auto Master/Slave mode

#define SEQ_CFG0_PATTERN_SYNCH  4 ; if set, pattern changes will be synchronized
#define SEQ_CFG0_FOLLOW_SONG	5 ; if set, the song position in the song page will be not be updated during song is played
#define SEQ_CFG0_PASTE_CLR_ALL	6 ; if set, the paste and copy function will be applied on all track settings (otherwise only on steps)

;; ==========================================================================

#define CS_STAT_DISPLAY_INIT_REQ	0	; request a display initialization
#define CS_STAT_DISPLAY_UPDATE_REQ	1	; request a display update
#define CS_STAT_CURSOR_FLASH		2	; handled by cs_menu_timer.inc
#define CS_STAT_WITHIN_PAGE		3	; if 0, show menu, else go into menu page
#define CS_STAT_DELAYED_CALLBACK	4	; if set, the CALLBACK function will be executed after 2 seconds
#define CS_STAT_MESSAGE_REQ		5	; if set, a CS message will be print for some seconds
#define CS_STAT_MESSAGE_HOLD		6	; if set, the CS message will stay active
#define CS_STAT_SONG_EDIT_BNK		7	; only used by song menu page - could also be moved to another variable later!

#define CS_STAT2_FAST_DISPLAY_UPDATE_REQ 0	; for permanent display updates, call more frequently
#define CS_STAT2_ALLOW_FAST_DISP_UPDATE 1    ; allows to display realtime informations
#define CS_STAT2_FAST_DISPLAY_UPDATE    2       ; set by CS menu handler when only realtime informations should be print
#define CS_STAT2_TRK_CHANGED		3	; used in some loops to notify that a track has been changed
#define CS_STAT2_COPYPASTE_FILLED_P	4	; set if pattern copy/paste buffer is not virgin
#define CS_STAT2_COPYPASTE_FILLED_M	5	; set if mixer copy/paste buffer is not virgin
#define CS_STAT2_UNDO_FILLED		6	; set if undo buffer is not virgin
#define CS_STAT2_ALL_SAME_VALUE		7	; set if the ALL function should set layer steps to same value

;; ==========================================================================

#define CS_MENU_CHARSET_ARROWS		0x01	; common charset
#define CS_MENU_CHARSET_VBARS		0x02	; horizontal bars
#define CS_MENU_CHARSET_HBARS		0x03	; vertical bars

;; ==========================================================================

	;; free memory for user applications:	
	;; 0x010-0x37f
        ;; 0x600-0xf7f (PIC18F4620 only!)

;; ==========================================================================
;;  General MBSEQ Control Variables in ACCESS page
;; ==========================================================================
MB_STAT			EQU	0x010
MB_BS_CHK_CTR		EQU	0x011
MIDI_DEVICE_ID		EQU	0x012

;; used by midi_evnt.inc
MIDI_EVNT_PORT		EQU	0x013
MIDI_EVNT0		EQU	0x014
MIDI_EVNT1		EQU	0x015
MIDI_EVNT_VALUE		EQU	0x016

;; used by midi_rxtx.inc
MIDI_RXTX_RX_CTR	EQU	0x017
MIDI_RXTX_TX_CTR	EQU	0x018

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
CS_MENU_REFRESH_ITEM_CTR EQU	0x02b	; used by cs_menu.inc to count through the displayed items
CS_MENU_DEL_CALLBACK_CTR EQU	0x02c	; handled by cs_menu_timer.inc
CS_MENU_CHARSET		EQU	0x02d	; used by cs_special_chars.inc
CS_MENU_PARAMETER_L	EQU	0x02e	; contains low-byte of current parameter value, must be located to an even address!!!
CS_MENU_PARAMETER_H	EQU	0x02f	; contains high-byte of current parameter value
CS_MENU_PARAMETER_MAX_L	EQU	0x030	; contains max value of current parameter, must be located to an even address!!!
CS_MENU_PARAMETER_MAX_H	EQU	0x031	; contains max value of current parameter
CS_MENU_PARAMETER_HOOK_L EQU	0x032	; contains low-byte of function which will be called after value change
CS_MENU_PARAMETER_HOOK_H EQU	0x033	; contains high-byte of function which will be called after value change
CS_MENU_DEL_CALLBACK_HOOK_L EQU	0x034	; contains low-byte of delayed callback function (e.g. used by save button)
CS_MENU_DEL_CALLBACK_HOOK_H EQU	0x035	; contains high-byte of delayed callback function (e.g. used by save button)
CS_MENU_GP_CALLBACK_HOOK_L EQU	0x036	; contains low-byte of callback function used for general purpose buttons
CS_MENU_GP_CALLBACK_HOOK_H EQU	0x037	; contains high-byte of callback function used for general purpose buttons
CS_MENU_GP_OVERLAY_DOUT_SR0 EQU	0x038	; contains the LED status of GP DOUT #0
CS_MENU_GP_OVERLAY_DOUT_SR1 EQU	0x039	; contains the LED status of GP DOUT #1
CS_MENU_PARAMETER_TABLE_L EQU	0x03a	; contains low-byte of parameter table
CS_MENU_PARAMETER_TABLE_H EQU	0x03b	; contains high-byte of parameter table
CS_MENU_CURSOR_POS	EQU	0x03c	; the cursor position within the current menu page
CS_MENU_CURSOR_POS_MAX	EQU	0x03d	; the max. cursor position
CS_MENU_CURSOR_LCD_POS	EQU	0x03e	; LCD position of the cursor
CS_MENU_CURSOR_LCD_WIDTH EQU	0x03f	; cursor width
CS_MENU_DOUBLECLICK_CTR  EQU    0x040   ; will be decremented until 0 to determine doubleclicks
CS_MENU_DOUBLECLICK_LAST_DIN EQU 0x041  ; stores the last DIN
CS_MENU_METER_DELAY_CTR	EQU	0x042	; delay counter for meters (cs_menu_timer.inc)
CS_MENU_METER_CTR_BEGIN	EQU	0x043	; meter decrement counter (for mute display, handled by cs_menu_timer.inc)
CS_MENU_METER_CTR_END	EQU	0x052	; (16 meters for 16 tracks)
CS_MENU_ENC_INCREMENTER EQU     0x053   ; used by seq_enc.inc and cs_m_*
CS_MENU_CURSOR_POS_OLD	EQU	0x054	; used by seq_enc.inc and cs_m_*
CS_STAT2		EQU	0x055	; additional status variables
CS_ROUTER_SOURCE	EQU	0x056	; used in cs_m_midicfg.inc
CS_ROUTER_TARGET	EQU	0x057	; used in cs_m_midicfg.inc
CS_IN_MENU_MSG		EQU	0x058	; can be used to start a short in-menu message to notify about something when a button has been pressed
CS_COPYPASTE_BEGIN	EQU	0x059	; used in "cs_m_utils.inc"
CS_COPYPASTE_END	EQU	0x05a	; used in "cs_m_utils.inc"
CS_MOVE_ENCODER		EQU	0x05b	; used in "cs_m_utils.inc"
CS_UNDO_TRK		EQU	0x05c	; used in "cs_m_utils.inc"
CS_MIXER_MAP		EQU	0x05d	; used in "cs_m_mixer.inc"
CS_MIXER_PAGE		EQU	0x05e	; used in "cs_m_mixer.inc"
CS_MIXER_SELECTED_TRK	EQU	0x05f	; used in "cs_m_mixer.inc"
CS_RANDOM_GEN_REQ	EQU	0x060	; used in "cs_m_trkrnd.inc"
CS_RANDOM_INTENSITY_LA	EQU	0x061	; used in "cs_m_trkrnd.inc"
CS_RANDOM_INTENSITY_LB	EQU	0x062	; used in "cs_m_trkrnd.inc"
CS_RANDOM_INTENSITY_LC	EQU	0x063	; used in "cs_m_trkrnd.inc"
CS_RANDOM_INTENSITY_TA	EQU	0x064	; used in "cs_m_trkrnd.inc"
CS_RANDOM_INTENSITY_TB	EQU	0x065	; used in "cs_m_trkrnd.inc"
CS_RANDOM_INTENSITY_TC	EQU	0x066	; used in "cs_m_trkrnd.inc"

;; ==========================================================================

PERFORMANCE_LOAD_CTR_L	EQU	0x067	; for seq_load.inc
PERFORMANCE_LOAD_CTR_H	EQU	0x068
PERFORMANCE_LOAD_REG_L	EQU	0x069
PERFORMANCE_LOAD_REG_H	EQU	0x06a
PERFORMANCE_REF_CTR	EQU	0x06b

;; ==========================================================================

	;; free: 0x6c-0x6f

;; ==========================================================================

;; for iic_midi module
_iic_midi_available_slaves EQU	0x070
_iic_midi_slave		EQU	0x071
IIC_MIDI_TMP		EQU	0x072
IIC_MIDI_TX_RETRY_CTR	EQU	0x073
IIC_MIDI_BYTES_TO_SEND	EQU	0x074
_iic_midi_rx_package	EQU	0x075; ..0x78
_iic_midi_tx_package	EQU	0x079; ..0x7c

;; ==========================================================================
MIDI_EVNT_PREV_INTERFACE EQU    0x07d


;; ==========================================================================

;; this 64 byte buffer is used by SEQ_BANK_Copy, SEQ_DUMP_Restore* and SEQ_DUMP_Store* for fast data transfers
SEQ_DATA_BUFFER		EQU	0x080
	;; ...
SEQ_DATA_BUFFER_END	EQU	0x0bf

;; ==========================================================================
;; 8 gate pins
GATES			EQU	0x0c0
GATES_LAST		EQU	0x0c1
	
;; ==========================================================================
;; used by AOUT driver ($MIOS_PATH/modules/aout/)
AOUT_INVERTED		EQU	0x0c2
AOUT_UPDATE_REQ		EQU	0x0c3
;; free: 0xc2-0xc7
AOUT_VALUES		EQU	0x0c8;..0x0d7 (8 * 16bit words)

;; ==========================================================================

BPM_DIGIT_0		EQU	0x0d8
BPM_DIGIT_1		EQU	0x0d9
BPM_DIGIT_2		EQU	0x0da
BPM_DIGIT_3		EQU	0x0db

	;; free: 0xdc-0xdf

;; ==========================================================================

ROUTER_IN0_SRC0_CHN	EQU	0x0e0 ; IIC1
ROUTER_IN0_SRC1_CHN	EQU	0x0e1 ; IIC2
ROUTER_IN0_SRC2_CHN	EQU	0x0e2 ; IIC3
ROUTER_IN0_SRC3_CHN	EQU	0x0e3 ; IIC4
ROUTER_IN0_SRC4_CHN	EQU	0x0e4 ; AOUT
ROUTER_IN0_SRC5_CHN	EQU	0x0e5 ; reserved
ROUTER_IN0_SRC6_CHN	EQU	0x0e6 ; reserved
ROUTER_IN0_SRC7_CHN	EQU	0x0e7 ; reserved

ROUTER_IN0_DST0_CHN	EQU	0x0e8 ; IIC1
ROUTER_IN0_DST1_CHN	EQU	0x0e9 ; IIC2
ROUTER_IN0_DST2_CHN	EQU	0x0ea ; IIC3
ROUTER_IN0_DST3_CHN	EQU	0x0eb ; IIC4
ROUTER_IN0_DST4_CHN	EQU	0x0ec ; AOUT
ROUTER_IN0_DST5_CHN	EQU	0x0ed ; reserved
ROUTER_IN0_DST6_CHN	EQU	0x0ee ; reserved
ROUTER_IN0_DST7_CHN	EQU	0x0ef ; reserved

ROUTER_IN1_SRC0_CHN	EQU	0x0f0 ; IIC1
ROUTER_IN1_SRC1_CHN	EQU	0x0f1 ; IIC2
ROUTER_IN1_SRC2_CHN	EQU	0x0f2 ; IIC3
ROUTER_IN1_SRC3_CHN	EQU	0x0f3 ; IIC4
ROUTER_IN1_SRC4_CHN	EQU	0x0f4 ; AOUT
ROUTER_IN1_SRC5_CHN	EQU	0x0f5 ; reserved
ROUTER_IN1_SRC6_CHN	EQU	0x0f6 ; reserved
ROUTER_IN1_SRC7_CHN	EQU	0x0f7 ; reserved

ROUTER_IN1_DST0_CHN	EQU	0x0f8 ; IIC1
ROUTER_IN1_DST1_CHN	EQU	0x0f9 ; IIC2
ROUTER_IN1_DST2_CHN	EQU	0x0fa ; IIC3
ROUTER_IN1_DST3_CHN	EQU	0x0fb ; IIC4
ROUTER_IN1_DST4_CHN	EQU	0x0fc ; AOUT
ROUTER_IN1_DST5_CHN	EQU	0x0fd ; reserved
ROUTER_IN1_DST6_CHN	EQU	0x0fe ; reserved
ROUTER_IN1_DST7_CHN	EQU	0x0ff ; reserved


;; ==========================================================================

SEQ_BASE		EQU	0x100

SEQ_CFG0		EQU	0x100	; merger configuration
SEQ_MIDI_CHANNEL	EQU	0x101	; channel which is used for transpose/arpeggiator function
SEQ_MIDI_SPLIT_NOTE	EQU	0x102	; split point for transpose/arpeggiator (used when SEQ_CFG0_T_A_SPLIT enabled)
SEQ_PATTERN		EQU	0x103	; selects pattern for SEQ_BANK* functions
SEQ_PATTERN_BANK	EQU	0x104	; selects pattern for SEQ_BANK* functions
SEQ_SBANK_SECTOR	EQU	0x105	; used by SEQ_SBANK to select the sector
SEQ_GROUP		EQU	0x106	; loop counter for track group (pattern 0-3)
SEQ_SELECTED_GROUP	EQU	0x107	; UI: the selected group (0-3, not bitwise selection)
SEQ_SELECTED_LAYERS	EQU	0x108	; UI: bit 2-0: selected layers
SEQ_SELECTED_TRG_LAYERS	EQU	0x109	; UI: bit 2-0: selected button trigger layers
SEQ_SELECTED_TRKS_G0	EQU	0x10a	; UI: bit 3-0: selected tracks of group 0, bit 7-4: used as tmp. storage when sel button pressed
SEQ_SELECTED_TRKS_G1	EQU	0x10b	; UI: bit 3-0: selected tracks of group 1, bit 7-4: used as tmp. storage when sel button pressed
SEQ_SELECTED_TRKS_G2	EQU	0x10c	; UI: bit 3-0: selected tracks of group 2, bit 7-4: used as tmp. storage when sel button pressed
SEQ_SELECTED_TRKS_G3	EQU	0x10d	; UI: bit 3-0: selected tracks of group 3, bit 7-4: used as tmp. storage when sel button pressed
SEQ_SELECTED_STEPS	EQU	0x10e	; UI: bit 0 selects between step 1-16 and 17-32
SEQ_CURRENT_STEP	EQU	0x10f	; the currently selected step (only one a time)
SEQ_EVNTS		EQU	0x110	; selects the step which is used by seq_fsr.inc functions
SEQ_EVNTT		EQU	0x111	; selects the track which is used by seq_core.inc and seq_fsr.inc functions

SEQ_SONG_POS		EQU	0x112	; current song position
SEQ_SONG_LOOP		EQU	0x113	; current loop counter (begins with MAX value, will be decremented)
SEQ_SONG_LOOP_MAX	EQU	0x114	; number of loops

SEQ_REQ			EQU	0x115	; see seq_core.inc
SEQ_STATE		EQU	0x116	; see seq_core.inc
SEQ_CLK_STEP_CTR	EQU	0x117	; step reference counter (0-255)
SEQ_CLK_SUBSTEP_CTR	EQU	0x118	; step reference counter for subticks (0-3)
SEQ_CLK_TICK6_CTR	EQU	0x119	; clock reference counter (0-24)
SEQ_CLK_TICK4_CTR	EQU	0x11a	; clock reference counter (0-24)

SEQ_STEPS_PER_MEASURE   EQU     0x11b   ; used by seq_core.inc (global setting to define number of steps per measure)

SEQ_MODE0		EQU	0x11c	; used by seq_core.inc
SEQ_MODE1		EQU	0x11d	; used by seq_core.inc

SEQ_RANDOM_SEED_L	EQU	0x11e	; current random value used by seq_core.inc
SEQ_RANDOM_SEED_H	EQU	0x11f	; current random value used by seq_core.inc

SEQ_TRKS_MUTED0		EQU	0x120	; muted tracks (low-byte)
SEQ_TRKS_MUTED1		EQU	0x121	; muted tracks (high-byte)

SEQ_BPM			EQU	0x122	; see seq_bpm.inc
SEQ_CLK_REQ_CTR		EQU	0x123	; see seq_bpm.inc
SEQ_INCOMING_CLK_DELAY	EQU	0x124	; see seq_bpm.inc
SEQ_INCOMING_CLK_CTR	EQU	0x125	; see seq_bpm.inc
SEQ_SENT_CLK_DELAY	EQU	0x126	; see seq_bpm.inc
SEQ_SENT_CLK_CTR	EQU	0x127	; see seq_bpm.inc

SEQ_EVNT_NUMBER_CTR	EQU	0x128	; used by seq_core.inc
SEQ_EVNT_NUMBER		EQU	0x129	; used by seq_layer.inc
SEQ_EVNT_CHORD_INDEX	EQU	0x12a	; used by seq_layer.inc
SEQ_EVNT0		EQU	0x12b	; used by seq_layer.inc
SEQ_EVNT1		EQU	0x12c	; used by seq_layer.inc
SEQ_EVNT2		EQU	0x12d	; used by seq_layer.inc
SEQ_EVNTL		EQU	0x12e	; used by seq_layer.inc
SEQ_EVNTL_COPY		EQU	0x12f	; used by seq_core.inc

SEQ_METRONOME_CHANNEL	EQU	0x130	; channel of metronome
SEQ_METRONOME_NOTE_MEASURE EQU	0x131	; played note on measure
SEQ_METRONOME_NOTE_BEAT	EQU	0x132	; played note on beat
SEQ_METRONOME_OFF_EVNT0	EQU	0x133	; stores the metronome OFF event, byte #0
SEQ_METRONOME_OFF_EVNT1	EQU	0x134	; stores the metronome OFF event, byte #1

SEQ_SYSEX_STATE		EQU	0x135	; used by seq_sysex.inc
SEQ_SYSEX_STATE2	EQU	0x136	; used by seq_sysex.inc
SEQ_SYSEX_ACTION	EQU	0x137	; used by seq_sysex.inc
SEQ_SYSEX_IN		EQU	0x138	; used by seq_sysex.inc
SEQ_SYSEX_CHECKSUM	EQU	0x139	; used by seq_sysex.inc
SEQ_SYSEX_ADDRESS_L	EQU	0x13a	; used by seq_sysex.inc
SEQ_SYSEX_ADDRESS_H	EQU	0x13b	; used by seq_sysex.inc
SEQ_SYSEX_TARGET_BANK	EQU	0x13c	; used by seq_sysex.inc
SEQ_SYSEX_TARGET_PATTERN EQU	0x13d	; used by seq_sysex.inc
SEQ_SYSEX_SOURCE	EQU	0x13e	; used by cs_m_sysex.inc

SEQ_MIDIPOS_STATE	EQU	0x13f	; used by the SEQ_MIDI_ClkPosParser
SEQ_MIDIPOS_LSB		EQU	0x140	; used by the SEQ_MIDI_ClkPosParser
SEQ_MIDIPOS_MSB		EQU	0x141	; used by the SEQ_MIDI_ClkPosParser

SEQ_BANKSTICK_CHK_CTR	EQU	0x142	; used in "seq_bank.inc"
SEQ_BANKSTICK_STATUS	EQU	0x143	; used in "seq_bank.inc" (each BankStick has an own available flag)
SEQ_BANKSTICK_SIZE	EQU	0x144	; used in "seq_bank.inc" (each BankStick has a size flag - 0=32k, 1=64k)

SEQ_DISPLAY_PAGE	EQU	0x145	; the selected display page (menu #0)

SEQ_GP_LED_L		EQU	0x146	; status of the general purpose LEDs (left side)
SEQ_GP_LED_R		EQU	0x147	; status of the general purpose LEDs (right side)
SEQ_BLM_ROW_CTR	        EQU	0x148	; used by seq_leds.inc when DEFAULT_TRACK_LEDS_ENABLED set
SEQ_BLM_COL_CTR		EQU	0x149	; used by "seq_button.inc"

SEQ_ENC_INC		EQU	0x14a	; used by seq_enc.inc to save the incrementer

SEQ_LCD_PREVIOUS_EVNTL	EQU	0x14b	; used by SEQ_LCD_PrnPTypeVal_L for perfect length display
SEQ_LCD_PREVIOUS_EVNTL_CTR EQU	0x14c

SEQ_SAVE_PATTERN	EQU	0x14d	; used by CS_M_SAVE* to store the save pattern
SEQ_SAVE_PATTERN_BANK	EQU	0x14e	; used by CS_M_SAVE* to store the save pattern

SEQ_PORTS_MUTED		EQU	0x14f	; used by seq_core.inc

SEQ_REMOTE_CTR		EQU	0x150	; used by "seq_remote.inc"

SEQ_RECORD_MODE		EQU	0x151	; used by "cs_m_record.inc"
SEQ_RECORD_STEP		EQU	0x152	; used by "cs_m_record.inc"
SEQ_RECORD_CURRENT_EVNT1 EQU	0x153	; used by "cs_m_record.inc"
SEQ_RECORD_LENGTH_CTR   EQU     0x154   ; used by "cs_m_record.inc" and "seq_core.inc"
SEQ_RECORD_POLY_CTR     EQU     0x155   ; used by "cs_m_record.inc" and "seq_core.inc"

SEQ_MORPH_CHANNEL	EQU	0x156	; used by "seq_midi.inc" and "cs_m_morph.inc"
SEQ_MORPH_CC		EQU	0x157	; used by "seq_midi.inc" and "cs_m_morph.inc"
SEQ_MORPH_VALUE		EQU	0x158	; used by "seq_midi.inc" and "cs_m_morph.inc"

SEQ_MORPH_EVNT1		EQU	0x159	; used by "seq_morph.inc"
SEQ_MORPH_EVNT2		EQU	0x15a	; used by "seq_morph.inc"
SEQ_MORPH_EVNTL		EQU	0x15b	; used by "seq_morph.inc"
SEQ_MORPH_MIN_VALUE	EQU	0x15c	; used by "seq_morph.inc"
SEQ_MORPH_MAX_VALUE	EQU	0x15d	; used by "seq_morph.inc"

SEQ_CORE_TRK_LASTPOS	EQU	0x15e	; used by "seq_core.inc"

SEQ_CORE_EXT_CLK_DIVIDER EQU	0x15f	; divider for external clock output
SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD EQU 0x160 ; preload value of divider

SEQ_CORE_INT_CLK_DIVIDER EQU	0x161	; divider for internal clock output
SEQ_CORE_INT_CLK_DIVIDER_PRELOAD EQU 0x162 ; preload value of divider

SEQ_SKIP_STEP_PROTECTION_CTR EQU 0x163	; used by "seq_core.inc"

SEQ_LAYER_STORED_EVNTT	EQU	0x164	; used by "seq_layer.inc"
SEQ_LAYER_STORED_EVNTS	EQU	0x165	; used by "seq_layer.inc"
SEQ_LAYER_STORED_PATTERN EQU	0x166	; used by "seq_layer.inc"
SEQ_LAYER_STORED_PATTERN_BANK EQU 0x167	; used by "seq_layer.inc"

SEQ_T_NOTE_STACK_HOLD	EQU	0x168	; holded transpose note

SEQ_GLOBAL_SCALE	EQU	0x169	; used by "seq_scale.inc"
SEQ_GLOBAL_SCALE_CTRL	EQU	0x16a	; used by "seq_scale.inc"
SEQ_GLOBAL_SCALE_KEYB_ROOT EQU	0x16b	; used by "seq_scale.inc"

SEQ_PATTERN_SYNC_TRANSFER_REQ EQU 0x16c	; synchronized transfer request for the patterns (bit 3-0)

SEQ_ROUTER_PORT		EQU	0x16d	; used in "seq_router.inc"
SEQ_ROUTER_MCLK_ENABLED	EQU	0x16e	; used in "seq_router.inc"

SEQ_NRPN_ADDRESS_LSB	EQU	0x16f	; used in "seq_midi.inc"
SEQ_NRPN_ADDRESS_MSB	EQU	0x170	; used in "seq_midi.inc"

SEQ_MIXER_MAP		EQU	0x171	; used in "seq_mbank.inc"

SEQ_PRFTCH_EVNTT	EQU	0x172	; used in "seq_prftch.inc"
SEQ_PRFTCH_EVNTS	EQU	0x173	; used in "seq_prftch.inc"
SEQ_PRFTCH_LAYER	EQU	0x174	; used in "seq_prftch.inc"

;; ==================================================================================

SEQ_SONG		EQU	0x175	; used by CS_M_SONG* only, don't change the order!
SEQ_EDIT_SONG_POS	EQU	0x176
SEQ_EDIT_SONG_ACTN	EQU	0x177
SEQ_EDIT_SONG_G0	EQU	0x178   ; used by "cs_m_song.inc"
SEQ_EDIT_SONG_G1	EQU	0x179
SEQ_EDIT_SONG_G2	EQU	0x17a
SEQ_EDIT_SONG_G3	EQU	0x17b
SEQ_EDIT_SONG_BANK_G0	EQU	0x17c   ; used by "cs_m_song.inc"
SEQ_EDIT_SONG_BANK_G1	EQU	0x17d
SEQ_EDIT_SONG_BANK_G2	EQU	0x17e
SEQ_EDIT_SONG_BANK_G3	EQU	0x17f
SEQ_SONG_RECURSION_CTR	EQU	0x180	; used in "seq_song.inc", SEQ_SONG_FetchPosDirect to avoid a live lock

SEQ_TAPTEMPO_BEAT_CTR	EQU	0x181	; used in "seq_core.inc" for tap tempo function
SEQ_TAPTEMPO_MEAN_L	EQU	0x182	; used in "seq_core.inc" for tap tempo function
SEQ_TAPTEMPO_MEAN_H	EQU	0x183	; used in "seq_core.inc" for tap tempo function
SEQ_TAPTEMPO_AVERAGE0_L	EQU	0x184	; used in "seq_core.inc" for tap tempo function
SEQ_TAPTEMPO_AVERAGE0_H	EQU	0x185	; used in "seq_core.inc" for tap tempo function
SEQ_TAPTEMPO_AVERAGE1_L	EQU	0x186	; used in "seq_core.inc" for tap tempo function
SEQ_TAPTEMPO_AVERAGE1_H	EQU	0x187	; used in "seq_core.inc" for tap tempo function
SEQ_TAPTEMPO_AVERAGE2_L	EQU	0x188	; used in "seq_core.inc" for tap tempo function
SEQ_TAPTEMPO_AVERAGE2_H	EQU	0x189	; used in "seq_core.inc" for tap tempo function
SEQ_TAPTEMPO_AVERAGE3_L	EQU	0x18a	; used in "seq_core.inc" for tap tempo function
SEQ_TAPTEMPO_AVERAGE3_H	EQU	0x18b	; used in "seq_core.inc" for tap tempo function

	;; free: 0x18c-0x19d

;; ==================================================================================

SEQ_IRQ_SAVED_PRODL	EQU	0x19e	; used by IRQ routines to save PRODL
SEQ_IRQ_SAVED_PRODH	EQU	0x19f	; used by IRQ routines to save PRODL

;; ==================================================================================

SEQ_PATTERN0		EQU	0x1a0	; holds the first pattern which is played by seq_core.inc
SEQ_PATTERN1		EQU	0x1a1	; holds the second pattern which is played by seq_core.inc
SEQ_PATTERN2		EQU	0x1a2	; holds the third pattern which is played by seq_core.inc
SEQ_PATTERN3		EQU	0x1a3	; holds the fourth pattern which is played by seq_core.inc

SEQ_PATTERN_BANK0	EQU	0x1a4	; appr. bank to SEQ_PATTERN0
SEQ_PATTERN_BANK1	EQU	0x1a5	; appr. bank to SEQ_PATTERN1
SEQ_PATTERN_BANK2	EQU	0x1a6	; appr. bank to SEQ_PATTERN2
SEQ_PATTERN_BANK3	EQU	0x1a7	; appr. bank to SEQ_PATTERN3

SEQ_NEXT_PATTERN0	EQU	0x1a8	; selects the next SEQ_PATTERN0
SEQ_NEXT_PATTERN1	EQU	0x1a9	; selects the next SEQ_PATTERN1
SEQ_NEXT_PATTERN2	EQU	0x1aa	; selects the next SEQ_PATTERN2
SEQ_NEXT_PATTERN3	EQU	0x1ab	; selects the next SEQ_PATTERN3

SEQ_NEXT_PATTERN_BANK0	EQU	0x1ac	; selects the next SEQ_PATTERN_BANK0
SEQ_NEXT_PATTERN_BANK1	EQU	0x1ad	; selects the next SEQ_PATTERN_BANK1
SEQ_NEXT_PATTERN_BANK2	EQU	0x1ae	; selects the next SEQ_PATTERN_BANK2
SEQ_NEXT_PATTERN_BANK3	EQU	0x1af	; selects the next SEQ_PATTERN_BANK3

SEQ_CHAINHNDSHK_G0	EQU	0x1b0	; used for pattern chaining
SEQ_CHAINHNDSHK_G1	EQU	0x1b1
SEQ_CHAINHNDSHK_G2	EQU	0x1b2
SEQ_CHAINHNDSHK_G3	EQU	0x1b3

SEQ_CHAINHNDSHK_NEXT_G0	EQU	0x1b4	; used for pattern chaining
SEQ_CHAINHNDSHK_NEXT_G1	EQU	0x1b5
SEQ_CHAINHNDSHK_NEXT_G2	EQU	0x1b6
SEQ_CHAINHNDSHK_NEXT_G3	EQU	0x1b7

SEQ_LOOPBACK_STORED_FSR0L EQU	0x1b8	; used by "midi_evnt.inc"
SEQ_LOOPBACK_STORED_FSR0H EQU	0x1b9	; used by "midi_evnt.inc"
SEQ_LOOPBACK_STORED_FSR2L EQU	0x1ba	; used by "midi_evnt.inc"
SEQ_LOOPBACK_STORED_FSR2H EQU	0x1bb	; used by "midi_evnt.inc"
SEQ_LOOPBACK_STORED_EVNTT EQU	0x1bc	; used by "midi_evnt.inc"

	;; free: 0x1bd-0x1bf

SEQ_T_NOTE_STACK_0	EQU	0x1c0	; stack for incoming notes (used by transpose function)
SEQ_T_NOTE_STACK_1	EQU	0x1c1
SEQ_T_NOTE_STACK_2	EQU	0x1c2
SEQ_T_NOTE_STACK_3	EQU	0x1c3
SEQ_T_NOTE_STACK_4	EQU	0x1c4
SEQ_T_NOTE_STACK_5	EQU	0x1c5
SEQ_T_NOTE_STACK_6	EQU	0x1c6
SEQ_T_NOTE_STACK_7	EQU	0x1c7
SEQ_NOTE_STACK_LEN	EQU	(SEQ_T_NOTE_STACK_7-SEQ_T_NOTE_STACK_0)+1

SEQ_A_NOTE_STACK_0	EQU	0x1c8	; stack for incoming notes (used by arpeggiator function)
SEQ_A_NOTE_STACK_1	EQU	0x1c9
SEQ_A_NOTE_STACK_2	EQU	0x1ca
SEQ_A_NOTE_STACK_3	EQU	0x1cb
SEQ_A_NOTE_STACK_4	EQU	0x1cc
SEQ_A_NOTE_STACK_5	EQU	0x1cd
SEQ_A_NOTE_STACK_6	EQU	0x1ce
SEQ_A_NOTE_STACK_7	EQU	0x1cf
	;; SEQ_A_NOTE_STACK_LEN	EQU	(SEQ_A_NOTE_STACK_7-SEQ_A_NOTE_STACK_0)+1
	;; (must be the same like SEQ_NOTE_STACK_LEN !!!)

SEQ_ARP_NOTE_0_SORTED	EQU	0x1d0	; sorted SEQ_A_NOTE_STACK
SEQ_ARP_NOTE_1_SORTED	EQU	0x1d1
SEQ_ARP_NOTE_2_SORTED	EQU	0x1d2
SEQ_ARP_NOTE_3_SORTED	EQU	0x1d3

SEQ_ARP_NOTE_0_SORTED_HOLD	EQU	0x1d4	; the same for hold mode
SEQ_ARP_NOTE_1_SORTED_HOLD	EQU	0x1d5
SEQ_ARP_NOTE_2_SORTED_HOLD	EQU	0x1d6
SEQ_ARP_NOTE_3_SORTED_HOLD	EQU	0x1d7

SEQ_ARP_NOTE_0_UNSORTED	EQU	0x1d8	; unsorted SEQ_A_NOTE_STACK
SEQ_ARP_NOTE_1_UNSORTED	EQU	0x1d9
SEQ_ARP_NOTE_2_UNSORTED	EQU	0x1da
SEQ_ARP_NOTE_3_UNSORTED	EQU	0x1db

SEQ_ARP_NOTE_0_UNSORTED_HOLD	EQU	0x1dc	; the same for hold mode
SEQ_ARP_NOTE_1_UNSORTED_HOLD	EQU	0x1dd
SEQ_ARP_NOTE_2_UNSORTED_HOLD	EQU	0x1de
SEQ_ARP_NOTE_3_UNSORTED_HOLD	EQU	0x1df

SEQ_LEDM_TRK0_A_L	EQU	0x1e0	; used for led matrix - colour1 LEDs
SEQ_LEDM_TRK0_A_R	EQU	0x1e1
SEQ_LEDM_TRK1_A_L	EQU	0x1e2
SEQ_LEDM_TRK1_A_R	EQU	0x1e3
SEQ_LEDM_TRK2_A_L	EQU	0x1e4
SEQ_LEDM_TRK2_A_R	EQU	0x1e5
SEQ_LEDM_TRK3_A_L	EQU	0x1e6
SEQ_LEDM_TRK3_A_R	EQU	0x1e7

SEQ_LEDM_TRK0_B_L	EQU	0x1e8	; used for led matrix - colour2 LEDs
SEQ_LEDM_TRK0_B_R	EQU	0x1e9
SEQ_LEDM_TRK1_B_L	EQU	0x1ea
SEQ_LEDM_TRK1_B_R	EQU	0x1eb
SEQ_LEDM_TRK2_B_L	EQU	0x1ec
SEQ_LEDM_TRK2_B_R	EQU	0x1ed
SEQ_LEDM_TRK3_B_L	EQU	0x1ee
SEQ_LEDM_TRK3_B_R	EQU	0x1ef

SEQ_BUTTONM_ROW0L_VALUES EQU	0x1f0	; used for button matrix - current values
SEQ_BUTTONM_ROW0R_VALUES EQU	0x1f1
SEQ_BUTTONM_ROW1L_VALUES EQU	0x1f2
SEQ_BUTTONM_ROW1R_VALUES EQU	0x1f3
SEQ_BUTTONM_ROW2L_VALUES EQU	0x1f4
SEQ_BUTTONM_ROW2R_VALUES EQU	0x1f5
SEQ_BUTTONM_ROW3L_VALUES EQU	0x1f6
SEQ_BUTTONM_ROW3R_VALUES EQU	0x1f7

SEQ_BUTTONM_ROW0L_CHANGED EQU	0x1f8	; used for button matrix - change flags
SEQ_BUTTONM_ROW0R_CHANGED EQU	0x1f9
SEQ_BUTTONM_ROW1L_CHANGED EQU	0x1fa
SEQ_BUTTONM_ROW1R_CHANGED EQU	0x1fb
SEQ_BUTTONM_ROW2L_CHANGED EQU	0x1fc
SEQ_BUTTONM_ROW2R_CHANGED EQU	0x1fd
SEQ_BUTTONM_ROW3L_CHANGED EQU	0x1fe
SEQ_BUTTONM_ROW3R_CHANGED EQU	0x1ff

;; ==================================================================================

	;; the queued "off events" for track 1-16
	;; note: each track can play up to 4 MIDI events -> 16 * 4 * 3 bytes allocated
	;; indexed with following formula: event byte = SEQ_TRKQUEUEx_BEGIN + 4*SEQ_EVNTT
SEQ_TRKQUEUEP_BEGIN	EQU	0x200
SEQ_TRKQUEUEP_END	EQU	0x23f

SEQ_TRKQUEUE0_BEGIN	EQU	0x240
SEQ_TRKQUEUE0_END	EQU	0x27f

SEQ_TRKQUEUE1_BEGIN	EQU	0x280
SEQ_TRKQUEUE1_END	EQU	0x2bf

;; -----------------------------------

SEQ_STPBUF_MOVED_LAYERA	EQU	0x2d0
SEQ_STPBUF_MOVED_LAYERB	EQU	0x2d1
SEQ_STPBUF_MOVED_LAYERC	EQU	0x2d2
SEQ_STPBUF_MOVED_TRG	EQU	0x2d3
SEQ_STPBUF_OLD_LAYERA	EQU	0x2d4
SEQ_STPBUF_OLD_LAYERB	EQU	0x2d5
SEQ_STPBUF_OLD_LAYERC	EQU	0x2d6
SEQ_STPBUF_OLD_TRG	EQU	0x2d7

;; free: 0x2d8-0x2df

;; -----------------------------------
;; 0x2e0-0x2ff allocated by SEQ_COPYPASTE_CFG !!!

;; -----------------------------------
;; For Undo function
SEQ_UNDO_LAYER_A	EQU	0x300	; ..0x31f
SEQ_UNDO_LAYER_B	EQU	0x320	; ..0x33f
SEQ_UNDO_LAYER_C	EQU	0x340	; ..0x35f
SEQ_UNDO_TRG_LAYERS	EQU	0x360	; ..0x37f

;; alternatively for mixer map
SEQ_MIXER_MAP_BASE	EQU	0x300	; ..0x37f
SEQ_MIXER_MAP_MIDI	EQU	0x300	; ..0x30f
SEQ_MIXER_MAP_PRG	EQU	0x310	; ..0x31f
SEQ_MIXER_MAP_VOL	EQU	0x320	; ..0x32f
SEQ_MIXER_MAP_PAN	EQU	0x330	; ..0x33f
SEQ_MIXER_MAP_CC1	EQU	0x340	; ..0x34f
SEQ_MIXER_MAP_CC2	EQU	0x350	; ..0x35f
SEQ_MIXER_MAP_CC1ASG	EQU	0x360	; ..0x36f
SEQ_MIXER_MAP_CC2ASG	EQU	0x370	; ..0x37f

;; -----------------------------------


;; ==================================================================================
;; up to here RAM is allocated which exists on PIC18F4620 only, and not on PIC18F452!
;; ==================================================================================

;; -----------------------------------
;; layer A (32 steps per track)
SEQ_LAYER_VALUES_TRK0_A	EQU     0x600
SEQ_LAYER_VALUES_TRK1_A	EQU     0x620
SEQ_LAYER_VALUES_TRK2_A	EQU     0x640
SEQ_LAYER_VALUES_TRK3_A	EQU     0x660
SEQ_LAYER_VALUES_TRK4_A	EQU     0x680
SEQ_LAYER_VALUES_TRK5_A	EQU     0x6a0
SEQ_LAYER_VALUES_TRK6_A	EQU     0x6c0
SEQ_LAYER_VALUES_TRK7_A	EQU     0x6e0
SEQ_LAYER_VALUES_TRK8_A	EQU     0x700
SEQ_LAYER_VALUES_TRK9_A	EQU     0x720
SEQ_LAYER_VALUES_TRK10_A EQU     0x740
SEQ_LAYER_VALUES_TRK11_A EQU     0x760
SEQ_LAYER_VALUES_TRK12_A EQU     0x780
SEQ_LAYER_VALUES_TRK13_A EQU     0x7a0
SEQ_LAYER_VALUES_TRK14_A EQU     0x7c0
SEQ_LAYER_VALUES_TRK15_A EQU     0x7e0

;; -----------------------------------
;; layer B (32 steps per track)
SEQ_LAYER_VALUES_TRK0_B	EQU     0x800
SEQ_LAYER_VALUES_TRK1_B	EQU     0x820
SEQ_LAYER_VALUES_TRK2_B	EQU     0x840
SEQ_LAYER_VALUES_TRK3_B	EQU     0x860
SEQ_LAYER_VALUES_TRK4_B	EQU     0x880
SEQ_LAYER_VALUES_TRK5_B	EQU     0x8a0
SEQ_LAYER_VALUES_TRK6_B	EQU     0x8c0
SEQ_LAYER_VALUES_TRK7_B	EQU     0x8e0
SEQ_LAYER_VALUES_TRK8_B	EQU     0x900
SEQ_LAYER_VALUES_TRK9_B	EQU     0x920
SEQ_LAYER_VALUES_TRK10_B EQU     0x940
SEQ_LAYER_VALUES_TRK11_B EQU     0x960
SEQ_LAYER_VALUES_TRK12_B EQU     0x980
SEQ_LAYER_VALUES_TRK13_B EQU     0x9a0
SEQ_LAYER_VALUES_TRK14_B EQU     0x9c0
SEQ_LAYER_VALUES_TRK15_B EQU     0x9e0

;; -----------------------------------
;; layer C (32 steps per track)
SEQ_LAYER_VALUES_TRK0_C	EQU     0xa00
SEQ_LAYER_VALUES_TRK1_C	EQU     0xa20
SEQ_LAYER_VALUES_TRK2_C	EQU     0xa40
SEQ_LAYER_VALUES_TRK3_C	EQU     0xa60
SEQ_LAYER_VALUES_TRK4_C	EQU     0xa80
SEQ_LAYER_VALUES_TRK5_C	EQU     0xaa0
SEQ_LAYER_VALUES_TRK6_C	EQU     0xac0
SEQ_LAYER_VALUES_TRK7_C	EQU     0xae0
SEQ_LAYER_VALUES_TRK8_C	EQU     0xb00
SEQ_LAYER_VALUES_TRK9_C	EQU     0xb20
SEQ_LAYER_VALUES_TRK10_C EQU     0xb40
SEQ_LAYER_VALUES_TRK11_C EQU     0xb60
SEQ_LAYER_VALUES_TRK12_C EQU     0xb80
SEQ_LAYER_VALUES_TRK13_C EQU     0xba0
SEQ_LAYER_VALUES_TRK14_C EQU     0xbc0
SEQ_LAYER_VALUES_TRK15_C EQU     0xbe0

;; -----------------------------------
;;  track record structure
SEQ_TRKCHAINx		EQU     0x00	; stored in track 1 only
SEQ_TRKMORPH_PATTERNx	EQU     0x00	; stored in track 2 only
SEQ_TRKSCALEx		EQU     0x00	; stored in track 3 only
SEQ_TRKSCALEROOTx	EQU     0x00	; stored in track 4 only (bit [7:4] used, bit [3:0] free)

SEQ_TRKMODEx		EQU     0x01	; track mode
SEQ_TRKEVNTx		EQU     0x02	; first MIDI byte
SEQ_TRKEVNTCONST1x	EQU	0x03	; event constant value #1
SEQ_TRKEVNTCONST2x	EQU	0x04	; event constant value #2
SEQ_TRKEVNTCONST3x	EQU	0x05	; event constant value #3
SEQ_TRKCHNx		EQU     0x06	; MIDI channel and port
SEQ_TRKDIR1x		EQU	0x07	; track direction (mode and replay)
SEQ_TRKDIR2x		EQU	0x08	; track direction (steps fwd and jump back)
SEQ_TRKDIVx		EQU     0x09	; clock divider
SEQ_TRKLENx		EQU     0x0a	; track length
SEQ_TRKLOOPx		EQU	0x0b	; loop point
SEQ_TRKTRANSPx		EQU     0x0c	; octave/semitones transpose value
SEQ_TRKSPARE1x		EQU     0x0d	; spare byte for future extensions
SEQ_TRKGROOVEx		EQU     0x0e	; groove mode and intensity
SEQ_TRKMORPHx		EQU	0x0f	; morph mode
SEQ_TRKASSGN0x		EQU	0x10	; assignments to gate/skip/acc/glide
SEQ_TRKASSGN1x		EQU     0x11	; assignments to roll/R.G/R.V
SEQ_TRKHUMANIZEx	EQU	0x12	; humanize mode and intensity
SEQ_TRKSPARE2x		EQU     0x13	; spare byte for future extensions

SEQ_TRKTRGA_0x		EQU     0x14	; step trigger control A
SEQ_TRKTRGA_1x		EQU     0x15
SEQ_TRKTRGA_2x		EQU     0x16
SEQ_TRKTRGA_3x		EQU     0x17
SEQ_TRKTRGB_0x		EQU     0x18	; step trigger control B
SEQ_TRKTRGB_1x		EQU     0x19
SEQ_TRKTRGB_2x		EQU     0x1a
SEQ_TRKTRGB_3x		EQU     0x1b
SEQ_TRKTRGC_0x		EQU     0x1c	; step trigger control C
SEQ_TRKTRGC_1x		EQU     0x1d
SEQ_TRKTRGC_2x		EQU     0x1e
SEQ_TRKTRGC_3x		EQU     0x1f

;; used to select the track record (pointer = SEQ_TRK0 + track_number*SEQ_TRKRECORD_LENGTH)
SEQ_TRKRECORD_LENGTH    EQU     0x20

;; -----------------------------------
SEQ_TRK0		EQU	0xc00
SEQ_TRK1		EQU	SEQ_TRK0 + 1 * SEQ_TRKRECORD_LENGTH	; 0xc20
SEQ_TRK2		EQU	SEQ_TRK0 + 2 * SEQ_TRKRECORD_LENGTH	; 0xc40
SEQ_TRK3		EQU	SEQ_TRK0 + 3 * SEQ_TRKRECORD_LENGTH	; 0xc60
SEQ_TRK4		EQU	SEQ_TRK0 + 4 * SEQ_TRKRECORD_LENGTH	; 0xc80
SEQ_TRK5		EQU	SEQ_TRK0 + 5 * SEQ_TRKRECORD_LENGTH	; 0xca0
SEQ_TRK6		EQU	SEQ_TRK0 + 6 * SEQ_TRKRECORD_LENGTH	; 0xcc0
SEQ_TRK7		EQU	SEQ_TRK0 + 7 * SEQ_TRKRECORD_LENGTH	; 0xce0
SEQ_TRK8		EQU	SEQ_TRK0 + 8 * SEQ_TRKRECORD_LENGTH	; 0xd00
SEQ_TRK9		EQU	SEQ_TRK0 + 9 * SEQ_TRKRECORD_LENGTH	; 0xd20
SEQ_TRK10		EQU	SEQ_TRK0 + 10 * SEQ_TRKRECORD_LENGTH	; 0xd40
SEQ_TRK11		EQU	SEQ_TRK0 + 11 * SEQ_TRKRECORD_LENGTH	; 0xd60
SEQ_TRK12		EQU	SEQ_TRK0 + 12 * SEQ_TRKRECORD_LENGTH	; 0xd80
SEQ_TRK13		EQU	SEQ_TRK0 + 13 * SEQ_TRKRECORD_LENGTH	; 0xda0
SEQ_TRK14		EQU	SEQ_TRK0 + 14 * SEQ_TRKRECORD_LENGTH	; 0xdc0
SEQ_TRK15		EQU	SEQ_TRK0 + 15 * SEQ_TRKRECORD_LENGTH	; 0xde0
SEQ_TRK_END		EQU	SEQ_TRK0 + 16 * SEQ_TRKRECORD_LENGTH-1	; 0xdff

;; -----------------------------------
;;  track variables record structure
SEQ_TRKVARSTATEx	EQU     0x00	; flags: see seq_core.inc (SEQ_TRKVARSTATE_*)
SEQ_TRKVARSTATE2x	EQU	0x01	; flags: see seq_core.inc (SEQ_TRKVARSTATE2_*)
SEQ_TRKVARSTEPx		EQU     0x02	; the track position
SEQ_TRKVARDIVCTRx	EQU     0x03	; the clock divider counter
SEQ_TRKVARPLYTICKx	EQU     0x04	; number of MIDI clock ticks until next step will be played
SEQ_TRKVARSTEPREPLYx	EQU	0x05	; step replay counter
SEQ_TRKVARSTEPSAVEDx	EQU	0x06	; saved step (for replay mechanism)
SEQ_TRKVARSTEPFWDx	EQU	0x07	; step forward counter
SEQ_TRKVARQUEUEL_Lx	EQU     0x08	; length counter ("note off" once it is zero)
SEQ_TRKVARQUEUEL_Hx	EQU     0x09	; length counter, high byte
SEQ_TRKVARRETRGx	EQU	0x0a	; the retrigger for trioles
SEQ_TRKVARRETRGRELOADx	EQU	0x0b	; reload value for retrigger counter
SEQ_TRKVARARPPOSx	EQU	0x0c	; arp position counter (for multi arg events)

;; used to select the track record (pointer = SEQ_TRKVAR0 + track_number*SEQ_TRKVARRECORD_LENGTH)
SEQ_TRKVARRECORD_LENGTH    EQU     0x10

;; -----------------------------------
SEQ_TRKVAR0		EQU	0xe00
SEQ_TRKVAR1		EQU	SEQ_TRKVAR0 + 1 * SEQ_TRKVARRECORD_LENGTH	; 0xe10
SEQ_TRKVAR2		EQU	SEQ_TRKVAR0 + 2 * SEQ_TRKVARRECORD_LENGTH	; 0xe20
SEQ_TRKVAR3		EQU	SEQ_TRKVAR0 + 3 * SEQ_TRKVARRECORD_LENGTH	; 0xe30
SEQ_TRKVAR4		EQU	SEQ_TRKVAR0 + 4 * SEQ_TRKVARRECORD_LENGTH	; 0xe40
SEQ_TRKVAR5		EQU	SEQ_TRKVAR0 + 5 * SEQ_TRKVARRECORD_LENGTH	; 0xe50
SEQ_TRKVAR6		EQU	SEQ_TRKVAR0 + 6 * SEQ_TRKVARRECORD_LENGTH	; 0xe60
SEQ_TRKVAR7		EQU	SEQ_TRKVAR0 + 7 * SEQ_TRKVARRECORD_LENGTH	; 0xe70
SEQ_TRKVAR8		EQU	SEQ_TRKVAR0 + 8 * SEQ_TRKVARRECORD_LENGTH	; 0xe80
SEQ_TRKVAR9		EQU	SEQ_TRKVAR0 + 9 * SEQ_TRKVARRECORD_LENGTH	; 0xe90
SEQ_TRKVAR10		EQU	SEQ_TRKVAR0 + 10 * SEQ_TRKVARRECORD_LENGTH	; 0xea0
SEQ_TRKVAR11		EQU	SEQ_TRKVAR0 + 11 * SEQ_TRKVARRECORD_LENGTH	; 0xeb0
SEQ_TRKVAR12		EQU	SEQ_TRKVAR0 + 12 * SEQ_TRKVARRECORD_LENGTH	; 0xec0
SEQ_TRKVAR13		EQU	SEQ_TRKVAR0 + 13 * SEQ_TRKVARRECORD_LENGTH	; 0xed0
SEQ_TRKVAR14		EQU	SEQ_TRKVAR0 + 14 * SEQ_TRKVARRECORD_LENGTH	; 0xee0
SEQ_TRKVAR15		EQU	SEQ_TRKVAR0 + 15 * SEQ_TRKVARRECORD_LENGTH	; 0xef0
SEQ_TRKVAR_END		EQU	SEQ_TRKVAR0 + 16 * SEQ_TRKVARRECORD_LENGTH-1	; 0xeff


;; -----------------------------------
;; For Copy&Paste function
SEQ_COPYPASTE_LAYER_A	EQU	0xf00	; ..0xf1f
SEQ_COPYPASTE_LAYER_B	EQU	0xf20	; ..0xf3f
SEQ_COPYPASTE_LAYER_C	EQU	0xf40	; ..0xf5f
SEQ_COPYPASTE_TRG_LAYERS EQU	0xf60	; ..0xf7f
SEQ_COPYPASTE_CFG	EQU	0x2e0	; ..0x2ff - note the special address range! It has been marked as reserved above!

;; also used by mixer
SEQ_COPYPASTE_MIXER	EQU	0xf00	; ..0xf7f

