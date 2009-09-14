; $Id$
;
; MIOS Definitions file
;
; ==========================================================================
;
;  Copyright 1998-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; ==========================================================================
;;  Include basic defines which characterize the derivative to
;;  simplify dependencies within MIOS source code
;; ==========================================================================
#include <hw_flags.h>
 
;; ==========================================================================
;;  Assembler directives & device specific include file
;;
;;  Natively supported devices are listed here.
;; ==========================================================================

#ifdef __18F452
	LIST R=DEC
#include <p18f452.inc>
#endif

#ifdef __18F4620
	LIST R=DEC
#include <p18f4620.inc>
#endif
	
#ifdef __18F4520
	LIST R=DEC
#include <p18f4520.inc>
#endif

#ifdef __18F4682
	LIST R=DEC
#include <p18f4682.inc>
#endif

#ifdef __18F4685
	LIST R=DEC
#include <p18f4685.inc>
#endif


;; ==========================================================================
;;  General constants
;; ==========================================================================

;; used by MIOS_MIDI_Interface*
MIOS_MIDI_INTERFACE_COMMON	EQU	0x00
MIOS_MIDI_INTERFACE_TO_HOST	EQU	0x01

;; used by MIOS_MIDI_Merger*
MIOS_MIDI_MERGER_DISABLED	EQU	0x00
MIOS_MIDI_MERGER_ENABLED	EQU	0x01
MIOS_MIDI_MERGER_MBLINK_EP	EQU	0x02
MIOS_MIDI_MERGER_MBLINK_FP	EQU	0x03

;; used by MIOS_ENC_PIN_TABLE
#ifdef MIOS_OLD_ENCODER_MODES
MIOS_ENC_MODE_NON_DETENTED	EQU	0x00
MIOS_ENC_MODE_DETENTED		EQU	0x80
MIOS_ENC_MODE_DETENTED1		EQU	0x80
MIOS_ENC_MODE_DETENTED2		EQU	0x81
MIOS_ENC_MODE_DETENTED3		EQU	0x82
#else
MIOS_ENC_MODE_NON_DETENTED	EQU	0xff
MIOS_ENC_MODE_DETENTED		EQU	0xaa
MIOS_ENC_MODE_DETENTED1		EQU	0xaa
MIOS_ENC_MODE_DETENTED2		EQU	0x22
MIOS_ENC_MODE_DETENTED3		EQU	0x88
MIOS_ENC_MODE_DETENTED4		EQU	0xa5
MIOS_ENC_MODE_DETENTED5		EQU	0x5a
#endif

;; used by MIOS_ENC_Speed*
MIOS_ENC_SPEED_SLOW		EQU	0
MIOS_ENC_SPEED_NORMAL		EQU	1
MIOS_ENC_SPEED_FAST		EQU	2

;; used by MIOS_LCD_Type*
MIOS_LCD_TYPE_CLCD		EQU	0x00
MIOS_LCD_TYPE_GLCD0		EQU	0x01
MIOS_LCD_TYPE_GLCD1		EQU	0x02
MIOS_LCD_TYPE_GLCD2		EQU	0x03
MIOS_LCD_TYPE_GLCD3		EQU	0x04
MIOS_LCD_TYPE_GLCD4		EQU	0x05
MIOS_LCD_TYPE_MLCD		EQU	0x06
MIOS_LCD_TYPE_GLCD_CUSTOM	EQU	0x07

;; location of default 5x8 font for graphical displays
MIOS_GLCD_FONT			EQU	0x7cfc

;; general config flags (never overwrite this flags directly!!!)
MIOS_BOX_CFG0_LCD_TYPE0		EQU 0	; selects LCD type, bit 0
MIOS_BOX_CFG0_LCD_TYPE1		EQU 1	; selects LCD type, bit 1
MIOS_BOX_CFG0_LCD_TYPE2		EQU 2	; selects LCD type, bit 2
MIOS_BOX_CFG0_USE_GLCD		EQU 3	; if 1, graphical LCD is connected
MIOS_BOX_CFG0_MERGER		EQU 4	; if 1, MIDI merger is enabled
MIOS_BOX_CFG0_MBLINK		EQU 5	; if 1, MIDIbox Link is enabled
MIOS_BOX_CFG0_TO_HOST		EQU 6	; if 1, MIDI interface will run with 38400 baud instead of 31250
MIOS_BOX_CFG0_20MHz		EQU 7	; if 1, it is assumed that box is running with 20 MHz, else with 40 MHz

MIOS_BOX_CFG1_BS_A0		EQU 0	; A0 of BankStick (never change the bit position)
MIOS_BOX_CFG1_BS_A1		EQU 1	; A1 of BankStick (never change the bit position)
MIOS_BOX_CFG1_BS_A2		EQU 2	; A2 of BankStick (never change the bit position)
MIOS_BOX_CFG1_BS_DIS_VERIFY	EQU 3	; if 1, writes to the BankStick will not be verified
MIOS_BOX_CFG1_IIC_STRETCH_CLK	EQU 4	; if 1, IIC clock stretching enabled

