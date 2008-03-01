# $Id$
# defines the rule for creating the blm.o object

GPASM_INCLUDE += -I $(MIOS_PATH)/modules/blm 
SDCC_INCLUDE += -I $(MIOS_PATH)/modules/blm 

# make rule
$(OUTDIR)/blm.o: $(MIOS_PATH)/modules/blm/blm.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(BLM_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/blm
