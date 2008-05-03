; $Id$
;
;  Change ID Application
;  Details are described in README.txt
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ==========================================================================
;;  MIOS Settings which are stored in the ID header
;; ==========================================================================

#ifndef MIOS_DEVICE_ID
#define MIOS_DEVICE_ID   0x00	; the device ID (0x00-0x7f)
#endif

#ifndef MIOS_LCD_TYPE
#define MIOS_LCD_TYPE    0	; 0 == CLCD
#endif				; 1 == KS0107/KS0108 or HD61202 based graphical LCD
				; 6 == MIDI link to a LCD or MBHP_TV
				; 7 == custom LCD driver

#ifndef MIOS_TO_COM_FLAG
#define MIOS_TO_COM_FLAG 0	; IMPORTANT: never alternate this flag if you don't
#endif				; own a to-COM interface, otherwise you won't be
				; able to reset this flag w/o a PIC programmer

#ifndef IIC_MIDI_ADDR
#define IIC_MIDI_ADDR 0x00	; if != 0x00 and != 0xff, MBHP_IIC_MIDI slave
#endif				; with the given address will be used as second MIDI OUT

;
;; ==========================================================================
;;  ID which should be programmed into the ID field
;;  You don't need to change it, the numbers are calculated from the settings
;;  above
;; ==========================================================================
ID_BYTE0	EQU	0x00
ID_BYTE1	EQU	0x00
ID_BYTE2	EQU	0x00
ID_BYTE3	EQU	0x00
ID_BYTE4	EQU	0x00
ID_BYTE5	EQU	IIC_MIDI_ADDR
ID_BYTE6	EQU	(MIOS_LCD_TYPE << 4) | (MIOS_TO_COM_FLAG)
ID_BYTE7	EQU	MIOS_DEVICE_ID
;
;; ---[ MIOS header file ]---
#include <mios.h>

;; ---[ useful macros ]---
#include <macros.h>

;; ---[ vectors to MIOS functions (never change!) ]---
#include <mios_vectors.inc>

;; ---[ user hooks (never change!) ]---
#include <user_vectors.inc>

;; ==========================================================================
;;  General Application Settings
;; ==========================================================================

;; ---[ variables used by application ]---
#include "app_defines.h"

; ==========================================================================

;; ---[ configuration table for MIDI processor and rotary encoders ]---
#include <mios_mt_table.inc>
#include <mios_enc_table.inc>

;; ---[ Custom LCD driver ]---
#include <app_lcd.inc>

;; ==========================================================================
;;  All MIOS hooks in one file
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; call change ID function from here
	;; this function will reset the core if the ID has been changed
	goto	CHANGE_ID_Start


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	return


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a debug command has been received
;;  via SysEx
;;  Input:
;;     o WREG, MIOS_PARAMETER1, MIOS_PARAMETER2, MIOS_PARAMETER3 like
;;       specified in the debug command
;;  Output:
;;     o return values WREG, MIOS_PARAMETER1, MIOS_PARAMETER2, MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_DebugTrigger
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the display content should be 
;;  initialized. Thats the case during startup and after a temporary message
;;  has been printed on the screen
;; --------------------------------------------------------------------------
USER_DISPLAY_Init
	call	MIOS_LCD_Clear

	TABLE_ADDR TEXT_WELCOME_0
	call	MIOS_LCD_PrintString

	;; print ID at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	IRQ_DISABLE
	TABLE_ADDR_FULL _IDLOC0
	lfsr	FSR1, FLASH_BUFFER
	call	MIOS_FLASH_Read
	clrf	TBLPTRU
	IRQ_ENABLE
	lfsr	FSR0, FLASH_BUFFER
	movf	POSTINC0, W
	call	MIOS_LCD_PrintHex2
	movf	POSTINC0, W
	call	MIOS_LCD_PrintHex2
	movf	POSTINC0, W
	call	MIOS_LCD_PrintHex2
	movf	POSTINC0, W
	call	MIOS_LCD_PrintHex2
	movf	POSTINC0, W
	call	MIOS_LCD_PrintHex2
	movf	POSTINC0, W
	call	MIOS_LCD_PrintHex2
	movf	POSTINC0, W
	call	MIOS_LCD_PrintHex2
	movf	POSTINC0, W
	call	MIOS_LCD_PrintHex2	

	return

TEXT_WELCOME_0	STRING 14, 0x00, "New ID Header:"

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedEvent
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI event has been received
;;  which has been specified in the CONFIG_MIDI_IN table
;;  Input:
;;     o number of entry in WREG
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyFoundEvent
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI event has not been completly
;;  received within 2 seconds
;; --------------------------------------------------------------------------
USER_MPROC_NotifyTimeout
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS before the transfer of a MIDI byte. 
;;  It can be used to monitor the Tx activity or to do any other actions
;;  (e.g. to switch a pin for multiplexed MIDI Outs) before the byte will 
;;  be sent.
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;;  Input:
;;     o transmitted byte in WREG
;; --------------------------------------------------------------------------
USER_MIDI_NotifyTx
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received.
;;  It can be used to monitor the Rx activity or to do any action - e.g.
;;  to react on realtime events like MIDI clock (0xf8) with a minimum latency
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;;  Input:
;;     o received byte in WREG
;; --------------------------------------------------------------------------
USER_MIDI_NotifyRx
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an button has been toggled
;;  Input:
;;     o Button number in WREG and MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
USER_DIN_NotifyToggle
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an encoder has been moved
;;  Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
USER_ENC_NotifyChange
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS before the shift register are loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Prepare
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS after the shift register have been loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Finish
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a pot has been moved
;;  Input:
;;     o Pot number in WREG and MIOS_PARAMETER1
;;     o LSB value in MIOS_PARAMETER2
;;     o MSB value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_AIN_NotifyChange
	return

