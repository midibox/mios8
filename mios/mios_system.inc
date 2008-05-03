; $Id$
;
; MIOS System Routines
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_Reset
;;  C_DECLARATION: void MIOS_Reset(void)
;;  DESCRIPTION: waits until the Tx MIDI buffer is empty, thereafter resets the OS
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MIOS_Reset
	;; clear screen
	call	MIOS_LCD_Clear

	;; setup default font
	TABLE_ADDR MIOS_GLCD_FONT
	call	MIOS_GLCD_FontInit

	;; print reboot message
	TABLE_ADDR TEXT_MIOS_REBOOT_0
	call	MIOS_LCD_PrintString

	;; suspend all IRQ routines expect for the MIDI handler
	rcall	MIOS_SystemSuspend

	;; feed the watchdog (last time before reset)
	clrwdt

	;; wait until all MIDI bytes have been transmitted
	call	MIOS_MIDI_TxBufferFlush
	
	;; RESET BOX!
	reset

	;; secure halt
MIOS_Reset_Endless
	rgoto	MIOS_Reset_Endless

TEXT_MIOS_REBOOT_0	da 0x1000, "Rebooting MIOS  "

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_SystemSuspend
;;  C_DECLARATION: void MIOS_SystemSuspend(void)
;;  DESCRIPTION: suspends all system and user tasks except for the IO MIDI handler
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: FSR1
;; --------------------------------------------------------------------------
MIOS_SystemSuspend
	;; skip DOUT flushing if system already suspended
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, MIOS_SystemSuspend_Now
	BRA_IFSET MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_ALL, ACCESS, MIOS_SystemSuspend_NoFlush
MIOS_SystemSuspend_Now

	;; suspend all IRQ routines expect for the MIDI handler
	bsf	MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_ALL
	bsf	MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER

	;; flush DOUT and SRM bits
	bcf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
#if MIOS_ENABLE_AIN && MIOS_ENABLE_MF
 	bcf	MIOS_MF_LAT_DOUT, MIOS_MF_PIN_DOUT
#endif
	movlw	0x80
	movwf	WREG
MIOS_SystemSuspend_SRs_Loop
        bsf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
#if MIOS_ENABLE_AIN && MIOS_ENABLE_MF
        bsf     MIOS_MF_LAT_SCLK, MIOS_MF_PIN_SCLK
#else
	nop
#endif
        bcf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
#if MIOS_ENABLE_AIN && MIOS_ENABLE_MF
        bcf     MIOS_MF_LAT_SCLK, MIOS_MF_PIN_SCLK
#endif
	decfsz	WREG, F
	rgoto	MIOS_SystemSuspend_SRs_Loop

        bcf	MIOS_SRIO_LAT_RCLK, MIOS_SRIO_PIN_RCLK	; latch disable
#if MIOS_ENABLE_AIN && MIOS_ENABLE_MF
        bcf	MIOS_MF_LAT_RCLK, MIOS_MF_PIN_RCLK
#else
	nop
#endif

        bsf	MIOS_SRIO_LAT_RCLK, MIOS_SRIO_PIN_RCLK	; latch enable
#if MIOS_ENABLE_AIN && MIOS_ENABLE_MF
        bsf	MIOS_MF_LAT_RCLK, MIOS_MF_PIN_RCLK
#endif

MIOS_SystemSuspend_NoFlush
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_SystemResume
;;  C_DECLARATION: void MIOS_SystemResume(void)
;;  DESCRIPTION: resumes all system tasks
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MIOS_SystemResume
	bcf	MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_ALL
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_UserSuspend
;;  C_DECLARATION: void MIOS_UserSuspend(void)
;;  DESCRIPTION: suspends all user tasks
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MIOS_UserSuspend
	;; suspend all IRQ routines expect for the MIDI handler
	bsf	MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_UserResume
;;  C_DECLARATION: void MIOS_UserResume(void)
;;  DESCRIPTION: resumes all user tasks
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MIOS_UserResume
	bcf	MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER
	return
