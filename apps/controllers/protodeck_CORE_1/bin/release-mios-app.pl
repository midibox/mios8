#!/usr/bin/perl -w
# $Id: release-mios-app.pl 405 2008-07-27 16:20:33Z tk $
#########################################################################################
#
# Release Script for MIOS applications
# Author: tk@midibox.org (2008-07-26)
#
# SYNTAX: release-mios-app.pl <app-dir> <release-dir> [-asm_and_c]
# EXAMPLE: release-mios-app.pl $MIOS_PATH/apps/synthesizers/midibox_sid_v2 ~/release/midibox_sid_v2_0_rc99
#
# The "-asm_and_c" option is currently only used by $MIOS_PATH/examples/lcd7 applications
# to release the assembly and C version in a single package (expecting asm/ and c/ subdirectory)
#
#########################################################################################

use Getopt::Long;


#########################################################################################
# Global Settings
#########################################################################################

$debug = 0;
$asm_and_c = 0;

#########################################################################################
# Get arguments
#########################################################################################

GetOptions(
  "debug" => \$debug,
  "asm_and_c" => \$asm_and_c,
  );

if( scalar(@ARGV) != 2 ) {
   die "SYNTAX: release-mios-app.pl <app-dir> <release-dir> [-asm_and_c]\n" .
       "EXAMPLE: release-mios-app.pl \$MIOS_PATH/apps/synthesizers/midibox_sid_v2 ~/release/midibox_sid_v2_0_rc99\n"
}

my ($app_dir, $release_dir) = @ARGV;

if( !-d $app_dir ) {
  die "ERROR: application directory '${app_dir}' doesn't exist!\n";
}

if( -e $release_dir ) {
  die "ERROR: release directory '${release_dir}' does already exist!\n";
}

if( -e "${release_dir}.zip" ) {
  die "ERROR: release file '${release_dir}'.zip does already exist!\n";
}


#########################################################################################
# Create release step-by-step
# do_exec will break on errors
#########################################################################################

# transfer files into release directory
do_exec("cp -r ${app_dir} ${release_dir}");

if( !$asm_and_c ) {
  # "make dist" to create dist.sh file, execute it and remove dist.sh
  do_exec("cd ${release_dir} && make dist && sh dist.sh && rm dist.sh");
} else {
  # "make dist" to create dist.sh file, execute it and remove dist.sh

  # asm directory
  do_exec("cd ${release_dir}/asm && make dist && sh dist.sh && rm dist.sh");

  # C directory
  do_exec("cd ${release_dir}/c && make dist && sh dist.sh && rm dist.sh");
}

# set timestamp (using "sh -c" for DOS compatibility)
do_exec("sh -c 'find ${release_dir} -exec touch {} \\;'");

# create .zip file of release (DOS: requires zip)
do_exec("zip -r ${release_dir}.zip ${release_dir}");

# remove directory
do_exec("rm -rf ${release_dir}");

# thats all!
exit;

#########################################################################################
### Subroutines
#########################################################################################

sub do_exec
{
  my ($cmd) = @_;

  print "${cmd}\n";

  system($cmd) && die "ERROR: execution failed!\n";
}
