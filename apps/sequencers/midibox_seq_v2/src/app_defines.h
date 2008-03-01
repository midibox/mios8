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

#define DEFAULT_DEVICE_ID		0x00	; the default device ID

;; ==========================================================================

#define MB_STAT_DONT_FLASH_L_GP_DOUT	0 ; if set, the 8 GP leds on the left side won't be flashed
#define MB_STAT_DONT_FLASH_R_GP_DOUT	1 ; if set, the 8 GP leds on the right side won't be flashed
#define MB_STAT_INFO_IN_SONG_PAGE	2 ; tmp. flag used by SEQ_LCD_PrintInfo
#define MB_STAT_USE_S_BANKSTICK_NEVER	3 ; if set, the song bankstick will never be used (32k BS has been connected)
#define MB_STAT_USE_S_BANKSTICK		4 ; if set, the song bankstick will be used by the BANK_ routines
#define MB_STAT_S_BANKSTICK_AVAILABLE	5 ; if set, the song bankstick is available
#define MB_STAT_USE_BANKSTICK		6 ; if set, the bankstick will be used by the BANK_ routines
#define MB_STAT_BANKSTICK_AVAILABLE	7 ; if set, the bankstick is available

;; ==========================================================================

#define BANKSTICK_MAGIC0	0x36 ; magic numbers - if they don't exists in bankstick at BANK_MAGIC_ADDRESS, 
#define BANKSTICK_MAGIC1	0x28 ; the stick will be ignored until it has been formatted

#define BANKSTICK_SONG_MAGIC0	0x7a ; the same for the song sector (or second bankstick)
#define BANKSTICK_SONG_MAGIC1	0x5c

;; ==========================================================================

#define SEQ_CFG0_MERGEROFF	0 ; if set, merger is disabled
#define SEQ_CFG0_MBLINK		1 ; if set, the MIDIbox Link is enabled

#define SEQ_CFG0_ARP_RESET      4 ; if set, arp tracks will be reset if no note is played
#define SEQ_CFG0_PATTERN_SYNCH  5 ; if set, pattern changes will be synchronized

;; ==========================================================================

#define CS_STAT_DISPLAY_INIT_REQ	0	; request a display initialization
#define CS_STAT_DISPLAY_UPDATE_REQ	1	; request a display update
#define CS_STAT_CURSOR_FLASH		2	; handled by cs_menu_timer.inc
#define CS_STAT_WITHIN_PAGE		3	; if 0, show menu, else go into menu page
#define CS_STAT_DELAYED_CALLBACK	4	; if set, the CALLBACK function will be executed after 2 seconds
#define CS_STAT_MESSAGE_REQ		5	; if set, a CS message will be print for some seconds
#define CS_STAT_MESSAGE_HOLD		6	; if set, the CS message will stay active
#define CS_STAT_STEP_DISPLAY_UPDATE	7	; if set, display will be updated on every sequencer step (used by View menu, Page #1 only)

;; ==========================================================================

#define CS_MENU_CHARSET_ARROWS		0x01	; common charset
#define CS_MENU_CHARSET_VBARS		0x02	; horizontal bars
#define CS_MENU_CHARSET_HBARS		0x03	; vertical bars

;; ==========================================================================

	;; free memory for user applications:	
	;; 0x010-0x37f

;; ==========================================================================
;;  General MBSEQ Control Variables in ACCESS page
;; ==========================================================================
MB_STAT			EQU	0x010
MB_BS_CHK_CTR		EQU	0x011
MIDI_DEVICE_ID		EQU	0x012

;; used by midi_evnt.inc
MIDI_EVNT0		EQU	0x013
MIDI_EVNT1		EQU	0x014
MIDI_EVNT_VALUE		EQU	0x015

;; used by midi_rxtx.inc
MIDI_RXTX_RX_CTR	EQU	0x016
MIDI_RXTX_TX_CTR	EQU	0x017

;; free: 0x018-0x01f

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

;; ==========================================================================
	
;; this 64 byte buffer is used by SEQ_BANK_Copy, SEQ_DUMP_Restore* and SEQ_DUMP_Store* for fast data transfers
SEQ_DATA_BUFFER		EQU	0x040
	;; ...
SEQ_DATA_BUFFER_END	EQU	0x07f

;; ==========================================================================

;; (pot layer A located to 0x1c0-0x1ff --- see code below)
;; -----------------------------------
;; pot layer B
SEQ_POT_VALUES_B_00     EQU     0x080
        ;; ...
SEQ_POT_VALUES_B_3F     EQU     0x0bf

;; -----------------------------------
;; pot layer C
SEQ_POT_VALUES_C_00     EQU     0x0c0
        ;; ...
