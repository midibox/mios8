$Id: README.TXT 156 2008-03-01 20:50:30Z tk $

Button/Duo-LED Example
===============================================================================
Copyright <year> <your name>
Licensed for personal non-commercial use only.
All other rights reserved.
===============================================================================

A precompiled binary is already part of this package:
   o project.hex (can be loaded into MIOS Studio)

Following tools are required to recompile the code:
   o SDCC
   o gputils

The details are described under
   http://www.midibox.org/dokuwiki/application_development

===============================================================================

Required hardware:
   o one MBHP_CORE module
   o one DINX2 module (or DINX4) and two DOUTX4 modules
     or dedicated board for BLM matrix as specified under:
     http://www.ucapps.de/mbhp/button_duoled_matrix.pdf

===============================================================================

This program demonstrates the so called "BLM" driver (-> $MIOS_PATH/modules/blm/)

This driver handles a 4x16 Button/Duo-LED scan matrix of up to 64 buttons and 
64 Duo-LEDs by using the circuit documented under:
  http://www.ucapps.de/mbhp/button_duoled_matrix.pdf

The hardware was originally used for MIDIbox SEQ V3, but could also be useful for
other projects.

Note that the buttons/LEDs can be easily arranged to a 8x8 matrix w/o software 
modifications.

Please note also, that the shift register assignments BLM_* need
to be adapted to your hardware (see description in $MIOS_PATH/modules/blm/blm.inc).

The setup used in this example is prepared for connecting the DIN/DOUT chain 
directly to the MBHP_CORE module (there are no additional DINX4 or DOUTX4
modules between the BLM and the core like on the MBSEQ V3 hardware).


The function "BLM_NotifyToggle()" in main.c is called when a button has been 
pressed or released. LEDs are accessible from the blm_row_green[8] and
blm_row_red[8] array.

By default, this application sends Note Events from Number 0 (C -1) to 63 (E 4)
and it receives the same numbers to control the LEDs:
   Velocity 0x00 (or Note off): green and red LED off
   Velocity 0x01..0x3f:         green LED on, red LED off
   Velocity 0x40..0x5f:         green LED off, red LED on
   Velocity 0x60..0x7f:         both LEDs on

Optionally some code can be enabled in the BLM_NotifyToggle function
(located in main.c) to cycle the LED colour with the appr. buttons.

Buttons can be debounced with a definable delay (-> blm_button_debounce_delay)
The resulting delay is calculated as <value> * 4 mS
The Init() function in main.c sets the value to 20 (-> 20*4 mS = 80 mS)
If you want to experiment with different delays, you could (temporary)
control it with a CC event from MPROC_NotifyReceivedEvnt() function in
main.c with following code:

  if( evnt0 == 0xb0 && evnt1 == 16 ) { // control debounce delay with CC#16 over Channel #1
    blm_button_debounce_delay = evnt2;
  }

Don't forget to remove this change, so that a common MIDI application
isn't able to modify the value unintentionally.


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

===============================================================================

Description about the most important files:

   - mios_wrapper\mios_wrapper.asm and mios_wrapper\mios_tables.inc:
     The MIOS wrapper code and MIOS specific configuration tables

   - pic18f452.c: exports PIC18F452 specific SFRs

   - main.c: the main program with all MIOS hooks

   - $MIOS_PATH/modules/blm/README.txt: details about BLM driver

There are additional .h files for all .c files which contain
general definitions and the declaration of global functions/variables
These .h files must be included into the program parts which
get use of these globals

===============================================================================
