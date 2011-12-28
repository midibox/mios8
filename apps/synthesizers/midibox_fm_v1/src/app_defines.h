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

#define MBFM_STAT_ENGINE_DISABLE	0 ; if set, MBFM SE handler will not be processed
#define MBFM_STAT_FORCE_REFRESH		1 ; if set, a refresh will be forced with next MBFM_SE_Update
#define MBFM_STAT_CC_DUMP_REQ		2 ; if set, a CC dump will be sent
#define MBFM_STAT_SHADOW_UPDATE		3 ; update of shadow parameter requested

#define MBFM_UPDATE0_FLAGS_MULT		0
#define MBFM_UPDATE0_AR_DR		1
#define MBFM_UPDATE0_SL_RR		2
#define MBFM_UPDATE0_CH_FB_CNT		3
#define MBFM_UPDATE0_WS			4
#define MBFM_UPDATE0_FNUML		5
#define MBFM_UPDATE0_FNUMH		6

#define MBFM_UPDATE1_OP1_KSL_TL		0
#define MBFM_UPDATE1_OP2_KSL_TL		1
#define MBFM_UPDATE1_OP3_KSL_TL		2
#define MBFM_UPDATE1_OP4_KSL_TL		3

#define MBFM_UPDATE_DR_GATES		0
#define MBFM_UPDATE_DR_BD_FNUM		1
#define MBFM_UPDATE_DR_HH_FNUM		2
#define MBFM_UPDATE_DR_TOM_FNUM		3
#define MBFM_UPDATE_DR_HH_OPEN		4
#define MBFM_UPDATE_DR_HH_CLOSE		5
#define MBFM_UPDATE_DR_SD		6

#define MBFM_STAT_VOICE_REFRESH		0
#define MBFM_STAT_VOICE_ACTIVE		1
#define MBFM_STAT_GATE_SET_REQ		2
#define MBFM_STAT_GATE_CLR_REQ		3
#define MBFM_STAT_NOTE_UPDATE_REQ	4
#define MBFM_STAT_PORTA_ACTIVE		5
#define MBFM_STAT_UPDATE_NOT_DONE	6
#define MBFM_STAT_UNISONO_2ND_VOICE	7

;; ==========================================================================

#define MBFM_SE_STAT_CYCLE2		0
#define MBFM_SE_STAT_UPDATE_REGS	1
#define MBFM_SE_STAT_FRQ_SWEEP_ACTIVE	2
#define MBFM_SE_STAT_MOD_VALUE_CHANGED	3

;; ==========================================================================

#define MBFM_CTRL1_LEGATO	0 ; if set, legato mode is activated instead of mono mode
#define MBFM_CTRL1_POLY		1 ; if set, poly mode is enabled
#define MBFM_CTRL1_SUS_KEY	2 ; if set, we have "fingered" portamento

#define MBFM_CTRL2_SYNC_WT		0	; sync Wavetable sequencer and arp
#define MBFM_CTRL2_SYNC_LFO		1	; sync LFOs
#define MBFM_CTRL2_SYNC_EG		2	; sync EGs

#define MBFM_CTRL2_SYNC_WT_NOTESTEP	4	; bit #0 of WT sync
#define MBFM_CTRL2_SYNC_WT_FREERUNNING	5	; bit #1 of WT sync

#define MBFM_CTRLE_VEL_MODE0	0 ; velocity mode bit #0
#define MBFM_CTRLE_VEL_MODE1	1 ; velocity mode bit #1

;; ==========================================================================

#define MBFM_LFO_STAT_DEC		0

#define MBFM_EG_STAT_ATTACK		0
#define MBFM_EG_STAT_DECAY1		1
#define MBFM_EG_STAT_DECAY2		2
#define MBFM_EG_STAT_SUSTAIN		3
#define MBFM_EG_STAT_RELEASE		4

;; ==========================================================================

#define MBFM_WT_STATE_STOP		0
#define MBFM_WT_STATE_RESET		1
#define MBFM_WT_STATE_STEP_DONE		2

;; ==========================================================================

#define MBFM_MIDI_SYNC_F8		0	; bit 0 notifies a received MIDI clock
#define	MBFM_MIDI_SYNC_FA		1	; bit 1 notifies a received MIDI clock start
#define MBFM_MIDI_SYNC_TICK_1           2       ; bit 6 notifies that the MBFM SE sequencers should be clocked with common resolution
#define MBFM_MIDI_SYNC_TICK_2           3       ; bit 7 notifies that the MBFM SE sequencers should be clocked with double resolution

;; ==========================================================================

