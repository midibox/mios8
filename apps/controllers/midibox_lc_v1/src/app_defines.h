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

	;; free memory for user applications:	
	;; 0x010-0x37f

; ==========================================================================
#define LC_STAT_SMPTE 0		; flag #0 set by DISPLAY_Print_[SMPTE|BEATS]
				; if set: SMPTE mode
				; if cleared: BEATS mode
#define LC_STAT_GLCD 1		; flag #1 set by DISPLAY_Init
				; if cleared: character LCD
				; if set: graphical LCD
#define LC_STAT_LEDMETERS 2	; if cleared: ledrings are working as normal
				; if set: ledrings are working as meters
#define LC_STAT_LAYER_SEL 3	; if cleared: button/led layer 0 active
				; if set: button/led layer 1 active
#define LC_STAT_GPC_SEL 4	; if cleared: logic control emulation mode
				; if set: general purpose controller mode
#define LC_STAT_CALI_SEL 5	; if cleared: logic control emulation mode
				; if set: motorfader calibration mode
#define LC_STAT_MSG_UPDATE_REQ 6; if set, host/GPC message will be updated

; ==========================================================================

;; status of LC emulation (see flags above)
LC_STAT			EQU	0x010

;; used by MIDI processor (lc_mproc.inc)
LC_ID			EQU	0x011
LC_SYSEX_IN		EQU	0x012
LC_SYSEX_STATE		EQU	0x013
LC_SYSEX_ACTION		EQU	0x014
LC_CURSOR		EQU	0x015

;; used by LED matrix handler in lc_srio.inc
SRIO_LED_CTR		EQU	0x016

;; used by ledring handler
LC_LEDRING_UPDATE_REQ	EQU	0x017

;; used by LCD handler
LC_DISPLAY_PAGE		EQU	0x018

;; MTC variables
LC_MTC_CURSOR		EQU	0x019

;; MF driver variables
LC_FADER_POS_L		EQU	0x01a	; received and transmitted low byte of fader position
LC_FADER_POS_H		EQU	0x01b	; received and transmitted high byte of fader position
LC_FADER_NUMBER		EQU	0x01c	; fader number

;; meter runtime variables
LC_METER_GLOBAL		EQU	0x01d	; global meter mode
LC_METER_CHANNEL	EQU	0x01e	; meter channel (used by SysEx handler)
LC_METER_UPDATE_REQ	EQU	0x01f	; set flag 0-7 to request an update of a meter

;; LC LED loop counter
LC_LED_LOOP_CTR		EQU	0x020

;; used by "midi_rxtx.inc"
MIDI_RXTX_RX_CTR	EQU	0x021
MIDI_RXTX_TX_CTR	EQU	0x022

; led digits counter, used by lc_leddigits.inc
LC_LEDDIGITS_CTR	EQU	0x023

;; LCD driver runtime variables
LCD_MSG_CHR		EQU	0x040
LCD_SMPTE_BEATS_X	EQU	0x041	; x-position of SMPTE/BEATS
LCD_SMPTE_BEATS_Y	EQU	0x042	; y-position of SMPTE/BEATS
LCD_SMPTE_BEATS_FONT_L	EQU	0x043	; low-byte to font
LCD_SMPTE_BEATS_FONT_H	EQU	0x044	; high-byte to font
LCD_MTC_X		EQU	0x045	; x-position of MTC digits
LCD_MTC_Y		EQU	0x046	; y-position of MTC digits
LCD_MTC_D		EQU	0x047	; distance between digits
LCD_MTC_FONT_L		EQU	0x048	; low-byte to font
LCD_MTC_FONT_H		EQU	0x049	; high-byte to font
LCD_STATUS_X		EQU	0x04a	; x-position of status digits
LCD_STATUS_Y		EQU	0x04b	; y-position of status digits
LCD_STATUS_D		EQU	0x04c	; distance between digits
LCD_STATUS_FONT_L	EQU	0x04d	; low-byte to font
LCD_STATUS_FONT_H	EQU	0x04e	; high-byte to font
LCD_MSG_X		EQU	0x04f	; x-position of host messages
LCD_MSG_Y		EQU	0x050	; y-position of host messages
LCD_MSG_D		EQU	0x051	; distance between digit
LCD_MSG_FONT_L		EQU	0x052	; low-byte to font
LCD_MSG_FONT_H		EQU	0x053	; high-byte to font
LCD_METER_X		EQU	0x054	; x-position of meter
LCD_METER_Y		EQU	0x055	; y-position of meter
LCD_METER_D		EQU	0x056	; distance between meters
LCD_METER_ICONS_L	EQU	0x057	; low-byte to meter icons
LCD_METER_ICONS_H	EQU	0x058	; high-byte to meter icons
LCD_KNOB_X		EQU	0x059	; x-position of knob
LCD_KNOB_Y		EQU	0x05a	; y-position of knob
LCD_KNOB_D		EQU	0x05b	; distance between knobs
LCD_KNOB_ICONS_L	EQU	0x05c	; low-byte to knob icons
LCD_KNOB_ICONS_H	EQU	0x05d	; high-byte to knob icons
LCD_RSM_X		EQU	0x05e	; x-position of RSM message
LCD_RSM_Y		EQU	0x05f	; y-position of RSM message
LCD_RSM_D		EQU	0x060	; distance between chars
LCD_RSM_FONT_L		EQU	0x061	; low-byte to font
LCD_RSM_FONT_H		EQU	0x062	; high-byte to font
LCD_SEL_X		EQU	0x063	; x-position of SEL message
LCD_SEL_Y		EQU	0x064	; y-position of SEL message
LCD_SEL_D		EQU	0x065	; distance between chars
LCD_SEL_FONT_L		EQU	0x066	; low-byte to font
LCD_SEL_FONT_H		EQU	0x067	; high-byte to font

