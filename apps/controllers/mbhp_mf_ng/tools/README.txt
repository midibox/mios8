# $Id$
===============================================================================
The gen_lookup_table.pl script allows to generate 10bit calibration tables
for outgoing fader values and internally used fader values of the motor control loop.

===============================================================================
Required tool: 'perl'
Optional tool: 'gnuplot' to display the table 


===============================================================================
Call:

- perl tools/gen_lookup_table.pl src/cal_fader
  To generate the calibration table for outgoing fader values

- perl tools/gen_lookup_table.pl src/cal_motor
  To generate the calibration table for internally used fader values which determine the fader position for motor control


Whenever a new table has been generated, enter 'make' in the base directory of the mbhp_mf_ng application to build a new .hex file

Actually two .hex files are generated:

- setup_mbhp_mf_ng_standard.hex
  Contains the complete firmware + EEPROM preset + calibration tables

- cal_tables_only.hex
  Contains only the calibration tables
  Use this .hex file if you want to update the calibration tables without touching the EEPROM configuration that
  you may have changed via the MBHP_MF_NG tool in MIOS Studio


If "make" isn't installed on your computer, you could also build the .hex files directory with following commands:
gpasm  -p p18f452  -I./src -I ./include/asm -I ./include/share -I ./modules/app_lcd/dummy  setup_mbhp_mf_ng_standard.asm
gpasm  -p p18f452  -I./src -I ./include/asm -I ./include/share -I ./modules/app_lcd/dummy  cal_tables_only.asm


===============================================================================

Changing the calibration curve: you've to edit the @x and @y parameter lists in gen_lookup_table.pl

With:
  my @x = (0, 1023);
  my @y = (0, 1023);

a direct translation takes place, means: values 0..1023 will be translated to 0..1023


With:
  my @x = (0, 10, 20, 40, 100, 300, 700, 900, 950, 1000, 1023);
  my @y = (0, 20, 40, 70, 110, 250, 800, 850, 920, 990, 1023);

a linear interpolation between the data points takes place.

The number of data points isn't limited.

After calling the gen_lookup_table.pl script, check the output of gnuplot and/or check the resulting
table in the .inc file that has been generated.