SEQ_POT_VALUES_C_3F     EQU     0x0ff

;; ==========================================================================

SEQ_BASE		EQU	0x100

SEQ_CFG0		EQU	0x100	; merger configuration
SEQ_MIDI_CHANNEL	EQU	0x101	; channel which is used for transpose/arpeggiator function
SEQ_PATTERN		EQU	0x102	; selects pattern for SEQ_BANK* functions
SEQ_SBANK_SECTOR	EQU	0x103	; used by SEQ_SBANK to select the sector
SEQ_GROUP		EQU	0x104	; selects the track group (pattern 0-3)
SEQ_SELECTED_LAYERS	EQU	0x105	; bit 2-0: selected layers
SEQ_SELECTED_TRKS	EQU	0x106	; bit 3-0: selected tracks, bit 7-4: used as tmp. storage when sel button pressed
SEQ_CURRENT_STEP	EQU	0x107	; the currently selected step (only one a time)
SEQ_EVNTS		EQU	0x108	; selects the step which is used by seq_fsr.inc functions
SEQ_EVNTT		EQU	0x109	; selects the track which is used by seq_core.inc and seq_fsr.inc functions
SEQ_EVNTD		EQU	0x10a	; selects the drum layer - only used in drum mode by seq_core.inc and seq_evnt.inc

SEQ_PATTERN0		EQU	0x10b	; holds the first pattern which is played by seq_core.inc
SEQ_PATTERN1		EQU	0x10c	; holds the second pattern which is played by seq_core.inc
SEQ_PATTERN2		EQU	0x10d	; holds the third pattern which is played by seq_core.inc
SEQ_PATTERN3		EQU	0x10e	; holds the fourth pattern which is played by seq_core.inc

SEQ_NEXT_PATTERN0	EQU	0x10f	; selects the next SEQ_PATTERN0
SEQ_NEXT_PATTERN1	EQU	0x110	; selects the next SEQ_PATTERN1
SEQ_NEXT_PATTERN2	EQU	0x111	; selects the next SEQ_PATTERN2
SEQ_NEXT_PATTERN3	EQU	0x112	; selects the next SEQ_PATTERN3

SEQ_SONG_POS		EQU	0x113	; current song position
SEQ_SONG_LOOP		EQU	0x114	; current loop counter (begins with MAX value, will be decremented)
SEQ_SONG_LOOP_MAX	EQU	0x115	; number of loops

SEQ_REQ			EQU	0x116	; see seq_core.inc
SEQ_STATE		EQU	0x117	; see seq_core.inc
SEQ_CLK_STEP_CTR	EQU	0x118	; step reference counter (0-15)
SEQ_CLK_TICK_CTR	EQU	0x119	; clock reference counter (0-24)

SEQ_MODE0		EQU	0x11a	; used by seq_core.inc
SEQ_MODE1		EQU	0x11b	; used by seq_core.inc

SEQ_PTYPE		EQU	0x11c	; see seq_ptype.inc

SEQ_RANDOM_SEED_L	EQU	0x11d	; current random value used by seq_core.inc
SEQ_RANDOM_SEED_H	EQU	0x11e	; current random value used by seq_core.inc

SEQ_TRKS_MUTED0		EQU	0x11f	; muted tracks (low-byte)
SEQ_TRKS_MUTED1		EQU	0x120	; muted tracks (high-byte)

SEQ_BPM			EQU	0x121	; see seq_bpm.inc
SEQ_CLK_REQ_CTR		EQU	0x122	; see seq_bpm.inc
SEQ_INCOMING_CLK_DELAY	EQU	0x123	; see seq_bpm.inc
SEQ_INCOMING_CLK_CTR	EQU	0x124	; see seq_bpm.inc
SEQ_SENT_CLK_DELAY	EQU	0x125	; see seq_bpm.inc
SEQ_SENT_CLK_CTR	EQU	0x126	; see seq_bpm.inc

SEQ_EVNTASSGN		EQU	0x127	; used by seq_evnt.inc
SEQ_EVNTOFFSET		EQU	0x128	; used by seq_evnt.inc
SEQ_EVNT0		EQU	0x129	; used by seq_evnt.inc
SEQ_EVNT1		EQU	0x12a	; used by seq_evnt.inc
SEQ_EVNT2		EQU	0x12b	; used by seq_evnt.inc
SEQ_EVNTL		EQU	0x12c	; used by seq_evnt.inc
SEQ_EVNTTHRS		EQU	0x12d	; used by seq_evnt.inc

