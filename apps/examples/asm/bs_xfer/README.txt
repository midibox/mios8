
BankStick Transfer Example

This example demonstrates step by step:
   1) how to define the content of a BankStick with the PIC assembler
   2) how to assemble the data dump
   3) how to convert the data to .syx format
   4) how to transfer the data to a BankStick via MIDI
   5) how to read back the data
   6) how to convert the returned data to a assembler file for editing

==========================================================================

Copyright (C) 2003  Thorsten Klose (tk@midibox.org)

==========================================================================
     
Step 1) how to define the content of a BankStick with the PIC assembler
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     
      ;; "EEPROM16" doesn't work here due to the automatic wraparound at 0x10000
      ;; -> the BankStick memory must be located at 0x410000-0x41ffff)
      LIST	P=PIC18F452, R=DEC
This main.asm file contains a LIST statement with P=PIC18F452 (EEPROM16
doesn't work here due to the automatic wraparound at 0x10000), so that
the assembler creates a .hex file with up to 64k data.
No instruction can be inserted into the data region, but only 
assembler directives like "fill", "db" and "dw"
-> see "bankstick_content.inc" which is included from this main.asm file

     
Step 2) how to assemble the data dump
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     
   o Open the main.asm file, here you can find all available include files 
   o Change the files for your needs 
   o enter "make" in the DOS/Unix command shell
     (see also http://www.midibox.org/dokuwiki/application_development )
   o After that, a new main.hex file can be found in the directory


Step 3) how to transfer the data to a BankStick via MIDI
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

   o can be done from the upload window of MIOS Studio

   NOTE: Only the second-level bootloader of MIOS allows to upload data
   to a BankStick. The first-level bootloader, which is active for the
   first 2 seconds after power-on will refuse the packages


Step 4) how to read back the data
        ~~~~~~~~~~~~~~~~~~~~~~~~~

  open the SysEx tool of MIDI Ox and write following SysEx strings 
  into the Command Window:
    -> 32k BankStick (24LC256):
       F0 00 00 7E 40 <device-id> <bs-number>1 40 00 20 00 F7
    -> 64k BankStick (24LC512):
       F0 00 00 7E 40 <device-id> <bs-number>1 40 00 40 00 F7

       (<device-id> == 00 to 7F)
       (<bs-number> == 0 to 7)

  Example: loading 32k from BankStick 7
    --> F0 00 00 7E 40 00 71 40 00 20 00 F7

  Click on "Command Window->Send/Receive SysEx" command

  you will receive ca. 37900 bytes

  NOTE: if you receive a 'F0 00 00 7E 40 00 0E 0A .. F7' error message during
  the download, the selected BankStick is not connected to your MIOS core.


Step 5) how to convert the returned data to a assembler file for editing
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  write the received SysEx blocks into a file and use the bin/syx2asm.pl 
  script which comes with this package

  Example: perl bin/syx2asm.pl saved_dump.syx converted_dump.asm

  This file can be edited, assembled, converted and uploaded again to the
  MIOS core.

--- Have fun! ---