#define BANKSTICK_MAGIC0	0x73 ; magic numbers - if they don't exists in bankstick at BANK_MAGIC_ADDRESS, 
#define BANKSTICK_MAGIC1	0x15 ; the stick will be ignored until it has been formatted

;; ==========================================================================

#if CS_ENABLED == 1
#define CS_STAT_STARTUP_DONE		0	; flag 0: for startup message
#define CS_STAT_DISPLAY_INIT_REQ	1	; flag 1: request a display initialization
#define CS_STAT_DISPLAY_UPDATE_REQ	2	; flag 2: request a display update
#define CS_STAT_DISPLAY_LONGLABEL_REQ	3	; flag 3: request a long label message
#define CS_STAT_CURSOR_FLASH		4	; flag 4: handled by cs_menu_timer.inc
#define CS_STAT_MODIFY_PARAMETER	5	; flag 5: modify parameter if set, else select parameter
#define CS_STAT_MODIFY_NAME		6	; flag 6: modify name if set, else modify parameter
#define CS_STAT_BIG_MSG			7	; flag 7: e.g. for temporary print parameter name

#define CS_MENU_MODE_MERGER		0	; flag 0: Link mode (merger on/off)
#define CS_MENU_MODE_CC			1	; flag 1: CC mode
#define CS_MENU_MODE_MJMP_N		2	; flag 2: jump to menu on matrix button action (low-active)
#define CS_MENU_MODE_INS_OP		3	; flag 3: InsOP buttons select Instrument(=0) or OP(=1)
#define CS_MENU_MODE_CFG		4	; flag 4: show CFG page
#define CS_MENU_MODE_VIEW		5	; flag 5: display view mode

#define CS_MENU_MATRIX_MODE_VERTICAL	0	; alternative vertical mode for modulation matrix LEDs

#define CS_EEPROM_WR_WT_REQ		0	; request a write to WT
#define CS_EEPROM_WR_WT_GNT		1	; grant the request

#endif
;; ==========================================================================

;; ==========================================================================
;;  General MBFM Control Variables in ACCESS page
;; ==========================================================================
MBFM_STAT		EQU	0x010
MBFM_PATCH		EQU	0x011
MBFM_BANK		EQU	0x012
MBFM_DRUMSET		EQU	0x013
MBFM_DBANK		EQU	0x014
MBFM_ENSEMBLE		EQU	0x015
MBFM_EBANK		EQU	0x016
MBFM_BANKSTICK_CHK_CTR	EQU	0x017
MBFM_BANKSTICK_STATUS	EQU	0x018

;; ==========================================================================
;;  Variables used by the control surface
;; ==========================================================================
#if CS_ENABLED == 1
CS_STAT			EQU	0x019	; control surface status
CS_EEPROM_WR		EQU	0x01a	; status of EEPROM write request/grant mechanism
CS_MENU			EQU	0x01b	; menu mode
CS_MENU_TABLE_0_L	EQU	0x01c	; pointer to first table entry, low-byte
CS_MENU_TABLE_0_H	EQU	0x01d	; pointer to first table entry, high-byte
CS_MENU_PAGE_OFFSET	EQU	0x01e	; page offset to first displayed entry
CS_MENU_CURSOR_POS	EQU	0x01f	; cursor position
CS_MENU_ENTRIES		EQU	0x020	; number of menu entries
CS_MENU_TIMER_CTR	EQU	0x021	; used by cs_menu_timer.inc
CS_CURSOR_CTR		EQU	0x022	; used by cs_menu_timer.inc
CS_SELECT_CTR		EQU	0x023	; used by cs_menu.inc, counted by cs_menu_timer.inc
CS_MENU_REFRESH_ITEM_CTR EQU	0x024	; used by cs_menu.inc to count through the displayed items
CS_MENU_ENTRY_TBLPTRL	EQU	0x025	; used by cs_menu.inc
CS_MENU_ENTRY_TBLPTRH	EQU	0x026	; used by cs_menu.inc
CS_MENU_MODE		EQU	0x027	; control mode flags (Link/CC/Edit mode)
CS_MENU_P2R_OR_MASK	EQU	0x028	; temporary used by P2R_R2P function
CS_MENU_P2R_AND_MASK	EQU	0x029	; temporary used by P2R_R2P function
CS_MENU_P2R_R2P_TMP	EQU	0x02a	; temporary used by P2R_R2P function
CS_MENU_WT_STEP		EQU	0x02b	; contains the current WT step
CS_MENU_WT_STEP_BUFFER	EQU	0x02c	; contains the step which corresponds to the buffer content
CS_MENU_WT_MODE		EQU	0x02d	; for the wavetable menu
CS_MENU_WT_P1		EQU	0x02e	; for the wavetable menu
CS_MENU_WT_P2		EQU	0x02f	; for the wavetable menu
CS_MENU_WT_P3		EQU	0x030	; for the wavetable menu
CS_MENU_SAVE_PATCH	EQU	0x031	; contains the number of saved patch
CS_MENU_SAVE_BANK	EQU	0x032	; contains the number of saved patch bank
CS_MENU_SAVE_DBANK	EQU	0x033	; contains the number of saved drumset bank
CS_MENU_SAVE_DRUMSET	EQU	0x034	; contains the number of saved drumset	
CS_MENU_SAVE_EBANK	EQU	0x035	; contains the number of saved ensemble bank
CS_MENU_SAVE_ENSEMBLE	EQU	0x036	; contains the number of saved ensemble
CS_MENU_SAVE_TYPE	EQU	0x037	; contains the selected save type
CS_MENU_SELECTED_INS_FLAGS EQU	0x038	; every flag stays for one instrument
CS_MENU_SELECTED_INS	EQU	0x039	; selected instrument
CS_MENU_NAME_POS	EQU	0x03a	; for the "edit name" mode

