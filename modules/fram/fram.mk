# defines the rule for creating the fram.o object, or assembling the fram.inc file directly

# add this directory to include path
GPASM_INCLUDE += -I $(MIOS_PATH)/modules/fram
SDCC_INCLUDE += -I $(MIOS_PATH)/modules/fram

# make rule for relocatable version
$(OUTDIR)/fram.o: $(MIOS_PATH)/modules/fram/fram.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(FRAM_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/fram
