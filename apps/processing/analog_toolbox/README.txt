$Id$

Analog Toolbox V1.2                    (C) 2005 Thorsten Klose (tk@midibox.org)
===============================================================================

This application allows to output analog values at the AOUT module
depending on analog values, MIDI events and internally generated waveforms

It's definitely not a professional tool, but just a toy for having
some fun!

Everybody is welcome for sharing additional modules and/or processing
algorithms to the MIDIbox community!

===============================================================================

A precompiled binary is already part of this package:
   o project.hex (can be loaded into MIOS Studio)
   o project.syx (can be loaded into any SysEx upload tool)

Following tools are required to recompile the code:
   o SDCC
   o gputils

The details are described under
   http://www.midibox.org/dokuwiki/application_development

===============================================================================

Required hardware:
   o one MBHP_CORE module
   o one MBHP_AOUT module

Optional hardware (not supported by default, but provided by MIOS):
   o up to 2*AINX4 for up to 64 analog inputs (reduces the sampling frequency!)
   o up to 4*DOUTX4 for up to 128 digital outputs
   o up to 4*DINX4 for up to 128 digital inputs
   o LCD - but note that it affects the realtime performance!
   o up to 8 BankSticks to store waveforms or other tables

===============================================================================

Configuration steps:

   o check the "general application settings" in main.h if changes are
     required for your hardware setup
     By default 8 analog inputs are used in "UnMuxed mode" (direct
     connection to CORE:J5 without AINX4 modules)

     Remember: unusued analog pins on the CORE module should be
     clamped to ground!

   o check the pin configuration for the AOUT module in aout.h
     The default pinning is equal to the pinning used by MIDIbox CV
  
   o open map.c and adapt the "signal processing" for your needs

   o the application can be rebuilt with the "make.bat" file
     (type "make" in a DOS command shell)

===============================================================================

Description about the most important files:

   - mios_wrapper\mios_wrapper.asm and mios_wrapper\mios_tables.inc:
     The MIOS wrapper code and MIOS specific configuration tables

   - pic18f452.c: exports PIC18F452 specific SFRs

   - main.c: the main program with all MIOS hooks

   - midi.c: the MIDI processing module (1)

   - lfo.c: provides two digital LFOs (2)

   - eg.c: provides an envelope generator (3)

   - map.c: handles the "signal processing" (analog output values are
     determined here) (4)

   - modules/aout/: the AOUT module driver (5)


There are additional .h files for all .c files which contain
general definitions and the declaration of global functions/variables
These .h files must be included into the program parts which
get use of these globals


-------------------------------------------------------------------------------
(1) midi.c

this module provides following global variables:

   midi_note_t midi_note[16]   (union with two elements: NOTE and GATE)

   midi_note[0]   for the first MIDI channel
   midi_note[1]   for the second MIDI channel
   ...
   midi_note[15]  for the 16th MIDI channel


   Union usage:
     midi_note[x].NOTE returns the note number (0-127), 
     midi_note[x].GATE returns the gate state (0 or 1)


   unsigned char midi_cc_chn0[128]   for CC values received on MIDI channel #1

   Examples:
     midi_cc_chn0[1]   returns the ModWheel Value
     midi_cc_chn0[7]   returns the Volume
     midi_cc_chn0[10]  returns the Pan position
     midi_cc_chn0[11]  returns the expression value


   unsigned int midi_pitch_bender[16]    for Pitch Bender values


-------------------------------------------------------------------------------
(2) lfo.c

By default two digital LFOs are provided, the selectable rate goes
from 0.016 Hz to 97.4 Hz (see lfo_table.inc) - however, above 20 Hz
the generated waveform doesn't look so nice anymore on the scope due 
to quantisation effects (on the other hands: some people drool for 
such effects ;-)

The LFOs generate a sawtooth waveform, which can be easily transformed
into other waveforms by using a waveform table (map.c contains an
example, how to convert this saw into a sinewave)

The LFOs are clocked from the USER_Timer() process each 1 mS (see
timer configuration in main.c)

Following global variables are available:
   unsigned char lfo0_rate,  lfo1_rate    the LFO rate from 0..255
   unsigned int  lfo0_value, lfo1_value   the LFO waveform from 0..65535 (16 bit)

-------------------------------------------------------------------------------
(3) eg.c

By default one digital ADSR envelope generator is provided, the selectable
attack/decay/release rate goes from 1 mS to 2731 mS (see eg_table.inc)

The EGs is clocked from the USER_Timer() process each 1 mS (see
timer configuration in main.c)

Following global variables are available:

   eg_state_t    eg0          provides the gate: eg0.GATE = 1 to set, = 0 to clear
   unsigned int  eg0_value    the EG value from 0..65535 (16bit)
   unsigned char eg0_attack, eg0_decay, eg0_sustain, eg0_release  the ADSR values from 0..255

-------------------------------------------------------------------------------
(4) map.c

Thats the central file where incoming and internal values are
routed to outgoing values (aout_value[0..7], ...)

Please read the file to get some inspirations

Note that sometimes values need to be converted depending on the
resolution. For better readability, some macros have been defined
which can be found in map.h

Example: AOUT value is always a 16bit value, MIOS_AIN_PinGet(0) delivers
a 10-bit value. In order to convert this to a 16bit value, just use
the CONV_10BIT_TO_16BIT macro:

   AOUT_Pin16bitSet(0, CONV_10BIT_TO_16BIT(MIOS_AIN_PinGet(0));

so that the most significant bit of the 10bit value is aligned to the
most significant bit of the 16bit value (in fact the value is
multiplicated by 1024, resp. it is leftshifted 6 times)


Additional notes to digital inputs and outputs: if DINX4 and/or DOUTX4
modules are connected, the number of shift registers have to be
defined in main.c, function USER_Init() in the following way:
   MIOS_SRIO_NumberSet(4); // for 4 shift registers

Thereafter you can get digital values with:
   MIOS_DIN_PinGet(<pin-number>)
and set values with:
   MIOS_DOUT_PinSet(<pin-number>, <value>)


Additional notes to module extensions: MIOS provides some hooks which
could be useful for some special jobs, e.g. MIDI clock processing.
This example demonstrates, how to add a MIDI clock handler:

a) open main.c, search for the MPROC_NotifyReceivedByte function and
add following lines:

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void MPROC_NotifyReceivedByte(unsigned char byte) __wparam
{
   if( byte == 0xf8 ) // MIDI clock has been received
     MAP_MIDIClock(); // call MAP_MIDIClock to notify this
}
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


b) open map.h, search for "prototypes", and add the function
declaration here:

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void MAP_MIDIClock(void);
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


c) open map.c, go to the bottom and add following lines:

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void MAP_MIDIClock(void)
{
  // do anything here
}
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


-------------------------------------------------------------------------------
(5) modules/aout

-> see modules/aout/README.txt

===============================================================================
