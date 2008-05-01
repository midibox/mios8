# $Id$
#
# following variables should be set before including this file:
#   - PROCESSOR e.g.: 18f452
#   - LKR_FILE  e.g.: $(MIOS_PATH)/etc/lkr/p$(PROCESSOR).lkr
#   - PROJECT   e.g.: project   # (.lst, .cod, .hex, .map will be added automatically)
#   - OBJS      e.g.: pic18f452.o mios_wrapper.o main.o
#   - GPASM_INCLUDE  e.g.: -I./ui  # (more include pathes will be added by .mk files)
#   - SDCC_INCLUDE   e.g.: -I./ui  # (more include pathes will be added by .mk files)
#   - GPASM_DEFINES  e.g.: -DDEBUG_MODE=0
#   - SDCC_DEFINES   e.g.: -DDEBUG_MODE=0
#   - MIOS_WRAPPER_DEFINES  e.g.: -DSTACK_HEAD=0x37f -DSTACK_IRQ_HEAD=0x33f
#   - DIST      e.g.: ./
#
# Application specific source files (.c, .asm, .s) have to be located in ./ or ./src,
# Modules can be added by including .mk files from $MIOS_PATH/modules/*/*.mk
#
# If source files are located anywhere else (e.g. ./ui), add the path to 
# [GPASM|SDCC]_INCLUDE, and define additional rules for <dir>/%.c and/or <dir>/%.asm and/or <dir>/%.s
#

# output directory
OUTDIR = _output

# GPASM execution via wrapper
GPASM = sh $(MIOS_BIN_PATH)/mios-gpasm -c

# SDCC execution via wrapper
SDCC = sh $(MIOS_BIN_PATH)/mios-sdcc -c

# GPLIB execution (w/o wrapper yet)
GPLIB = gplib -c

# GPLINK execution (w/o wrapper yet)
GPLINK = gplink

# include files used by GPASM
GPASM_INCLUDE = -I./src -I $(MIOS_PATH)/include/asm

# additional defines used by SDCC
GPASM_DEFINES += # reserved for future "default extensions"

# add default flags for GPASM
GPASM_FLAGS += -p p$(PROCESSOR)

# include files used by SDCC
SDCC_INCLUDE += -I./src -I $(MIOS_PATH)/include/c

# additional defines used by SDCC
SDCC_DEFINES += # reserved for future "default extensions"

# add default flags for SDCC
SDCC_FLAGS += -mpic16 -p$(PROCESSOR) --fommit-frame-pointer --optimize-goto --optimize-cmp --disable-warning 85 --obanksel=2

# add default flags for GPLINK
GPLINK_FLAGS += -s $(LKR_FILE) $(MIOS_PATH)/lib/libsdcc.lib $(MIOS_PATH)/lib/pic$(PROCESSOR).lib

# add files for distribution
DIST += $(MIOS_PATH)/include/makefile/common.mk $(MIOS_PATH)/include/c $(MIOS_PATH)/include/asm
DIST += $(LKR_FILE)
DIST += $(MIOS_PATH)/lib/libsdcc.lib $(MIOS_PATH)/lib/pic$(PROCESSOR).lib
DIST += $(MIOS_BIN_PATH)/mios-gpasm $(MIOS_BIN_PATH)/mios-sdcc

# rule to create a .hex file
# note: currently we always require a "cleanall", since dependencies (e.g. on .h files) are not properly declared
# later we could try it w/o "cleanall", and propose the usage of this step to the user
$(PROJECT).hex: cleanall mk_outdir $(addprefix $(OUTDIR)/, $(OBJS))
	$(GPLINK) $(GPLINK_FLAGS) -m -o $(PROJECT).hex $(addprefix $(OUTDIR)/, $(OBJS))

# default rule for compiling .c programs
# note that the same is required for files located in current, and src/ directory
# (haven't found a better way for re-using a rule yet)
$(OUTDIR)/%.o: %.c
	$(SDCC) $(SDCC_FLAGS) $(SDCC_INCLUDE) $(SDCC_DEFINES) $< -o $@
$(OUTDIR)/%.o: src/%.c
	$(SDCC) $(SDCC_FLAGS) $(SDCC_INCLUDE) $(SDCC_DEFINES) $< -o $@

# default rule for compiling .asm programs
# we expect, that they don't need to be converted through the wrapper
$(OUTDIR)/%.o: %.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $< -o $@
$(OUTDIR)/%.o: src/%.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $< -o $@

# default rule for compiling .s programs (these are assembler programs as well)
# for .s we expect, that they need to be converted through the wrapper
$(OUTDIR)/%.o: %.s
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) -fixasm $< -o $@
$(OUTDIR)/%.o: %.S
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) -fixasm $< -o $@
$(OUTDIR)/%.o: src/%.s
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) -fixasm $< -o $@
$(OUTDIR)/%.o: src/%.S
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) -fixasm $< -o $@

# include mios_wrapper related rules
include $(MIOS_PATH)/modules/mios_wrapper/mios_wrapper.mk

# create directory that contains object files
mk_outdir:
	mkdir -p $(OUTDIR)

# clean temporary files
clean:
	rm -rf $(OUTDIR)/*
	rm -rf $(OUTDIR)
	rm -rf *.cod *.map *.lst

# clean temporary files + project image
cleanall: clean
	rm -rf *.hex

# creating a distribution (release) package
dist: clean
#	echo $(foreach dir, $(DIST), `echo 'XXX $(dir)' | sed -e "s/$MIOS_BIN_PATH/.\/bin/"` )
	# damned, this doesn't work under windows, and "basename" doesn't work properly as well
	# due to <device-name>:\...
	# call a perl script instead
	perl $(MIOS_BIN_PATH)/release-app.pl $(DIST)
