# $Id$
# defines the rule for creating the aout.o object

SDCC_INCLUDE += -I $(MIOS_PATH)/modules/aout 

# make rule
$(OUTDIR)/aout.o: $(MIOS_PATH)/modules/aout/aout.c
	$(SDCC) $(SDCC_FLAGS) $(SDCC_INCLUDE) $(SDCC_DEFINES) $(AOUT_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/aout
