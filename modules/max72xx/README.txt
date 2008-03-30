$Id$

Driver for MAX7221 and MAX7119 LED multiplexer ICs


C usage example -> $MIOS_PATH/apps/controllers/stribe
ASM usage example -> not available yet


Files
~~~~~

  max72xx.inc: can be included into "asm-only" programs (non-relocatable code)
  max72xx.asm: relocatable wrapper for C based applications
  max72xx.h:   C headers
  max72xx.mk:  include this into Makefile to integrate the module


Defines
~~~~~~~

  used in max72xx.inc
    C programs (relocatable code): can be overruled with -D<define>=<value> from command line 
    Assembly programs: has to be defined within program before "#include <iic_midi.inc>"
  
; Number of connected MAX72xx chips
#define MAX72XX_CHAINLENGTH 1

; Pin Configuration
#define MAX72XX_LAT_CS		LATC	; The chip select pin
#define MAX72XX_TRIS_CS		TRISC	; is connected to Port C.0
#define MAX72XX_PIN_CS		0	; (CANNOT be shared with other outputs!)

#define MAX72XX_LAT_SCLK	LATC	; The shift clock input pin SCLK
#define MAX72XX_TRIS_SCLK	TRISC	; is connected to Port C.1
#define MAX72XX_PIN_SCLK	1	; (can be shared with other outputs)

#define MAX72XX_LAT_DIN		LATC	; The data input pin DIN
#define MAX72XX_TRIS_DIN	TRISC	; is connected to Port C.2
#define MAX72XX_PIN_DIN		2	; (can be shared with other outputs)


Integration Hints
~~~~~~~~~~~~~~~~~

C based Applications
~~~~~~~~~~~~~~~~~~~~

  1) Makefile: add "max72xx.o" to OBJS variable

  2) Makefile: include max72xx.mk, and enhance MAX72XX_DEFINES if required:
---
# include MAX72xx driver
MAX72XX_DEFINES += -DMAX72XX_CHAINLENGTH=2
include $(MIOS_PATH)/modules/max72xx/max72xx.mk
---

  3) main.c: #include <max72xx.h>

  4) main.c: add MAX72XX_Init() to Init() hook



"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  1a) add application specific #defines to the assembly code (e.g. in
      your setup_*.asm file), e.g.:
      #define MAX72XX_CHAINLENGTH 2

  1b) main.asm (or main.inc): #include <max72xx.inc>

  2) Makefile: include max72xx.mk

  3) main.asm (or main.inc): call MAX72XX_Init from USER_Init hook

  4) app_defines.inc: add following array to a free location
---
;; MAX72xx module
_max72xx_digits         EQU     0x080; .. MAX72XX_CHAINLENGTH * 8
---

  5) take care, that this array consumes 8 bytes per MAX72XX, e.g.
     if one MAX72XX is used: 0x080..0x087
     if two MAX72XX are used: 0x080..0x08f
     etc...


