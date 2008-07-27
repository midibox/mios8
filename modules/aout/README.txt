$Id$

Driver for AOUT modules
=======================


Oversight
~~~~~~~~~

This driver provides functions to access DACs which are used in
the MIDIbox hardware platform (MBHP) to output analog control
voltages (MBHP_AOUT*).

The API is identical for all AOUT modules to allow highest flexibility. New
transfer protocols can be added w/o touching an existing application.

DAC and CS/CLK/DIN pin have to selected from the Makefile/Assembly source
during compile time.


Implementation Concept
~~~~~~~~~~~~~~~~~~~~~~

The application interface (API) has been tailored around AOUT modules, which
are part of the MIDIbox Hardware Platform (MBHP), and MIOS based applications
like MIDIbox CV (CV control via MIDI), MIDIbox SEQ (CV control from a 
sequencer) and MIDIbox SID/FM (CV control from a synthesizer)

Up to 8 analog outputs are supported (*)
An interface to control digital pins is available as well (**)

Output voltages are managed in 16bit resolution. Although none of the current
modules support the full resolution, from programming and performance
perspective it makes sense to organize CV values this way.

An additional function for setting pins in 7bit resolution is available,
which left-aligns the value to the 16bit format. The usage of this function is 
prefered for applications which are working at common MIDI resolution.

Before voltage changes are transfered to the external hardware, the 
AOUT_Pin16bitSet/AOUT_Pin7bitSet function compares the new value with the 
current one. If equal, the register transfer will be omitted, otherwise it
will be requested and performed once AOUT_Update() is called.

This method has two advantages:
   - if AOUT_Pin*bitSet doesn't notice value changes, the appr. AOUT channels
     won't be updated to save CPU performance
   - all CV pins will be updated at the same moment


(*) currently only limited by the AOUT_UPDATE_REQ variable. This could be enhanced 
in future if really required, but this would cost performance!

(**) currently only supported for MBHP_AOUT, since MAX525 provides such a digital output pin


Supported AOUT interfaces
~~~~~~~~~~~~~~~~~~~~~~~~~

AOUT_INTERFACE_TYPE == 0
   selects a dummy module which with empty function stubs (-> no external accesses)

AOUT_INTERFACE_TYPE == 1
   CV: up to 2 cascaded MAX525 (MBHP_AOUT module), each MAX525 provides 
   4 channels with 12bit resolution
   Digital: 1 pin per IC

AOUT_INTERFACE_TYPE == 2
   CV: up to 8 cascaded 74HC595 (each MBHP_AOUT_LC module provides 2 74HC595)
       resolution is selectable with AOUT_LC_RESOLUTION_OPTION_M1 .. 4:
       AOUT_LC_RESOLUTION_OPTION_Mx == 0: 12/4 bit configuration
       AOUT_LC_RESOLUTION_OPTION_Mx == 1: 8/8 bit configuration
       AOUT_LC_RESOLUTION_OPTION_Mx == 2: combines 4 shift registers. The first
          two SRs are working like option 0 (12/4). The third SR outputs the 
          lower bits of second channel, and the fourth SR the 8bit value of 
          third channel.
          The fourth channel value is ignored by this configuration
          (same for channel 4-7: channel 8 igored)
          For this "combined" option, it is recommented to set both modes
          (M1,M2 and/or M3/M4) to value 2 for future compatibility
   Digital: none

AOUT_INTERFACE_TYPE == 3
   CV: one TLV5630 (MBHP_AOUT_NG module) with 8 channels at 12bit resolution
   Digital: none


C usage examples
~~~~~~~~~~~~~~~~

  -> $MIOS_PATH/apps/processing/analog_toolbox


Assembler usage examples
~~~~~~~~~~~~~~~~~~~~~~~~

  -> $MIOS_PATH/apps/processing/midibox_cv
  -> $MIOS_PATH/apps/sequencers/*
  -> $MIOS_PATH/apps/synthesizers/*


Functions
~~~~~~~~~

See aout.inc for function descriptions
See aout.h for C declarations


Files
~~~~~

  aout.inc: toplevel of AOUT driver. Can be included into "asm-only" programs (non-relocatable code)
  sdcard.asm: relocatable wrapper for C based applications
  aout.h:   C header
  aout.mk:  include this into Makefile to integrate the module
  aout_*.inc: low-level drivers for the selected interface


Defines
~~~~~~~

  C programs (relocatable code): can be overruled with -D<define>=<value> from command line 
  Assembly programs: add #define statements within program before "#include <aout.inc>"

AOUT_INTERFACE_TYPE     ; 0..3, see description above

AOUT_LC_RESOLUTION_OPTION_M1  ; 0..2, see description above
AOUT_LC_RESOLUTION_OPTION_M2
AOUT_LC_RESOLUTION_OPTION_M3
AOUT_LC_RESOLUTION_OPTION_M4


AOUT_LAT_CS=LATC	; The chip select pin
AOUT_TRIS_CS=TRISC	; is connected to Port C.5
AOUT_PIN_CS=5		; (CANNOT be shared with other outputs!)

AOUT_LAT_DIN=LATC	; The data input pin DIN
AOUT_TRIS_SCLK=TRISC	; is connected to Port C.4
AOUT_PIN_SCLK=4		; (can be shared with other outputs)

AOUT_LAT_SCLK=LATD	; The shift clock input pin SCLK
AOUT_TRIS_SCLK=TRISD	; is connected to Port D.5
AOUT_PIN_SCLK=5		; (can be shared with other inputs)



Integration Hints
~~~~~~~~~~~~~~~~~

C based Applications
~~~~~~~~~~~~~~~~~~~~

  1) Makefile: add "aout.o" to OBJS variable

  2) Makefile: include aout.mk, and enhance AOUT_DEFINES if required:
---

# include SD Card Driver
# use following pin assignments:
AOUT_DEFINES += -DAOUT_INTERFACE_TYPE=1
AOUT_DEFINES += -DAOUT_LAT_CS=LATC -DAOUT_TRIS_CS=TRISC -DAOUT_PIN_CS=0
AOUT_DEFINES += -DAOUT_LAT_DIN=LATC -DAOUT_TRIS_DIN=TRISC -DAOUT_PIN_DIN=4
AOUT_DEFINES += -DAOUT_LAT_SCLK=LATD -DAOUT_TRIS_SCLK=TRISD -DAOUT_PIN_SCLK=5
include $(MIOS_PATH)/modules/aout/aout.mk
---

  3) main.c: #include <aout.h>

  4) main.c: add following code to Init() hook
---
  // initialize AOUT driver
  AOUT_Init();
---

  5) main.c: add following code to Tick() hook
---
  // update AOUT channels
  AOUT_Update();
---


"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

integration is possible, but not documented yet
However, most applications like MBCV, MBSID, MBFM, MBSEQ are using this driver!