GPC_OFFSET		EQU	0x068	; offset for general purpose controller list

;; the absolute values of Vpots in GPC mode
GPC_ABS_VALUES_BEGIN	EQU	0x80
GPC_ABS_VALUES_END	EQU	0xff

;; meter variables
METER_MODE_BEGIN	EQU	0x100	; meter modes transmitted by host application
METER_MODE_END		EQU	0x107
METER_LEVEL_BEGIN	EQU	0x108	; meter levels transmitted by host application
METER_LEVEL_END		EQU	0x10f
METER_CTR_BEGIN		EQU	0x110	; meter decrement counter (handled by lc_timer.inc)
METER_CTR_END		EQU	0x117

;; ledring variables
LEDRING_POS_BEGIN	EQU	0x118
LEDRING_POS_END		EQU	0x11f

LEDRING_PATTERN_VPOT0_L	EQU	0x120
LEDRING_PATTERN_VPOT0_H	EQU	0x121
LEDRING_PATTERN_VPOT1_L	EQU	0x122
LEDRING_PATTERN_VPOT1_H	EQU	0x123
LEDRING_PATTERN_VPOT2_L	EQU	0x124
LEDRING_PATTERN_VPOT2_H	EQU	0x125
LEDRING_PATTERN_VPOT3_L	EQU	0x126
LEDRING_PATTERN_VPOT3_H	EQU	0x127
LEDRING_PATTERN_VPOT4_L	EQU	0x128
LEDRING_PATTERN_VPOT4_H	EQU	0x129
LEDRING_PATTERN_VPOT5_L	EQU	0x12a
LEDRING_PATTERN_VPOT5_H	EQU	0x12b
LEDRING_PATTERN_VPOT6_L	EQU	0x12c
LEDRING_PATTERN_VPOT6_H	EQU	0x12d
LEDRING_PATTERN_VPOT7_L	EQU	0x12e
LEDRING_PATTERN_VPOT7_H	EQU	0x12f

METER_PATTERN_VPOT0_L	EQU	0x130
METER_PATTERN_VPOT0_H	EQU	0x131
METER_PATTERN_VPOT1_L	EQU	0x132
METER_PATTERN_VPOT1_H	EQU	0x133
METER_PATTERN_VPOT2_L	EQU	0x134
METER_PATTERN_VPOT2_H	EQU	0x135
METER_PATTERN_VPOT3_L	EQU	0x136
METER_PATTERN_VPOT3_H	EQU	0x137
METER_PATTERN_VPOT4_L	EQU	0x138
METER_PATTERN_VPOT4_H	EQU	0x139
METER_PATTERN_VPOT5_L	EQU	0x13a
METER_PATTERN_VPOT5_H	EQU	0x13b
METER_PATTERN_VPOT6_L	EQU	0x13c
METER_PATTERN_VPOT6_H	EQU	0x13d
METER_PATTERN_VPOT7_L	EQU	0x13e
METER_PATTERN_VPOT7_H	EQU	0x13f

;; MTC digits
LC_MTC_DIGIT_0		EQU	0x140	; rightmost digit
LC_MTC_DIGIT_1		EQU	0x141
LC_MTC_DIGIT_2		EQU	0x142
LC_MTC_DIGIT_3		EQU	0x143
LC_MTC_DIGIT_4		EQU	0x144
LC_MTC_DIGIT_5		EQU	0x145
LC_MTC_DIGIT_6		EQU	0x146
LC_MTC_DIGIT_7		EQU	0x147
LC_MTC_DIGIT_8		EQU	0x148
LC_MTC_DIGIT_9		EQU	0x149	; leftmost digit
LC_MTC_DIGIT_C0		EQU	0x14a	; first colon
LC_MTC_DIGIT_C1		EQU	0x14b	; second colon
LC_MTC_DIGIT_C2		EQU	0x14c	; third colon
LC_MTC_DIGIT_SPACE	EQU	0x14d	; optional space
LC_STATUS_DIGIT_A	EQU	0x14e	; assignment display, second char
LC_STATUS_DIGIT_B	EQU	0x14f	; assignment display, first char


;; led status array
LED_STATUS_BEGIN	EQU	0x180	; LED status, only used by the 2-CLCD option to print
;; ---					; Mono/Solo/Select on Screen
LED_STATUS_END		EQU	0x1ff

;; the host message (2x55 characters) is saved here so that it can be print again on display page switching
MSG_HOST_BEGIN		EQU	0x200
MSG_HOST_END		EQU	0x27f

;; 8 characters used by the CLCD to print the status of Rec/Solo/Mute
RSM_MSG_BEGIN		EQU	0x280
RSM_MSG_END		EQU	0x287

;; 8 characters used by the CLCD to print the status of select
SEL_MSG_BEGIN		EQU	0x288
SEL_MSG_END		EQU	0x28f

;; the message used by the general purpose controller mode
MSG_GPC_BEGIN		EQU	0x300
MSG_GPC_END		EQU	0x37f
