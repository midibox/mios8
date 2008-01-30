; $Id$
;
; MIOS definition file, part 2
; Extents mios.h by some internal definitions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ==========================================================================
;;  Internal MIOS Registers and address ranges
;; ==========================================================================

;; ==========================================================================
;; mios_mproc.inc
MIOS_UPLOAD_BUFFER      EQU     0x100
MIOS_UPLOAD_BUFFER_END  EQU     0x4ff

;; ==========================================================================
;; mios_ain.inc
MIOS_AIN_RESULT_L_00	EQU	0x380
;; ...
MIOS_AIN_RESULT_L_3F	EQU	0x3bf

MIOS_AIN_RESULT_H_00	EQU	0x3c0
;; ...
MIOS_AIN_RESULT_H_3F	EQU	0x3ff

;; ==========================================================================
;; mios_mf.inc
;; if motorfaders are enabled, the purpose of some AIN register will be changed
MIOS_MF_NUMBER		EQU	0x390
MIOS_MF_CTR_UPDATE_CTR	EQU	0x391
MIOS_MF_SR0		EQU	0x392
MIOS_MF_SR1		EQU	0x393
MIOS_MF_VALUE_L		EQU	0x394
MIOS_MF_VALUE_H		EQU	0x395
MIOS_MF_TARGET_POS_L	EQU	0x396
MIOS_MF_TARGET_POS_H	EQU	0x397
MIOS_MF_TIMEOUT_CTR	EQU	0x398
MIOS_MF_DEADBAND	EQU	0x399
MIOS_MF_SUSPEND		EQU	0x39a
MIOS_MF_PWM_DUTY_CYCLE_UP EQU	0x39b
MIOS_MF_PWM_DUTY_CYCLE_DOWN EQU	0x39c
MIOS_MF_PWM_PERIOD	EQU	0x39d
	;; free: 0x39d-0x39e
MIOS_MF_AIN_CHANGED_FLAG EQU	0x39f

MIOS_MF_PWM_CTR0	EQU	0x3a0
;; ...
MIOS_MF_PWM_CTR7	EQU	0x3a7

MIOS_MF_MANUAL_CTR0	EQU	0x3a8
;; ...
MIOS_MF_MANUAL_CTR7	EQU	0x3af

MIOS_MF_TIMEOUT_CTR0	EQU	0x3b0
;; ...
MIOS_MF_TIMEOUT_CTR7	EQU	0x3b7

MIOS_MF_REPEAT_CTR0	EQU	0x3b8
;; ...
MIOS_MF_REPEAT_CTR7	EQU	0x3bf

MIOS_MF_TARGET_POS_L0	EQU	0x3d0
;; ...
MIOS_MF_TARGET_POS_L7	EQU	0x3d7

MIOS_MF_TARGET_POS_H0	EQU	0x3d8
;; ...
MIOS_MF_TARGET_POS_H7	EQU	0x3df

MIOS_MF_STATE0		EQU	0x3e0
;; ...
MIOS_MF_STATE7		EQU	0x3e7

;; ==========================================================================

MIOS_DIN_REG0_00	EQU	0x400
;; ...
MIOS_DIN_REG0_7F	EQU	0x47f

MIOS_DIN_REG1_00	EQU	0x480
;; ...
MIOS_DIN_REG1_7F	EQU	0x4ff

;; ==========================================================================

;; mios_midi.inc
MIOS_RX_BUFFER		EQU	0x500
MIOS_RX_BUFFER_END	EQU	0x53f

MIOS_TX_BUFFER		EQU	0x540
MIOS_TX_BUFFER_END	EQU	0x56f

;; ==========================================================================
;; address range 0x570-0x58d (LCD registers) defined in mios.h

;; ==========================================================================
;; additional temporary registers used by MIOS internal routines
MIOS_TMP1		EQU	0x590
MIOS_TMP2		EQU	0x591
MIOS_TMP3		EQU	0x592
MIOS_TMP_CTR		EQU	0x593
MIOS_IRQ_TMP_CTR	EQU	0x594

;; ==========================================================================
;; mios_midi.inc
MIOS_RX_BUFFER_TMP	EQU	0x595
MIOS_RX_BUFFER_HEAD	EQU	0x596
MIOS_RX_BUFFER_TAIL	EQU	0x597
MIOS_TX_BUFFER_TMP	EQU	0x598
MIOS_TX_BUFFER_HEAD	EQU	0x599
MIOS_TX_BUFFER_TAIL	EQU	0x59a
MIOS_MIDI_DEVICE_ID	EQU	0x59b
MIOS_MIDI_RUNSTATUS	EQU	0x59c
MIOS_MIDI_EXPBYTES	EQU	0x59d

