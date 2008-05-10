$Id$

From http://www.midibox.org/forum/index.php?topic=587.0

---------
The toy for this weekend is a MIDI delay which works different 
from the existing ones, because it allows to route the delayed 
notes to different channels. In this way very interesting sequences 
can be realized without much effort. My favourite configuration 
consists of four instances of virtual synthesizers which are playing 
the same base sound, but with slight modifications (Cutoff-frequency, 
octave, volume, panorama). 

4 independent delay lines are available, they can be clocked by an 
internal BPM generator or via external MIDI clock (when BPM is set to zero)

Here a short demo tune:
   http://www.midibox.org/mios/magic_midi_delay_demo.mp3

However, this is just a (not well mixed) example - most important point 
is that I had a lot of fun during live-playing! 


The source code is also a good example, how to interact with the LCD, buttons, 
pots, encoders, LEDs

Minimal configuration: one core module
parameters can be controlled via MIDI (see main.asm)

Options: one LCD (character or graphical) which shows the parameters
30 buttons to control the parameters directly
alternatively 8 encoders to control delay and transpose value of 4 channels 
and one jog wheel to control the BPM
alternatively 14 pots or faders to control the parameters directly

mods are welcome!

Have fun! ;-)
