# defines the rule for creating the pic18f4685.o object
# included from $MIOS_PATH/include/makefile/common.mk

# make rule
$(OUTDIR)/pic18f4685.o: $(MIOS_PATH)/modules/libdev/pic18f4685.c
	$(SDCC) $(SDCC_FLAGS) $(SDCC_INCLUDE) $(SDCC_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/libdev/pic18f4685.c $(MIOS_PATH)/modules/libdev/pic18f4685.h $(MIOS_PATH)/modules/libdev/pic18f4685.mk
