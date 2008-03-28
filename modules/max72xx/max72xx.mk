# $Id$
# defines the rule for creating the max72xx.o object

GPASM_INCLUDE += -I $(MIOS_PATH)/modules/max72xx
SDCC_INCLUDE += -I $(MIOS_PATH)/modules/max72xx

# make rule
$(OUTDIR)/max72xx.o: $(MIOS_PATH)/modules/max72xx/max72xx.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(MAX72XX_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/max72xx