CS_MENU_PARAMETER_L	EQU	0x03c	; contains low-byte of current parameter value, must be located to an even address!!!
CS_MENU_PARAMETER_H	EQU	0x03d	; contains high-byte of current parameter value
CS_MENU_PARAMETER_MAX_L	EQU	0x03e	; contains max value of current parameter, must be located to an even address!!!
CS_MENU_PARAMETER_MAX_H	EQU	0x03f	; contains max value of current parameter

SAVED_CS_MENU		EQU	0x040	; used by cs_menu.inc::CS_MENU_ButtonInc
SAVED_CS_MENU_PAGE_OFFSET EQU	0x041	; used by cs_menu.inc::CS_MENU_ButtonInc
SAVED_CS_MENU_CURSOR_POS EQU	0x042	; used by cs_menu.inc::CS_MENU_ButtonInc

CS_MENU_PARAMETER_IX	EQU	0x043	; the parameter index (offset within Ix record) which is modified
CS_MENU_SELECTED_OP_FLAGS EQU	0x044	; every flag stays for one operator
CS_MENU_SELECTED_OP	EQU	0x045	; the selected operator
CS_MENU_SELECTED_LFO	EQU	0x046	; the selected LFO
CS_MENU_SELECTED_BD_HH	EQU	0x047	; the selected bassdrum OP/HH open/closed
CS_MENU_SELECTED_AOUT	EQU	0x048	; the selected AOUT
CS_MENU_MATRIX_CTR	EQU	0x049	; counter used by CS_MENU_MATRIX_Handler
CS_MENU_MATRIX_RBUTTONS EQU	0x04a	; the state of the row buttons
CS_MENU_MATRIX_SELECTED EQU	0x04b	; selected matrix row [3..0] and column [7..4]

CS_MENU_WT_MODIFIED	EQU	0x04c	; each parameter column has it's own flag
CS_MENU_PLAY_INS_FLAGS	EQU	0x04d	; [3:0] play instrument flags
#endif

;; ==========================================================================
#if DISPLAY_PERFORMANCE
PERFORMANCE_LOAD_CTR_L	EQU	0x050	; for mbfm_load.inc
PERFORMANCE_LOAD_CTR_H	EQU	0x051
PERFORMANCE_LOAD_REG_L	EQU	0x052
PERFORMANCE_LOAD_REG_H	EQU	0x053
PERFORMANCE_REF_CTR	EQU	0x054
#else
METER_VALUE_I1		EQU	0x050   ; for cs_menu_matrix.inc
METER_VALUE_I2		EQU	0x051
METER_VALUE_I3		EQU	0x052
METER_VALUE_I4		EQU	0x053
METER_I_ACTIVITY_FLAGS  EQU	0x054
#endif

#if USE_CUSTOM_CHAR_HANDLER
LAST_CHARSET 	EQU 	0x055	
#endif

#if CS_SAMMICH_FM
SAMMICH_FLAGS 	EQU 	0x056	
SAMMICH_BAK   	EQU 	0x057	
SOURCE_BUTTON	EQU		0
#endif

	;; free: (0x55)0x58-0x5b

;; ==========================================================================
GATES			EQU	0x05c
GATES_LAST		EQU	0x05d

;; ==========================================================================
;; AOUT registers used by $MIOS_PATH/modules/aout.inc
AOUT_INVERTED		EQU	0x05e
AOUT_UPDATE_REQ		EQU	0x05f
AOUT_VALUES		EQU	0x060;..0x6f

