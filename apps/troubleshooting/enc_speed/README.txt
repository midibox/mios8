$Id$

ENC Speed Test

This application helps to find your favourite speed setting

The encoder pins are specified in mios_tables.inc, by default pin #0 and #1
are used.

The speed setting can be selected by two buttons, the pin numbers
are defined in the header of main.asm

With these buttons you can switch from:
      MIOS_ENC_SPEED_SLOW with predivider == 0
over  ...
over  MIOS_ENC_SPEED_SLOW with predivider == 7
over  MIOS_ENC_SPEED_NORMAL
over  MIOS_ENC_SPEED_FAST with divider == 0
over  ...
to    MIOS_ENC_SPEED_FAST with divider == 5
