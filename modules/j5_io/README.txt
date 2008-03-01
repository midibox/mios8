$Id$

This driver provides functions to use the pins at MBHP_CORE:J5 port
as digital inputs and/or outputs


Files
~~~~~

  j5_io.inc: can be included into "asm-only" programs (non-relocatable code)
  j5_io.asm: relocatable wrapper for C based applications
  j5_io.h:   C headers
  j5_io.mk:  include this into Makefile to integrate the module


Defines
~~~~~~~

  used in j5_io.inc
    C programs (relocatable code): can be overruled with -D<define>=<value> from command line 
    Assembly programs: has to be defined within program before "#include <j5_io.inc>"
  
;; if set, input functions won't be provided by the driver
#define J5_IO_DONT_USE_INPUT_FUNCTIONS	0

;; if set, output functions won't be provided by the driver
#define J5_IO_DONT_USE_OUTPUT_FUNCTIONS	0

Both defines are provided for saving code size in older "over-featured"
PIC18F452 based MIDIbox applications


Available Functions
~~~~~~~~~~~~~~~~~~~

see j5_io.inc for descriptions, and j5_io.h for C declarations


Integration Hints
~~~~~~~~~~~~~~~~~

C based Applications
~~~~~~~~~~~~~~~~~~~~

  1) Makefile: add "j5_io.o" to OBJS variable

  2) Makefile: include j5_io.mk, and enhance J5_IO_DEFINES if required:
---
# include J5_IO driver
J5_IO_DEFINES += -DJ5_IO_DONT_USE_INPUT_FUNCTIONS=1
include $(MIOS_PATH)/modules/j5_io/j5_io.mk
---

  3) main.c: add J5_Init(<tris-value>) to Init() hook



"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  1a) main.asm (or main.inc): #include <j5_io.inc>

  1b) optionally: add application specific #defines to the
      assembly code before the #include statement
      #define J5_IO_DONT_USE_INPUT_FUNCTIONS 1
      #include <j5_io.inc>     


  2) Makefile: include j5_io.mk

  3) main.asm (or main.inc): add following code to USER_Init hook
     movlw   0x<tris-value>
     call    J5_IO_Init

     <tris-value>: e.g. 0x00 for output, 0xff for input