;; ==========================================================================
;; MBFM specific aout registers
;; don't change structure - it's stored this way in BankStick (-> see mbfm_bank.inc)
MBFM_AOUT_BASE		EQU	0x070;..0x7f
MBFM_AOUT_RECORD_LEN	EQU	0x02

MBFM_AOUTx_ASSIGN	EQU	0x00	; AOUT channel assignment
MBFM_AOUTx_OFFSET	EQU	0x01	; 7bit offset of channel

;; ==========================================================================

MBFM_BASE		EQU	0x080

MBFM_CFG0		EQU	0x080	; MIDI configuration
MBFM_CFG1		EQU	0x081	; note handler configuration

MBFM_DRUM_MIDI_CHANNEL	EQU	0x082

MBFM_MIDI_DEVICE	EQU	0x083	; used by mbfm_sysex.inc
MBFM_SYSEX_STATE	EQU	0x084   ; used by mbfm_sysex.inc
MBFM_SYSEX_STATE2	EQU	0x085	; used by mbfm_sysex.inc
MBFM_SYSEX_ACTION	EQU	0x086	; used by mbfm_sysex.inc
MBFM_SYSEX_IN		EQU	0x087	; used by mbfm_sysex.inc
MBFM_SYSEX_CHECKSUM	EQU	0x088	; used by mbfm_sysex.inc
MBFM_SYSEX_ADDRESS	EQU	0x089	; used by mbfm_sysex.inc
MBFM_SYSEX_TYPE		EQU	0x08a	; used by mbfm_sysex.inc

MBFM_CURRENT_VOICE	EQU	0x08b	; current voice which will be handled by cv_midi.inc and cv_map.inc
MBFM_CURRENT_INSTRUMENT	EQU	0x08c	; current instrument which will be handled by cv_midi.inc and cv_map.inc
MBFM_CURRENT_CHANNEL	EQU	0x08d
MBFM_MIDI_PARAMETER1	EQU	0x08e
MBFM_MIDI_PARAMETER2	EQU	0x08f

MBFM_SE_STAT		EQU	0x090		
MBFM_SE_VOICE_CTR	EQU	0x091
MBFM_SE_OP		EQU	0x092
MBFM_SE_FRQ_L		EQU	0x093
MBFM_SE_FRQ_H		EQU	0x094
MBFM_SE_MOD_VALUE	EQU	0x095
MBFM_SE_TARGET_FRQ_L	EQU	0x096
MBFM_SE_TARGET_FRQ_H	EQU	0x097
MBFM_SE_MOD_RESULT_L	EQU	0x098
MBFM_SE_MOD_RESULT_H	EQU	0x099
MBFM_SE_MOD_RESULT_U	EQU	0x09a

MBFM_REG_VOICE_OFFSET	EQU	0x09b

MBFM_DRUM_GATES_TRIGGER	EQU	0x09c
MBFM_UPDATE_DR		EQU	0x09d

MBFM_MIDI_SYNC		EQU	0x09e
MBFM_MIDI_SYNC_CTR	EQU	0x09f
MBFM_MIDI_SYNC_CTR_REG	EQU	0x0a0

MBFM_RANDOM_SEED_L	EQU	0x0a1
MBFM_RANDOM_SEED_H	EQU	0x0a2

;; -----------------------------------

SAVED_PRODL		EQU	0x0a4	; used by mbfm_se.inc
SAVED_PRODH		EQU	0x0a5	; used by mbfm_se.inc
SAVED_FSR0L		EQU	0x0a6	; used by mbfm_se.inc
SAVED_FSR0H		EQU	0x0a7	; used by mbfm_se.inc

MUL_A_L			EQU	0x0a8	; used by math_mul16_16.inc
MUL_A_H			EQU	0x0a9	; used by math_mul16_16.inc
MUL_B_L			EQU	0x0aa	; used by math_mul16_16.inc
MUL_B_H			EQU	0x0ab	; used by math_mul16_16.inc
MUL_R_0			EQU	0x0ac	; used by math_mul16_16.inc
MUL_R_1			EQU	0x0ad	; used by math_mul16_16.inc
MUL_R_2			EQU	0x0ae	; used by math_mul16_16.inc
MUL_R_3			EQU	0x0af	; used by math_mul16_16.inc

;; ==========================================================================
MBFM_VOICE_QUEUE_0	EQU	0x0b0
MBFM_VOICE_QUEUE_5	EQU	0x0b5
MBFM_VOICE_QUEUE_LEN	EQU	MBFM_VOICE_QUEUE_5 - MBFM_VOICE_QUEUE_0 + 1

