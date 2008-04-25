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

#define SID_STAT_ENGINE_DISABLE		0 ; if set, SID tick and SID interrupt will not be processed
#define SID_STAT_ENGINE_DISABLE_LEVEL0	1 ; same as flag 0, but used during startup to overcome a wdt timeout issue when no LCD is connected
#define SID_STAT_SIDPLAYER		2 ; exclusive access for SID player
#define SID_STAT_SIDPLAYER_CS_DISABLE	3 ; set if CS buttons/encoders should be ignored
#define SID_STAT_CLK_SLAVE		4 ; if set, clock generator in slave mode (flag should be outside erasable SID_BASE structure)
#define SID_STAT_SE_CYCLE		5 ; switches between first and second engine cycle

;; ==========================================================================

	;; patch
#define BANKSTICK_MAGIC0	0xf3 ; magic numbers - if they don't exists in bankstick at BANK_MAGIC_ADDRESS, 
#define BANKSTICK_MAGIC1	0x8b ; the stick will be ignored until it has been formatted

	;; ensemble
#define BANKSTICK_E_MAGIC0	0xa9 ; this name has been selected intentionally ;)
#define BANKSTICK_E_MAGIC1	0x4e

;; ==========================================================================

#define CS_STAT_SID_AVAILABLE		0	; flag 0: for MBNET: edits not possible when slave not available
#define CS_STAT_DISPLAY_INIT_REQ	1	; flag 1: request a display initialization
#define CS_STAT_DISPLAY_UPDATE_REQ	2	; flag 2: request a display update
#define CS_STAT_CURSOR_FLASH		3	; flag 3: handled by cs_menu_timer.inc
#define CS_STAT_MODIFY_PARAMETER	4	; flag 4: modify parameter if set, else select parameter
#define CS_STAT_MODIFY_NAME		5	; flag 5: modify name if set, else modify parameter
#define CS_STAT_PAGE_MSG		6	; flag 6: the page message is print on screen - don't update screen
#define CS_STAT_BIG_MSG			7	; flag 7: e.g. for temporary print parameter name

#define CS_STAT2_WT_DEC_VIEW		0	; flag 0: decimal view in WTC and WTE page
#define CS_STAT2_EDIT_UPDATE_REQ	1	; flag 1: requests edit buffer update
#define CS_STAT2_COPY_BUFFER_FILLED	2	; flag 2: set if copy buffer filled
#define CS_STAT2_LEVEL_METERS		3	; flag 3: show level meters on LED matrix
#define CS_STAT2_PLAY_SID1		4	; SIDx/Menu button combination: toggles play state
#define CS_STAT2_PLAY_SID2		5	; DON'T CHANGE THE BIT NUMBERS!!!
#define CS_STAT2_PLAY_SID3		6	; 
#define CS_STAT2_PLAY_SID4		7	;

#define CS_STAT3_MODIFY_SEQ		0	; flag 0: edit drum sequence
#define CS_STAT3_MODIFY_SEQ_ROTATE	1	; flag 1: activate rotate function in drum editor
#define CS_STAT3_SHIFT_PAGE		2	; flag 2: show SHIFT page

#define CS_MENU_MODE_SHIFT_PRESSED	0	; flag 0: Shift Button pressed
#define CS_MENU_MODE_CC			1	; flag 1: CC mode
#define CS_MENU_MODE_EDIT		2	; flag 2: edit mode
#define	CS_MENU_MODE_EDIT_NOTIFIER	3	; flag 3: notifies when any value has been changed (can also be mapped to a LED)
#define	CS_MENU_MODE_EDIT_FLASHED	4	; flag 4: the flashed version of the edit button
#define CS_MENU_MODE_MATRIX_METER	5	; flag 5: alternative meter mode for modulation sources
#define CS_MENU_MODE_MATRIX_METER_DISP	6	; flag 6: used in cs_menu_matrix.inc to decide if meters should be displayed
#define CS_MENU_MODE_MENU_PRESSED	7	; flag 7: Menu Button pressed


;; ==========================================================================

;; ==========================================================================
;;  General SID Control Variables in ACCESS page
;;  These variables are not cleared on a patch change!
;; ==========================================================================
SID_STAT		EQU	0x010
SID_ENSEMBLE		EQU	0x011
SID_MIDI_DEVICE		EQU	0x012
SID_PATCH		EQU	0x013
SID_BANK		EQU	0x014
SID_BANKSTICK_CHK_CTR	EQU	0x015
SID_BANKSTICK_STATUS	EQU	0x016	; 8 flags for 8 BankSticks, 1=available
SID_BANKSTICK_SIZE	EQU	0x017	; 8 flags for 8 BankSticks, 0=32k, 1=64k

