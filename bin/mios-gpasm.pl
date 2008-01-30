#!/usr/bin/perl -w
#
# MIOS specific wrapper for gpasm
#
# Executes gpasm
# Swaps FSR0/FSR1 related registers if "-fixasm" is added to command line
# to ensure compatibility with MIOS register usage for SDCC generated files
#
# Thorsten Klose (2008-01-25)
#
# SYNTAX: mios-gpasm.pl <[-fixasm]> <gpasm-options> file
#

# set to 1 if "-fixasm" is found in commandline
$fixasm = 0;

# set to 1 if "-o" or "--output" has been found in commandline (ommits to add our own -o option)
$rename_output_file = 1;

# holds remaining options
@gpasm_options = ();

# holds file(s)
@gpasm_files = ();

# go through command line arguments
foreach $arg (@ARGV) {

  # add options/file to arrays, filter out -fixasm
  if( $arg =~ /\.(asm|s)$/i ) {
    push @gpasm_files, $arg;
  } else {
    if( $arg eq "-fixasm" ) {
      $fixasm = 1;
    } else {
      push @gpasm_options, $arg;
    }
    
    if( $arg eq "-o" || $arg eq "--output" ) {
      $rename_output_file = 0; # output file already specified
    } 
  }
}

if( !$fixasm ) {
  # pass arguments directly to gpasm if -fixasm not specificied
  system("gpasm " . join(" ", @ARGV)) && die "$0 exited with error status.\n";
} else {
  # else check if only one file has been specified
  if( !scalar(@gpasm_files) ) {
    die "No file specified.\nSYNTAX: $0 <[-fixasm]> <gpasm-options> file\n";
  } else {

    # convert assembler file(s), and pass to gpasm
    # note that gpasm only supports a single file - this is just to make it somehow future-proven
    foreach $file (@gpasm_files) {
      # determine extension
      if( !($file =~ /(\.asm|\.s)$/i) ) {
	die "ERROR: $0 isn't able to determine the file extension!\n";
      }
      my $ext = $1;
      my $file_without_ext = substr($file, 0, -(length($ext)));

      my $tmp_file = "${file_without_ext}__mios-gpasm-tmp${ext}";

      print "$0 modifies ${file}, result in ${tmp_file}\n";
      fixasm($file, $tmp_file);

      my $cmd = "gpasm " . join(" ", @gpasm_options) . " " . $tmp_file . ($rename_output_file ? " -o ${file_without_ext}.o" : "");
      print "[$0] ${cmd}\n";
      my $error_status = system($cmd);

      # unlink($tmp_file) || print "WARNING: $0 isn't able to remove $tmp_file\n";
      # (disabled - we want to see the converted file for more transparancy)

      # error?
      if( $error_status ) {
	die "$0 exited with error status.\n";
      }
    }
  }

}

exit;




##
## Assembler file conversion function
##
sub fixasm
{
  my ($in_file, $out_file) = @_;

  open(IN, "<${in_file}") || die "ERROR: $0 isn't able to open '${in_file}' for reading!\n";
  open(OUT, ">${out_file}") || die "ERROR: cannot open '${out_file}' for writing!\n";

  while( <IN> ) {
    # to prevent "Error - section '.registers' can not fit the absolute section. Section '.registers' start = 0x00000000, length=0x00000003"
    # for MIOS it is also required to set the start address to 0x010
    s/^\.registers\tudata_ovr\t0x0000/.registers\tudata_ovr\t0x010/;

    # many MIOS functions are using FSR1[LH] as internal working pointer. 
    # FSR1 is used as stack pointer by SDCC --- to avoid problems, I'm
    # swapping FSR1x and FSR0x
    s/^INDF1\s+equ\s+0xfe7/INDF1\tequ\t0xfef ;; normaly 0xfe7, changed by $0/;
    s/^POSTINC1\s+equ\s+0xfe6/POSTINC1\tequ\t0xfee ;; normaly 0xfe6, changed by $0/;
    s/^POSTDEC1\s+equ\s+0xfe5/POSTDEC1\tequ\t0xfed ;; normaly 0xfe5, changed by $0/;
    s/^PREINC1\s+equ\s+0xfe4/PREINC1\tequ\t0xfec ;; normaly 0xfe4, changed by $0/;
    s/^PLUSW1\s+equ\s+0xfe3/PLUSW1\tequ\t0xfeb ;; normaly 0xfe3, changed by $0/;
    s/^FSR1H\s+equ\s+0xfe2/FSR1H\tequ\t0xfea ;; normaly 0xfe2, changed by $0/;
    s/^FSR1L\s+equ\s+0xfe1/FSR1L\tequ\t0xfe9 ;; normaly 0xfe1, changed by $0/;

    s/^INDF0\s+equ\s+0xfef/INDF0\tequ\t0xfe7 ;; normaly 0xfef, changed by $0/;
    s/^POSTINC0\s+equ\s+0xfee/POSTINC0\tequ\t0xfe6 ;; normaly 0xfee, changed by $0/;
    s/^POSTDEC0\s+equ\s+0xfed/POSTDEC0\tequ\t0xfe5 ;; normaly 0xfed, changed by $0/;
    s/^PREINC0\s+equ\s+0xfec/PREINC0\tequ\t0xfe4 ;; normaly 0xfec, changed by $0/;
    s/^PLUSW0\s+equ\s+0xfeb/PLUSW0\tequ\t0xfe3 ;; normaly 0xfeb, changed by $0/;
    s/^FSR0H\s+equ\s+0xfea/FSR0H\tequ\t0xfe2 ;; normaly 0xfea, changed by $0/;
    s/^FSR0L\s+equ\s+0xfe9/FSR0L\tequ\t0xfe1 ;; normaly 0xfe9, changed by $0/;

    print OUT $_;
  }
  close(IN);
  close(OUT);
}