;; general status flags (never overwrite this flags directly!!!)
MIOS_BOX_STAT_BS_AVAILABLE	EQU 0	; if 1, BankStick is available
MIOS_BOX_STAT_MBLINK_TUNNEL_PASS EQU 1  ; if 1, a MBLinked event will be tunnled
MIOS_BOX_STAT_SUSPEND_ALL       EQU 2   ; if 1, all system and user tasks are suspended
MIOS_BOX_STAT_SUSPEND_USER      EQU 3   ; if 1, all user tasks are suspended
MIOS_BOX_STAT_AUTOREPEAT	EQU 4	; if 1, start the auto-repeat handler
MIOS_BOX_STAT_MLCD_TRANSFER	EQU 5	; if 1, a transfer to the MLCD has been started
MIOS_BOX_STAT_FERR		EQU 6	; if 1, a frame error happened during MIDI receive

;; ==========================================================================
;;  MIOS Special Function Registers
;; ==========================================================================

MIOS_BOX_CFG0	EQU	0x000
MIOS_BOX_CFG1	EQU	0x001
MIOS_BOX_STAT	EQU	0x002
MIOS_PARAMETER1	EQU	0x003
MIOS_PARAMETER2	EQU	0x004
MIOS_PARAMETER3	EQU	0x005

;; ==========================================================================
;;  temporary registers for main programs
;; ==========================================================================
TMP1		EQU	0x006
TMP2		EQU	0x007
TMP3		EQU	0x008
TMP4		EQU	0x009
TMP5		EQU	0x00a

;; ==========================================================================
;;  temporary registers for IRQs
;; ==========================================================================
IRQ_TMP1	EQU	0x00b
IRQ_TMP2	EQU	0x00c
IRQ_TMP3	EQU	0x00d
IRQ_TMP4	EQU	0x00e
IRQ_TMP5	EQU	0x00f

;; ==========================================================================
;;  free memory space for user application:
;; ==========================================================================
	;; 0x010-0x37f

;; ==========================================================================
;;  MIOS System Registers -- not defined here -- 
;;  never use these memory locations!
;; ==========================================================================
	;; 0x380-0x6ff

;; ==========================================================================
;;  MIOS System Registers for custom LCD driver
;; ==========================================================================
MIOS_GLCD_BUFFER	EQU	0x570	; note: the buffer must be located at ...0-...7
MIOS_LCD_OPTION1	EQU	0x578	; contains the first LCD option given by MIOS_LCD_TypeSet
MIOS_LCD_OPTION2	EQU	0x579	; contains the second LCD option given by MIOS_LCD_TypeSet
MIOS_LCD_CURSOR_POS	EQU	0x57a	; the current cursor pos of characters (GLCD: multiplied by width)
MIOS_GLCD_GCURSOR_X	EQU	0x57b	; for GLCDs: the current X position of graphical cursor
MIOS_GLCD_GCURSOR_Y	EQU	0x57c	; for GLCDs: the current Y position of graphical cursor
MIOS_GLCD_FONT_WIDTH	EQU	0x57d	; for GLCDs: the fontwidth given by MIOS_GLCD_FontInit
MIOS_GLCD_FONT_HEIGHT	EQU	0x57e	; for GLCDs: the fontheight given by MIOS_GLCD_FontInit
MIOS_GLCD_FONT_X0	EQU	0x57f	; for GLCDs: the first byte within a char entry
MIOS_GLCD_FONT_OFFSET	EQU	0x580	; for GLCDs: the byte offset between the characters
MIOS_GLCD_FONT_PTRL	EQU	0x581	; for GLCDs: pointer to the character table, low-byte
MIOS_GLCD_FONT_PTRH	EQU	0x582	; for GLCDs: pointer to the character table, high-byte
MIOS_LCD_TIMEOUT0	EQU	0x583   ; can be used for timeout loops
MIOS_LCD_TIMEOUT1	EQU	0x584   ; can be used for timeout loops
MIOS_GLCD_TMP1		EQU	0x585   ; can be used as temporary buffer
MIOS_GLCD_TMP2		EQU	0x586   ; can be used as temporary buffer
MIOS_GLCD_TMP3		EQU	0x587   ; can be used as temporary buffer
MIOS_GLCD_TMP4		EQU	0x588   ; can be used as temporary buffer
MIOS_LCD_Y0_OFFSET	EQU	0x589   ; Y0 offset of LCD
MIOS_LCD_Y1_OFFSET	EQU	0x58a   ; Y1 offset of LCD
MIOS_LCD_Y2_OFFSET	EQU	0x58b   ; Y2 offset of LCD
MIOS_LCD_Y3_OFFSET	EQU	0x58c   ; Y3 offset of LCD
MIOS_LCD_CURSOR_POS_REAL EQU	0x58d	; unmapped cursor position which has been set with MIOS_LCD_CursorSet
MIOS_GLCD_FONT_PTRU	EQU	0x58e	; for GLCDs: pointer to the character table, upper-byte (>64k flash derivatives only)