SEQ_METRONOME_CHANNEL	EQU	0x12e	; channel of metronome
SEQ_METRONOME_OFF_EVNT0	EQU	0x12f	; stores the metronome OFF event, byte #0
SEQ_METRONOME_OFF_EVNT1	EQU	0x130	; stores the metronome OFF event, byte #1

SEQ_NOTE_STACK_0	EQU	0x131	; stack for incoming notes (used by transpose function)
SEQ_NOTE_STACK_1	EQU	0x132
SEQ_NOTE_STACK_2	EQU	0x133
SEQ_NOTE_STACK_3	EQU	0x134
SEQ_NOTE_STACK_LEN	EQU	(SEQ_NOTE_STACK_3-SEQ_NOTE_STACK_0)+1

SEQ_NOTE_STACK_0_HOLD	EQU	0x135	; hold stack for incoming notes (used by transpose function)
SEQ_NOTE_STACK_1_HOLD	EQU	0x136
SEQ_NOTE_STACK_2_HOLD	EQU	0x137
SEQ_NOTE_STACK_3_HOLD	EQU	0x138

SEQ_ARP_NOTE_0		EQU	0x139	; sorted SEQ_NOTE_STACK_x (used by arpeggiator function)
SEQ_ARP_NOTE_1		EQU	0x13a
SEQ_ARP_NOTE_2		EQU	0x13b
SEQ_ARP_NOTE_3		EQU	0x13c

SEQ_ARP_NOTE_0_HOLD	EQU	0x13d	; sorted SEQ_NOTE_STACK_x_HOLD (used by arpeggiator function)
SEQ_ARP_NOTE_1_HOLD	EQU	0x13e
SEQ_ARP_NOTE_2_HOLD	EQU	0x13f
SEQ_ARP_NOTE_3_HOLD	EQU	0x140

SEQ_SYSEX_STATE		EQU	0x141	; used by seq_sysex.inc
SEQ_SYSEX_STATE2	EQU	0x142	; used by seq_sysex.inc
SEQ_SYSEX_ACTION	EQU	0x143	; used by seq_sysex.inc
SEQ_SYSEX_IN		EQU	0x144	; used by seq_sysex.inc
SEQ_SYSEX_CHECKSUM	EQU	0x145	; used by seq_sysex.inc
SEQ_SYSEX_ADDRESS	EQU	0x146	; used by seq_sysex.inc

SEQ_MIDIPOS_STATE	EQU	0x147	; used by the SEQ_MIDI_ClkPosParser
SEQ_MIDIPOS_LSB		EQU	0x148	; used by the SEQ_MIDI_ClkPosParser
SEQ_MIDIPOS_MSB		EQU	0x149	; used by the SEQ_MIDI_ClkPosParser

SEQ_SONG		EQU	0x14a	; used by CS_M_SONG* only, don't change the order!
SEQ_EDIT_SONG_POS	EQU	0x14b
SEQ_EDIT_SONG_PLAY	EQU	0x14c
SEQ_EDIT_SONG_P0	EQU	0x14d
SEQ_EDIT_SONG_P1	EQU	0x14e
SEQ_EDIT_SONG_P2	EQU	0x14f
SEQ_EDIT_SONG_P3	EQU	0x150
	
