$Id: $

; ==========================================================================

GLCD Fonts and Icons for MIOS GLCD drivers

; ==========================================================================

Copyright 2002 Thorsten Klose (tk@midibox.org)
Licensed for personal non-commercial use only.
All other rights reserved.

; ==========================================================================

Created in December 2002 for MIDIbox LC project, which was the very
first MIOS application

The .xpm files are manually created with "xpaint"

The appr. inc files have been converted with $MIOS_BIN_PATH/bin/convpix.pl

The .asm files are wrappers for relocatable objects, used in
C applications



Integration Hints
~~~~~~~~~~~~~~~~~

C based Applications (example for glcd_font_big)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  1) Makefile: add "glcd_font_big.o" to OBJS variable

  2) Makefile: include glcd_font.mk

---
include $(MIOS_PATH)/modules/glcd_font/glcd_font.mk
---

  3) .c program: add
---
#include <glcd_font.h>
---
     and select font with:
---
  MIOS_GLCD_FontInit(GLCD_FONT_BIG);
---


"Assembler-Only" Applications (example for glcd_font_big)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  1) main.asm (or main.inc): include the <glcd_font_big.inc> file
     at the end of your *.asm file to integrate the bitmap into 
     your application

  2) Makefile: include glcd_font.mk

---
include $(MIOS_PATH)/modules/glcd_font/glcd_font.mk
---

  3) assembler code: add
---
	TABLE_ADDR GLCD_FONT_BIG		;; use big font
	call	MIOS_GLCD_FontInit
---
     to select a font


Integration Examples
~~~~~~~~~~~~~~~~~~~~

  -> see $MIOS_PATH/apps/examples/lcd7/ks0108
