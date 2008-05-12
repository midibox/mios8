$Id$

SID Testtone generator

This MIOS application generates a simple 1kHz triangle testtone
on the SID. There is no "MIDI Note On" event required to start the
sound, you should hear it immediately after the MIOS bootphase

In addition, a 1 kHz pulse will be generated on the CS lines to the
two SIDs. This feature allows you to test the output amplifier of the
SID module (unplug SID, connect socket pin #8 with #27)

