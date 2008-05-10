$Id$

DIN Velocity UnMuxed V1.0              (C) 2005 Thorsten Klose (tk@midibox.org)
===============================================================================

This application can be used to do some experiments with "velocity buttons"

The buttons must provide three contacts: one contact, which is closed with
the "middle pin" when the button is pressed, and another contact, 
which is closed with the "middle pin" when the button is depressed. 
Such buttons are mostly called "On-(On) pushbuttons", "push-push 
momentary buttons" or "SPDT buttons (Single Push, Double Throw). 

The outer contacts have to be connected to a pair of two digital 
input pins of a DINX4 module, the middle contact to ground. 
See also http://www.ucapps.de/mios/din_velocity_unmuxed.pdf

Note that this application is not able to scan a keyboard matrix, but
it could be useful to realize some simple drumpads

Velocity measuring works in the following way: so long the "depressed"
contact is closed, a velocity counter is preloaded with 127
Once it is released, this counter will be decremented each millisecond
until the "pressed" contact is closed (see din_vel.c, DIN_VEL_Tick())

The measured delay will be scaled to the velocity value 
(see DIN_VEL_NotifyToggle())

The scaling can be changed in din_vel.c, velocity_table[]
Currently this table contains an exponential scaling which has been
generated with utils\velocity_table.pl

Depending on the button you are using a modification of this table
might lead to better results.

By default 16 buttons are handled by the application, this number can
be increased to up to 64 in din_vel.h

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
   o at least one DINX4 module for 16 push-push buttons

Optional hardware:
   o up to 4 DINX4 modules for up to 64 push-push buttons
   o a 2x16 LCD

===============================================================================

Configuration steps:

   o check the "general application settings" in main.h if changes are
     required for your hardware setup (e.g., number of shift registers)

   o check the "DIN velocity settings" in din_vel.h 

   o the application can be rebuilt with the "make.bat" file
     (type "make" in a DOS command shell)

===============================================================================

Description about the most important files:

   - main.c: the main program with all MIOS hooks

   - din_vel.c: the velocity handler

   - utils\velocity_table.pl: has been used to generate the scaling table

There are additional .h files for all .c files which contain
general definitions and the declaration of global functions/variables
These .h files must be included into the program parts which
get use of these globals

===============================================================================