;; ==========================================================================
;;  Variables used by the control surface
;; ==========================================================================
CS_MENU_PARAMETER_L	EQU	0x018	; contains low-byte of current parameter value, must be located to an even address!!!
CS_MENU_PARAMETER_H	EQU	0x019	; contains high-byte of current parameter value
CS_MENU_PARAMETER_MAX_L	EQU	0x01a	; contains max value of current parameter, must be located to an even address!!!
CS_MENU_PARAMETER_MAX_H	EQU	0x01b	; contains max value of current parameter
CS_STAT			EQU	0x01c	; control surface status
CS_STAT2		EQU	0x01d	; control surface status
CS_STAT3		EQU	0x01e	; control surface status
CS_MENU			EQU	0x01f	; menu page
CS_MENU_TABLE_0_L	EQU	0x020	; pointer to first table entry, low-byte
CS_MENU_TABLE_0_H	EQU	0x021	; pointer to first table entry, high-byte
CS_MENU_PAGE_OFFSET	EQU	0x022	; page offset to first displayed entry
CS_MENU_CURSOR_POS	EQU	0x023	; cursor position
CS_MENU_ENTRIES		EQU	0x024	; number of menu entries
CS_MENU_TIMER_CTR	EQU	0x025	; used by cs_menu_timer.inc
CS_CURSOR_CTR		EQU	0x026	; used by cs_menu_timer.inc
CS_SELECT_CTR		EQU	0x027	; used by cs_menu.inc, counted by cs_menu_timer.inc
CS_MENU_REFRESH_ITEM_CTR EQU	0x028	; used by cs_menu.inc to count through the displayed items
CS_MENU_ENTRY_TBLPTRL	EQU	0x029	; used by cs_menu.inc
CS_MENU_ENTRY_TBLPTRH	EQU	0x02a	; used by cs_menu.inc
CS_MENU_MODE		EQU	0x02b	; control mode flags (Link/CC/Edit mode)
CS_MENU_WT_POS		EQU	0x02c	; contains the current WT pos
CS_MENU_WT_SEQ		EQU	0x02d	; contains the current WT sequence (used in bassline/drum mode)
CS_MENU_SAVE_ENS	EQU	0x02e	; contains the ensemble number for save function
CS_MENU_SAVE_PATCH	EQU	0x02f	; contains the patch number for save function
CS_MENU_SAVE_BANK	EQU	0x030	; contains the bank number for save function
CS_MENU_REINIT_CFG_CTR	EQU	0x031	; reinit configuration if counter == 1
CS_MENU_NAME_POS	EQU	0x032	; for the "edit name" mode
CS_MENU_SID		EQU	0x033	; number of SID which is currently handled
CS_MENU_SID_M_PATCH	EQU	0x034	; patch number of master	;(don't change the order of these 4 registers)
CS_MENU_SID_S1_PATCH	EQU	0x035	; patch number of first slave
CS_MENU_SID_S2_PATCH	EQU	0x036	; patch number of second slave
CS_MENU_SID_S3_PATCH	EQU	0x037	; patch number of third slave
CS_MENU_SID_M_BANK	EQU	0x038	; bank number of master	        ;(don't change the order of these 4 registers)
CS_MENU_SID_S1_BANK	EQU	0x039	; bank number of first slave
CS_MENU_SID_S2_BANK	EQU	0x03a	; bank number of second slave
CS_MENU_SID_S3_BANK	EQU	0x03b	; bank number of third slave
CS_MENU_SID_M_CHN	EQU	0x03c	; MIDI channel of master	;(don't change the order of these 4 registers)
CS_MENU_SID_S1_CHN	EQU	0x03d	; MIDI channel of first slave
CS_MENU_SID_S2_CHN	EQU	0x03e	; MIDI channel of second slave
CS_MENU_SID_S3_CHN	EQU	0x03f	; MIDI channel of third slave
CS_MENU_TX_M_CTR	EQU	0x040	; transmit counter master, if > 0, patch tx will be delayed
CS_MENU_TX_S1_CTR	EQU	0x041	; transmit counter master, if > 0, patch tx will be delayed
CS_MENU_TX_S2_CTR	EQU	0x042	; transmit counter master, if > 0, patch tx will be delayed
CS_MENU_TX_S3_CTR	EQU	0x043	; transmit counter master, if > 0, patch tx will be delayed
CS_MENU_SELECTED_SID_FLAGS EQU  0x044	; every flag stays for one SID
CS_MENU_SELECTED_SID_LR	EQU	0x045	; selected SID L/R
CS_MENU_SELECTED_INS	EQU	0x046	; the selected ensemble
CS_MENU_SELECTED_OSC	EQU	0x047	; the selected oscillator
CS_MENU_SELECTED_OSC_FLAGS EQU	0x048	; every flag stays for one oscillator
CS_MENU_SELECTED_OSC_CTRL EQU	0x049	; selects Env/Misc and Assign
CS_MENU_SELECTED_LFO	EQU	0x04a	; the selected LFO
CS_MENU_SELECTED_ENV	EQU	0x04b	; the selected envelope
CS_MENU_SELECTED_ENV_CTRL EQU	0x04c	; selects Env/Assign
CS_MENU_SELECTED_MOD	EQU	0x04d	; the selected mod entry
CS_MENU_SELECTED_MOD_SRC EQU	0x04e	; the selected mod source entry
CS_MENU_SELECTED_MOD_TARG EQU	0x04f	; the selected mod target entry
CS_MENU_SELECTED_TRG	EQU	0x050	; the selected trigger entry
CS_MENU_SELECTED_TRK	EQU	CS_MENU_SELECTED_TRG	; only for drum engine: selected sequencer track
CS_MENU_SELECTED_KNOB	EQU	0x051	; the selected knob entry
CS_MENU_SELECTED_WT	EQU	0x052	; the selected wavetable
SAVED_CS_MENU		EQU	0x053	; used by cs_menu.inc::CS_MENU_ButtonInc
SAVED_CS_ENGINE		EQU	SAVED_CS_MENU ;  used by cs_menu_buttons.inc::CS_MENU_SID, can be overlayed 
SAVED_CS_MENU_PAGE_OFFSET EQU	0x054	; used by cs_menu.inc::CS_MENU_ButtonInc
SAVED_CS_MENU_CURSOR_POS EQU	0x055	; used by cs_menu.inc::CS_MENU_ButtonInc
CS_MENU_PARAMETER_IX	EQU	0x056	; the parameter index (offset within Ix record) which is modified
CS_MENU_USED_ENCODER	EQU	0x057	; used by cs_menu_enc.inc for FAST function
CS_MENU_ENC_INCREMENTER EQU     0x058   ; allows to determine the incrementer value of the encoder which modified the item
CS_MENU_GET_PATCH_ADDR_L EQU	0x059	; used by cs_menu_mbnet.inc to fetch a patch stepwise
CS_MENU_GET_PATCH_ADDR_H EQU	0x05a	; used by cs_menu_mbnet.inc to fetch a patch stepwise
CS_MENU_MATRIX_CTR	EQU	0x05b	; counter used by CS_MENU_MATRIX_Handler
CS_MENU_MATRIX_FLASH_CTR EQU	0x05c	; counter used by CS_MENU_MATRIX_Handler
CS_MENU_MATRIX_SELECTED_ROW EQU	0x05d	; selected matrix row (every button has one flag)
CS_MENU_MATRIX_SELECTED_COL EQU	0x05e	; selected matrix column (every button has one flag)
CS_MENU_EDIT_BUFFER_SID EQU	0x05f	; reference to SID patch which is stored in edit buffer


;; ==========================================================================
;; NOTE for PIC18F4685: 0x60-0x7f NOT directly accessible via ACCESS bank!
;; ==========================================================================
CS_MENU_MATRIX_BEGIN	EQU	0x060	; content of modulation matrix (8 entries)
CS_MENU_MATRIX_END	EQU	0x067

CS_MENU_LASTPAGE_ENG_0	EQU	0x068	; last selected page of lead engine (used in cs_menu_buttons.inc, see comments there)
CS_MENU_LASTPAGE_ENG_1	EQU	0x069	; last selected page of bassline engine (don't change offset to _0 entry)
CS_MENU_LASTPAGE_ENG_2	EQU	0x06a	; last selected page of drum engine (don't change offset to _0 entry)
CS_MENU_LASTPAGE_ENG_3	EQU	0x06b	; last selected page of multi engine (don't change offset to _0 entry)

SRM_COL_CTR		EQU	0x06c	; used by "cs_menu_matrix.inc" (button handler)

SYX_SID_SELECTION	EQU	0x06d	; used by sid_sysex.inc, we have to store it outside patch range, so that it won't be erased during patch init

	;; free: 0x6e..0x79


TIMER0_RELOAD_L		EQU	0x07a	; used by sid_bpm.inc
TIMER0_RELOAD_H		EQU	0x07b	; used by sid_bpm.inc

SAVED_FSR0L		EQU	0x07c	; used by sid_se.inc
SAVED_FSR0H		EQU	0x07d	; used by sid_se.inc
SAVED_PRODL		EQU	0x07e	; used by sid_se.inc
SAVED_PRODH		EQU	0x07f	; used by sid_se.inc

;; ==========================================================================
;; ensemble edit buffer (for a single SID)
;; only used by CS
SID_ENS_BUFFER		EQU	0x080;..0x0bf

SID_ENSx_BANK		EQU	0x00	; [2:0] selected bank
SID_ENSx_PATCH		EQU	0x01	; [6:0] selected patch
SID_ENSx_CTRL1		EQU	0x02	; [1:0] clock master/slave/auto, [6] mono
SID_ENSx_CTRL2		EQU	0x03	; [0] F2A, [1] V2A, [3] P2A, [4] K2A, [5] O2A
SID_ENSx_RESERVED1	EQU	0x04	; was global clock divider --- not used anymore, replaced by BPM!
SID_ENSx_BPM		EQU	0x05	; global BPM
SID_ENSx_SUPERPOLY_CTRL EQU	0x06    ; [2:0] number of used voices (0..4)
SID_ENSx_AOUT_INVERTED	EQU	0x07	; inversion flags for AOUT Channel 1..8

SID_ENSx_CALI_FIL1_MIN_L EQU	0x08	; filter calibration values
SID_ENSx_CALI_FIL1_MIN_H EQU	0x09	; filter calibration values
SID_ENSx_CALI_FIL1_MAX_L EQU	0x0a	; filter calibration values
SID_ENSx_CALI_FIL1_MAX_H EQU	0x0b	; filter calibration values
SID_ENSx_CALI_FIL2_MIN_L EQU	0x0c	; filter calibration values
SID_ENSx_CALI_FIL2_MIN_H EQU	0x0d	; filter calibration values
SID_ENSx_CALI_FIL2_MAX_L EQU	0x0e	; filter calibration values
SID_ENSx_CALI_FIL2_MAX_H EQU	0x0f	; filter calibration values

SID_ENSx_I1_BASE	EQU	0x10	; base address within SID section of instrument 1
SID_ENSx_I2_BASE	EQU	0x18	; base address within SID section of instrument 2
SID_ENSx_I3_BASE	EQU	0x20	; base address within SID section of instrument 3
SID_ENSx_I4_BASE	EQU	0x28	; base address within SID section of instrument 4
SID_ENSx_I5_BASE	EQU	0x30	; base address within SID section of instrument 5
SID_ENSx_I6_BASE	EQU	0x38	; base address within SID section of instrument 6

SID_ENSx_Ix_RECORD_LEN	EQU	0x08

SID_ENSx_Ix_CHN		EQU	0x00	; [3:0] MIDI channel, [4] disable channel
SID_ENSx_Ix_SPLIT_L	EQU	0x01	; lower split point
SID_ENSx_Ix_SPLIT_U	EQU	0x02	; upper split point
SID_ENSx_Ix_TRN		EQU	0x03	; transpose
SID_ENSx_Ix_RESERVED1	EQU	0x04	; reserved
SID_ENSx_Ix_RESERVED2	EQU	0x05	; reserved
SID_ENSx_Ix_RESERVED3	EQU	0x06	; reserved
SID_ENSx_Ix_RESERVED4	EQU	0x07	; reserved

;; ==========================================================================

;; global ENS settings - reserved RAM area: 0x0c0-0x0cf (0x10 bytes)
;; preloaded from environment

SID_LOCAL_ENS		EQU	0x0c0;..0x0cf
;; offsets identical with SID_ENSx_* from 0x00-0x0f

;; ==========================================================================

BM_ROW0_VALUES		EQU	0x0d0	; used for button matrix - current values
BM_ROW1_VALUES		EQU	0x0d1
BM_ROW2_VALUES		EQU	0x0d2
BM_ROW3_VALUES		EQU	0x0d3
BM_ROW4_VALUES		EQU	0x0d4
BM_ROW5_VALUES		EQU	0x0d5
BM_ROW6_VALUES		EQU	0x0d6
BM_ROW7_VALUES		EQU	0x0d7

BM_ROW0_CHANGED		EQU	0x0d8	; used for button matrix - change flags
BM_ROW1_CHANGED		EQU	0x0d9
BM_ROW2_CHANGED		EQU	0x0da
BM_ROW3_CHANGED		EQU	0x0db
BM_ROW4_CHANGED		EQU	0x0dc
BM_ROW5_CHANGED		EQU	0x0dd
BM_ROW6_CHANGED		EQU	0x0de
BM_ROW7_CHANGED		EQU	0x0df

LM_ROW0_VALUES		EQU	0x0e0	; used for LED matrix - current values
LM_ROW1_VALUES		EQU	0x0e1
LM_ROW2_VALUES		EQU	0x0e2
LM_ROW3_VALUES		EQU	0x0e3
LM_ROW4_VALUES		EQU	0x0e4
LM_ROW5_VALUES		EQU	0x0e5
LM_ROW6_VALUES		EQU	0x0e6
LM_ROW7_VALUES		EQU	0x0e7

;; ==========================================================================

	;; alternative view of mod matrix: meters
METER_VALUES0		EQU	0x0e8
METER_VALUES1		EQU	0x0e9
METER_VALUES2		EQU	0x0ea
METER_VALUES3		EQU	0x0eb
METER_VALUES4		EQU	0x0ec
METER_VALUES5		EQU	0x0ed
METER_VALUES6		EQU	0x0ee
METER_VALUES7		EQU	0x0ef

;; ==========================================================================
	;; SID player (some variables could be shared - SID player works exclusively)

SIDPLAY_STATE		EQU	0x0f0	; used by sidplay.inc: the SysEx state
SIDPLAY_CMD		EQU	0x0f1	; used by sidplay.inc: the SysEx command
SIDPLAY_DATA_CTR	EQU	0x0f2	; used by sidplay.inc: the SysEx Data counter
SIDPLAY_REG_CTR		EQU	0x0f3	; used by sidplay.inc: the SID register counter
SIDPLAY_BUFFER		EQU	0x0f4	; used by sidplay.inc: the mask/msb buffer (8 bytes)
SIDPLAY_BUFFER_END	EQU	0x0fb

;; ==========================================================================

;; used by superpoly mode in sid_midi_l.inc
SID_SUPERPOLY_VOICE_NOTE_TAB EQU 0x0fc  ;  4 bytes used

;; ==========================================================================
;;  a lot of SID control variables in BANKED page
;; ==========================================================================
SID_BASE		EQU	0x100		; address space of registers which are directly accessed via BANKED
SIDSE_CLEAR_BEGIN	EQU	0x100		; used to initialize the whole SIDSE memory
SIDSE_CLEAR_128B_BLOCKS	EQU	5		; number of 128 byte blocks which should be cleared (here: 2 = 0x100..0x37f)

;; ==========================================================================
SID_CURRENT_MIDIVOICE	EQU	0x100	; selects the currently handled MIDI voice
SID_CURRENT_VOICE	EQU	0x101	; selects the currently handled SID voice
SID_CURRENT_VOICE_ASSG	EQU	0x102	; contains the current voice assignment [2:0] and the number of voices [6:4]
SID_CURRENT_CHANNEL	EQU	0x103	; used in sid_midi.inc
SID_MIDI_PARAMETER1	EQU	0x104	; used in sid_midi.inc
SID_MIDI_PARAMETER2	EQU	0x105	; used in sid_midi.inc
SID_MIDI_FLAGS		EQU	0x106	; used in sid_midi.inc
SID_MIDIVOICE_SUSTAIN	EQU	0x107	; used in sid_midi.inc - each voice has a sustain flag, which is controlled via CC#64
SID_PAR_IX		EQU	0x108	; parameter index

;; ==========================================================================
SID_KNOB_NUM		EQU	0x109	; temporary used in sid_knob.inc
SID_KNOB_VALUE_L	EQU	0x10a	; temporary used in sid_knob.inc
SID_KNOB_VALUE_H	EQU	0x10b	; temporary used in sid_knob.inc

;; ==========================================================================
SID_RANDOM_SEED_L	EQU	0x10c	; used in sid_rnd.inc
SID_RANDOM_SEED_H	EQU	0x10d	; used in sid_rnd.inc

;; ==========================================================================
SID_SYSEX_STATE		EQU	0x10e	; used by sid_sysex.inc
SID_SYSEX_STATE2	EQU	0x10f	; used by sid_sysex.inc
;; free: 0x110
SID_SYSEX_IN		EQU	0x111	; used by sid_sysex.inc
SID_SYSEX_CHECKSUM	EQU	0x112	; used by sid_sysex.inc
SID_SYSEX_DATA		EQU	0x113	; used by sid_sysex.inc
SID_SYSEX_ADDRESS_L	EQU	0x114	; used by sid_sysex.inc
SID_SYSEX_ADDRESS_H	EQU	0x115	; used by sid_sysex.inc
SID_SYSEX_TYPE		EQU	0x116	; used by sid_sysex.inc

;; ==========================================================================
MUL_A_L			EQU	0x117	; used by math_mul16_16.inc, should be located in SID_BASE bank
MUL_A_H			EQU	0x118	; note: used by sid_se.inc (interrupt handler)
MUL_B_L			EQU	0x119	; interrupts must be disabled if used in main task
MUL_B_H			EQU	0x11a
MUL_R_0			EQU	0x11b
MUL_R_1			EQU	0x11c
MUL_R_2			EQU	0x11d
MUL_R_3			EQU	0x11e

;; ==========================================================================
SID_SE_STATE		EQU	0x11f	; used in sid_se.inc
SID_SE_ELEMENT_NUM	EQU	0x120	; used in sid_se.inc
SID_SE_FRQ_L		EQU	0x121	; used in sid_se.inc
SID_SE_FRQ_H		EQU	0x122	; used in sid_se.inc
SID_SE_TARGET_FRQ_L	EQU	0x123	; used in sid_se.inc
SID_SE_TARGET_FRQ_H	EQU	0x124	; used in sid_se.inc
SID_SE_TRANSPOSED_NOTE	EQU	0x125	; used in sid_se.inc
SID_SE_MOD_TRANSITION	EQU	0x126	; used in sid_se.inc
SID_SE_MOD_SRC1_L	EQU	0x127	; used in sid_se.inc
SID_SE_MOD_SRC1_H	EQU	0x128	; used in sid_se.inc
SID_SE_MOD_SRC2_L	EQU	0x129	; used in sid_se.inc
SID_SE_MOD_SRC2_H	EQU	0x12a	; used in sid_se.inc
SID_SE_MOD_RES1_L	EQU	0x12b	; used in sid_se.inc
SID_SE_MOD_RES1_H	EQU	0x12c	; used in sid_se.inc
SID_SE_MOD_RES1_U	EQU	0x12d	; used in sid_se.inc
SID_SE_MOD_RES2_L	EQU	0x12e	; used in sid_se.inc
SID_SE_MOD_RES2_H	EQU	0x12f	; used in sid_se.inc
SID_SE_MOD_RES2_U	EQU	0x130	; used in sid_se.inc
SID_SE_LFO_OVERRUN	EQU	0x131	; used in sid_se.inc
SID_SE_GLOBAL_CLK_CTR	EQU	0x132	; used in sid_se.inc
SID_SE_TRG_EVNT_L	EQU	0x133	; used in sid_se.inc
SID_SE_TRG_EVNT_H	EQU	0x134	; used in sid_se.inc
SID_SE_TRG_EVNT_U	EQU	0x135	; used in sid_se.inc
SID_SE_TRG_EVNT_ENVA	EQU	0x136	; used in sid_se_m.inc
SID_SE_TRG_EVNT_ENVR	EQU	0x137	; used in sid_se_m.inc
SID_SE_PHASE_SYNC_REQ	EQU	0x138	; used in sid_se.inc and sid_sr.inc
SID_SE_SR_UPDATE_SYNC	EQU	0x139	; used in sid_se.inc and sid_sr.inc
SID_SE_EXT_ALLOCATED	EQU	0x13a	; used in sid_se.inc

;; ==========================================================================
SID_CLK_REQ_CTR		EQU	0x13b	; used by MIDI clock handler to quadruple clock 
SID_INCOMING_CLK_DELAY	EQU	0x13c	; 
SID_INCOMING_CLK_CTR	EQU	0x13d	; 
SID_SENT_CLK_DELAY	EQU	0x13e	; 
SID_SENT_CLK_CTR	EQU	0x13f	; 

;; ==========================================================================
;;  Modulation Sources
;; ==========================================================================
SID_MOD_SRC_NUM		EQU	30
SID_MOD_SRC_BASE	EQU	0x140

;; lead engine
SID_MOD_SRC_L_ENV1_L	EQU	0x140
SID_MOD_SRC_L_ENV1_H	EQU	0x141
SID_MOD_SRC_L_ENV2_L	EQU	0x142
SID_MOD_SRC_L_ENV2_H	EQU	0x143
SID_MOD_SRC_L_LFO1_L	EQU	0x144
SID_MOD_SRC_L_LFO1_H	EQU	0x145
SID_MOD_SRC_L_LFO2_L	EQU	0x146
SID_MOD_SRC_L_LFO2_H	EQU	0x147
SID_MOD_SRC_L_LFO3_L	EQU	0x148
SID_MOD_SRC_L_LFO3_H	EQU	0x149
SID_MOD_SRC_L_LFO4_L	EQU	0x14a
SID_MOD_SRC_L_LFO4_H	EQU	0x14b
SID_MOD_SRC_L_LFO5_L	EQU	0x14c
SID_MOD_SRC_L_LFO5_H	EQU	0x14d
SID_MOD_SRC_L_LFO6_L	EQU	0x14e
SID_MOD_SRC_L_LFO6_H	EQU	0x14f
SID_MOD_SRC_L_MOD1_L	EQU	0x150
SID_MOD_SRC_L_MOD1_H	EQU	0x151
SID_MOD_SRC_L_MOD2_L	EQU	0x152
SID_MOD_SRC_L_MOD2_H	EQU	0x153
SID_MOD_SRC_L_MOD3_L	EQU	0x154
SID_MOD_SRC_L_MOD3_H	EQU	0x155
SID_MOD_SRC_L_MOD4_L	EQU	0x156
SID_MOD_SRC_L_MOD4_H	EQU	0x157
SID_MOD_SRC_L_MOD5_L	EQU	0x158
SID_MOD_SRC_L_MOD5_H	EQU	0x159
SID_MOD_SRC_L_MOD6_L	EQU	0x15a
SID_MOD_SRC_L_MOD6_H	EQU	0x15b
SID_MOD_SRC_L_MOD7_L	EQU	0x15c
SID_MOD_SRC_L_MOD7_H	EQU	0x15d
SID_MOD_SRC_L_MOD8_L	EQU	0x15e
SID_MOD_SRC_L_MOD8_H	EQU	0x15f
SID_MOD_SRC_L_MDW_L	EQU	0x160
SID_MOD_SRC_L_MDW_H	EQU	0x161
SID_MOD_SRC_L_KEY_L	EQU	0x162
SID_MOD_SRC_L_KEY_H	EQU	0x163
SID_MOD_SRC_L_KNOB1_L	EQU	0x164
SID_MOD_SRC_L_KNOB1_H	EQU	0x165
SID_MOD_SRC_L_KNOB2_L	EQU	0x166
SID_MOD_SRC_L_KNOB2_H	EQU	0x167
SID_MOD_SRC_L_KNOB3_L	EQU	0x168
SID_MOD_SRC_L_KNOB3_H	EQU	0x169
SID_MOD_SRC_L_KNOB4_L	EQU	0x16a
SID_MOD_SRC_L_KNOB4_H	EQU	0x16b
SID_MOD_SRC_L_KNOB5_L	EQU	0x16c
SID_MOD_SRC_L_KNOB5_H	EQU	0x16d
SID_MOD_SRC_L_VEL_L	EQU	0x16e
SID_MOD_SRC_L_VEL_H	EQU	0x16f
SID_MOD_SRC_L_PBN_L	EQU	0x170
SID_MOD_SRC_L_PBN_H	EQU	0x171
SID_MOD_SRC_L_ATH_L	EQU	0x172
SID_MOD_SRC_L_ATH_H	EQU	0x173
SID_MOD_SRC_L_WT1_L	EQU	0x174
SID_MOD_SRC_L_WT1_H	EQU	0x175
SID_MOD_SRC_L_WT2_L	EQU	0x176
SID_MOD_SRC_L_WT2_H	EQU	0x177
SID_MOD_SRC_L_WT3_L	EQU	0x178
SID_MOD_SRC_L_WT3_H	EQU	0x179
SID_MOD_SRC_L_WT4_L	EQU	0x17a
SID_MOD_SRC_L_WT4_H	EQU	0x17b



;; ==========================================================================
;;  Modulation Targets
;; ==========================================================================
SID_MOD_TARG_NUM	EQU	40
SID_MOD_TARG_BASE	EQU	0x180
SID_MOD_TARG_PITCH1_L	EQU	0x180
SID_MOD_TARG_PITCH1_H	EQU	0x181
SID_MOD_TARG_PITCH1_U	EQU	0x182
SID_MOD_TARG_PITCH2_L	EQU	0x183
SID_MOD_TARG_PITCH2_H	EQU	0x184
SID_MOD_TARG_PITCH2_U	EQU	0x185
SID_MOD_TARG_PITCH3_L	EQU	0x186
SID_MOD_TARG_PITCH3_H	EQU	0x187
SID_MOD_TARG_PITCH3_U	EQU	0x188
SID_MOD_TARG_PITCH4_L	EQU	0x189
SID_MOD_TARG_PITCH4_H	EQU	0x18a
SID_MOD_TARG_PITCH4_U	EQU	0x18b
SID_MOD_TARG_PITCH5_L	EQU	0x18c
SID_MOD_TARG_PITCH5_H	EQU	0x18d
SID_MOD_TARG_PITCH5_U	EQU	0x18e
SID_MOD_TARG_PITCH6_L	EQU	0x18f
SID_MOD_TARG_PITCH6_H	EQU	0x190
SID_MOD_TARG_PITCH6_U	EQU	0x191
SID_MOD_TARG_PW1_L	EQU	0x192
SID_MOD_TARG_PW1_H	EQU	0x193
SID_MOD_TARG_PW1_U	EQU	0x194
SID_MOD_TARG_PW2_L	EQU	0x195
SID_MOD_TARG_PW2_H	EQU	0x196
SID_MOD_TARG_PW2_U	EQU	0x197
SID_MOD_TARG_PW3_L	EQU	0x198
SID_MOD_TARG_PW3_H	EQU	0x199
SID_MOD_TARG_PW3_U	EQU	0x19a
SID_MOD_TARG_PW4_L	EQU	0x19b
SID_MOD_TARG_PW4_H	EQU	0x19c
SID_MOD_TARG_PW4_U	EQU	0x19d
SID_MOD_TARG_PW5_L	EQU	0x19e
SID_MOD_TARG_PW5_H	EQU	0x19f
SID_MOD_TARG_PW5_U	EQU	0x1a0
SID_MOD_TARG_PW6_L	EQU	0x1a1
SID_MOD_TARG_PW6_H	EQU	0x1a2
SID_MOD_TARG_PW6_U	EQU	0x1a3
SID_MOD_TARG_FIL1_L	EQU	0x1a4
SID_MOD_TARG_FIL1_H	EQU	0x1a5
SID_MOD_TARG_FIL1_U	EQU	0x1a6
SID_MOD_TARG_FIL2_L	EQU	0x1a7
SID_MOD_TARG_FIL2_H	EQU	0x1a8
SID_MOD_TARG_FIL2_U	EQU	0x1a9
SID_MOD_TARG_VOL1_L	EQU	0x1aa
SID_MOD_TARG_VOL1_H	EQU	0x1ab
SID_MOD_TARG_VOL1_U	EQU	0x1ac
SID_MOD_TARG_VOL2_L	EQU	0x1ad
SID_MOD_TARG_VOL2_H	EQU	0x1ae
SID_MOD_TARG_VOL2_U	EQU	0x1af
SID_MOD_TARG_LD1_L	EQU	0x1b0
SID_MOD_TARG_LD1_H	EQU	0x1b1
SID_MOD_TARG_LD1_U	EQU	0x1b2
SID_MOD_TARG_LD2_L	EQU	0x1b3
SID_MOD_TARG_LD2_H	EQU	0x1b4
SID_MOD_TARG_LD2_U	EQU	0x1b5
SID_MOD_TARG_LD3_L	EQU	0x1b6
SID_MOD_TARG_LD3_H	EQU	0x1b7
SID_MOD_TARG_LD3_U	EQU	0x1b8
SID_MOD_TARG_LD4_L	EQU	0x1b9
SID_MOD_TARG_LD4_H	EQU	0x1ba
SID_MOD_TARG_LD4_U	EQU	0x1bb
SID_MOD_TARG_LD5_L	EQU	0x1bc
SID_MOD_TARG_LD5_H	EQU	0x1bd
SID_MOD_TARG_LD5_U	EQU	0x1be
SID_MOD_TARG_LD6_L	EQU	0x1bf
SID_MOD_TARG_LD6_H	EQU	0x1c0
SID_MOD_TARG_LD6_U	EQU	0x1c1
SID_MOD_TARG_LR1_L	EQU	0x1c2
SID_MOD_TARG_LR1_H	EQU	0x1c3
SID_MOD_TARG_LR1_U	EQU	0x1c4
SID_MOD_TARG_LR2_L	EQU	0x1c5
SID_MOD_TARG_LR2_H	EQU	0x1c6
SID_MOD_TARG_LR2_U	EQU	0x1c7
SID_MOD_TARG_LR3_L	EQU	0x1c8
SID_MOD_TARG_LR3_H	EQU	0x1c9
SID_MOD_TARG_LR3_U	EQU	0x1ca
SID_MOD_TARG_LR4_L	EQU	0x1cb
SID_MOD_TARG_LR4_H	EQU	0x1cc
SID_MOD_TARG_LR4_U	EQU	0x1cd
SID_MOD_TARG_LR5_L	EQU	0x1ce
SID_MOD_TARG_LR5_H	EQU	0x1cf
SID_MOD_TARG_LR5_U	EQU	0x1d0
SID_MOD_TARG_LR6_L	EQU	0x1d1
SID_MOD_TARG_LR6_H	EQU	0x1d2
SID_MOD_TARG_LR6_U	EQU	0x1d3
SID_MOD_TARG_EXT1_L	EQU	0x1d4
SID_MOD_TARG_EXT1_H	EQU	0x1d5
SID_MOD_TARG_EXT1_U	EQU	0x1d6
SID_MOD_TARG_EXT2_L	EQU	0x1d7
SID_MOD_TARG_EXT2_H	EQU	0x1d8
SID_MOD_TARG_EXT2_U	EQU	0x1d9
SID_MOD_TARG_EXT3_L	EQU	0x1da
SID_MOD_TARG_EXT3_H	EQU	0x1db
SID_MOD_TARG_EXT3_U	EQU	0x1dc
SID_MOD_TARG_EXT4_L	EQU	0x1dd
SID_MOD_TARG_EXT4_H	EQU	0x1de
SID_MOD_TARG_EXT4_U	EQU	0x1df
SID_MOD_TARG_EXT5_L	EQU	0x1e0
SID_MOD_TARG_EXT5_H	EQU	0x1e1
SID_MOD_TARG_EXT5_U	EQU	0x1e2
SID_MOD_TARG_EXT6_L	EQU	0x1e3
SID_MOD_TARG_EXT6_H	EQU	0x1e4
SID_MOD_TARG_EXT6_U	EQU	0x1e5
SID_MOD_TARG_EXT7_L	EQU	0x1e6
SID_MOD_TARG_EXT7_H	EQU	0x1e7
SID_MOD_TARG_EXT7_U	EQU	0x1e8
SID_MOD_TARG_EXT8_L	EQU	0x1e9
SID_MOD_TARG_EXT8_H	EQU	0x1ea
SID_MOD_TARG_EXT8_U	EQU	0x1eb
SID_MOD_TARG_WT1_L	EQU	0x1ec
SID_MOD_TARG_WT1_H	EQU	0x1ed
SID_MOD_TARG_WT1_U	EQU	0x1ee
SID_MOD_TARG_WT2_L	EQU	0x1ef
SID_MOD_TARG_WT2_H	EQU	0x1f0
SID_MOD_TARG_WT2_U	EQU	0x1f1
SID_MOD_TARG_WT3_L	EQU	0x1f2
SID_MOD_TARG_WT3_H	EQU	0x1f3
SID_MOD_TARG_WT3_U	EQU	0x1f4
SID_MOD_TARG_WT4_L	EQU	0x1f5
SID_MOD_TARG_WT4_H	EQU	0x1f6
SID_MOD_TARG_WT4_U	EQU	0x1f7

;; define memory area which should be cleared before calculating new results
SID_MOD_TARG_CLEAR_BEGIN EQU	SID_MOD_TARG_BASE
SID_MOD_TARG_CLEAR_END	 EQU	0x1ff

;; ==========================================================================
;;  Voice records
;; ==========================================================================
SID_Vx_NUM		EQU	6
SID_Vx_RECORD_LEN	EQU	0x18

SID_Vx_ASSIGNED_MV	EQU	0x00
SID_Vx_STATE		EQU	0x01
SID_Vx_STATE2		EQU	0x02
SID_Vx_NOTE		EQU	0x03
SID_Vx_ARP_NOTE		EQU	0x04
SID_Vx_PLAYED_NOTE	EQU	0x05
SID_Vx_TRANSP_NOTE	EQU	0x06
SID_Vx_OLD_TRANSP_NOTE	EQU	0x07
SID_Vx_PITCHBENDER	EQU	0x08
SID_Vx_TARGET_FRQ_L	EQU	0x09
SID_Vx_TARGET_FRQ_H	EQU	0x0a
SID_Vx_OLD_TARGET_FRQ_L	EQU	0x0b
SID_Vx_OLD_TARGET_FRQ_H	EQU	0x0c
SID_Vx_PORTA_FRQ_L	EQU	0x0d
SID_Vx_PORTA_FRQ_H	EQU	0x0e
SID_Vx_PORTA_CTR_L	EQU	0x0f
SID_Vx_PORTA_CTR_H	EQU	0x10
SID_Vx_SET_DELAY_CTR_L	EQU	0x11
SID_Vx_SET_DELAY_CTR_H	EQU	0x12
SID_Vx_CLR_DELAY_CTR_L	EQU	0x13
SID_Vx_CLR_DELAY_CTR_H	EQU	0x14
	;; free offsets: 0x15-0x17 (take care: 0x14 temporary allocated by SID_Vx_D_MODEL_PAR3)

	;; drum engine uses some of these registers on a different way
SID_Vx_DRUM		EQU	SID_Vx_NOTE		; drum number
SID_Vx_D_MODEL_TBLPTRL	EQU	SID_Vx_PORTA_FRQ_L	; pointer to drum model (-> sid_dmodel.inc)
SID_Vx_D_MODEL_TBLPTRH	EQU	SID_Vx_PORTA_FRQ_H
SID_Vx_D_MODEL_TYPE	EQU	SID_Vx_PORTA_CTR_L
SID_Vx_D_MODEL_BASENOTE	EQU	SID_Vx_PLAYED_NOTE
SID_Vx_D_MODEL_GATELENGTH EQU	SID_Vx_PORTA_CTR_H
SID_Vx_D_MODEL_WAVEFORM	EQU	SID_Vx_OLD_TARGET_FRQ_L
SID_Vx_D_MODEL_PULSEWIDTH EQU	SID_Vx_OLD_TARGET_FRQ_H
SID_Vx_D_MODEL_WT_SPEED EQU	SID_Vx_ARP_NOTE
SID_Vx_D_MODEL_WT_END	EQU	SID_Vx_TRANSP_NOTE
SID_Vx_D_MODEL_WT_LOOP	EQU	SID_Vx_OLD_TRANSP_NOTE
SID_Vx_D_MODEL_PAR3	EQU	0x15; temporary in voice array


SIDL_V1_BASE		EQU	0x200;..0x217
SIDL_V2_BASE		EQU	0x218;..0x22f
SIDL_V3_BASE		EQU	0x230;..0x247
SIDR_V1_BASE		EQU	0x248;..0x25f
SIDR_V2_BASE		EQU	0x260;..0x277
SIDR_V3_BASE		EQU	0x278;..0x29f

;; ==========================================================================
;;  LFO records
;; ==========================================================================
SID_LFOx_NUM		EQU	12
SID_LFOx_RECORD_LEN	EQU	4

SID_LFOx_CTR_L		EQU	0x00	; MUST be located at offset 0
SID_LFOx_CTR_H		EQU	0x01	; MUST be located at offset 1
SID_LFOx_DELAY_CTR_L	EQU	0x02
SID_LFOx_DELAY_CTR_H	EQU	0x03

SID_LFO1_BASE		EQU	0x2a0;..0x2a3
SID_LFO2_BASE		EQU	0x2a4;..0x2a7
SID_LFO3_BASE		EQU	0x2a8;..0x2ab
SID_LFO4_BASE		EQU	0x2ac;..0x2af
SID_LFO5_BASE		EQU	0x2b0;..0x2b3
SID_LFO6_BASE		EQU	0x2b4;..0x2b7
SID_LFO7_BASE		EQU	0x2b8;..0x2bb
SID_LFO8_BASE		EQU	0x2bc;..0x2bf
SID_LFO9_BASE		EQU	0x2c0;..0x2c3
SID_LFO10_BASE		EQU	0x2c4;..0x2c7
SID_LFO11_BASE		EQU	0x2c8;..0x2cb
SID_LFO12_BASE		EQU	0x2cc;..0x2cf

;; ==========================================================================
;;  Envelope records
;; ==========================================================================
SID_ENVx_NUM		EQU	6	; (prepared for multi engine)
SID_ENVx_RECORD_LEN	EQU	6

SID_ENVx_CTR_L		EQU	0x00	; MUST be located at offset 0
SID_ENVx_CTR_H		EQU	0x01	; MUST be located at offset 1
SID_ENVx_DELAY_CTR_L	EQU	0x02
SID_ENVx_DELAY_CTR_H	EQU	0x03
SID_ENVx_STATE		EQU	0x04

SID_ENV1_BASE		EQU	0x2d0;..0x2d4
SID_ENV2_BASE		EQU	0x2d5;..0x2d9
SID_ENV3_BASE		EQU	0x2da;..0x2de
SID_ENV4_BASE		EQU	0x2df;..0x2e3
SID_ENV5_BASE		EQU	0x2e4;..0x2e8
SID_ENV6_BASE		EQU	0x2e9;..0x2ed

;; ==========================================================================
;;  Wavetable records
;; ==========================================================================
SID_WTx_NUM		EQU	6
SID_WTx_RECORD_LEN	EQU	2

SID_WTx_POS		EQU	0x00
SID_WTx_DIV_CTR		EQU	0x01

SID_WT1_BASE		EQU	0x2ee;..0x2ef
SID_WT2_BASE		EQU	0x2f0;..0x2f1
SID_WT3_BASE		EQU	0x2f2;..0x2f3
SID_WT4_BASE		EQU	0x2f4;..0x2f5
SID_WT5_BASE		EQU	0x2f6;..0x2f7
SID_WT6_BASE		EQU	0x2f8;..0x2f9

;; ==========================================================================
;;  Bassline/Drum Sequencer records
;; ==========================================================================
SID_SEQx_NUM		EQU	2
SID_SEQx_RECORD_LEN	EQU	3

SID_SEQx_POS		EQU	0x00
SID_SEQx_DIV_CTR	EQU	0x01
SID_SEQx_MISC		EQU	0x02	; [2:0] subctr, [3] SEQ On, [4] sequencer running

SID_SEQ1_BASE		EQU	0x2ea;..0x2fc
SID_SEQ2_BASE		EQU	0x2fd;..0x2ff


;; ==========================================================================
;;  SID L/R register file + shadow (used in sid_sr.inc)
;; ==========================================================================
SIDL_BASE		EQU	0x300;..0x31f
SIDL_SHADOW_BASE	EQU	0x320;..0x33f
SIDR_BASE		EQU	0x340;..0x35f
SIDR_SHADOW_BASE	EQU	0x360;..0x37f

	;; SID register offsets
SIDx_V1_FRQ_L		EQU	0x00
SIDx_V1_FRQ_H		EQU	0x01
SIDx_V1_PW_L		EQU	0x02
SIDx_V1_PW_H		EQU	0x03
SIDx_V1_CTRL		EQU	0x04
SIDx_V1_ENV_AD		EQU	0x05
SIDx_V1_ENV_SR		EQU	0x06

SIDx_V2_FRQ_L		EQU	0x07
SIDx_V2_FRQ_H		EQU	0x08
SIDx_V2_PW_L		EQU	0x09
SIDx_V2_PW_H		EQU	0x0a
SIDx_V2_CTRL		EQU	0x0b
SIDx_V2_ENV_AD		EQU	0x0c
SIDx_V2_ENV_SR		EQU	0x0d

SIDx_V3_FRQ_L		EQU	0x0e
SIDx_V3_FRQ_H		EQU	0x0f
SIDx_V3_PW_L		EQU	0x10
SIDx_V3_PW_H		EQU	0x11
SIDx_V3_CTRL		EQU	0x12
SIDx_V3_ENV_AD		EQU	0x13
SIDx_V3_ENV_SR		EQU	0x14

SIDx_FC_L		EQU	0x15
SIDx_FC_H		EQU	0x16
SIDx_RES_FCHN		EQU	0x17
SIDx_MODE_VOL		EQU	0x18

SIDx_SWINSID_V1_PHASE	EQU	0x1a
SIDx_SWINSID_V2_PHASE	EQU	0x1b
SIDx_SWINSID_V3_PHASE	EQU	0x1c
SIDx_SWINSID_V1_MODE	EQU	0x1d
SIDx_SWINSID_V2_MODE	EQU	0x1e
SIDx_SWINSID_V3_MODE	EQU	0x1f


;; ==========================================================================
;;  AIN RAM area not used
;; ==========================================================================

;; ==========================================================================
;;  Sequencer Copy/Paste Buffer
;;  (located in AIN RAM area - we have our own AIN handler)
;; ==========================================================================
CS_COPY_PASTE_BUFFER_BEGIN	EQU	0x380	; 32 bytes
CS_COPY_PASTE_BUFFER_END	EQU	0x39f

;; ==========================================================================
;;  NRPN arrays (each MIDI channel has an own entry)
;; ==========================================================================
NRPN_ADDRESS_LSB		EQU	0x3a0	; ..0x3af for all 16 channels
NRPN_ADDRESS_MSB		EQU	0x3b0	; ..0x3bf for all 16 channels
NRPN_DATA_LSB			EQU	0x3c0	; ..0x3cf for all 16 channels

;; ==========================================================================
;;  Filter Interpolation
;; ==========================================================================
FIP1_BASE		EQU	0x3d0 ; - 0x3d5
FIP2_BASE		EQU	0x3d6 ; - 0x3dd

FIPx_TARGET_VALUE_L	EQU	0x00
FIPx_TARGET_VALUE_H	EQU	0x01
FIPx_VALUE_L		EQU	0x02
FIPx_VALUE_H		EQU	0x03
FIPx_DIV_VALUE_L	EQU	0x04
FIPx_DIV_VALUE_H	EQU	0x05

;; ==========================================================================
	;; free: 0x3de-0x3e2

;; ==========================================================================
;;  AIN registers
;;  (located in AIN RAM area - we have our own AIN handler)
;; ==========================================================================
AIN_INPUT_CTR		EQU	0x3e0
AIN_INPUT_CHANGED	EQU	0x3e1
AIN_INPUT_CHANGED_TMP	EQU	0x3e2
AIN_VALUE_0		EQU	0x3e3
AIN_VALUE_1		EQU	0x3e4
AIN_VALUE_2		EQU	0x3e5
AIN_VALUE_3		EQU	0x3e6
AIN_VALUE_4		EQU	0x3e7
AIN_VALUE_5		EQU	0x3e8
AIN_VALUE_6		EQU	0x3e9
AIN_VALUE_7		EQU	0x3ea

;; ==========================================================================
;;  Gate registers
;;  (located in AIN RAM area - we have our own AIN handler)
;; ==========================================================================
GATES			EQU	0x3eb
GATES_LAST		EQU	0x3ec

;; ==========================================================================
;;  AOUT registers (used in $MIOS_PATH/modules/aout.inc)
;;  (located in AIN RAM area - we have our own AIN handler)
;; ==========================================================================
AOUT_INVERTED		EQU	0x3ee
AOUT_UPDATE_REQ		EQU	0x3ef
AOUT_VALUES		EQU	0x3f0  ; ..0x37f (8 * 16bit words)

;; ==========================================================================
;;  Patch Record Structure
;;  Note: on each byte/bit assignment change, the tables under
;;  doc/mbsidv2_sysex_implementation.txt have to be adapted as well!
;; ==========================================================================

SID_Ix_NAME_x		EQU	0x000; 0x00-0x0f

;; === general parameters, structure identical for each engine ===
SID_Ix_ENGINE		EQU	0x010	; [1:0] engine (Lead/Bassline/Drum/Multi), [7:2]: reserved
SID_Ix_HW_FLAGS		EQU	0x011	; hardware info flags: [1:0] 0=no SID restriction, 1=6581, 2=6582/8580, 3=SwinSID; [3]: stereo, [7:4] caps: 0=470pF, 1=1nF, 2=2.2nF, 3=4.7nF, 4=6.8nF, 5=10nF, 6=22nF, 7=47nF, 8=100nF
SID_Ix_OPT1_FLAGS	EQU	0x012	; [0] ABW, [7:1] reserved for future engine options
SID_Ix_OPT2_FLAGS	EQU	0x013	; reserved for future engine options

SID_Ix_CUSTOM_SW	EQU	0x014	; custom switch flags #1-8
	;; free: 0x015-0x17

SID_Ix_Px_ASSIGN1	EQU	0x00	; Knob Parameter assignment #1
SID_Ix_Px_ASSIGN2	EQU	0x01	; Knob Parameter assignment #2
SID_Ix_Px_VALUE		EQU	0x02	; Knob initial value
SID_Ix_Px_MIN		EQU	0x03	; Knob min value
SID_Ix_Px_MAX		EQU	0x04	; Knob max value
SID_Ix_P_K1_BASE	EQU	0x018;-0x1c	Knob #1
SID_Ix_P_K2_BASE	EQU	0x01d;-0x21	Knob #2
SID_Ix_P_K3_BASE	EQU	0x022;-0x26	Knob #3
SID_Ix_P_K4_BASE	EQU	0x027;-0x2b	Knob #4
SID_Ix_P_K5_BASE	EQU	0x02c;-0x30	Knob #5
SID_Ix_P_KV_BASE	EQU	0x031;-0x35	Velocity
SID_Ix_P_KP_BASE	EQU	0x036;-0x3a	Pitch Bender
SID_Ix_P_KA_BASE	EQU	0x03b;-0x3f     Aftertouch

SID_Ix_EXT_PAR1_L	EQU	0x040	; external parameter #1 (e.g. for AOUT)
SID_Ix_EXT_PAR1_H	EQU	0x041
SID_Ix_EXT_PAR2_L	EQU	0x042	; external parameter #2
SID_Ix_EXT_PAR2_H	EQU	0x043
SID_Ix_EXT_PAR3_L	EQU	0x044	; external parameter #3
SID_Ix_EXT_PAR3_H	EQU	0x045
SID_Ix_EXT_PAR4_L	EQU	0x046	; external parameter #4
SID_Ix_EXT_PAR4_H	EQU	0x047
SID_Ix_EXT_PAR5_L	EQU	0x048	; external parameter #5
SID_Ix_EXT_PAR5_H	EQU	0x049
SID_Ix_EXT_PAR6_L	EQU	0x04a	; external parameter #6
SID_Ix_EXT_PAR6_H	EQU	0x04b
SID_Ix_EXT_PAR7_L	EQU	0x04c	; external parameter #7
SID_Ix_EXT_PAR7_H	EQU	0x04d
SID_Ix_EXT_PAR8_L	EQU	0x04e	; external parameter #8
SID_Ix_EXT_PAR8_H	EQU	0x04f

;; === engine specific parameters ===

;; --- Lead ---

;; general flags
SID_Ix_L_FLAGS1		EQU	0x050	; mono/legato [0], WTO [1], SusKey [2]
SID_Ix_L_OSC_DETUNE	EQU	0x051	; detune left/right oscillators
SID_Ix_L_VOLUME		EQU	0x052	; 7 bit value, only 4 bit used
SID_Ix_L_OSC_PHASE	EQU	0x053   ; oscillator phase offset

;; filter sections (uses SID_Ix_L_Fx_* offsets)
SID_Ix_L_S1F_BASE	EQU	0x054;..0x059
SID_Ix_L_S2F_BASE	EQU	0x05a;..0x05f

;; oscillator sections (uses SID_Ix_Vx_* offsets)
SID_Ix_L_S1V1_BASE	EQU	0x060;..0x06f    SID1 Voice1
SID_Ix_L_S1V2_BASE	EQU	0x070;..0x07f    SID1 Voice2
SID_Ix_L_S1V3_BASE	EQU	0x080;..0x08f    SID1 Voice3
SID_Ix_L_S2V1_BASE	EQU	0x090;..0x09f    SID2 Voice1
SID_Ix_L_S2V2_BASE	EQU	0x0a0;..0x0af    SID2 Voice2
SID_Ix_L_S2V3_BASE	EQU	0x0b0;..0x0bf    SID2 Voice3

;; LFO sections (uses SID_Ix_LFOx_* offsets)
SID_Ix_L_LFO1_BASE	EQU	0x0c0;..0x0c4    LFO1
SID_Ix_L_LFO2_BASE	EQU	0x0c5;..0x0c9    LFO2
SID_Ix_L_LFO3_BASE	EQU	0x0ca;..0x0ce    LFO3
SID_Ix_L_LFO4_BASE	EQU	0x0cf;..0x0d3    LFO4
SID_Ix_L_LFO5_BASE	EQU	0x0d4;..0x0d8    LFO5
SID_Ix_L_LFO6_BASE	EQU	0x0d9;..0x0dd    LFO6

;; free: 0xde and 0xdf

;; ENV sections (uses SID_Ix_ENVx_* offsets)
SID_Ix_L_ENV1_BASE	EQU	0x0e0;..0x0ef    ENV1
SID_Ix_L_ENV2_BASE	EQU	0x0f0;..0x0ff    ENV2

;; MOD sections (uses SID_Ix_MODx_* offsets)
SID_Ix_L_MOD1_BASE	EQU	0x100;..0x107   Modulator1
SID_Ix_L_MOD2_BASE	EQU	0x108;..0x10f   Modulator2
SID_Ix_L_MOD3_BASE	EQU	0x110;..0x117   Modulator3
SID_Ix_L_MOD4_BASE	EQU	0x118;..0x11f   Modulator4
SID_Ix_L_MOD5_BASE	EQU	0x120;..0x127   Modulator5
SID_Ix_L_MOD6_BASE	EQU	0x128;..0x12f   Modulator6
SID_Ix_L_MOD7_BASE	EQU	0x130;..0x137   Modulator7
SID_Ix_L_MOD8_BASE	EQU	0x138;..0x13f   Modulator8

;; TRG sections
SID_Ix_L_TRG_BASE	EQU	0x140;..0x16f
SID_Ix_L_TRG_NOn_BASE	EQU	0x140;..0x142	Note On
SID_Ix_L_TRG_NOf_BASE	EQU	0x143;..0x145	Note Off
SID_Ix_L_TRG_E1S_BASE	EQU	0x146;..0x148	ENV1 Sustain Phase
SID_Ix_L_TRG_E2S_BASE	EQU	0x149;..0x14b	ENV2 Sustain Phase
SID_Ix_L_TRG_L1P_BASE	EQU	0x14c;..0x14e	LFO1 Period
SID_Ix_L_TRG_L2P_BASE	EQU	0x14f;..0x151	LFO2 Period
SID_Ix_L_TRG_L3P_BASE	EQU	0x152;..0x154	LFO3 Period
SID_Ix_L_TRG_L4P_BASE	EQU	0x155;..0x157	LFO4 Period
SID_Ix_L_TRG_L5P_BASE	EQU	0x158;..0x15a	LFO5 Period
SID_Ix_L_TRG_L6P_BASE	EQU	0x15b;..0x15d	LFO6 Period
SID_Ix_L_TRG_Clk_BASE	EQU	0x15e;..0x160	Clock
SID_Ix_L_TRG_Cl6_BASE	EQU	0x161;..0x163	Clock/6
SID_Ix_L_TRG_C24_BASE	EQU	0x164;..0x166	Clock/24
SID_Ix_L_TRG_MSt_BASE	EQU	0x167;..0x169	MIDI Clock Start

;; free: 0x6a-0x6b

;; WT sections (uses SID_Ix_WTx_* offsets)
SID_Ix_L_WT1_BASE	EQU	0x16c;..0x170
SID_Ix_L_WT2_BASE	EQU	0x171;..0x175
SID_Ix_L_WT3_BASE	EQU	0x176;..0x17a
SID_Ix_L_WT4_BASE	EQU	0x17b;..0x17f


;; --- offset definitions ---
SID_Ix_L_Fx_CHN_MODE	EQU	0x0	; [2:0] assigned oscillators, [3] Ext. Input, [6:4] filter mode, [7] OSC3 off
SID_Ix_L_Fx_CUTOFF_L	EQU	0x1	; cut-off frequency (12 bit, 11 bit used)
SID_Ix_L_Fx_CUTOFF_H	EQU	0x2     ; bit [7] contols FIP (interpolation option)
SID_Ix_L_Fx_RESONANCE	EQU	0x3	; resonance (8bit, 4 bit used)
SID_Ix_L_Fx_KEYTRACK	EQU	0x4	; keytracking
SID_Ix_L_Fx_RESERVED2	EQU	0x5	; reserved for future expansions

SID_Ix_Vx_FLAGS1	EQU	0x00	; [1:0] portamento (0), constant time glide (1), glissando (2), [2] GAE (gate always enabled)
SID_Ix_Vx_WAVEFORM	EQU	0x01	; [3:0] waveform, [4] voice off, [5] sync, [6] ring, [7] reserved
SID_Ix_Vx_AD		EQU	0x02	; 2*4bit value for attack [7:4] and decay [3:0]
SID_Ix_Vx_SR		EQU	0x03	; 2*4bit value for sustain [7:4] and sustain [3:0]
SID_Ix_Vx_PULSEWIDTH_L	EQU	0x04	; [7:0] low byte
SID_Ix_Vx_PULSEWIDTH_H	EQU	0x05    ; [3:0] high nibble, [7:4] reserved
SID_Ix_Vx_ACCENT	EQU	0x06	; not used by lead engine, could be replaced by something else
SID_Ix_Vx_SWINSID_PHASE	EQU	0x06	; used by lead engine of SwinSID option enabled
SID_Ix_Vx_DELAY		EQU	0x07	; 8bit
SID_Ix_Vx_TRANSPOSE	EQU	0x08	; 7bit
SID_Ix_Vx_FINETUNE	EQU	0x09	; 8bit
SID_Ix_Vx_PITCHRANGE	EQU	0x0a	; 7bit
SID_Ix_Vx_PORTAMENTO	EQU	0x0b	; 8bit
SID_Ix_Vx_ARP_MODE	EQU	0x0c	; [0] enable, [2:1] dir up/down/U&D/D&U, [3] random, [4] sorted, [5] hold, [6] sync, [7] CAC
SID_Ix_Vx_ARP_SPEED_DIV EQU	0x0d	; [7] oneshot, [6] reserved, [5:0] own speed or derived from global clock (dividable)
SID_Ix_Vx_ARP_GL_RNG	EQU	0x0e	; [4:0] gatelength [7:5] arp octave range (seperate for each voice)
SID_Ix_Vx_SWINSID_MODE	EQU	0x0f	; [3:0] waveform of second OSC, [4] enable 2nd OSC, [5] x2 pitch, [6] reverse waveform

SID_Ix_LFOx_MODE	EQU	0x00	; [0] enable, [2] clksync, [3] oneshot, [7:4] waveform
SID_Ix_LFOx_DEPTH	EQU	0x01	; LFO depth (8 bit instead of 7 like MBSID V1!)
SID_Ix_LFOx_RATE	EQU	0x02	; LFO rate (8 bit instead of 7 like MBSID V1!)
SID_Ix_LFOx_DELAY	EQU	0x03	; LFO depth (8 bit instead of 7 like MBSID V1!)
SID_Ix_LFOx_PHASE	EQU	0x04	; LFO phase

SID_Ix_L_ENVx_MODE	EQU	0x00	; ENV mode ([2:0] loop begin, [3] reserved, [6:4] loop end, [7] BPM sync)
SID_Ix_L_ENVx_DEPTH	EQU	0x01	; 8bit value
SID_Ix_L_ENVx_DELAY	EQU	0x02	; 8bit value
SID_Ix_L_ENVx_ATTACK1	EQU	0x03	; 8bit value
SID_Ix_L_ENVx_ATTLVL	EQU	0x04	; 8bit value
SID_Ix_L_ENVx_ATTACK2	EQU	0x05	; 8bit value
SID_Ix_L_ENVx_DECAY1	EQU	0x06	; 8bit value
SID_Ix_L_ENVx_DECLVL	EQU	0x07	; 8bit value
SID_Ix_L_ENVx_DECAY2	EQU	0x08	; 8bit value
SID_Ix_L_ENVx_SUSTAIN	EQU	0x09	; 8bit value
SID_Ix_L_ENVx_RELEASE1	EQU	0x0a	; 8bit value
SID_Ix_L_ENVx_RELLVL	EQU	0x0b	; 8bit value
SID_Ix_L_ENVx_RELEASE2	EQU	0x0c	; 8bit value
SID_Ix_L_ENVx_ATT_CURVE	EQU	0x0d	; 8bit value
SID_Ix_L_ENVx_DEC_CURVE	EQU	0x0e	; 8bit value
SID_Ix_L_ENVx_REL_CURVE	EQU	0x0f	; 8bit value

SID_Ix_MODx_SRC1	EQU	0x00	; modulation source #1 - LFO, ENV, Analog In, Velocity, Modwheel, Aftertouch, PitchBend, Keynumber, Knob #1-5
SID_Ix_MODx_SRC2	EQU	0x01	; modulation source #2
SID_Ix_MODx_OP		EQU	0x02	; [7] invert target 1/L [6] invert target 2/R [5:4] reserved [3:0] modulation operation (SRC1 only, SRC2 only, SRC1+SRC2, SRC1-SRC2, SRC1*SRC2, XOR, OR, AND, Min, Max)
SID_Ix_MODx_DEPTH	EQU	0x03	; 8bit value (-128..+127)
SID_Ix_MODx_TARG_L	EQU	0x04	; modulation target SIDL [2:0] Pitch Voice 1/2/3, [5:3] PW Voice 1/2/3, [6] Filter, [7] Volume
SID_Ix_MODx_TARG_R	EQU	0x05	; modulation target SIDR [2:0] Pitch Voice 1/2/3, [5:3] PW Voice 1/2/3, [6] Filter, [7] Volume
SID_Ix_MODx_TARG_X1	EQU	0x06	; additional selectable target #1
SID_Ix_MODx_TARG_X2	EQU	0x07	; additional selectable target #2

SID_Ix_WTx_SPEED	EQU	0x00	; [5:0] clock divider, [6] channel target SIDL, [7] channel target SIDR
SID_Ix_WTx_ASSGN	EQU	0x01	; parameter assignment
SID_Ix_WTx_BEGIN	EQU	0x02	; [6:0] start position in wavetable, [7] position controlled by MOD
SID_Ix_WTx_END		EQU	0x03	; [6:0] end position in wavetable
SID_Ix_WTx_LOOP		EQU	0x04	; [6:0] loop position in wavetable, [7] one shot

SID_Ix_WAVETABLE	EQU	0x180;..0x1ff

;; --------------------------------------------------------------------------

;; --- Multi ---

;; general flags
SID_Ix_M_FLAGS1		EQU	0x050	; not used - FLAGS1 is available for each instrument seperately (SID_Ix_M_Vx_FLAGS1)
	;; free: 0x51
SID_Ix_M_VOLUME		EQU	0x052	; 7 bit value, only 4 bit used
	;; free: 0x53

SID_Ix_M_S1F_BASE	EQU	0x054;..0x059
SID_Ix_M_S2F_BASE	EQU	0x05a;..0x05f

SID_Ix_M_I1_BASE	EQU	0x060;..0x08f    Multi Instrument #1
SID_Ix_M_I2_BASE	EQU	0x090;..0x0bf    Multi Instrument #2
SID_Ix_M_I3_BASE	EQU	0x0c0;..0x0ef    Multi Instrument #3
SID_Ix_M_I4_BASE	EQU	0x0f0;..0x11f    Multi Instrument #4
SID_Ix_M_I5_BASE	EQU	0x120;..0x14f    Multi Instrument #5
SID_Ix_M_I6_BASE	EQU	0x150;..0x17f    Multi Instrument #6

;; enhancement of SID_Ix_Vx_* structure (re-used from lead engine)
SID_Ix_M_Vx_FLAGS2	EQU	0x10	; mono/legato [0], WTO [1], SusKey [2], Poly [3]
SID_Ix_M_Vx_VOICE_ASSGN EQU	0x11	; [3:0] voice assignment, [7:4] reserved
SID_Ix_M_Vx_VELOCITY_ASSGN EQU	0x12	; velocity assignment
SID_Ix_M_Vx_PITCHBENDER_ASSGN EQU 0x13	; pitchbender assignment

SID_Ix_M_Vx_LFO1_MODE	EQU	0x14	; same as SID_Ix_LFOx_MODE
SID_Ix_M_Vx_LFO1_DEPTH_P EQU	0x15	; 8bit, depth of pitch modulation
SID_Ix_M_Vx_LFO1_RATE	EQU	0x16	; same as SID_Ix_LFOx_RATE
SID_Ix_M_Vx_LFO1_DELAY	EQU	0x17	; same as SID_Ix_LFOx_DELAY
SID_Ix_M_Vx_LFO1_PHASE	EQU	0x18	; same as SID_Ix_LFOx_PHASE
SID_Ix_M_Vx_LFO1_DEPTH_PW EQU	0x19	; 8bit, depth of pulsewidth modulation
SID_Ix_M_Vx_LFO1_DEPTH_F EQU	0x1a	; 8bit, depth of cutoff modulation
SID_Ix_M_Vx_LFO2_MODE	EQU	0x1b	; same as SID_Ix_LFOx_MODE
SID_Ix_M_Vx_LFO2_DEPTH_P EQU	0x1c	; 8bit, depth of pitch modulation
SID_Ix_M_Vx_LFO2_RATE	EQU	0x1d	; same as SID_Ix_LFOx_RATE
SID_Ix_M_Vx_LFO2_DELAY	EQU	0x1e	; same as SID_Ix_LFOx_DELAY
SID_Ix_M_Vx_LFO2_PHASE	EQU	0x1f	; same as SID_Ix_LFOx_PHASE
SID_Ix_M_Vx_LFO2_DEPTH_PW EQU	0x20	; 8bit, depth of pulsewidth modulation
SID_Ix_M_Vx_LFO2_DEPTH_F EQU	0x21	; 8bit, depth of cutoff modulation

SID_Ix_M_Vx_ENV_MODE	EQU	0x22	; same as SID_Ix_ENVx_MODE, but [6:4] control the curve assignments
SID_Ix_M_Vx_ENV_DEPTH_P	EQU	0x23	; 8bit, depth of pitch modulation
SID_Ix_M_Vx_ENV_DEPTH_PW EQU	0x24	; 8bit, depth of pulsewidth modulation
SID_Ix_M_Vx_ENV_DEPTH_F EQU	0x25	; 8bit, depth of cutoff modulation
SID_Ix_M_Vx_ENV_ATTACK	EQU	0x26	; same as SID_Ix_ENVx_ATTACK1
SID_Ix_M_Vx_ENV_DECAY	EQU	0x27	; same as SID_Ix_ENVx_DECAY2
SID_Ix_M_Vx_ENV_SUSTAIN EQU	0x28	; same as SID_Ix_ENVx_SUSTAIN
SID_Ix_M_Vx_ENV_RELEASE EQU	0x29	; same as SID_Ix_ENVx_RELEASE
SID_Ix_M_Vx_ENV_CURVE EQU	0x2a	; 8bit value, assignments in SID_Ix_ENVx_MODE[6:4]

SID_Ix_M_Vx_WT_SPEED	EQU	0x2b	; same as SID_Ix_WTx_SPEED
SID_Ix_M_Vx_WT_ASSGN	EQU	0x2c	; same as SID_Ix_WTx_ASSGN
SID_Ix_M_Vx_WT_BEGIN	EQU	0x2d	; same as SID_Ix_WTx_BEGIN
SID_Ix_M_Vx_WT_END	EQU	0x2e	; same as SID_Ix_WTx_END
SID_Ix_M_Vx_WT_LOOP	EQU	0x2f	; same as SID_Ix_WTx_LOOP

;; --------------------------------------------------------------------------

;; --- Bassline ---

;; general flags
SID_Ix_B_FLAGS1		EQU	0x050	; not used - FLAGS1 is available for each instrument seperately (SID_Ix_B_Vx_FLAGS1)
	;; free: 0x51
SID_Ix_B_VOLUME		EQU	0x052	; 7 bit value, only 4 bit used
	;; free: 0x53

SID_Ix_B_S1F_BASE	EQU	0x054;..0x059
SID_Ix_B_S2F_BASE	EQU	0x05a;..0x05f

SID_Ix_B_S1V1_BASE	EQU	0x060;..0x0af    SID1 Voice1
SID_Ix_B_S2V1_BASE	EQU	0x0b0;..0x0ff    SID2 Voice1

;; enhancement of SID_Ix_Vx_* structure (re-used from lead engine)
SID_Ix_B_Vx_RESERVED1	EQU	0x2c
SID_Ix_B_Vx_RESERVED2	EQU	0x2d
SID_Ix_B_Vx_RESERVED3	EQU	0x2e
SID_Ix_B_Vx_RESERVED4	EQU	0x2f

SID_Ix_B_Vx_FLAGS2	EQU	0x10	; mono/legato [0], WTO [1], SusKey [2], OSC Phase Sync [3]
;; 0x11..0x13 not used (dedicated voice/PB/vel assignments)

SID_Ix_B_Vx_LFO1_MODE	EQU	0x14	; same as SID_Ix_LFOx_MODE
SID_Ix_B_Vx_LFO1_DEPTH_P EQU	0x15	; 8bit, depth of pitch modulation
SID_Ix_B_Vx_LFO1_RATE	EQU	0x16	; same as SID_Ix_LFOx_RATE
SID_Ix_B_Vx_LFO1_DELAY	EQU	0x17	; same as SID_Ix_LFOx_DELAY
SID_Ix_B_Vx_LFO1_PHASE	EQU	0x18	; same as SID_Ix_LFOx_PHASE
SID_Ix_B_Vx_LFO1_DEPTH_PW EQU	0x19	; 8bit, depth of pulsewidth modulation
SID_Ix_B_Vx_LFO1_DEPTH_F EQU	0x1a	; 8bit, depth of cutoff modulation
SID_Ix_B_Vx_LFO2_MODE	EQU	0x1b	; same as SID_Ix_LFOx_MODE
SID_Ix_B_Vx_LFO2_DEPTH_P EQU	0x1c	; 8bit, depth of pitch modulation
SID_Ix_B_Vx_LFO2_RATE	EQU	0x1d	; same as SID_Ix_LFOx_RATE
SID_Ix_B_Vx_LFO2_DELAY	EQU	0x1e	; same as SID_Ix_LFOx_DELAY
SID_Ix_B_Vx_LFO2_PHASE	EQU	0x1f	; same as SID_Ix_LFOx_PHASE
SID_Ix_B_Vx_LFO2_DEPTH_PW EQU	0x20	; 8bit, depth of pulsewidth modulation
SID_Ix_B_Vx_LFO2_DEPTH_F EQU	0x21	; 8bit, depth of cutoff modulation

SID_Ix_B_Vx_ENV_MODE	EQU	0x22	; same as SID_Ix_ENVx_MODE, but [6:4] control the curve assignments
SID_Ix_B_Vx_ENV_DEPTH_P	EQU	0x23	; 8bit, depth of pitch modulation
SID_Ix_B_Vx_ENV_DEPTH_PW EQU	0x24	; 8bit, depth of pulsewidth modulation
SID_Ix_B_Vx_ENV_DEPTH_F EQU	0x25	; 8bit, depth of cutoff modulation
SID_Ix_B_Vx_ENV_ATTACK	EQU	0x26	; same as SID_Ix_ENVx_ATTACK1
SID_Ix_B_Vx_ENV_DECAY	EQU	0x27	; same as SID_Ix_ENVx_DECAY2
SID_Ix_B_Vx_ENV_SUSTAIN EQU	0x28	; same as SID_Ix_ENVx_SUSTAIN
SID_Ix_B_Vx_ENV_RELEASE EQU	0x29	; same as SID_Ix_ENVx_RELEASE
SID_Ix_B_Vx_ENV_CURVE EQU	0x2a	; 8bit value, assignments in SID_Ix_ENVx_MODE[6:4]

SID_Ix_B_Vx_SEQ_SPEED	EQU	0x2b	; [5:0] sequencer speed, [6] reserved, [7] 16step sync (sync to measure)
SID_Ix_B_Vx_SEQ_NUM	EQU	0x2c	; [3:0] sequence number (0-8, 8=disabled)
SID_Ix_B_Vx_SEQ_LENGTH	EQU	0x2d	; [3:0] steps (0-15)
SID_Ix_B_Vx_SEQ_ASSGN	EQU	0x2e	; same as SID_Ix_WTx_ASSGN
SID_Ix_B_Vx_SEQ_RESERVED2 EQU	0x2f

SID_Ix_B_Vx_ENV_DECAY_A	EQU	0x030	; ENV decay used on accented notes

	;; free offsets: 0x31..0x4f

SID_Ix_B_SEQUENCES	EQU	0x100;..0x1ff
	;; structure: 2 bytes for each step (selected with address bit #7)
	;;              lower byte: note/octave/glide/gate flags
	;;              upper byte: accent and assignable parameter
	;;            16 steps per sequence
	;;            8 sequences

;; --------------------------------------------------------------------------

;; --- Drums ---

;; general flags
SID_Ix_D_SEQ_SPEED	EQU	0x050	; [5:0] sequencer speed, [6] sequencer on, [7] 16step sync
SID_Ix_D_SEQ_NUM	EQU	0x051	; sequence number (0-8, 8=disabled)
SID_Ix_D_VOLUME		EQU	0x052	; 7 bit value, only 4 bit used
SID_Ix_D_SEQ_LENGTH	EQU	0x053	; steps (0-15)

SID_Ix_D_S1F_BASE	EQU	0x054;..0x059
SID_Ix_D_S2F_BASE	EQU	0x05a;..0x05f

SID_Ix_D_I1_BASE	EQU	0x060;..0x069    Drum Instrument #1
SID_Ix_D_I2_BASE	EQU	0x06a;..0x073    Drum Instrument #2
SID_Ix_D_I3_BASE	EQU	0x074;..0x07d    Drum Instrument #3
SID_Ix_D_I4_BASE	EQU	0x07e;..0x087    Drum Instrument #4
SID_Ix_D_I5_BASE	EQU	0x088;..0x091    Drum Instrument #5
SID_Ix_D_I6_BASE	EQU	0x092;..0x09b    Drum Instrument #6
SID_Ix_D_I7_BASE	EQU	0x09c;..0x0a5    Drum Instrument #7
SID_Ix_D_I8_BASE	EQU	0x0a6;..0x0af    Drum Instrument #8
SID_Ix_D_I9_BASE	EQU	0x0b0;..0x0b9    Drum Instrument #9
SID_Ix_D_I10_BASE	EQU	0x0ba;..0x0c3    Drum Instrument #10
SID_Ix_D_I11_BASE	EQU	0x0c4;..0x0cd    Drum Instrument #11
SID_Ix_D_I12_BASE	EQU	0x0ce;..0x0d7    Drum Instrument #12
SID_Ix_D_I13_BASE	EQU	0x0d8;..0x0e1    Drum Instrument #13
SID_Ix_D_I14_BASE	EQU	0x0e2;..0x0eb    Drum Instrument #14
SID_Ix_D_I15_BASE	EQU	0x0ec;..0x0f5    Drum Instrument #15
SID_Ix_D_I16_BASE	EQU	0x0f6;..0x0ff    Drum Instrument #16

SID_Ix_Dx_FLAGS1	EQU	0x00	; [7:4] voice assignment, [3:0] reserved
SID_Ix_Dx_MODEL		EQU	0x01	; drum model
SID_Ix_Dx_AD		EQU	0x02	; 2*4bit value for attack [7:4] and decay [3:0]
SID_Ix_Dx_SR		EQU	0x03	; 2*4bit value for sustain [7:4] and sustain [3:0]
SID_Ix_Dx_TUNE		EQU	0x04	; 8bit
SID_Ix_Dx_PAR1		EQU	0x05	; 8bit
SID_Ix_Dx_PAR2		EQU	0x06	; 8bit
SID_Ix_Dx_PAR3		EQU	0x07	; 8bit
SID_Ix_Dx_VELOCITY_ASSGN EQU	0x08	; velocity assignment
SID_Ix_Dx_RESERVED1	EQU	0x09	; reserved for future extensions

SID_Ix_D_SEQUENCES	EQU	0x100;..0x1ff
	;; structure: 4 bytes for 16 steps
	;;              two bytes for gate, two bytes for accent
	;;            8 tracks per sequence
	;;            8 sequences

;; ==========================================================================

MBNET_BASE		EQU	0x600	; used for SET_BSR

MBNET_STATE		EQU	0x600
MBNET_RX_BUFFER_PTR_L	EQU	0x601
MBNET_RX_BUFFER_PTR_H	EQU	0x602
MBNET_MY_ID		EQU	0x603
MBNET_MASTER_ID		EQU	0x604
MBNET_SLAVE_ID		EQU	0x605
MBNET_TOS		EQU	0x606
MBNET_EID_L		EQU	0x607
MBNET_EID_H		EQU	0x608
MBNET_NODE_AVAIL	EQU	0x609
MBNET_NODE_INCOMPATIBLE	EQU	0x60a
MBNET_TIMEOUT_CTR_L	EQU	0x60b
MBNET_TIMEOUT_CTR_H	EQU	0x60c
MBNET_LOOP_CTR		EQU	0x60d
MBNET_RETRY_WAIT_CTR	EQU	0x60e
MBNET_RETRY_NODE	EQU	0x60f

MBNET_RETRY_NODE_CTR_BEGIN EQU	0x610	; ..0x617

MBNET_NODE_INFO_BEGIN	EQU	0x620	; ..0x65f  ; (8*8 bytes)

MBNET_CLONE_BUFFER	EQU	0x700	; 64 bytes, must be aligned, can be any free address since chip will be reset after cloning


;; ==========================================================================
;;  MIDI Voice records
;; ==========================================================================
SID_MVx_NUM		EQU	6	; note: changing this value could lead to troubles in sid_ens.inc (ensemble structure...)
SID_MVx_RECORD_LEN	EQU	0x18

SID_MVx_MIDI_CHANNEL	EQU	0x00	; from ensemble
SID_MVx_SPLIT_LOWER	EQU	0x01	; from ensemble
SID_MVx_SPLIT_UPPER	EQU	0x02	; from ensemble
SID_MVx_TRANSPOSE	EQU	0x03	; from ensemble
SID_MVx_PITCHBENDER	EQU	0x04	; only for Multi Engine ----- we will clear on patch changes from here, see sid_patch.inc
SID_MVx_LAST_VOICE	EQU	0x05	; only for Multi Engine
SID_MVx_ARP_STATE	EQU	0x06	; arp variables
SID_MVx_ARP_DIV_CTR	EQU	0x07
SID_MVx_ARP_GL_CTR	EQU	0x08
SID_MVx_ARP_NOTE_CTR	EQU	0x09
SID_MVx_ARP_OCT_CTR	EQU	0x0a

SID_MVx_WT_STACK_0	EQU	0x0b	; special sorted note stack for WT sequencer (Arp note feature)
SID_MVx_WT_STACK_1	EQU	0x0c
SID_MVx_WT_STACK_2	EQU	0x0d
SID_MVx_WT_STACK_3	EQU	0x0e

SID_MVx_NOTE_STACK_PTR	EQU	0x0f	; the note stack
SID_MVx_NOTE_STACK_0	EQU	0x10
SID_MVx_NOTE_STACK_1	EQU	0x11
SID_MVx_NOTE_STACK_2	EQU	0x12
SID_MVx_NOTE_STACK_3	EQU	0x13
SID_MVx_NOTE_STACK_4	EQU	0x14
SID_MVx_NOTE_STACK_5	EQU	0x15
SID_MVx_NOTE_STACK_6	EQU	0x16
SID_MVx_NOTE_STACK_7	EQU	0x17

SID_MV1_BASE		EQU	0x660;..0x677
SID_MV2_BASE		EQU	0x678;..0x68f
SID_MV3_BASE		EQU	0x690;..0x6a7
SID_MV4_BASE		EQU	0x6a8;..0x6bf
SID_MV5_BASE		EQU	0x6c0;..0x6d7
SID_MV6_BASE		EQU	0x6d8;..0x6ef

SID_MVx_NOTE_STACK_LEN	EQU	SID_MVx_NOTE_STACK_7 - SID_MVx_NOTE_STACK_0 + 1


;; ==========================================================================
;;  Voice Queue
;; ==========================================================================

SID_VOICE_QUEUE_0	EQU	0x6f0
SID_VOICE_QUEUE_5	EQU	0x6f5
SID_VOICE_QUEUE_LEN	EQU	SID_VOICE_QUEUE_5 - SID_VOICE_QUEUE_0 + 1

SID_VOICE_INSTR_0	EQU	0x6f6
SID_VOICE_INSTR_5	EQU	0x6fb
SID_VOICE_INSTR_OFFSET	EQU	SID_VOICE_INSTR_0 - SID_VOICE_QUEUE_0

	;; free: 0x6fc-0x6ff

;; ==========================================================================

;; patch buffer of the MBSID engine
SID_PATCH_BUFFER	EQU	0x700	; ..0x8ff

;; shadowed patch buffer of the MBSID engine (used in sid_se.inc for non-destructive modulations)
SID_PATCH_BUFFER_SHADOW	EQU	0x900	; ..0xbff
;; note: if some memory is required, 0xa80-0xaff could be used, because the wavetable only needs to exist once

;; patch buffer of the MBSID engine, which is edited with the CS
SID_EDIT_BUFFER		EQU	0xb00	; ..0xcff
