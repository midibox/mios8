# $Id$
#
# following variables should be set before including this file:
#   - PROCESSOR       e.g.: p18f452
#   - GPASM_FLAGS     e.g.: -m
#   - GPASM_INCLUDE   e.g.: -I./ui  # (more include pathes will be added by .mk files)
#   - GPASM_DEFINES         e.g.: -DDEBUG_MODE=0
#   - DIST      e.g.: ./
#
# Application specific source file (.asm) has to be located in ./ or ./src
#

# output directory
OUTDIR = _output

# GPASM execution
GPASM = gpasm

# include files used by GPASM
GPASM_INCLUDE += -I./src -I $(MIOS_PATH)/include/asm

# add default flags for GPASM
GPASM_FLAGS += -p $(PROCESSOR)

# add files for distribution
DIST += $(MIOS_PATH)/include/makefile/asm_only.mk $(MIOS_PATH)/include/asm


# rule for all .asm file(s)
# output: .hex file
%.hex: %.asm
	gpasm $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $<

# clean temporary files
clean:
	rm -rf *.cod *.lst *.err

# clean temporary files + project image
cleanall: clean
	rm -rf *.hex

# creating a distribution (release) package
dist: clean
#	echo $(foreach dir, $(DIST), `echo 'XXX $(dir)' | sed -e "s/$MIOS_BIN_PATH/.\/bin/"` )
	# damned, this doesn't work under windows, and "basename" doesn't work properly as well
	# due to <device-name>:\...
	# we should prefer a perl script (for more flexibility)
	# However, here are the DIST pathes:
	echo $(DIST)
