$Id: README.txt 69 2008-02-01 00:20:18Z tk $

Module Skeleton

Your description goes here. As for this skeleton:
Search for "mod_skel" to find stuff you should change when making your module

Files
~~~~~

  mod_skel.inc: can be included into "asm-only" programs (non-relocatable code)
  mod_skel.asm: relocatable wrapper for C based applications
  mod_skel.h:	C headers
  mod_skel.mk:  include this into Makefile to integrate the driver


Integration Hints
~~~~~~~~~~~~~~~~~

C based Applications
~~~~~~~~~~~~~~~~~~~~

  1) Makefile: add "mod_skel.o" to OBJS variable
  
---
# list of objects that should be created and linked
OBJS = mios_wrapper.o mod_skel.o app_lcd.o main.o
---

  2) Makefile: include mod_skel.mk, and enhance mod_skel_DEFINE if required:

---
# include mod_skel driver	; in our example it allows the 'incf' if this is 1, or if you skip this line
mod_skel_DEFINE += -Dmod_skel_def=0

include $(MIOS_PATH)/modules/mod_skel/mod_skel.mk
---

  3) main.c: #include <mod_skel.h>

  4) main.c: add following code to Tick() hook
  
---
  // do the mod_skel thing
  mod_skel_function();
---  

  5) main.c: add code to main.c for functions called by your module:


---
/////////////////////////////////////////////////////////////////////////////
// This function is called by mod_skel
/////////////////////////////////////////////////////////////////////////////
void mod_skel_extfunc(void) __wparam
{
 // Just a dummy empty function
 --mod_skel_var;
}
---

  6) main.c: etc etc etc


  
  
  
"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


  1a) add application specific #defines to the assembly code (e.g. in
      your main.asm file), e.g.:
---	  
#define mod_skel_def=1
---
	  
  1b) main.asm (or main.inc) include the module source: 

---  
#include <mod_skel.inc>
---

  2) Makefile: include mod_skel.mk, and enhance mod_skel_DEFINE if required:

---
# include mod_skel driver	; in our example if this is 1, or by default if you skip this line, it allows the 'incf'
mod_skel_DEFINE += -Dmod_skel_def=0

include $(MIOS_PATH)/modules/mod_skel/mod_skel.mk
---

  3) app_defines.inc: add the variables you can find declared in mod_skel.asm
     Possible Range: 0x10..0x7f
	 Make sure the EQU address is unique in your app!
	 
---
;; for mod_skel module
_mod_skel_var	EQU	0x067
mod_skel_int	EQU	0x068; ..0x69
---

  4) main.asm (or main.inc): add the following code to the USER_Tick function: 
  
---
;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	call mod_skel_function
	return
---

  
  5) main.asm (or main.inc): add functions which are called from your module:
  
---
;; --------------------------------------------------------------------------
;;  This function is called by mod_skel module
;; --------------------------------------------------------------------------
mod_skel_extfunc
_mod_skel_extfunc
	decf _mod_skel_var, F, BANKED
	return
---

    
  6) main.asm (or main.inc): etc etc etc
  

  