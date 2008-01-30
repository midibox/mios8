# $Id$
# defines the rule for creating the pic18f452.o object
# included from $MIOS_PATH/include/makefile/common.mk

# make rule
$(OUTDIR)/pic18f452.o: $(MIOS_PATH)/modules/libdev/pic18f452.c
	$(SDCC) $(SDCC_FLAGS) $(SDCC_INCLUDE) $(SDCC_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/libdev/pic18f452.c $(MIOS_PATH)/modules/libdev/pic18f452.h