;; ==========================================================================

MBFM_DR_BASE		EQU	0x0c0

MBFM_DR_BD_M_FLAGS_MULT	EQU	0x0c0
MBFM_DR_BD_C_FLAGS_MULT	EQU	0x0c1
MBFM_DR_HH_O_FLAGS_MULT	EQU	0x0c2
MBFM_DR_HH_C_FLAGS_MULT	EQU	0x0c3
MBFM_DR_SD_FLAGS_MULT	EQU	0x0c4
MBFM_DR_TOM_FLAGS_MULT	EQU	0x0c5
MBFM_DR_CYM_FLAGS_MULT	EQU	0x0c6

MBFM_DR_BD_M_KSL_TL	EQU	0x0c7
MBFM_DR_BD_C_KSL_TL	EQU	0x0c8
MBFM_DR_HH_O_KSL_TL	EQU	0x0c9
MBFM_DR_HH_C_KSL_TL	EQU	0x0ca
MBFM_DR_SD_KSL_TL	EQU	0x0cb
MBFM_DR_TOM_KSL_TL	EQU	0x0cc
MBFM_DR_CYM_KSL_TL	EQU	0x0cd

MBFM_DR_BD_M_AR_DR	EQU	0x0ce
MBFM_DR_BD_C_AR_DR	EQU	0x0cf
MBFM_DR_HH_O_AR_DR	EQU	0x0d0
MBFM_DR_HH_C_AR_DR	EQU	0x0d1
MBFM_DR_SD_AR_DR	EQU	0x0d2
MBFM_DR_TOM_AR_DR	EQU	0x0d3
MBFM_DR_CYM_AR_DR	EQU	0x0d4

MBFM_DR_BD_M_SL_RR	EQU	0x0d5
MBFM_DR_BD_C_SL_RR	EQU	0x0d6
MBFM_DR_HH_O_SL_RR	EQU	0x0d7
MBFM_DR_HH_C_SL_RR	EQU	0x0d8
MBFM_DR_SD_SL_RR	EQU	0x0d9
MBFM_DR_TOM_SL_RR	EQU	0x0da
MBFM_DR_CYM_SL_RR	EQU	0x0db

MBFM_DR_BD_M_WS		EQU	0x0dc
MBFM_DR_BD_C_WS		EQU	0x0dd
MBFM_DR_HH_O_WS		EQU	0x0de
MBFM_DR_HH_C_WS		EQU	0x0df
MBFM_DR_SD_WS		EQU	0x0e0
MBFM_DR_TOM_WS		EQU	0x0e1
MBFM_DR_CYM_WS		EQU	0x0e2

MBFM_DR_BD_CH_FB_CNT	EQU	0x0e3
MBFM_DR_HH_SD_CH_FB_CNT	EQU	0x0e4
MBFM_DR_CYM_TOM_CH_FB_CNT EQU	0x0e5

MBFM_DR_BD_FRQ		EQU	0x0e6
MBFM_DR_BD_DECR		EQU	0x0e7
MBFM_DR_HH_FRQ		EQU	0x0e8
MBFM_DR_HH_DECR		EQU	0x0e9
MBFM_DR_TOM_FRQ		EQU	0x0ea
MBFM_DR_TOM_DECR	EQU	0x0eb

MBFM_DR_BD_SPLIT_LOWER	EQU	0x0ec
MBFM_DR_BD_SPLIT_UPPER	EQU	0x0ed
MBFM_DR_HH_O_SPLIT_LOWER EQU	0x0ee
MBFM_DR_HH_O_SPLIT_UPPER EQU	0x0ef
MBFM_DR_HH_C_SPLIT_LOWER EQU	0x0f0
MBFM_DR_HH_C_SPLIT_UPPER EQU	0x0f1
MBFM_DR_SD_SPLIT_LOWER	EQU	0x0f2
MBFM_DR_SD_SPLIT_UPPER	EQU	0x0f3
MBFM_DR_TOM_SPLIT_LOWER	EQU	0x0f4
MBFM_DR_TOM_SPLIT_UPPER	EQU	0x0f5
MBFM_DR_CYM_SPLIT_LOWER	EQU	0x0f6
MBFM_DR_CYM_SPLIT_UPPER	EQU	0x0f7

	;; last address which should be stored in BankStick. Everything else will be set to zero
MBFM_DR_LAST_BANKSTICK_ADDR EQU	MBFM_DR_CYM_SPLIT_UPPER - MBFM_DR_BASE

	;; free 0xf8
MBFM_PAR_IX		EQU	0x0f9

