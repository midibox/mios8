$Id$

Driver for Button/Duo-LED matrix

This driver handles a 4x16 Button/Duo-LED scan matrix of up to 64 buttons 
and 64 Duo-LEDs by using the circuit documented under:
  http://www.ucapps.de/mbhp/button_duoled_matrix.pdf

The hardware was originally used for MIDIbox SEQ V3, but could also be 
useful for other projects.

Note that the buttons/LEDs can be easily arranged to a 8x8 matrix w/o 
software modifications.

Please note also, that the shift register assignments BLM_*
need to be adapted to your hardware (see description in $MIOS_PATH/modules/blm/blm.inc). 
The setup is prepared for connecting the DIN/DOUT chain directly to the 
MBHP_CORE module (there are no additional DINX4 or DOUTX4 modules 
between the BLM and the core like on the MBSEQ V3 hardware).

The external function "BLM_NotifyToggle()" (part of your application) is 
called when a button has been pressed or released. LEDs are accessible from 
the blm_row_green[8] and blm_row_red[8] array.

Buttons can be debounced with a definable delay (-> blm_button_debounce_delay)
The resulting delay is calculated as <value> * 4 mS

Column/Row Scrambling:
The blm_button_row alternates between the two sides of the original
hardware. Within a 4x16 matrix, following (col,row) numbers are
assigned to the button/LEDs:

  (0,0)(1,0)(2,0)(3,0)(4,0)(5,0)(6,0)(7,0) (0,1)(1,1)(2,1)(3,1)(4,1)(5,1)(6,1)(7,1)
  (0,2)(1,2)(2,2)(3,2)(4,2)(5,2)(6,2)(7,2) (0,3)(1,3)(2,3)(3,3)(4,3)(5,3)(6,3)(7,3)
  (0,4)(1,4)(2,4)(3,4)(4,4)(5,4)(6,4)(7,4) (0,5)(1,5)(2,5)(3,5)(4,5)(5,5)(6,5)(7,5)
  (0,6)(1,6)(2,6)(3,6)(4,6)(5,6)(6,6)(7,6) (0,7)(1,7)(2,7)(3,7)(4,7)(5,7)(6,7)(7,7)

This results into following scrambling, when the sides are arranged to
a 8x8 matrix:
  (0,0)(1,0)(2,0)(3,0)(4,0)(5,0)(6,0)(7,0)
  (0,2)(1,2)(2,2)(3,2)(4,2)(5,2)(6,2)(7,2)
  (0,4)(1,4)(2,4)(3,4)(4,4)(5,4)(6,4)(7,4)
  (0,6)(1,6)(2,6)(3,6)(4,6)(5,6)(6,6)(7,6)
  (0,1)(1,1)(2,1)(3,1)(4,1)(5,1)(6,1)(7,1)
  (0,3)(1,3)(2,3)(3,3)(4,3)(5,3)(6,3)(7,3)
  (0,5)(1,5)(2,5)(3,5)(4,5)(5,5)(6,5)(7,5)
  (0,7)(1,7)(2,7)(3,7)(4,7)(5,7)(6,7)(7,7)


C usage example -> $MIOS_PATH/apps/examples/button_duoled_matrix


Files
~~~~~

  blm.inc: can be included into "asm-only" programs (non-relocatable code)
  blm.asm: relocatable wrapper for C based applications
  blm.h:   C headers
  blm.mk:  include this into Makefile to integrate the module


Defines
~~~~~~~

  C programs (relocatable code): can be overruled with -D<define>=<value> from command line 
  Assembly programs: has to be defined within program before "#include <iic_midi.inc>"

  -> see also descriptions in blm.inc



Integration Hints
~~~~~~~~~~~~~~~~~

C based Applications
~~~~~~~~~~~~~~~~~~~~

  1) Makefile: add "blm.o" to OBJS variable

  2) Makefile: include blm.mk, and enhance BLM_DEFINES if required:
---
# include Button/Duo-LED driver
# use shift register assignments which are matching with MBSEQ V3 hardware
BLM_DEFINES += -DBLM_DOUT_L1=6 -DBLM_DOUT_R1=9
BLM_DEFINES += -DBLM_DOUT_CATHODES1=5 -DBLM_DOUT_CATHODES2=8
BLM_DEFINES += -DBLM_CATHODES_INV_MASK=0x00
BLM_DEFINES += -DBLM_DOUT_L2=7 -DBLM_DOUT_R2=10
BLM_DEFINES += -DBLM_DIN_L=11 -DBLM_DIN_R=12
BLM_DEFINES += -DBLM_NO_DEBOUNCING=0
include $(MIOS_PATH)/modules/blm/blm.mk
---

  3) main.c: #include <blm.h>

  4) main.c: add following code to Init() hook
---
  // initialize the scan matrix driver
  BLM_Init();

  // set initial debounce delay (should be done after BLM_Init(), as this function overwrites the value)
  blm_button_debounce_delay = 20;    // (-> 20 * 4 mS = 80 mS)

  // initialize the shift registers
  MIOS_SRIO_NumberSet(16);           // use up to 16 shift registers
  MIOS_SRIO_UpdateFrqSet(1);         // set update frequency
  MIOS_SRIO_TS_SensitivitySet(0);    // disable touch sensors
---

  5) main.c: add following code to Tick() hook
---
  // call the scan matrix button handler
  BLM_ButtonHandler();
---

  6) main.c: add following code:
---
/////////////////////////////////////////////////////////////////////////////
// This function is NOT called by MIOS, but by the scan matrix handler
// in $MIOS_PATH/modules/blm, when a pin of the scan matrix has been toggled
// Note: in addition to "pin" and "value", the "blm_button_column" and
// "blm_button_row" are available as global variables (defined in blm.h)
/////////////////////////////////////////////////////////////////////////////
void BLM_NotifyToggle(unsigned char pin, unsigned char value) __wparam
{
  // add button handling code here
}
---



"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

integration is possible, but not documented yet

