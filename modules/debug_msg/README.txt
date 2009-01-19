$Id$

This module provides some help functions to send text messages to the
MIOS Terminal, which is part of MIOS Studio

A message always starts with a Header, followed by any number of characters, 
and is finished with the Footer.

The MIOS Terminal will print the message once the Footer has been received.


Files
~~~~~

  debug_msg.inc: can be included into "asm-only" programs (non-relocatable code)
  debug_msg.asm: relocatable wrapper for C based applications
  debug_msg.h:   C headers
  debug_msg.mk:  include this into Makefile to integrate the module


Functions
~~~~~~~~~

extern void DEBUG_MSG_SendHeader(void) __wparam;
extern void DEBUG_MSG_SendFooter(void) __wparam;
extern void DEBUG_MSG_SendChar(char) __wparam;
extern void DEBUG_MSG_SendCString(char *str) __wparam;
extern void DEBUG_MSG_SendBCD1(unsigned char) __wparam;
extern void DEBUG_MSG_SendBCD2(unsigned char) __wparam;
extern void DEBUG_MSG_SendBCD3(unsigned char) __wparam;
extern void DEBUG_MSG_SendBCD4(unsigned int) __wparam;
extern void DEBUG_MSG_SendBCD5(unsigned int) __wparam;
extern void DEBUG_MSG_SendHex1(unsigned char) __wparam;
extern void DEBUG_MSG_SendHex2(unsigned char) __wparam;


For assembly:
   - paramter expected in WREG if not void
   - "unsigned int": MSB expected in MIOS_PARAMETER1 like known from MIOS_LCD_PrintBCD4/5
   - DEBUG_MSG_SendHex2 destroys PRODL!
   - DEBUG_MSG_SendCString replaced by DEBUG_MSG_SendASMString
     This special variation expects the zero-terminated string *after* the function call
     (see coding example below)


Defines
~~~~~~~

  none


Simple C example
~~~~~~~~~~~~~~~~

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // set shift register update frequency
  MIOS_SRIO_UpdateFrqSet(1); // ms

  // we need to set at least one IO shift register pair
  MIOS_SRIO_NumberSet(16); // for 128 pins

  // debouncing value for DINs
  MIOS_SRIO_DebounceSet(10);
}


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam
{
#if 1
  // only for debugging - remove this (or change "#if 1" to "#if 0") in a common application!
  DEBUG_MSG_SendHeader();
  DEBUG_MSG_SendCString("Button: ");
  DEBUG_MSG_SendBCD3(pin);
  DEBUG_MSG_SendChar(' ');
  DEBUG_MSG_SendCString(pin_value ? "depressed" : "pressed");
  DEBUG_MSG_SendFooter();
#endif
}


"Simple" Assembler example
~~~~~~~~~~~~~~~~~~~~~~~~~~

You will notice, that it is much easier to use this feature in C!


;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; set shift register update frequency
	movlw	1 ; ms
	call	MIOS_SRIO_UpdateFrqSet

	;; maximum number of DIN/DOUT shift registers (1..16)
	movlw	16 ; for 128 pins
	call	MIOS_SRIO_NumberSet

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
#if 1
	;; only for debugging - remove this (or change "#if 1" to "#if 0") in a common application!

	;; SendBCD will destroy MIOS_PARAMETER[123] - store them in TMP[123]
	movff	MIOS_PARAMETER1, TMP1
	movff	MIOS_PARAMETER2, TMP2
	movff	MIOS_PARAMETER3, TMP3

	call	DEBUG_MSG_SendHeader

	call	DEBUG_MSG_SendASMString
	db	"Button: ", 0

	movf	TMP1, W
	call	DEBUG_MSG_SendBCD3

	movlw	' '
	call	DEBUG_MSG_SendChar

	movf	TMP2, W
	bz	USER_DIN_NotifyToggle_0
USER_DIN_NotifyToggle_1
	call	DEBUG_MSG_SendASMString
	db	"depressed", 0
	rgoto	USER_DIN_NotifyToggle_Cont

USER_DIN_NotifyToggle_0
	call	DEBUG_MSG_SendASMString
	db	"pressed", 0
USER_DIN_NotifyToggle_Cont
	call	DEBUG_MSG_SendFooter

	;; SendBCD destroyed MIOS_PARAMETER[123] - restore them from TMP[123]
	movff	TMP1, MIOS_PARAMETER1
	movff	TMP2, MIOS_PARAMETER2
	movff	TMP3, MIOS_PARAMETER3
#endif

	return



Integration Hints
~~~~~~~~~~~~~~~~~

C based Applications
~~~~~~~~~~~~~~~~~~~~

  1) Makefile: add "debug_msg.o" to OBJS variable

  2) Makefile: include debug_msg.mk
---
# include debug message module
include $(MIOS_PATH)/modules/debug_msg/debug_msg.mk
---

  3) your .c file: #include <debug_msg.h>



"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  1) main.asm (or main.inc): #include <debug_msg.inc>

  2) Makefile: include debug_msg.mk
---
# include debug message module
include $(MIOS_PATH)/modules/debug_msg/debug_msg.mk
---
