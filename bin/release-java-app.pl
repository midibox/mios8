#!/usr/bin/perl -w
# $Id$
#########################################################################################
#
# Release Script for Java Applications
# Author: tk@midibox.org (2009-02-6)
#
#########################################################################################

use Getopt::Long;

#########################################################################################
# Check if MIOS_PATH is set
#########################################################################################

if( !exists $ENV{MIOS_PATH} ) { die "ERROR: MIOS_PATH environment variable not set!\n"; }

$JAVA_DIR = "$ENV{MIOS_PATH}/java";
if( !-d $JAVA_DIR ) {
  die "ERROR: java directory ${JAVA_DIR} doesn't exist!\n";
}

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
   die "SYNTAX: release-mios-studio.pl <release-file> <release-dir>\n" .
       "EXAMPLE: release-mios-studio.pl release_files/MIOSStudio.txt ~/release/MIOSStudio_beta8_3\n"
}

my ($release_file, $release_dir) = @ARGV;

if( !-e $release_file ) {
  die "ERROR: release file '${release_file}' doesn't exist!\n";
}

if( -e $release_dir ) {
  die "ERROR: release directory '${release_dir}' does already exist!\n";
}

if( -e "${release_dir}.jar" ) {
  die "ERROR: .jar file '${release_dir}'.jar does already exist!\n";
}

if( -e "${release_dir}.zip" ) {
  die "ERROR: release file '${release_dir}'.zip does already exist!\n";
}


my $main_class = $release_file;
$main_class =~ s/.*\///;
$main_class =~ s/\.txt$//;

print "Releasing '${main_class}' to ${release_dir}\n";


#########################################################################################
# Read Release file
#########################################################################################

@java_files = ();

open(IN, "<${release_file}") || die "ERROR: cannot open ${release_file} for reading!\n";
while( <IN> ) {
  chop;
  s/^\s*//;
  s/\s*$//;
  s/#.*//;
  if( length($_) ) {
    print "Adding file(s): $_\n";
    push @java_files, $_;
  }
}


#########################################################################################
# Create release step-by-step
# do_exec will break on errors
#########################################################################################

# create release directory
do_exec("mkdir ${release_dir}");

# transfer files into release directory
foreach $java_file (@java_files) {
  my @subdirs = split("/", $java_file);
  my $subdir = join("/", splice(@subdirs, 0, -1));
  if( length($subdir) ) {
    do_exec("mkdir -p ${release_dir}/${subdir}");
  }
  do_exec("cp -r ${JAVA_DIR}/${java_file} ${release_dir}/${subdir}");
}

# remove svn files (here we could add "sh -c" for DOS compatibility, but we would need to solve the recursive-' issue before...)
# ignore error state!
system("find ${release_dir} -regex '.*/.svn\$' -exec rm -rf {} \\;");

# remove .class files
system("find ${release_dir} -regex 'class$' -exec rm -rf {} \\;");

# set timestamp
do_exec("find ${release_dir} -exec touch {} \\;");

# compile java code
do_exec("cd ${release_dir}; javac org/midibox/*/*.java org/midibox/*/*/*.java org/midibox/*/*/*/*.java *.java");

# create manifest
my $manifest = "META-INF/MANIFEST.MF";
do_exec("mkdir -p ${release_dir}/META-INF");
do_exec("echo \"Manifest-Version: 1.0\" > ${release_dir}/${manifest}\n");
do_exec("echo \"Main-Class: ${main_class}\" >> ${release_dir}/${manifest}\n");

# create .jar file of release
do_exec("cd ${release_dir}; jar cvfm ${release_dir}.jar ${manifest} .");

# create .zip file of release
my @release_path = split("/", $release_dir);
my $release_name = $release_path[scalar(@release_path)-1];
do_exec("cd " . join("/", splice(@release_path, 0, -1)) . "; zip ${release_name}.zip ${release_name}.jar");

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
