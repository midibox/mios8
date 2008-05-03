
ain64_din128_dout128 V2.1
===============================================================================

This application demonstrates, how to configure MIOS in order to handle with
up to 64 pots, 128 buttons and 128 LEDs

It can be used as a template for own applications

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

Optional hardware:
   o up to 4 DINX4 modules for up to 128 digital inputs
   o up to 4 DOUTX4 modules for up to 128 digital outputs
   o up to 2 AINX4 modules for up to 64 pots
   o 2x16 LCD

===============================================================================

Configuration steps:

   o check the "general application settings" in main.h if changes are
     required for your hardware setup (e.g., if no AINX4 multiplexers
     are connected to the core)

   o the application can be rebuilt by typing "make" in the command shell

   o Hint: unused analog inputs must be tied to ground, otherwise
     the core will transmit a lot of random CC events

     See also http://www.ucapps.de/mbhp/auaimbctg.pdf

===============================================================================
