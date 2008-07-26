#!/usr/bin/perl -w
# $Id$
#########################################################################################
#
# Release Script for MIOS base package
# Author: tk@midibox.org (2008-07-26)
#
# SYNTAX: release-base.pl <release-dir>
# EXAMPLE: release-base.pl ~/release/mios_base_v1_0
#
#########################################################################################

use Getopt::Long;

#########################################################################################
# Check if MIOS_PATH and MIOS_BIN_PATH is set
#########################################################################################

if( !exists $ENV{MIOS_PATH} ) { die "ERROR: MIOS_PATH environment variable not set!\n"; }
if( !exists $ENV{MIOS_BIN_PATH} ) { die "ERROR: MIOS_BIN_PATH environment variable not set!\n"; }

#########################################################################################
# Global Settings
#########################################################################################

$debug = 0;

# MIOS base directories
@base_dirs = (
  "bin",
  "etc",
  "doc",
  "lib",
  "include",
  "modules",
  "apps/examples",
  "apps/templates",
);


#########################################################################################
# Get arguments
#########################################################################################

GetOptions(
  "debug" => \$debug,
  );

if( scalar(@ARGV) != 1 ) {
   die "SYNTAX: release-base.pl <release-dir>\n" .
       "EXAMPLE: release-base.pl ~/release/mios_base_v1_0\n"
}

my ($release_dir) = @ARGV;

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

# create release directory
do_exec("mkdir ${release_dir}");

# transfer files into release directory
foreach $base_dir (@base_dirs) {
  do_exec("mkdir -p ${release_dir}/${base_dir}");
  do_exec("cp -r $ENV{MIOS_PATH}/${base_dir}/* ${release_dir}/${base_dir}");
}

# remove svn files (here we could add "sh -c" for DOS compatibility, but we would need to solve the recursive-' issue before...)
# ignore error state!
system("find ${release_dir} -regex '.*/.svn\$' -exec rm -rf {} \\;");

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
