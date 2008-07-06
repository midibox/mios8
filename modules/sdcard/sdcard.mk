# $Id$
# defines the rule for creating the sdcard.o object

GPASM_INCLUDE += -I $(MIOS_PATH)/modules/sdcard
SDCC_INCLUDE += -I $(MIOS_PATH)/modules/sdcard

# make rule
$(OUTDIR)/sdcard.o: $(MIOS_PATH)/modules/sdcard/sdcard.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(SDCARD_DEFINES) $< -o $@
$(OUTDIR)/sdcard_file.o: $(MIOS_PATH)/modules/sdcard/sdcard_file.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(SDCARD_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/sdcard
