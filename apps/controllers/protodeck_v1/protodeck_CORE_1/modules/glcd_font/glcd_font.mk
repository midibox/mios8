# $Id: glcd_font.mk 328 2008-05-05 19:35:12Z tk $
# defines the rule for creating the glcd_font_*.o objects,
# allows also to assemble the glcd_font_*.inc files directly

# add this directory to include path
GPASM_INCLUDE += -I $(MIOS_PATH)/modules/glcd_font
SDCC_INCLUDE += -I $(MIOS_PATH)/modules/glcd_font

# make rule for relocatable version
$(OUTDIR)/glcd_font_big.o: $(MIOS_PATH)/modules/glcd_font/glcd_font_big.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(GLCD_FONT_DEFINES) $< -o $@
$(OUTDIR)/glcd_font_small.o: $(MIOS_PATH)/modules/glcd_font/glcd_font_small.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(GLCD_FONT_DEFINES) $< -o $@
$(OUTDIR)/glcd_font_normal.o: $(MIOS_PATH)/modules/glcd_font/glcd_font_normal.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(GLCD_FONT_DEFINES) $< -o $@
$(OUTDIR)/glcd_font_knob_icons.o: $(MIOS_PATH)/modules/glcd_font/glcd_font_knob_icons.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(GLCD_FONT_DEFINES) $< -o $@
$(OUTDIR)/glcd_font_meter_icons_h.o: $(MIOS_PATH)/modules/glcd_font/glcd_font_meter_icons_h.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(GLCD_FONT_DEFINES) $< -o $@
$(OUTDIR)/glcd_font_meter_icons_v.o: $(MIOS_PATH)/modules/glcd_font/glcd_font_meter_icons_v.asm
	$(GPASM) $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) $(GLCD_FONT_DEFINES) $< -o $@

# directories and files that should be part of the distribution (release) package
DIST += $(MIOS_PATH)/modules/glcd_font