;; ==========================================================================
;; mios_mproc.inc
MIOS_MPROC_TIMEOUT_L	EQU	0x59e
MIOS_MPROC_CTR		EQU	MIOS_MPROC_TIMEOUT_L
MIOS_MPROC_TIMEOUT_H	EQU	0x59f
MIOS_MPROC_EVNT0	EQU	MIOS_MPROC_TIMEOUT_H
MIOS_MPROC_TIMEOUT_U	EQU	0x5a0
MIOS_MPROC_STATE	EQU	0x5a1
MIOS_MPROC_IN		EQU	0x5a2
MIOS_MPROC_LASTEVENT0   EQU     0x5a3
MIOS_MPROC_LASTEVENT1   EQU     0x5a4
MIOS_MPROC_LASTEVENT2   EQU     0x5a5
MIOS_MPROC_SYXSTATE	EQU	0x5a6
MIOS_MPROC_ACTION	EQU	0x5a7
MIOS_MPROC_CHECKSUM	EQU	0x5a8
MIOS_MPROC_ADDRESS_EXT	EQU	0x5a9

MIOS_MPROC_SYX_BIT7_CTR	EQU	MIOS_TMP_CTR
MIOS_MPROC_SYX_BIT8_CTR	EQU	MIOS_TMP1
MIOS_MPROC_SYX_BYTE7	EQU	MIOS_TMP2
MIOS_MPROC_SYX_BYTE8	EQU	MIOS_TMP3

;; ==========================================================================
;; mios_srio.inc
MIOS_SRIO_NUMBER	EQU	0x5aa
MIOS_SRIO_UPDATE_CTR	EQU	0x5ab
MIOS_SRIO_UPDATE_FRQ	EQU	0x5ac
MIOS_SRIO_TS_SENSITIVITY EQU	0x5ad
MIOS_SRIO_DEBOUNCE	EQU	0x5ae
MIOS_SRIO_DEBOUNCE_CTR	EQU	0x5af

;; ==========================================================================
;; mios_din.inc
MIOS_DIN_AUTO_CTR	EQU	0x5b0

;; ==========================================================================
;; mios_ain.inc
MIOS_AIN_DEADBAND	EQU	0x5b1
MIOS_AIN_POTSPEED	EQU	0x5b2
MIOS_AIN_NUMBER		EQU	0x5b3
MIOS_AIN_CTRL		EQU	0x5b4
MIOS_AIN_INPUT_CTR	EQU	0x5b5
MIOS_AIN_LAST_POT0	EQU	0x5b6
MIOS_AIN_LAST_POT1	EQU	0x5b7

;; ==========================================================================
;; mios_isr.inc, mios_delay.inc and mios_display.inc
MIOS_GP_CTR		EQU	0x5b8
MIOS_DELAY_CTR		EQU	0x5b9
MIOS_MESSAGE_CTR	EQU	0x5ba

;; ==========================================================================
;; mios_isr.inc
MIOS_IRQ_FSR1L		EQU	0x5bb
MIOS_IRQ_FSR1H		EQU	0x5bc
MIOS_IRQ_TBLPTRL	EQU	0x5bd
MIOS_IRQ_TBLPTRH	EQU	0x5be
MIOS_IRQ_TBLPTRU	EQU	0x5bf		; only stored when code size >= 0x10000
MIOS_IRQ_TABLAT		EQU	0x5c0
MIOS_IRQ_PARAMETER1	EQU	0x5c1
MIOS_IRQ_PARAMETER2	EQU	0x5c2
MIOS_IRQ_PARAMETER3	EQU	0x5c3

MIOS_IRQ_FSR2L		EQU	0x5c4		; new for SDCC
MIOS_IRQ_FSR2H		EQU	0x5c5

;; ==========================================================================
;; mios_iic.inc
MIOS_IIC_BUFFER		EQU	0x5c6
MIOS_IIC_CTR		EQU	0x5c7

;; ==========================================================================
;; mios_timer.inc
MIOS_TIMER3_RELOAD_L	EQU	0x5c8
MIOS_TIMER3_RELOAD_H	EQU	0x5c9

;; ==========================================================================
;; mios_hlp.inc
MIOS_AFT_PTRL		EQU	0x5ca

;; ==========================================================================
;; mios_hlp.inc
MIOS_IIC_MIDI_ADDR	EQU	0x5cb
MIOS_MIDI_IIC_TMP	EQU	0x5cc
MIOS_MIDI_IIC_RETRY_CTR	EQU	0x5cd

;; ==========================================================================

	;; free: 0x5ce-0x5cf
	;;       0x58e

;; ==========================================================================
;; mios_srio.inc/mios_din.inc/mios_dout.inc
MIOS_SR_DOUT_0		EQU	0x5d0
;; ...
MIOS_SR_DOUT_F		EQU	0x5df

MIOS_SR_DIN_0		EQU	0x5e0
;; ...
MIOS_SR_DIN_F		EQU	0x5ef

MIOS_SR_DIN_CHANGED_0	EQU	0x5f0
;; ...
MIOS_SR_DIN_CHANGED_F	EQU	0x5ff
