# $Id$
# defines the rule for creating the iic_eeprom8.o object

GPASM_INCLUDE += -I $(MIOS_PATH)/modules/iic_eeprom8
SDCC_INCLUDE += -I $(MIOS_PATH)/modules/iic_eeprom8

# make rule
$(OUTDIR)/iic_eeprom8.o: $(MIOS_PATH)/modules/iic_eeprom8/iic_eeprom8.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(IIC_EEPROM8_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/iic_eeprom8
