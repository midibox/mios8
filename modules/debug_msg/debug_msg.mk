# $Id$
# defines the rule for creating the debug_msg.o object

GPASM_INCLUDE += -I $(MIOS_PATH)/modules/debug_msg
SDCC_INCLUDE += -I $(MIOS_PATH)/modules/debug_msg

# make rule
$(OUTDIR)/debug_msg.o: $(MIOS_PATH)/modules/debug_msg/debug_msg.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(DEBUG_MSG_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/debug_msg
