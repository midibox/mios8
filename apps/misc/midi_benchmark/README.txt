$Id$

MIDI Performance Measuring Application

This is some kind of benchmark which helps to evaluate the performance
of your MIDI interface (and computer)

To get the test running you have to create a MIDI feedback loop.

Examples:
   Direct Feedback:   CORE1 OUT -> CORE1 IN
   Merger Feedback:   CORE1 OUT -> CORE2 IN -> CORE2 OUT -> CORE1 IN
   Computer Feedback: CORE1 OUT -> PC MIDI IN -> MIDI-Ox -> PC MIDI OUT -> CORE1 IN

A MIDI controller event will be sent and should be received "immediately"
is measured with timer3, prescaler 1:8 - this means, that the counter 
result has to be multiplied by 8 and 100 nS (@40 MHz) to get the
absolute delay

Note that the delay is always greater than 960 uS, since this is the
initial transfer time of a controller event

The current, minimal and maximum result will be print on LCD
To determine the jitter, calculate: (max-min) / 2

IMPORTANT: remove the feedback loop before uploading a new application,
otherwise MIOS will wait endless before rebooting the core!!!

And here the results:

   o direct feedback:
     Min: 1218, Max:  1262  ->  0.97 mS /  1.01 mS  ->  +/- 0.02 mS Jitter

   o merger feedback (2 cores):
     Min: 1664, Max:  1735  ->  1.33 mS /  1.38 mS  ->  +/- 0.03 mS Jitter

   o RME Hammerfall DSP/Multiface, WinXP, Feedback via MIDI-Ox
     Min: 2497, Max:  2599  ->  2.00 mS /  2.07 mS  ->  +/- 0.04 mS Jitter

   o RME Hammerfall DSP/Multiface, WinXP, Feedback via Logic Audio
     Min: 2565, Max:  3820  ->  2.05 mS /  3.05 mS  ->  +/- 0.50 mS Jitter

   o MBHP_USB (WinXP Legacy Driver), Feedback via MIDI-Ox
     Min: 3320, Max:  4555  ->  2.66 mS /  3.64 mS  ->  +/- 0.50 mS Jitter

   o MBHP_USB (WinXP Legacy Driver), Feedback via Logic Audio
     Min: 3510, Max:  6291  ->  2.81 mS /  5.03 mS  ->  +/- 1.11 mS Jitter

   o MIDIsport 2x2, WinXP, Feedback via MIDI-Ox
     Min: 6219, Max:  9960  ->  4.98 mS /  7.97 mS  ->  +/- 1.5 mS Jitter

   o MIDIsport 2x2, WinXP, Feedback via Logic Audio
     Min: 7608, Max: 13446  ->  6.08 mS / 10.76 mS  ->  +/- 2.3 mS Jitter


For more results see:
   http://www.midibox.org/forum/index.php?topic=2342.0