MBFM_DR_BD_FRQL		EQU	0x0fa
MBFM_DR_BD_FRQH		EQU	0x0fb
MBFM_DR_HH_FRQL		EQU	0x0fc
MBFM_DR_HH_FRQH		EQU	0x0fd
MBFM_DR_TOM_FRQL	EQU	0x0fe
MBFM_DR_TOM_FRQH	EQU	0x0ff

;; ==========================================================================
;; instrument memory storage
;; one instrument record allocates 0x80 = 128 bytes
MBFM_Ix_NUM		EQU	4
MBFM_Ix_RECORD_LEN	EQU	0x80

MBFM_Ix_NAME_x		EQU	0x00 ; -0x10
MBFM_Ix_OP1_FLAGS_MULT	EQU	0x10
MBFM_Ix_OP2_FLAGS_MULT	EQU	0x11
MBFM_Ix_OP3_FLAGS_MULT	EQU	0x12
MBFM_Ix_OP4_FLAGS_MULT	EQU	0x13
MBFM_Ix_OP1_KSL_TL	EQU	0x14
MBFM_Ix_OP2_KSL_TL	EQU	0x15
MBFM_Ix_OP3_KSL_TL	EQU	0x16
MBFM_Ix_OP4_KSL_TL	EQU	0x17
MBFM_Ix_OP1_AR_DR	EQU	0x18
MBFM_Ix_OP2_AR_DR	EQU	0x19
MBFM_Ix_OP3_AR_DR	EQU	0x1a
MBFM_Ix_OP4_AR_DR	EQU	0x1b
MBFM_Ix_OP1_SL_RR	EQU	0x1c
MBFM_Ix_OP2_SL_RR	EQU	0x1d
MBFM_Ix_OP3_SL_RR	EQU	0x1e
MBFM_Ix_OP4_SL_RR	EQU	0x1f
MBFM_Ix_OP1_WS		EQU	0x20
MBFM_Ix_OP2_WS		EQU	0x21
MBFM_Ix_OP3_WS		EQU	0x22
MBFM_Ix_OP4_WS		EQU	0x23
MBFM_Ix_OP12_CH_FB_CNT	EQU	0x24
MBFM_Ix_OP34_CH_FB_CNT	EQU	0x25
MBFM_Ix_CTRL1		EQU	0x26
MBFM_Ix_CTRL2		EQU	0x27
MBFM_Ix_AOUT_MOD_ASSGN	EQU	0x28
MBFM_Ix_FINETUNE	EQU	0x29
MBFM_Ix_PITCHRANGE	EQU	0x2a
MBFM_Ix_PORTA_RATE	EQU	0x2b
MBFM_Ix_VELOCITY_ASSIGN	EQU	0x2c
MBFM_Ix_VELOCITY_INIT	EQU	0x2d
MBFM_Ix_VELOCITY_DEPTH	EQU	0x2e
MBFM_Ix_AFTERTOUCH_ASSIGN EQU	0x2f
MBFM_Ix_AFTERTOUCH_INIT	EQU	0x30
MBFM_Ix_AFTERTOUCH_DEPTH EQU	0x31
MBFM_Ix_MODWHEEL_ASSIGN	EQU	0x32
MBFM_Ix_MODWHEEL_INIT	EQU	0x33
MBFM_Ix_MODWHEEL_DEPTH	EQU	0x34
MBFM_Ix_WT_RATE		EQU	0x35
MBFM_Ix_WT_PAR1		EQU	0x36
MBFM_Ix_WT_PAR2		EQU	0x37
MBFM_Ix_WT_PAR3		EQU	0x38
MBFM_Ix_LFO1_MODE	EQU	0x39
MBFM_Ix_LFO1_PHASE	EQU	0x3a
MBFM_Ix_LFO1_RATE	EQU	0x3b
MBFM_Ix_LFO1_DEPTH_P	EQU	0x3c
MBFM_Ix_LFO1_DEPTH_V1	EQU	0x3d
MBFM_Ix_LFO1_DEPTH_V2	EQU	0x3e
MBFM_Ix_LFO1_DEPTH_V3	EQU	0x3f
MBFM_Ix_LFO1_DEPTH_V4	EQU	0x40
MBFM_Ix_LFO1_DEPTH_L	EQU	0x41
MBFM_Ix_LFO1_DEPTH_A	EQU	0x42
MBFM_Ix_LFO2_MODE	EQU	0x43
MBFM_Ix_LFO2_PHASE	EQU	0x44
MBFM_Ix_LFO2_RATE	EQU	0x45
MBFM_Ix_LFO2_DEPTH_P	EQU	0x46
MBFM_Ix_LFO2_DEPTH_V1	EQU	0x47
MBFM_Ix_LFO2_DEPTH_V2	EQU	0x48
MBFM_Ix_LFO2_DEPTH_V3	EQU	0x49
MBFM_Ix_LFO2_DEPTH_V4	EQU	0x4a
MBFM_Ix_LFO2_DEPTH_L	EQU	0x4b
MBFM_Ix_LFO2_DEPTH_A	EQU	0x4c
MBFM_Ix_EG5_MODE	EQU	0x4d
MBFM_Ix_EG5_ATTACK	EQU	0x4e
MBFM_Ix_EG5_ATTLVL	EQU	0x4f
MBFM_Ix_EG5_DECAY1	EQU	0x50
MBFM_Ix_EG5_DECLVL	EQU	0x51
MBFM_Ix_EG5_DECAY2	EQU	0x52
MBFM_Ix_EG5_SUSTAIN	EQU	0x53
MBFM_Ix_EG5_RELEASE	EQU	0x54
MBFM_Ix_EG5_CURVE	EQU	0x55
MBFM_Ix_EG5_DEPTH_P	EQU	0x56
MBFM_Ix_EG5_DEPTH_V1	EQU	0x57
MBFM_Ix_EG5_DEPTH_V2	EQU	0x58
MBFM_Ix_EG5_DEPTH_V3	EQU	0x59
MBFM_Ix_EG5_DEPTH_V4	EQU	0x5a
MBFM_Ix_EG5_DEPTH_A	EQU	0x5b
MBFM_Ix_EG5_DEPTH_L	EQU	0x5c

