;$Id$
;
; Specific Defines and variables of MIOS Application
;
; ==========================================================================
;
;  Copyright 1998-2010 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


	;; free memory for user applications:	
	;; 0x010-0x37f

LC_FADER_NUMBER		EQU	0x010 ; stores the fader number
LC_FADER_POS_L		EQU	0x011 ; stores the fader position (low byte)
LC_FADER_POS_H		EQU	0x012 ; stores the fader position (high byte)
LC_ID			EQU	0x013 ; if 0x00:	LC emulation disabled, otherwise 0x10/0x11/0x14/0x15 for different LC/Mackie modes
LC_RECEIVED_SYSEX_BYTE	EQU	0x014 ; stores the received sysex byte for multiple sysex parsers
LC_SYSEX_IN		EQU	0x015 ; used in lc_sysex.inc
LC_SYSEX_STATE		EQU	0x016 ; used in lc_sysex.inc
LC_SYSEX_ACTION		EQU	0x017 ; used in lc_sysex.inc

PERIODIC_MS_CTR 	EQU	0x020

;; ==========================================================================
;; for fader move traces
TRACE_MEM_START		EQU	0x100
TRACE_MEM_END		EQU	0x2ff

;; ==========================================================================
;; base addresses for AIN and MF driver
AIN_BASE	EQU	0x300
MF_BASE		EQU	0x300

;; ==========================================================================
;; configuration data
;; alignment must match with mf_presets.inc!
MF_MODE_0	EQU	0x300 ;.. 0x307
MF_MIN_VALUE_L_0 EQU	0x308 ;.. 0x30f (only low-byte will be stored: 0..255)
MF_MAX_VALUE_L_0 EQU	0x310 ;.. 0x317 (only low-byte will be stored: 768..1023)
MF_MIN_DUTY_UP_0 EQU	0x318 ;.. 0x31f
MF_MAX_DUTY_UP_0 EQU	0x320 ;.. 0x327
MF_MIN_DUTY_DOWN_0 EQU	0x328 ;.. 0x32f
MF_MAX_DUTY_DOWN_0 EQU	0x330 ;.. 0x337
;; free: 0x338..0x33f

;; ==========================================================================
;; variable data (not stored in EEPROM)
;; must be below 0x380, mf.inc simplifies addressing based on this constraint
MF_MANUAL_CTR_0	EQU	0x340 ;.. 0x347
MF_TIMEOUT_CTR_0 EQU	0x348 ;.. 0x34f
MF_REPEAT_CTR_0	EQU	0x350 ;.. 0x357
MF_TARGET_POS_L_0 EQU	0x358 ;.. 0x35f
MF_TARGET_POS_H_0 EQU	0x360 ;.. 0x367
MF_PWM_DUTY_0	EQU	0x368 ;.. 0x36f

;; ==========================================================================
;; ain.inc
AIN_RESULT_L_00	EQU	0x380 ;.. 0x387
;; ...
AIN_RESULT_L_07	EQU	0x387

AIN_RESULT_H_00	EQU	0x3c0	; don't move this address, the original MIOS driver switches between L/H by toggling bit #6
;; ...
AIN_RESULT_H_07	EQU	0x3c7


AIN_DEADBAND	EQU	0x388
AIN_POTSPEED	EQU	0x389
AIN_NUMBER	EQU	0x38a
AIN_CTRL	EQU	0x38b
AIN_INPUT_CTR	EQU	0x38c
AIN_POT_CTR	EQU	0x38d

;; ==========================================================================
;; mf.inc
;; if motorfaders are enabled, the purpose of some AIN register will be changed
MF_NUMBER		EQU	0x390
MF_VALUE_L		EQU	0x391
MF_VALUE_H		EQU	0x392
MF_TARGET_POS_L		EQU	0x393
MF_TARGET_POS_H		EQU	0x394
MF_TARGET_DIFF		EQU	0x395
MF_TARGET_DIFF_H	EQU	0x396
MF_TARGET_MOTOR_DIR	EQU	0x397
MF_TIMEOUT_CTR		EQU	0x398
MF_DEADBAND		EQU	0x399
MF_SUSPEND		EQU	0x39a

MF_PWM_STEPS		EQU	0x39b
MF_TMP1			EQU	0x39c
MF_STORED_PRODL		EQU	0x39d
MF_STORED_PRODH		EQU	0x39e
MF_AIN_CHANGED_FLAG	EQU	0x39f
MF_MOTOR_DIR		EQU	0x3a0 ; direction of motor (0: down, 1: up)
MF_PWM_STEPS_COUNTER	EQU	0x3a1 ; the global period counter for generating PWM
MF_PWM_DUTY_STATUS_LED	EQU	0x3a2 ; the duty cycle for status LED (sweeped during runtime)
MF_PWM_STATUS_LED_SWEEP_CTR EQU	0x3a3 ; for duty cycle sweep effect
MF_PWM_STATUS_LED_SWEEP_DIR EQU	0x3a4 ; for duty cycle sweep effect

MF_OPERATION_MODE 	EQU	0x3a5	      ; see doc
MF_TOUCHSENSOR_MODE 	EQU	0x3a6	      ; see doc

MF_TRACE_FADER		EQU	0x3a7	      ; selects fader for trace mechanism
MF_TRACE_SCALE		EQU	0x3a8	      ; scale in mS (-1)
MF_TRACE_SCALE_CTR	EQU	0x3a9
MF_TRACE_STATE		EQU	0x3aa
MF_TRACE_PTR_L		EQU	0x3ab
MF_TRACE_PTR_H		EQU	0x3ac
MF_TRACE_VALUE_L	EQU	0x3ad
MF_TRACE_VALUE_H	EQU	0x3ae

MF_SYSEX_STATE		EQU	0x3af	; used by mf_sysex.inc
MF_SYSEX_STATE2		EQU	0x3b0	; used by mf_sysex.inc
MF_SYSEX_IN		EQU	0x3b1	; used by mf_sysex.inc
MF_SYSEX_CHECKSUM	EQU	0x3b2	; used by mf_sysex.inc
MF_SYSEX_ADDRESS	EQU	0x3b3	; used by mf_sysex.inc
MF_SYSEX_DATA		EQU	0x3b4	; used by mf_sysex.inc
MF_SYSEX_PATCH		EQU	0x3b5	; used by mf_sysex.inc

	;; 0x3c0..0x3c8 reserved for AIN_RESULT_H_00!!!