;; --------------------------------------------------------------------------
;;  This function changes the ID header of the PIC
;; --------------------------------------------------------------------------
CHANGE_ID_Start
	clrwdt

	;; ---[ read current content of block ]---
	TABLE_ADDR_FULL _IDLOC0
	lfsr	FSR1, FLASH_BUFFER
FLASH_Read
	;; align TBLPTRL
	movlw	0xc0
	andwf	TBLPTRL, F

	clrf	TMP1
FLASH_ReadLoop
	tblrd*+
	movff	TABLAT, POSTINC1
	incf	TMP1, F
	BRA_IFCLR TMP1, 6, ACCESS, FLASH_ReadLoop

	;; ---[ only overwrite if old ID is not equal to new ID ]---
	movf	FLASH_BUFFER+0, W
	xorlw	ID_BYTE0
	bnz	FLASH_Overwrite
	movf	FLASH_BUFFER+1, W
	xorlw	ID_BYTE1
	bnz	FLASH_Overwrite
	movf	FLASH_BUFFER+2, W
	xorlw	ID_BYTE2
	bnz	FLASH_Overwrite
	movf	FLASH_BUFFER+3, W
	xorlw	ID_BYTE3
	bnz	FLASH_Overwrite
	movf	FLASH_BUFFER+4, W
	xorlw	ID_BYTE4
	bnz	FLASH_Overwrite
	movf	FLASH_BUFFER+5, W
	xorlw	ID_BYTE5
	bnz	FLASH_Overwrite
	movf	FLASH_BUFFER+6, W
	xorlw	ID_BYTE6
	bnz	FLASH_Overwrite
	movf	FLASH_BUFFER+7, W
	xorlw	ID_BYTE7
	bz	FLASH_SkipOverwrite

FLASH_Overwrite
	;; ---[ overwrite ID field with new values ]---
	movlw	ID_BYTE0
	movwf	FLASH_BUFFER+0
	movlw	ID_BYTE1
	movwf	FLASH_BUFFER+1
	movlw	ID_BYTE2
	movwf	FLASH_BUFFER+2
	movlw	ID_BYTE3
	movwf	FLASH_BUFFER+3
	movlw	ID_BYTE4
	movwf	FLASH_BUFFER+4
	movlw	ID_BYTE5
	movwf	FLASH_BUFFER+5
	movlw	ID_BYTE6
	movwf	FLASH_BUFFER+6
	movlw	ID_BYTE7
	movwf	FLASH_BUFFER+7

	;; ---[ erase ID block ]---
	lfsr	FSR1, FLASH_BUFFER
	TABLE_ADDR_FULL _IDLOC0
FLASH_Erase
	bsf	EECON1, EEPGD		; point to FLASH program memory
	bcf	EECON1, CFGS		; access FLASH (ID is not in config sector)
	bsf	EECON1, WREN		; enable write to memory
	bsf	EECON1, FREE		; enable Row Erase operation
	movlw	0x55			; write 55h
	movwf	EECON2
	movlw	0xaa			; write AAh
	movwf	EECON2
	bsf	EECON1, WR		; start erase (CPU stall)
	nop				; workaround for PIC18F4620 (and similar newer PICs)
	tblrd*-				; dummy read decrement

	;; ---[ write to block ]---
FLASH_WriteBlock
	clrf	TMP1			; 8 * 8 bytes row to write
FLASH_WriteBlockLoop
FLASH_WriteBlock_ToHRegs
	movf	TMP1, W
	movff	PLUSW1, TABLAT		; transfer byte into table latch
	tblwt+*				; write data, perform a short write
	incf	TMP1, F			; loop until buffers are full
	movf	TMP1, W
	andlw	0x07
	bnz	FLASH_WriteBlock_ToHRegs

FLASH_ProgramMemory
	bsf	EECON1, EEPGD		; point to FLASH program memory
	bcf	EECON1, CFGS		; access FLASH (ID is not in config sector)
	bsf	EECON1, WREN		; enable write to memory
	movlw	0x55			; write 55h
	movwf	EECON2
	movlw	0xaa			; write AAh
	movwf	EECON2
	bsf	EECON1, WR		; start program (CPU stall)

	;; note: the ID block has only 8 bytes, we have to stop here
	;; otherwise the final content will be the last written 
	;; 8 bytes
#if 0
	BRA_IFCLR TMP1, 6, ACCESS, FLASH_WriteBlockLoop
#endif


	bcf	EECON1, WREN		; disable write to memory

	
	;; ---[ ID write finished, reset ]---
	reset

FLASH_SkipOverwrite

	;; ---[ ID write not required, return ]---
	clrf	TBLPTRU

	return

	END

