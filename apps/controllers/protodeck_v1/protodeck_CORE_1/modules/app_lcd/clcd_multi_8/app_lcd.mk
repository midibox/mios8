# $Id: app_lcd.mk 346 2008-06-05 23:56:31Z tk $
# defines the rule for creating the app_lcd.o object, or assembling the app_lcd.inc file directly

# add this directory to include path
GPASM_INCLUDE += -I $(MIOS_PATH)/modules/app_lcd/clcd_multi_8

# make rule for relocatable version
$(OUTDIR)/app_lcd.o: $(MIOS_PATH)/modules/app_lcd/clcd_multi_8/app_lcd.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(APP_LCD_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/app_lcd/clcd_multi_8
