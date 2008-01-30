#!/usr/bin/perl -w
#
# MIOS specific wrapper for sdcc
#
# Executes sdcc
# Uses "mios-gpasm -fixasm" wrapper to assemble the generated code
#
# Thorsten Klose (2008-01-25)
#
# SYNTAX: mios-sdcc.pl <sdcc-arguments>
#

if( !exists $ENV{MIOS_BIN_PATH} ) { die "ERROR: MIOS_BIN_PATH variable not set!\n"; }

# switch between / and \ depending on OS
my $mios_gpasm = "perl $ENV{MIOS_BIN_PATH}" . ($^O eq "MSWin32" ? "\\" : "/") . "mios-gpasm.pl -fixasm";

system("sdcc --asm=\"${mios_gpasm}\" " . join(" ", @ARGV))
  && die "$0 exited with error status.\n";

