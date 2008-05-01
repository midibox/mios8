# $Id$

# output directory
OUTDIR = _output

# SDCC execution via wrapper
SDCC = sh $(MIOS_BIN_PATH)/mios-sdcc -c

# gplib execution
GPLIB = gplib -c

# flags
SDCC_FLAGS = -mpic16 --fommit-frame-pointer --optimize-goto --optimize-cmp --disable-warning 85 --obanksel=2

# default rule
all: mk_outdir
	$(SDCC) $(SDCC_FLAGS) -p18f452 src/pic18f452.c -o $(OUTDIR)/pic18f452.o
	$(GPLIB) pic18f452.lib $(OUTDIR)/pic18f452.o
	$(SDCC) $(SDCC_FLAGS) -p18f4620 src/pic18f4620.c -o $(OUTDIR)/pic18f4620.o
	$(GPLIB) pic18f4620.lib $(OUTDIR)/pic18f4620.o
	$(SDCC) $(SDCC_FLAGS) -p18f4685 src/pic18f4685.c -o $(OUTDIR)/pic18f4685.o
	$(GPLIB) pic18f4685.lib $(OUTDIR)/pic18f4685.o

# create directory that contains object files
mk_outdir:
	mkdir -p $(OUTDIR)

# clean temporary files
clean:
	rm -rf $(OUTDIR)/*
	rm -rf $(OUTDIR)

# clean temporary files + libraries
cleanall: clean
	rm -rf *.lib
