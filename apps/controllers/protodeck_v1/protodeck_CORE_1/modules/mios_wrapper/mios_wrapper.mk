# $Id: mios_wrapper.mk 50 2008-01-30 21:47:50Z tk $
# defines the rule for creating the mios_wrapper.o object
# included from $MIOS_PATH/include/makefile/common.mk

# make rule
$(OUTDIR)/mios_wrapper.o: $(MIOS_PATH)/modules/mios_wrapper/mios_wrapper.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(MIOS_WRAPPER_DEFINES) -I $(MIOS_PATH)/modules/mios_wrapper $<  -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/mios_wrapper
