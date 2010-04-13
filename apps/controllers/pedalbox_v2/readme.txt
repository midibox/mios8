 ..........               .....         ......   .......... ..                                  ....
 .Z7ZZZZZZZ:.             .ZZ=..        .,Z=:.   .ZZZZZZZZZ~..                                  Z=~.
 $ZI7+~,.ZZI, .....     ...ZZ=,  .........Z=:.   .ZZ=,..ZZ=~..  ......   ....... .........   ...Z=~.
 .Z+.=ZZZ7?,.ZZ=,ZZ:. .ZZZZZZ=..ZZ7~:OZ~..Z=:.   .ZZZZZ$?7OZ,,.IZZ+~ZZ,..ZZ=~,ZZ:.ZZZZZZ~,..ZZZZZ=~.
 .Z7?+?+~...ZZZZZZ++.ZZII~.ZZ=...,ZZZZZ=..Z=:.   .ZZI=:,...Z?:ZZ7=,..Z=: .$ZZZZZ=.ZZ7?:+=.ZZ7+:.Z=~.
 .Z?=.     ZZ+~:.IZ:,Z=:..ZZZ=.ZZ+=,?ZZ=..Z=:.   .ZZ:...ZZZ7+.Z7:. .Z$?.ZZ+:,ZZZ~.ZZ=... Z?=..ZZZ=~.
 .ZZ=..    .ZZZZZ?7,.ZZZZ?+ZZ+.ZZZZ$+ZZ?.,Z?~    .ZZZZZ+??=,. .ZZZZZ7+,.ZZZZ?~ZZ=.ZZ+,   ZZZZ$I:ZZ=.
 .:~.      .,~==~, ..,~=~,.,~:.,~=~:.,~, .~:.    .,~~:,.....  .:~=~:. ..:==~,.::,.,~,.   .~=~:..::. 
               ...     .   .. .. ...     . .     .  ...                   . . .      .    . .   .   
           ... ......               ...           .....    ...........                              
           ZZ.$ZZZZ$,.              .Z::          .ZZ~.    IZ7ZZZZZZ,.                              
         ..ZZ???=,=Z+:  ....        .Z~~   .....  .ZZ=.    :Z++~,,ZZ+,    .....  ..    ...          
           ZZ,..ZZZ$=.~ZZZZZ,.  .?ZZZZ~~..ZZZZZZ:,.ZZ=.    :ZZZZZZZZZ.. .7ZZZZO..ZZ.  .ZZ~          
           ZZZZ?II=..Z$:~ZZZI: ZZII=.Z~~..:,:ZZZ=:.ZZ=.    :ZII+=~:.ZZ:.Z77~:.Z$~.,ZZZZ7I,          
           ZZ?~,. ..:Z++=~.Z:.Z$?,..ZZ~:.ZZ+=,OZ=:.ZZ~.    :Z~.  .$Z$7:ZZ+.  ?ZI:..ZZZZ..           
           ZZ=.    ..ZZZZZZI?.ZZZOZ$:ZI,ZZZZZ$IZ=:.ZZ=.    ZZZZZZ$II=..~ZZZZZ$7=.ZZII:.ZZ~.         
          .:+:.     .:=++?+,..,=+?=:,=+::=+?+~:++..~+~    .:+++=~,.    .:=++?~...:?~ ..,++.  
		  
Pedal Box / Pedal Board V2.6
===============================================================================
Copyright (C) 2010  Mick Crozier (mick@durisian.com)
Licensed for personal non-commercial use only.
All other rights reserved.
===============================================================================

The Pedal Box was designed to be a very basic extension to a floorboard.
Giving it an extra 8 Controller Pedals, and a display to see patch change / CC names, as well as their values. 

It has now expanded with options to build a complete MIOS Powered Floorboard. 

You will need the PC Editor software to create the required bankstick files
see more info here: http://www.midibox.org/dokuwiki/doku.php?id=pedal_box

===============================================================================
A precompiled binary is already part of this package:
   o project.hex (can be loaded into MIOS Studio)

Following tools are required to recompile the code:
   o SDCC
   o gputils

The details are described under
   http://www.midibox.org/dokuwiki/application_development

The details are described under http://www.ucapps.de/mios_c.html


===============================================================================

Required hardware:
   o 1 x MBHP_CORE module
   o Large Character 16x2 LCD Display
   o 1 x 10k pot or CV pedals
   o 2 x 24LC256 Bankstick
   0 A MIDI Floorboard / programable HUI. Or 1 x DINX1 Module and 1 button

Optional hardware:
   o 8 x 10k pot or CV pedals
   o 1 x MBHP_DINX4 module + 1 x MBHP_DINX1 module
   o 40 x buttons
   o 2 x MBHP_DOUTX4 module
   o 40 x LEDs
   o 8 x relays
   o 2 x 7 segment LED digits or a 2x20 LCD Display
   
===============================================================================

Included Files:

main.h                
main.c
pbx_config.h           -- Hardware configuration setting
pbx_midi_config.h      -- Midi Implementation for Pedal Box
pbx.h
pbx.c                  -- functions, handlers
pbx_lcd.c              -- lcd handlers
pbx_digits.c           -- 7 segment LED digits driver


===============================================================================

MORE INFORMATION - SEE PEDAL BOX IN THE WIKI

http://www.midibox.org/dokuwiki/doku.php?id=pedal_box

===============================================================================
