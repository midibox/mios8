# $Id$
# defines the rule for creating the blm.o object

GPASM_INCLUDE += -I $(MIOS_PATH)/modules/blm_scalar
SDCC_INCLUDE += -I $(MIOS_PATH)/modules/blm_scalar

# make rule
$(OUTDIR)/blm_scalar.o: $(MIOS_PATH)/modules/blm_scalar/blm_scalar.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(BLM_SCALAR_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/blm_scalar