MBFM_Ix_BANK		EQU	0x60
MBFM_Ix_PATCH		EQU	0x61
MBFM_Ix_MIDI_CHANNEL	EQU	0x62
MBFM_Ix_SPLIT_LOWER	EQU	0x63
MBFM_Ix_SPLIT_UPPER	EQU	0x64
MBFM_Ix_CTRLE		EQU	0x65
MBFM_Ix_VOLUME		EQU	0x66
MBFM_Ix_TRANSPOSE	EQU	0x67
MBFM_Ix_UNISONO		EQU	0x68
MBFM_Ix_ACHN_OP12	EQU	0x69
MBFM_Ix_ACHN_OP34	EQU	0x6a

MBFM_Ix_ENSEMBLE_BEGIN	EQU	MBFM_Ix_BANK	; used by mbfm_bank.inc
MBFM_Ix_ENSEMBLE_END	EQU	MBFM_Ix_ACHN_OP34

MBFM_Ix_MODWHEEL	EQU	0x6f
MBFM_Ix_VELOCITY	EQU	0x70
MBFM_Ix_AFTERTOUCH	EQU	0x71
MBFM_Ix_PITCHBENDER	EQU	0x72
MBFM_Ix_LAST_VOICE	EQU	0x73
MBFM_Ix_WT_STATE	EQU	0x74
MBFM_Ix_WT_CTR		EQU	0x75
MBFM_Ix_WT_POS		EQU	0x76
MBFM_Ix_WT_CLK_REQ_CTR	EQU	0x77

MBFM_Ix_NOTE_STACK_0	EQU	0x78
MBFM_Ix_NOTE_STACK_1	EQU	0x79
MBFM_Ix_NOTE_STACK_2	EQU	0x7a
MBFM_Ix_NOTE_STACK_3	EQU	0x7b
MBFM_Ix_NOTE_STACK_4	EQU	0x7c
MBFM_Ix_NOTE_STACK_5	EQU	0x7d
MBFM_Ix_NOTE_STACK_6	EQU	0x7e
MBFM_Ix_NOTE_STACK_7	EQU	0x7f


	;; base addresses of instrument records
MBFM_I1_BASE		EQU	0x100 + 0 * MBFM_Ix_RECORD_LEN	; 0x100-0x17f
MBFM_I2_BASE		EQU	0x100 + 1 * MBFM_Ix_RECORD_LEN	; 0x180-0x1ff
MBFM_I3_BASE		EQU	0x100 + 2 * MBFM_Ix_RECORD_LEN	; 0x200-0x27f
MBFM_I4_BASE		EQU	0x100 + 3 * MBFM_Ix_RECORD_LEN	; 0x280-0x2ff

MBFM_Ix_NOTE_STACK_LEN	EQU	MBFM_Ix_NOTE_STACK_7 - MBFM_Ix_NOTE_STACK_0 + 1

;; ==========================================================================

