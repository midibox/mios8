# $Id$

# define the processor, linker file and project name
PROCESSOR = p16f88
LKR_FILE  = project.lkr
PROJECT   = project

# list of objects that should be created and linked
OBJS = main.o flash.o midi.o prot.o iic.o iic_cmd.o

# include pathes (more will be added by .mk files)
GPASM_INCLUDE = 
SDCC_INCLUDE  = 

# optional defines that should be passed to GPASM/SDCC
GPASM_DEFINES = -DDEBUG_MODE=0
SDCC_DEFINES  = -DDEBUG_MODE=0


###############################################################################
# Make rules
# (we are not using $MIOS_PATH/include/makefiles/common.mk, since this
# isn't a MIOS project
###############################################################################

# output directory
OUTDIR = _output

# GPASM execution
GPASM = gpasm -c

# SDCC execution
SDCC = sdcc -c

# GPLINK execution
GPLINK = gplink

# include files used by GPASM
GPASM_INCLUDE = -I./src 

# additional defines used by SDCC
GPASM_DEFINES += # reserved for future "default extensions"

# add default flags for GPASM
GPASM_FLAGS += -p $(PROCESSOR)

# include files used by SDCC
SDCC_INCLUDE += -I./src 

# additional defines used by SDCC
SDCC_DEFINES += # reserved for future "default extensions"

# add default flags for SDCC
SDCC_FLAGS += -mpic16 -$(PROCESSOR) --fommit-frame-pointer --optimize-goto --optimize-cmp --disable-warning 85 --obanksel=2 -pleave-reset-vector

# add default flags for GPLINK
GPLINK_FLAGS += -s $(LKR_FILE)

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
