FRAM Module Test application
===============================================================================
Copyright (C) 2009 Matthias Mächler (maechler@mm-computing.ch / thismaechler@gmx.ch)
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
   o one FRAM module
   o one LCD display

Optional hardware:
   o one DIN module with six buttons (pins 0 - 5)

===============================================================================

The application uses the FRAM-module driver to test 1 - 32 connected 
FRAM-devices (Ramtron FM24C64 / FM24C256 / FM24C512). One FM24C512 acts like
two FM24C256, therefor only max. 16 of these chips can be connected.
To connect more than 8(4) devices, you need to enable multiplexing for the FRAM
driver, and the device's SDA/SCL lines must connected to a analog multiplexer.

Configuration
--------------
Be sure the correct address range and device count is given accoring to your 
device. See defines and comments at the top of main.c

If you don't change anything, the default for FRAM-moudule will be selected
(J10 RC4: SCL, RC5: SDA, multiplexing disabled), in main.c 8 devices with
and address range of 0x7FFF are configured. This equals 4 x FM24C512 or
8 x FM24C256.

If you use multiplexed devices, enable the define in the Makefile. All possible
FRAM_DEFINES that you can change are prepared but commented out.

If you use multiplexing:
  FRAM_DEFINES += -DFRAM_MULTIPLEX_ENABLE=1

If you want to connect your FRAM devices to the standard MIOS IIC port J4:
  FRAM_DEFINES += -DFRAM_MIOS_IIC=1
  
If you notice unstable communication to the device (very long connection cable,
bad slew rates etc.), increase this value step by step and check if the problem
disappears:
  FRAM_DEFINES += -DFRAM_IIC_SLOWDOWN=1

For further information about the pin assignments, default values and defines,
refer the README.txt of the FRAM-module.



Test phases
--------------
The application runs 8 test-phases in series. Each run tests the whole address
range of each device. Test byte values will be generated in the following way:
LSB(address+device_addr+test_phase_offset). This ensures that each device and
each test phase has it's own test value offsets, to ensure that device and
address will be selected correctly, and no values will be "inherit" from one
phase to another. These are the phases:

1. Test buffer write/read/compare:
----
  Buffers of 256 bytes will be written and immediately read after writing. This
  test run uses FRAM high level functions. The read data will be compared to 
  the test values written before.
  
2. Test single byte write/read/compare:
----
  Single bytes will be written and immediately read after writing. The read
  byte will be compared to the one that was written before. FRAM high level 
  functions will be used for this test run.
  
3. Test subsequent buffer write:
----
  Performs subsequent buffer write using FRAM low-level functions. In each 
  session, 32 x 256byte buffers will be written, the whole device- and 
  address range will be covered.
  
4. Test subsequent buffer read/compare:
----
  Performs subsequent buffer read using FRAM low-level functions. In each 
  session, 32 x 256byte buffers will be read, the whole device- and 
  address range will be covered. The values read should be the ones that were written
  in the last phase. They will be compared to the originally written values.
  In this test run, problems with the multiplexer or the device addressing 
  show up. This will *not* happen in test-run 2, because the data is read 
  immediately after writing. The compare will not fail because the same wrong
  device is selected as with the write. In subsequent write, the whole device-
  and address range will be written in one run, and each 256bytes sector has 
  its own test value start offset. Wrong device selection will cause overwriting
  of data that will show up as error in this test run.
  If you want to analyze the problem in deep, set the test_value_deviceaddr_only
  define to 1. This will cause the program to write the device address to each
  byte of the given device. On error abort, you see the value that was read
  and the value it should equal to, this allows to analyze on which stage  
  the chip selection fails (chip address selectors / multiplexer).
  
5. Test subsequent single byte write:
----
  Performs subsequent byte write using FRAM low-level functions. In each 
  session, 32 x 256bytes will be written, the whole device- and address range 
  will be covered.
  
6. Test subsequent byte read/compare:
----
  Performs subsequent byte read using FRAM low-level functions. In each 
  session, 32 x 256bytes will be read, the whole device- and address range 
  will be covered. The values read should be the ones that were written
  in the last phase. They will be compared to the originally written values.
  
7. Speed test subsequent buffer write:
----
  Performs subsequent buffer write using FRAM low-level functions. In each 
  session, 32 x 256byte buffers will be written, the whole device- and 
  address range will be covered. The buffer will not be initialized with
  values, it doesn't matter what data will be written, instead the time needed
  for the whole run will be measured and shows up at the end in case of
  success.
  
8. Speed test subsequent buffer read:
----
  Performs subsequent buffer read using FRAM low-level functions. In each 
  session, 32 x 256byte buffers will be read, the whole device- and 
  address range will be covered. No data compare will take place, instead
  the time needed for the whole run will be measured and shows up at the 
  test summary screen after finishing.
  
  
If all the test runs finished successfully, you will see a (alternating) summary 
screen that shows you following information
* Success message
* Device count / address range
* Speed test results (in mS, time needed for the whole device/address range)

In case of a error-abort, you also see alternating screens which show you
* In which phase and at which operation the error occured. Possible operations
  are: Begin Read, Begin Write, Write, Read/Compare.
* Device- and memory-address at which the error occured
* [FRAM_ERROR code and FRAM_REG value] or [Read byte value / Should-be value]



Test phase triggering with buttons
------------------------------------
If you have a DIN module connected, you can trigger test phases with pin 0 (e.g.
button push, pin to ground).

pin 0: phase 1(buffer wr/rd/cp)
pin 1: phase 2(byte wr/rd/cp)
pin 2: phase 3(subseq. buffer wr)
pin 3: phase 5(subseq. byte wr)
pin 4: phase 7(speed test buffer wr)
pin 5: phase 8(speed test buffer rd)

The following phases will be performed continous. 
Note that it would make no sense to trigger the phases 4 and 6, because they 
read and compare the values written in phases 3/5 before.


===============================================================================
