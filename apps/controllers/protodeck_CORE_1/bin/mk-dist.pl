#!/usr/bin/perl -w
# $Id: mk-dist.pl 413 2008-07-29 04:49:28Z stryd_one $
#########################################################################################
#
# Creates dist.sh file for a proper MIOS application release
# Author: tk@midibox.org (2008-03-05)
#
# Called from Makefile with "make dist"
# Don't use this sript directly!
# To release an application, use "release-miosapp.pl" instead
#
#########################################################################################

use Getopt::Long;
use Cwd;


#########################################################################################
# Check if MIOS_PATH and MIOS_BIN_PATH is set
#########################################################################################

if( !exists $ENV{MIOS_PATH} ) { die "ERROR: MIOS_PATH environment variable not set!\n"; }
if( !exists $ENV{MIOS_BIN_PATH} ) { die "ERROR: MIOS_BIN_PATH environment variable not set!\n"; }


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

if( !scalar(@ARGV) ) {
  die "SYNTAX: mk-dist.pl <files> [<-debug>]\n";
}


#########################################################################################
# Determine MIOS base/bin and current working directory
#########################################################################################

my $basedir = $ENV{MIOS_PATH};
print "MIOS_PATH = ${basedir}\n";

my $bindir = $ENV{MIOS_BIN_PATH};
print "MIOS_BIN_PATH = ${bindir}\n";

my $workdir = getcwd;
print "Release Directory = ${workdir}\n";


#########################################################################################
# Determine relative paths to MIOS_PATH
#########################################################################################

my %rel_path = ();
my %rel_path_work = ();
my $err = "";

foreach $path (@ARGV) {
  if( substr($path, 0, 2) eq "./" ) {
    $rel_path_work{$path} = $path;
  } elsif( substr($path, 0, length($workdir)) eq $workdir ) {
    $rel_path_work{$path} = "." . substr($path, length($workdir));
  } elsif( substr($path, 0, length($bindir)) eq $bindir ) {
    $rel_path{$path} = "./bin/" . substr($path, length($bindir));
  } elsif( substr($path, 0, length($basedir)) eq $basedir ) {
    $rel_path{$path} = "." . substr($path, length($basedir));
  } else {
    $err .= "${path}\n";
  }
}

if( length($err) ) {
  die "ERROR: not able to determine relative path to following files/directories:\n${err}";
}


#########################################################################################
# Generate commands which are required for the release:
#########################################################################################

my $cmds = ();


# currently not fully automated, because we need some kind of protection, which prevents
# that somebody types "make dist" within the original directory of an application

# note: "cp -p" unfortunately not available on BSD version of cp command (e.g. MacOS)
# therefore we have to create new directories before...

my %dirs = ();
foreach $path (keys %rel_path) {
  my @path = split("/", $rel_path{$path});
  while( scalar(@path) ) {
    @path = splice(@path, 0, -1);
    my $dir = join("/", @path);
    if( length($dir) && $dir ne "." ) {
      $dirs{$dir} = 1;
    }
  }
}

foreach $path (sort keys %dirs) {
  $path =~ s/\\/\//g; # for DOS: convert \ to /
  push @cmds, "mkdir ${path}";
}


foreach $path (sort keys %rel_path) {
  my $rel = $rel_path{$path};
  $path =~ s/\\/\//g; # for DOS: convert \ to /
  $rel =~ s/\\/\//g; # for DOS: convert \ to /
  push @cmds, "cp -rf ${path} ${rel}";
}


#foreach $path (sort keys %rel_path_work) {
#  push @cmds, "cp -rf $path $rel_path_work{$path}";
#}

push @cmds, "find . -regex '.*/.svn\$' -type d -exec rm -rf {} \\;";

push @cmds, "rm -f Makefile.orig";
push @cmds, "mv Makefile Makefile.orig";
push @cmds, "echo \"MIOS_PATH = .\" > Makefile";
push @cmds, "echo \"MIOS_BIN_PATH = ./bin\" >> Makefile";
push @cmds, "echo \"export MIOS_PATH\" >> Makefile";
push @cmds, "echo \"export MIOS_BIN_PATH\" >> Makefile";
push @cmds, "echo \"include Makefile.orig\" >> Makefile";


push @cmds, "make";
push @cmds, "make clean";

push @cmds, "echo Done. You can remove dist.sh now!";




open(OUT, ">dist.sh") || die "ERROR: cannot open dist.sh for writing!\n";
print OUT join("\n", @cmds) . "\n";
close(OUT);

print "###############################################################################\n";
print "Commit all your changes to the SVN server before doing a release!\n";
print "Thereafter copy the application into a new directory, and type 'sh dist.sh'\n";
print "If the script passes successfully, you can .zip the directory and release it\n";
print "###############################################################################\n";

