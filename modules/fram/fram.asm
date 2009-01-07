; Wrapper for fram.inc
; To be used for relocatable programs
;
; ==========================================================================

	radix	dec

#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; If set to 1, mios_iic functions will be used (@400kHZ)
#ifndef FRAM_MIOS_IIC
#define FRAM_MIOS_IIC 0
#endif

; ==========================================================================
; Import/Export Labels
; ==========================================================================

;	global	_mod_skel_function		; (for C, declaration in mod_skel.h)
;	global	_mod_skel_var

; ==========================================================================
;  Declare variables 
; ==========================================================================

;this buffer is only needed when fram_iic.inc is used instead of mios_iic
#if FRAM_MIOS_IIC==0
FRAM_IIC_VARS UDATA
FRAM_IIC_BUF RES 1
#endif


; ==========================================================================
; Start code section and include driver code
; ==========================================================================
FRAM CODE

#include "fram.inc"

; ==========================================================================

	;; C function wrappers
	;; These move the arguments of your C functions into the appropriate variables
	
	;; YOU ONLY NEED ONE OF THESE FOR EACH FUNCTION IN YOUR MODULE
	;; There are three examples here, but this skeleton only uses the first
	

;_mod_skel_function		; (no arguments, or direct passing of variables)


; _mod_skel_function	; example shows 
; 						; (one byte argument, function declared with  __wparam keyword)
;						; EG: void mod_skel_function(unsigned char myvar);
; 	movff	WREG,	_mod_skel_var
; 	rgoto	mod_skel_function


; _mod_skel_function	; example shows 
; 						; a char (1 byte, stared in W thanks to __wparam), 
; 						; an int (2 bytes)
;						; EG: void mod_skel_function(unsigned char myvar, signed int myint);
; 	movff	WREG,	_mod_skel_var		; get first byte of arguments from W
; 
; 	movff	FSR0L, FSR2L		; get other arguments from stack
; 	movff	PREINC2, _yourint+0
; 	movff	PREINC2, _yourint+1
; 	rgoto	mod_skel_function


; ==========================================================================
	END
