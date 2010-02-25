# $Id: j5_io.mk 149 2008-03-01 14:35:40Z tk $
# defines the rule for creating the j5_io.o object

GPASM_INCLUDE += -I $(MIOS_PATH)/modules/j5_io 
SDCC_INCLUDE += -I $(MIOS_PATH)/modules/j5_io 

# make rule
$(OUTDIR)/j5_io.o: $(MIOS_PATH)/modules/j5_io/j5_io.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(J5_IO_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/j5_io
