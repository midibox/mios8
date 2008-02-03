# $Id$
# defines the rule for creating the iic_midi.o object

GPASM_INCLUDE += -I $(MIOS_PATH)/modules/iic_midi
SDCC_INCLUDE += -I $(MIOS_PATH)/modules/iic_midi

# make rule
$(OUTDIR)/iic_midi.o: $(MIOS_PATH)/modules/iic_midi/iic_midi.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(IIC_MIDI_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/iic_midi
