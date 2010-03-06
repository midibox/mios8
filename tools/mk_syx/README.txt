$Id$

mk_syx.pl                                                         tk@midibox.org
--------------------------------------------------------------------------------

The perl script mk_syx.pl converts a MIDIbox .ini file into a 
.syx (SysEx) file which can be transfered to the MIDIbox via MIOS Studio 2
or any other SysEx Loader

Note that this script version works only for the PIC18F based variant of 
MIDIbox64, MIDIbox64E and MIDIbox MF

--------------------------------------------------------------------------------


Step by Step
~~~~~~~~~~~~

   -> customize the .ini file for your needs
   -> start the script (e.g. type "make" in command shell, this will execute the Makefile)
   -> transfer the .syx file to your MIDIbox

If the SysEx Device ID of your MIDIbox is != 0, the mk_midio128_syx.pl script has to be
executed with "-device_id <id>" argument, e.g. "-device_id 4" for device ID 4
(if you are using the Makefile, change the parameter there)


How to start the script?
~~~~~~~~~~~~~~~~~~~~~~~~

   o Linux/MacOS/etc...
     Perl comes with most Posix systems like Linux, MacOS X, BSD, Solaris, ... - 
     if you own such an OS, just start the script with (example)

        ./mk_syx.pl midibox64.ini
        -> the resulting syx file is named "midibox64.syx"        

     or type "make" to convert all *.ini files at once


   o DOS/Windows Users should install ActivePerl, which is available at the 
     following site:

        http://aspn.activestate.com/ASPN/Downloads/ActivePerl/index/

     Get the .msi package and follow the installation instructions 
     (Win98 users have to install the Microsoft Windows Install 1.1+, 
     too)

     To start the script just open a command window and type (i.e.)

        perl mk_syx.pl midibox64.ini
        -> the resulting syx file is named "midibox64.syx"

     If a Posix environment is installed on your windows PC (see also
     http://www.midibox.org/dokuwiki/application_development) you, can 
     type "make" in the DOS shell to execute the Makefile



How to transfer the .syx file?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

   - Start MIOS Studio 2
   - select the MIDI In and Out ports
   - start the SysEx tool with Tools->SysEx Tool
   - ensure that the delay between sysex blocks is set to 750 mS
   - load the .syx file
   - press the SEND button

   See http://www.uCApps.de/howto_tools_mbsyx.html for more detailed instructions