SEQ_DISPLAY_PAGE	EQU	0x151	; the selected display page (menu #0)

SEQ_GP_LED0		EQU	0x152	; status of the general purpose LEDs (left side)
SEQ_GP_LED1		EQU	0x153	; status of the general purpose LEDs (right side)
SEQ_STEP_LEDS_CTR	EQU	0x154	; used by seq_leds.inc when DEFAULT_TRACK_LEDS_ENABLED set

SEQ_ENC_INC		EQU	0x155	; used by seq_enc.inc to save the incrementer

SEQ_LCD_PREVIOUS_EVNTL	EQU	0x156	; used by SEQ_LCD_PrnPTypeVal_L for perfect length display
SEQ_LCD_PREVIOUS_EVNTL_CTR EQU	0x157

SEQ_SAVE_PATTERN	EQU	0x158	; used by CS_M_SAVE* to store the save pattern
SEQ_SAVE_MODE		EQU	0x159	; used by CS_M_SAVE* to store the save mode

SEQ_REMOTE_CTR		EQU	0x15a	; used by "seq_remote.inc"

SEQ_RECORD_STEP		EQU	0x15b	; used by "cs_m_record.inc"

SEQ_MORPH_VALUE		EQU	0x15c	; used by "seq_midi.inc" and "cs_m_morph.inc"
SEQ_MORPH_PATTERN0	EQU	0x15d	; used by "seq_morph.inc" - morph pattern for track 1-4
SEQ_MORPH_PATTERN1	EQU	0x15e	; used by "seq_morph.inc" - morph pattern for track 5-8
SEQ_MORPH_PATTERN2	EQU	0x15f	; used by "seq_morph.inc" - morph pattern for track 9-12
SEQ_MORPH_PATTERN3	EQU	0x160	; used by "seq_morph.inc" - morph pattern for track 13-16

SEQ_MORPH_EVNT1		EQU	0x161	; used by "seq_morph.inc"
SEQ_MORPH_EVNT2		EQU	0x162	; used by "seq_morph.inc"
SEQ_MORPH_EVNTL		EQU	0x163	; used by "seq_morph.inc"
SEQ_MORPH_MIN_VALUE	EQU	0x164	; used by "seq_morph.inc"
SEQ_MORPH_MAX_VALUE	EQU	0x165	; used by "seq_morph.inc"

SEQ_CORE_TRK_LASTPOS	EQU	0x166	; used by "seq_core.inc"

SEQ_CORE_EXT_CLK_DIVIDER EQU	0x167	; divider for external clock output
SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD EQU 0x168 ; preload value of divider

SEQ_CCFDBK_SAVED_FSR1L	EQU	0x169
SEQ_CCFDBK_SAVED_FSR1H	EQU	0x16a

;; -----------------------------------

	;; free: 0x16b

;; -----------------------------------
;; 8 gate pins
GATES			EQU	0x16c
GATES_LAST		EQU	0x16d

;; -----------------------------------

;; used by AOUT driver ($MIOS_PATH/modules/aout/)
AOUT_INVERTED		EQU	0x16e
AOUT_UPDATE_REQ		EQU	0x16f
AOUT_VALUES		EQU	0x170;..0x17f (8 * 16bit words)

;; -----------------------------------
;; pot layer A
SEQ_POT_VALUES_A_00     EQU     0x180
        ;; ...
SEQ_POT_VALUES_A_3F     EQU     0x1bf
;; -----------------------------------

	;; free: 0x1c0-0x1ff

;; -----------------------------------
;;  track record structure
SEQ_TRKSTATEx           EQU     0x00	; flags: see seq_core.inc (SEQ_TRKSTATE_*)
SEQ_TRKMUTED0x          EQU     0x01	; muted steps 1-8
SEQ_TRKMUTED1x          EQU     0x02	; muted steps 9-16
SEQ_TRKPLYTICKx         EQU     0x03	; number of MIDI clock ticks until next step will be played
SEQ_TRKSTEPx            EQU     0x04	; the track step
SEQ_TRKPOSx             EQU     0x05	; the track position
SEQ_TRKQUEUELx          EQU     0x06	; length counter ("note off" once it is zero)
SEQ_TRKRETRGx		EQU	0x07	; the retrigger for trioles
SEQ_TRKEVNT0x           EQU     0x08	; stores the first MIDI byte
SEQ_TRKMODEx            EQU     0x09	; stores the track mode (3:0) and the track direction (5:4)
SEQ_TRKDIVLENx          EQU     0x0a	; stores the clock divider and the track length
SEQ_TRKASSGNx           EQU     0x0b	; stores the layer assignments
SEQ_TRKTRANSPx          EQU     0x0c	; stores the octave/semitones transpose value
SEQ_TRKGROOVEx          EQU     0x0d	; stores the groove mode and intensity
SEQ_TRKHUMANIZEx	EQU	0x0e	; stores the humanize mode and intensity
SEQ_TRKMORPHx		EQU	0x0f	; stores the morph mode
;; used to select the track record (pointer = SEQ_TRK0 + track_number*SEQ_TRKRECORD_LENGTH)
SEQ_TRKRECORD_LENGTH    EQU     0x10

;; -----------------------------------
;; Live Editing Tracks
SEQ_TRK0		EQU	0x200
SEQ_TRK1		EQU	SEQ_TRK0 + 1 * SEQ_TRKRECORD_LENGTH
SEQ_TRK2		EQU	SEQ_TRK0 + 2 * SEQ_TRKRECORD_LENGTH
SEQ_TRK3		EQU	SEQ_TRK0 + 3 * SEQ_TRKRECORD_LENGTH

;; Tracks played from EEPROM
SEQ_TRK4		EQU	SEQ_TRK0 + 4 * SEQ_TRKRECORD_LENGTH
SEQ_TRK5		EQU	SEQ_TRK0 + 5 * SEQ_TRKRECORD_LENGTH
SEQ_TRK6		EQU	SEQ_TRK0 + 6 * SEQ_TRKRECORD_LENGTH
SEQ_TRK7		EQU	SEQ_TRK0 + 7 * SEQ_TRKRECORD_LENGTH
SEQ_TRK8		EQU	SEQ_TRK0 + 8 * SEQ_TRKRECORD_LENGTH
SEQ_TRK9		EQU	SEQ_TRK0 + 9 * SEQ_TRKRECORD_LENGTH
SEQ_TRK10		EQU	SEQ_TRK0 + 10 * SEQ_TRKRECORD_LENGTH
SEQ_TRK11		EQU	SEQ_TRK0 + 11 * SEQ_TRKRECORD_LENGTH
SEQ_TRK12		EQU	SEQ_TRK0 + 12 * SEQ_TRKRECORD_LENGTH
SEQ_TRK13		EQU	SEQ_TRK0 + 13 * SEQ_TRKRECORD_LENGTH
SEQ_TRK14		EQU	SEQ_TRK0 + 14 * SEQ_TRKRECORD_LENGTH
SEQ_TRK15		EQU	SEQ_TRK0 + 15 * SEQ_TRKRECORD_LENGTH
SEQ_TRK_END		EQU	SEQ_TRK0 + 16 * SEQ_TRKRECORD_LENGTH-1

;; -----------------------------------

;; -----------------------------------
SEQ_D_TRKMUTED_0x	EQU     0x00	; muted steps  1- 4
SEQ_D_TRKMUTED_1x	EQU     0x01	; muted steps  5- 8
SEQ_D_TRKMUTED_2x	EQU     0x02	; muted steps  9-12
SEQ_D_TRKMUTED_3x	EQU     0x03	; muted steps 13-16

SEQ_D_TRKACC_0x		EQU     0x04	; accented steps  1- 4
SEQ_D_TRKACC_1x		EQU     0x05	; accented steps  5- 8
SEQ_D_TRKACC_2x		EQU     0x06	; accented steps  9-12
SEQ_D_TRKACC_3x		EQU     0x07	; accented steps 13-16

SEQ_D_TRKEVNT1		EQU     0x08	; stores the note of whole drum layer
SEQ_D_TRKEVNT_L	        EQU     0x09	; stores the gate length of whole drum layer (only valid in Drum layer A)
SEQ_D_TRKEVNT2_N	EQU     0x0a	; stores the velocity (normal)
SEQ_D_TRKEVNT2_A	EQU     0x0b	; stores the velocity (accent)
	;; free: 0x0c-0x0f
SEQ_D_TRKRECORD_LENGTH	EQU	0x10

;; -----------------------------------
;; Overlay the drum records with SEQ_POT_VALUES_[ABC]
SEQ_D_TRK0_A		EQU	SEQ_POT_VALUES_A_00 + 0x00
SEQ_D_TRK0_B		EQU	SEQ_POT_VALUES_B_00 + 0x00
SEQ_D_TRK0_C		EQU	SEQ_POT_VALUES_C_00 + 0x00

SEQ_D_TRK1_A		EQU	SEQ_POT_VALUES_A_00 + 0x10
SEQ_D_TRK1_B		EQU	SEQ_POT_VALUES_B_00 + 0x10
SEQ_D_TRK1_C		EQU	SEQ_POT_VALUES_C_00 + 0x10

SEQ_D_TRK2_A		EQU	SEQ_POT_VALUES_A_00 + 0x20
SEQ_D_TRK2_B		EQU	SEQ_POT_VALUES_B_00 + 0x20
SEQ_D_TRK2_C		EQU	SEQ_POT_VALUES_C_00 + 0x20

SEQ_D_TRK3_A		EQU	SEQ_POT_VALUES_A_00 + 0x30
SEQ_D_TRK3_B		EQU	SEQ_POT_VALUES_B_00 + 0x30
SEQ_D_TRK3_C		EQU	SEQ_POT_VALUES_C_00 + 0x30

	;; values of Trk4-16 have to be read from BankStick

;; -----------------------------------

	;; the queued "off events" for track 1-16
	;; note: *3 in drum mode! -> 16 * 3 * 2 bytes allocated
	;; indexed with following formula: event byte = SEQ_TRKQUEUEx_BEGIN + 3*SEQ_EVNTT + SEQ_EVNTD
SEQ_TRKQUEUE0_BEGIN	EQU	0x300	; 0x300-0x32f
SEQ_TRKQUEUE0_END	EQU	0x32f

SEQ_TRKQUEUE1_BEGIN	EQU	0x330	; 0x330-0x35f
SEQ_TRKQUEUE1_END	EQU	0x35f

;; -----------------------------------

;;  free for your pleasure: 0x360-0x37f
;;  and 0x380-0x3ff if the AIN handler is disabled
