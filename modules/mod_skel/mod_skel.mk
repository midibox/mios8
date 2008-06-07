# $Id: mod_skel.mk 69 2008-02-01 00:20:18Z tk $
# defines the rule for creating the mod_skel.o object, or assembling the mod_skel.inc file directly

# add this directory to include path
GPASM_INCLUDE += -I $(MIOS_PATH)/modules/mod_skel
SDCC_INCLUDE += -I $(MIOS_PATH)/modules/mod_skel 

# make rule for relocatable version
$(OUTDIR)/mod_skel.o: $(MIOS_PATH)/modules/mod_skel/mod_skel.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(mod_skel_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/mod_skel
