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

#define SID_STAT_ENGINE_DISABLE		0 ; if set, SID tick and SID interrupt will not be processed
#define SID_STAT_FORCE_REFRESH		1 ; if set, a refresh will be forced with next SID_Tick
#define SID_STAT_CC_DUMP_REQ		2 ; if set, a CC dump will be sent
#define SID_STAT_SAVED_GIE		3 ; save the GIE bit for nested interrupt disables
#define SID_STAT_TRIGGER_RESET		4 ; if set, the SID will be reset with next SID_Tick

;; ==========================================================================

#define SID_PLAY_MODE_POLY		0
#define SID_PLAY_MODE_SUS_KEY		1
#define SID_PLAY_MODE_LEGATO_OFF	2
#define SID_PLAY_MODE_ONLY_WT_OFF	3

;; ==========================================================================

#define SID_MIDI_SYNC_WT_ARP		0	; sync Wavetable sequencer and arp
#define SID_MIDI_SYNC_LFO		1	; sync LFOs
#define SID_MIDI_SYNC_ENV		2	; sync EGs
#define SID_MIDI_SYNC_SEND_CLK		3	; send MIDI clock
#define SID_MIDI_SYNC_F8		4	; bit 3 notifies a received MIDI clock
#define	SID_MIDI_SYNC_FA		5	; bit 4 notifies a received MIDI clock start
#define SID_MIDI_SYNC_TICK_1            6       ; bit 6 notifies that the SIDSW engine should be clocked with common resolution
#define SID_MIDI_SYNC_TICK_2            7       ; bit 7 notifies that the SIDSW engine should be clocked with double resolution

;; ==========================================================================

#define BANKSTICK_MAGIC0	0x73 ; magic numbers - if they don't exists in bankstick at BANK_MAGIC_ADDRESS, 
#define BANKSTICK_MAGIC1	0x12 ; the stick will be ignored until it has been formatted

;; ==========================================================================

#if CS_ENABLED == 1
#define CS_STAT_DISPLAY_INIT_REQ	1	; flag 1: request a display initialization
#define CS_STAT_DISPLAY_UPDATE_REQ	2	; flag 2: request a display update
#define CS_STAT_DISPLAY_LONGLABEL_REQ	3	; flag 3: request a long label message
#define CS_STAT_CURSOR_FLASH		4	; flag 4: handled by cs_menu_timer.inc
#define CS_STAT_MODIFY_PARAMETER	5	; flag 5: modify parameter if set, else select parameter
#define CS_STAT_MODIFY_NAME		6	; flag 6: modify name if set, else modify parameter
#define CS_STAT_UPDATE_PARAMETERS	7	; flag 7: update CS parameters when a CC has been received

#define CS_STAT2_EEPROM_WR_WT_REQ	0	; request a write to WT
#define CS_STAT2_EEPROM_WR_WT_GNT	1	; grant the request
#define CS_STAT2_PLAY_SID1		4	; SIDx/Menu button combination: toggles play state
#define CS_STAT2_PLAY_SID2		5	; DON'T CHANGE THE BIT NUMBERS!!!
#define CS_STAT2_PLAY_SID3		6	; 
#define CS_STAT2_PLAY_SID4		7	; 

#define CS_MENU_MODE_LINK		0	; flag 0: Link mode (merger on/off)
#define CS_MENU_MODE_CC			1	; flag 1: CC mode
#define CS_MENU_MODE_EDIT		2	; flag 2: edit mode
#define	CS_MENU_MODE_EDIT_NOTIFIER	3	; flag 3: notifies when any value has been changed (can also be mapped to a LED)
#define	CS_MENU_MODE_EDIT_FLASHED	4	; flag 4: the flashed version of the edit button
#define CS_MENU_MODE_MATRIX_METER	5	; alternative meter mode for modulation sources
#define CS_MENU_MODE_MATRIX_METER_DISP	6	; used in cs_menu_matrix.inc to decide if meters should be displayed

#endif
;; ==========================================================================
	;; free memory for user applications:	
	;; 0x010-0x37f

;; ==========================================================================
;;  General SID Control Variables in ACCESS page
;; ==========================================================================
SID_STAT		EQU	0x010

SID_PATCH		EQU	0x011
SID_BANK		EQU	0x012
SID_BANKSTICK_CHK_CTR	EQU	0x013
SID_BANKSTICK_STATUS	EQU	0x014
SID_MIDI_DEVICE		EQU	0x015

