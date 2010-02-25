# $Id: aout.mk 130 2008-02-29 00:12:53Z tk $
# defines the rule for creating the aout.o object

GPASM_INCLUDE += -I $(MIOS_PATH)/modules/aout 
SDCC_INCLUDE += -I $(MIOS_PATH)/modules/aout 

# make rule
$(OUTDIR)/aout.o: $(MIOS_PATH)/modules/aout/aout.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(AOUT_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/aout
