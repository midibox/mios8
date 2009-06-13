/*
===============================================================================
ACSensorizer                                                            v0.5.x
===============================================================================

Copyright (C) 2006 Michael Markert http://www.audiocommander.de

Released under GNU General Public License
http://www.gnu.org/licenses/gpl.html
 
This program is free software; 
you can redistribute it and/or modify it under the terms of the GNU General 
Public License as published by the Free Software Foundation

YOU ARE ALLOWED TO COPY AND CHANGE 
BUT YOU MUST RELEASE THE SOURCE TOO (UNDER GNU GPL) IF YOU RELEASE YOUR PRODUCT 
YOU ARE NOT ALLOWED TO USE IT WITHIN PROPRIETARY CLOSED-SOURCE PROJECTS

PLEASE NOTE THAT MIOS IS COPYRIGHTED BY THORSTEN KLOSE (tk@midibox.org) 
AND IS LICENSED TO PERSONAL USE ONLY. 
COMMERCIAL USE OF MIOS AND MBHP (MIOS BASED HARDWARE PLATFORM) IS FORBIDDEN!

PLEASE NAME THE AUTHOR!
Please send your improvements or add-ons if you extend this application!
===============================================================================

A precompiled binary is already part of this package:
   o ACSensorizer.hex (can be loaded into MIOS Studio)
   o ACSensorizer.syx (can be loaded into any SysEx upload tool)
 
The following tools & helpers are part of this package:
   o ACSensorizer Remote (Remote Controller for Mac OS X 10.4.)
     with this program you don't need no HUI (simulate encoders & buttons by midi)
   o Mini Audicle Configuration Script Examples
     you need Mini Audicle installed to send NRPN messages to ACSensorizer

Following tools are required to recompile the code:
   o SDCC v2.5.0
   o gputils
   o perl
   
An Xcode project for OSX is part of this package.
I recommend using the free Code::Blocks IDE for Windows.

The details are described under http://www.ucapps.de/mios_c.html
Also see http://www.midibox.org

===============================================================================

Required hardware:
   o one MBHP_CORE module
   o one MBHP_LCD module (2x16)
   o one MBHP_DIN module for
        o 4 Encoders (no buttons):
        o 4 pushButtons (LOAD/SAVE, PANIC, PEDAL)
        o 1 external Pedal

Sensors:
   o up to 8 (default: 4) sensors connected to unmuxed AIN (J5 of MBHP_CORE)

===============================================================================

ACSensorizer sensorizes up to 8 sensors and interpolates its AIN-readings.
The main target of this application are sensoric devices delivering not exactly 
0 - 5 V, like pressure-, distance-, resistor-based sensors or softPots.



Main Features:

- supports up to 8 sensors
- enable/disable single AINs
- assignable CH and Controller-Number or send Note-ONs
- harmonizer with 20 scales (minor, major, blues, spanish ...)
- syncronizer: master/slave mode (autodetect clock input switches to slave) and selectable BPM
- quantized events: 1/2/3/4/6/8/12/16/24/32/48, selectable per sensor
- slowdown: slows down the signal and increases the gaps between generated values
- adjustable input range by sense-min and sense-max (10bit values, ignore if not matched)
- sense-factor: used for signal interpolation.
                uses fast bitshifting or complex division depending on value
- AUTO-sense feature: auto-calibration of sensor, detect MIN/MAX by sensing
- adjustable output range with "scale from and scale to"
- invert signal
- pedal modes:  filter => only forward if pedal down
                panic  => send panic on release pedal
                combination of all pedal mode options are possible
- detect release: send 0-value if signal drops below sense-min
- bankstick support: 1 connected bankstick provides 2 banks with 127 patches each
- midi configurable: full configuration possible with NRPN-messages 
                     NRPN-MSB CC99 for sensorSelect / sysEx mode
                     NRPN-LSB CC98 for controlType
                     DataEntry MSB CC6 and LSB CC38 for controlValue
- Mac OS Onscreen Config Program, Patch Viewer and MiniAudicle Setup Script examples included!
- HUI-Input to control all sensors with just 4 switch-encoders, 
    pedal, store- & panic-button
- LCD (2x16) with clearly structured menu for sensor-select, prg-select, sensor-config & -settings
- ACSim Console Debugger: code integrated and ready to use configured for XCode2
    select "ACSim" as target and test the application via command-line
    inspect variables with a (graphical) debugger (GDB support within XCode2)
    visit http://www.midibox.org -> there's a tutorial how to use Code::Blocks



The application can be recompiled with a variety of strictly separated #define-
options. For example setting SENSORIZER_INTERFACE_HUI to 0 compiles the application 
without hardware input controls and therefore reduces the file- and application space.
Compiling without HUI, BANKSTICK and LCD generates code with approx. 3 or 4 pages;
compiling with all options will result in an application file of approx. 18 pages.


ACSensorizer can be controlled with 4 Encoders & 4 buttons:


                                (missing...)
                                      Table A: Encoder Setup

  

an on-screen LCD-menu is provided:

        | Menu          | Description                                           |
        |---------------|-------------------------------------------------------|
        | BNK|PRG       | Bank (2 per Bankstick)        | Program (Patch Num)   |
        | PATCH         | Patch Name (7 chars)                                  |
        |---------------|-------------------------------------------------------|
        | ENA|VAL       | Sensor Enabled                | Value (Turn to send)  |
        | CH |CC        | Channel                       | CC 1..127 or Notes 0  |
        |---------------| ------------------------------|-----------------------|
        | Q  |BPM       | Quantized to x.               | Beats Per Minute      |
        | BAS|SCL       | Base Note (C to H)            | Scale                 |
        |---------------| ------------------------------|-----------------------|
        | PED|RLD       | PedalMode (see below)         | Release Detect        |
        | INV|AUT       | Invert Signal                 | AutoSense Mode        |
        | MIN|MAX       | Sense Minimum Value           | Sense Maximum Value   |
        | F  |SLW       | Sense Factor                  | Slowdown Factor       |
        | FRM|TO        | Interpolate From              | Interpolate To        |
        |---------------|-------------------------------------------------------|
                                                               Table B: LCD-Menu

Note, that the Harmonizer-Menu section (Q|BPM & BAS|SCL) is only harmonizing if 
a value of '0' (zero) has been selected as CC# (shown as "NTE" = Note-On).
Because CC# 0 is a BankSelect Controller Message, I found it very unlikely that 
anyone might wish to send this controller in a sensorized fashion. This decision
helped saving a few bytes and space is valueable on a PIC ;)

        
        
    ............................................................................
    function : sizeof: scaled t           : description
    ............................................................................
    CH       :  7bit : 4bit   { 1..16   } : Midi-Channel of sensor
    CC       :  7bit :        { 0..127  } : Midi-Controller Number (1..127)
                                          :  or 0 for NOTES*
    slowdown :  7bit :        { 0..127  } : drops AIN_Notifications
    value    :  7bit :        { 0..127  } : sends value based on last value
    senseMin :  7bit : 10bit  { 0..1023 } : sensor min value (eg. 0080), threshold
    senseMax :  7bit : 10bit  { 0..1023 } : sensor max value (eg. 0800), cutoff
    scaleFrom:  7bit :        { 0..127  } : send values in range (FROM) .. (TO)
    scaleTo  :  7bit :        { 0..127  } : send values in range (FROM) .. (TO)
    ............................................................................
                                         Table C1: sensor configuration controls

    ............................................................................
    function : sizeof: scaled t           : description
    ............................................................................
    BNK      :  7bit : 4bit   { 0..14   } : Bank number
    PRG      :  7bit : --     { 0..127  } : Patch number
    PATCH    :  7 x 8bit      { --      } : Patch name ("1234567")
    Q        :  7bit : 5bit   { 0..31   } : Quantized to Note Value (start)
    BPM      :  8bit : --     { 0..255  } : Beats per Minute (1..255)
    BAS      :  7bit : 4bit   { 0..11   } : Harmony Base Note (C) .. (H)
    SCL      :  7bit : --     { --      } : Harmony Scale (eg MAJ or min)
    ............................................................................
                          Table C1: patch, harmony & sync configuration controls*

    
    ON/OFF           : switches sensor AIN-line ON or OFF
    AUTOSENSE_MIN    : store last AIN_10bit_value as senseMin (find minimum)
    AUTOSENSE_MAX    : store last AIN_10bit_value as senseMax (find maximum)
    PEDAL_MODE       : FILTER (only sense when pressed)
                       HOLD (also send CC64, Damper)
                       PANIC (send PANIC on release for setted sensor-CH)
                       and combinations of all above
    RELEASE_DETECT   : sends a value of '0' if sensed below SENSE_MIN
    SENSE_FACTOR     : sFactor = (sMax-sMin) / 127;
                       7bitValue = sensedValue * sFactor
                       => uses fast bitshifting if factor is 1,2,4,8,16
   *HARMONY          : The Harmony Menu affects sensor only when CC is set to 0!
    BASE_NOTE        : The Base Note used for harmonic calculations, eg. C# (0..11)
    SCALE            : Notes get shifted to next possible note in selected scale
                       Adding additional scales is easy: just increase SCALE_MAX
                       and add more scales to the scale_table.
                                           

===============================================================================


MIDI Implementation Table

---------------------------------------------------------------
Message Type		 Received / Sent	Notes
---------------------------------------------------------------
NOTE ON						x | x		Harmonized
NOTE OFF					x | x		Harmonized
POLY AFTERTOUCH				- | -
CC							x | x		See CC Message Table
PROGRAM CHANGE				G | G		Global Channel only
CHANNEL AFTERTOUCH			- | -
PITCH WHEEL					- | -
---------------------------------------------------------------
CC Message Type		 Received | Sent
---------------------------------------------------------------
CC#0 to CC#127				  | x		Received: see below
 0: Bank Select				G | G		Global Channel only
 6: Data MSB				x | x		See NRPN Table F
38: Data LSB				x | x		See NRPN Table F
64: Pedal					x | x		Pedal
66: Sostenuto				x | x		Pedal
67: SoftPedal				x | x		Pedal
80: Harmony Base			x | x		Harmony (ACMidiProtocol)
81: Harmony Scale			x | x		0..20
82: Harmony Base Listen		x | x		<> 63
83: Harmony Scale Prev		x | x		 > 63
84: Harmony Scale Next		x | x		 > 63
85: Harmony Random			x | x		 > 63
87: Quantize BPM			x | x		BPM = [0..127] + 60
89: Quantize Toggle Next	- | x		 > 63 Next Q for Sensor (CH)
90: Quantize Set			x | x		Set quantize for Sensor (CH)
98: NRPN LSB				x |	x		See NRPN Table E
99: NRPN MSB				x | x		See NRPN Table E
---------------------------------------------------------------
							 Table D: MIDI Implementation Chart

Legend:
x	YES
-	NO
G	Global Channel only

the channel numbers that are counted from 1 to 16 appear as 0 to 15 in code!
For more information on ACMidiProtocol supporting interchangeable notifications
on BPM, Harmony and Scale changes, see ACMidiProtocol.h!


===============================================================================

ACSensorizer can also be configured by NRPN (Non-Registered Parameter Numbers)
settings are only handled if received on global midi-ch

-> use NRPN MSB (CC99, 0x63) to select the appropriate sensor (default 0x0 to 0x3)
-> use NRPN LSB (CC98, 0x62) to select the appropriate controlType    (see table E)
-> set values with DataMSB    (CC6, 0x06) and    DataLSB    (CC38, 0x26)    (see table F)

NOTE: settings values by DataIncrement (CC96, 0x60) and DataDecrement (CC97, 0x61)
      is not supported due to codesize!

-> there is a chuck script for miniAudicle contained in this release, that makes
   sending NRPNs quite easy. just type the values and send them to m5 sensorizer
   to download miniAudicle (Linux/Mac/Win) go to http://audicle.cs.princeton.edu/mini/
   
-> there is also a Mac OS X universal binary included to simulate encoder movements
   this way it's possible to easily control the Sensorizer without HUI!
   note that SENSORIZER_INTERFACE_HUI must not be disabled to use this functionality
   

   
    | MSB    99     | LSB    98     |
    |===============|===============|
    | SensorNum     | Ctrl Param    |
    | 0x0 - 0x7     | 0 - 127       |
    |===============|===============|
    | 0x60          | 0 - 8         | Select Sensor Wheel
    | 0x61          | 0 - 10        | Select Menu Wheel
    | 0x62          | 0 - 127       | Data Wheel Left
    | 0x63          | 0 - 127       | Data Wheel Right
    |===============|===============|
    | ON:  126      |               |
    | OFF: 127      |               |
    |===============|===============|
         Table E: NRPN Control Types


    |    NRPN LSB   | Control Params          |    DataEntry                |
    |===============|=======================================================|
    |    0x00       | sensor.enabled          |    0x0 / 0x1                |
    |    0x01       | sensor.pedalMode        |    0x0 - 0x7  *1)           |
    |    0x02       | sensor.autoSense        |    0x0 - 0x2  *2)           |
    |    0x03       | sensor.invert           |    0x0 / 0x1                |
    |    0x04       | sensor.releaseDetect    |    0x0 / 0x1                |
    |===============|=======================================================|
    |    0x10       | slowdown                |    0 - 127                  |
    |    0x11       | sense_min               |    0 - 1023   *3)           |
    |    0x12       | sense_max               |    0 - 1023   *3)           |
    |    0x13       | sense_factor            |    0 - 64     *4)           |
    |===============|=======================================================|
    |    0x21       | scale_from              |    0 - 127                  |
    |    0x22       | scale_to                |    0 - 127                  |
    |===============|=======================================================|
    |    0x70       | CH                      |    0 - 15                   |
    |    0x71       | CC                      |    0 - 127                  |
    |===============|=======================================================|
                                Table F: NRPN Control Types and Data Entries

    *1) sensor.pedalMode:   PEDAL_OFF                   0x0
                            PEDAL_HOLD                  0x1*
                            PEDAL_PANIC                 0x2
                            PEDAL_HOLD_PANIC            0x3*
                            PEDAL_FILTER                0x4
                            PEDAL_FILTER_HOLD           0x5*
                            PEDAL_FILTER_PANIC          0x6
                            PEDAL_FILTER_HOLD_PANIC     0x7*
                            * = not yet supported
    
    *2) sensor.autoSense    AUTOSENSE_OFF               0x0
                            AUTOSENSE_MIN               0x1
                            AUTOSENSE_MAX               0x2
                            
    *3) 10-bit values       use DataMSB + DataLSB
    
    *4) if possible use only the fast bitshifting factors 1,2,4,8,16...
        buffer-overflows or timing issues may occur if complex divisions have
        to be calculated for more than one sensor...



===============================================================================

ACSensorizer supports writing and reading to a connected bankstick.
Each patch consists of 2 pages ˆ 64 bytes => 128 bytes
2 banks with 128 patches each are available per 1 connected bankstick

Choose the appropriate bank by sending a Coarse-Adjust Bankselect (CC#0).
In HUI-Mode, switching a bank on the device also sends the current Bank/PRG.

here's are memory-map of one patch;
all patches are backward-compatible to version 0.2.0
the first table shows patches of Sensorizer until version 0.3.x
the second shows patches from version 0.4.x

    | Data            |    Size (Bytes)    |    Page     |    Address      |
    |======================================================================|
    | Version         |     1 =  1         |    1  0x00  |     0     0x00  |
    | PatchName       |    15 = 16         |    1        |     1     0x01  |
    |   <reserved>    |     8 = 24         |    1        |    16     0x10  |
    | sensor[8]       |     8 = 32         |    1        |    24     0x18  |
    | CH[8]           |     8 = 40         |    1        |    32     0x20  |
    | CC[8]           |     8 = 48         |    1        |    40     0x28  |
    |   <reserved>    |    16 = 64         |    1        |    48     0x30  |
    |-----------------|--------------------|-------------|-----------------|
    | slowdown[8]     |     8 =  8         |    2  0x40  |     0     0x00  |
    | sense_factor[8] |     8 = 16         |    2        |     8     0x08  |
    | sens_min[8].MSB |     8 = 24         |    2        |    16     0x10  |
    | sens_min[8].LSB |     8 = 32         |    2        |    24     0x18  |
    | sens_max[8].MSB |     8 = 40         |    2        |    32     0x20  |
    | sens_max[8].LSB |     8 = 48         |    2        |    40     0x28  |
    | scale_from[8]   |     8 = 56         |    2        |    48     0x30  |
    | scale_to[8]     |     8 = 64         |    2        |    56     0x38  |
    |======================================================================|
                   Table G1: Bankstick Patch Content for Sensorizer < 0.3.9


    | Data            |    Size (Bytes)    |    Page     |    Address      |
    |======================================================================|
    | Version         |     1 =  1         |    1  0x00  |     0     0x00  |
    | PatchName       |     8 =  9         |    1        |     1     0x01  |
    |   <reserved>    |     7 = 16         |    1        |    10     0x0A  |
    | BPM             |     1 = 17         |    1        |    16     0x10  |
    |   <reserved>    |     5 = 22         |    1        |    17     0x11  |
    | harmony_base    |     1 = 23         |    1        |    22     0x16  |
    | harmony_scale   |     1 = 24         |    1        |    23     0x17  |
    | sensor[8]       |     8 = 32         |    1        |    24     0x18  |
    | CH[8]           |     8 = 40         |    1        |    32     0x20  |
    | CC[8]           |     8 = 48         |    1        |    40     0x28  |
    | sync_q[8]       |     8 = 56         |    1        |    48     0x30  |
    |   <reserved>    |     8 = 64         |    1        |    56     0x38  |
    |-----------------|--------------------|-------------|-----------------|
    | slowdown[8]     |     8 =  8         |    2  0x40  |     0     0x00  |
    | sense_factor[8] |     8 = 16         |    2        |     8     0x08  |
    | sens_min[8].MSB |     8 = 24         |    2        |    16     0x10  |
    | sens_min[8].LSB |     8 = 32         |    2        |    24     0x18  |
    | sens_max[8].MSB |     8 = 40         |    2        |    32     0x20  |
    | sens_max[8].LSB |     8 = 48         |    2        |    40     0x28  |
    | scale_from[8]   |     8 = 56         |    2        |    48     0x30  |
    | scale_to[8]     |     8 = 64         |    2        |    56     0x38  |
    |======================================================================|
                   Table G2: Bankstick Patch Content for Sensorizer > 0.4.0

    
patch addresses are patch * 0x80; or PIC-optimized: (unsigned int)patch << 7
in case of uneven bank addresses (1,3,5,7,9,11,13): 16256 is added to the page-address!

example hex-output of patch#0 for Sensorizer < 0.3.9:
**MIOS_BANKSTICK_WritePage at 0x0 
0:     02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
16:    00 00 00 00 00 00 00 00 80 80 80 80 40 08 00 00  ............@...
32:    00 00 00 00 04 04 04 04 14 15 16 17 00 00 00 00  ................
48:    00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
**MIOS_BANKSTICK_WritePage at 0x40 
0:     08 08 08 08 00 00 00 00 04 04 04 04 00 00 00 00  ................
16:    00 00 00 00 7f 00 00 00 40 40 40 40 7f 00 00 00  ........@@@@....
32:    03 03 03 03 00 00 00 00 70 70 70 70 40 40 40 40  ........pppp@@@@
48:    00 00 00 00 00 00 00 00 7f 7f 7f 7f 00 00 00 00  ................

example hex-output of patch#0 for Sensorizer > 0.4.0
**MIOS_BANKSTICK_WritePage at 0x0 
 0:     04 44 65 66 61 75 6c 74 00 ff 00 00 00 00 00 00  .Default........
16:     78 00 00 00 00 00 00 14 81 81 81 81 00 00 00 00  x...............
32:     00 00 00 00 00 00 00 00 14 15 16 17 18 19 1a 1b  ................
48:     08 08 08 08 08 08 08 08 00 00 00 00 00 00 00 00  ................
**MIOS_BANKSTICK_WritePage at 0x40 
 0:     02 02 02 02 02 02 02 02 04 04 04 04 04 04 04 04  ................
16:     00 00 00 00 00 00 00 00 40 40 40 40 40 40 40 40  ........@@@@@@@@
32:     03 03 03 03 03 03 03 03 70 70 70 70 70 70 70 70  ........pppppppp
48:     00 00 00 00 00 00 00 00 7f 7f 7f 7f 7f 7f 7f 7f  ................

finally, a reference to the last active patch (bank/prg) is stored in EEPROM
to enable reloading on next startup. each time a patch is successfully loaded
or stored the reference is automatically saved. 
the available EEPROM adress range is from 0xF00000 to 0xF000FF (256 bytes):


    | EEPROM Address       | Data              | Size (bytes)              |
    |======================================================================|
    | 0x00 - 0x01          | last bank         | 1 byte                    |
    | 0x01 - 0x02          | last patch        | 1 byte                    |
    | 0x02 - 0xFF          |  <reserved>       | 254 bytes                 |
    |======================================================================|
                                                    Table H: EEPROM Content


===============================================================================

Version History:
 * v0.1.0       2005-12         First testing versions, no code reused
 * v0.1.1       2006-01         AIN-config, gate, expander, main functionalities
 * v0.1.2       2006-01         Improved expander, inverter, smaller filesize
 * v0.1.3       2006-04         Cleanup, code-splitting, rewrite (many fixes!)
 * v0.1.4       2006-04         Improved inverter, expander, signal routing
 * v0.1.5       2006-04         Added Bankstick Accessors (16byte / sensor)
 * v0.1.6       2006-06         Rewrote Bankstick related functions, optimized vars
 * v0.2.0       2006-07         Rewritten from scratch, optimized var-access & efficiency
 *                              added ENC and LCD menu (2x16/4x20 optimized) support (in main.c)
 * v0.2.1       2006-08         Rewrote Bankstick Support (in main.c), 2x64 bytes per patch
 *                              patches to v0.2.0 aren't backward compatible anymore
 * v0.2.2       2006-08         improved algorithms, added sensemode (efficient or exact)
 *                              added pedalMode, releaseDetect config and PRG-CH
 *                              initial release
 * v0.2.3       2006-09         minor bugfixing
 *                              removed MidiTrough and used MIOS_Merger
 *                              two example miniAudicle/Chuck NRPN scripts are now included
 * v0.2.4       2006-09 Ã       fixed severe bank select bug (B,D,F,H,J,L were not accessible)
 * v0.3.0       2006-09         rewritten main class, better HUI concept
 *                              re-organized Encoder Input
 *                              autoload patch #0 (in case of reset)
 *                              added pedal to start/stop AutoSense Mode
 *                              corrected sensor-level-view for sensors 4..8
 * v0.3.1       2006-10         added basic clock-sync support / Continue-Hack for m4
 *                              added clock-forwarding, even if mios_merger disabled
 * v0.3.2       2006-11         fixed PRG-CH bug
 * v0.4.0       2006-12         added harmonizer, prepared for syncronizer
 *                              patches maintain full backward-compatibility to v0.2.1 versions
 * v0.4.1       2006-12         added HUI Midi Remote Control (Simulate Encoder Movements)
 * v0.4.2       2007-04         implemented syncronizer from kII, overworked to support
 *                              quantized event triggering (AIN readings)
 *                              overall code cleanup, namespace improvements
 *                              implemented ACMidiProtocol to enable inter-application messaging
 * v0.4.3       2007-05         added bpm control (48..255 bpm), patches save harmonies & sync
 *								fixed broken master/slave detection
 *                              CCs Sustain, Sustenuto & Soft Pedal (Damper) may also control the Pedal
 *                              Patch Names are now supported (7 chars max)
 *								Last active patch is stored in EEPROM and loaded on startup
 * v0.4.4		2007-07	Ã		ENC_Speed optimized for Voti.nl encoders
 *								Sync Mode Sensing Bugfix (incl. Autosense)
 *								PRG_CH broadcasting as #define option on read/write
 *								Documentation cleanup & updates
 * v0.4.5		2007-08	Ã		sync start signal now recognized (SLAVE & MASTER), 
 *								continue is sent each bar (MASTER), start on patch load (MASTER)
 *								harmonizer now working correctly with base notes
 *								ACMidiProtocol base notes now recognized correctly
 * v0.4.6		2007-11	Ã		fixed MST/SLV autodetection bug, now behaves correctly
 *								PANIC also sends STOP in MASTER mode (LOAD sends START)
 *								Global Channel now defaults to CH16, messages are sent/received
 *								on all channels except for PRG_CH (Global CH only)
 * v0.4.7		2007-12			Added DOUT support for external Sensors with LEDs
 *								now recognizes QUANTIZE_SET as well (ACMidiProtocol)
 *								fixed QUANTIZE_BPM bug
 *								now doesn't send unchanged values anymore
 * v0.4.8		2008-01			Added DIN support for remaining 15 DIN-Inputs (optional)
 * v0.4.9		2008-02			LCD Messages now are stopped on Encoder Movements
 *				2008-03			Bugfix for Xtended DIN (grey model), 
 *								there's a bug on Mikes DIN-module (see description below)
 *								added #define model configurations SENSORIZER_MODEL_ID
 * v0.5.0		2009-02			Applied compliance to new MIOS_BASE dev platform
 *				2009-05			Bugfix: now all 16 Banks (=8 Banksticks can be addressed)
 *								(previously only 15 working)
 *				2009-06	Ã		pre-release via midibox svn (beta)
 *                              
 *                      Ã       release version
 
===============================================================================

done (check):
- <nothing to check atm>

bugs:
- <no software bugs known atm>
- there's a hardware bug in my first mini model (grey) and the Extended Input selection keypad, probably to do with Mikes DIN PCB (no problem with SMASH's versions so far). The two DIN pins #24, #25 (#8,#9) are jittering, so I disabled them in code and instead took the two free pins #29, #30 (=> #13, #14). With the newly added compilation options SENSORIZER_MODEL_ID, this additional bugfix code is only added for SENSORIZER_MODEL_ID_ONE

2do:
- Fix ENC NOT WORKING for Model1 (grey) => 0.4.9. working perfect ??? maybe because of the new platform compliance?
- extend DOUT 
	Instant visual feedback by patterns 
	- Blink one after each other @1/8 rythms (=> 8 sensors!)
	- ON while sensing signals
- sysEx support: sends sysEx dump of current/all patch(es)
- add NRPN control for sync_q, sync_l, harmonizer_base, harmonizer_scale?

=============================================================================*/