CS_FM_WT_PATCH		EQU	0x17
CS_FM_WT_MODE		EQU	0x7c; (used as buffer, will be mapped to 0x80-0xff)
CS_FM_WT_P1		EQU	0x7d; (used as buffer, will be mapped to 0x80-0xff)
CS_FM_WT_P2		EQU	0x7e; (used as buffer, will be mapped to 0x80-0xff)
CS_FM_WT_P3		EQU	0x7f; (used as buffer, will be mapped to 0x80-0xff)

;; ==========================================================================
;; NOTE: MBFM_Px_xxx defined in mbfm_par_table.inc!
;; ==========================================================================

;; ==========================================================================
;; voice memory storage
;; one voice record allocates 0x20 = 32 bytes
MBFM_Vx_NUM		EQU	6
MBFM_Vx_RECORD_LEN	EQU	0x20

MBFM_Vx_UPDATE0		EQU	0x00
MBFM_Vx_UPDATE1		EQU	0x01
MBFM_Vx_FNUML		EQU	0x02
MBFM_Vx_FNUMH		EQU	0x03
MBFM_Vx_OP1_KSL_TL	EQU	0x04
MBFM_Vx_OP2_KSL_TL	EQU	0x05
MBFM_Vx_OP3_KSL_TL	EQU	0x06
MBFM_Vx_OP4_KSL_TL	EQU	0x07
MBFM_Vx_OP1_VOL		EQU	0x08
MBFM_Vx_OP2_VOL		EQU	0x09
MBFM_Vx_OP3_VOL		EQU	0x0a
MBFM_Vx_OP4_VOL		EQU	0x0b
MBFM_Vx_STAT		EQU	0x0c
MBFM_Vx_NOTE		EQU	0x0d
MBFM_Vx_VELOCITY	EQU	0x0e
MBFM_Vx_ASSIGNED_INSTR	EQU	0x0f
MBFM_Vx_FRQ_L		EQU	0x10
MBFM_Vx_FRQ_H		EQU	0x11
MBFM_Vx_TARGET_FRQ_L	EQU	0x12
MBFM_Vx_TARGET_FRQ_H	EQU	0x13
MBFM_Vx_LFO1_STAT	EQU	0x14
MBFM_Vx_LFO1_RATE	EQU	0x15
MBFM_Vx_LFO1_CTR	EQU	0x16
MBFM_Vx_LFO1_VALUE	EQU	0x17
MBFM_Vx_LFO2_STAT	EQU	0x18
MBFM_Vx_LFO2_RATE	EQU	0x19
MBFM_Vx_LFO2_CTR	EQU	0x1a
MBFM_Vx_LFO2_VALUE	EQU	0x1b
MBFM_Vx_EG5_STAT	EQU	0x1c
MBFM_Vx_EG5_CTR_L	EQU	0x1d
MBFM_Vx_EG5_CTR_H	EQU	0x1e

MBFM_V1_BASE		EQU	0x300 + 0 * MBFM_Vx_RECORD_LEN	; 0x300
MBFM_V2_BASE		EQU	0x300 + 1 * MBFM_Vx_RECORD_LEN	; 0x320
MBFM_V3_BASE		EQU	0x300 + 2 * MBFM_Vx_RECORD_LEN	; 0x340
MBFM_V4_BASE		EQU	0x300 + 3 * MBFM_Vx_RECORD_LEN	; 0x360
MBFM_V5_BASE		EQU	0x300 + 4 * MBFM_Vx_RECORD_LEN	; 0x380
MBFM_V6_BASE		EQU	0x300 + 5 * MBFM_Vx_RECORD_LEN	; 0x3a0

;; ==========================================================================
#if WAVETABLES_IN_RAM
MBFM_I1_WAVETABLE_BASE	EQU	0x600
MBFM_I2_WAVETABLE_BASE	EQU	0x680
MBFM_I3_WAVETABLE_BASE	EQU	0x700
MBFM_I4_WAVETABLE_BASE	EQU	0x780
#else
MBFM_EE_BUFFER		EQU	0x300		; buffer temporary used for writing WT data into EEPROM
						; voices have to be re-initialized after use
#endif


#if USE_SHADOW_BUFFERS
	;; shadow addresses of instrument records
MBFM_I1_SHADOW		EQU	0x800 + 0 * MBFM_Ix_RECORD_LEN	; 0x800-0x87f
MBFM_I2_SHADOW		EQU	0x800 + 1 * MBFM_Ix_RECORD_LEN	; 0x880-0x8ff
MBFM_I3_SHADOW		EQU	0x800 + 2 * MBFM_Ix_RECORD_LEN	; 0x900-0x97f
MBFM_I4_SHADOW		EQU	0x800 + 3 * MBFM_Ix_RECORD_LEN	; 0x980-0x9ff
#endif
