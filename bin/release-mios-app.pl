#!/usr/bin/perl -w
# $Id$
#########################################################################################
#
# Release Script for MIOS applications
# Author: tk@midibox.org (2008-07-26)
#
# SYNTAX: release-mios-app.pl <app-dir> <release-dir>
# EXAMPLE: release-mios-app.pl $MIOS_PATH/apps/synthesizers/midibox_sid_v2 ~/release/midibox_sid_v2_0_rc99
#
#########################################################################################

use Getopt::Long;


#########################################################################################
# Global Settings
#########################################################################################

$debug = 0;

#########################################################################################
# Get arguments
#########################################################################################

GetOptions(
  "debug" => \$debug,
  );

if( scalar(@ARGV) != 2 ) {
   die "SYNTAX: release-mios-app.pl <app-dir> <release-dir>\n" .
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

# "make dist" to create dist.sh file
do_exec("cd ${release_dir}; make dist");

# execute dist.sh file
do_exec("cd ${release_dir}; source dist.sh");

# remove dist.sh file
do_exec("cd ${release_dir}; rm dist.sh");

# set timestamp
do_exec("find ${release_dir} -exec touch {} \\;");

# create .zip file of release
do_exec("zip -r ${release_dir}.zip ${release_dir}");

# remove directory
do_exec("/bin/rm -rf ${release_dir}");

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