;; ==========================================================================
;;  Variables used by the MIDI processor in sid_sysex.inc
;; ==========================================================================
SID_SYSEX_STATE		EQU	0x016
SID_SYSEX_ACTION	EQU	0x017
SID_SYSEX_IN		EQU	0x018
SID_SYSEX_CHECKSUM	EQU	0x019
SID_SYSEX_ADDRESS	EQU	0x01a

;; ==========================================================================
;;  Variables used by the control surface
;; ==========================================================================
#if CS_ENABLED == 1
CS_STAT			EQU	0x01b	; control surface status
CS_STAT2		EQU	0x01c	; control surface status
CS_MENU			EQU	0x01d	; menu mode
CS_MENU_TABLE_0_L	EQU	0x01e	; pointer to first table entry, low-byte
CS_MENU_TABLE_0_H	EQU	0x01f	; pointer to first table entry, high-byte
CS_MENU_PAGE_OFFSET	EQU	0x020	; page offset to first displayed entry
CS_MENU_CURSOR_POS	EQU	0x021	; cursor position
CS_MENU_ENTRIES		EQU	0x022	; number of menu entries
CS_MENU_TIMER_CTR	EQU	0x023	; used by cs_menu_timer.inc
CS_CURSOR_CTR		EQU	0x024	; used by cs_menu_timer.inc
CS_SELECT_CTR		EQU	0x025	; used by cs_menu.inc, counted by cs_menu_timer.inc
CS_MENU_REFRESH_ITEM_CTR EQU	0x026	; used by cs_menu.inc to count through the displayed items
CS_MENU_ENTRY_TBLPTRL	EQU	0x027	; used by cs_menu.inc
CS_MENU_ENTRY_TBLPTRH	EQU	0x028	; used by cs_menu.inc
CS_MENU_MODE		EQU	0x029	; control mode flags (Link/CC/Edit mode)
CS_MENU_P2R_R2P_TMP	EQU	0x02a	; temporary used by P2R_R2P function
CS_MENU_WT_POS		EQU	0x02b	; contains the current WT pos
CS_MENU_WT_POS_BUFFER	EQU	0x02c	; contains the pos which corresponds to the buffer content
CS_MENU_SAVE_PATCH	EQU	0x02d	; contains the patch number for save function
CS_MENU_SAVE_BANK	EQU	0x02e	; contains the bank number for save function
CS_MENU_REINIT_CFG_CTR	EQU	0x02f	; reinit configuration if counter == 1
CS_MENU_PARAMETER_L	EQU	0x030	; contains low-byte of current parameter value, must be located to an even address!!!
CS_MENU_PARAMETER_H	EQU	0x031	; contains high-byte of current parameter value
CS_MENU_PARAMETER_MAX_L	EQU	0x032	; contains max value of current parameter, must be located to an even address!!!
CS_MENU_PARAMETER_MAX_H	EQU	0x033	; contains max value of current parameter
CS_MENU_NAME_POS	EQU	0x034	; for the "edit name" mode
CS_MENU_SID		EQU	0x035	; number of SID which is currently handled
CS_MENU_SID_M_PATCH	EQU	0x036	; patch number of master	;(don't change the order of these 4 registers)
CS_MENU_SID_S1_PATCH	EQU	0x037	; patch number of first slave
CS_MENU_SID_S2_PATCH	EQU	0x038	; patch number of second slave
CS_MENU_SID_S3_PATCH	EQU	0x039	; patch number of third slave
CS_MENU_SID_M_BANK	EQU	0x03a	; bank number of master	        ;(don't change the order of these 4 registers)
CS_MENU_SID_S1_BANK	EQU	0x03b	; bank number of first slave
CS_MENU_SID_S2_BANK	EQU	0x03c	; bank number of second slave
CS_MENU_SID_S3_BANK	EQU	0x03d	; bank number of third slave
CS_MENU_SID_M_CHN	EQU	0x03e	; MIDI channel of master	;(don't change the order of these 4 registers)
CS_MENU_SID_S1_CHN	EQU	0x03f	; MIDI channel of first slave
CS_MENU_SID_S2_CHN	EQU	0x040	; MIDI channel of second slave
CS_MENU_SID_S3_CHN	EQU	0x041	; MIDI channel of third slave
CS_MENU_TX_M_CTR	EQU	0x042	; transmit counter master, if > 0, patch tx will be delayed
CS_MENU_TX_S1_CTR	EQU	0x043	; transmit counter master, if > 0, patch tx will be delayed
CS_MENU_TX_S2_CTR	EQU	0x044	; transmit counter master, if > 0, patch tx will be delayed
CS_MENU_TX_S3_CTR	EQU	0x045	; transmit counter master, if > 0, patch tx will be delayed
CS_MENU_SELECTED_SID_FLAGS EQU  0x046	; every flag stays for one SID
CS_MENU_SELECTED_OSC	EQU	0x047	; the selected oscillator
CS_MENU_SELECTED_OSC_FLAGS EQU	0x048	; every flag stays for one oscillator
CS_MENU_SELECTED_OSC_CTRL EQU	0x049	; selects Env/Misc and Assign
CS_MENU_SELECTED_LFO	EQU	0x04a	; the selected LFO
CS_MENU_SELECTED_ENV	EQU	0x04b	; the selected envelope
CS_MENU_SELECTED_ENV_CTRL EQU	0x04c	; selects Env/Assign
CS_MENU_SELECTED_MOD	EQU	0x04d	; the selected mod entry
SAVED_CS_MENU		EQU	0x04e	; used by cs_menu.inc::CS_MENU_ButtonInc
SAVED_CS_MENU_PAGE_OFFSET EQU	0x04f	; used by cs_menu.inc::CS_MENU_ButtonInc
SAVED_CS_MENU_CURSOR_POS EQU	0x050	; used by cs_menu.inc::CS_MENU_ButtonInc
CS_MENU_ENC_ABS_VAL_BEGIN EQU	0x051	; absolute values of assign function (8 values)
CS_MENU_ENC_ABS_VAL_END EQU	0x059
CS_MENU_MATRIX_CTR	EQU	0x05a	; counter used by CS_MENU_MATRIX_Handler
CS_MENU_MATRIX_FLASH_CTR EQU	0x05b	; counter used by CS_MENU_MATRIX_Handler
CS_MENU_MATRIX_SELECTED_ROW EQU	0x05c	; selected matrix row (every button has one flag)
CS_MENU_MATRIX_SELECTED_COL EQU	0x05d	; selected matrix column (every button has one flag)
CS_MENU_MATRIX_BEGIN	EQU	0x05e	; content of modulation matrix (8 entries)
CS_MENU_MATRIX_END	EQU	0x065
CS_MENU_WT_MODIFIED	EQU	0x066	; each parameter column has it's own flag
#endif

;; ==========================================================================

MIDI_RXTX_RX_CTR	EQU	0x067
MIDI_RXTX_TX_CTR	EQU	0x068

;; ==========================================================================

AOUT_FILTER_TMP_L	EQU	0x069	; used in sid_sw.inc as temporary storage for filter value
AOUT_FILTER_TMP_H	EQU	0x06a

;; ==========================================================================

	;; free: 0x6b-0x6d

;; ==========================================================================

AOUT_INVERTED		EQU	0x06e
AOUT_UPDATE_REQ		EQU	0x06f
AOUT_VALUES		EQU	0x070  ; ..0x07f (8 * 16bit words)

;; ==========================================================================
	;; note: 0x080-0x0ff used by CS_SIDS1
;; ==========================================================================
;;  a lot of SID control variables in BANKED page
;; ==========================================================================
SIDSW_CLEAR_BEGIN	EQU	0x100		; used to initialize the whole SIDSW memory
SIDSW_CLEAR_END		EQU	0x1ff		; also used as 256 byte download buffer (!)

SID_BASE		EQU	0x100		; address space of SID chip (used: 0x00-0x18)
SID_V1_FRQ_L		EQU	SID_BASE+0x00
SID_V1_FRQ_H		EQU	SID_BASE+0x01
SID_V1_PW_L		EQU	SID_BASE+0x02
SID_V1_PW_H		EQU	SID_BASE+0x03
SID_V1_CTRL		EQU	SID_BASE+0x04
SID_V1_ENV_AD		EQU	SID_BASE+0x05
SID_V1_ENV_SR		EQU	SID_BASE+0x06

SID_V2_FRQ_L		EQU	SID_BASE+0x07
SID_V2_FRQ_H		EQU	SID_BASE+0x08
SID_V2_PW_L		EQU	SID_BASE+0x09
SID_V2_PW_H		EQU	SID_BASE+0x0a
SID_V2_CTRL		EQU	SID_BASE+0x0b
SID_V2_ENV_AD		EQU	SID_BASE+0x0c
SID_V2_ENV_SR		EQU	SID_BASE+0x0d

SID_V3_FRQ_L		EQU	SID_BASE+0x0e
SID_V3_FRQ_H		EQU	SID_BASE+0x0f
SID_V3_PW_L		EQU	SID_BASE+0x10
SID_V3_PW_H		EQU	SID_BASE+0x11
SID_V3_CTRL		EQU	SID_BASE+0x12
SID_V3_ENV_AD		EQU	SID_BASE+0x13
SID_V3_ENV_SR		EQU	SID_BASE+0x14

SID_FC_L		EQU	SID_BASE+0x15
SID_FC_H		EQU	SID_BASE+0x16
SID_RES_FCHN		EQU	SID_BASE+0x17
SID_MODE_VOL		EQU	SID_BASE+0x18

SID_PLAY_MODE		EQU	SID_BASE+0x19	; internal SIDSW register
SID_OSC_PHASE_SYNC	EQU	SID_BASE+0x1a	; internal SIDSW register
SID_LFO_RANDOM_SEED_L	EQU	SID_BASE+0x1b	; internal SIDSW register
SID_LFO_RANDOM_SEED_H	EQU	SID_BASE+0x1c	; internal SIDSW register
SID_FILTER_LFOS		EQU	SID_BASE+0x1d	; internal SIDSW register
SID_FILTER_ENVS		EQU	SID_BASE+0x1e	; internal SIDSW register
SID_ENV_CURVES		EQU	SID_BASE+0x1f	; internal SIDSW register

SID_BASE_END		EQU	0x11f

;; ==========================================================================

SID_SHADOW_BASE		EQU	0x120		; shadow addresses, necessary to determine changes (used: 0x00-0x18)

	;; (SID_SHADOW_BASE + 0x00) to (SID_SHADOW_BASE + 0x18) allocated by shadow registers

SID_WT_STATE		EQU	SID_SHADOW_BASE+0x19	; internal SIDSW register
SID_WT_RATE		EQU	SID_SHADOW_BASE+0x1a	; internal SIDSW register
SID_WT_CLK_REQ_CTR	EQU	SID_SHADOW_BASE+0x1b	; internal SIDSW register
SID_WT_CTR		EQU	SID_SHADOW_BASE+0x1c	; internal SIDSW register
SID_WT_POS		EQU	SID_SHADOW_BASE+0x1d	; internal SIDSW register
SID_WT_PATCH		EQU	SID_SHADOW_BASE+0x1e	; internal SIDSW register
SID_WT_SAVED_PATCH	EQU	SID_SHADOW_BASE+0x1f	; internal SIDSW register

SID_SHADOW_END		EQU	0x13f

;; ==========================================================================

SID_Vx_RECORD_LEN	EQU	32

SID_Vx_MIDI_CHANNEL	EQU	0x00
SID_Vx_SPLIT_LOWER	EQU	0x01
SID_Vx_SPLIT_UPPER	EQU	0x02
SID_Vx_NOTE		EQU	0x03
SID_Vx_MODE		EQU	0x04
SID_Vx_TARGET_FRQ_L	EQU	0x05
SID_Vx_TARGET_FRQ_H	EQU	0x06
SID_Vx_PITCHBENDER	EQU	0x07
SID_Vx_TRANSPOSE	EQU	0x08
SID_Vx_FINETUNE		EQU	0x09
SID_Vx_PITCHRANGE	EQU	0x0a
SID_Vx_PORTA_RATE	EQU	0x0b
SID_Vx_PW_VALUE		EQU	0x0c
SID_Vx_NOTE_DELAY	EQU	0x0d
SID_Vx_NOTE_DELAY_CTR	EQU	0x0e
SID_Vx_ARP_RATE		EQU	0x0f
SID_Vx_ARP_NOTE_NUMBER	EQU	0x10
SID_Vx_ARP_CTR		EQU	0x11
SID_Vx_PITCH_LFOS	EQU	0x12
SID_Vx_PW_LFOS		EQU	0x13
SID_Vx_ENVS		EQU	0x14

SID_Vx_SPARE1		EQU	0x15	; allocated
SID_Vx_SPARE2		EQU	0x16	; only free of constant time portamento not activated
SID_Vx_SPARE3		EQU	0x17	; only free of constant time portamento not activated
SID_Vx_PORTA_FRQ_L      EQU     SID_Vx_SPARE2
SID_Vx_PORTA_FRQ_H	EQU	SID_Vx_SPARE3

SID_Vx_ARP_NOTE_0	EQU	0x18
SID_Vx_ARP_NOTE_1	EQU	0x19
SID_Vx_ARP_NOTE_2	EQU	0x1a
SID_Vx_ARP_NOTE_3	EQU	0x1b
SID_Vx_NOTE_STACK_0	EQU	0x1c
SID_Vx_NOTE_STACK_1	EQU	0x1d
SID_Vx_NOTE_STACK_2	EQU	0x1e
SID_Vx_NOTE_STACK_3	EQU	0x1f
SID_NOTE_STACK_LEN	EQU	SID_Vx_NOTE_STACK_3-SID_Vx_NOTE_STACK_0+1

SID_V1_BASE		EQU	0x140 + 0 * SID_Vx_RECORD_LEN	; 0x0140-0x15f
SID_V2_BASE		EQU	0x140 + 1 * SID_Vx_RECORD_LEN	; 0x0160-0x17f
SID_V3_BASE		EQU	0x140 + 2 * SID_Vx_RECORD_LEN	; 0x0180-0x19f

; DIRTY (no other registers are free): using spare V1 register to store the number of WT ticks
SID_WT_303_TICK_CTR	EQU	SID_V1_BASE + SID_Vx_SPARE1	
; DIRTY (no other registers are free): using spare V2/V3 registers to filter key tracking and 7bit resonance
SID_KEYTRACKING		EQU	SID_V2_BASE + SID_Vx_SPARE1
SID_RESONANCE_7BIT	EQU	SID_V3_BASE + SID_Vx_SPARE1

;; ==========================================================================

SID_LFOx_RECORD_LEN	EQU	7
SID_LFOx_MODE		EQU	0x00
SID_LFOx_RATE		EQU	0x01
SID_LFOx_CTR		EQU	0x02
SID_LFOx_VALUE		EQU	0x03
SID_LFOx_DEPTH		EQU	0x04
SID_LFOx_RVALUE_L	EQU	0x05
SID_LFOx_RVALUE_H	EQU	0x06

SID_LFO1_BASE		EQU	0x1a0 + 0 * SID_LFOx_RECORD_LEN	; 0x01a0-0x1a6
SID_LFO2_BASE		EQU	0x1a0 + 1 * SID_LFOx_RECORD_LEN	; 0x01a7-0x1ad
SID_LFO3_BASE		EQU	0x1a0 + 2 * SID_LFOx_RECORD_LEN	; 0x01ae-0x1b4
SID_LFO4_BASE		EQU	0x1a0 + 3 * SID_LFOx_RECORD_LEN	; 0x01b5-0x1bb
SID_LFO5_BASE		EQU	0x1a0 + 4 * SID_LFOx_RECORD_LEN	; 0x01bc-0x1c2
SID_LFO6_BASE		EQU	0x1a0 + 5 * SID_LFOx_RECORD_LEN	; 0x01c3-0x1c9

;; ==========================================================================

SID_ENVx_RECORD_LEN	EQU	11
SID_ENVx_MODE		EQU	0x00
SID_ENVx_CTR_L		EQU	0x01
SID_ENVx_CTR_H		EQU	0x02
SID_ENVx_ATTACK		EQU	0x03
SID_ENVx_DECAY		EQU	0x04
SID_ENVx_SUSTAIN	EQU	0x05
SID_ENVx_RELEASE	EQU	0x06
SID_ENVx_CURVE		EQU	0x07
SID_ENVx_DEPTH		EQU	0x08
SID_ENVx_RVALUE_L	EQU	0x09
SID_ENVx_RVALUE_H	EQU	0x0a

SID_ENV1_BASE		EQU	0x1ca + 0 * SID_ENVx_RECORD_LEN	; 0x01ca-0x1d4
SID_ENV2_BASE		EQU	0x1ca + 1 * SID_ENVx_RECORD_LEN	; 0x01d5-0x1df

;; ==========================================================================

SID_ASSIGN_VELOCITY	EQU	0x1e0
SID_INIT_VELOCITY	EQU	0x1e1
SID_DEPTH_VELOCITY	EQU	0x1e2
SID_LAST_VELOCITY	EQU	0x1e3

SID_ASSIGN_MODWHEEL	EQU	0x1e4
SID_INIT_MODWHEEL	EQU	0x1e5
SID_DEPTH_MODWHEEL	EQU	0x1e6
SID_LAST_MODWHEEL	EQU	0x1e7

SID_ASSIGN_AFTERTOUCH	EQU	0x1e8
SID_INIT_AFTERTOUCH	EQU	0x1e9
SID_DEPTH_AFTERTOUCH	EQU	0x1ea
SID_LAST_AFTERTOUCH	EQU	0x1eb

;; ==========================================================================

SID_ASSIGN_WT_P1	EQU	0x1ec
SID_ASSIGN_WT_P2	EQU	0x1ed
SID_ASSIGN_WT_P3	EQU	0x1ee

;; ==========================================================================

SID_MIDI_SYNC		EQU	0x1ef
SID_MIDI_SYNC_CTR	EQU	0x1f0
SID_MIDI_SYNC_CTR_REG	EQU	0x1f1

;; ==========================================================================

SID_FC_VALUE		EQU	0x1f2

SID_SW_VOICE		EQU	0x1f3

SID_SW_LFO_NUMBER	EQU	0x1f4
SID_SW_ENV_NUMBER	EQU	SID_SW_LFO_NUMBER

;; ==========================================================================

SID_SE_OPTION		EQU	0x1f5

;; ==========================================================================

SAVED_PRODL		EQU	0x1f6	; used by sid_sw.inc
SAVED_PRODH		EQU	0x1f7	; used by sid_sw.inc

;; ==========================================================================

MUL_A_L			EQU	0x1f8	; used by math_mul16_16.inc
MUL_A_H			EQU	0x1f9
MUL_B_L			EQU	0x1fa
MUL_B_H			EQU	0x1fb
MUL_R_0			EQU	0x1fc
MUL_R_1			EQU	0x1fd
MUL_R_2			EQU	0x1fe
MUL_R_3			EQU	0x1ff

;; ==========================================================================
BANKSTICK_FORMAT_BEGIN	EQU	0x200		; buffer temporary used to format the BankStick
BANKSTICK_FORMAT_END	EQU	0x2ff           ; CS variables have to be initialized again after formatting

#if CS_ENABLED == 1
	;; base address of Control Surface Registers, SID Master
CS_SIDM_BASE		EQU	0x200

	;; base addresses of SID Slaves
CS_SIDS1_BASE		EQU	0x080
CS_SIDS2_BASE		EQU	0x280
CS_SIDS3_BASE		EQU	0x300

	;; offsets to registers (we are using the same like for the presets (sid_presets.inc),
	;; so that SID_SYSEX_TABLE can be reused
CS_SID_NAME_x		EQU	0x00; 0x00-0x0f

CS_SID_VOLUME		EQU	0x10
CS_SID_POLY		EQU	0x11
CS_SID_SUSKEY		EQU	0x12
CS_SID_MIDI_SYNC	EQU	0x13
CS_SID_OSC_PHASE_SYNC	EQU	0x14
CS_SID_ENV_CURVES	EQU	0x15
CS_SID_SE_OPTION	EQU	0x16
CS_SID_WT_PATCH		EQU	0x17
CS_SID_FILTER_CHANNELS	EQU	0x18
CS_SID_FILTER_MODE	EQU	0x19
CS_SID_FILTER_CUTOFF	EQU	0x1a
CS_SID_FILTER_RESONANCE	EQU	0x1b

CS_SID_FILTER_LFO	EQU	0x1c
CS_SID_FILTER_ENV	EQU	0x1d
CS_SID_ENV_1_CURVE	EQU	0x1e
CS_SID_ENV_2_CURVE	EQU	0x1f

CS_SID_VOICE1_OFFSET	EQU	0x20; 0x20-0x2f
CS_SID_VOICE2_OFFSET	EQU	0x30; 0x30-0x3f
CS_SID_VOICE3_OFFSET	EQU	0x40; 0x40-0x4f
CS_SID_VOICEx_TRANSPOSE	EQU	0x00
CS_SID_VOICEx_FINETUNE	EQU	0x01
CS_SID_VOICEx_PITCHRANGE EQU	0x02
CS_SID_VOICEx_PORTAMENTO EQU	0x03
CS_SID_VOICEx_WAVEFORM	EQU	0x04
CS_SID_VOICEx_PULSEWIDTH EQU	0x05
CS_SID_VOICEx_ARPEGGIATOR EQU	0x06
CS_SID_VOICEx_DELAY	EQU	0x07
CS_SID_VOICEx_ATTACK	EQU	0x08
CS_SID_VOICEx_DECAY	EQU	0x09
CS_SID_VOICEx_SUSTAIN	EQU	0x0a
CS_SID_VOICEx_RELEASE	EQU	0x0b
CS_SID_VOICEx_LFO_PITCH	EQU	0x0c
CS_SID_VOICEx_LFO_PW	EQU	0x0d
CS_SID_VOICEx_ENV	EQU	0x0e

CS_SID_VOICE1_SPLIT_L	EQU	0x2f
CS_SID_VOICE2_SPLIT_L	EQU	0x3f
CS_SID_VOICE3_SPLIT_L	EQU	0x4f

CS_SID_VELOCITY_ASSIGN	EQU	0x50
CS_SID_VELOCITY_INIT	EQU	0x51
CS_SID_VELOCITY_DEPTH	EQU	0x52
CS_SID_MODULATION_ASSIGN EQU	0x53
CS_SID_MODULATION_INIT	EQU	0x54
CS_SID_MODULATION_DEPTH	EQU	0x55
CS_SID_AFTERTOUCH_ASSIGN EQU	0x56
CS_SID_AFTERTOUCH_INIT	EQU	0x57
CS_SID_AFTERTOUCH_DEPTH	EQU	0x58
CS_SID_WAVETABLE_RATE	EQU	0x59
CS_SID_WAVETABLE_P1_ASSIGN EQU	0x5a
CS_SID_WAVETABLE_P2_ASSIGN EQU	0x5b
CS_SID_WAVETABLE_P3_ASSIGN EQU	0x5c
CS_SID_VOICE1_SPLIT_U	EQU	0x5d
CS_SID_VOICE2_SPLIT_U	EQU	0x5e
CS_SID_VOICE3_SPLIT_U	EQU	0x5f

CS_SID_LFO1_OFFSET	EQU	0x60; 0x60-0x62
CS_SID_LFO2_OFFSET	EQU	0x63; 0x63-0x65
CS_SID_LFO3_OFFSET	EQU	0x66; 0x66-0x68
CS_SID_LFO4_OFFSET	EQU	0x69; 0x69-0x6b
CS_SID_LFO5_OFFSET	EQU	0x6c; 0x6c-0x6e
CS_SID_LFO6_OFFSET	EQU	0x6f; 0x6f-0x71
CS_SID_LFOx_MODE	EQU	0x00
CS_SID_LFOx_RATE	EQU	0x01
CS_SID_LFOx_DEPTH	EQU	0x02

CS_SID_ENV1_OFFSET	EQU	0x72; 0x72-0x76
CS_SID_ENV2_OFFSET	EQU	0x77; 0x77-0x7b
CS_SID_ENVx_DEPTH	EQU	0x00
CS_SID_ENVx_ATTACK	EQU	0x01
CS_SID_ENVx_DECAY	EQU	0x02
CS_SID_ENVx_SUSTAIN	EQU	0x03
CS_SID_ENVx_RELEASE	EQU	0x04

CS_SID_WT_MODE		EQU	0x7c; (used as buffer, will be mapped to 0x80-0xff)
CS_SID_WT_P1		EQU	0x7d; (used as buffer, will be mapped to 0x80-0xff)
CS_SID_WT_P2		EQU	0x7e; (used as buffer, will be mapped to 0x80-0xff)
CS_SID_WT_P3		EQU	0x7f; (used as buffer, will be mapped to 0x80-0xff)
#endif

;; ==========================================================================

FIP1_BASE		EQU	0x3e0 ; - 0x3e8
FIP2_BASE		EQU	0x3e9 ; - 0x3f1

FIPx_TARGET_VALUE_L	EQU	0x00
FIPx_TARGET_VALUE_H	EQU	0x01
FIPx_VALUE_L		EQU	0x02
FIPx_VALUE_H		EQU	0x03
FIPx_DIV_VALUE_L	EQU	0x04
FIPx_DIV_VALUE_H	EQU	0x05
FIPx_CHANGE_CTR		EQU	0x06
FIPx_PART_CTR		EQU	0x07
FIPx_PART_CTR_RELOAD	EQU	0x08

;; ==========================================================================
